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


#ifndef NW_EVT_OPENVIEWEREVENT_H
#define NW_EVT_OPENVIEWEREVENT_H

#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_OpenViewerEvent_Class_s NW_Evt_OpenViewerEvent_Class_t;
typedef struct NW_Evt_OpenViewerEvent_s NW_Evt_OpenViewerEvent_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_OpenViewerEvent_ClassPart_s {
  void** unused;
} NW_Evt_OpenViewerEvent_ClassPart_t;

struct NW_Evt_OpenViewerEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_OpenViewerEvent_ClassPart_t NW_Evt_OpenViewerEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_OpenViewerEvent_s {
  NW_Evt_Event_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_OpenViewerEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_OpenViewerEvent))

#define NW_Evt_OpenViewerEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_OpenViewerEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_OpenViewerEvent_Class_t NW_Evt_OpenViewerEvent_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_Evt_OpenViewerEvent_t*
NW_Evt_OpenViewerEvent_New (void);

NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_OpenViewerEvent_Initialize (NW_Evt_OpenViewerEvent_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_OPENVIEWEREVENT_H */
