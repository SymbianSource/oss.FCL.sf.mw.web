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
* Description:  Creating and deleting of url response
*
*/


#ifndef URLRESPONSE_H
#define URLRESPONSE_H

#ifdef __cplusplus
extern "C" {
#endif
    //  INCLUDES
#include "nwx_defs.h"
#include "nwx_buffer.h"
#include "BrsrStatusCodes.h"

// CONSTANTS

// MACROS

// DATA TYPES
typedef enum {
    NW_CACHE_NORMAL,      // Normal Http load, use cache if not stale
    NW_CACHE_HISTPREV,    // History Prev load, use cache even if stale,
						  // unless must-revalidate
    NW_CACHE_NOCACHE,     // Do not use cache, even if fresh
    NW_CACHE_ONLYCACHE    // Only load from cache, for loading saved deck
} NW_Cache_Mode_t;


typedef struct NW_Url_Resp_s
{
    NW_Ucs2     *uri;               // The original URL of the request
									// NULL terminated
    NW_Buffer_t *body;              // The body of the request
    void        *headers;           // The response headers
    NW_Uint8    *contentTypeString; // The string representation of content type
    NW_Ucs2     *contentTypeString16; // The string representation of content type
    NW_Uint8    *contentLocationString; // The string representation of Content Base header
    NW_Bool     noStore;            // HTTP cache-control directive.
    NW_Uint8    *boundary;          // The string representation of boundary (multipart)
    NW_Ucs2     *lastModified;      // The string representation of last-modified header. Used by ECMAScript
    NW_Uint16   charset;            // WAP encoding of response's character set
    NW_Uint8    *charsetString;     // The charset string
    NW_Uint8    byteOrder;          // Byte order: big-endian or little-endian for 2-byte charset or single byte
    NW_Uint32   httpStatus;         // Response status as received from server
    NW_Uint8    method;             // Method used for the final request
    NW_Uint16   transId;            // The transaction id of the response
    void* 		certInfo;           // The certificate information
    NW_Uint32   contentLength;      // The content length of the response. Used only for HEAD requests
} NW_Url_Resp_t;


// A URLPartialResponseCallback is passed into this component to receive partial response
typedef TBrowserStatusCode (NW_Url_RespCallback_t)
	(TBrowserStatusCode   status,        // The load status
     NW_Uint16     transId,       // The transaction id of the response
     NW_Int32      chunkIndex,      // chunk index
     void          *loadContext,  // A context to pass back with the response
     NW_Url_Resp_t *resp);        // The response structure

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

/**
* Create a url response structure
* aUri The response URL, NULL terminated.
* aBody The body of the request.
* aBodyLength The length of the body of the request.
* aHeaders The response headers.
* aContentTypeString The string representation of the content type.
* aCharset The WAP encoding of the response's character set.
* aCharsetString The response's character set.
* aHttpStatus The response status, as received from the server.
* aMethod The method used for the final request.
* aTransId The transaction id of the response.
* aContentLength The content length of the response. Used only for HEAD requests.
*/

NW_Url_Resp_t* UrlLoader_UrlResponseNew(const NW_Ucs2   *aUri,
                                        NW_Uint8        *aBody,
                                        NW_Uint32       aBodyLength,
                                        void            *aHeaders,
                                        NW_Uint8        *aContentTypeString,
                                        NW_Uint8        *aContentLocationString,
                                        NW_Bool         noStore,
                                        NW_Uint8        *aBoundaryString,
                                        NW_Ucs2         *aLastModified,
                                        NW_Uint16       aCharset,
                                        NW_Uint8        *aCharsetString,
                                        NW_Uint32       aHttpStatus,
                                        NW_Uint8        aMethod,
                                        NW_Uint16       aTransId,
                                        NW_Uint32       aContentLength);
/*//R
NW_Url_Resp_t* UrlLoader_UrlResponseNew(const NW_Ucs2   	*aUri,
                                        const NW_Uint8      *aBody,
                                        NW_Uint32       	aBodyLength,
                                        void            	*aHeaders,
                                        const NW_Uint8      *aContentTypeString,
                                        const NW_Uint8      *aContentLocationString,
                                        NW_Bool         	noStore,
                                        const NW_Uint8      *aBoundaryString,
                                        const NW_Ucs2         	*aLastModified,
                                        const NW_Uint16     aCharset,
                                        NW_Uint8        	*aCharsetString,
                                        NW_Uint32       	aHttpStatus,
                                        NW_Uint8        	aMethod,
                                        NW_Uint16       	aTransId,
                                        NW_Uint32       	aContentLength);
*/

/**
* Free a url response structure
*/
void UrlLoader_UrlResponseDelete(NW_Url_Resp_t* aResp);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif      // URLRESPONSE_H

// End of File
