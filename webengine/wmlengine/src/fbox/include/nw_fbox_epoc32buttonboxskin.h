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


#ifndef NW_FBOX_Epoc32ButtonBoxSKIN_H
#define NW_FBOX_Epoc32ButtonBoxSKIN_H

#include "nw_fbox_buttonboxskin.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32ButtonBoxSkin_Class_s NW_FBox_Epoc32ButtonBoxSkin_Class_t;
typedef struct NW_FBox_Epoc32ButtonBoxSkin_s NW_FBox_Epoc32ButtonBoxSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32ButtonBoxSkin_ClassPart_s {
  void** unused;
} NW_FBox_Epoc32ButtonBoxSkin_ClassPart_t;
  
struct NW_FBox_Epoc32ButtonBoxSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_ButtonBoxSkin_ClassPart_t NW_FBox_ButtonBoxSkin;
  NW_FBox_Epoc32ButtonBoxSkin_ClassPart_t NW_FBox_Epoc32ButtonBoxSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_Epoc32ButtonBoxSkin_s {
  NW_FBox_ButtonBoxSkin_t super;

  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Epoc32ButtonBoxSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Epoc32ButtonBoxSkin))

#define NW_FBox_Epoc32ButtonBoxSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_Epoc32ButtonBoxSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Epoc32ButtonBoxSkin_Class_t NW_FBox_Epoc32ButtonBoxSkin_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_Epoc32ButtonBoxSKIN_H */
