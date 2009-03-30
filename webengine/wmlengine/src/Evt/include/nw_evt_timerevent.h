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


#ifndef NW_EVT_TIMEREVENT_H
#define NW_EVT_TIMEREVENT_H

#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_TimerEvent_Class_s NW_Evt_TimerEvent_Class_t;
typedef struct NW_Evt_TimerEvent_s NW_Evt_TimerEvent_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_Evt_TimerType_e {
  NW_Evt_TimerType_Expired
} NW_Evt_TimerType_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_TimerEvent_ClassPart_s {
  void** unused;
} NW_Evt_TimerEvent_ClassPart_t;

struct NW_Evt_TimerEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_TimerEvent_ClassPart_t NW_Evt_TimerEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_TimerEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Evt_TimerType_t TimerType;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_TimerEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_TimerEvent))

#define NW_Evt_TimerEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_TimerEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_TimerEvent_Class_t NW_Evt_TimerEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_TimerEvent_GetTimerType(_object) \
  ((const NW_Evt_TimerType_t) NW_Evt_TimerEventOf (_object)->TimerType)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_TimerEvent_Initialize (NW_Evt_TimerEvent_t* event,
                             NW_Evt_TimerType_t TimerType);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_TIMEREVENT_H */
