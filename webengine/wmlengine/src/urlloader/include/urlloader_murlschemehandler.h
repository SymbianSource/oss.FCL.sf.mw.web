/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling of general url loading
*
*/


#ifndef MURLSCHEMEHANDLER_H
#define MURLSCHEMEHANDLER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "urlloader_loaderutils.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Base class for all loaders.
*
*  @lib urlloader.lib
*  @since 2.0
*/
class MUrlSchemeHandler : public CBase
{

public: // New functions

    /**
    * Make a generic load request
    * @since 2.0
    * @param aUrl The request URL.
    * @param aHeader The request headers or NULL if no request specific headers.
    * @param aData The body and body length of the request.
    * @param aMethod The method to use for the request could be
	*				 NW_URL_METHOD_GET or NW_URL_METHOD_POST.
    * @param aTransId The request's transaction id.
    * @param aLoadContext The load context that should be returned with the response.
    * @param aLoadCallback The callback to call when the response is ready.
    * @param aPartialLoadCallback The callback to call when a chunk is ready.
    * @param aCacheMode The cache mode. It is relevant only in case of http or https.
    * @param aDefaultHeadersMask A flag indicating what default headers should be used.
	* @param aLoadType The load type
	* @param aIsTopLevel A boolean denoting if this url is top level (page, not images)
    * @param aIsScript Is it a WmlScript load event?
    * @return ETrue if processed the load request, EFalse otherwise
    */
    virtual TBool HandleLoadRequestL(const TUint16* aUrl, void* aHeader, TPtr8& aData,
                          TUint8 aMethod, TUint16 aTransId, void* aLoadContext,
                          NW_Url_RespCallback_t* aPartialLoadCallback,
                          NW_Cache_Mode_t aCacheMode, TUint aDefaultHeadersMask,
                          TUint8 aLoadType, TBool aIsTopLevel, TBool aIsScript,
                          NW_Uint8 aReason, void* aLoadData) = 0;

    /**
    * Call the partial response callback with a chunk.
    * @since 2.0
    * @param aUri The response URL.
    * @param aBody The response body
    * @param aHeaders The response headers. Currently this is always NULL.
    * @param aContentType The content type.
    * @param aContentLocationString The content location string.
    * @param aNoStore "no-store" indicator prevents saving images.
    * @param aBoundaryString The boundary.
    * @param aCharset The character set.
    * @param aCharsetString The character set.
    * @param aHttpStatus The Http status code.
    * @param aMethod The method NW_URL_METHOD_GET or NW_URL_METHOD_POST .
    * @param aTransId The transaction id.
    * @param aContentLength The content length of the response. Used only for HEAD requests.
    * @param aErr The status of the response
    * @param aLoadContext The context that was passed with the request.
    * @param aPartialCallback The callback to call when a chunk is ready.    
    * @return TBrowserStatusCode status of the partial buffer processing 
    */
    inline TBrowserStatusCode PartialResponse(const TUint16* aUri, TPtr8& aBody,
                         void* aHeaders, TUint8* aContentTypeString, 
                         TUint8* aContentLocationString,  NW_Bool aNoStore, TUint8* aBoundaryString, TUint16* aLastModified, 
                         TUint16 aCharset, TUint8* aCharsetString, TUint aHttpStatus, TUint8 aMethod,
                         TUint16 aTransId, TInt aChunkIndex, void* aCertInfo, 
                         TUint aContentLength, TInt aErr,
                         void* aLoadContext, NW_Url_RespCallback_t* aPartialCallback );

    /**
    * Call the partial response callback with an error code.
    * @since 2.0
    * @param aTransId The transaction id.
    * @param aErr The status of the response
    * @param aLoadContext The context that was passed with the request.
    * @param aCallback The callback to call when the response is ready.
    * @return TBrowserStatusCode status of the partial buffer processing
    */
    inline TBrowserStatusCode PartialResponse(TUint16 aTransId, TInt aErr, void* aLoadContext,
						 NW_Url_RespCallback_t* aPartialCallback);

private:

    // Boolean to determine if 300 or 500 errors have associated content
    TBool iIsBodyPresent;

};

#include "urlloader_murlschemehandler.inl"

#endif      // MURLSCHEMEHANDLER_H

// End of File
