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


#ifndef NW_FBOX_SKIN_H
#define NW_FBOX_SKIN_H

#include "nw_object_dynamic.h"
#include "nw_lmgr_box.h"
#include "NW_FBox_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Skin_Class_s NW_FBox_Skin_Class_t;
typedef struct NW_FBox_Skin_s NW_FBox_Skin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

typedef
TBrowserStatusCode
(*NW_FBox_Skin_Draw_t) (NW_FBox_Skin_t* skin,
                        NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus);

typedef
TBrowserStatusCode
(*NW_FBox_Skin_GetSize_t)(NW_FBox_Skin_t* skin,
                          NW_LMgr_Box_t* box,
                          NW_GDI_Dimension3D_t* size);

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Skin_ClassPart_s {
  NW_FBox_Skin_Draw_t draw;
  NW_FBox_Skin_GetSize_t getSize;
} NW_FBox_Skin_ClassPart_t;
  
struct NW_FBox_Skin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_Skin_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  struct NW_FBox_FormBox_s* formBox;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Skin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Skin))

#define NW_FBox_SkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_Skin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Skin_Class_t NW_FBox_Skin_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_Skin_GetFormBox(_object)\
  (NW_FBox_SkinOf(_object)->formBox)
    
#define NW_FBox_Skin_Draw(_object, _box, _deviceContext, _hasFocus) \
  (NW_Object_Invoke (_object, NW_FBox_Skin, draw) \
  ((_object), (_box), (_deviceContext), (_hasFocus)))

#define NW_FBox_Skin_GetSize(_object, _box, _size) \
  (NW_Object_Invoke (_object, NW_FBox_Skin, getSize) \
  ((_object), (_box), (_size)))


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_SKIN_H */
