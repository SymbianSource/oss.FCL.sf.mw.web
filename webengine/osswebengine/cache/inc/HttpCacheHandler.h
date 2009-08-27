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
* Description:  Definition of CHttpCacheHandler
*
*/

#ifndef CHTTPCACHEHANDLER_H
#define CHTTPCACHEHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <BrCtlDefs.h>
#include <http/rhttpsession.h>
#include <stringpool.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheEntry;
class CHttpCacheLookupTable;
class CHttpCacheStreamHandler;
class RHTTPTransaction;
class RHTTPHeaders;
class RStringPool;
class MHTTPDataSupplier;
class CIdle;
class CHttpCacheEvictionHandler;
class THttpCacheEntry;
class CHttpCacheObserver;
class CHttpCacheFileWriteHandler;

// CLASS DECLARATION

NONSHARABLE_CLASS( THttpCachePostponeParameters )
    {
    public:
        THttpCachePostponeParameters();

        TBool   iEnabled;
        TInt    iFreeRamThreshold;
        TInt    iImmediateWriteThreshold;
        TInt    iWriteTimeout;
    };

NONSHARABLE_CLASS( TCompressedEntry )
    {
    public:
        ~TCompressedEntry() { if(!IsCompressed()) delete iName.iNameAsHBuf; };
        static TCompressedEntry *NewL( const TEntry& aEntry );

    private:
        TCompressedEntry(){};
        void ConstructL( const TEntry& aEntry );
        enum {
            EFilenameStoredAsUint32 = 0x01
        };

    public:
        TBool IsCompressed() { return (iFlags && TCompressedEntry::EFilenameStoredAsUint32); };
        static TBool ConvertANameToUint32( const TDesC& aName, TUint32& aConverted);
        static TInt ConvertAsciiToIntSingleHexDigit(const TUint16& aDigitChar);
        TUint32 GetSize() { return iSize; };
        HBufC* GetNameL();
        TUint32 GetCompressedName() { return iName.iNameAsUint32; };

    private:
        TUint32 iFlags;
        TUint32 iSize;
        union {
            TUint32 iNameAsUint32;
            HBufC*  iNameAsHBuf;
        }iName;
    };

NONSHARABLE_CLASS( CCustomCacheDirList ) : public CBase
    {
    public:
        static CCustomCacheDirList* NewL(CDir *aSrc);
        ~CCustomCacheDirList() { iDirList.ResetAndDestroy(); };

    public:
        TBool ValidateCacheEntryL( const CHttpCacheEntry& aEntry );
        TInt Count();
        HBufC* NameAtL( TInt aIndex );

    private:
        CCustomCacheDirList();
        void ConstructL(CDir *aSrc);

    private:
        RPointerArray<TCompressedEntry> iDirList;
    };

NONSHARABLE_CLASS( CCacheDirectoryFiles ) : public CBase
    {
    public:
        static CCacheDirectoryFiles* NewL(RFs aRfs, const TDesC& aDir);
        static CCacheDirectoryFiles* NewLC(RFs aRfs, const TDesC& aDir);

    public:
        /*
         * Check to see if the entry passed in has a valid file in the directory listing
         * returns ETrue if file present and size matches entry
         * otherwise EFalse.
         * Removes matching files from stored list.
         */
        TBool ValidateEntryL(const CHttpCacheEntry& aEntry);

        /*
         * Delete any files in the stored list from the filesystem
         * Intended to be called after you have validated all the entries
         * against the list - the content of the list will now only be the
         * orphan files in the cache subdirectories.
         * IMPORTANT: only call this once you've looked at all your entries.
         */
        void RemoveLeftoverFilesL();

    private:
        CCacheDirectoryFiles(RFs aRfs, const TDesC& aDir) : iRfs(aRfs), iDir(aDir) {};
        ~CCacheDirectoryFiles();
        void ConstructL();

    private:
        RFs iRfs;
        const TDesC& iDir;

        RPointerArray<CCustomCacheDirList> iDirContent;
    };

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheHandler : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        static CHttpCacheHandler* NewL( TInt aSize,
            const TDesC& aDirectory,
            const TDesC& aIndexFile,
            TInt aCriticalLevel,
            const THttpCachePostponeParameters& aPostpone);

        /**
        * Destructor.
        */
        virtual ~CHttpCacheHandler();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt RequestL( RHTTPTransaction& aTrans, TBrCtlDefs::TBrCtlCacheMode aCacheMode, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt RequestHeadersL( RHTTPTransaction& aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        HBufC8* RequestNextChunkL( RHTTPTransaction& aTrans, TBool& aLastChunk, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ReceivedResponseHeadersL( RHTTPTransaction& aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ReceivedResponseBodyDataL( RHTTPTransaction& aTrans, MHTTPDataSupplier& aBodyDataSupplier, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void ResponseComplete( RHTTPTransaction& aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void RequestClosed( RHTTPTransaction* aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt RemoveAllL();

        /**
        *
        * @since 7.1
        * @param
        * @return
        */
        TInt ListFiles(RPointerArray<TDesC>& aFilenameList);

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt RemoveL( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool Find( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool SaveL( const TDesC8& aUrl, const TDesC8& aHeader, const TDesC8& aContent );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TInt AddHeaderL( const TDesC8& aUrl, const TDesC8& aName, const TDesC8& aValue );

        /**
        * Some time response time form the origin server is not matching
        * with GMt time. Adjust the response time.
        *
        * @since 3.1
        * @param
        * @return
        */
        void AdjustResponseTime(RHTTPTransaction& aTrans);

        /**
        * Some other application modified the cache and the lookup table needs to be updated
        *
        * @since 3.1
        * @param
        * @return
        */
        void UpdateLookupTable();

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        void SaveLookupTableL();

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        CHttpCacheHandler( TInt aSize );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aDirectory, const TDesC& aIndexFile,  TInt aCriticalLevel, const THttpCachePostponeParameters& aPostpone);

    private: //

        /**
        * Some other application modified the cache and the lookup table needs to be updated
        *
        * @since 3.1
        * @param
        * @return
        */
        void UpdateLookupTableL();

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        TBool CacheNeedsValidationL( CHttpCacheEntry& aCacheEntry, RHTTPTransaction& aTrans, TBrCtlDefs::TBrCtlCacheMode aCacheMode );

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        TBool CacheNeedsSpaceL( TInt aSize );

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        TBool IsCacheable( RHTTPTransaction& aTrans, TBool& aProtectedEntry );

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        TBool HandleResponseOkL( CHttpCacheEntry& aEntry, RHTTPTransaction& aTrans );

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        TBool HandleResponseNotModifiedL( CHttpCacheEntry& aEntry, RHTTPTransaction& aTrans );

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        void OpenLookupTableL();

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        void OpenLookupTableL(CHttpCacheLookupTable* aLookupTable);

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        void DeleteCacheEntry( CHttpCacheEntry& aEntry, TBool aUpdate = ETrue );


        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        TBool SaveBuffer( CHttpCacheEntry& aEntry, const TDesC8& aBuffer, TBool aBody = EFalse );

        /**
         *
         * @since 7.1
         * @param
         * @return
         */
        void GenerateValidationFilename(TDes& aFilename, const TDesC& aIndexFilename) const;

        /**
         *
         * @since 7.1
         * @param
         * @return
         */
        void ValidateCacheEntriesL();

    private:    // Data

        // hash table for cache entries
        CHttpCacheLookupTable*          iLookupTable;   // owned
        // stream manager
        CHttpCacheStreamHandler*        iStreamHandler; // owned
        //
        CHttpCacheEvictionHandler*      iEvictionHandler; // owned
        // cache size
        TInt                            iSize;
        // cache directory
        HBufC*                          iDirectory; // owned
        // name of the index file
        HBufC*                          iIndexFile; // owned
        // Observing changes in cache
        CHttpCacheObserver* iHttpCacheObserver; // owned
        // An opened and configured file server session
        RFs iRfs;
        //
        CHttpCacheFileWriteHandler*     iPostponeHandler;   // owned
    };

#endif      // CHTTPCACHEHANDLER_H

// End of File
