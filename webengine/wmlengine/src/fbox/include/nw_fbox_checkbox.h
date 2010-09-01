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


#ifndef NW_FBOX_CHECKBOX_H
#define NW_FBOX_CHECKBOX_H

#include "nw_fbox_formbox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_CheckBox_Class_s NW_FBox_CheckBox_Class_t;
typedef struct NW_FBox_CheckBox_s NW_FBox_CheckBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_CheckBox_ClassPart_s {
  void** unused;
} NW_FBox_CheckBox_ClassPart_t;
  
struct NW_FBox_CheckBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
  NW_FBox_CheckBox_ClassPart_t NW_FBox_CheckBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_CheckBox_s {
  NW_FBox_FormBox_t super;
  NW_Bool focus;
  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_CheckBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_CheckBox))

#define NW_FBox_CheckBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_CheckBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_FBox_CheckBox_Class_t NW_FBox_CheckBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#if 0 /* TODO: remove old code */
#define NW_FBox_CheckBox_ChangeState(_checkBox) \
  (NW_FBox_CheckBoxOf(_checkBox)->state = (NW_Bool)(!(NW_FBox_CheckBoxOf(_checkBox)->state)))

#define NW_FBox_CheckBox_GetState(_checkBox) \
  (NW_FBox_CheckBoxOf(_checkBox)->state)
#endif
NW_Bool
NW_FBox_CheckBox_GetState(NW_FBox_CheckBox_t* thisObj);

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_CheckBox_t*
NW_FBox_CheckBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_CHECKBOX_H */
