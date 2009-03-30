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


#ifndef NW_FBOX_INLINEINPUTSKIN_H
#define NW_FBOX_INLINEINPUTSKIN_H

#include "nw_fbox_inputskin.h"
#include "nw_fbox_formbox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineInputSkin_Class_s NW_FBox_InlineInputSkin_Class_t;
typedef struct NW_FBox_InlineInputSkin_s NW_FBox_InlineInputSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineInputSkin_ClassPart_s {
  void** unused;
} NW_FBox_InlineInputSkin_ClassPart_t;
  
struct NW_FBox_InlineInputSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_InputSkin_ClassPart_t NW_FBox_InputSkin;
  NW_FBox_InlineInputSkin_ClassPart_t NW_FBox_InlineInputSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_InlineInputSkin_s {
  NW_FBox_InputSkin_t super;

  /* member variables */
  NW_Text_Length_t displayIndex;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InlineInputSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_InlineInputSkin))

#define NW_FBox_InlineInputSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_InlineInputSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_InlineInputSkin_Class_t NW_FBox_InlineInputSkin_Class;


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INLINEINPUTSKIN_H */
