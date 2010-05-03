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


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include "urlloader_urlloaderI.h"
#include "urlloader_curlloader.h"
#include "urlloader_loaderutils.h"
#include "httploader_mlinkresolvercallbacks.h"
#include "nwx_defs.h"
#include "nwx_ctx.h"
#include <uri16.h>
#include "BrsrStatusCodes.h"

//R
//#include "httploadereventtouilistener.h"

//lint -save -e774 Boolean within 'if' always evaluates to True

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT(KSchemeHttps, "https");
_LIT(KSchemeFile, "file");
_LIT(KSchemeHttp, "http");


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} // extern "C"
#endif

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CKHttpLoader::UrlLoader_GetUrlLoader
// Get a pointer to the url loader.
// -----------------------------------------------------------------------------
//
static CKUrlLoader* UrlLoader_GetUrlLoader()
{
    return (CKUrlLoader*)NW_Ctx_Get(NW_CTX_URL_LOADER, 0);
}


// -----------------------------------------------------------------------------
// CKHttpLoader::UrlLoader_HttpRequestHeadersNewL
// Create a HttpHeaders structure with some or all of the following arguments.
// Each field can contain a value, or can be NULL
// -----------------------------------------------------------------------------
//
static HttpRequestHeaders* UrlLoader_HttpRequestHeadersNewL(const TUint8* aAccept,
                                                            const TUint8* aContentType,
                                                            const TUint16* aReferer,
                                                            const TUint8* aBoundary,
                                                            TUint aContentLength,
                                                            const TUint8* aExpectedContentType)
{
    TUint8* accept = NULL;
    TUint8* contentType = NULL;
    TUint8* referer = NULL;
    TUint8* boundary = NULL;
    TUint8* expectedContentType = NULL;
    TInt pushed = 0;
    HttpRequestHeaders* headers = NULL;
    // If accept header is set, allocate and copy it
    if (aAccept != NULL)
    {
        accept = new TUint8 [User::StringLength(aAccept) + 1];
        User::LeaveIfNull(accept);
        Mem::Copy(accept, aAccept, User::StringLength(aAccept) + 1);
        CleanupStack::PushL(accept);
        pushed++;
    }

    // If contentType header is set, allocate and copy it
    if (aContentType != NULL)
    {
        contentType = new TUint8 [User::StringLength(aContentType) + 1];
        User::LeaveIfNull(contentType);
        Mem::Copy(contentType, aContentType, User::StringLength(aContentType) + 1);
        CleanupStack::PushL(contentType);
        pushed++;
    }

    // If referer header is set, allocate and copy it
    if (aReferer != NULL)
    {
        TUint refererLength = User::StringLength(aReferer) + 1;
        TPtrC16 referer16(aReferer);
        TPtr8 referer8(NULL, 0);
        referer = new TUint8 [refererLength];
        User::LeaveIfNull(referer);
        CleanupStack::PushL(referer);
        pushed++;

        referer8.Set(referer, 0, refererLength);
        referer8.Copy(referer16);
        referer8.ZeroTerminate();
    }

    // If the boundary for multipart is set, allocate and copy it
    if (aBoundary != NULL)
    {
        boundary = new TUint8 [User::StringLength(aBoundary) + 1];
        User::LeaveIfNull(boundary);
        Mem::Copy(boundary, aBoundary, User::StringLength(aBoundary) + 1);
        CleanupStack::PushL(boundary);
        pushed++;
    }
        
    // If expectedContentType header is set, allocate and copy it
    if (aExpectedContentType != NULL)
    {
        expectedContentType = new TUint8 [User::StringLength(aExpectedContentType) + 1];
        User::LeaveIfNull(expectedContentType);
        Mem::Copy(expectedContentType, aExpectedContentType, User::StringLength(aExpectedContentType) + 1);
        CleanupStack::PushL(expectedContentType);
        pushed++;
    }
    headers = new(ELeave) HttpRequestHeaders;
    headers->accept = accept;
    headers->contentType = contentType;
    headers->referer = referer;
    headers->boundary = boundary;
    headers->contentLength = aContentLength;
    headers->expectedContentType = expectedContentType;
    CleanupStack::Pop(pushed);
    return headers;
}

// ========================== OTHER EXPORTED FUNCTIONS =========================
#ifdef __cplusplus
extern "C" {
#endif



// -----------------------------------------------------------------------------
// UrlLoader_Shutdown
// Free the loaders and close the session to the http stack.
// -----------------------------------------------------------------------------
//
void UrlLoader_Shutdown()
{
    CKUrlLoader* urlLoader = UrlLoader_GetUrlLoader();

    if (urlLoader != NULL)
    {
        delete urlLoader;
        NW_Ctx_Set(NW_CTX_URL_LOADER, 0, NULL);
    }
}


// -----------------------------------------------------------------------------
// UrlLoader_Request
// Make a load request to the loaders.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode UrlLoader_Request(const NW_Ucs2* aUrl, void* aHeader,
                                     NW_Uint8* aData, NW_Uint32 aDataLen,
                                     NW_Uint8 aMethod, NW_Uint16* aTransId,
                                     void* aLoadContext,
                                     NW_Url_RespCallback_t* aLoadCallback,
                                     NW_Cache_Mode_t aCacheMode,
                                     NW_Uint32 aDefaultHeadersMask,
                                     NW_Uint8 loadType,
                                     NW_Bool aIsTopLevel,
                                     NW_Bool aIsScript,
                                     NW_Uint8 aReason,
                                     void* aLoadData)
    {
    CKUrlLoader* urlLoader = UrlLoader_GetUrlLoader();
    TBrowserStatusCode status = KBrsrFailure;
    __ASSERT_DEBUG(urlLoader != NULL, LoaderUtils::PanicLoader());
    
    
    if (urlLoader != NULL)
        {
        status = urlLoader->LoadRequest(aUrl, aHeader, aData, aDataLen,
                                        aMethod, aTransId, aLoadContext,
                                        aLoadCallback, aCacheMode,
                                        aDefaultHeadersMask,
                                        loadType, aIsTopLevel, aIsScript,
                                        aReason, aLoadData);
        if (status != KBrsrSuccess)
            {
            if ((aDataLen > 0) && (aData != NULL))
                {
                delete [] aData;
                }
            UrlLoader_HeadersFree(aHeader);
            }
        }

    return status;
    }


// -----------------------------------------------------------------------------
// UrlLoader_HeadersNew
// Create a HttpHeaders structure with some or all of the following arguments.
// Each field can contain a value, or can be NULL
// -----------------------------------------------------------------------------
//
void* UrlLoader_HeadersNew(const NW_Uint8* aAccept,
                           const NW_Uint8* aContentType,
                           const NW_Ucs2* aReferer,
                           const NW_Uint8* aBoundary,
                           NW_Uint32 aContentLength,
                           const NW_Uint8* aExpectedContentType)
{
    TInt ret = KErrNone;
    HttpRequestHeaders* headers = NULL;
    TRAP(ret, headers = UrlLoader_HttpRequestHeadersNewL(aAccept,
														 aContentType,
														 aReferer,
														 aBoundary,
														 aContentLength,
                                                         aExpectedContentType));
    if (ret == KErrNone)
	{
        return headers;
	}

    return NULL;
}


// -----------------------------------------------------------------------------
// UrlLoader_HeadersFree
// Free the headers structure.
// -----------------------------------------------------------------------------
//
void UrlLoader_HeadersFree(void* aHeaders)
{
    if (aHeaders != NULL)
    {
        HttpRequestHeaders* headers = (HttpRequestHeaders*)aHeaders;
        delete [] headers->accept;
        delete [] headers->contentType;
        delete [] headers->referer;
        delete [] headers->boundary;
        delete headers;
    }
}


// -----------------------------------------------------------------------------
// UrlLoader_HeadersCopy
// Copy the headers structure.
// -----------------------------------------------------------------------------
//
void* UrlLoader_HeadersCopy(const void* aHeaders)
{
    if (aHeaders == NULL)
	{
        return NULL;
    }
	
	HttpRequestHeaders* newHeaders = NULL;

    HttpRequestHeaders* headers = (HttpRequestHeaders*)aHeaders;

    newHeaders = (HttpRequestHeaders*)UrlLoader_HeadersNew(headers->accept,
														   headers->contentType,
														   NULL,
														   headers->boundary,
														   headers->contentLength,
                                                           headers->expectedContentType);
    if (headers->referer != NULL)
    {
        newHeaders->referer =
				new TUint8 [User::StringLength(headers->referer) + 1];
        
		if (newHeaders->referer != NULL)
        {
            Mem::Copy(newHeaders->referer, headers->referer,
					  User::StringLength(headers->referer) +1);
        }
        else
        {
            UrlLoader_HeadersFree(newHeaders);
            newHeaders = NULL;
        }
    }
    return newHeaders;
}



// -----------------------------------------------------------------------------
// GetFlags
// Local helper function for UrlLoader_UpdateSecurityStatus.
// Gets the flags on urlLoader->iSecureFlag.
// Finds if a security note is needed and displays it
// -----------------------------------------------------------------------------
//
void GetFlags(TUint   aSecurityFlag,
			  TUint8* aIsPrevTopLevelSecure,
              TUint8* aIsCurrentTopLevelSecure,
			  TUint8* aWasNoteDisplayedForThisTopLevel,
              TUint8* aWasNoteAcceptedForThisTopLevel)
    {
    __ASSERT_DEBUG(aIsPrevTopLevelSecure != NULL, LoaderUtils::PanicLoader());
    __ASSERT_DEBUG(aIsCurrentTopLevelSecure != NULL, LoaderUtils::PanicLoader());
    __ASSERT_DEBUG(aWasNoteDisplayedForThisTopLevel != NULL,
                   LoaderUtils::PanicLoader());
    __ASSERT_DEBUG(aWasNoteAcceptedForThisTopLevel != NULL,
                   LoaderUtils::PanicLoader());
    
    *aIsPrevTopLevelSecure = (TUint8)(aSecurityFlag & 0xFF);
    *aIsCurrentTopLevelSecure = (TUint8)((aSecurityFlag >> 8) & 0xFF);
    *aWasNoteDisplayedForThisTopLevel = (TUint8)((aSecurityFlag >> 16) & 0xFF);
    *aWasNoteAcceptedForThisTopLevel = (TUint8)((aSecurityFlag >> 24) & 0xFF);
    }




// -----------------------------------------------------------------------------
// UrlLoader_InitializeL
// Header: urlloader_urloaderi.h
// Initialize the loaders
// -----------------------------------------------------------------------------
//
void UrlLoader_InitializeL(MBrCtlSpecialLoadObserver* aSpecialLoadObserver,
						   MHttpLoader_LinkResolverCallbacks* aLinkCallbacks,
						   MWmlInterface* aWKWmlInterface)
    {
    CKUrlLoader* urlLoader = NULL;
    urlLoader = CKUrlLoader::NewL(aSpecialLoadObserver, aLinkCallbacks,aWKWmlInterface);
    NW_Ctx_Set(NW_CTX_URL_LOADER, 0, urlLoader);
    }


// -----------------------------------------------------------------------------
// IsSubmit
// Description: Determines if the load is a result of sumbit or not.
//				Local helper function for HandleTopLevelSecurityStateChange.
//                
// Parameters:   *aUrl                 - pointer to the url to load
//               aMethod               - post or get
//               aIsTopLevel           - is top level request or not
//               aHasHeaders           - a flag is the request contains hedears
//
// Return Value: EFalse 
//               ETrue 
// -----------------------------------------------------------------------------
//
NW_Bool IsSubmit(const NW_Ucs2 *aUrl, NW_Uint8 aMethod,
				 NW_Bool aIsTopLevel, NW_Bool aHasHeaders)
{
    __ASSERT_DEBUG(aUrl != NULL, LoaderUtils::PanicLoader());
	if (aUrl == NULL)
	{
		return EFalse;
	}

	TPtrC urlPtr(aUrl);
	if (aIsTopLevel)
	{
		if (aMethod == NW_URL_METHOD_POST || (aHasHeaders && urlPtr.Locate('?')))
		{
			return ETrue;
		}
	}

	return EFalse;
}



// -----------------------------------------------------------------------------
// UrlLoader_FindUrlScheme
// Determine the scheme of the url.
// -----------------------------------------------------------------------------
//
TUrlLoaderScheme UrlLoader_FindUrlScheme(const NW_Ucs2* aUrl)
    {
    if (aUrl == NULL)
        {
        return ESchemeInvalid;
        }
    
    TUriParser parser;
    TPtrC uriPtr(aUrl);
    if (parser.Parse(uriPtr) != KErrNone)
        {
        return ESchemeInvalid;
        }
    
    if (!parser.IsSchemeValid())
        {
        return ESchemeInvalid;
        }
    
    TPtrC schemePtr(parser.Extract(EUriScheme));
    TInt schemeLength = schemePtr.Length();
    
    // HTTP scheme is the default
    if (schemeLength == 0)
        {
        return ESchemeHttp;
        }
    
    // Identify the scheme
    if (schemeLength == KSchemeHttp().Length() &&
        uriPtr.FindF(KSchemeHttp) == 0)
        {
        return ESchemeHttp;
        }

    if (schemeLength == KSchemeHttps().Length() &&
        uriPtr.FindF(KSchemeHttps) == 0)
        {
        return ESchemeHttps;
        }
    
    if (schemeLength == KSchemeFile().Length() &&
        uriPtr.FindF(KSchemeFile) == 0)
        {
        return ESchemeFile;
        }
    
    return ESchemeUnknown;
    }


// -----------------------------------------------------------------------------
// UrlLoader_IsBrowserScheme
// Determine if the url is handled by the browser.
// -----------------------------------------------------------------------------
//
NW_Bool UrlLoader_IsBrowserScheme(const NW_Ucs2* aUrl)
    {
    TUrlLoaderScheme urlScheme = UrlLoader_FindUrlScheme(aUrl);
    
    return (urlScheme != ESchemeUnknown);
    }

// -----------------------------------------------------------------------------
// UrlLoader_IsUrlInCache
// Check if a specific url is in cache.
// -----------------------------------------------------------------------------
//
NW_Bool UrlLoader_IsUrlInCache(NW_Ucs2* aUrl)
    {
    NW_Bool ret = NW_FALSE;
    CKUrlLoader* urlLoader = UrlLoader_GetUrlLoader();
    
    if (urlLoader )
        {
        ret = (NW_Bool)urlLoader->IsUrlInCache(aUrl);
        }
    
    return ret;
    }

#ifdef __cplusplus
} /* extern "C" */
#endif

//lint -restore

//  End of File
