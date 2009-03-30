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


#ifndef NW_ADT_VECTORITERATOR_H
#define NW_ADT_VECTORITERATOR_H

#include "nw_adt_iterator.h"
#include "nw_adt_vector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_VectorIterator_Class_s NW_ADT_VectorIterator_Class_t;
typedef struct NW_ADT_VectorIterator_s NW_ADT_VectorIterator_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_VectorIterator_ClassPart_s {
  void** unused;
} NW_ADT_VectorIterator_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_VectorIterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Iterator_ClassPart_t NW_ADT_Iterator;
  NW_ADT_VectorIterator_ClassPart_t NW_ADT_VectorIterator;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_VectorIterator_s {
  NW_ADT_Iterator_t super;

  /* member variables */
  const NW_ADT_Vector_t* vector;
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t end;
  NW_ADT_IteratorDirection_t direction;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_VectorIterator_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_VectorIterator))

#define NW_ADT_VectorIteratorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_VectorIterator))

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_VectorIterator_Class_t NW_ADT_VectorIterator_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_VectorIterator_GetIndex(_vectorIterator) \
  ((NW_ADT_Vector_Metric_t) \
     (NW_ADT_VectorIteratorOf (_vectorIterator)->index))

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_VectorIterator_t*
_NW_ADT_VectorIterator_Initialize (NW_ADT_VectorIterator_t* vectorIterator,
                                   const NW_ADT_Vector_t* vector,
                                   NW_ADT_Vector_Metric_t start,
                                   NW_ADT_Vector_Metric_t end,
                                   NW_ADT_IteratorDirection_t direction);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
#define NW_ADT_VectorIterator_Initialize(_vectorIterator, _vector, _start, _end, _direction) \
  (_NW_ADT_VectorIterator_Initialize ((_vectorIterator), NW_ADT_VectorOf (_vector), (_start), (_end), (_direction)))

#define NW_ADT_VectorIterator_New(_vector, _start, _end, _direction) \
  (_NW_ADT_VectorIterator_New (NW_ADT_VectorOf (_vector), (_start), (_end), (_direction)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_VECTORITERATOR_H */
