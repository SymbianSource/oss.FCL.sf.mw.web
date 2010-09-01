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


#ifndef NW_EVT_FONTSIZECHANGEEVENT_H
#define NW_EVT_FONTSIZECHANGEEVENT_H

#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_FontSizeChangeEvent_Class_s NW_Evt_FontSizeChangeEvent_Class_t;
typedef struct NW_Evt_FontSizeChangeEvent_s NW_Evt_FontSizeChangeEvent_t;

typedef enum NW_Evt_FontSizeChangeEvent_FontLevel_e
{
  NW_Evt_FontSizeChangeEvent_FontLevel_AllSmall,
  NW_Evt_FontSizeChangeEvent_FontLevel_Smaller,
  NW_Evt_FontSizeChangeEvent_FontLevel_Normal,
  NW_Evt_FontSizeChangeEvent_FontLevel_Larger,    
  NW_Evt_FontSizeChangeEvent_FontLevel_AllLarge
}NW_Evt_FontSizeChangeEvent_FontLevel_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_FontSizeChangeEvent_ClassPart_s {
  void** unused;
} NW_Evt_FontSizeChangeEvent_ClassPart_t;

struct NW_Evt_FontSizeChangeEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_Evt_FontSizeChangeEvent_ClassPart_t NW_Evt_FontSizeChangeEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Evt_FontSizeChangeEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Evt_FontSizeChangeEvent_FontLevel_t fontLevel;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_FontSizeChangeEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Evt_FontSizeChangeEvent))

#define NW_Evt_FontSizeChangeEventOf(_object) \
  (NW_Object_Cast (_object, NW_Evt_FontSizeChangeEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_FontSizeChangeEvent_Class_t NW_Evt_FontSizeChangeEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Evt_FontSizeChangeEvent_GetLevel(_object) \
  ((const NW_Evt_FontSizeChangeEvent_FontLevel_t) NW_Evt_FontSizeChangeEventOf (_object)->fontLevel)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* takes one of TGDIDeviceContextFontSizeLevel as input parameter */
NW_Evt_FontSizeChangeEvent_t*
NW_Evt_FontSizeChangeEvent_New (NW_Evt_FontSizeChangeEvent_FontLevel_t fontLevel);

NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_FontSizeChangeEvent_Initialize (NW_Evt_FontSizeChangeEvent_t* event,
                             NW_Evt_FontSizeChangeEvent_FontLevel_t fontLevel);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_EVT_FONTSIZECHANGEEVENT_H */
