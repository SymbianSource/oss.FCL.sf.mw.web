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


#ifndef NW_WML1X_CONTENTHANDLERI_H
#define NW_WML1X_CONTENTHANDLERI_H

#include "nw_hed_compositecontenthandleri.h"
#include "nw_wmlscriptch_iwmlbrowserlibi.h"
#include "nw_wmlscriptch_iwmlscriptlisteneri.h"
#include "nw_markup_numbercollectori.h"


#include "nw_wml1x_wml1xcontenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
extern const NW_WmlsCh_IWmlBrowserLib_Class_t NW_Wml1x_ContentHandler_IWmlBrowserLib_Class;
extern const NW_WmlsCh_IWmlScriptListener_Class_t NW_Wml1x_ContentHandler_IWmlScriptListener_Class;
extern const NW_Markup_NumberCollector_Class_t NW_Wml1x_ContentHandler_NumberCollector_Class;
extern const NW_Object_Class_t* const _NW_Wml1x_ContentHandler_InterfaceList[];

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp);

extern
void
_NW_Wml1x_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_Cancel (NW_HED_DocumentNode_t* documentNode, 
				 NW_HED_CancelType_t cancelType);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_Initialize (NW_HED_DocumentNode_t* documentNode,
                                     TBrowserStatusCode aInitStatus);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                     NW_LMgr_Box_t** boxTree);

extern
void
_NW_Wml1x_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree);

extern
void
_NW_Wml1x_ContentHandler_Resume (NW_HED_DocumentNode_t* documentNode);

extern
void
_NW_Wml1x_ContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IntraPageNavigationCompleted (NW_HED_DocumentNode_t* documentNode);

extern
void
_NW_Wml1x_ContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode);

extern
void
_NW_Wml1x_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode);

extern
const NW_Text_t*
_NW_Wml1x_ContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_HandleRequest (NW_HED_ContentHandler_t* thisObj,
                                        NW_HED_UrlRequest_t* urlRequest);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_FeatureQuery (NW_HED_ContentHandler_t* thisObj,
                                       NW_HED_ContentHandler_Feature_t featureId);

extern 
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Init (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib);

extern 
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_GetVar (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib,
                                                const NW_Ucs2 *var, NW_Ucs2 **ret_string);

extern 
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_SetVar (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib,
                                                const NW_Ucs2 *var, const NW_Ucs2 *value);

extern 
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Go (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib,
                                            const NW_Ucs2 *url, const NW_Ucs2 *param,
                                            NW_Http_Header_t *header);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Prev (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_NewContext (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlBrowserLib_Refresh (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowserLib);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlScriptListener_Start (NW_WmlsCh_IWmlScriptListener_t *wmlScrListener, 
                                                   const NW_Ucs2 *url);

extern
TBrowserStatusCode
_NW_Wml1x_ContentHandler_IWmlScriptListener_Finish (NW_WmlsCh_IWmlScriptListener_t *wmlScrListener,
                                                    const TBrowserStatusCode status, const NW_Ucs2 *message);

extern
NW_LMgr_Box_t*
_NW_Wml1x_ContentHandler_NumberCollector_GetBoxTree (NW_Markup_NumberCollector_t* numberCollector);

extern
NW_Text_t*
_NW_Wml1x_ContentHandler_NumberCollector_GetHrefAttr (NW_Markup_NumberCollector_t* numberCollector,
                                                     NW_LMgr_Box_t* box);
extern
NW_Text_t*
_NW_Wml1x_ContentHandler_NumberCollector_GetAltAttr (NW_Markup_NumberCollector_t* numberCollector,
                                                     NW_LMgr_Box_t* box);

extern
TBrowserStatusCode
HandleInvalidImage( NW_Wml1x_ContentHandler_t  *thisObj,
                                                     NW_HED_DocumentNode_t* documentNode,
                                                     NW_HED_DocumentNode_t* childNode,
                                                     NW_Url_Resp_t* response,
                                                     void* pElId);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_CONTENTHANDLERI_H */
