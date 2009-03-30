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


#ifndef NW_HED_ENTITYSET_H
#define NW_HED_ENTITYSET_H

#include "nw_object_base.h"
#include "nwx_defs.h"
#include "nwx_string.h"
#include "NW_HED_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_EntitySet_Class_s NW_HED_EntitySet_Class_t;
typedef struct NW_HED_EntitySet_s NW_HED_EntitySet_t;

typedef struct NW_HED_EntitySet_Entry_s NW_HED_EntitySet_Entry_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Ucs2
(*NW_HED_EntitySet_GetEntityChar_t) (const NW_HED_EntitySet_t* entitySet,
                                     NW_Ucs2* name);
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_EntitySet_ClassPart_s {
  NW_Uint32 numEntries;
  NW_Uint32 numCaseInsensitiveEntries;
  const NW_HED_EntitySet_Entry_t* entries;
  NW_HED_EntitySet_GetEntityChar_t getEntityChar;
} NW_HED_EntitySet_ClassPart_t;

struct NW_HED_EntitySet_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_HED_EntitySet_ClassPart_t NW_HED_EntitySet;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_EntitySet_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_EntitySet_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_EntitySet))

#define NW_HED_EntitySetOf(_object) \
  (NW_Object_Cast (_object, NW_HED_EntitySet))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_EntitySet_Class_t NW_HED_EntitySet_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_EntitySet_GetEntityChar(_entitySet, _name) \
  (NW_Object_Invoke (_entitySet, NW_HED_EntitySet, getEntityChar) \
   ((_entitySet), (_name)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_ENTITYSET_H */
