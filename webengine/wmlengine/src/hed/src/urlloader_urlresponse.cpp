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


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include "urlloader_urlresponse.h"

//R->ul #include "urlloader_urlloaderint.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// UrlLoader_UrlResponseNew
// Create a url response structure
// -----------------------------------------------------------------------------
//
NW_Url_Resp_t* UrlLoader_UrlResponseNew(const NW_Ucs2   *aUri,
                                        NW_Uint8        *aBody,
                                        NW_Uint32       aBodyLength,
                                        void            *aHeaders,
                                        NW_Uint8        *aContentTypeString,
					                    NW_Uint8        *aContentLocationString,
                                        NW_Bool         aNoStore,
                                        NW_Uint8        *aBoundaryString,
                                        NW_Ucs2         *aLastModified,
                                        NW_Uint16       aCharset,
                                        NW_Uint8        *aCharsetString,
                                        NW_Uint32       aHttpStatus,
                                        NW_Uint8        aMethod,
                                        NW_Uint16       aTransId,
                                        NW_Uint32       aContentLength)
{
    NW_Buffer_t* buffer = NULL;
    NW_Url_Resp_t* resp = NULL;
    NW_Ucs2* uri = NULL;

    buffer = new NW_Buffer_t;
    resp = new NW_Url_Resp_t;
    uri = new NW_Ucs2 [(User::StringLength(aUri) + 1)];

    //lint -e{774} Boolean always evaluates to False
    if (buffer == NULL || resp == NULL || uri == NULL)
    {
        delete buffer;
        delete resp;
        delete uri;
        return NULL;
    }

    Mem::Copy(uri, aUri, (User::StringLength(aUri) + 1) * sizeof(NW_Ucs2));
    resp->uri = uri;
    resp->body = buffer;
    resp->body->data = aBody;
    resp->body->length = aBodyLength;
    resp->body->allocatedLength = aBodyLength;
    resp->headers = aHeaders;
    resp->contentTypeString = aContentTypeString;
    resp->contentTypeString16 = NULL;
    resp->contentLocationString = aContentLocationString;
    resp->noStore = aNoStore;
    resp->boundary = aBoundaryString;
    resp->lastModified = aLastModified;
    resp->charset = aCharset;
    resp->charsetString = aCharsetString;
    resp->httpStatus = aHttpStatus;
    resp->method = aMethod;
    resp->transId = aTransId;
    resp->certInfo = NULL;
    resp->contentLength = aContentLength;
    return resp;
}


// -----------------------------------------------------------------------------
// UrlLoader_UrlResponseDelete
// Free a url response structure
// -----------------------------------------------------------------------------
//
void UrlLoader_UrlResponseDelete(NW_Url_Resp_t* aResp)
{
    if (aResp != NULL)
    {
        delete aResp->uri;
        if (aResp->body != NULL && aResp->body->length > 0)
            delete aResp->body->data; 
        delete aResp->body;
        //UrlLoader_HeadersFree(aResp->headers);
        delete aResp->contentTypeString;
        delete aResp->contentTypeString16;
        delete aResp->contentLocationString;
        delete aResp->boundary;
        delete aResp->lastModified;
        delete aResp->charsetString;
        delete aResp;
    }
}


//  End of File
