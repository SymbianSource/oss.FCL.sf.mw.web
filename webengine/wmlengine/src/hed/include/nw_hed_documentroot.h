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


#ifndef NW_HED_DOCUMENTROOT_H
#define NW_HED_DOCUMENTROOT_H

#include "nw_hed_documentnode.h"
#include "nw_hed_icompositenode.h"
#include "nw_hed_iloadrecipient.h"
#include "nw_hed_iloaderlistener.h"

#include "nw_hed_mimetable.h"
#include "nw_hed_appservices.h"
#include "nw_hed_historystack.h"
#include "nw_hed_loader.h"
#include "nw_hed_context.h"
#include "nw_hed_urlrequest.h"
#include "nw_adt_map.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- */
class MHEDLoadObserver;
class CWmlControl;
/* ------------------------------------------------------------------------- */
/* 
** TODO these should be moved to an external file.  The main reason is that
** DocumentRoot should remain pure, in that it doesn't have knowledge about
** specific content handler issues -- such as the variable contexts it may
** define.
*/
#define NW_HED_DocumentRoot_ContextPickId 0
#define NW_HED_DocumentRoot_ContextGlobalId 1
#define NW_Wml1x_ContentHandler_ContextTaskId 2

/* ------------------------------------------------------------------------- */
typedef struct NW_HED_DocumentRoot_Class_s NW_HED_DocumentRoot_Class_t;
typedef struct NW_HED_DocumentRoot_s NW_HED_DocumentRoot_t;
class MHEDDocumentListener;

/* ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_DocumentRoot_DocumentDisplayed_t) (NW_HED_DocumentRoot_t* thisObj);

/* ------------------------------------------------------------------------- */
typedef struct NW_HED_DocumentRoot_ClassPart_s {
  NW_HED_DocumentRoot_DocumentDisplayed_t documentDisplayed;
} NW_HED_DocumentRoot_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_HED_DocumentRoot_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_DocumentRoot_ClassPart_t NW_HED_DocumentRoot;
};

/* ------------------------------------------------------------------------- */
struct NW_HED_DocumentRoot_s {
  NW_HED_DocumentNode_t super;

  /* interface implementations */
  NW_HED_ICompositeNode_t NW_HED_ICompositeNode;
  NW_HED_ILoadRecipient_t NW_HED_ILoadRecipient;
  NW_HED_ILoaderListener_t NW_HED_ILoaderListener;

  /* member variables */
  MHEDDocumentListener* documentListener;

  NW_HED_DocumentNode_t* childNode;

  /*need for cancel mechanism*/
  NW_HED_UrlRequest_t* nextUrlRequestToLoad;

  NW_HED_Loader_t* loader;
  NW_ADT_DynamicVector_t* loadList;

  NW_HED_HistoryStack_t* historyStack;
  NW_ADT_Map_t* contextMap;
  NW_HED_MimeTable_t* mimeTable;
  NW_HED_AppServices_t* appServices;
  void* browserApp_Ctx;

  NW_Bool isLoading;
  NW_Int16 isIntraPageNav;
  NW_Bool isCancelling;
  NW_Bool isTopLevelRequest;
  NW_Int32 chunkIndex;
  
  NW_Int16 documentErrorClass;
  NW_WaeError_t documentError;
  void* certInfo;

//  void* ecmaHost;

  // referrer url that points to prev document url , 
  // if accessed from a link
  NW_Text_t* referrerUrl;
};

/* ------------------------------------------------------------------------- */
#define NW_HED_DocumentRoot_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_DocumentRoot))

#define NW_HED_DocumentRootOf(_object) \
  (NW_Object_Cast (_object, NW_HED_DocumentRoot))

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_DocumentRoot_Class_t NW_HED_DocumentRoot_Class;

/* ------------------------------------------------------------------------- *
   public method prototypes
 * ------------------------------------------------------------------------- */


NW_HED_EXPORT
void*
NW_HED_DocumentRoot_GetCertInfo (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
const NW_Text_t*
NW_HED_DocumentRoot_GetTitle (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
const NW_Text_t*
NW_HED_DocumentRoot_GetDocumentReferrer(NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
void
NW_HED_DocumentRoot_SetDocumentReferrer(NW_HED_DocumentRoot_t* thisObj, 
                                        const NW_Text_t* referrerUrl);



NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_StartRequest (NW_HED_DocumentRoot_t* thisObj,
                                  NW_HED_DocumentNode_t* owner,
                                  const NW_HED_UrlRequest_t* urlRequest,
                                  void* clientData );

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_StartRequest_tId2 (NW_HED_DocumentRoot_t* thisObj,
                                       const NW_HED_UrlRequest_t* aUrlRequest,
                                       void* aLoadRecipient,
                                       void* aClientData,
                                       NW_Uint16* aTransactionId, 
                                       void* aLoadContext = NULL);


NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_StartRequest_tId (NW_HED_DocumentRoot_t* thisObj,
                                  NW_HED_DocumentNode_t* owner,
                                  const NW_HED_UrlRequest_t* urlRequest,
                                  void* clientData ,
                                  NW_Uint16* aTransactionId,
                                  void* aLoadContext = NULL);


NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_StartLoad (NW_HED_DocumentRoot_t* thisObj,
                               NW_HED_DocumentNode_t* owner,
                               const NW_Text_t* url,
                               NW_Uint8 reason,
                               void* clientData,
                               NW_Uint8 loadType,
							   NW_Cache_Mode_t cacheMode);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_HistoryLoad (NW_HED_DocumentRoot_t* thisObj,
                                 NW_HED_HistoryStack_Direction_t direction);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_SetLoadObserver (NW_HED_DocumentRoot_t* thisObj,
                                     NW_Uint16 aTransationId,
                                     MHEDLoadObserver* aLoadObserver);

NW_HED_EXPORT
void
_NW_HED_DocumentRoot_DeleteRequests (NW_HED_DocumentRoot_t* thisObj,
                                     NW_HED_DocumentNode_t* childNode);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_HandleIntraPageNavigationStarted (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_HandleIntraPageNavigationCompleted (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
const NW_Text_t*
NW_HED_DocumentRoot_GetURL (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
const NW_HED_UrlRequest_t*
NW_HED_DocumentRoot_GetUrlRequest (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_AddContext (NW_HED_DocumentRoot_t* thisObj,
                                NW_HED_Context_t* context,
                                NW_Int8 *contextId);
NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_RemoveContext (NW_HED_DocumentRoot_t* thisObj,
                                   NW_Int8 contextId);
NW_HED_EXPORT
NW_HED_Context_t*
NW_HED_DocumentRoot_GetContext (NW_HED_DocumentRoot_t* thisObj,
                                NW_Int8 contextId);
NW_HED_EXPORT
TBrowserStatusCode
NW_HED_DocumentRoot_SetDocumentListener (NW_HED_DocumentRoot_t* thisObj,
                                         MHEDDocumentListener* documentListener);
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_CollectNumbers (NW_HED_DocumentRoot_t* documentRoot,
                                     NW_ADT_DynamicVector_t* dynamicVector);

NW_HED_EXPORT
NW_Bool
_NW_HED_DocumentRoot_ECMLExists (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_FillAll (NW_HED_DocumentRoot_t* thisObj, NW_LMgr_Box_t** firstNotFilled);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_FillOne (NW_HED_DocumentRoot_t* thisObj, NW_Ucs2* value, NW_LMgr_Box_t* node);

NW_HED_EXPORT
NW_HED_DocumentRoot_t*
NW_HED_DocumentRoot_New (NW_HED_MimeTable_t* mimeTable,
                         NW_HED_AppServices_t* appServices,
                         void* browserAppCtx);
                         
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_ShowImages(NW_HED_DocumentRoot_t *thisObj);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_DocumentRoot_GetViewImageList(NW_HED_DocumentRoot_t *thisObj, NW_ADT_DynamicVector_t* dynamicVector);

NW_HED_EXPORT
NW_Bool
NW_HED_DocumentRoot_IsContextSwitchPending (NW_HED_DocumentRoot_t* thisObj);

NW_HED_EXPORT
void
NW_HED_DocumentRoot_SetHistoryControl(NW_HED_DocumentRoot_t* thisObj, CWmlControl* aWmlControl);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_DocumentRoot_GetAppServices(_documentRoot) \
  (NW_OBJECT_CONSTCAST(void*) \
   NW_HED_DocumentRootOf (_documentRoot)->appServices)

#define NW_HED_DocumentRoot_GetBrowserAppCtx(_documentRoot) \
  (NW_OBJECT_CONSTCAST(NW_HED_AppServices_t*) \
   NW_HED_DocumentRootOf (_documentRoot)->browserApp_Ctx)

#define NW_HED_DocumentRoot_GetChildNode(_documentRoot) \
  (NW_OBJECT_CONSTCAST(NW_HED_DocumentNode_t*) \
   NW_HED_DocumentRootOf (_documentRoot)->childNode)

#define NW_HED_DocumentRoot_GetDocumentListener(_documentRoot) \
   (NW_HED_DocumentRootOf (_documentRoot)->documentListener)

#define NW_HED_DocumentRoot_GetLoader(_documentRoot) \
  (NW_OBJECT_CONSTCAST(NW_HED_Loader_t*) \
   NW_HED_DocumentRootOf(_documentRoot)->loader)

#define NW_HED_DocumentRoot_GetHistoryStack(_documentRoot) \
  (NW_OBJECT_CONSTCAST(NW_HED_HistoryStack_t*) \
   NW_HED_HistoryStackOf((_documentRoot)->historyStack))

#define NW_HED_DocumentRoot_IsLoading(_documentRoot) \
  (NW_OBJECT_CONSTCAST(NW_Bool) \
   NW_HED_DocumentRootOf(_documentRoot)->isLoading)

#define NW_HED_DocumentRoot_IsIntraPageNav(_documentRoot) \
  (NW_OBJECT_CONSTCAST(NW_Bool) \
   NW_HED_DocumentRootOf(_documentRoot)->isIntraPageNav)

#define NW_HED_DocumentRoot_DeleteRequests(_documentRoot, _childNode) \
  (_NW_HED_DocumentRoot_DeleteRequests ( \
     NW_HED_DocumentRootOf (_documentRoot), \
     NW_HED_DocumentNodeOf (_childNode)))

#define NW_HED_DocumentRoot_CollectNumbers(_documentRoot, _dynamicVector) \
  (_NW_HED_DocumentRoot_CollectNumbers ( \
       NW_HED_DocumentRootOf (_documentRoot), \
       NW_ADT_DynamicVectorOf (_dynamicVector)))

#define NW_HED_DocumentRoot_ECMLExists(_documentRoot) \
  (_NW_HED_DocumentRoot_ECMLExists ( \
       NW_HED_DocumentRootOf (_documentRoot)))

#define NW_HED_DocumentRoot_FillAll(_documentRoot, _firstNotFilled) \
  (_NW_HED_DocumentRoot_FillAll ( \
       NW_HED_DocumentRootOf (_documentRoot), \
       (_firstNotFilled)))

#define NW_HED_DocumentRoot_FillOne(_documentRoot, _value, _node) \
  (_NW_HED_DocumentRoot_FillOne ( \
       NW_HED_DocumentRootOf (_documentRoot), \
       (_value), \
       (_node)))

#define NW_HED_DocumentRoot_GetViewImageList(_documentRoot, _dynamicVector) \
  (_NW_HED_DocumentRoot_GetViewImageList ( \
       NW_HED_DocumentRootOf (_documentRoot), \
       NW_ADT_DynamicVectorOf (_dynamicVector)))

#define NW_HED_DocumentRoot_ShowImages(_documentRoot) \
  (_NW_HED_DocumentRoot_ShowImages ( \
     NW_HED_DocumentRootOf (_documentRoot)))

#define NW_HED_DocumentRoot_GetRootBox(_documentRoot) \
  (NW_HED_DocumentRootOf(_documentRoot)->documentListener->GetRootBox())

/* ------------------------------------------------------------------------- */
#define NW_HED_DocumentRoot_DocumentDisplayed(_object) \
  (NW_Object_Invoke (_object, NW_HED_DocumentRoot, documentDisplayed) \
   (NW_HED_DocumentRootOf (_object)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_DOCUMENTROOT_H */
