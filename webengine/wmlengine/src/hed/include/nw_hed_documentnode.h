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


#ifndef NW_HED_DOCUMENTNODE_H
#define NW_HED_DOCUMENTNODE_H

#include "nw_object_dynamic.h"

#include "nw_lmgr_box.h"
#include "nw_evt_event.h"
#include "nw_hed_urlrequest.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
class MHEDLoadObserver;

typedef struct NW_HED_DocumentNode_Class_s NW_HED_DocumentNode_Class_t;
typedef struct NW_HED_DocumentNode_s NW_HED_DocumentNode_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_HED_CancelType_e {
  NW_HED_CancelType_User,
  NW_HED_CancelType_NewRequest,
  NW_HED_CancelType_Error
} NW_HED_CancelType_t;

typedef struct NW_HED_DocumentRoot_LoadContext_s {
  NW_HED_DocumentNode_t* owner;
  MHEDLoadObserver* loadObserver;
  void* clientData;
  NW_Uint16 transactionId;
  NW_Bool virtualRequest;
} NW_HED_DocumentRoot_LoadContext_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_DocumentNode_Cancel_t) (NW_HED_DocumentNode_t* documentNode, 
				 NW_HED_CancelType_t cancelType);

typedef
TBrowserStatusCode
(*NW_HED_DocumentNode_PartialLoadCallback_t) (NW_HED_DocumentNode_t* documentNode,
                                              TBrowserStatusCode status,
                                              NW_Int32 chunkIndex,
                                              NW_HED_UrlRequest_t* urlRequest,
                                              NW_Url_Resp_t* response,
                                              NW_Uint16 transactionId,
				                                      void* context);

typedef
TBrowserStatusCode
(*NW_HED_DocumentNode_Initialize_t) (NW_HED_DocumentNode_t* documentNode,
                                     TBrowserStatusCode aInitStatus);


typedef
TBrowserStatusCode
(*NW_HED_DocumentNode_NodeChanged_t) (NW_HED_DocumentNode_t* documentNode,
                                      NW_HED_DocumentNode_t* childNode);

typedef
TBrowserStatusCode
(*NW_HED_DocumentNode_GetBoxTree_t) (NW_HED_DocumentNode_t* documentNode,
                                     NW_LMgr_Box_t** boxTree);

typedef
NW_Uint8
(*NW_HED_DocumentNode_ProcessEvent_t) (NW_HED_DocumentNode_t* documentNode,
                                       NW_LMgr_Box_t* box,
                                       NW_Evt_Event_t* event,
                                       void* context);

typedef
void
(*NW_HED_DocumentNode_HandleError_t) (NW_HED_DocumentNode_t* documentNode,
                                      const NW_HED_UrlRequest_t *urlRequest,
                                      NW_Int16 errorClass,
                                      NW_WaeError_t error);

typedef
void
(*NW_HED_DocumentNode_Suspend_t) (NW_HED_DocumentNode_t* documentNode, 
                                  NW_Bool aDestroyBoxTree);

typedef
void
(*NW_HED_DocumentNode_Resume_t) (NW_HED_DocumentNode_t* documentNode);

typedef
void
(*NW_HED_DocumentNode_AllLoadsCompleted_t) (NW_HED_DocumentNode_t* documentNode);

typedef
TBrowserStatusCode
(*NW_HED_DocumentNode_IntraPageNavigationCompleted_t) (NW_HED_DocumentNode_t* documentNode);

typedef
void
(*NW_HED_DocumentNode_LoseFocus_t) (NW_HED_DocumentNode_t* documentNode);

typedef
void
(*NW_HED_DocumentNode_GainFocus_t) (NW_HED_DocumentNode_t* documentNode);


typedef
void
(*NW_HED_DocumentNode_HandleLoadComplete_t) (NW_HED_DocumentNode_t* documentNode,
                                      const NW_HED_UrlRequest_t *urlRequest,
                                      NW_Int16 errorClass,
                                      TBrowserStatusCode error,
                                      NW_Uint16 transactionId,
                                      void* context );

/* ------------------------------------------------------------------------- *
   object class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_DocumentNode_ClassPart_s {
  NW_HED_DocumentNode_Cancel_t cancel;
  NW_HED_DocumentNode_PartialLoadCallback_t partialLoadCallback;
  NW_HED_DocumentNode_Initialize_t initialize;
  NW_HED_DocumentNode_NodeChanged_t nodeChanged;
  NW_HED_DocumentNode_GetBoxTree_t getBoxTree;
  NW_HED_DocumentNode_ProcessEvent_t processEvent;
  NW_HED_DocumentNode_HandleError_t handleError;
  NW_HED_DocumentNode_Suspend_t suspend;
  NW_HED_DocumentNode_Resume_t resume;
  NW_HED_DocumentNode_AllLoadsCompleted_t allLoadsCompleted;
  NW_HED_DocumentNode_IntraPageNavigationCompleted_t intraPageNavigationCompleted;
  NW_HED_DocumentNode_LoseFocus_t loseFocus;
  NW_HED_DocumentNode_GainFocus_t gainFocus;
  NW_HED_DocumentNode_HandleLoadComplete_t handleLoadComplete;
} NW_HED_DocumentNode_ClassPart_t;

struct NW_HED_DocumentNode_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
};

/* ------------------------------------------------------------------------- *
   object instance definition
 * ------------------------------------------------------------------------- */
struct NW_HED_DocumentNode_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_HED_DocumentNode_t* parent;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_DocumentNode_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_DocumentNode))

#define NW_HED_DocumentNodeOf(_object) \
  (NW_Object_Cast (_object, NW_HED_DocumentNode))

/* ------------------------------------------------------------------------- *
   public static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_DocumentNode_Class_t NW_HED_DocumentNode_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_DocumentNode_t*
_NW_HED_DocumentNode_GetRootNode (NW_HED_DocumentNode_t* documentNode);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_DocumentNode_Cancel(_documentNode, _cancelType) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, cancel) \
   (NW_HED_DocumentNodeOf (_documentNode), (_cancelType)))

#define NW_HED_DocumentNode_PartialLoadCallback(_documentNode, _status, _chunkIndex, _request, _response, _transactionId, _context) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, partialLoadCallback) \
   (NW_HED_DocumentNodeOf (_documentNode), \
    (_status), (_chunkIndex), (_request), (_response), (_transactionId), (_context)))

#define NW_HED_DocumentNode_Initialize(_documentNode, _initStatus) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, initialize) \
   (NW_HED_DocumentNodeOf (_documentNode), (_initStatus)))

#define NW_HED_DocumentNode_NodeChanged(_documentNode, _childNode) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, nodeChanged) \
   (NW_HED_DocumentNodeOf (_documentNode), NW_HED_DocumentNodeOf (_childNode)))

#define NW_HED_DocumentNode_GetBoxTree(_documentNode, _boxTree) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, getBoxTree) \
   (NW_HED_DocumentNodeOf (_documentNode), (_boxTree)))

#define NW_HED_DocumentNode_ProcessEvent(_documentNode, _activeBox, _event, _context) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, processEvent) \
   (NW_HED_DocumentNodeOf (_documentNode), NW_LMgr_BoxOf (_activeBox), \
    NW_Evt_EventOf (_event), (_context)))

#define NW_HED_DocumentNode_HandleError(_documentNode, _urlRequest, _errorClass, _error) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, handleError) \
   (NW_HED_DocumentNodeOf (_documentNode), (_urlRequest), (_errorClass), (_error)))

#define NW_HED_DocumentNode_Suspend(_documentNode, _destroyBoxTree) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, suspend) \
   (NW_HED_DocumentNodeOf (_documentNode), (_destroyBoxTree)))

#define NW_HED_DocumentNode_Resume(_documentNode) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, resume) \
   (NW_HED_DocumentNodeOf (_documentNode)))

#define NW_HED_DocumentNode_AllLoadsCompleted(_documentNode) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, allLoadsCompleted) \
   (NW_HED_DocumentNodeOf (_documentNode)))

#define NW_HED_DocumentNode_IntraPageNavigationCompleted(_documentNode) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, intraPageNavigationCompleted) \
   (NW_HED_DocumentNodeOf (_documentNode)))

#define NW_HED_DocumentNode_LoseFocus(_documentNode) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, loseFocus) \
   (NW_HED_DocumentNodeOf (_documentNode)))

#define NW_HED_DocumentNode_GainFocus(_documentNode) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, gainFocus) \
   (NW_HED_DocumentNodeOf (_documentNode)))

#define NW_HED_DocumentNode_GetRootNode(_documentNode) \
  (_NW_HED_DocumentNode_GetRootNode (NW_HED_DocumentNodeOf (_documentNode)))

#define NW_HED_DocumentNode_GetParentNode(_documentNode) \
  (NW_OBJECT_CONSTCAST(NW_HED_DocumentNode_t*) NW_HED_DocumentNodeOf (_documentNode)->parent)

#define NW_HED_DocumentNode_HandleLoadComplete(_documentNode, _urlRequest, _errorClass, _error, _transactionId, _context) \
  (NW_Object_Invoke (_documentNode, NW_HED_DocumentNode, handleLoadComplete) \
   (NW_HED_DocumentNodeOf (_documentNode), (_urlRequest), (_errorClass), (_error), (_transactionId), (_context) ))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_DOCUMENTNODE_H */
