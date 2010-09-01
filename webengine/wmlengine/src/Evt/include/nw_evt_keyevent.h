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


#ifndef NW_EVT_KEYEVENT_H
#define NW_EVT_KEYEVENT_H

#include "nw_evt_event.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_KeyEvent_Class_s NW_Evt_KeyEvent_Class_t;
typedef struct NW_Evt_KeyEvent_s NW_Evt_KeyEvent_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
/*typedef enum NW_Evt_KeyType_e {
  NW_Evt_KeyType_0,
  NW_Evt_KeyType_1,
  NW_Evt_KeyType_2,
  NW_Evt_KeyType_3,
  NW_Evt_KeyType_4,
  NW_Evt_KeyType_5,
  NW_Evt_KeyType_6,
  NW_Evt_KeyType_7,
  NW_Evt_KeyType_8,
  NW_Evt_KeyType_9,
  NW_Evt_KeyType_Star,
  NW_Evt_KeyType_Hash,
  NW_Evt_KeyType_Send,
  NW_Evt_KeyType_End,
  NW_Evt_KeyType_Left,
  NW_Evt_KeyType_Right,
  NW_Evt_KeyType_Up,
  NW_Evt_KeyType_Down,
  NW_Evt_KeyType_Shift,
  NW_Evt_KeyType_CapsLock,
  NW_Evt_KeyType_NumLock,
  NW_Evt_KeyType_SoftA,
  NW_Evt_KeyType_SoftB,
  NW_Evt_KeyType_SoftC
} NW_Evt_KeyType_t;*/

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_KeyEvent_ClassPart_s {
  void** unused;
} NW_Evt_KeyEvent_ClassPart_t;

struct NW_Evt_KeyEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_KeyEvent_ClassPart_t NW_Evt_KeyEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_KeyEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Bool longPress;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_KeyEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_KeyEvent))

#define NW_Evt_KeyEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_KeyEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_KeyEvent_Class_t NW_Evt_KeyEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_KeyEvent_IsLongPress(_object) \
  (NW_Evt_KeyEventOf (_object)->longPress)

#define NW_Evt_KeyEvent_SetLongPress(_object) \
  (NW_Evt_KeyEventOf (_object)->longPress = NW_TRUE)


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_KEYEVENT_H */
