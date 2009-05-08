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
* Description:  Definition of CHttpCacheEntry
*
*/

#ifndef CHTTPCACHEENTRY_H
#define CHTTPCACHEENTRY_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// CONSTANTS
const TInt KBufferSize32k = 32768;
const TInt KBufferSizeZero = 0;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RHTTPTransaction;
class MHTTPDataSupplier;
class RFileWriteStream;
class RFileReadStream;
class CHttpCacheEvictionHandler;
class CHttpCacheStreamHandler;

// CLASS DECLARATION

/**
*  
*  @lib 
*  @since 3.1
*/
class CHttpCacheEntry : public CBase
    {
    public:  // Constructors and destructor        
        
        enum TCacheEntryState
            {
            ECacheUninitialized,
            ECacheInitialized,
            ECacheRequesting,
            ECacheResponding,
            ECacheComplete,
            ECacheDestroyed
            };

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param 
        * @param 
        * @return CHttpCacheEntry object.
        */
        static CHttpCacheEntry* NewL( const TDesC8& aUrl, 
            CHttpCacheEvictionHandler& aEvictionHandler );

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param 
        * @param 
        * @return CHttpCacheEntry object.
        */
        static CHttpCacheEntry* NewLC( const TDesC8& aUrl, 
            CHttpCacheEvictionHandler& aEvictionHandler );
        
        /**
        * Destructor.
        */
        virtual ~CHttpCacheEntry();
        
    public: // new functions
        
        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void SetState( TCacheEntryState aState ); 

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline TCacheEntryState State() { return iState; }

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void SetFileNameL( const TFileName& aFileName );

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        const TDesC& Filename() const { return *iFileName; }
        
        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline const TDesC8& Url() const { return *iUrl; }
        
        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline TInt64 LastAccessed() const { return iLastAccessed; }

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void Accessed();
        
        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline TUint16 Ref() const { return iRef; }

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline TUint BodySize() const { return iBodySize; }

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void SetBodySize( TUint aSize );

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline TUint16 HeaderSize() const { return iHeaderSize; }

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline void SetHeaderSize( TUint16 aHeaderSize ) { iHeaderSize = aHeaderSize; }

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        inline RFile& HeaderFile() { return iHeaderFile; }

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        inline RFile& BodyFile() { return iBodyFile; }

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        TPtr8 CacheBuffer();
        
        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        void SetCacheBufferL( TInt aCacheBufferSize );

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        inline TBool Protected() const { return iProtected; }

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void SetProtected();

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        TInt Internalize( RFileReadStream& aReadStream );

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        TInt Externalize( RFileWriteStream& aWriteStream );

        /**
        * 
        * @since 3.1
        * @param 
        * @return 
        */
        void Accessed(TInt64 aLastAccessed, TUint16 aRef);

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        inline TBool BodyFileDeleteNeeded() { return iBodyFileDeleteNeeded; }

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        inline void SetBodyFileDeleteNeeded( TBool aBodyFileDeleteNeeded )
                    {
                    iBodyFileDeleteNeeded = aBodyFileDeleteNeeded;
                    }

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        TBool CacheFilesOpened() { return iCacheFilesOpened; }

        /**
        * 
        * @since 7.1
        * @param 
        * @return 
        */
        void SetCacheFilesOpened( TBool aCacheFilesOpened );

    public : 

        // support linked list
        static const TInt iOffset;

    private:
        
        /**
        * Construct.
        * @since 3.1
        * @param 
        * @param 
        * @return CHttpCacheEntry object.
        */
        CHttpCacheEntry( CHttpCacheEvictionHandler& aEvictionHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aUrl );    

    private:    // Data

        //
        HBufC8*                         iUrl;               // owned
        // if empty, then use url to get filename
        HBufC*                          iFileName;          // owned
        //
        TCacheEntryState                iState;
        //
        TInt64                          iLastAccessed;
        // ref counter
        TUint16                         iRef;
        // content size
        TUint                           iBodySize;
        // header size
        TUint16                         iHeaderSize;
        // protected content like css, script
        TUint8                          iProtected;
        //
        TSglQueLink                     iSqlQueLink;
        //
        CHttpCacheEvictionHandler*      iEvictionHandler;   // not owned
        // Etrue if added to the eviction table
        TUint8                          iEvictionCandidate;
        //
        TBool                           iBodyFileDeleteNeeded;
        //
        RFile                           iHeaderFile;        // owned
        //
        RFile                           iBodyFile;          // owned
        //
        HBufC8*                         iCacheBuffer;       // owned
        // ETrue if files open (and attached to StreamHandler) for read/write
        TBool                           iCacheFilesOpened;
    };

#endif      // CHTTPCACHEENTRY_H
            
// End of File
