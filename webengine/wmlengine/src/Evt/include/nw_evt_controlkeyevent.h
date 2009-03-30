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


#ifndef NW_EVT_CONTROLKEYEVENT_H
#define NW_EVT_CONTROLKEYEVENT_H

#include "nw_evt_keyevent.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_ControlKeyEvent_Class_s NW_Evt_ControlKeyEvent_Class_t;
typedef struct NW_Evt_ControlKeyEvent_s NW_Evt_ControlKeyEvent_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_Evt_ControlKeyType_e {
  NW_Evt_ControlKeyType_None,
  NW_Evt_ControlKeyType_Send,
  NW_Evt_ControlKeyType_Home,
  NW_Evt_ControlKeyType_End,
  NW_Evt_ControlKeyType_Left,
  NW_Evt_ControlKeyType_Right,
  NW_Evt_ControlKeyType_Up,
  NW_Evt_ControlKeyType_Down,
  NW_Evt_ControlKeyType_Shift,
  NW_Evt_ControlKeyType_CapsLock,
  NW_Evt_ControlKeyType_NumLock,
  NW_Evt_ControlKeyType_SoftA,
  NW_Evt_ControlKeyType_SoftB,
  NW_Evt_ControlKeyType_SoftC,
  NW_Evt_ControlKeyType_Clear
} NW_Evt_ControlKeyType_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_ControlKeyEvent_ClassPart_s {
  void** unused;
} NW_Evt_ControlKeyEvent_ClassPart_t;

struct NW_Evt_ControlKeyEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_KeyEvent_ClassPart_t NW_Evt_KeyEvent;
  NW_Evt_ControlKeyEvent_ClassPart_t NW_Evt_ControlKeyEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_ControlKeyEvent_s {
  NW_Evt_KeyEvent_t super;

  /* member variables */
  NW_Evt_ControlKeyType_t keyVal;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_ControlKeyEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_ControlKeyEvent))

#define NW_Evt_ControlKeyEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_ControlKeyEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_ControlKeyEvent_Class_t NW_Evt_ControlKeyEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_ControlKeyEvent_GetKeyVal(_object) \
  (NW_OBJECT_CONSTCAST(NW_Evt_ControlKeyType_t) NW_Evt_ControlKeyEventOf (_object)->keyVal)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_Evt_ControlKeyEvent_t* 
NW_Evt_ControlKeyEvent_New (NW_Evt_ControlKeyType_t keyType);

NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_ControlKeyEvent_Initialize (NW_Evt_ControlKeyEvent_t* event,
                             NW_Evt_ControlKeyType_t keyType);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_CONTROLKEYEVENT_H */
