/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_INTERACTOR_H
#define NW_FBOX_INTERACTOR_H

#include "nw_object_dynamic.h"
#include "nw_lmgr_activebox.h"
#include "nw_evt_event.h"
#include "NW_FBox_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Interactor_Class_s NW_FBox_Interactor_Class_t;
typedef struct NW_FBox_Interactor_s NW_FBox_Interactor_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

typedef
NW_Uint8
(*NW_FBox_Interactor_ProcessEvent_t) (NW_FBox_Interactor_t* Interactor,
                                      NW_LMgr_ActiveBox_t* box,
                                      NW_Evt_Event_t* event);

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Interactor_ClassPart_s {
  NW_FBox_Interactor_ProcessEvent_t processEvent;
} NW_FBox_Interactor_ClassPart_t;
  
struct NW_FBox_Interactor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Interactor_ClassPart_t NW_FBox_Interactor;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_Interactor_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_LMgr_Box_t* formBox;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Interactor_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Interactor))

#define NW_FBox_InteractorOf(object) \
  (NW_Object_Cast (object, NW_FBox_Interactor))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Interactor_Class_t NW_FBox_Interactor_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_Interactor_GetFormBox(_object)\
  (NW_FBox_InteractorOf(_object)->formBox)
    
#define NW_FBox_Interactor_ProcessEvent(_object, _box, _event) \
  (NW_Object_Invoke (_object, NW_FBox_Interactor, processEvent) \
  ((_object), (_box), (_event)))


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INTERACTOR_H */
