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


#ifndef NW_FBOX_KEYMAPPER_H
#define NW_FBOX_KEYMAPPER_H

#include "nw_object_base.h"
#include "nw_evt_keyevent.h"
#include "NW_FBox_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_KeyMapper_Class_s NW_FBox_KeyMapper_Class_t;
typedef const NW_Object_Base_t NW_FBox_KeyMapper_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef enum NW_FBox_KeyMapper_Mode_e {
  NW_FBox_KeyMapper_Mode_Lowercase,
  NW_FBox_KeyMapper_Mode_Uppercase,
  NW_FBox_KeyMapper_Mode_Numeric,
  NW_FBox_KeyMapper_Mode_Locked = 0x80
} NW_FBox_KeyMapper_Mode_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Ucs2
(*NW_FBox_KeyMapper_MapKey_t) (NW_FBox_KeyMapper_t* thisObj,
                               NW_FBox_KeyMapper_Mode_t* mode,
                               NW_Ucs2 key,
                               NW_Uint8 *repeated);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_KeyMapper_ClassPart_s {
  NW_FBox_KeyMapper_MapKey_t mapKey;
} NW_FBox_KeyMapper_ClassPart_t;

struct NW_FBox_KeyMapper_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_FBox_KeyMapper_ClassPart_t NW_FBox_KeyMapper;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_KeyMapper_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_KeyMapper))

#define NW_FBox_KeyMapperOf(object) \
  (NW_Object_Cast (object, NW_FBox_KeyMapper))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_KeyMapper_Class_t NW_FBox_KeyMapper_Class;
NW_FBOX_EXPORT NW_FBox_KeyMapper_t NW_FBox_KeyMapper;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_KeyMapper_MapKey(_object, _mode, _key, _repeated) \
  (NW_Object_Invoke (_object, NW_FBox_KeyMapper, mapKey) \
  ((_object), (_mode), (_key), (_repeated)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_KEYMAPPER_H */
