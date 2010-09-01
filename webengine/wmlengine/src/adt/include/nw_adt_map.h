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


#ifndef NW_ADT_MAP_H
#define NW_ADT_MAP_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_Map_Class_s NW_ADT_Map_Class_t;
typedef struct NW_ADT_Map_s NW_ADT_Map_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
NW_ADT_Vector_Metric_t
(*NW_ADT_Map_Lookup_t) (const NW_ADT_Map_t* thisObj,
                        const void* key);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_Map_ClassPart_s {
  NW_ADT_Map_Lookup_t lookup;
} NW_ADT_Map_ClassPart_t;

struct NW_ADT_Map_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Map_ClassPart_t NW_ADT_Map;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ADT_Map_s {
  NW_Object_Dynamic_t super;

  /* member Map */
  NW_Uint32 keySize;
  NW_Uint32 valueSize;
  NW_ADT_DynamicVector_t* vector;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ADT_Map_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_Map))

#define NW_ADT_MapOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_Map))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_Map_Class_t NW_ADT_Map_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_Map_GetKeySize(_map) \
  (NW_OBJECT_CONSTCAST(NW_Uint32) NW_ADT_MapOf (_map)->keySize)

#define NW_ADT_Map_GetValueSize(_map) \
  (NW_OBJECT_CONSTCAST(NW_Uint32) NW_ADT_MapOf (_map)->valueSize)

#define NW_ADT_Map_GetVector(_object) \
  (NW_OBJECT_CONSTCAST(NW_ADT_DynamicVector_t*) NW_ADT_MapOf (_object)->vector)

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_Map_Set (NW_ADT_Map_t* thisObj,
                const void* key,
                const void* value);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_Map_Get (const NW_ADT_Map_t* thisObj,
                const void* key,
                void* value);

/* note that the pointer returned by this call may point to an odd or otherwise unaligned 
** address.  A memcopy must be used to move the contents pointed to into a new variable
*/
NW_ADT_EXPORT
void**
NW_ADT_Map_GetUnsafe (const NW_ADT_Map_t* thisObj,
                      const void* key);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_Map_Remove (NW_ADT_Map_t* thisObj,
                   const void* key);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_Map_Copy (const NW_ADT_Map_t* thisObj, 
                 NW_ADT_Map_t* dest);


TBrowserStatusCode
NW_ADT_Map_Clear (NW_ADT_Map_t* thisObj);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_Map_t*
NW_ADT_Map_New (NW_Uint32 keySize,
                NW_Uint32 valueSize,
                NW_ADT_DynamicVector_t* dynamicVector);

NW_ADT_EXPORT
NW_ADT_Map_t*
NW_ADT_FixedMap_New (NW_Uint32 keySize,
                     NW_Uint32 valueSize,
                     NW_ADT_Vector_Metric_t capacity);

NW_ADT_EXPORT
NW_ADT_Map_t*
NW_ADT_ResizableMap_New (NW_Uint32 keySize,
                         NW_Uint32 valueSize,
                         NW_ADT_Vector_Metric_t initialCapacity,
                         NW_ADT_Vector_Metric_t capacityIncrement);

NW_ADT_EXPORT
NW_ADT_Map_t*
NW_ADT_SegmentedMap_New (NW_Uint32 keySize,
                         NW_Uint32 valueSize,
                         NW_ADT_Vector_Metric_t segmentSize);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_MAP_H */
