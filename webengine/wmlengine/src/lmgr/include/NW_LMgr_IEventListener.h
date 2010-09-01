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


#ifndef NW_LMgr_IEventListener_h
#define NW_LMgr_IEventListener_h

#include "nw_object_interface.h"
#include "nw_evt_event.h"
#include "NW_LMgr_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_IEventListener_Class_s NW_LMgr_IEventListener_Class_t;
typedef struct NW_LMgr_IEventListener_s NW_LMgr_IEventListener_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Uint8
(*NW_LMgr_IEventListener_ProcessEvent_t) (NW_LMgr_IEventListener_t* eventListener,
				          NW_Evt_Event_t* event);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_IEventListener_ClassPart_s {
  NW_LMgr_IEventListener_ProcessEvent_t processEvent;
} NW_LMgr_IEventListener_ClassPart_t;
  
struct NW_LMgr_IEventListener_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_LMgr_IEventListener_ClassPart_t NW_LMgr_IEventListener;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_LMgr_IEventListener_s {
  NW_Object_Interface_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_IEventListener_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_IEventListener))

#define NW_LMgr_IEventListenerOf(object) \
  (NW_Object_Cast (object, NW_LMgr_IEventListener))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_IEventListener_Class_t NW_LMgr_IEventListener_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_IEventListener_ProcessEvent(_eventListener, _event) \
  (NW_Object_Invoke(_eventListener, NW_LMgr_IEventListener, processEvent) \
    (NW_LMgr_IEventListenerOf (_eventListener), NW_Evt_EventOf (_event)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_LMgr_IEventListener_h */
