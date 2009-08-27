/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of CHttpCacheFileWriteHandler
*
*/

// INCLUDE FILES
#include "HttpCacheFileWriteHandler.h"
#include "HttpCacheEntry.h"
#include "HttpCacheHandler.h"
#include "HttpCacheStreamHandler.h"
#include "HttpCachePostponeWriteUtilities.h"
#include "HttpCacheUtil.h"
#include <HttpCacheManagerInternalCRKeys.h>
#include <centralrepository.h>
#include <hal.h>
#include <f32file.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMaxCollectCount = 5;    // collect a max of 5 items.

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

void CHttpCacheFileWriteHandler::OutputQueueContentToDebug()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:  %d objects on queue. Contents:"), iObjectQueue.Count());
    TBuf<80> txt;
    TInt totalSize=0;
    for(TInt tmploop = 0; tmploop <iObjectQueue.Count(); tmploop++)
        {
        CHttpCacheEntry* entry = iObjectQueue[tmploop];
        txt.Format(_L("CACHEPOSTPONE:    %d:  %d bytes - Cache:%08x -"), tmploop, entry->BodySize(), entry );
        totalSize+=entry->BodySize();
        HttpCacheUtil::WriteUrlToLog( 0, txt, entry->Url() );
        }
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:    %d bytes cached"), totalSize);
#endif
    }

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::CHttpCacheFileWriteHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheFileWriteHandler::CHttpCacheFileWriteHandler(CHttpCacheHandler* aHandler, CHttpCacheStreamHandler* aStreamHandler, RFs& aRfs)
    : CActive(EPriorityHigh),
      iCacheHandler( aHandler ),
      iCacheStreamHandler(aStreamHandler),
      iFs(aRfs)
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::ConstructL(const TInt aWriteTimeout)
    {
    iObjectQueue.Reset();
    iObjectQueue.ReserveL(32);

    iWaitTimer = CHttpCacheWriteTimeout::NewL( aWriteTimeout );
    CActiveScheduler::Add(this);

    MemoryManager::AddCollector(this);
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheFileWriteHandler* CHttpCacheFileWriteHandler::NewL(CHttpCacheHandler* aHandler, CHttpCacheStreamHandler* aStreamHandler, RFs& aRfs, const TInt aWriteTimeout)
    {
    CHttpCacheFileWriteHandler* self = new( ELeave ) CHttpCacheFileWriteHandler(aHandler, aStreamHandler, aRfs);

    CleanupStack::PushL( self );
    self->ConstructL(aWriteTimeout);
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHttpCacheFileWriteHandler::~CHttpCacheFileWriteHandler()
    {
    Cancel();

    DumpAllObjects();

    if ( iWaitTimer )
        {
        iWaitTimer->Cancel();
        delete iWaitTimer;
        }

    MemoryManager::RemoveCollector( this );
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::DumpAllObjectsL
// Emergency method - write everything to disk synchronously.
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::DumpAllObjects()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: >>FileWriteHandler::DumpAllObjects"));
    OutputQueueContentToDebug();
#endif
    for ( TInt i=0; i < iObjectQueue.Count(); i++ )
        {
        iCacheStreamHandler->Flush(*iObjectQueue[i]);
        }
        iObjectQueue.Reset();
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::DumpAllObjects"));
#endif
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::CompareHttpCacheEntrySize
// -----------------------------------------------------------------------------
//
TInt CHttpCacheFileWriteHandler::CompareHttpCacheEntrySize( const CHttpCacheEntry& aFirst, const CHttpCacheEntry& aSecond )
    {
    TInt first = aFirst.BodySize();
    TInt second = aSecond.BodySize();

    if ( first > second )
        {
        return -1;
        }

    if ( second > first )
        {
        return 1;
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::CollectMemory
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::CollectMemory(TUint aRequired)
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: >>FileWriteHandler::CollectMemory looking for %d bytes"), aRequired);
    OutputQueueContentToDebug();
#endif
    if ( !iObjectQueue.Count() )
        {
        return;
        }

    TInt count = KMaxCollectCount;
    while ( aRequired && count && iObjectQueue.Count() )
        {
        count--;
        CHttpCacheEntry* entry = iObjectQueue[0];
        iObjectQueue.Remove(0);
        TInt size = entry->BodySize();
        iCacheStreamHandler->Flush(*entry);
        aRequired -= size;
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::AddEntry
// -----------------------------------------------------------------------------
//
TInt CHttpCacheFileWriteHandler::AddEntry(TAddStatus &aAddStatus, CHttpCacheEntry* aEntry)
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: >>FileWriteHandler::AddEntry"));
#endif

    if ( iImmediateWriteThreshold && ( aEntry->BodySize() <= iImmediateWriteThreshold ) )
        {
        aAddStatus = EBodySmallerThanThreshold;
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:   File smaller than minimum"));
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::AddEntry"));
#endif
        return KErrNone;
        }

    // only ask about available system memory, deliberately conservative.
    TInt freeMem;
    HAL::Get(HALData::EMemoryRAMFree, freeMem);

#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:  %d free memory (according to HAL)"), freeMem);
#endif

    if ( freeMem < iFreeRamThreshold )
        {
        aAddStatus = ENotEnoughFreeMemory;
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:   Not enough spare RAM to postpone"));
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::AddEntry"));
#endif
        iLowMemoryState = ETrue;

        if ( iWaitTimer->IsActive() )
            {
            // we have some items queued for write, begin to flush them since we're going to run out of memory soon anyway.
            iWaitTimer->Cancel();
            BeginWriting();
            }

        return KErrNone;
        }

    // if we get here, we're not in low memory state any more.
    iLowMemoryState = EFalse;

    // add entry to queue
    TInt err = iObjectQueue.InsertInOrderAllowRepeats(aEntry, TLinearOrder<CHttpCacheEntry>(CompareHttpCacheEntrySize));

    #ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheFileWriteHandler: Added object %08x to postpone queue."), aEntry);
    OutputQueueContentToDebug();
#endif

    // reset timer
    if ( err == KErrNone )
        {
        aAddStatus = EAddedOk;
        iWaitTimer->Start( CHttpCacheFileWriteHandler::WriteTimeout, this );
        }
    else
        {
        aAddStatus = ECheckReturn;
        }

#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::AddEntry"));
#endif

    return err;
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::RemoveEntry
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheFileWriteHandler::RemoveEntry(CHttpCacheEntry *aEntry)
    {
    CHttpCacheEntry *entry = aEntry;

#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: >>FileWriteHandler::RemoveEntry called for entry %08x"), aEntry);
#endif

    // take object off list.
    if ( aEntry == iObjectFlushing && IsActive() )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheFileWriteHandler::RemoveEntry - entry %08x is currently being written. Returning 'not found'."), aEntry);
#endif
        // the object will be removed from the list when it's done writing out, so we don't need to worry about it
        entry = 0;
        // back off from flushing anything else for a bit in case we want that as well..
        iWaitTimer->Start(CHttpCacheFileWriteHandler::WriteTimeout, this);
        }
    else
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE: CHttpCacheFileWriteHandler::RemoveEntry - entry %08x not active."), aEntry);
#endif
        TInt index = iObjectQueue.Find( aEntry );
        if ( index >= 0 )
            {
            iObjectQueue.Remove( index );
            if ( !iObjectQueue.Count() )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: CHttpCacheFileWriteHandler::RemoveEntry - nothing left on list, stopping timer."));
#endif
                // nothing on the list, so stop the timer.
                iWaitTimer->Cancel();
                }
            }
        }

#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::RemoveEntry"));
#endif

    return entry;
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::RemoveAll
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::RemoveAll()
    {
    // empty list - note that HttpCacheEntries aren't owned.
    iObjectQueue.Reset();
    // stop us if we're active
    Cancel();
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::WriteTimeout
// -----------------------------------------------------------------------------
//
TInt CHttpCacheFileWriteHandler::WriteTimeout(TAny* aObject)
    {
    CHttpCacheFileWriteHandler *obj = (CHttpCacheFileWriteHandler *)aObject;

#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: FileWriteHandler::WriteTimeout expired"));
#endif

    obj->BeginWriting();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::BeginWriting
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::BeginWriting()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: >>FileWriteHandler::BeginWriting"));
#endif

    if ( !IsActive() )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:   Setting FileWriteHandler %08x to active."), this);
#endif
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus *stat = &iStatus;
        User::RequestComplete(stat, KErrNone);
        }
#ifdef __CACHELOG__
    else
        {
        HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:   FileWriteHandler %08x already active!"), this);
        }

    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::BeginWriting"));
#endif
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::RunL
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::RunL()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: >>FileWriteHandler::RunL"));
    OutputQueueContentToDebug();
#endif

    TInt result = iStatus.Int();

    // first, see if we have been writing anything
    if ( iObjectFlushing )
        {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:     iObjectFlushing set, been writing something."));
#endif

        // should always be first item, find just in case...
        TInt index = iObjectQueue.Find(iObjectFlushing);

#ifdef __CACHELOG__
        if ( index < 0 )
            HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:     iObjectFlushing (%08x) not found in object queue!"), iObjectFlushing);
#endif

        if ( index >= 0 )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:    iObjectFlushing (%08x) is at position %d in list"), iObjectFlushing, index);
#endif
            // the object might not exist in the queue.. how can this happen?
            iObjectQueue.Remove(index);
            //
            if ( result != KErrNone )
                {
#ifdef __CACHELOG__
                HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:     FileWriteHandler::RunL Failure while writing object %08x"), iObjectFlushing);
#endif
                // write failed.  Clean up this entry.
                // first, remove it from the cache handler so that we won't try to reuse a dead entry
                iCacheHandler->RemoveL( iObjectFlushing->Url() );
                }
            }
        iObjectFlushing = 0;
        }

    // next, check to see if we've added anything to the cache while we've been writing out.
    TInt count = iObjectQueue.Count();
    if ( iWaitTimer->IsActive() )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:     FileWriteHandler::RunL New entry detected on postpone queue, wait for timeout again."));
#endif
        // something has been added to the queue, back off until it completes.
        // this case intentionally left blank...
        }
    else
        {
        // remove any items from the top of the queue which have no body data.
        while ( iObjectQueue.Count() && iObjectQueue[0]->BodySize() == 0 )
            {
            iObjectQueue.Remove(0);
            };

        // check to see if there is anything ready to write out
        if ( iObjectQueue.Count() )
            {
            SetActive();
            iStatus = KRequestPending;
            iCacheStreamHandler->FlushAsync( *iObjectQueue[0], iStatus );
            iObjectFlushing = iObjectQueue[0];
#ifdef __CACHELOG__
            HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:     FileWriteHandler::RunL continue cache flush, Starting object %08x."), iObjectFlushing);
#endif
            }
        else
            {   // nothing left to write, go idle.
    #ifdef __CACHELOG__
            HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:     FileWriteHandler::RunL complete with nothing else to write."));
    #endif
            iCacheHandler->SaveLookupTableL();
            iLowMemoryState = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::Collect
// -----------------------------------------------------------------------------
//
TUint CHttpCacheFileWriteHandler::Collect(TUint aRequired)
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteFormatLog(0, _L("CACHEPOSTPONE:  >>FileWriteHandler::Collect on FileWriteHandler %08x (low memory collector)"), this);
#endif
    if ( iWaitTimer->IsActive() )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:    Wait timer is active, cancel it and call DumpAllObjects"));
#endif

        iWaitTimer->Cancel();
        CollectMemory( aRequired );
        iLowMemoryState = ETrue;
        BeginWriting();
        }
#ifdef __CACHELOG__
    else
        {
        HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE:    Wait timer not active."));
        }
    HttpCacheUtil::WriteLog(0, _L("CACHEPOSTPONE: <<FileWriteHandler::Collect"));
#endif
    return 0;
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::Restore
// -----------------------------------------------------------------------------
//
void CHttpCacheFileWriteHandler::Restore()
    {
    // not supported
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::Priority
// -----------------------------------------------------------------------------
//
TOOMPriority CHttpCacheFileWriteHandler::Priority()
    {
    return EOOM_PriorityLow;
    }

// -----------------------------------------------------------------------------
// CHttpCacheFileWriteHandler::IsCacheEntryPostponed
// -----------------------------------------------------------------------------
//
TBool CHttpCacheFileWriteHandler::IsCacheEntryPostponed(const CHttpCacheEntry* aEntry)
    {
    TInt index = iObjectQueue.Find( aEntry );

    if ( index >= 0 )
        {
        return ETrue;
        }

    return EFalse;
    }

//  End of File
