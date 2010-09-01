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


#ifndef _NW_XHTML_ContentHandlerI_h_
#define _NW_XHTML_ContentHandlerI_h_

#include "nw_hed_compositecontenthandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"

#include "nw_wmlscriptch_iwmlbrowserlibi.h"
#include "nw_wmlscriptch_iwmlscriptlisteneri.h"
#include "nw_markup_numbercollectori.h"

#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 /* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
extern const NW_WmlsCh_IWmlBrowserLib_Class_t NW_XHTML_ContentHandler_IWmlBrowserLib_Class;
extern const NW_WmlsCh_IWmlScriptListener_Class_t NW_XHTML_ContentHandler_IWmlScriptListener_Class;
extern const NW_Markup_NumberCollector_Class_t NW_XHTML_ContentHandler_NumberCollector_Class;
extern const NW_Object_Class_t* const NW_XHTML_InterfaceList[];
extern const NW_Markup_Module_t* const _NW_XHTML_ContentHandler_Modules[];

/* ------------------------------------------------------------------------- *
   protected consts
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ContentHandler_ChildEntry_s
{
  NW_DOM_ElementNode_t* domElement;
  NW_HED_DocumentNode_t* childNode;
} NW_XHTML_ContentHandler_ChildEntry_t;
 
/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp);

extern
void
_NW_XHTML_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject);

TBrowserStatusCode
_NW_XHTML_ContentHandler_Initialize( NW_HED_DocumentNode_t* documentNode,
                                     TBrowserStatusCode aInitStatus );
extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_PartialNextChunk( NW_HED_ContentHandler_t* contentHandler,
                                           NW_Int32 chunkIndex,
                                           NW_Url_Resp_t* response,
                                           void* context);

extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_NodeChanged (NW_HED_DocumentNode_t* documentNode,
                                      NW_HED_DocumentNode_t* childNode);

extern
NW_Uint8
_NW_XHTML_ContentHandler_ProcessEvent (NW_HED_DocumentNode_t* documentNode,
                                       NW_LMgr_Box_t* box,
                                       NW_Evt_Event_t* event,
                                       void* context);

extern
void
_NW_XHTML_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree);

extern
void
_NW_XHTML_CompositeContentHandler_Resume (NW_HED_DocumentNode_t* documentNode);

extern
void
_NW_XHTML_ContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode);

extern
void
_NW_XHTML_ContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode);

extern
void
_NW_XHTML_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode);


extern
void
_NW_XHTML_DocumentNode_HandleLoadComplete(NW_HED_DocumentNode_t* documentNode,
                                          const NW_HED_UrlRequest_t *urlRequest,
                                          NW_Int16 errorClass,
                                          TBrowserStatusCode error,
                                          NW_Uint16 transactionId,
                                          void* context );

extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_HandleRequest (NW_HED_ContentHandler_t* thisObj,
                                        NW_HED_UrlRequest_t* urlRequest);

extern
const NW_Text_t*
_NW_XHTML_ContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler);

extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_ResolveURL (NW_HED_ContentHandler_t* thisObj,
                                     NW_Text_t* url, NW_Text_t** retUrl);
extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_CreateBoxTree (NW_HED_ContentHandler_t* contentHandler,
                                        NW_LMgr_Box_t** boxTree);

extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_Cancel( NW_HED_DocumentNode_t* documentNode, 
					                       NW_HED_CancelType_t cancelType );

extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_DocumentDisplayed (NW_HED_ContentHandler_t* contentHandler);

extern
TBrowserStatusCode
_NW_XHTML_ContentHandler_INumberCollector (NW_HED_INumberCollector_t *numberCollector,
                                           NW_ADT_DynamicVector_t    *dynamicVector);

extern
NW_LMgr_Box_t*
_NW_XHTML_ContentHandler_NumberCollector_GetBoxTree (NW_Markup_NumberCollector_t* numberCollector);

extern
NW_Text_t*
_NW_XHTML_ContentHandler_NumberCollector_GetHrefAttr (NW_Markup_NumberCollector_t* numberCollector,
                                                      NW_LMgr_Box_t* box);
extern
NW_Text_t*
_NW_XHTML_ContentHandler_NumberCollector_GetAltAttr (NW_Markup_NumberCollector_t* numberCollector,
                                                     NW_LMgr_Box_t* box);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_XHTML_ContentHandlerI_h_ */
