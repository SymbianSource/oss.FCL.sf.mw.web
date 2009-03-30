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


#ifndef NW_FBOX_BUTTONBOXSKIN_H
#define NW_FBOX_BUTTONBOXSKIN_H

#include "nw_fbox_skin.h"
#include "nw_fbox_formbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_ButtonBoxSkin_Class_s NW_FBox_ButtonBoxSkin_Class_t;
typedef struct NW_FBox_ButtonBoxSkin_s NW_FBox_ButtonBoxSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_FBox_ButtonBoxSkin_DrawContent_t) (NW_FBox_ButtonBoxSkin_t* thisObj,
                                        NW_LMgr_Box_t* box,
                                        CGDIDeviceContext* deviceContext,
                                        NW_Uint8 hasFocus);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_ButtonBoxSkin_ClassPart_s {
  NW_FBox_ButtonBoxSkin_DrawContent_t drawContent;
} NW_FBox_ButtonBoxSkin_ClassPart_t;
  
struct NW_FBox_ButtonBoxSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_ButtonBoxSkin_ClassPart_t NW_FBox_ButtonBoxSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_ButtonBoxSkin_s {
  NW_FBox_Skin_t super;

  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_ButtonBoxSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_ButtonBoxSkin))

#define NW_FBox_ButtonBoxSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_ButtonBoxSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_ButtonBoxSkin_Class_t NW_FBox_ButtonBoxSkin_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */
    
#define NW_FBox_ButtonBoxSkin_DrawContent(_object, _box, _deviceContext, _hasFocus) \
  (NW_Object_Invoke (_object, NW_FBox_ButtonBoxSkin, drawContent) \
  ((_object), (_box), (_deviceContext), (_hasFocus)))

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_ButtonBoxSkin_t*
NW_FBox_ButtonBoxSkin_New(NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
