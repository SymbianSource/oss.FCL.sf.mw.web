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


#ifndef NW_FBOX_TABLEKEYMAPPER_H
#define NW_FBOX_TABLEKEYMAPPER_H

#include "nw_fbox_keymapper.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_TableKeyMapper_Class_s NW_FBox_TableKeyMapper_Class_t;
typedef const NW_Object_Base_t NW_FBox_TableKeyMapper_t;

typedef struct NW_FBox_TableKeyMapper_KeyMapEntry_s NW_FBox_TableKeyMapper_KeyMapEntry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_TableKeyMapper_ClassPart_s {
  const NW_FBox_TableKeyMapper_KeyMapEntry_t** keyMaps;
} NW_FBox_TableKeyMapper_ClassPart_t;

struct NW_FBox_TableKeyMapper_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_FBox_KeyMapper_ClassPart_t NW_FBox_KeyMapper;
  NW_FBox_TableKeyMapper_ClassPart_t NW_FBox_TableKeyMapper;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_TableKeyMapper_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_TableKeyMapper))

#define NW_FBox_TableKeyMapperOf(object) \
  (NW_Object_Cast (object, NW_FBox_TableKeyMapper))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_TableKeyMapper_Class_t NW_FBox_TableKeyMapper_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_TABLEKEYMAPPER_H */
