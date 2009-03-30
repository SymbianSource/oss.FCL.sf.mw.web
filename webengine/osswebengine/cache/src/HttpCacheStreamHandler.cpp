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
* Description:  Implementation of CHttpCacheStreamHandler
*
*/

// INCLUDE FILES
#include <f32file.h>
#include "HttpCacheStreamHandler.h"
#include "HttpCacheEntry.h"
#include "HttpCacheUtil.h"
#include "HttpCacheHandler.h"
#include <centralrepository.h>
#include <SysUtilDomainCRKeys.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KHttpCacheActiveCount = 20;
const TInt KBufferSize = 32768;
#if 0
const TInt KHttpCacheChunkSize = 2048;
#endif // 0
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::CHttpCacheStreamEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamEntry::CHttpCacheStreamEntry(
    RFs& aRfs,
    CHttpCacheEntry& aHttpCacheEntry,
    TDriveUnit aDrive,
    TInt64 aCriticalLevel )
    : iRfs( aRfs ), iHttpCacheEntry( &aHttpCacheEntry ), iDrive( aDrive ), iCriticalLevel( aCriticalLevel )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamEntry::ConstructL()
    {
    // consistency check on header/body files
    // open the file or create one
    iFileOk = ( iHttpCacheEntry->State() == CHttpCacheEntry::ECacheUninitialized ? CreateNewFilesL() : OpenCacheFiles() );
    if( !iFileOk )
        {
        User::Leave( KErrCorrupt );
        }
    else if( iFileOk && iHttpCacheEntry->State() == CHttpCacheEntry::ECacheUninitialized )
        {
        iHttpCacheEntry->SetState( CHttpCacheEntry::ECacheInitialized );
        }
    iCacheBuffer = HBufC8::NewL( KBufferSize );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamEntry* CHttpCacheStreamEntry::NewL(
    RFs& aRfs,
    CHttpCacheEntry& aHttpCacheEntry,
    TDriveUnit aDrive,
    TInt64 aCriticalLevel )
    {
    CHttpCacheStreamEntry* self = new( ELeave ) CHttpCacheStreamEntry( aRfs,
        aHttpCacheEntry, aDrive, aCriticalLevel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CHttpCacheStreamEntry::~CHttpCacheStreamEntry()
    {
    // commit changes
    if( iFileOk )
        {
        iHeaderFile.Close();
        iBodyFile.Close();
        }
    delete iCacheBuffer;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::Erase
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamEntry::Erase()
    {
    //
    HttpCacheUtil::WriteUrlToLog( 0, _L( "erase files associated with" ), iHttpCacheEntry->Url() );
    iHeaderFile.Close();
    iBodyFile.Close();
    // dont care about return vales
    // as we cannot do much
    TFileName bodyFileName;
    // get body filename
    BodyFileName( bodyFileName );

    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( bodyFileName, headerFileName );
    //
    TInt status;
    status = iRfs.Delete( bodyFileName );
    HttpCacheUtil::WriteLog( 0, bodyFileName, status );
    //
    status = iRfs.Delete( headerFileName );
    HttpCacheUtil::WriteLog( 0, headerFileName, status );
    // do not close them twice
    iFileOk = EFalse;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::HeadersL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamEntry::HeadersL()
    {
    //
    HBufC8* headerStr = NULL;
    TInt headerLen( 0 );
    TInt err( iHeaderFile.Size( headerLen ) );
    //
    if( err == KErrNone && headerLen > 0 )
        {
        headerStr = HBufC8::NewL( headerLen );
        TPtr8 ptr( headerStr->Des() );
        // read headers
        iHeaderFile.Read( 0, ptr, headerLen );
        }
    return headerStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::NextChunkL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamEntry::NextChunkL(
    TBool& aLastChunk )
    {
    HBufC8* bodyStr = NULL;
#if 0
    // incremental chunk handling
    TInt size;
    TInt contentSize( iBodyFile.Size( size ) );
    size = Min( KHttpCacheChunkSize, contentSize );

    bodyStr = HBufC8::NewL( size );
    TPtr8 ptr( bodyStr->Des() );
    //
    iBodyFile.Read( ptr, size );
    // check if we are at the end of the file
    aLastChunk = ( bodyStr->Length() != size );
#else // 0
    // read body
    TInt size;
    TInt err( iBodyFile.Size( size ) );
    if( err == KErrNone && size > 0 )
        {
        bodyStr = HBufC8::NewL( size );
        TPtr8 ptr( bodyStr->Des() );
        //
        iBodyFile.Read( ptr, size );
        }
    aLastChunk = ETrue;
#endif // 0
    return bodyStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::SaveHeaders
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheStreamEntry::SaveHeaders(
    const TDesC8& aHeaderStr )
    {
    TInt save( KErrNone );

    if( aHeaderStr.Length() )
        {
        // below critical level
        TBool below( ETrue );

        below = DiskSpaceBelowCriticalLevel( aHeaderStr.Length() );

        if( !below )
            {
            // save headers
            // Don't force a flush, as the File Server takes care of write and read consistency.
            iHttpCacheEntry->SetHeaderSize( aHeaderStr.Length() );
            save = iHeaderFile.Write( aHeaderStr );
            }
        else
            {
            save = KErrDiskFull;
            }
        }
    return save;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::RemoveHeaders
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamEntry::RemoveHeaders()
    {
    // destroy data
    iHeaderFile.SetSize( 0 );
    iHttpCacheEntry->SetHeaderSize( 0 );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::SaveBodyData
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheStreamEntry::SaveBodyData(
    const TDesC8& aBodyStr )
    {
    TBool save( KErrNone );
    TInt bodyLength( aBodyStr.Length() );

    if( bodyLength )
        {
        TPtr8 buffer( iCacheBuffer->Des() );
        if( buffer.Length() + bodyLength > buffer.MaxLength() )
            {
            //
            HBufC8* overflowBuffer = NULL;
            TInt cut( -1 );
            // running out of space
            TPtrC8 writePtr;
            if( buffer.Length() == 0 )
                {
                // buffer is empty and the body is bigger than the buffer
                writePtr.Set( aBodyStr );
                }
            else
                {
                cut =  buffer.MaxLength() - buffer.Length();
                // enough space for the leftover?
                if( bodyLength - cut > buffer.MaxLength() )
                    {
                    // not enough
                    // put the buffer and the body together and
                    // write it in one go.
                    overflowBuffer = HBufC8::New( buffer.Length() + bodyLength );
                    if( !overflowBuffer )
                        {
                        return KErrNoMemory;
                        }
                    TPtr8 overflowPtr( overflowBuffer->Des() );
                    overflowPtr.Copy( buffer );
                    overflowPtr.Append( aBodyStr );
                    writePtr.Set( overflowBuffer->Des() );
                    // empty buffer
                    buffer.Zero();
                    // no leftover left
                    cut = -1;
                    }
                else
                    {
                    // fill the 32k
                    buffer.Append( aBodyStr.Left( cut ) );
                    writePtr.Set( buffer );
                    }
                }

            // write to the disk
            TBool below;
            below = DiskSpaceBelowCriticalLevel( writePtr.Length() );

            if( !below )
                {
                // write body
                save = iBodyFile.Write( writePtr );
                }
            else
                {
                save = KErrDiskFull;
                // reset buffers
                buffer.Zero();
                }
            //
            if( save == KErrNone && cut >= 0 )
                {
                // copy the leftover in to the buffer
                buffer.Copy( aBodyStr.Mid( cut ) );
                }
            delete overflowBuffer;
            }
        else
            {
            buffer.Append( aBodyStr );
            save = KErrNone;
            }
        // update size information
        iHttpCacheEntry->SetSize( iHttpCacheEntry->Size() + bodyLength );
        }
    return save;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::RemoveBodyData
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamEntry::RemoveBodyData()
    {
    // destroy data
    iCacheBuffer->Des().Zero();
    iBodyFile.SetSize( 0 );
    iHttpCacheEntry->SetSize( 0 );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::Flush
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheStreamEntry::Flush()
    {
    TInt saveOk( KErrNone );

    if( iCacheBuffer->Length() )
        {
        TPtr8 bufferPtr( iCacheBuffer->Des() );

        TBool below;
        below = DiskSpaceBelowCriticalLevel( bufferPtr.Length() );

        if( !below )
            {
            // append body
            saveOk = iBodyFile.Write( bufferPtr );
            }
        else
            {
            saveOk = KErrDiskFull;
            }
        bufferPtr.Zero();
        }
    return saveOk;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::OpenCacheFiles
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamEntry::OpenCacheFiles()
    {
    TInt statusHeader;
    TInt statusBody;
    //
    TFileName bodyFileName;
    // get body filename
    BodyFileName( bodyFileName );
    // header filename
    TFileName headerFileName;
    //
    HttpCacheUtil::GetHeaderFileName( bodyFileName, headerFileName );

    statusHeader = iHeaderFile.Open( iRfs, headerFileName, EFileShareExclusive | EFileWrite );
    statusBody = iBodyFile.Open( iRfs, bodyFileName, EFileShareExclusive | EFileWrite );
    return ( statusHeader == KErrNone && statusBody == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::CreateNewFilesL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamEntry::CreateNewFilesL()
    {
    TInt statusHeader( KErrNotFound );
    TInt statusBody( KErrNotFound );
    TPath sessionPath;
    User::LeaveIfError( iRfs.SessionPath( sessionPath ) );

    //Given the full URL, generates a fully qualified path for saving the HTTP response
    HBufC* bodyFileName = HttpCacheUtil::GenerateNameLC( iHttpCacheEntry->Url(), sessionPath );
    TPtrC bodyFileNamePtr( *bodyFileName );
    // get header file name
    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( bodyFileNamePtr, headerFileName );

    // create a file or replace if it exists. 
    statusBody = iBodyFile.Replace( iRfs, bodyFileNamePtr, EFileShareExclusive | EFileWrite );
    if( statusBody == KErrNone )
        {
        // header file should not fail
        statusHeader = iHeaderFile.Replace( iRfs, headerFileName, EFileShareExclusive | EFileWrite );
        }
    //
    TBool fileOk( statusHeader == KErrNone && statusBody == KErrNone );
#ifdef __CACHELOG__ 
    HttpCacheUtil::WriteUrlToLog( 0, bodyFileNamePtr, iHttpCacheEntry->Url() );
#endif 

    //
    if( fileOk )
        {
        iHttpCacheEntry->SetFileNameL( bodyFileNamePtr );
#ifdef __CACHELOG__ 
        HttpCacheUtil::WriteUrlToLog( 0, _L8("files are fine") );
#endif 
        }
    else
        {
        // corrupt entry. delete the file
        if( statusBody == KErrNone )
            {
            iRfs.Delete( bodyFileNamePtr );
            }
        // ???
        __ASSERT_DEBUG( EFalse,
            User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
        }
    CleanupStack::PopAndDestroy( bodyFileName );
    return fileOk;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::BodyFileName
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamEntry::BodyFileName(
    TFileName& aBodyFileName )
    {
    TFileName bodyFileName;
    aBodyFileName.Copy( iHttpCacheEntry->Filename()  );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamEntry::BodyFileName
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamEntry::DiskSpaceBelowCriticalLevel(
    TInt aContentSize )
    {
    TVolumeInfo vinfo;
    TInt errorCode = iRfs.Volume( vinfo, iDrive );

    return( errorCode != KErrNone || ( vinfo.iFree - aContentSize ) <= iCriticalLevel );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::CHttpCacheStreamHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler::CHttpCacheStreamHandler() : iDiskFull( EFalse )
    {
    // change the iDiskFull back to false if somebody freed some disk space.
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::ConstructL(
    const TDesC& aDirectory,
    TInt aCriticalLevel )
    {
    User::LeaveIfError( iRfs.Connect() );
    // set path for the entries
    iRfs.SetSessionPath( aDirectory );
    iActiveEntries = new( ELeave )CArrayPtrFlat<CHttpCacheStreamEntry>( KHttpCacheActiveCount );
    // get drive letter for sysutil
    TParsePtrC pathParser( aDirectory );
    iDrive = pathParser.Drive();
    iCriticalLevel = aCriticalLevel;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler* CHttpCacheStreamHandler::NewL(
    const TDesC& aDirectory ,
    TInt aCriticalLevel)
    {
    CHttpCacheStreamHandler* self = new( ELeave ) CHttpCacheStreamHandler();

    CleanupStack::PushL( self );
    self->ConstructL( aDirectory , aCriticalLevel);
    CleanupStack::Pop();

    return self;
    }

// Destructor
CHttpCacheStreamHandler::~CHttpCacheStreamHandler()
    {
    if( iActiveEntries )
        {
        iActiveEntries->ResetAndDestroy();
        }
    delete iActiveEntries;
    iRfs.Close();
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::AttachL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::AttachL(
    CHttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    // check for duplicates
    for( TInt i = 0; i < iActiveEntries->Count(); i++ )
        {
        __ASSERT_DEBUG( iActiveEntries->At( i )->CacheEntry() != &aCacheEntry,
            User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
        }
#endif // __CACHELOG__
    TBool attached( ETrue );
    // create and save stream entry
    CHttpCacheStreamEntry* streamEntry = NULL;

    TRAPD( err, streamEntry = CHttpCacheStreamEntry::NewL( iRfs, aCacheEntry, iDrive, iCriticalLevel ) );
    if( err == KErrCorrupt )
        {
        //
        attached = EFalse;
        }
    else if( err == KErrNoMemory )
        {
        User::Leave( err );
        }
    else if( streamEntry )
        {
        iActiveEntries->AppendL( streamEntry );
        }
    return attached;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Detach
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::Detach(
    const CHttpCacheEntry& aCacheEntry )
    {
    TInt index;
    CHttpCacheStreamEntry* streamEntry = FindStreamEntry( aCacheEntry, &index );
    __ASSERT_DEBUG( streamEntry != NULL, User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );

    if( streamEntry )
        {
        delete streamEntry;
        iActiveEntries->Delete( index );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Erase
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::Erase(
    const CHttpCacheEntry& aCacheEntry )
    {
    TInt index;
    CHttpCacheStreamEntry* streamEntry = FindStreamEntry( aCacheEntry, &index );
    __ASSERT_DEBUG( streamEntry != NULL, User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );

    if( streamEntry )
        {
        streamEntry->Erase();
        //
        iContentSize-=aCacheEntry.Size();
        iContentSize-=aCacheEntry.HeaderSize();
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::HeadersL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamHandler::HeadersL(
    CHttpCacheEntry& aCacheEntry )
    {
    CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
    HBufC8* headerStr = NULL;
    //
    __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
    //
    if( entry )
        {
        headerStr = entry->HeadersL();
        }
    return headerStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::NextChunkL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamHandler::NextChunkL(
    CHttpCacheEntry& aCacheEntry,
    TBool& aLastChunk )
    {
    //
    CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
    HBufC8* bodyStr = NULL;
    //
    __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
    //
    if( entry )
        {
        bodyStr = entry->NextChunkL( aLastChunk );
        }
    return bodyStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::SaveHeaders
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::SaveHeaders(
    CHttpCacheEntry& aCacheEntry,
    const TDesC8& aHeaderStr )
    {
    TBool saved( KErrGeneral );
    if( !iDiskFull )
        {
        CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
        //
        __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
        //
        if( entry )
            {
            saved = entry->SaveHeaders( aHeaderStr );
            // update content size in cache
            if( saved == KErrNone )
                {
                iContentSize+=aHeaderStr.Length();
                }
            else if( saved == KErrDiskFull )
                {
                iDiskFull = ETrue;
                }
            }
        }
    return( saved == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::RemoveHeaders
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::RemoveHeaders(
    CHttpCacheEntry& aCacheEntry )
    {
    CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
    //
    __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
    //
    if( entry )
        {
        iContentSize-=aCacheEntry.HeaderSize();
        entry->RemoveHeaders();
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::SaveBodyData
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::SaveBodyData(
    CHttpCacheEntry& aCacheEntry,
    const TDesC8& aBodyStr )
    {
    TInt saved( KErrGeneral );
    if( !iDiskFull )
        {
        CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
        //
        __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
        //
        if( entry )
            {
            saved = entry->SaveBodyData( aBodyStr );
            if( saved == KErrNone )
                {
                iContentSize+=aBodyStr.Length();
                }
            else if( saved == KErrDiskFull )
                {
                iDiskFull = ETrue;
                }
            }
        }
    return( saved == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::RemoveBodyData
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::RemoveBodyData(
    CHttpCacheEntry& aCacheEntry )
    {
    CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
    //
    __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
    //
    if( entry )
        {
        iContentSize-=aCacheEntry.Size();
        entry->RemoveBodyData();
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Flush
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::Flush(
    CHttpCacheEntry& aCacheEntry )
    {
    TInt saved( KErrGeneral );
    if( !iDiskFull )
        {
        CHttpCacheStreamEntry* entry = FindStreamEntry( aCacheEntry );
        //
        __ASSERT_DEBUG( entry != NULL,  User::Panic( _L("cacheHandler Panic"), KErrCorrupt )  );
        //
        if( entry )
            {
            saved = entry->Flush();
            //
            if( saved == KErrDiskFull )
                {
                iDiskFull = ETrue;
                }
            }
        }
    return( saved == KErrNone );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::FindStreamEntry
//
// -----------------------------------------------------------------------------
//
CHttpCacheStreamEntry* CHttpCacheStreamHandler::FindStreamEntry(
    const CHttpCacheEntry& aCacheEntry,
    TInt* aIndex )
    {
    CHttpCacheStreamEntry* streamEntry = NULL;

    for( TInt i = 0; i < iActiveEntries->Count(); i++ )
        {
        CHttpCacheStreamEntry* entry = iActiveEntries->At( i );

        if( entry && entry->CacheEntry() == &aCacheEntry )
            {
            streamEntry = entry;
            if( aIndex )
                {
                *aIndex = i;
                }
            break;
            }
        }
    return streamEntry;
    }

//  End of File
