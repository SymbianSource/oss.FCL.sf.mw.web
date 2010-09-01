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


#ifndef NW_FBOX_OPTIONBOXSKIN_H
#define NW_FBOX_OPTIONBOXSKIN_H

#include "nw_fbox_skin.h"
#include "nw_fbox_formbox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_OptionBoxSkin_Class_s NW_FBox_OptionBoxSkin_Class_t;
typedef struct NW_FBox_OptionBoxSkin_s NW_FBox_OptionBoxSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_OptionBoxSkin_ClassPart_s {
  void** unused;
} NW_FBox_OptionBoxSkin_ClassPart_t;
  
struct NW_FBox_OptionBoxSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_OptionBoxSkin_ClassPart_t NW_FBox_OptionBoxSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_OptionBoxSkin_s {
  NW_FBox_Skin_t super;

  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_OptionBoxSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_OptionBoxSkin))

#define NW_FBox_OptionBoxSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_OptionBoxSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_OptionBoxSkin_Class_t NW_FBox_OptionBoxSkin_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

    
/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_OptionBoxSkin_t*
NW_FBox_OptionBoxSkin_New(NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
