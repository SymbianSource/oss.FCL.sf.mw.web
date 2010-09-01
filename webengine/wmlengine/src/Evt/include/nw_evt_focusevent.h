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


#ifndef NW_EVT_FOCUSEVENT_H
#define NW_EVT_FOCUSEVENT_H

#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_FocusEvent_Class_s NW_Evt_FocusEvent_Class_t;
typedef struct NW_Evt_FocusEvent_s NW_Evt_FocusEvent_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_FocusEvent_ClassPart_s {
  void** unused;
} NW_Evt_FocusEvent_ClassPart_t;

struct NW_Evt_FocusEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_FocusEvent_ClassPart_t NW_Evt_FocusEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_FocusEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Bool hasFocus;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_FocusEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_FocusEvent))

#define NW_Evt_FocusEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_FocusEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_FocusEvent_Class_t NW_Evt_FocusEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_FocusEvent_GetHasFocus(_object) \
  (NW_Evt_FocusEventOf (_object)->hasFocus)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_FocusEvent_Initialize (NW_Evt_FocusEvent_t* event,
                               NW_Bool hasFocus);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_FOCUSEVENT_H */
