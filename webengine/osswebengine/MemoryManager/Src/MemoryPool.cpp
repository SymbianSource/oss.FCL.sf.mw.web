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
* Description:  
*
*
*/


// INCLUDES

#include <e32base.h>
#include <e32std.h>
#include <e32hal.h>
#include "MemoryPool.h"
#include "StopScheduler.h"
#include "fast_malloc.h"
#include <OOMMonitorSession.h>

// CONSTANTS

// CLASS DECLARATIONS

//-----------------------------------------------------------------------------
// CMemoryPool::AddCollector
//-----------------------------------------------------------------------------
void CMemoryPool::AddCollector( MMemoryCollector* aCollector )
    {
    // sort the collectors according to their priorities
    TInt i;
    for( i=0; i<iCollectors.Count(); ++i )
      {
      if( aCollector->Priority() <= iCollectors[i]->Priority() ) break;
      }

    iCollectors.Insert( aCollector, i );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::RemoveCollector
//-----------------------------------------------------------------------------
void CMemoryPool::RemoveCollector( MMemoryCollector* aCollector )
    {
    TInt idx = iCollectors.Find( aCollector );
    if( idx != KErrNotFound )
        iCollectors.Remove( idx );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::AddStopper
//-----------------------------------------------------------------------------
void CMemoryPool::AddStopper( MOOMStopper* aStopper )
    {
    if( !iStopScheduler )
        iStopScheduler = new CStopScheduler(*this);
    iStoppers.Append( aStopper );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::RemoveStopper
//-----------------------------------------------------------------------------
void CMemoryPool::RemoveStopper( MOOMStopper* aStopper )
    {
	if( !iStopScheduler ) return;
    TInt idx = iStoppers.Find( aStopper );
    if( idx != KErrNotFound )
        iStoppers.Remove( idx );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::SetNotifier
//-----------------------------------------------------------------------------
void CMemoryPool::SetNotifier( MOOMNotifier* aNotifier )
    {
    iNotifier = aNotifier;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::Create()
//-----------------------------------------------------------------------------
TBool CMemoryPool::Create()
    {
    iNestedChecks = 0;

    iCollectors.Reset();
    iIsStopping = EFalse;
    iIsCollecting = EFalse;
    iMemStatus = ENoOOM;

    // stop scheduler
    iStopScheduler = 0;
    iNotifier = 0;

    return ETrue;
    }


//-----------------------------------------------------------------------------
// CMemoryPool::SetStopping
//-----------------------------------------------------------------------------
void CMemoryPool::SetStopping( TBool aStopping )
    {
    iIsStopping = aStopping;
    // all operations are stopped, must have some memory available
    if( !iIsStopping ) iMemStatus = ENoOOM;

    // notify the client when stopping is done
    if( !aStopping && iNotifier )
        {
        iNotifier->Notify();
        }
    }

//-----------------------------------------------------------------------------
// CMemoryPool::CollectMemory
//-----------------------------------------------------------------------------
void CMemoryPool::CollectMemory(TUint aSize)
    {
    if( iIsCollecting ) return;

    iIsCollecting = ETrue;
    for( TInt i=0; i<iCollectors.Count(); ++i )
      {
        iCollectors[i]->Collect(aSize);
      }

    User::CompressAllHeaps();
    iIsCollecting = EFalse;
	
	if (iStopScheduler)
		iStopScheduler->Start( CStopScheduler::ECheckMemory, aSize );
    }

//-----------------------------------------------------------------------------
// CMemoryPool::RestoreCollectors
//-----------------------------------------------------------------------------
void CMemoryPool::RestoreCollectors( TOOMPriority aPriority )
  {
  if( iIsCollecting ) return;

    for( TInt i=0; i<iCollectors.Count(); ++i )
      {
        if( iCollectors[i]->Priority() >= aPriority )
            iCollectors[i]->Restore();
      }
  }

//-----------------------------------------------------------------------------
// CMemoryPool::~CMemoryPool
//-----------------------------------------------------------------------------
CMemoryPool::~CMemoryPool()
    {
    iCollectors.Reset();
    iStoppers.Reset();
    delete iStopScheduler;

    // NOTE: remove this when UI spec has OOM notifier defined
    delete iNotifier;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::AllocFromPool
//-----------------------------------------------------------------------------
TAny* CMemoryPool::AllocFromPool( TUint aSize )
    {
    // reset the status for next allocation
    iMemStatus &= ~ERescueOOM;

    TAny *p = DoAlloc( aSize );

    // check memory manager status
    if( !p || iMemStatus & ERescueOOM )
        {
        if( !iIsCollecting )
            {
            CollectMemory();
            }

        if( !p )
            p = DoAlloc( aSize );

        NotifyAndStop();
        }

    return p;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::SetStatus
//-----------------------------------------------------------------------------
void CMemoryPool::SetStatus( TOOMCheckResult aType )
    {
    iMemStatus |= aType;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::NotifyAndStop()
//-----------------------------------------------------------------------------
void CMemoryPool::NotifyAndStop()
  {
  if( !iIsStopping )
    {
        iIsStopping = ETrue;
		if (iStopScheduler)
			iStopScheduler->Start( CStopScheduler::EStopLoading, 0 );
    }
  }

//-----------------------------------------------------------------------------
// CPlainAllocator::DoAlloc
//-----------------------------------------------------------------------------
TUint CFastMemoryPool::FreeMemory(TFreeMem& aFree )
    {
    return free_memory( aFree.iPool, aFree.iHeap, aFree.iHal );
    }

//-----------------------------------------------------------------------------
// CFastMemoryPool::DoAlloc
//-----------------------------------------------------------------------------
TAny* CFastMemoryPool::DoAlloc( TUint aSize )
    {
    return fast_malloc( aSize );
    }

//-----------------------------------------------------------------------------
// CFastMemoryPool::ReAllocate
//-----------------------------------------------------------------------------
TAny* CFastMemoryPool::ReAllocate( TAny* aPtr, TUint aSize )
    {
    // reset the status for next allocation
    iMemStatus &= ~ERescueOOM;

    TAny* p = fast_realloc( aPtr, aSize );

    // check memory manager status
    if( !p || iMemStatus & ERescueOOM )
        {
        if( !iIsCollecting )
            {
            CollectMemory();
            }

        if( !p )
            p = fast_realloc( aPtr, aSize );

        NotifyAndStop();
        }

    return p;
    }

//-----------------------------------------------------------------------------
// CFastMemoryPool::PreCheck
//-----------------------------------------------------------------------------
TBool CFastMemoryPool::PreCheck( TUint aTotalSize, TUint aMaxBufSize, const TDesC8& /*aCheckerName*/ )
    {
    // avoid small checkings
    if( aTotalSize < 1024 ) return ETrue;

    if( !fast_pre_check( aTotalSize, aMaxBufSize ) )
        {
        CollectMemory(aTotalSize);
        ROomMonitorSession oomMs;
        if ( oomMs.Connect() == KErrNone ) {
            oomMs.RequestFreeMemory( aTotalSize );
            oomMs.Close();
        }
        if( !fast_pre_check( aTotalSize, aMaxBufSize ) )
            {
            iMemStatus |= ECheckOOM;
            NotifyAndStop();
            return EFalse;
            }
        }

    return ETrue;
    }

//-----------------------------------------------------------------------------
// CMemoryPool::PostCheck
//-----------------------------------------------------------------------------
TUint CFastMemoryPool::PostCheck()
    {
    fast_post_check();
    return iMemStatus;
    }

//-----------------------------------------------------------------------------
// CPlainAllocator::Free
//-----------------------------------------------------------------------------
void CFastMemoryPool::Free( TAny* aPtr )
    {
    return fast_free( aPtr );
    }

//-----------------------------------------------------------------------------
// CPlainAllocator::MemorySize
//-----------------------------------------------------------------------------
TUint CFastMemoryPool::MemorySize( TAny* aPtr )
    {
    return fast_malloc_size( aPtr );
    }

//-----------------------------------------------------------------------------
// CPlainAllocator::SetRescueBufferSize
//-----------------------------------------------------------------------------
void CFastMemoryPool::SetRescueBufferSize( TInt aSize )
    {
    fast_set_rescue_buffer_size( aSize );
    }

void CFastMemoryPool::RestoreRescueBuffer()
    {
    alloc_rescue_buffer();
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::~CDefaultMemoryPool
//-----------------------------------------------------------------------------
CDefaultMemoryPool::~CDefaultMemoryPool()
    {
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::DoAlloc
//-----------------------------------------------------------------------------
TAny* CDefaultMemoryPool::DoAlloc(TUint aSize)
    {
    return User::Alloc( aSize );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::ReAllocate
//-----------------------------------------------------------------------------
TAny* CDefaultMemoryPool::ReAllocate( TAny* aPtr, TUint aSize )
    {   
    return User::ReAlloc( aPtr, aSize );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::FreeMemory
//-----------------------------------------------------------------------------
TUint CDefaultMemoryPool::FreeMemory(TFreeMem& /*aFree*/)
    {
    // free memory in Hal
    TMemoryInfoV1Buf info;
    UserHal::MemoryInfo( info );
    return info().iFreeRamInBytes;
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::Free
//-----------------------------------------------------------------------------
void CDefaultMemoryPool::Free(TAny* aPtr)
    {
    User::Free( aPtr );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::MemorySize
//-----------------------------------------------------------------------------
TUint CDefaultMemoryPool::MemorySize(TAny* aPtr)
    {
    return User::AllocLen( aPtr );
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::SetRescueBufferSize
//-----------------------------------------------------------------------------
void CDefaultMemoryPool::SetRescueBufferSize( TInt aSize )
    {
    iRescueBufferSize = aSize;
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::PreCheck
//-----------------------------------------------------------------------------
TBool CDefaultMemoryPool::PreCheck( TUint aTotalSize, TUint /*aMaxBufSize*/, const TDesC8& /*aCheckerName*/ )
    {
    // avoid small checkings
    if( aTotalSize < 1024 ) return ETrue;

    // free memory in Hal
    TMemoryInfoV1Buf info;
    UserHal::MemoryInfo( info );
    TInt sizeNeeded = aTotalSize + iRescueBufferSize;
    if( sizeNeeded > info().iFreeRamInBytes )
        {
        CollectMemory(sizeNeeded);
        
        // check memory again
        UserHal::MemoryInfo( info );

        if(sizeNeeded > info().iFreeRamInBytes ) 
            {
            NotifyAndStop();
            iMemStatus |= ECheckOOM;
            return EFalse;
            }
        }

    return ETrue;
    }

//-----------------------------------------------------------------------------
// CDefaultMemoryPool::PostCheck
//-----------------------------------------------------------------------------
TUint CDefaultMemoryPool::PostCheck()
    {
    return iMemStatus;
    }

void CDefaultMemoryPool::RestoreRescueBuffer()
    {
    // do nothing here.
    }

// END OF FILE
