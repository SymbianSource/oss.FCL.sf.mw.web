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
* Description:  Implementation of CHttpCacheHandler
*
*/

// INCLUDE FILES
#include "HttpCacheHandler.h"
#include "HttpCacheManager.h"
#include "HttpCacheEntry.h"
#include "HttpCacheLookupTable.h"
#include "HttpCacheStreamHandler.h"
#include "HttpCacheUtil.h"
#include "HttpCacheEvictionHandler.h"
#include "HttpCacheObserver.h"
#include <http/RHTTPTransaction.h>
#include <http/rhttpsession.h>
#include <http/rhttpheaders.h>
#include <HttpStringConstants.h>
#include <httperr.h>
#include <s32file.h>
#include <BrCtlDefs.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
void PanicCacheHandler(
    TInt aError)
    {
    _LIT(KCachePanic, "cacheHandler Panic");

    User::Panic( KCachePanic, aError );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheHandler::CHttpCacheHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheHandler::CHttpCacheHandler(
    TInt aSize ) : iSize( aSize )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::ConstructL(
    const TDesC& aDirectory,
    const TDesC& aIndexFile,
    TInt aCriticalLevel)
    {
	User::LeaveIfError(iRfs.Connect());
    //
    iIndexFile = aIndexFile.AllocL();
    //
    iDirectory = aDirectory.AllocL();
    //
    iEvictionHandler = CHttpCacheEvictionHandler::NewL();
    //
    iStreamHandler = CHttpCacheStreamHandler::NewL( iDirectory->Des(), aCriticalLevel );
    //
    iLookupTable = CHttpCacheLookupTable::NewL( *iEvictionHandler, *iStreamHandler );
    //1. Set up top-level cache directory if it doesn't exist. 
    TEntry entry;
    TInt err( KErrNone );
    if (iRfs.Entry(iDirectory->Des(), entry) != KErrNone)
        {
        err = iRfs.MkDirAll(iDirectory->Des());
        }

    //2. Create subdirectories to store header/body files
    if ( err == KErrNone || err == KErrAlreadyExists ) { //harmless errors
    
        __ASSERT_DEBUG ( (iDirectory->Des().LocateReverse( KPathDelimiter ) == (iDirectory->Des().Length() - 1)),
                PanicCacheHandler( KErrCorrupt ) ); // We assume that iDirectory is terminated by a forward slash
        
        HBufC* subDir = HBufC::NewL(KMaxPath); // Base cache dir + subdir name + another delimiter
        _LIT(KFormat,"%S%x%c");
        for (TUint i = 0; i < KCacheSubdirCount; i++)
            {
            TPtrC ptr (iDirectory->Des());
            subDir->Des().Format(KFormat, &ptr, i, KPathDelimiter); 
            TInt err2 = iRfs.MkDir(subDir->Des()); 
            __ASSERT_DEBUG ( (err2 == KErrNone || err2 == KErrAlreadyExists), PanicCacheHandler( err2 ) ); 
            }
        delete subDir;
        //end cache dir + subdir creation
    } else {
        User::Leave(err);
    }

    OpenLookupTableL();
    //
    iHttpCacheObserver = CHttpCacheObserver::NewL(iDirectory, iIndexFile, this);
    iHttpCacheObserver->StartObserver();
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheHandler* CHttpCacheHandler::NewL(
    TInt aSize,
    const TDesC& aDirectory,
    const TDesC& aIndexFile,
    TInt aCriticalLevel)
    {
    CHttpCacheHandler* self = new( ELeave ) CHttpCacheHandler( aSize );

    CleanupStack::PushL( self );
    self->ConstructL( aDirectory, aIndexFile, aCriticalLevel );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHttpCacheHandler::~CHttpCacheHandler()
    {
    TRAP_IGNORE( SaveLookupTableL() );
    //
    delete iHttpCacheObserver;
    //
    if ( iEvictionHandler )
        {
        iEvictionHandler->RemoveAll();
        }
    //
    delete iLookupTable;
    //
    delete iEvictionHandler;
    //
    delete iStreamHandler;
    //
    delete iDirectory;
    //
    delete iIndexFile;
    //
    iRfs.Close();
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::RequestL
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheHandler::RequestL(
    RHTTPTransaction& aTrans,
    TBrCtlDefs::TBrCtlCacheMode aCacheMode,
    THttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog( 0, _L( "request item" ), aTrans.Request().URI().UriDes() );
#endif

    TInt status( KErrNotFound );
    CHttpCacheEntry* entry = NULL;
    // 0. check if we need to check cache at all (protected vs no cache mode)
    // 1. check if the url is in the cache
    // 2. check if it is complete
    // 3. see if it is useable

    // use protected item on reload
    // currently not and do not use cache for post
    if( aCacheMode != TBrCtlDefs::ECacheModeNoCache &&
        HttpCacheUtil::MethodFromStr( aTrans.Request().Method(), aTrans.Session().StringPool() ) != EMethodPost )
        {
        // If the cacheMode is noCache then it must ignore the cached entry.
        entry = iLookupTable->Find( aTrans.Request().URI().UriDes() );
        // check if the trailing slash is missing
        if( !entry )
            {
            TUriC8 uri = aTrans.Request().URI();

            if( uri.Extract( EUriPath ).Length() == 0 )
                {
                CUri8* fixeduri = CUri8::NewLC( uri );
                fixeduri->SetComponentL( _L8("/"), EUriPath );
                //
                entry = iLookupTable->Find( fixeduri->Uri().UriDes() );
                //
                CleanupStack::PopAndDestroy(); // fixeduri
                }
            }
        //
        if( entry && entry->State() == CHttpCacheEntry::ECacheComplete )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "item is in the cache" ) );
#endif            
            //
            status = CacheNeedsValidationL( *entry, aTrans, aCacheMode ) ? KErrNotReady : KErrNone;
            // entry could be invalidated at this point. check for status to make sure
            // the entry is still valid
            }
        // prepare stream for request
        if( status == KErrNone )
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "prepare item for sending" ) );
#endif
            // attach entry to the stream
            if( iStreamHandler->AttachL( *entry ) )
                {
                entry->SetState( CHttpCacheEntry::ECacheRequesting );
                entry->Accessed();
                }
            else
                {
                // cleanup on the corrupt entry
                DeleteCacheEntry( *entry );
                entry = NULL;
                // item is not in cache
                status = KErrNotFound;
                }
            }
        // cleanup
        if( status == KErrNone && entry )
            {
            // save handler and entry so that
            // on next call we don't have to start a lookup again
            aCacheEntry.iCacheHandler = this;
            aCacheEntry.iCacheEntry = entry;
            }
        else
            {
            // cleanup response headers
            //
            // response is not in the cache, so remove all the response header fields
            // set by cache handler
            RHTTPHeaders responseHeaders = aTrans.Response().GetHeaderCollection();
            responseHeaders.RemoveAllFields();
            }
        }
#ifdef __CACHELOG__
    else
        {
        HttpCacheUtil::WriteLog( 0, _L( "reload: do not use cache" ) );
        }
    if( status != KErrNone && entry )
        {
        //
        HttpCacheUtil::WriteLog( 0, _L( "item needs validation" ) );
        }
    else
        {
        //
        HttpCacheUtil::WriteLog( 0, _L( "item is not in the cache" ) );
        }
#endif // __CACHELOG__

    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::RequestHeadersL
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheHandler::RequestHeadersL(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    HttpCacheUtil::WriteUrlToLog( 0, _L( "request http headers" ), aTrans.Request().URI().UriDes() );
    //
    TInt status( KErrNotFound );
    CHttpCacheEntry* entry = aCacheEntry.iCacheEntry;
    //
    if( entry && entry->State() == CHttpCacheEntry::ECacheRequesting )
        {
        // response headers should already have all the headers
        // as RequestL call adds them all.
        // no need to do much here
        // get header from file
        status = KErrNone;
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "sending http headers" ) );
#endif        
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::RequestNextChunkL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheHandler::RequestNextChunkL(
    RHTTPTransaction& aTrans,
    TBool& aLastChunk,
    THttpCacheEntry& aCacheEntry )
    {
    (void)aTrans; //suppress compiler and PC-lint warnings
    //
    HBufC8* bodyStr = NULL;
    CHttpCacheEntry* entry = aCacheEntry.iCacheEntry;
    //
    if( entry && entry->State() == CHttpCacheEntry::ECacheRequesting )
        {
        // get next chunk
        bodyStr = iStreamHandler->NextChunkL( *entry, aLastChunk );
        }
    return bodyStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::RequestClosed
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::RequestClosed(
    RHTTPTransaction* aTrans,
    THttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    if ( aTrans )
        HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheHandler::RequestClosed on url =" ), aTrans->Request().URI().UriDes() );
#endif

    // Fine, make sure transaction is moved to the complete list
    CHttpCacheEntry* entry = aCacheEntry.iCacheEntry;

    if ( entry )
        {
        // normal close on a request - when the content is loaded from the cache
        if ( entry->State() == CHttpCacheEntry::ECacheRequesting )
            {
            entry->SetState( CHttpCacheEntry::ECacheComplete );
            iStreamHandler->Detach( *entry );
            }
        // transaction is closed without being completed
        else if ( entry->State() == CHttpCacheEntry::ECacheResponding ||
                  entry->State() == CHttpCacheEntry::ECacheDestroyed )
            {
            // remove uncompleted/destroyed entry
            iStreamHandler->Detach( *entry );
            DeleteCacheEntry( *entry );
            entry = NULL;
            aCacheEntry.iCacheEntry = NULL;
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheHandler::RequestClosed - uncompleted entry" ) );
#endif
            }
        else if ( entry->State() == CHttpCacheEntry::ECacheComplete )
            {
            // normal close on the request - when the content is saved to the cache
            // ResponseComplete has already been called
            // check if the stream is released
            // __ASSERT_DEBUG( !entry->CacheFilesOpened() ) , PanicCacheHandler( KErrCorrupt ) );
            }
        else
            {
            __ASSERT_DEBUG( EFalse , PanicCacheHandler( KErrCorrupt ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::AdjustResponseTime
//
// -----------------------------------------------------------------------------
//

void CHttpCacheHandler::AdjustResponseTime(RHTTPTransaction& aTrans)
    {
    RStringF fieldName;
    THTTPHdrVal dateValue;
    TDateTime date;
    TTime now;
    TInt err;
    TTime serverResponseTime;
    RHTTPHeaders respHeaders = aTrans.Response().GetHeaderCollection();
    RStringPool strP = aTrans.Session().StringPool();

    // Get the current time. All internet dates are GMT

    now.UniversalTime();

    // Get the date from the headers. Compare this with the response
    // GMT time. If the server time is wrong then adjust the time here.

    fieldName = strP.StringF( HTTP::EDate, RHTTPSession::GetTable() );
    err = respHeaders.GetField( fieldName, 0, dateValue );
    if( err == KErrNotFound || dateValue.Type() != THTTPHdrVal::KDateVal )
        {
        serverResponseTime = 0;
        }
    else
        {
        serverResponseTime = TTime( dateValue.DateTime() );
        }


    // Implies some thing wrong with with origin server time.

    if ( now < serverResponseTime )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "Response time is less than GMT time" ) );
#endif
        // Remove the field first. It is must otherwise raw field
        // data is not modified. The cache data is written from the
        // raw field data.
        if( respHeaders.RemoveField(fieldName) == KErrNone )
            {
            dateValue.SetDateTime( now.DateTime() );
            TRAP_IGNORE( respHeaders.SetFieldL( fieldName, dateValue ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CHttpCacheHandler::ReceivedResponseHeadersL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::ReceivedResponseHeadersL(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L("---> CHttpCacheHandler::ReceivedResponseHeadersL"), 0);
    HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheHandler::ReceivedResponseHeadersL - received http headers" ), aTrans.Request().URI().UriDes() );
#endif

    TBool protectedEntry( EFalse );
    // check if the item is cacheable
    // no item should be bigger than the 1/3 of the cache size
    if( HttpCacheUtil::IsCacheable( aTrans, ( iSize / 3 ), protectedEntry ) )
        {
        // check if the entry is already in the cache
        CHttpCacheEntry* entry = iLookupTable->Find( aTrans.Request().URI().UriDes() );
        if( entry )
            {
#ifdef __CACHELOG__
        	HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                       _L("CHttpCacheHandler::ReceivedResponseHeadersL"),
                                       entry->Filename(),
                                       entry->Url(),
                                       entry->BodySize(), 
                                       ELogEntrySize );
#endif

            if( entry->State() != CHttpCacheEntry::ECacheComplete )
                {
                // multiple incoming entries doh.
#ifdef __CACHELOG__
        		HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                       _L("CHttpCacheHandler::ReceivedResponseHeadersL - ERROR MULTIPLE requests"),
                                       entry->Filename(),
                                       entry->Url(),
                                       entry->BodySize(), 
                                       ELogEntrySize );
#endif
                // ignore this one and the first will proceed.
                entry = NULL;
                }
            }
        else
            {
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheHandler::ReceivedResponseHeadersL - create new cache item" ) );
#endif
            //Check adjustment of response time is required or not.
            AdjustResponseTime( aTrans );
            // hash it
            entry = iLookupTable->InsertL( aTrans.Request().URI().UriDes() );
            if( entry )
                {
                // protect this entry
                if( protectedEntry )
                    {
#ifdef __CACHELOG__
        			HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                       _L("CHttpCacheHandler::ReceivedResponseHeadersL - this is protected item"),
                                       entry->Filename(),
                                       entry->Url(),
                                       entry->BodySize(), 
                                       ELogEntrySize );
#endif
                    entry->SetProtected();
                    RHTTPHeaders responseHeaders = aTrans.Response().GetHeaderCollection();
                    RStringPool strP = aTrans.Session().StringPool();

                    // double expiration time
                    HttpCacheUtil::AdjustExpirationTimeL( responseHeaders, strP );
                    }
                }
            else
                {
                // no luck with the lookuptable
                __ASSERT_DEBUG( EFalse, PanicCacheHandler( KErrCorrupt ) );
                }
            }
        // save headers
        if( entry )
            {
            // attach it to the stream handler
            if( iStreamHandler->AttachL( *entry ) )
                {
                entry->SetState( CHttpCacheEntry::ECacheResponding );
                // 1. handle only 304 and 200
                // 2. check if either the header or the body ( or both ) need to be updated
                // 3. update the headers anyway in case of notmodified (304)
                // 4. remove the old body in case of bodyupdate
                TInt httpStatus( aTrans.Response().StatusCode() );

#ifdef __CACHELOG__
                HttpCacheUtil::WriteLog( 0,  _L("CHttpCacheHandler::ReceivedResponseHeadersL - status code ="), httpStatus );
#endif
                TBool ok( EFalse );
                if( httpStatus == HTTPStatus::EOk )
                    {
                    ok = HandleResponseOkL( *entry, aTrans );
                    }
                else if( httpStatus == HTTPStatus::ENotModified )
                    {
                    ok = HandleResponseNotModifiedL( *entry, aTrans );
                    }

                // entry could be corrupted at this point
                if( ok )
                    {
                    // save handler and entry so that
                    // on next call we don't have to start a lookup again
                    aCacheEntry.iCacheHandler = this;
                    aCacheEntry.iCacheEntry = entry;
                    }
                else
                    {
                    iStreamHandler->Detach( *entry );
                    DeleteCacheEntry( *entry );
                    entry = NULL;
                    }
                }
            else
                {
                DeleteCacheEntry( *entry );
                entry = NULL;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::ReceivedResponseBodyDataL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::ReceivedResponseBodyDataL(
    RHTTPTransaction& aTrans,
    MHTTPDataSupplier& aBodyDataSupplier,
    THttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheHandler::ReceivedResponseBodyDataL - received body from url =" ), aTrans.Request().URI().UriDes() );
#endif

    // 1. check if we are caching this resource
    // 2. update the body data
    CHttpCacheEntry* entry = aCacheEntry.iCacheEntry;

    if ( entry && entry->State() == CHttpCacheEntry::ECacheResponding )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L("---> CHttpCacheHandler::ReceivedResponseBodyDataL"), entry->BodySize() );
#endif
        HBufC8* bodyStr = HttpCacheUtil::BodyToBufferL( aBodyDataSupplier );
        if ( bodyStr )
            {
            // Do we have old body data to remove first
            if ( entry->BodyFileDeleteNeeded() ) {
                iStreamHandler->RemoveBodyData( *entry );
                entry->SetBodyFileDeleteNeeded( EFalse );
            	}
            
            // erase entry if we are unable to save it (low disk space)
            if( !SaveBuffer( *entry, bodyStr->Des(), ETrue ) )
                {
                // detach it from the stream and erase it
                iStreamHandler->Detach( *entry );
                DeleteCacheEntry( *entry );
#ifdef __CACHELOG__                
                HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheHandler::ReceivedResponseBodyDataL - body cannot be saved" ) );
#endif                
                entry = NULL;
                // remove entry
                aCacheEntry.iCacheEntry = NULL;
                }
#ifdef __CACHELOG__
            else
                {
                HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheHandler::ReceivedResponseBodyDataL - body is saved" ) );
                }
#endif // __CACHELOG__
            //
            delete bodyStr;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::ResponseComplete
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::ResponseComplete(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    HttpCacheUtil::WriteUrlToLog( 0, _L( "response complete" ), aTrans.Request().URI().UriDes() );
    // 1. check if we are caching this resource
    // 2. mark the entry as complete
    CHttpCacheEntry* entry = aCacheEntry.iCacheEntry;

    if ( entry )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L("---> CHttpCacheHandler::ResponseComplete"), entry->BodySize() );
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheHandler::ResponseComplete"),
                                           entry->Filename(),
                                           entry->Url(),
                                           entry->BodySize(),
                                           ELogEntrySize );
#endif

        if ( entry->State() == CHttpCacheEntry::ECacheResponding )
            {
            // Flush the entry
            if ( !iStreamHandler->Flush( *entry ) )
                {
                // We failed saving (flush), cleanup
                iStreamHandler->Detach( *entry );

                // Deleting the entry frees cache buffer
                DeleteCacheEntry( *entry );
                entry = NULL;
                aCacheEntry.iCacheEntry = NULL;
                }
            else
                {
                // We successfully saved (flush) body
                entry->SetState( CHttpCacheEntry::ECacheComplete );
                iStreamHandler->Detach( *entry );

                // Clear the flushed cache buffer, we were using for incoming body
                entry->SetCacheBufferL( KBufferSizeZero );
                }
            }
        else if( entry->State() == CHttpCacheEntry::ECacheDestroyed )
            {
            iStreamHandler->Detach( *entry );

            // Deleting the entry frees cache buffer
            DeleteCacheEntry( *entry, EFalse );
            entry = NULL;
            aCacheEntry.iCacheEntry = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// Removes all entries in the Cache lookup table, commits table to disk. 
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheHandler::RemoveAllL()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "remove all items" ) );
#endif
    TInt numberOfBytes;
    // clear all the inactive entries
    numberOfBytes = iLookupTable->RemoveAll();
    // and save it. user initiated. no need to do idle save
    SaveLookupTableL();
    return numberOfBytes;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::ListFiles
// Adds all filenames known to this Cache to aFilenameList
// -----------------------------------------------------------------------------
//
TInt CHttpCacheHandler::ListFiles(RPointerArray<TDesC>& aFilenameList)
    {
    return iLookupTable->ListFiles(aFilenameList);
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::RemoveL
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheHandler::RemoveL( const TDesC8& aUrl )
    {
    TInt status( KErrNotFound );
    HttpCacheUtil::WriteUrlToLog( 0, _L( "remove item:" ), aUrl );
    CHttpCacheEntry* entry = iLookupTable->Find( aUrl );

    if ( entry )
        {
        if ( entry->State() == CHttpCacheEntry::ECacheComplete )
            {
            // Delete from lookup table
            status = iLookupTable->Remove( aUrl );
            }
        else
            {
            // Mark it as deleted and erase it when the
            // trans is complete
            entry->SetState( CHttpCacheEntry::ECacheDestroyed );
            status = KErrNone;
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::Find
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::Find( const TDesC8& aUrl )
    {
    // find
    CHttpCacheEntry* entry = iLookupTable->Find( aUrl );
    return ( entry ? ( entry->State() == CHttpCacheEntry::ECacheComplete ) : EFalse );
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::SaveL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::SaveL(
    const TDesC8& aUrl,
    const TDesC8& aHeader,
    const TDesC8& aContent )
    {
    TBool saved( EFalse );
    // check if entry exist. do not overwrite.
    CHttpCacheEntry* entry = iLookupTable->Find( aUrl );
    if( !entry )
        {
        entry = iLookupTable->InsertL( aUrl );
        // prepare for saving
        if( entry && iStreamHandler->AttachL( *entry ) )
            {
            // save header and body
            saved = SaveBuffer( *entry, aHeader, EFalse ) && SaveBuffer( *entry, aContent, ETrue );
            if( saved )
                {
                // flush
                saved = iStreamHandler->Flush( *entry );
                if( saved )
                    {
                    entry->SetState( CHttpCacheEntry::ECacheComplete );
                    }
                }
            iStreamHandler->Detach( *entry );
            }
        // cleanup
        if( !saved && entry )
            {
            DeleteCacheEntry( *entry );
            }
        }
    return saved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::AddHeaderL
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheHandler::AddHeaderL(
    const TDesC8& aUrl,
    const TDesC8& aName,
    const TDesC8& aValue )
    {
    TInt status( KErrNotFound );
    //
    CHttpCacheEntry* entry = iLookupTable->Find( aUrl );
    if( entry )
        {
        TBool attached;
        //
        attached = iStreamHandler->AttachL( *entry );
        // get headers
        HBufC8* headersStr = iStreamHandler->HeadersL( *entry );
        if( headersStr )
            {
            CleanupStack::PushL( headersStr );
            // alter headers and save them
            HBufC8* newHeaderStr = HttpCacheUtil::AddHeaderLC( aName, aValue, headersStr->Des() );
            if( newHeaderStr )
                {
                // remove old headers first
                iStreamHandler->RemoveHeaders( *entry );
                // save new headers
                if( !SaveBuffer( *entry, newHeaderStr->Des(), EFalse ) )
                    {
                    status = KErrDirFull;
                    // failed. should we save the original headers?
                    TBool saveOk( SaveBuffer( *entry, headersStr->Des(), EFalse ) );
                    // original save should never fail
                    __ASSERT_DEBUG( saveOk, PanicCacheHandler( KErrCorrupt ) );

                    if( !saveOk )
                        {
                        // sorry, we made this entry corrupt. remove it
                        iStreamHandler->Detach( *entry );
                        DeleteCacheEntry( *entry );
                        entry = NULL;
                        }
                    }
                else
                    {
                    status = KErrNone;
                    }
                CleanupStack::PopAndDestroy(); // newHeaderStr
                }
            CleanupStack::PopAndDestroy(); // headersStr
            }
        // detach
        if( attached )
            {
            iStreamHandler->Detach( *entry );
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::CacheNeedsValidationL
//
// Note: This function check if cache needs validation
//       Delete cache entry from this function is too early, should avoid it
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::CacheNeedsValidationL(
    CHttpCacheEntry& aCacheEntry,
    RHTTPTransaction& aTrans,
    TBrCtlDefs::TBrCtlCacheMode aCacheMode )
    {
    // The entry isn't useable unless otherwise stated
    TBool mustRevalidate( ETrue );

    // If the cacheMode is PreferCache then it must use it even if it
    // is expired, etc.  This is typically used during history navigation.
    //
    // Note: Cache also set the cacheMode to PreferCache after a resource has
    //       been validated -- that is when it tries to refetch an entry from
    //       cache after receiving a 304 response.
    // get cached headers
    if( iStreamHandler->AttachL( aCacheEntry ) )
        {
        HBufC8* headersStr = iStreamHandler->HeadersL( aCacheEntry );
        CleanupStack::PushL( headersStr );
        iStreamHandler->Detach( aCacheEntry );
        // headersStr == NULL happens if you erase the cache directory
        // using a file manager
        if( headersStr )
            {
            // use response headers for retreiving cached headers
            RHTTPHeaders responseHeaders = aTrans.Response().GetHeaderCollection();
            RHTTPHeaders requestHeaders = aTrans.Request().GetHeaderCollection();
            RStringPool strP = aTrans.Session().StringPool();
            // convert the buffer to httpHeader
            HttpCacheUtil::BufferToHeadersL( headersStr->Des(), responseHeaders, strP );

            // check if we need to validate the cahce
            if( aCacheMode == TBrCtlDefs::ECacheModeOnlyCache || aCacheMode == TBrCtlDefs::ECacheModeHistory  )
                {
                // no validation required
#ifdef __CACHELOG__
                HttpCacheUtil::WriteLog( 0, _L( "prefer cache mode. no need to revalidate" ), aCacheMode );
#endif                
                mustRevalidate = EFalse;
                }
            else
                {
                // Get the pragma no-cache header from the headers
                // no-cache on request header means "do not use cache"
                if( !HttpCacheUtil::PragmaNoCache( aTrans ) )
                    {
                    if( !HttpCacheUtil::CacheTimeIsFresh( requestHeaders, responseHeaders, strP  ) )
                        {
#ifdef __CACHELOG__
                        HttpCacheUtil::WriteLog( 0, _L( "cache item is not fresh. needs revalidation" ) );
#endif
                        // MKLE-7PRD27: Avoid removing cache entry here 
                        mustRevalidate = ETrue;
                        // add headers like EIfModifiedSince, EETag, EIfNoneMatch
                        HttpCacheUtil::AddValidationHeaders( responseHeaders, requestHeaders, strP );
                        }
                    else
                        {
#ifdef __CACHELOG__
                        HttpCacheUtil::WriteLog( 0, _L( "cache item is fresh. needs no revalidation" ) );
#endif                        
                        //
                        mustRevalidate = EFalse;
                        }
                    }
                else
                    {
                    // needs validation
#ifdef __CACHELOG__
                    HttpCacheUtil::WriteLog( 0, _L( "no cache/no store header present. need revalidation" ) );
#endif
                    mustRevalidate = ETrue;
                    }

                }
            }
        CleanupStack::PopAndDestroy(); // headersStr
        }
    else
        {
        DeleteCacheEntry( aCacheEntry );
        // needs validation
        mustRevalidate = ETrue;
        }
    return mustRevalidate;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::CacheNeedsSpaceL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::CacheNeedsSpaceL( TInt aSize )
    {
    TBool ok( ETrue );
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "the cache is this big" ), iSize );
    HttpCacheUtil::WriteLog( 0, _L( "we occupy" ), iStreamHandler->SavedContentSize()  );
    HttpCacheUtil::WriteLog( 0, _L( "this item needs space:" ), aSize );
#endif // __CACHELOG__

    // check if we need extra space
    if ( iStreamHandler->SavedContentSize() + aSize > iSize )
        {

#ifdef __CACHELOG__
        // items in the cache
        TInt size( 0 );
        HttpCacheUtil::WriteLog( 0, _L( "cached items" ) );

        const CArrayPtrFlat<CHttpCacheEntry>& entries = iLookupTable->Entries();
        for( TInt i = 0; i < entries.Count(); i++ )
            {
            CHttpCacheEntry* entry = entries.At( i );
            if( entry && entry != (CHttpCacheEntry*)0xffffffff )
                {
                HttpCacheUtil::WriteUrlToLog( 0, entry->Url(), entry->BodySize() );
                size += entry->BodySize();
                size += entry->HeaderSize();
                }
            }
        HttpCacheUtil::WriteLog( 0, _L( "occupy with headers:" ), size );
#endif // __CACHELOG__

        CArrayPtrFlat<CHttpCacheEntry>* evictedList = iEvictionHandler->EvictL( aSize );
        if ( evictedList && evictedList->Count() )
            {
            // destroy items
            CHttpCacheEntry* entry;
            for( TInt i = 0; i < evictedList->Count(); i++ )
                {
                //
                entry = evictedList->At( i );
                if( entry )
                    {
                    // destroy
                    iLookupTable->Remove( entry->Url() );
                    }
                }

            // ok = ETrue if there is enough space
            ok = ( iStreamHandler->SavedContentSize() + aSize < iSize );
            }
        else
            {
            // interesting...nobody can be evicted...
            // they are all protected entries?
            // or the incoming -not yet complete- items take the entire cache?
#ifdef __CACHELOG__
            HttpCacheUtil::WriteLog( 0, _L( "NO SPACE can be released!!!" ) );
#endif            
            ok = EFalse;
            }
        delete evictedList;
        }
    return ok;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::HandleResponseOkL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::HandleResponseOkL(
    CHttpCacheEntry& aEntry,
    RHTTPTransaction& aTrans )
    {
    TBool saveOk( ETrue );
    RHTTPHeaders responseHeader = aTrans.Response().GetHeaderCollection();
    RStringPool strP = aTrans.Session().StringPool();
    HBufC8* responseHeaderStr = HttpCacheUtil::HeadersToBufferLC( responseHeader, strP );

    TBool update( ETrue );
    // get cached headers to compare
    HBufC8* cachedHeaderStr = iStreamHandler->HeadersL( aEntry );
    
    // we've got some headers to update, check if we really need to update them
    if ( cachedHeaderStr )
        {
        CleanupStack::PushL( cachedHeaderStr );

        update = HttpCacheUtil::CacheNeedsUpdateL( responseHeader, cachedHeaderStr->Des(), strP );

        CleanupStack::PopAndDestroy(); // cachedHeaderStr
        }

    if ( update )
        {
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                       _L("CHttpCacheHandler::HandleResponseOkL - cache UPDATE needed"),
                                       aEntry.Filename(),
                                       aEntry.Url(),
                                       aEntry.BodySize(), 
                                       ELogEntrySize );
#endif
        if ( aEntry.HeaderSize() )
            {
            // remove headers from headerFile first
            iStreamHandler->RemoveHeaders( aEntry );
            }

        // save new headerFile
        saveOk = SaveBuffer( aEntry, responseHeaderStr->Des() );
        
        if ( aEntry.BodySize() )
            {
            // We will remove this body data, after we confirm that we get new
            // body data
            aEntry.SetBodyFileDeleteNeeded( ETrue );
            }

        // Setup a cache buffer to hold the incoming body
        aEntry.SetCacheBufferL( KBufferSize32k );
        }
    else
        {
        // if neither the header nor the body need to be updated, then
        // detach entry to protect from being updated
#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheHandler::HandleResponseOkL - no update needed, ignore response" ) );
#endif        
        //
        aEntry.SetState( CHttpCacheEntry::ECacheComplete );
        iStreamHandler->Detach( aEntry );
        // pretend that save was ok.
        saveOk = ETrue;
        }

    CleanupStack::PopAndDestroy(); // responseHeaderStr

    // destroy corrupt entry by returning EFalse
    return saveOk;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::HandleResponseNotModifiedL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::HandleResponseNotModifiedL(
    CHttpCacheEntry& aEntry,
    RHTTPTransaction& aTrans )
    {
    // oos? -out of space
    TBool saveOk( ETrue );
    RHTTPHeaders responseHeader = aTrans.Response().GetHeaderCollection();
    RStringPool strP = aTrans.Session().StringPool();
    HBufC8* responseHeaderStr = HttpCacheUtil::HeadersToBufferLC( responseHeader, strP );
    // If a cache uses a received 304 response to update a cache entry,
    // the cache MUST update the entry to reflect any new field values given in the response.
    HBufC8* mergedHeadersStr = NULL;
    HBufC8* cachedHeaderStr = iStreamHandler->HeadersL( aEntry );
    CleanupStack::PushL( cachedHeaderStr );
    // don't merge with empty headers
    if( cachedHeaderStr )
        {
        mergedHeadersStr = HttpCacheUtil::MergeHeadersLC( cachedHeaderStr->Des(), responseHeader, strP );
        CleanupStack::Pop(); // mergedHeadersStr
        }
    // don't update empty headers
    if( mergedHeadersStr || responseHeaderStr )
        {
        // remove cached headers first
        iStreamHandler->RemoveHeaders( aEntry );
        // save merged headers  (reponse + cached)
        if( mergedHeadersStr )
            {
            saveOk = SaveBuffer( aEntry, mergedHeadersStr->Des() );
            }
        else if( responseHeaderStr )
            {
            // save responseheader instead
            saveOk = SaveBuffer( aEntry, responseHeaderStr->Des() );
            }
        // if save failed, let's see if we can save old
        // headers
        if( !saveOk && cachedHeaderStr )
            {
            saveOk = SaveBuffer( aEntry, cachedHeaderStr->Des() );
            }
        }
    // do not remove the body as it was not modified
    delete mergedHeadersStr;
    CleanupStack::PopAndDestroy( 2 ); // cachedHeaderStr, responseHeaderStr
    // check if save was ok.
    // or nothing was not saved at all
    if( saveOk )
        {
        // this item does not need update
        aEntry.SetState( CHttpCacheEntry::ECacheComplete );
        iStreamHandler->Detach( aEntry );
        }
    return saveOk;
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::OpenLookupTableL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::OpenLookupTableL()
    {
    OpenLookupTableL(iLookupTable);
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::OpenLookupTableL
// Opens the index*.dat lookup table from file system. 
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::OpenLookupTableL(CHttpCacheLookupTable* aLookupTable)
    {
    // read entries from index.dat
    RFileReadStream readStream;

        iRfs.SetSessionPath( iDirectory->Des() );

        TInt ret = KErrNone;
        TInt tryCount = 0;
        for (tryCount = 0; tryCount < 5; tryCount++) 
            {
            ret = readStream.Open( iRfs, iIndexFile->Des(), EFileRead | EFileShareAny );
            if (ret == KErrInUse)
                {
                // When the cache is full, it takes 65 - 85 miliseconds to write the index.
                // So wait 50 miliseconds and try again
                User::After(50000);
                }
            else
                {
                break;
                }
            }
        if( ret == KErrNone )
            {
            CleanupClosePushL( readStream );
            aLookupTable->InternalizeL( readStream, iDirectory->Des() );
            CleanupStack::PopAndDestroy(1); // readStream
            }
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::SaveLookupTableL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::SaveLookupTableL()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L( "lookup table is saved" ) );
#endif
    // save entries to index.dat
    RFileWriteStream writeStream;

    // Don't get notified about own changes
    iHttpCacheObserver->Cancel();
    TInt ret = KErrNone;
    TInt tryCount = 0;
    for (tryCount = 0; tryCount < 5; tryCount++) 
        {
        ret = writeStream.Replace( iRfs, iIndexFile->Des(), EFileWrite );
        if (ret == KErrInUse)
            {
            // When the cache is full, it takes 65 - 85 miliseconds to write the index.
            // So wait 50 miliseconds and try again
            User::After(50000);
            }
        else
            {
            break;
            }
        }
    if( ret == KErrNone )
        {
        CleanupClosePushL( writeStream );
        iLookupTable->ExternalizeL( writeStream );
        writeStream.CommitL();
        CleanupStack::PopAndDestroy(); // writeStream
        }
    iHttpCacheObserver->StartObserver();
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::DeleteCacheEntry
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::DeleteCacheEntry(
    CHttpCacheEntry& aEntry,
    TBool aUpdate )
    {
    // suppress compiler and PC-lint warnings
    (void)aUpdate;

#ifdef __CACHELOG__
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                       _L("CHttpCacheHandler::DeleteCacheEntry"),
                                       aEntry.Filename(),
                                       aEntry.Url(),
                                       aEntry.BodySize(), 
                                       ELogEntrySize );
#endif

    // Remove from the lookuptable
    iLookupTable->EraseCacheEntry( aEntry.Url() );
    }

// -----------------------------------------------------------------------------
// There used to be a CHttpCacheHandler::FixLookupTableL here. 
// Go back in SVN to re-discover it :)
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CHttpCacheHandler::SaveBuffer
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheHandler::SaveBuffer(
    CHttpCacheEntry& aEntry,
    const TDesC8& aBuffer,
    TBool aBody )
    {
    TBool ok( EFalse );
    
    TRAPD( err, ok = CacheNeedsSpaceL( aBuffer.Length() ) );
    if ( err == KErrNone && ok )
        {
        // Did we have a safe save of the body or header
        ok = aBody ? iStreamHandler->SaveBodyData( aEntry, aBuffer ) : iStreamHandler->SaveHeaders( aEntry, aBuffer );
        }
#ifdef __CACHELOG__
    else
        {
        // stop saving this entry
        HttpCacheUtil::WriteUrlToLog( 0, _L( "item cannot be saved. remove it please" ), aEntry.Url() );
        }
#endif // __CACHELOG__
    return ok;
    }


// -----------------------------------------------------------------------------
// CHttpCacheHandler::UpdateLookupTable
//
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::UpdateLookupTable()
    {
    TRAP_IGNORE(UpdateLookupTableL());
    iHttpCacheObserver->StartObserver();
    }

// -----------------------------------------------------------------------------
// CHttpCacheHandler::UpdateLookupTableL
// Slow method due to much file-system interaction. Don't call it from performance critical code. 
// -----------------------------------------------------------------------------
//
void CHttpCacheHandler::UpdateLookupTableL()
    {
    CHttpCacheEvictionHandler* evictionHandler = CHttpCacheEvictionHandler::NewL();
    CleanupStack::PushL(evictionHandler);
    CHttpCacheLookupTable* lookupTable = CHttpCacheLookupTable::NewL( *evictionHandler, *iStreamHandler );
    CleanupStack::PushL(lookupTable);
    OpenLookupTableL(lookupTable);
    iLookupTable->MergeL(lookupTable, iRfs);
	CleanupStack::PopAndDestroy(2); // lookupTable, evictionHandler
    }
//  End of File
