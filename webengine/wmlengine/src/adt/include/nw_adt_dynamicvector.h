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


#ifndef NW_ADT_DYNAMICVECTOR_H
#define NW_ADT_DYNAMICVECTOR_H

#include "nw_adt_vector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_DynamicVector_Class_s NW_ADT_DynamicVector_Class_t;
typedef struct NW_ADT_DynamicVector_s NW_ADT_DynamicVector_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_ADT_DynamicVector_ResizeCapacity_t) (NW_ADT_DynamicVector_t* dynamicVector,
                                          NW_ADT_Vector_Metric_t capacityNeeded);

typedef
TBrowserStatusCode
(*NW_ADT_DynamicVector_MoveElements_t) (NW_ADT_DynamicVector_t* dynamicVector,
                                        NW_ADT_Vector_Metric_t srcIndex,
                                        NW_ADT_Vector_Metric_t dstIndex);

typedef
TBrowserStatusCode
(*NW_ADT_DynamicVector_Clear_t) (NW_ADT_DynamicVector_t* dynamicVector);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_DynamicVector_ClassPart_s {
  NW_ADT_DynamicVector_ResizeCapacity_t resizeCapacity;
  NW_ADT_DynamicVector_MoveElements_t moveElements;
  NW_ADT_DynamicVector_Clear_t clear;
} NW_ADT_DynamicVector_ClassPart_t;

struct NW_ADT_DynamicVector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Vector_ClassPart_t NW_ADT_Vector;
  NW_ADT_DynamicVector_ClassPart_t NW_ADT_DynamicVector;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ADT_DynamicVector_s {
  NW_ADT_Vector_t super;

  /* member variables */
  NW_ADT_Vector_Metric_t elementSize;
  NW_ADT_Vector_Metric_t capacity;
  NW_ADT_Vector_Metric_t size;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ADT_DynamicVector_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_DynamicVector))

#define NW_ADT_DynamicVectorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_DynamicVector))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_DynamicVector_Class_t NW_ADT_DynamicVector_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
void**
_NW_ADT_DynamicVector_ReplaceAt (NW_ADT_DynamicVector_t* dynamicVector,
                                 void* element,
                                 NW_ADT_Vector_Metric_t index);

NW_ADT_EXPORT
void**
_NW_ADT_DynamicVector_InsertAt (NW_ADT_DynamicVector_t* dynamicVector,
                                void* element,
                                NW_ADT_Vector_Metric_t index);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_DynamicVector_RemoveAt (NW_ADT_DynamicVector_t* dynamicVector,
                                NW_ADT_Vector_Metric_t index);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_DynamicVector_RemoveElement (NW_ADT_DynamicVector_t* dynamicVector,
                                     void* element);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_DynamicVector_InsertVectorAt (NW_ADT_DynamicVector_t* dynamicVector,
                                     NW_ADT_Vector_t* vector,
                                     NW_ADT_Vector_Metric_t where);
/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_DynamicVector_GetCapacity(_dynamicVector) \
  (NW_OBJECT_CONSTCAST(NW_ADT_Vector_Metric_t) NW_ADT_DynamicVectorOf (_dynamicVector)->capacity)

#define NW_ADT_DynamicVector_Clear(_dynamicVector) \
  (NW_Object_Invoke(_dynamicVector, NW_ADT_DynamicVector, clear) \
   (_dynamicVector))

#define NW_ADT_DynamicVector_ReplaceAt(_dynamicVector, _element, _index) \
  (_NW_ADT_DynamicVector_ReplaceAt (NW_ADT_DynamicVectorOf (_dynamicVector), (_element), (_index)))

#define NW_ADT_DynamicVector_InsertAt(_dynamicVector, _element, _index) \
  (_NW_ADT_DynamicVector_InsertAt (NW_ADT_DynamicVectorOf (_dynamicVector), (_element), (_index)))

#define NW_ADT_DynamicVector_InsertVectorAt(_dynamicVector, _vector, _index) \
  (_NW_ADT_DynamicVector_InsertVectorAt (NW_ADT_DynamicVectorOf (_dynamicVector), NW_ADT_VectorOf (_vector), (_index)))

#define NW_ADT_DynamicVector_RemoveAt(_dynamicVector, _index) \
  (_NW_ADT_DynamicVector_RemoveAt (NW_ADT_DynamicVectorOf (_dynamicVector), (_index)))

#define NW_ADT_DynamicVector_RemoveElement(_dynamicVector, _element) \
  (_NW_ADT_DynamicVector_RemoveElement (NW_ADT_DynamicVectorOf (_dynamicVector), (_element)))

#define NW_ADT_DynamicVector_Trim(_dynamicVector) \
  (_NW_ADT_DynamicVector_Trim (NW_ADT_DynamicVectorOf (_dynamicVector)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_DYNAMICVECTOR_H */
