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


#ifndef NW_XHTML_SAVETOPHONEBKEVENT_H
#define NW_XHTML_SAVETOPHONEBKEVENT_H

#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_SaveToPhoneBkEvent_Class_s NW_XHTML_SaveToPhoneBkEvent_Class_t;
typedef struct NW_XHTML_SaveToPhoneBkEvent_s NW_XHTML_SaveToPhoneBkEvent_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_SaveToPhoneBkEvent_ClassPart_s {
  NW_Uint8 unused;
} NW_XHTML_SaveToPhoneBkEvent_ClassPart_t;

struct NW_XHTML_SaveToPhoneBkEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_XHTML_SaveToPhoneBkEvent_ClassPart_t NW_XHTML_SaveToPhoneBkEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_SaveToPhoneBkEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_SaveToPhoneBkEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_XHTML_SaveToPhoneBkEvent))

#define NW_XHTML_SaveToPhoneBkEventOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_SaveToPhoneBkEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_SaveToPhoneBkEvent_Class_t NW_XHTML_SaveToPhoneBkEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
NW_XHTML_SaveToPhoneBkEvent_Initialize (NW_XHTML_SaveToPhoneBkEvent_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_UNICODEKEYEVENT_H */
