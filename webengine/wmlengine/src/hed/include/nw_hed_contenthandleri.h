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


#ifndef NW_HED_CONTENTHANDLERI_H
#define NW_HED_CONTENTHANDLERI_H

#include "nw_hed_documentnodei.h"
#include "nw_hed_contenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_HED_ContentHandler_CreateBoxTree(_contentHandler, _boxTree) \
  (NW_Object_Invoke (_contentHandler, NW_HED_ContentHandler, createBoxTree) ( \
     NW_HED_ContentHandlerOf (_contentHandler), (_boxTree)))

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
const NW_Text_t*
_NW_HED_ContentHandler_GetURL (NW_HED_ContentHandler_t* contentHandler);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_ResolveURL (NW_HED_ContentHandler_t* contentHandler,
                                   NW_Text_t* url, NW_Text_t** retUrl);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp);

NW_HED_EXPORT
void
_NW_HED_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_PartialNextChunk( NW_HED_ContentHandler_t* thisObj,
                                         NW_Int32 chunkIndex,
                                         NW_Url_Resp_t* response,
                                         void* context); 

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                   NW_LMgr_Box_t** boxTree);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_Initialize( NW_HED_DocumentNode_t* documentNode,
                                   TBrowserStatusCode aInitStatus );

NW_HED_EXPORT
void
_NW_HED_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, 
                                NW_Bool aDestroyBoxTree);

NW_HED_EXPORT
void
_NW_HED_ContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_ContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
_NW_HED_ContentHandler_CreateHistoryEntry (NW_HED_ContentHandler_t* contentHandler, 
                                           NW_HED_UrlRequest_t* urlRequest, 
                                           NW_HED_UrlResponse_t* urlResponse);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
_NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry (NW_HED_ContentHandler_t* thisObj,
                                                        const NW_Text_t* requestUrl,
                                                        const NW_Text_t* responseUrl);
NW_HED_EXPORT
NW_HED_UrlResponse_t*
_NW_HED_ContentHandler_NewUrlResponse (NW_HED_ContentHandler_t* contentHandler,
                                       NW_Url_Resp_t* response);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_HandleRequest (NW_HED_ContentHandler_t* thisObj,
                                      NW_HED_UrlRequest_t* urlRequest);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_FeatureQuery (NW_HED_ContentHandler_t* thisObj,
                                     NW_HED_ContentHandler_Feature_t featureId);


NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ContentHandler_ResponseComplete( NW_HED_ContentHandler_t* thisObj,
                                         NW_Url_Resp_t* response,
                                         void* context); 

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_CONTENTHANDLERI_H */
