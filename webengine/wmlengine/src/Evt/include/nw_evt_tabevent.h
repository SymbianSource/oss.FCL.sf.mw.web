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


#ifndef NW_EVT_TABEVENT_H
#define NW_EVT_TABEVENT_H

#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_TabEvent_Class_s NW_Evt_TabEvent_Class_t;
typedef struct NW_Evt_TabEvent_s NW_Evt_TabEvent_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_Evt_TabEvent_Direction_e {
  NW_Evt_TabEvent_Direction_Left,
  NW_Evt_TabEvent_Direction_Right,
  NW_Evt_TabEvent_Direction_Up,
  NW_Evt_TabEvent_Direction_Down
} NW_Evt_TabEvent_Direction_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_TabEvent_ClassPart_s {
  void** unused;
} NW_Evt_TabEvent_ClassPart_t;

struct NW_Evt_TabEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_TabEvent_ClassPart_t NW_Evt_TabEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_TabEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Evt_TabEvent_Direction_t direction;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_TabEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_TabEvent))

#define NW_Evt_TabEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_TabEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_TabEvent_Class_t NW_Evt_TabEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_TabEvent_GetDirection(_object) \
  (NW_OBJECT_CONSTCAST(NW_Evt_TabEvent_Direction_t) NW_Evt_TabEventOf (_object)->direction)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_Evt_TabEvent_t*
NW_Evt_TabEvent_New (NW_Evt_TabEvent_Direction_t direction);

NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_TabEvent_Initialize (NW_Evt_TabEvent_t* event,
                                NW_Evt_TabEvent_Direction_t direction);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_TABEVENT_H */
