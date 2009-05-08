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
// CHttpCacheStreamHandler::CHttpCacheStreamHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler::CHttpCacheStreamHandler()
    {
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
    iActiveEntries = new( ELeave )CArrayPtrFlat<CHttpCacheEntry>( KHttpCacheActiveCount );
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

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHttpCacheStreamHandler::~CHttpCacheStreamHandler()
    {
    if ( iActiveEntries )
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
TBool CHttpCacheStreamHandler::AttachL( CHttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
    // check for duplicates
    for ( TInt i = 0; i < iActiveEntries->Count(); i++ )
        {
        __ASSERT_DEBUG( iActiveEntries->At( i ) != &aCacheEntry,
        User::Panic( _L("cacheStreamHandler Panic"), KErrCorrupt )  );
        }
#endif // __CACHELOG__
   
    TBool cacheFilesOpened( EFalse );

    if ( aCacheEntry.CacheFilesOpened() )
        {
        // Cache files already opened, no need to reopen
        cacheFilesOpened = ETrue;
        }
    else if ( aCacheEntry.State() == CHttpCacheEntry::ECacheUninitialized )
        {
        // Create new cache files, they don't already exist
        cacheFilesOpened = CreateNewFilesL( aCacheEntry );
        }
    else
        {
        // Open existing cache files
        cacheFilesOpened = OpenCacheFiles( aCacheEntry );
        }
    
    if ( cacheFilesOpened )
        {
        // Add to our active array, if not already there
        TInt index( -1 );
        FindCacheEntryIndex( aCacheEntry, &index );
        if ( index == -1 )
            {
            iActiveEntries->AppendL( &aCacheEntry );
            }
        }

    aCacheEntry.SetCacheFilesOpened( cacheFilesOpened );

    // Return ETrue, if files opened and attached
    return cacheFilesOpened;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Detach
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::Detach( CHttpCacheEntry& aCacheEntry )
    {
    // Close the files, this will commit changes
    if ( aCacheEntry.CacheFilesOpened() )
        {
        aCacheEntry.BodyFile().Close();
        aCacheEntry.HeaderFile().Close();
        aCacheEntry.SetCacheFilesOpened( EFalse );
        }

    // Delete from our active array
    TInt index( -1 );
    FindCacheEntryIndex( (const CHttpCacheEntry&)aCacheEntry, &index );
    if ( index >= 0 )
        {
        iActiveEntries->Delete( index );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::EraseCacheFile
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::EraseCacheFile( CHttpCacheEntry& aCacheEntry )
    {
    HttpCacheUtil::WriteUrlToLog( 0, _L( "CHttpCacheStreamHandler::Erase - erase files associated with" ), aCacheEntry.Url() );

    aCacheEntry.HeaderFile().Close();
    aCacheEntry.BodyFile().Close();
    aCacheEntry.SetCacheFilesOpened( EFalse );

    // Get body filename
    TFileName bodyFileName = aCacheEntry.Filename();

    // Get header filename
    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( bodyFileName, headerFileName );

    TInt statusBody( KErrNotFound );
    statusBody = iRfs.Delete( bodyFileName );

    TInt statusHeader( KErrNotFound );
    statusHeader = iRfs.Delete( headerFileName );

    // Adjust the size
    iContentSize -= aCacheEntry.BodySize();
    iContentSize -= aCacheEntry.HeaderSize();

#ifdef __CACHELOG__
    if ( statusBody != KErrNone ) {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CCHttpCacheStreamEntry::Erase - ERROR bodyFile delete"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           statusBody,
                                           ELogFileErrorCode );
        }
    else {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CCHttpCacheStreamEntry::Erase - SUCCESS bodyFile delete"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
        }
    if ( statusHeader != KErrNone ) {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CCHttpCacheStreamEntry::Erase - ERROR headerFile delete"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           statusHeader,
                                           ELogFileErrorCode );
        }
    else {
        HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CCHttpCacheStreamEntry::Erase - SUCCESS headerFile delete"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
        }
#endif //__CACHELOG__

    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::HeadersL
//
// -----------------------------------------------------------------------------
//
HBufC8* CHttpCacheStreamHandler::HeadersL( CHttpCacheEntry& aCacheEntry )
    {
    HBufC8* headerStr = NULL;
    TInt headerLen( 0 );
    TInt err( KErrNone );

    if ( !aCacheEntry.CacheFilesOpened() )
        {
        OpenCacheFiles( aCacheEntry );
        }

    err = aCacheEntry.HeaderFile().Size( headerLen );
    if ( err == KErrNone && headerLen > 0 )
        {
        headerStr = HBufC8::NewL( headerLen );
        TPtr8 ptr( headerStr->Des() );
        // read headers
        aCacheEntry.HeaderFile().Read( 0, ptr, headerLen );
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
    HBufC8* bodyStr = NULL;
    // Read whole body
    TInt size( 0 );
    TInt sizeErr( KErrNone );

    if ( !aCacheEntry.CacheFilesOpened() )
        {
        OpenCacheFiles( aCacheEntry );
        }

    sizeErr = aCacheEntry.BodyFile().Size( size );
    if ( sizeErr == KErrNone && size > 0 )
        {
        bodyStr = HBufC8::NewL( size );
        TPtr8 ptr( bodyStr->Des() );

        TInt readErr( KErrNone );
        readErr = aCacheEntry.BodyFile().Read( ptr, size );

#ifdef __CACHELOG__
        if ( readErr != KErrNone ) {
            HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                               _L("CCHttpCacheStreamEntry::NextChunkL - bodyFile.read"),
                                               aCacheEntry.Filename(),
                                               aCacheEntry.Url(),
                                               readErr,
                                               ELogFileErrorCode );
            }
        else {
            HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                               _L("CCHttpCacheStreamEntry::NextChunkL - bodyFile.read"),
                                               aCacheEntry.Filename(),
                                               aCacheEntry.Url(),
                                               ptr.Length(),
                                               ELogEntrySize );
            }
#endif  // __CACHELOG__

        }
    aLastChunk = ETrue;

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
    TBool headerSaved( EFalse );

    if ( !aCacheEntry.CacheFilesOpened() )
        {
        OpenCacheFiles( aCacheEntry );
        }

    if ( aHeaderStr.Length() && IsDiskSpaceAvailable( aHeaderStr.Length() ) )
        {
        // We have space on disk, save headers. Don't force a flush, as the
        // File Server takes care of write and read consistency.
        TInt writeErr = aCacheEntry.HeaderFile().Write( aHeaderStr );

        if ( writeErr == KErrNone )
            {
            aCacheEntry.SetHeaderSize( aHeaderStr.Length() );
            iContentSize += aHeaderStr.Length();
            headerSaved = ETrue;
            }
        }

    return headerSaved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::RemoveHeaders
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::RemoveHeaders( CHttpCacheEntry& aCacheEntry )
    {
    iContentSize -= aCacheEntry.HeaderSize();

    // Destroy data
    aCacheEntry.HeaderFile().SetSize( 0 );
    aCacheEntry.SetHeaderSize( 0 );
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
    TInt bodySaved( EFalse );
    TInt newBodyLength( aBodyStr.Length() );
    TPtr8 buffer( aCacheEntry.CacheBuffer() );

    if ( newBodyLength && buffer.MaxLength() )
        {
        // Calculate if we have enough space in the buffer for incoming body
        if ( buffer.Length() + newBodyLength > buffer.MaxLength() )
            {
            // Incoming data is too big for the buffer
            HBufC8* overflowBuffer = NULL;
            TInt bufferSpaceLeft( -1 );
            TPtrC8 writePtr;

            if ( buffer.Length() == 0 )
                {
                // Buffer is empty and the body is bigger than the buffer,
                // just take all of the incoming data
                writePtr.Set( aBodyStr );
                }
            else
                {
                // We have some data in buffer, how much space do we have left
                bufferSpaceLeft = buffer.MaxLength() - buffer.Length();
                
                if ( newBodyLength - bufferSpaceLeft > buffer.MaxLength() )
                    {
                    // Not enough space, so lets put the buffer and the new
                    // body together and write it in one go.
                    overflowBuffer = HBufC8::New( buffer.Length() + newBodyLength );
                    if ( !overflowBuffer )
                        {
                        return EFalse;
                        }

                    TPtr8 overflowPtr( overflowBuffer->Des() );
                    overflowPtr.Copy( buffer );
                    overflowPtr.Append( aBodyStr );
                    writePtr.Set( overflowBuffer->Des() );

                    // empty buffer
                    buffer.Zero();
                    // no leftover left
                    bufferSpaceLeft = -1;
                    }
                else
                    {
                    // Copy what we have enough space for
                    buffer.Append( aBodyStr.Left( bufferSpaceLeft ) );
                    writePtr.Set( buffer );
                    }
                }

            // Write to the disk, if we have disk space
            TInt writeErr( KErrNone );
            if ( IsDiskSpaceAvailable( writePtr.Length() ) )
                {

                if ( !aCacheEntry.CacheFilesOpened() )
                    {
                    OpenCacheFiles( aCacheEntry );
                    }

                // We have enough disk space, save body
                TInt writeErr = aCacheEntry.BodyFile().Write( writePtr );
                bodySaved = ETrue;
                }
            else
                {
                // We don't have enough disk space, clean up 
                bodySaved = EFalse;
                buffer.Zero();
                }

            if ( writeErr == KErrNone && bufferSpaceLeft >= 0 )
                {
                // Copy what we can of the leftover in to the buffer
                buffer.Copy( aBodyStr.Mid( bufferSpaceLeft ) );
                }
            delete overflowBuffer;
            }
        else
            {
            // We have enough space in buffer, add and wait for next body
            // before writing to file
            buffer.Append( aBodyStr );
            bodySaved = ETrue;
            }

        // Body saved, update state
        aCacheEntry.SetBodySize( aCacheEntry.BodySize() + newBodyLength );
        iContentSize += aBodyStr.Length();
        }

    return bodySaved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::RemoveBodyData
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::RemoveBodyData( CHttpCacheEntry& aCacheEntry )
    {
#ifdef __CACHELOG__
     HttpCacheUtil::WriteLogFilenameAndUrl( 0,
                                           _L("CHttpCacheStreamHandler::RemoveBodyData - CLEAR iCacheBuffer, entrySize, and iBodyFile"),
                                           aCacheEntry.Filename(),
                                           aCacheEntry.Url(),
                                           aCacheEntry.BodySize(),
                                           ELogEntrySize );
#endif

    // Remove data 
    iContentSize -= aCacheEntry.BodySize();
    aCacheEntry.SetBodySize( 0 );
    aCacheEntry.CacheBuffer().Zero();
    aCacheEntry.BodyFile().SetSize( 0 );
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::Flush
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::Flush( CHttpCacheEntry& aCacheEntry )
    {
    TBool saved( EFalse );
    TInt writeErr( KErrGeneral );
    TInt cacheBufferLen( aCacheEntry.CacheBuffer().Length() );

    if ( cacheBufferLen && IsDiskSpaceAvailable( cacheBufferLen ) )
        {
        // We have enough space, save cache buffer
        TPtr8 bufferPtr( aCacheEntry.CacheBuffer() );
        if ( bufferPtr.Length() )
            {
            if ( !aCacheEntry.CacheFilesOpened() )
                {
                OpenCacheFiles( aCacheEntry );
                }

            writeErr = aCacheEntry.BodyFile().Write( bufferPtr );
            if ( writeErr == KErrNone )
                {
                saved = ETrue;
                }
            }

        // Clear the buffer
        bufferPtr.Zero();
        }

    return saved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::OpenCacheFiles
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::OpenCacheFiles( CHttpCacheEntry& aCacheEntry )
    {
    TInt statusHeader( KErrNotFound );
    TInt statusBody( KErrNotFound );

    // get body filename
    TFileName bodyFileName = aCacheEntry.Filename();

    // header filename
    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( bodyFileName, headerFileName );

    statusHeader = aCacheEntry.HeaderFile().Open( iRfs, headerFileName, EFileShareExclusive | EFileWrite );
    statusBody = aCacheEntry.BodyFile().Open( iRfs, bodyFileName, EFileShareExclusive | EFileWrite );

    TBool fileOk( statusHeader == KErrNone && statusBody == KErrNone );
    if ( fileOk )
        {
        aCacheEntry.SetCacheFilesOpened( ETrue );
        }

    return fileOk;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::CreateNewFilesL
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::CreateNewFilesL( CHttpCacheEntry& aCacheEntry )
    {
    TInt statusHeader( KErrNotFound );
    TInt statusBody( KErrNotFound );
    TPath sessionPath;
    User::LeaveIfError( iRfs.SessionPath( sessionPath ) );
    
    // Given the full URL, generates a fully qualified path for saving the HTTP response
    HBufC* bodyFileName = HttpCacheUtil::GenerateNameLC( aCacheEntry.Url(), sessionPath );
    TPtrC bodyFileNamePtr( *bodyFileName );
 
    // Create header file name from body file name
    TFileName headerFileName;
    HttpCacheUtil::GetHeaderFileName( bodyFileNamePtr, headerFileName );

    // Create the body file or replace it, if it exists. 
    statusBody = aCacheEntry.BodyFile().Replace( iRfs, bodyFileNamePtr, EFileShareExclusive | EFileWrite );
    if ( statusBody == KErrNone )
        {
        // Header file should not fail
        statusHeader = aCacheEntry.HeaderFile().Replace( iRfs, headerFileName, EFileShareExclusive | EFileWrite );
        }

#ifdef __CACHELOG__ 
    HttpCacheUtil::WriteUrlToLog( 0, bodyFileNamePtr, aCacheEntry.Url() );
#endif 

    TBool fileOk( statusHeader == KErrNone && statusBody == KErrNone );
    if ( fileOk )
        {
        // Both body and header files created correctly 
        aCacheEntry.SetFileNameL( bodyFileNamePtr );
        aCacheEntry.SetState( CHttpCacheEntry::ECacheInitialized );
        }
    else
        {
        // Only the body file created, no header file, delete body file 
        iRfs.Delete( bodyFileNamePtr );
        iRfs.Delete( headerFileName );

        aCacheEntry.SetBodySize( 0 );
        aCacheEntry.SetHeaderSize( 0 );

#ifdef __CACHELOG__
        HttpCacheUtil::WriteLog( 0, _L( "CHttpCacheEntry::CreateNewFilesL - DELETE body file, header file failed" ) );
#endif
//        __ASSERT_DEBUG( EFalse, User::Panic( _L("CHttpCacheHandler::CreateNewFilesL Panic"), KErrCorrupt )  );
        }

    CleanupStack::PopAndDestroy( bodyFileName );

    return fileOk;
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::FindCacheEntryIndex
//
// -----------------------------------------------------------------------------
//
void CHttpCacheStreamHandler::FindCacheEntryIndex(
    const CHttpCacheEntry& aCacheEntry,
    TInt* aIndex )
    {
    *aIndex = -1;
    for ( TInt i = 0; i < iActiveEntries->Count(); i++ )
        {
        CHttpCacheEntry* entry = iActiveEntries->At( i );

        if ( entry == &aCacheEntry )
            {
            if ( aIndex )
                {
                *aIndex = i;
                }
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheStreamHandler::IsDiskSpaceAvailable
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheStreamHandler::IsDiskSpaceAvailable( TInt aContentSize )
    {
    TBool diskSpaceAvailable( EFalse );
    TVolumeInfo vinfo;
    TInt errorCode = iRfs.Volume( vinfo, iDrive );

    if ( errorCode == KErrNone && ( vinfo.iFree - aContentSize ) > iCriticalLevel )
        {
        // We have space on the disk for the content
        diskSpaceAvailable = ETrue;
        }

    return diskSpaceAvailable;
    }

//  End of File
