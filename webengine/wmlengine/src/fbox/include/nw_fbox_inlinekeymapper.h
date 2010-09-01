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


#ifndef NW_FBOX_INLINEKEYMAPPER_H
#define NW_FBOX_INLINEKEYMAPPER_H

#include "nw_fbox_tablekeymapper.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineKeyMapper_Class_s NW_FBox_InlineKeyMapper_Class_t;
typedef const NW_Object_Base_t NW_FBox_InlineKeyMapper_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineKeyMapper_ClassPart_s {
  void** unused;
} NW_FBox_InlineKeyMapper_ClassPart_t;

struct NW_FBox_InlineKeyMapper_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_FBox_KeyMapper_ClassPart_t NW_FBox_KeyMapper;
  NW_FBox_TableKeyMapper_ClassPart_t NW_FBox_TableKeyMapper;
  NW_FBox_InlineKeyMapper_ClassPart_t NW_FBox_InlineKeyMapper;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InlineKeyMapper_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_InlineKeyMapper))

#define NW_FBox_InlineKeyMapperOf(object) \
  (NW_Object_Cast (object, NW_FBox_InlineKeyMapper))


/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_InlineKeyMapper_Class_t NW_FBox_InlineKeyMapper_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_INLINEKEYMAPPER_H */
