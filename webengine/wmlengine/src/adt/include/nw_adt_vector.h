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


#ifndef NW_ADT_VECTOR_H
#define NW_ADT_VECTOR_H

#include "nw_object_dynamic.h"
#include "NW_ADT_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   constants
 * ------------------------------------------------------------------------- */
#define NW_ADT_Vector_AtEnd   ((NW_ADT_Vector_Metric_t) ~0)
#define NW_ADT_Vector_AtFront ((NW_ADT_Vector_Metric_t) 0)

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_Vector_Class_s NW_ADT_Vector_Class_t;
typedef struct NW_ADT_Vector_s NW_ADT_Vector_t;

/* ------------------------------------------------------------------------- */
typedef NW_Uint16 NW_ADT_Vector_Metric_t;

/* ------------------------------------------------------------------------- */
typedef
NW_ADT_Vector_Metric_t
(*NW_ADT_Vector_GetElementSize_t) (const NW_ADT_Vector_t* vector);

typedef
NW_ADT_Vector_Metric_t
(*NW_ADT_Vector_GetSize_t) (const NW_ADT_Vector_t* vector);

typedef
NW_Uint8*
(*NW_ADT_Vector_AddressAt_t) (const NW_ADT_Vector_t* vector,
			      NW_ADT_Vector_Metric_t index);

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_Vector_ClassPart_s {
  NW_ADT_Vector_GetElementSize_t getElementSize;
  NW_ADT_Vector_GetSize_t getSize;
  NW_ADT_Vector_AddressAt_t addressAt;
} NW_ADT_Vector_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_Vector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Vector_ClassPart_t NW_ADT_Vector;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_Vector_s {
  NW_Object_Dynamic_t super;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_Vector_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_Vector))

#define NW_ADT_VectorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_Vector))

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_Vector_Class_t NW_ADT_Vector_Class;

/* ------------------------------------------------------------------------- *
   public methods prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
void**
_NW_ADT_Vector_ElementAt (const NW_ADT_Vector_t* vector,
			  NW_ADT_Vector_Metric_t index);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_Vector_GetElementAt (const NW_ADT_Vector_t* vector,
			     NW_ADT_Vector_Metric_t index,
                             void* element);

NW_ADT_EXPORT
NW_ADT_Vector_Metric_t
_NW_ADT_Vector_GetElementIndex (const NW_ADT_Vector_t* vector,
                                void* target);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_Vector_GetElementSize(_vector) \
  (NW_Object_Invoke (_vector, NW_ADT_Vector, getElementSize) (\
    NW_ADT_VectorOf (_vector)))

#define NW_ADT_Vector_GetSize(_vector) \
  (NW_Object_Invoke (_vector, NW_ADT_Vector, getSize) (\
    NW_ADT_VectorOf (_vector)))

#define NW_ADT_Vector_ElementAt(_vector, _index) \
  (_NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (_vector), (_index)))

#define NW_ADT_Vector_GetElementAt(_vector, _index, _element) \
  (_NW_ADT_Vector_GetElementAt (NW_ADT_VectorOf (_vector), (_index), (_element)))

#define NW_ADT_Vector_GetElementIndex(_vector, _target) \
  (_NW_ADT_Vector_GetElementIndex (NW_ADT_VectorOf (_vector), (_target)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_VECTOR_H */
