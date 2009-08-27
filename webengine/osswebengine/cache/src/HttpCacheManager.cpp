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
* Description:  Implementation of CHttpCacheManager
*
*/

// INCLUDE FILES
#include "HttpCacheManager.h"
#include "HttpCacheHandler.h"
#include "HttpCacheUtil.h"
#include "HttpCacheFileWriteHandler.h"
#include <CenRepNotifyHandler.h>
#include <HttpCacheManagerInternalCRKeys.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include <FeatMgr.h>
#include <http/RHTTPTransaction.h>
#include <SysUtilDomainCRKeys.h>
#include <eikenv.h>
#include <f32file.h>
#include <e32hashtab.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// kbyte
const TUint KDefaultCacheSize = 1048576; // 1MB = 1024*1024
_LIT( KDefaultCacheDir, "c:\\cache\\");
_LIT( KDefaultOperatorCacheDir, "c:\\cache\\op\\");
_LIT( KDefaultVssCacheDir, "c:\\cache\\vss\\");
_LIT( KDefaultIndexFile, "index.dat" );
_LIT( KDefaultOpIndexFile, "index_op.dat" );
_LIT( KDefaultVSSIndexFile, "index_vss.dat" );
_LIT( KIndexFileExtension, ".dat" );
_LIT( KValidationFileExtension, ".val" );
_LIT8( KVSSHeaderFileldName, "X-Vodafone-Content" );
_LIT8( KVSSHeaderFileldValue, "Portal" );


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheManager::CHttpCacheManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheManager::CHttpCacheManager():iCacheFolder( KDefaultCacheDir )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheManager::ConstructL()
    {
    CreateCacheHandlersL();

    // read offline mode
    if( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        // check if connection is not allowed and
        // come up with a listerner on the offline setting
        TInt connAllowed( 1 );

        iOfflineRepository = CRepository::NewL( KCRUidCoreApplicationUIs );

        iOfflineRepository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );

        iOfflineMode = !connAllowed;

        iOfflineNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iOfflineRepository,
            CCenRepNotifyHandler::EIntKey, (TUint32)KCoreAppUIsNetworkConnectionAllowed );
        iOfflineNotifyHandler->StartListeningL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpCacheManager* CHttpCacheManager::NewL()
    {
    CHttpCacheManager* self = new( ELeave ) CHttpCacheManager();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CHttpCacheManager::~CHttpCacheManager()
    {
    delete iOfflineNotifyHandler;
    delete iOfflineRepository;
    delete iCache;
    delete iOperatorCache;
    delete iOpDomain;
    delete iphoneSpecificCache;
    delete iVSSWhiteList;
    delete iFileWriteHandler;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::VSSRequestCheck
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheManager::VSSRequestCheckL( const RHTTPTransaction& aTrans,
                                           const RHTTPHeaders& aHttpHeader,
                                           const TDesC8& aUrl )
    {
    TBool VSSTransaction ( EFalse );
    if ( iVSSCacheEnabled && HttpCacheUtil::VSSCacheContent( aUrl, iVSSWhiteList ) )
        {
        RStringPool strP = aTrans.Session().StringPool();

#ifdef __CACHELOG__
        RHTTPHeaders headers ( aHttpHeader );
        HBufC8* responseHeaderStr8 = HttpCacheUtil::HeadersToBufferLC( headers, strP );
        HBufC* responseHeaderStr;
        if( responseHeaderStr8 )
            {
            responseHeaderStr = HBufC::NewL( responseHeaderStr8->Length() );
            responseHeaderStr->Des().Copy( responseHeaderStr8->Des() );
            CleanupStack::PopAndDestroy(); //  responseHeaderStr8;
            TPtrC headerStr( responseHeaderStr->Des() );
            HttpCacheUtil::WriteLog( 0, _L("========== VSS Header Start =========\n") );
            HttpCacheUtil::WriteLog( 0, responseHeaderStr->Des() );
            HttpCacheUtil::WriteLog( 0, _L("========== VSS Header End =========\n") );
            delete responseHeaderStr;
            }
#endif

        TPtrC8 nameStr ( KVSSHeaderFileldName() );

        RStringF VSSnameStr = strP.OpenFStringL( nameStr );
        CleanupClosePushL<RStringF>( VSSnameStr);

        THTTPHdrVal tempVal;
        if ( aHttpHeader.GetField( VSSnameStr, 0, tempVal ) == KErrNone )
            {
            TPtrC8 valueStr ( KVSSHeaderFileldValue() );
            RStringF VSSValueStr = strP.OpenFStringL( valueStr );
            CleanupClosePushL<RStringF>( VSSValueStr );

            if ( tempVal == VSSValueStr )
                {
                VSSTransaction      = ETrue;
                }
            CleanupStack::PopAndDestroy(); // VSSValueStr
            }
        CleanupStack::PopAndDestroy(); // VSSnameStr
        } //end if( iVSSCacheEnabled && HttpCacheUtil::VSSCacheContent( aUrl, iVSSWhiteList ) )
    return VSSTransaction;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RequestL(
    RHTTPTransaction& aTrans,
    TBrCtlDefs::TBrCtlCacheMode aCacheMode,
    THttpCacheEntry& aCacheEntry )
    {
    //
    TInt status( KErrNotFound );
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        CHttpCacheHandler* cache = CacheHandler( aTrans.Request().URI().UriDes(), NULL ) ;
        __ASSERT_DEBUG( cache, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
        // adjust cache mode in case of offline operation
        if( iOfflineMode )
            {
            aCacheMode = TBrCtlDefs::ECacheModeOnlyCache;
            }
        //
        if( cache )
            {
            status = cache->RequestL( aTrans, aCacheMode, aCacheEntry );
            // save cache handler even if the entry is not in the cache
            // so that when the response comes back, we do not need to
            // check the url
             if( status == KErrNotFound && iVSSCacheEnabled )
                {
                 status = iphoneSpecificCache->RequestL( aTrans, aCacheMode, aCacheEntry );
                 if( (status  == KErrNotReady) || (status == KErrNone ) )
                 {
                 aCacheEntry.iCacheHandler = iphoneSpecificCache;
                 }
                 else
                 {
                 // No entry found in any cache. Default to normal cache
                 aCacheEntry.iCacheHandler = cache;
                 }
                }
            else
            {
            aCacheEntry.iCacheHandler = cache;
            }
           } //end if( cache )

        }//end  if( iCacheEnabled || iVSSCacheEnabled )
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestHeadersL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RequestHeadersL(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    TInt status( KErrNotFound );
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        status = aCacheEntry.iCacheHandler->RequestHeadersL( aTrans, aCacheEntry );
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestNextChunkL
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CHttpCacheManager::RequestNextChunkL(
    RHTTPTransaction& aTrans,
    TBool& aLastChunk,
    THttpCacheEntry& aCacheEntry )
    {
    HBufC8* bodyStr = NULL;
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        bodyStr = aCacheEntry.iCacheHandler->RequestNextChunkL( aTrans, aLastChunk, aCacheEntry );
        }

    return bodyStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestClosed
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::RequestClosed(
    RHTTPTransaction* aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        aCacheEntry.iCacheHandler->RequestClosed( aTrans, aCacheEntry );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ReceivedResponseHeadersL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::ReceivedResponseHeadersL(
  RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    if ( iCacheEnabled || iVSSCacheEnabled )
        {
        if ( iVSSCacheEnabled &&
             VSSRequestCheckL( aTrans, aTrans.Response().GetHeaderCollection(),
                               aTrans.Request().URI().UriDes() ) )
            {
            //Modify the cache handler if VSS specific
            aCacheEntry.iCacheHandler = iphoneSpecificCache;
            }
        if ( aCacheEntry.iCacheHandler )
            {
            aCacheEntry.iCacheHandler->ReceivedResponseHeadersL( aTrans, aCacheEntry );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ReceivedResponseBodyDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::ReceivedResponseBodyDataL(
  RHTTPTransaction& aTrans,
    MHTTPDataSupplier& aBodyDataSupplier,
    THttpCacheEntry& aCacheEntry )
    {
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        aCacheEntry.iCacheHandler->ReceivedResponseBodyDataL( aTrans, aBodyDataSupplier, aCacheEntry );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ResponseComplete
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::ResponseComplete(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        aCacheEntry.iCacheHandler->ResponseComplete( aTrans, aCacheEntry );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RemoveAllL
// Removes all files from HTTP cache. Also tries to remove orphaned files,
// i.e files found on disk, but not registered in the cache's lookup table
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RemoveAllL()
    {
    TInt numOfBytes( 0 );
    // do not remove op cache
    if( iCacheEnabled )
        {
        numOfBytes = iCache->RemoveAllL();
        }

    //failure here is not mission critical
    TRAP_IGNORE( RemoveOrphanedFilesL() );

    return numOfBytes;
    }

// -----------------------------------------------------------------------------
// FilePathHash
// Hash function for Symbian file paths: discards case first
// -----------------------------------------------------------------------------
static TUint32 FilepathHash(const TDesC& aDes)
{
    //since this function is intensively used by the HashMap,
    //keeping (slow) heap allocation out of it.
    TBuf<KMaxPath> normalized ( aDes );

    normalized.LowerCase();
    return DefaultHash::Des16( normalized );
}

// -----------------------------------------------------------------------------
// FilepathIdent
// Comparator for Symbian file paths: Use case-insensitive compare
// -----------------------------------------------------------------------------
static TBool FilepathIdent(const TDesC& aL, const TDesC& aR)
{
    return ( aL.CompareF(aR) == 0 );
}

// -----------------------------------------------------------------------------
// CHttpCacheManager::RemoveOrphanedFilesL
// Removes header/body files that exist on the file-system, but are not known to the in-memory Cache lookup table(s)
// We do this because cache performance degrades substantially if there are too many files in a Symbian FAT32 directory.
// Browser crash or out-of-battery situations may cause SaveLookuptable() to be not called, leading to such "orphaned files".
// Due to high file-server interaction, don't call this method from performance critical code.
// -----------------------------------------------------------------------------
void CHttpCacheManager::RemoveOrphanedFilesL()
    {
    //Map that contains pointers to fully-qualified file paths as Keys, and "to be deleted flag" as Value.
    RPtrHashMap<TDesC, TInt> onDiskFilesMap(&FilepathHash, &FilepathIdent);
    CleanupClosePushL( onDiskFilesMap );

    //Pointers to the following TInt are used as VALUES in the HashMap...
    // so they must be in scope for the lifecycle of the HashMap.
    const TInt needsDelete( 1 );
    const TInt noDelete( 0 );

    //collects objects that need to be deleted later on
    RPointerArray<HBufC> cleanupList;
    CleanupResetAndDestroyPushL( cleanupList );

    RFs rfs = CCoeEnv::Static()->FsSession();
    CDirScan* scanner = CDirScan::NewLC( rfs );

    //Configure CDirScan to tell you all contents of a particular directory hierarchy
    scanner->SetScanDataL( iCacheFolder, KEntryAttNormal, ESortNone );
    CDir* matchingFiles( 0 );

    //Step 1. Find out all files on disk: by walking the directory hierarchy, one directory at a time
    for (;;)
        {
        //1a. Get list of files in current directory, NULL if no directory left in tree
        scanner->NextL( matchingFiles );
        if ( !matchingFiles )
            break;

        TPtrC dir( scanner->FullPath() );

        //1b. Add any files found to the HashTable
        const TInt nMatches = matchingFiles->Count();
        for ( TInt i = 0; i < nMatches; i++ )
            {
            TEntry entry ( (*matchingFiles)[i] ) ;
            TPtrC ext( entry.iName.Right( KIndexFileExtension().Length() ));

            if ( ext.CompareF( KIndexFileExtension ) != 0 && // ignore any .dat index files
                 ext.CompareF( KValidationFileExtension ) != 0 ) // ignore any .val index files
                {
                HBufC* fullPath = HBufC::NewL( dir.Length() +  entry.iName.Length() );
                cleanupList.Append( fullPath ); //keep object safe for later destruction
                fullPath->Des().Append( dir );
                fullPath->Des().Append( entry.iName ); //a fully qualified file path
                onDiskFilesMap.Insert( fullPath, &needsDelete ); //add to the hash
                }
            }

        delete matchingFiles;
        } // End of step 1: adding all known files on disk to Map

    CleanupStack::PopAndDestroy( 1, scanner );

#ifdef __CACHELOG__
    {
    HttpCacheUtil::WriteFormatLog(0, _L("-----------START PRINTING MAP OF SIZE %d---------"), onDiskFilesMap.Count());
    TPtrHashMapIter<TDesC, TInt> iter(onDiskFilesMap);
    const TDesC* key;
    while ((key = iter.NextKey()) != 0)
        {
        const TInt val = *(iter.CurrentValue());
        HttpCacheUtil::WriteFormatLog(0, _L("MAP WALK: %S, with value = %d "), key, val);
        }
    HttpCacheUtil::WriteFormatLog(0, _L("-----------DONE PRINTING MAP-------------"));
    }
#endif

    //Step 2. Get list of known (non-orphaned) files in each Cache's in-memory lookup table. Flag them as DO NOT DELETE
    RPointerArray<TDesC> knownFiles;
    CleanupClosePushL( knownFiles );
    //Ask CacheHandlers to add their KNOWN files to this array. No ownership transfer occurs.
    //Don't go ahead if any of the cache handlers choke to insure correct deletion of files.
    if (iCache)
        User::LeaveIfError( iCache->ListFiles( knownFiles ) );
    if (iOperatorCache)
        User::LeaveIfError( iOperatorCache->ListFiles( knownFiles ) );
    if (iphoneSpecificCache)
        User::LeaveIfError( iphoneSpecificCache->ListFiles( knownFiles ) );

    //2a. HashTable lookup, and modification of flag
    for (TInt i = 0; i < knownFiles.Count(); i++)
        {
        //lookup filename
        TInt* ptr = onDiskFilesMap.Find( *(knownFiles[i]) );
        if (ptr)
            {
            // Reinsert into Map, this time with NO DELETE
            onDiskFilesMap.Insert( knownFiles[i], &noDelete );
#if 0 // no header files any more.
            // Add the header file to HashMap
            HBufC* headerFile = HBufC::NewL( KHttpCacheHeaderExt().Length() +  (*(knownFiles[i])).Length() );
            cleanupList.Append( headerFile ); //keep for later destruction
            TPtr ptr( headerFile->Des() );
            HttpCacheUtil::GetHeaderFileName( *(knownFiles[i]), ptr );
            onDiskFilesMap.Insert( headerFile, &noDelete ); // register Header files as NO DELETE
#endif
            }
        }

    knownFiles.Close();
    CleanupStack::Pop( 1, &knownFiles );

    //Step 3. Delete all files on disk that don't belong to any of the Cache Handlers.
    CFileMan* fileMan = CFileMan::NewL( rfs );
    TPtrHashMapIter<TDesC, TInt> iter( onDiskFilesMap );
    const TDesC* key;
    while ((key = iter.NextKey()) != 0)
        {
        const TInt value ( *(iter.CurrentValue()) );
        if ( value == 1 ) { // file needs deletion
            fileMan->Delete( *key );
        }
        }
    delete fileMan;

    CleanupStack::Pop(1, &cleanupList);
    cleanupList.ResetAndDestroy(); //should delete all HBufC objects

    CleanupStack::Pop(1, &onDiskFilesMap);
    onDiskFilesMap.Close(); // doesn't own any K,V object

    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RemoveL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RemoveL(
    const TDesC8& aUrl )
    {
    TInt status( KErrNotFound );
    if( iCacheEnabled )
        {
        // do not remove op cache
        status = iCache->RemoveL( aUrl );
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::Find
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CHttpCacheManager::Find(
    const TDesC8& aUrl )
    {
    TBool found( EFalse );
    //
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        CHttpCacheHandler* cache = CacheHandler( aUrl, NULL );

        __ASSERT_DEBUG( cache, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
        //
        if( cache )
            {
            found = cache->Find( aUrl );
            }

        if( !found && iVSSCacheEnabled )
            {
            found = iphoneSpecificCache->Find( aUrl );
            }
        }
    return found;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::VSSHeaderCheck
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheManager::VSSHeaderCheck( TDes8*  aHttpHeaderString ) const
    {
    TBool found ( EFalse );
    if( aHttpHeaderString->Size() > 0 )
        {
        TPtrC8 nameStr8( KVSSHeaderFileldName() );

        TInt VSSnameLocation = aHttpHeaderString->FindC( nameStr8 ) ;

        if ( VSSnameLocation != KErrNotFound )
            {
            TPtrC8 valueStr8( KVSSHeaderFileldValue() );
            TInt VSSvalueLocation = aHttpHeaderString->FindC( KVSSHeaderFileldValue() );

            if ( (VSSvalueLocation != KErrNotFound ) && ( VSSnameLocation < VSSvalueLocation ) )
                {
                found = ETrue;
                }
            } //end if ( VSSnameLocation != KErrNotFound )
        } //end  if( aHttpHeaderString.Size() > 0 )

    return found;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::SaveL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CHttpCacheManager::SaveL(
    const TDesC8& aUrl,
    const TDesC8& aHeader,
    const TDesC8& aContent,
    const TDesC8&  aHttpHeaderString)
    {
    TBool saved( EFalse );
    //
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        HBufC8* headerStr = HBufC8::NewL( aHttpHeaderString.Length() );
        headerStr->Des().Copy( aHttpHeaderString );
        TPtr8 headerStrPtr8 ( headerStr->Des() ); //Any Type of TPtrc8
        CHttpCacheHandler* cache = CacheHandler( aUrl, &headerStrPtr8 );
        delete headerStr;
        __ASSERT_DEBUG( cache, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
        //
        if( cache )
            {
            saved = cache->SaveL( aUrl, aHeader, aContent );
            }
        }
    return saved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::AddHeaderL
// Not supported for Vodafone specific storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::AddHeaderL(
    const TDesC8& aUrl,
    const TDesC8& aName,
    const TDesC8& aValue )
    {
    TBool status( KErrNotFound );
    //
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        CHttpCacheHandler* cache = CacheHandler( aUrl, NULL );
        __ASSERT_DEBUG( cache, User::Panic( _L("cacheHandler Panic"), KErrCorrupt ) );
        //
        if( cache )
            {
            status = cache->AddHeaderL( aUrl, aName, aValue );
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::CreateCacheHandlersL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheManager::HandleNotifyString(
    const TUint32 aKeyId,
    const TDesC& aValue )
    {
    // check offline mode
    if( aKeyId == KCoreAppUIsNetworkConnectionAllowed )
        {
        iOfflineMode = ( aValue == _L("1") ? EFalse : ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::CreateCacheHandlersL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheManager::CreateCacheHandlersL()
    {
    // read cache settings
    CRepository* repository = CRepository::NewLC( KCRUidCacheManager );
    CRepository* repositoryDiskLevel = CRepository::NewLC( KCRUidDiskLevel );
    TInt err;

    // Get Cache Postpone Parameters.
    //
    THttpCachePostponeParameters postpone;

    if (KErrNone == repository->Get(KCacheWritePostponeEnabled, postpone.iEnabled) )
        {
        User::LeaveIfError( repository->Get( KCacheWritePostponeFreeRAMThreshold, postpone.iFreeRamThreshold ) );
        User::LeaveIfError( repository->Get( KCacheWritePostponeImmediateWriteThreshold, postpone.iImmediateWriteThreshold ) );
        User::LeaveIfError( repository->Get( KCacheWritePostponeWriteTimeout, postpone.iWriteTimeout ) );
        }

    // cache on/off
    TInt cacheEnabled( 0 );
    err = repository->Get( KCacheManagerHttpCacheEnabled, cacheEnabled );

    iCacheEnabled = cacheEnabled;

        // cache size
        TInt cacheSize( KDefaultCacheSize );
        repository->Get( KCacheManagerHttpCacheSize, cacheSize );

         repository->Get( KCacheManagerHttpCacheFolder, iCacheFolder );
        // fix folder by appending trailing \\ to the end -symbian thing
        // unless it is already there
        if( iCacheFolder.LocateReverse( '\\' ) != iCacheFolder.Length() - 1  )
            {
            iCacheFolder.Append( _L("\\") );
            }

    // get drive letter for sysutil
    TParsePtrC pathParser( iCacheFolder );
    TDriveUnit drive = pathParser.Drive();
    // get critical level
    // RAM drive can have different critical level
    TVolumeInfo vinfo;
    User::LeaveIfError( CCoeEnv::Static()->FsSession().Volume( vinfo, drive ) );
    //
    TInt criticalLevel;
    User::LeaveIfError( repositoryDiskLevel->Get( ( vinfo.iDrive.iType == EMediaRam ? KRamDiskCriticalLevel : KDiskCriticalThreshold ),
        criticalLevel ) );

    if ( (err == KErrNone) && iCacheEnabled )
        {
        // create cache handler
        iCache = CHttpCacheHandler::NewL( cacheSize, iCacheFolder, KDefaultIndexFile(), criticalLevel, postpone);

        // create operator cache. same settings
        if ( FeatureManager::FeatureSupported( KFeatureIdOperatorCache ) )
            {
            TBuf<512> url;
            // if domain is missing, then no need to read further
            if ( repository->Get( KOperatorDomainUrl, url ) == KErrNone )
                {

                HBufC8* opDomain8 = HBufC8::NewL( url.Length() );
                CleanupStack::PushL(opDomain8);
                opDomain8->Des().Append( url );

                TInt slashPos = opDomain8->LocateReverse('/');
                if (slashPos == -1)
                    {
                    slashPos = 0;
                    }

                TPtrC8 temp = opDomain8->Left(slashPos);
                iOpDomain = temp.AllocL();
                CleanupStack::PopAndDestroy(opDomain8);

                // op cache size
                TInt opCacheSize( KDefaultCacheSize );
                repository->Get( KOperatorCacheSize, opCacheSize );

                // op cache folder
                TFileName opCacheFolder( KDefaultOperatorCacheDir );
                repository->Get( KOperatorCacheFolder, opCacheFolder );

                if ( opCacheFolder.LocateReverse( '\\' ) != opCacheFolder.Length() - 1  )
                    {
                    opCacheFolder.Append( _L("\\") );
                    }

                // create op cache
                iOperatorCache = CHttpCacheHandler::NewL( opCacheSize, opCacheFolder, KDefaultOpIndexFile(), criticalLevel, postpone);
                }
            } //end if( FeatureManager::FeatureSupported( KFeatureIdOperatorCache ) )
        } //end if( iCacheEnabled )

    TInt VSScacheEnabled( 0 );
    err = repository->Get( KPhoneSpecificCacheEnabled, VSScacheEnabled );

    iVSSCacheEnabled = VSScacheEnabled;

    if ( (err == KErrNone) && iVSSCacheEnabled )
        {
        // cache size
        TInt VSScacheSize( KDefaultCacheSize );
        repository->Get( KPhoneSpecificCacheSize, VSScacheSize );

        // cache folder
        TFileName VSScacheFolder( KDefaultVssCacheDir );
        // ignore cache folder. use c:\ to save memory. (same for operator cache. see below)
        repository->Get( KPhoneSpecificCacheFolder, VSScacheFolder );
        // fix folder by appending trailing \\ to the end -symbian thing
        // unless it is already there
        if( VSScacheFolder.LocateReverse( '\\' ) != VSScacheFolder.Length() - 1  )
            {
            VSScacheFolder.Append( _L("\\") );
            }

        //Get the white list
        TBuf<2048> whiteList;

        if ( repository->Get( KPhoneSpecificCacheDomainUrl, whiteList ) == KErrNone )
            {
            iVSSWhiteList = HBufC8::NewL( whiteList.Length() );
            iVSSWhiteList->Des().Append( whiteList );
            }
        else
            {
            iVSSWhiteList = NULL;
            }

        // create cache handler
        iphoneSpecificCache = CHttpCacheHandler::NewL( VSScacheSize, VSScacheFolder, KDefaultVSSIndexFile(), criticalLevel, postpone);
        }

    CleanupStack::PopAndDestroy(2); // repository, , repositoryDiskLevel
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::CacheHandler
//
// -----------------------------------------------------------------------------
//
CHttpCacheHandler* CHttpCacheManager::CacheHandler(
    const TDesC8& aUrl,
    TDes8* aHttpHeaderString ) const
    {
    CHttpCacheHandler* cache;
    if( iVSSCacheEnabled && aHttpHeaderString && VSSHeaderCheck( aHttpHeaderString ) )
        {
        cache = iphoneSpecificCache;
        }
    else
        {
        cache = ( ( iOpDomain && HttpCacheUtil::OperatorCacheContent( iOpDomain->Des(), aUrl ) ) ? iOperatorCache : iCache );
        }
    return cache;
    }
//  End of File
