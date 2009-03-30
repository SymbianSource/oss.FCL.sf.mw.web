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


#ifndef NW_HED_DOCUMENTROOTI_H
#define NW_HED_DOCUMENTROOTI_H

#include "nw_hed_documentnodei.h"
#include "nw_hed_icompositenodei.h"
#include "nw_hed_iloadrecipienti.h"
#include "nw_hed_iloaderlisteneri.h"
#include "nw_hed_documentroot.h"
#include "BrsrStatusCodes.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
//class CWmlControl;

/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ICompositeNode_Class_t NW_HED_DocumentRoot_ICompositeNode_Class;
NW_HED_EXPORT const NW_HED_ILoadRecipient_Class_t NW_HED_DocumentRoot_ILoadRecipient_Class;
NW_HED_EXPORT const NW_HED_ILoaderListener_Class_t NW_HED_DocumentRoot_ILoaderListener_Class;
NW_HED_EXPORT const NW_Object_Class_t* const NW_HED_DocumentRoot_InterfaceList[];

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_Object_t*
_NW_HED_DocumentRoot_QueryInterface (NW_Object_Core_t* core,
                                     const NW_Object_Class_t* objClass);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_Cancel (NW_HED_DocumentNode_t* documentNode,
			     NW_HED_CancelType_t cancelType);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_PartialLoadCallback(NW_HED_DocumentNode_t* documentNode,
                                         TBrowserStatusCode status,
                                         NW_Int32 chunkIndex,
                                         NW_HED_UrlRequest_t* urlRequest,
                                         NW_Url_Resp_t* response,
                                         NW_Uint16 transactionId,
                                         void* context);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_NodeChanged (NW_HED_DocumentNode_t* documentNode,
                                  NW_HED_DocumentNode_t* childNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                 NW_LMgr_Box_t** boxTree);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_HandleError (NW_HED_DocumentNode_t* documentNode,
                                  const NW_HED_UrlRequest_t *urlRequest,
                                  NW_Int16 errorClass,
                                  NW_WaeError_t error);
NW_HED_EXPORT
void
_NW_HED_DocumentRoot_Suspend (NW_HED_DocumentNode_t* documentNode,
                              NW_Bool aDestroyBoxTree);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_Resume (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_LoseFocus (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_GainFocus (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_DocumentDisplayed (NW_HED_DocumentRoot_t* documentNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ICompositeNode_RemoveChild (NW_HED_ICompositeNode_t* icompositeNode,
                                                 NW_HED_DocumentNode_t* childNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ICompositeNode_GetChildren (NW_HED_ICompositeNode_t* icompositeNode,
                                                 NW_ADT_Iterator_t** childIterator);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoadRecipient_ProcessLoad (NW_HED_ILoadRecipient_t* loadRecipient,
                                                 TBrowserStatusCode loadStatus,
                                                 NW_Uint16 transactionId,
                                                 NW_Url_Resp_t* response,
                                                 NW_HED_UrlRequest_t* urlRequest,
                                                 void* clientData);
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad (NW_HED_ILoadRecipient_t* loadRecipient,
                                                        TBrowserStatusCode loadStatus,
                                                        NW_Uint16 transactionId,
                                                        NW_Int32 chunkIndex,
                                                        NW_Url_Resp_t* response,
                                                        NW_HED_UrlRequest_t* urlRequest,
                                                        void* clientData);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_ILoaderListener_IsLoading (NW_HED_ILoaderListener_t* loaderListener,
                                                NW_HED_Loader_t* loader,
												NW_Bool state);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoaderListener_LoadProgressOn (NW_HED_ILoaderListener_t* loaderListener,
                                                     NW_Uint16 transId );

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoaderListener_LoadProgressOff (NW_HED_ILoaderListener_t* loaderListener,
                                                      NW_Uint16 transId );
                                                      
//NW_HED_EXPORT
//void
//NW_HED_DocumentRoot_SetHistoryControl(NW_HED_DocumentRoot_t* thisObj, 
//									  CWmlControl* aWmlControl);

                                                      
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_DOCUMENTROOTI_H */
