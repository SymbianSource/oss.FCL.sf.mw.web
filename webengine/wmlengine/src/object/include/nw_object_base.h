/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_OBJECT_BASE_H
#define NW_OBJECT_BASE_H

#include "nw_object_core.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Base_Class_s NW_Object_Base_Class_t;
typedef struct NW_Object_Base_s NW_Object_Base_t;

typedef struct NW_Object_Base_secondaryListEntry_s NW_Object_Base_secondaryListEntry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Base_ClassPart_s {
  const NW_Object_Class_t* const* secondaryList;
} NW_Object_Base_ClassPart_t;

struct NW_Object_Base_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Base_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Base_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Object_Base))

#define NW_Object_BaseOf(_object) \
  (NW_Object_Cast (_object, NW_Object_Base))

#define NW_Object_Base_InterfaceOffset(_type, _interface) \
  ((NW_Uint32) &(((_type##_t*)0)->_interface))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Base_Class_t NW_Object_Base_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_BASE_H */
