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
* Description:  Definition of CHttpCacheManager
*
*/

#ifndef CHTTPCACHEMANAGER_H
#define CHTTPCACHEMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <CenRepNotifyHandler.h>
#include <BrCtlDefs.h>
#include <http/rhttpsession.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheEntry;
class CHttpCacheHandler;
class CCenRepNotifyHandler;
class CRepository;
class RHTTPTransaction;
class MHTTPDataSupplier;

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
class THttpCacheEntry
    {
    public:

        //
        CHttpCacheHandler*  iCacheHandler;

        //
        CHttpCacheEntry*    iCacheEntry;
    };

/**
*
*  @lib
*  @since 3.1
*/
class CHttpCacheManager : public CBase, public MCenRepNotifyHandlerCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        IMPORT_C static CHttpCacheManager* NewL();

        /**
        * Destructor.
        */
        virtual ~CHttpCacheManager();

    public: // new functions

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TInt RequestL( RHTTPTransaction& aTrans, TBrCtlDefs::TBrCtlCacheMode aCacheMode,
            THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TInt RequestHeadersL( RHTTPTransaction& aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C HBufC8* RequestNextChunkL( RHTTPTransaction& aTrans, TBool& aLastChunk, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C void ReceivedResponseHeadersL( RHTTPTransaction& aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
    IMPORT_C void ReceivedResponseBodyDataL( RHTTPTransaction& aTrans, MHTTPDataSupplier& aBodyDataSupplier, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
    IMPORT_C void ResponseComplete( RHTTPTransaction& aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
    IMPORT_C void RequestClosed( RHTTPTransaction* aTrans, THttpCacheEntry& aCacheEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TInt RemoveAllL();

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TInt RemoveL( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TBool Find( const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TBool SaveL( const TDesC8& aUrl, const TDesC8& aHeader,
                     const TDesC8& aContent, const TDesC8&  aHttpHeaderString );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        IMPORT_C TInt AddHeaderL( const TDesC8& aUrl, const TDesC8& aName, const TDesC8& aValue );

    public: // from MCenRepNotifyHandlerCallback

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void HandleNotifyString( const TUint32 aKeyId, const TDesC& aValue );

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        CHttpCacheManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        void CreateCacheHandlersL();

         /**
        *
        * @since 7.1
        * @param
        * @return
        */
        void RemoveOrphanedFilesL(); 
        
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        CHttpCacheHandler* CacheHandler( const TDesC8& aUrl,  TDes8* aHttpHeaderString ) const;


    /**
        *
        * @since 3.1
        * @param
        * @return
        */
        TBool VSSRequestCheck( const RHTTPTransaction& aTrans,
                               const RHTTPHeaders& aHttpHeader, const TDesC8& aUrl );
    /**
        *
        * @since 3.1
        * @param
        * @return
        */

        TBool VSSHeaderCheck(  TDes8*  aHttpHeaderString ) const ;

    private:    // Data

        CHttpCacheHandler*                         iCache;      // owned
        
        TPath	                                   iCacheFolder;
        
        CHttpCacheHandler*                         iOperatorCache;      // owned
        //
        CHttpCacheHandler*                         iphoneSpecificCache;      // owned
        //


        HBufC8*                                    iOpDomain;       // owned
        //
        CCenRepNotifyHandler*                      iOfflineNotifyHandler; // owned
        //
        CRepository*                               iOfflineRepository; // owned
        //
        TBool                                      iOfflineMode;
        //
        TBool                                      iCacheEnabled;
        //
        HBufC8*                                    iVSSWhiteList;       // owned
        //
        TBool                                      iVSSCacheEnabled;
        TBool                                      iVSSTransaction;
    };

#endif      // CHttpCacheManager_H

// End of File