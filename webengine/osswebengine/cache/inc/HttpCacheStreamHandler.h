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
* Description:  Definition of CHttpCacheStreamHandler  
*
*/

#ifndef CHTTPCACHESTREAMHANDLER_H
#define CHTTPCACHESTREAMHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <s32file.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheEntry;
class RFileWriteStream;
class RFileReadStream;

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheStreamEntry : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheStreamEntry object.
        */
        static CHttpCacheStreamEntry* NewL( RFs& aRfs, CHttpCacheEntry& aHttpCacheEntry, TDriveUnit aDrive,
            TInt64 aCriticalLevel );

        /**
        * Destructor.
        */
        virtual ~CHttpCacheStreamEntry();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline CHttpCacheEntry* CacheEntry() const { return iHttpCacheEntry; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Erase();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* HeadersL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* NextChunkL( TBool& aLastChunk );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt SaveHeaders( const TDesC8& aHeaderStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveHeaders();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt SaveBodyData( const TDesC8& aBodyStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveBodyData();

        /**
        * Flush
        * @since 3.1
        * @param
        * @return
        */
        TInt Flush();

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheStreamEntry object.
        */
        CHttpCacheStreamEntry( RFs& aRfs, CHttpCacheEntry& aHttpCacheEntry, TDriveUnit aDrive,
            TInt64 aCriticalLevel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void GetHeaderFileName( const TFileName& aBodyFileName, TFileName& aHeaderFileName );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool OpenCacheFiles();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool CreateNewFilesL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void BodyFileName( TFileName& aBodyFileName );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool DiskSpaceBelowCriticalLevel( TInt aContentSize );

    private:    // Data

        //
        TBool               iFileOk;
        //
        RFs                 iRfs;       // not owned
        //
        RFile               iHeaderFile;      // owned
        //
        RFile               iBodyFile;      // owned
        //
        CHttpCacheEntry*    iHttpCacheEntry; // not owned
        //
        TDriveUnit          iDrive;
        //
        TInt64              iCriticalLevel;
        //
        HBufC8*             iCacheBuffer;   // owned
    };

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheStreamHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CHttpCacheStreamHandler object.
        */
        static CHttpCacheStreamHandler* NewL( const TDesC& aDirectory, TInt aCriticalLevel );

        /**
        * Destructor.
        */
        virtual ~CHttpCacheStreamHandler();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TInt SavedContentSize() const { return iContentSize; }

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool AttachL( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Detach( const CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void Erase( const CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* HeadersL( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* NextChunkL( CHttpCacheEntry& aCacheEntry, TBool& aLastChunk );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool SaveHeaders( CHttpCacheEntry& aCacheEntry, const TDesC8& aHeaderStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveHeaders( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool SaveBodyData( CHttpCacheEntry& aCacheEntry, const TDesC8& aBodyStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RemoveBodyData( CHttpCacheEntry& aCacheEntry );

        /**
        * Flush
        * @since 3.1
        * @param
        * @return
        */
        TBool Flush( CHttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline void SetStartupCacheSize( TInt aContentSize ) { iContentSize = aContentSize; }

#ifdef _DEBUG
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        inline TBool Find( const CHttpCacheEntry& aCacheEntry ) { return ( FindStreamEntry( aCacheEntry ) != NULL );  }
#endif // _DEBUG
    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        CHttpCacheStreamHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aDirectory, TInt aCriticalLevel );

    private:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        CHttpCacheStreamEntry* FindStreamEntry( const CHttpCacheEntry& aCacheEntry, TInt* aIndex = NULL );



    private:    // Data

        //
        RFs                                         iRfs;               // owned
        // attached entries
        CArrayPtrFlat<CHttpCacheStreamEntry>*       iActiveEntries;     // owned
        //
        TInt                                        iContentSize;
        //
        TDriveUnit          iDrive;
        //
        TInt64              iCriticalLevel;
        //
        TInt                iDiskFull;
    };

#endif      // CHTTPCACHESTREAMHANDLER_H

// End of File
