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


#ifndef NW_EVT_POINTEREVENT_H
#define NW_EVT_POINTEREVENT_H

#include "nw_evt_event.h"
#include "nw_gdi_types.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_PointerEvent_Class_s NW_Evt_PointerEvent_Class_t;
typedef struct NW_Evt_PointerEvent_s NW_Evt_PointerEvent_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_Evt_PointerEvent_Type_e {
  NW_Evt_PointerEvent_Type_Up,
  NW_Evt_PointerEvent_Type_Down,
  NW_Evt_PointerEvent_Type_Move
} NW_Evt_PointerEvent_Type_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_PointerEvent_ClassPart_s {
  void** unused;
} NW_Evt_PointerEvent_ClassPart_t;

struct NW_Evt_PointerEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_PointerEvent_ClassPart_t NW_Evt_PointerEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_PointerEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Evt_PointerEvent_Type_t type;
  NW_GDI_Point2D_t location;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_PointerEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_PointerEvent))

#define NW_Evt_PointerEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_PointerEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_PointerEvent_Class_t NW_Evt_PointerEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_PointerEvent_GetType(_object) \
  (NW_OBJECT_CONSTCAST(NW_Evt_PointerEvent_Type_t) NW_Evt_PointerEventOf (_object)->type)

#define NW_Evt_PointerEvent_GetLoc(_object) \
  (NW_OBJECT_CONSTCAST(NW_GDI_Point2D_t*) &NW_Evt_PointerEventOf (_object)->location)

#define NW_Evt_PointerEvent_GetXLoc(_object) \
  (NW_OBJECT_CONSTCAST(NW_GDI_Metric_t) NW_Evt_PointerEventOf (_object)->location.x)

#define NW_Evt_PointerEvent_GetYLoc(_object) \
  (NW_OBJECT_CONSTCAST(NW_GDI_Metric_t) NW_Evt_PointerEventOf (_object)->location.y)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT
NW_Evt_PointerEvent_t*
NW_Evt_PointerEvent_New (NW_Evt_PointerEvent_Type_t type, 
                             NW_GDI_Metric_t x,
                             NW_GDI_Metric_t y);

NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_PointerEvent_Initialize (NW_Evt_PointerEvent_t* event,
                                    NW_Evt_PointerEvent_Type_t type, 
                                    NW_GDI_Metric_t x,
                                    NW_GDI_Metric_t y);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_POINTEREVENT_H */

