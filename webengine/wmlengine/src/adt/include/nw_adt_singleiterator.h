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


#ifndef NW_ADT_SINGLEITERATOR_H
#define NW_ADT_SINGLEITERATOR_H

#include "nw_adt_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_SingleIterator_Class_s NW_ADT_SingleIterator_Class_t;
typedef struct NW_ADT_SingleIterator_s NW_ADT_SingleIterator_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_SingleIterator_ClassPart_s {
  void** unused;
} NW_ADT_SingleIterator_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_SingleIterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Iterator_ClassPart_t NW_ADT_Iterator;
  NW_ADT_SingleIterator_ClassPart_t NW_ADT_SingleIterator;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_SingleIterator_s {
  NW_ADT_Iterator_t super;

  /* member variables */
  void* element;
  NW_Uint16 elementSize;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_SingleIterator_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_SingleIterator))

#define NW_ADT_SingleIteratorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_SingleIterator))

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_SingleIterator_Class_t NW_ADT_SingleIterator_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_SingleIterator_t*
NW_ADT_SingleIterator_New (void* element,
                           NW_Uint16 elementSize);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_SINGLEITERATOR_H */
