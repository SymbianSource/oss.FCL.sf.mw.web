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
* Description:  Implementation of CHttpCacheEntry
*
*/

// INCLUDE FILES
#include "HttpCacheEntry.h"
#include "HttpCacheEvictionHandler.h"
#include "HttpCacheStreamHandler.h"
#include "HttpCacheUtil.h"
#include <s32file.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt CHttpCacheEntry::iOffset = _FOFF( CHttpCacheEntry, iSlink );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheEntry::CHttpCacheEntry
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheEntry::CHttpCacheEntry(
    CHttpCacheEvictionHandler& aEvictionHandler )
    : iState( ECacheUninitialized ),
      iEvictionHandler( &aEvictionHandler )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::ConstructL(
    const TDesC8& aUrl )
    {
    iUrl = aUrl.AllocL();
    iFileName = KNullDesC().AllocL();
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheEntry::NewL(
    const TDesC8& aUrl,
    CHttpCacheEvictionHandler& aEvictionHandler )
    {
    CHttpCacheEntry* self = new( ELeave ) CHttpCacheEntry( aEvictionHandler );

    CleanupStack::PushL( self );
    self->ConstructL( aUrl );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheEntry* CHttpCacheEntry::NewLC(
    const TDesC8& aUrl,
    CHttpCacheEvictionHandler& aEvictionHandler )
    {
    CHttpCacheEntry* self = CHttpCacheEntry::NewL( aUrl, aEvictionHandler );
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CHttpCacheEntry::~CHttpCacheEntry()
    {
  if( iVictim && iEvictionHandler)
    {
    iEvictionHandler->Remove( *this );
    }
    delete iUrl;
    delete iFileName;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetState
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetState(
    TCacheEntryState aState )
    {
    // add entry to the eviction table once it gets completed
    if( aState == ECacheComplete && !iVictim )
        {
        // don't add it twice
        iEvictionHandler->Insert( *this );
        iVictim = ETrue;
        }
    iState = aState;
    }


// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetFileNameL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetFileNameL(
    const TFileName& aFileName )
    {
    delete iFileName;
    iFileName = NULL;
    iFileName = aFileName.AllocL();
    }


// -----------------------------------------------------------------------------
// CHttpCacheEntry::Accessed
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::Accessed()
    {
    TTime now;
    now.HomeTime();

    iLastAccessed = now.Int64();

    iRef++;
    if( iVictim )
        {
        iEvictionHandler->Accessed( *this );
        }
#ifdef __CACHELOG__
    _LIT( KAccessFormat, "entry accessed: %d" );
    TBuf<100> buf;
    buf.Format( KAccessFormat, iRef );
    HttpCacheUtil::WriteUrlToLog( 0, buf, iUrl->Des() );
#endif // __CACHELOG__
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetSize
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetSize(
    TUint aSize )
    {
    if( iSize && !aSize )
        {
        // body removal
        // remove itself from the eviction table.
        // this is no longer a victim
      if( iVictim )
        {
        iEvictionHandler->Remove( *this );
            iVictim = EFalse;
        }
        }
    iSize = aSize;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::SetProtected
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::SetProtected()
    {
    iProtected = ETrue;
    // iRef
    iRef = 50;
    HttpCacheUtil::WriteLog( 0, _L( "protected item" ) );
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Internalize
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEntry::Internalize(
    RFileReadStream& aReadStream )
    {
    TRAPD( err,
     TInt len;
    
    // url length
    len = aReadStream.ReadInt32L();
    delete iUrl;
    iUrl=NULL;
    iUrl = HBufC8::NewL( len );
    TPtr8 ptr8( iUrl->Des() );
    // url
    aReadStream.ReadL( ptr8, len );
    
    // filename length
    len = aReadStream.ReadInt32L();
    HBufC* filename = HBufC::NewLC( len );
    TPtr ptr( filename->Des() );
    // url
    aReadStream.ReadL( ptr, len );
    //
    SetFileNameL( filename->Des() );
    //
    CleanupStack::PopAndDestroy(); // filename
    // la
    TReal64 la;
    la = aReadStream.ReadReal64L();
    iLastAccessed = la;
    // ref
    iRef = aReadStream.ReadUint32L();
    // size
    iSize = aReadStream.ReadUint32L( );
    // size
    iHeaderSize = aReadStream.ReadUint32L( );
    // protected
    iProtected = aReadStream.ReadInt32L();
    //
    SetState( ECacheComplete ); );
    return err;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Externalize
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheEntry::Externalize(
    RFileWriteStream& aWriteStream )
    {
    TRAPD( err,
    // url length
    aWriteStream.WriteInt32L( iUrl->Length() );
    // url
    aWriteStream.WriteL( iUrl->Des() );
    // filename length
    aWriteStream.WriteInt32L( iFileName->Length() );
    // filename
    aWriteStream.WriteL( iFileName->Des() );
    // la
    aWriteStream.WriteReal64L( iLastAccessed );
    // ref
    aWriteStream.WriteUint32L( iRef );
    // size
    aWriteStream.WriteUint32L( iSize );
    // size
    aWriteStream.WriteUint32L( iHeaderSize );
    // protected
    aWriteStream.WriteInt32L( iProtected ); );
    return err;
    }

// -----------------------------------------------------------------------------
// CHttpCacheEntry::Accessed
//
// -----------------------------------------------------------------------------
//
void CHttpCacheEntry::Accessed(TInt64 aLastAccessed, TUint16 aRef)
    {
    iLastAccessed = aLastAccessed;
    iRef = aRef;
    if( iVictim )
        {
        iEvictionHandler->Accessed( *this );
        }
#ifdef __CACHELOG__
    _LIT( KAccessFormat, "entry accessed: %d" );
    TBuf<100> buf;
    buf.Format( KAccessFormat, iRef );
    HttpCacheUtil::WriteUrlToLog( 0, buf, iUrl->Des() );
#endif // __CACHELOG__

    }

//  End of File
