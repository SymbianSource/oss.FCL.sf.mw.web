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
* Description:  C interface to url loader
*
*/


#ifndef URLLOADERINT_H
#define URLLOADERINT_H

//  INCLUDES
#include "nwx_ctx.h" // this file includes nwx_defs.h, which includes nwx_status.h
#include "urlloader_urlresponse.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" {
#endif
    
// CONSTANTS
    
#define NW_NUM_INSTANCES 3


// MACROS
    
// DATA TYPES
    
// The "TEnterStatus", relates to what security state the page is going to.
// This "page loading" status, is passed in HandleBrowserLoadEventL() using the
// TBrCtlLoadEvent enum. (see brctldefs.h)  
// These constants must stay in sync with viewshellobserver.h
//R

enum TEnterStatus
    {
    EEnteringSecurePage,
    EExitingSecurePage,
    ESomeItemsNotSecure,
    ESubmittingToNonSecurePage,
    ERedirectConfirmation,
    ERepostConfirmation,
    ESecureItemInNonSecurePage
    // others to come        
    };

// The following schemes are only those supported by the browser
typedef enum
    {
    ESchemeInvalid,
    ESchemeUnknown,
    ESchemeHttps,
    ESchemeMailto,
    ESchemeFile,
    ESchemeHttp,
    ESchemeWtai,
    ESchemeTel
} TUrlLoaderScheme;
    
// Structure for holding context data
typedef struct
    {
  void *uniqueNum[NW_NUM_INSTANCES+1];
  void *contexts[NW_NUM_INSTANCES+1] [NW_NUM_CONTEXTS+1];
} NW_ContextArray_t;

// Structure for holding extra request headers information
typedef struct
    {
	NW_Uint8* accept;
	NW_Uint8* contentType;
	NW_Uint8* referer;
	NW_Uint8* boundary;
	NW_Uint32 contentLength;
    NW_Uint8* expectedContentType;
    } HttpRequestHeaders;
    
typedef enum
    {
	ECookieFilter,
	ECacheFilter
    } TFilters;
    
// FUNCTION PROTOTYPES
    
// FORWARD DECLARATIONS
    
// CLASS DECLARATION
    
/**
* Cancel all the pending requests due to a disconnect request.
*/
void UrlLoader_Disconnect();

/**
* Cancel a specific request, identified by its id.
*/
void UrlLoader_Cancel(NW_Uint16 aTid);

/**
* Cancel all requests.
*/
void UrlLoader_CancelAll();

/**
* Check if a specific url is in cache.
*/
NW_Bool UrlLoader_IsUrlInCache(NW_Ucs2* aUrl);

/**
* Get the number of entries in cache.
*/
NW_Uint32 UrlLoader_GetNumCacheEntries();

/**
* Remove all the cached items.
*/
NW_Uint32 UrlLoader_ClearCache();

/**
* Remove a specific item from cache, identified by its url.
*/
TBrowserStatusCode UrlLoader_ClearItemInCache(const NW_Ucs2* aUrl);

/**
* Set a header on a specic item, identified by its url.
*/
TBrowserStatusCode UrlLoader_SetHeaderInCache(const NW_Ucs2* aUrl,
                                              unsigned char* aName,
                                              NW_Ucs2* aValue);

/**
* Remove all the cookies.
*/
NW_Uint32 UrlLoader_ClearCookies();

/**
* Free the loaders and close the session to the http stack.
*/
void UrlLoader_Shutdown();

/**
* Make a load request to the loaders.
* aUrl The request URL
* aHeader The request headers or NULL if no request specific headers.
* aData The body of the request.
* aDataLen The body length of the request.
* aMethod The method to use for the request could be
*		  NW_URL_METHOD_GET or NW_URL_METHOD_POST.
* aTransId The request's transaction id.
* aLoadContext The load context that should be returned with the response.
* aPartialLoadCallback The callback to call when a chunk is ready.
* aCacheMode The cache mode, it is relevant only in case of http or https.
* aDefaultHeadersMask A flag indicating what default headers should be used.
* loadType The load type
* aIsTopLevel Boolean ETrue if page is top level (images, css, etc are not top)
* aIsScript Boolean ETrue if Request is being made by script.
*/
TBrowserStatusCode UrlLoader_Request(const NW_Ucs2* aUrl,
                                     void* aHeader,
                                     NW_Uint8* aData,
                                     NW_Uint32 aDataLen,
                                     NW_Uint8 aMethod,
                                     NW_Uint16* aTransId,
                                     void* aLoadContext,
                                     NW_Url_RespCallback_t* aLoadCallback,
                                     NW_Cache_Mode_t aCacheMode,
                                     NW_Uint32 aDefaultHeadersMask,
                                     NW_Uint8 loadType,
                                     NW_Bool aIsTopLevel,
                                     NW_Bool aIsScript,
                                     NW_Uint8 aReason,
                                     void* aLoadData);

/**
* Create a HttpHeaders structure with some or all of the following arguments.
* Each field can contain a value, or can be NULL.
* aAccept The accept header
* aContentType The content type of the request body
* aReferer The referer
* aBoundary The boundry if it is a multipart
* aContentLength The length of the request body
*/
void* UrlLoader_HeadersNew(const NW_Uint8* aAccept,
						   const NW_Uint8* aContentType,
						   const NW_Ucs2* aReferer,
						   const NW_Uint8* aBoundary,
						   NW_Uint32 aContentLength,
                           const NW_Uint8* aExpectedContentType);

/**
* Free the headers structure.
*/
void UrlLoader_HeadersFree(void* aHeaders);

/**
* Copy the headers.
*/
void* UrlLoader_HeadersCopy(const void* aHeaders);

/**
* Load / Unload filter specified by UID.
*/
void UrlLoader_ChangeFilterLoadStatusL(TFilters aFilter, NW_Bool aLoad);

/**
* Update security status on the UrlLoader
*/
void UrlLoader_UpdateSecurityStatus(TBrowserStatusCode aLoadStatus,
                                    NW_Url_Resp_t* aResponse);

/**
* Determine the scheme of the URL.
*/
TUrlLoaderScheme UrlLoader_FindUrlScheme(const NW_Ucs2* aUrl);

/**
* Determine if the URL is handled by the browser.
*/
NW_Bool UrlLoader_IsBrowserScheme(const NW_Ucs2* aUrl);

/**
* Set Internet Access Point id to use for connection
*/
void UrlLoader_SetIAPid(NW_Uint32 aIapId);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif      // URLLOADERINT_H

// End of File
