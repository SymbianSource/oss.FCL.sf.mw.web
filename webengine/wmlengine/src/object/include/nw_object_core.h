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


#ifndef NW_OBJECT_CORE_H
#define NW_OBJECT_CORE_H

#include "nw_object_object.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Core_Class_s NW_Object_Core_Class_t;
typedef struct NW_Object_Core_s NW_Object_Core_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
NW_Object_t*
(*NW_Object_Core_QuerySecondary_t) (const NW_Object_Core_t* core,
                                    const NW_Object_Class_t* objClass);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Core_ClassPart_s {
  const NW_Object_Class_t* super;
  NW_Object_Core_QuerySecondary_t querySecondary;
} NW_Object_Core_ClassPart_t;

struct NW_Object_Core_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Core_s {
  const NW_Object_Class_t* objClass;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Core_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Object_Core))

#define NW_Object_CoreOf(_object) \
  (NW_Object_Cast (_object, NW_Object_Core))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Core_Class_t NW_Object_Core_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Object_Core_GetClass(_object) \
  (NW_OBJECT_CONSTCAST (NW_Object_Class_t*) \
     NW_Object_CoreOf (_object)->objClass)

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_CORE_H */
