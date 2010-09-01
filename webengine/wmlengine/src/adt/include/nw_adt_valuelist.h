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


#ifndef NW_ADT_VALUELIST_H
#define NW_ADT_VALUELIST_H

#include "nw_object_dynamic.h"
#include "nw_adt_map.h"
#include "nw_adt_iterator.h"
#include "nw_adt_types.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_ValueList_Class_s NW_ADT_ValueList_Class_t;
typedef struct NW_ADT_ValueList_s NW_ADT_ValueList_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_ValueList_Entry_s {
  NW_ADT_Value_t value;
  unsigned type : 4;
  unsigned freeNeeded : 1;
} NW_ADT_ValueList_Entry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_ValueList_ClassPart_s {
  void** unused;
} NW_ADT_ValueList_ClassPart_t;

struct NW_ADT_ValueList_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_ValueList_ClassPart_t NW_ADT_ValueList;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ADT_ValueList_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_ADT_Map_t* map;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ADT_ValueList_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_ValueList))

#define NW_ADT_ValueListOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_ValueList))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_ValueList_Class_t NW_ADT_ValueList_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_ValueList_GetSize(_propertyList) \
  (NW_ADT_Vector_GetSize (NW_ADT_VectorOf (NW_ADT_ValueListOf (_propertyList)->list)))

NW_ADT_EXPORT 
TBrowserStatusCode
NW_ADT_ValueList_Get (const NW_ADT_ValueList_t* thisObj,
                      void* key,
                      NW_ADT_ValueList_Entry_t* entry);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_ValueList_Set (NW_ADT_ValueList_t* thisObj,
                      void* key,
                      const NW_ADT_ValueList_Entry_t* entry);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_ValueList_Remove (const NW_ADT_ValueList_t* thisObj,
                         void* key);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_ValueList_Copy (const NW_ADT_ValueList_t* thisObj,
                       NW_ADT_ValueList_t* dest);

NW_ADT_EXPORT
NW_ADT_Iterator_t*
NW_ADT_ValueList_GetElements (NW_ADT_ValueList_t* thisObj);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_ValueList_t*
NW_ADT_ValueList_New (NW_Uint32 keySize,
                      NW_ADT_Vector_Metric_t capacity,
                      NW_ADT_Vector_Metric_t increment);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_VALUELIST_H */
