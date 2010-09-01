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


#ifndef NW_FBOX_INLINETEXTAREASKIN_H
#define NW_FBOX_INLINETEXTAREASKIN_H

#include "nw_fbox_textareaskin.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineTextAreaSkin_Class_s NW_FBox_InlineTextAreaSkin_Class_t;
typedef struct NW_FBox_InlineTextAreaSkin_s NW_FBox_InlineTextAreaSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineTextAreaSkin_ClassPart_s {
  void** unused;
} NW_FBox_InlineTextAreaSkin_ClassPart_t;
  
struct NW_FBox_InlineTextAreaSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_InputSkin_ClassPart_t NW_FBox_InputSkin;
  NW_FBox_TextAreaSkin_ClassPart_t NW_FBox_TextAreaSkin;
  NW_FBox_InlineTextAreaSkin_ClassPart_t NW_FBox_InlineTextAreaSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_InlineTextAreaSkin_s {
  NW_FBox_InputSkin_t super;

  /* member variables */
  NW_Uint16 displayRowOffset;
  NW_ADT_DynamicVector_t* textList;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InlineTextAreaSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_InlineTextAreaSkin))

#define NW_FBox_InlineTextAreaSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_InlineTextAreaSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_InlineTextAreaSkin_Class_t NW_FBox_InlineTextAreaSkin_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INLINETEXTAREASKIN_H */
