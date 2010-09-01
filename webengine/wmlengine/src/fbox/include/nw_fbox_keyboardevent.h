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


#ifndef NW_FBOX_KEYBOARDEVENT_H
#define NW_FBOX_KEYBOARDEVENT_H

#include "nw_evt_event.h"
#include "NW_FBox_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_KeyBoardEvent_Class_s NW_FBox_KeyBoardEvent_Class_t;
typedef struct NW_FBox_KeyBoardEvent_s NW_FBox_KeyBoardEvent_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_FBox_KeyBoardEvent_KeyType_e {
  NW_FBox_KeyBoardEvent_KeyType_CharKeySet,
  NW_FBox_KeyBoardEvent_KeyType_ExtendedKeySet
}NW_FBox_KeyBoardEvent_KeyType_t;


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_KeyBoardEvent_ClassPart_s {
  void** unused;
} NW_FBox_KeyBoardEvent_ClassPart_t;

struct NW_FBox_KeyBoardEvent_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Evt_Event_ClassPart_t NW_Evt_Event;
  NW_FBox_KeyBoardEvent_ClassPart_t NW_FBox_KeyBoardEvent;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_FBox_KeyBoardEvent_s {
  NW_Evt_Event_t super;

  /* member variables */
  NW_Ucs2 c;
  NW_FBox_KeyBoardEvent_KeyType_t keyType;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_KeyBoardEvent_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_KeyBoardEvent))

#define NW_FBox_KeyBoardEventOf(_object) \
  (NW_Object_Cast (_object, NW_FBox_KeyBoardEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_KeyBoardEvent_Class_t NW_FBox_KeyBoardEvent_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_FBox_KeyBoardEvent_GetChar(_object) \
  (NW_FBox_KeyBoardEventOf (_object)->c)

#define NW_FBox_KeyBoardEvent_GetKeyType(_object) \
  ((const NW_FBox_KeyBoardEvent_KeyType_t) NW_FBox_KeyBoardEventOf (_object)->keyType)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_KeyBoardEvent_Initialize (NW_FBox_KeyBoardEvent_t* event,
                             NW_Ucs2 c, NW_FBox_KeyBoardEvent_KeyType_t keyType);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_KEYBOARDEVENT_H */
