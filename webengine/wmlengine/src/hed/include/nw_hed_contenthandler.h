/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef NW_HED_CONTENTHANDLER_H
#define NW_HED_CONTENTHANDLER_H

#include "nw_hed_documentnode.h"
#include "nw_hed_historyentry.h"
#include "nw_hed_urlrequest.h"
#include "nw_hed_urlresponse.h"
#include "nw_text_ucs2.h"
#include <urlloader_urlresponse.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
typedef enum NW_HED_ContentHandler_Feature_e {
  NW_HED_ContentHandler_NoFeatures = 0,
  NW_HED_ContentHandler_HitoryPrevUseStale
} NW_HED_ContentHandler_Feature_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_HED_ContentHandler_Class_s NW_HED_ContentHandler_Class_t;
typedef struct NW_HED_ContentHandler_s NW_HED_ContentHandler_t;

/* ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_ContentHandler_PartialNextChunk_t) (NW_HED_ContentHandler_t* thisObj,
                                             NW_Int32 chunkIndex,
                                             NW_Url_Resp_t* response,
                                             void* context);

typedef
const NW_Text_t*
(*NW_HED_ContentHandler_GetTitle_t) (NW_HED_ContentHandler_t* thisObj);

typedef
const NW_Text_t*
(*NW_HED_ContentHandler_GetURL_t) (NW_HED_ContentHandler_t* thisObj);

typedef
TBrowserStatusCode
(*NW_HED_ContentHandler_ResolveURL_t) (NW_HED_ContentHandler_t* thisObj, NW_Text_t* url, NW_Text_t** retUrl);

typedef
NW_HED_HistoryEntry_t*
(*NW_HED_ContentHandler_CreateHistoryEntry_t) (NW_HED_ContentHandler_t* thisObj,
                                               NW_HED_UrlRequest_t* urlRequest, 
                                               NW_HED_UrlResponse_t* urlResponse);

typedef
NW_HED_HistoryEntry_t*
(*NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry_t) (NW_HED_ContentHandler_t* thisObj,
                                                            const NW_Text_t* requestUrl,
                                                            const NW_Text_t* responseUrl);

typedef
NW_HED_UrlResponse_t*
(*NW_HED_ContentHandler_NewUrlResponse_t) (NW_HED_ContentHandler_t* thisObj,
                                           NW_Url_Resp_t* response);

typedef
TBrowserStatusCode
(*NW_HED_ContentHandler_CreateBoxTree_t) (NW_HED_ContentHandler_t* thisObj,
                                          NW_LMgr_Box_t** boxTree);

typedef
TBrowserStatusCode
(*NW_HED_ContentHandler_HandleRequest_t) (NW_HED_ContentHandler_t* thisObj,
                                          NW_HED_UrlRequest_t* urlRequest);

typedef
TBrowserStatusCode
(*NW_HED_ContentHandler_FeatureQuery_t) (NW_HED_ContentHandler_t* thisObj,
                                         NW_HED_ContentHandler_Feature_t featureId);

typedef
TBrowserStatusCode
(*NW_HED_ContentHandler_ResponseComplete_t) (NW_HED_ContentHandler_t* thisObj,
                                             NW_Url_Resp_t* response,
                                             void* context);

/* ------------------------------------------------------------------------- */
typedef struct NW_HED_ContentHandler_ClassPart_s {
  NW_HED_ContentHandler_PartialNextChunk_t partialNextChunk;
  NW_HED_ContentHandler_GetTitle_t getTitle;
  NW_HED_ContentHandler_GetURL_t getUrl;
  NW_HED_ContentHandler_ResolveURL_t resolveUrl;
  NW_HED_ContentHandler_CreateHistoryEntry_t createHistoryEntry;
  NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry_t createIntraDocumentHistoryEntry;
  NW_HED_ContentHandler_NewUrlResponse_t newUrlResponse;
  NW_HED_ContentHandler_CreateBoxTree_t createBoxTree;
  NW_HED_ContentHandler_HandleRequest_t handleRequest;
  NW_HED_ContentHandler_FeatureQuery_t  featureQuery;
  NW_HED_ContentHandler_ResponseComplete_t responseComplete;
} NW_HED_ContentHandler_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_HED_ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
};

/* ------------------------------------------------------------------------- */
struct NW_HED_ContentHandler_s {
  NW_HED_DocumentNode_t super;

  /* member variables */
  NW_HED_HistoryEntry_t* associatedHistoryEntry;
  NW_LMgr_Box_t* boxTree;
  NW_Bool isEmbeddable;
  NW_Int32 outstandingDecoderOps;
  NW_Bool needsRelayout;
  NW_Bool initialized;

  // partial buffer
  NW_Url_Resp_t* response;
};

/* ------------------------------------------------------------------------- */
#define NW_HED_ContentHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_ContentHandler))

#define NW_HED_ContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_ContentHandler))

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ContentHandler_Class_t NW_HED_ContentHandler_Class;

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT
const NW_HED_UrlRequest_t*
_NW_HED_ContentHandler_GetUrlRequest(NW_HED_ContentHandler_t* thisObj);

NW_HED_EXPORT
const NW_HED_UrlResponse_t*
_NW_HED_ContentHandler_GetUrlResponse(NW_HED_ContentHandler_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_SetAssociatedHistoryEntry (NW_HED_ContentHandler_t* thisObj, 
                                                 NW_HED_HistoryEntry_t* entry);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
_NW_HED_ContentHandler_GetAssociatedHistoryEntry (NW_HED_ContentHandler_t* thisObj);

/* ------------------------------------------------------------------------- */
#define NW_HED_ContentHandler_PartialNextChunk(_object, _chunkIndex, _response, _context) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, partialNextChunk) \
   (NW_HED_ContentHandlerOf (_object), (_chunkIndex), (_response), (_context)))

#define NW_HED_ContentHandler_GetTitle(_object) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, getTitle) \
   (NW_HED_ContentHandlerOf (_object)))

#define NW_HED_ContentHandler_CreateHistoryEntry(_object, _urlRequest, _urlResponse) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, createHistoryEntry) \
   (NW_HED_ContentHandlerOf(_object), NW_HED_UrlRequestOf (_urlRequest), \
    NW_HED_UrlResponseOf (_urlResponse)))

#define NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry(_object, _requestUrl, _responseUrl) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, createIntraDocumentHistoryEntry) \
   (NW_HED_ContentHandlerOf(_object), NW_TextOf (_requestUrl), \
    NW_TextOf (_responseUrl)))

#define NW_HED_ContentHandler_NewUrlResponse(_object, _response) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, newUrlResponse) \
   (NW_HED_ContentHandlerOf(_object), (_response)))

#define NW_HED_ContentHandler_GetURL(_object) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, getUrl) \
   (NW_HED_ContentHandlerOf (_object)))

#define NW_HED_ContentHandler_ResolveURL(_object, _url, _retUrl) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, resolveUrl) \
   (NW_HED_ContentHandlerOf (_object), (_url), (_retUrl)))

#define NW_HED_ContentHandler_HandleRequest(_object, _urlRequest) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, handleRequest) \
   (NW_HED_ContentHandlerOf (_object), NW_HED_UrlRequestOf (_urlRequest)))

#define NW_HED_ContentHandler_FeatureQuery(_object, _featureId) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, featureQuery) \
   (NW_HED_ContentHandlerOf (_object), _featureId))

#define NW_HED_ContentHandler_GetUrlRequest(_object) \
  (_NW_HED_ContentHandler_GetUrlRequest \
     (NW_HED_ContentHandlerOf (_object)))

#define NW_HED_ContentHandler_GetUrlResponse(_object) \
  (_NW_HED_ContentHandler_GetUrlResponse \
     (NW_HED_ContentHandlerOf (_object)))

#define NW_HED_ContentHandler_SetAssociatedHistoryEntry(_object, _entry) \
  (_NW_HED_ContentHandler_SetAssociatedHistoryEntry \
     (NW_HED_ContentHandlerOf (_object), NW_HED_HistoryEntryOf (_entry)))

#define NW_HED_ContentHandler_GetAssociatedHistoryEntry(_object) \
  (_NW_HED_ContentHandler_GetAssociatedHistoryEntry \
     (NW_HED_ContentHandlerOf (_object)))

#define NW_HED_ContentHandler_SetIsEmbeddable(_object, _value) \
  (NW_HED_ContentHandlerOf (_object)->isEmbeddable = (_value))

#define NW_HED_ContentHandler_ResponseComplete(_object, _response, _context) \
  (NW_Object_Invoke (_object, NW_HED_ContentHandler, responseComplete) \
   (NW_HED_ContentHandlerOf (_object), (_response), (_context)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_CONTENTHANDLER_H */
