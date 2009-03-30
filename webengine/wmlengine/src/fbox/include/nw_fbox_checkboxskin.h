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


#ifndef NW_FBOX_CHECKBOXSKIN_H
#define NW_FBOX_CHECKBOXSKIN_H

#include "nw_fbox_skin.h"
#include "nw_fbox_formbox.h"

#ifdef __cplusplus
NW_FBOX_EXPORT "C" {
#endif /* __cplusplus */
  
 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_CheckBoxSkin_Class_s NW_FBox_CheckBoxSkin_Class_t;
typedef struct NW_FBox_CheckBoxSkin_s NW_FBox_CheckBoxSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_CheckBoxSkin_ClassPart_s {
  void** unused;
} NW_FBox_CheckBoxSkin_ClassPart_t;
  
struct NW_FBox_CheckBoxSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_CheckBoxSkin_ClassPart_t NW_FBox_CheckBoxSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_CheckBoxSkin_s {
  NW_FBox_Skin_t super;

  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_CheckBoxSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_CheckBoxSkin))

#define NW_FBox_CheckBoxSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_CheckBoxSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_CheckBoxSkin_Class_t NW_FBox_CheckBoxSkin_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

    
/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_CheckBoxSkin_t*
NW_FBox_CheckBoxSkin_New(NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // NW_FBOX_EXPORT "C" {
#endif /* __cplusplus */

#endif
