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


#ifndef NW_LMGR_EVENTHANDLER_H
#define NW_LMGR_EVENTHANDLER_H

#include "nw_object_dynamic.h"
#include "nw_evt_event.h"
#include "nw_lmgr_box.h"

#ifdef __cplusplus
extern "C" {
#endif
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_EventHandler_Class_s NW_LMgr_EventHandler_Class_t;
typedef struct NW_LMgr_EventHandler_s NW_LMgr_EventHandler_t;

/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
enum {
  NW_LMgr_EventNotAbsorbed = 0,
  NW_LMgr_EventAbsorbed = 1
};

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Uint8
(*NW_LMgr_EventHandler_ProcessEvent_t) (NW_LMgr_EventHandler_t* eventHandler,
                                        NW_LMgr_Box_t* box,
                                        NW_Evt_Event_t* event);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_EventHandler_ClassPart_s {
  NW_LMgr_EventHandler_ProcessEvent_t processEvent;
} NW_LMgr_EventHandler_ClassPart_t;

struct NW_LMgr_EventHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_EventHandler_ClassPart_t NW_LMgr_EventHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_EventHandler_s {
  NW_Object_Dynamic_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_EventHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_EventHandler))

#define NW_LMgr_EventHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_EventHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_EventHandler_Class_t NW_LMgr_EventHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_EventHandler_ProcessEvent(_eventHandler, _activeBox, _event) \
  (NW_LMgr_EventHandler_GetClassPart (_eventHandler). \
     processEvent ((_eventHandler), (_activeBox), (_event)))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_LMGR_EVENTHANDLER_H */
