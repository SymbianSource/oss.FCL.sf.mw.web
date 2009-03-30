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
* Description:  Definition of CHttpCacheUtil
*
*/

#ifndef CHTTPCACHEUTIL_H
#define CHTTPCACHEUTIL_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <http/rhttpheaders.h>

// CONSTANTS
enum TCacheLoadMethod
    {
    EMethodGet,
    EMethodConnect,
    EMethodDelete,
    EMethodHead,
    EMethodOptions,
    EMethodPut,
    EMethodPost,
    EMethodTrace,
    };

const TInt KCacheVersionNumber = 1;
//make 16 subdirectories named 0 through F for organizing cached files
const TUint32 KCacheSubdirCount = 16;
_LIT( KHttpCacheHeaderExt, ".h" );

//#define __CACHELOG__

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RHTTPHeaders;
class RHTTPTransaction;
class MHTTPDataSupplier;
class RStringPool;
class CHttpCacheEntry;

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
class HttpCacheUtil
    {
    public:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void SetEntryOnTransL( CHttpCacheEntry& aEntry, RHTTPTransaction& aTrans, RStringF& aCacheEntryStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static CHttpCacheEntry* EntryOnTransL( RHTTPTransaction& aTrans, RStringF& aCacheEntryStr );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static HBufC8* HeadersToBufferLC( RHTTPHeaders& aHttpHeaders, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void BufferToHeadersL( const TDesC8& aHeaderStr, RHTTPHeaders& aHttpHeaders, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static HBufC8* BodyToBufferL( MHTTPDataSupplier& aBodySupplier );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static HBufC8* MergeHeadersLC( const TDesC8& aCachedHeaderStr, RHTTPHeaders& aResponseHeaders, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static HBufC8* AddHeaderLC( const TDesC8& aName, const TDesC8& aValue, const TDesC8& aHeaders );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static HBufC8* HeaderFieldFromBufferLC( const TDesC8& aHeaders, const TDesC8& aKey );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TCacheLoadMethod MethodFromStr( RStringF aMethodStr, RStringPool aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool ProtectedEntry( RHTTPHeaders& aHttpHeaders, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool CacheTimeIsFresh( const RHTTPHeaders& aRequestHeaders, const RHTTPHeaders& aCachedHeaders,
            RStringPool aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void AddValidationHeaders( const RHTTPHeaders aCachedHeaders, RHTTPHeaders& aRequestHeaders, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool OperatorCacheContent( const TDesC8& aOpDomain, const TDesC8& aUrl );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool VSSCacheContent( const TDesC8& aUrl, const HBufC8* aWhiteList );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool PragmaNoCache( RHTTPTransaction& aTrans );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void GetHeaderFileName( const TDesC& aBodyFileName, TDes& aHeaderFileName );
        
        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void AdjustExpirationTimeL( RHTTPHeaders& aResponseHeaders, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @param
        * @return
        */
        static TBool CacheNeedsUpdateL( RHTTPHeaders& aResponseHeader, const TDesC8& aCachedHeaderStr, RStringPool& aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TBool IsCacheable( RHTTPTransaction& aTrans, TUint aMaxSize, TBool& aProtectedEntry );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TInt GetCacheControls( const RHTTPHeaders& aHeaders, TInt64* aMaxAge, TInt64* aMinFresh, TInt64* aMaxStale, TBool* aMustRevalidate,
            TBool* aNoCache, TBool* aNoStore, RStringPool aStrP );

    /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static HBufC* GenerateNameLC(const TDesC8& aUrl, const TDesC& aBaseDir );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void WriteLog( TInt aLogLevel, TPtrC aBuf, TInt aAny = 0xffff );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void WriteUrlToLog( TInt aLogLevel, const TDesC8& aUrl, TInt aAny = 0xffff );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static void WriteUrlToLog( TInt aLogLevel, const TDesC& aTxt, const TDesC8& aUrl, TInt aAny = 0xffff );

    private:

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TInt64 Freshness( const RHTTPHeaders& aHeaders, TTime aResponseTime, RStringPool aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TInt64 Age( const RHTTPHeaders& aHeaders, TTime aRequestTime, TTime aResponseTime, RStringPool aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TInt GetCacheControlDirective( const RHTTPHeaders& aHeaders, TInt aIndex, RStringF& aDirective, TInt64* aDirectiveValue,
            char** aExtraValue, RStringPool aStrP );

        /**
        *
        * @since 3.1
        * @param
        * @return
        */
        static TInt ExtractCacheControlDirectiveValue( RStringPool aStrP, RStringF& aDirective,  TDesC8& aValue, TInt64* aDirectiveValue,
            char** aExtraValue );
    };

#endif      // CHTTPCACHEUTIL_H

// End of File
