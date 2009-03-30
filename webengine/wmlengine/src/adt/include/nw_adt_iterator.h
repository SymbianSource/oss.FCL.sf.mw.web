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


#ifndef NW_ADT_ITERATOR_H
#define NW_ADT_ITERATOR_H

#include "nw_object_dynamic.h"
#include "NW_ADT_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_Iterator_Class_s NW_ADT_Iterator_Class_t;
typedef struct NW_ADT_Iterator_s NW_ADT_Iterator_t;

/* ------------------------------------------------------------------------- */
typedef enum NW_ADT_IteratorDirection_e {
  NW_ADT_IteratorDirection_Increment = 1,
  NW_ADT_IteratorDirection_Decrement = -1
} NW_ADT_IteratorDirection_t;

/* ------------------------------------------------------------------------- */
typedef
NW_Int32
(*NW_ADT_Iterator_HasMoreElements_t) (const NW_ADT_Iterator_t* iterator);

typedef
void**
(*NW_ADT_Iterator_NextElement_t) (NW_ADT_Iterator_t* iterator);

typedef
NW_Uint32
(*NW_ADT_Iterator_GetElementSize_t) (NW_ADT_Iterator_t* iterator);

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_Iterator_ClassPart_s {
  NW_ADT_Iterator_HasMoreElements_t hasMoreElements;
  NW_ADT_Iterator_NextElement_t nextElement;
  NW_ADT_Iterator_GetElementSize_t getElementSize;
} NW_ADT_Iterator_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_Iterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Iterator_ClassPart_t NW_ADT_Iterator;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_Iterator_s {
  NW_Object_Dynamic_t super;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_Iterator_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_Iterator))

#define NW_ADT_IteratorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_Iterator))

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_Iterator_Class_t NW_ADT_Iterator_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Iterator_GetNextElement (NW_ADT_Iterator_t* iterator,
                                 void* element);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_Iterator_HasMoreElements(_iterator) \
  (NW_Object_Invoke (_iterator, NW_ADT_Iterator, hasMoreElements) (\
     NW_ADT_IteratorOf (_iterator)))

#define NW_ADT_Iterator_NextElement(_iterator) \
  (NW_Object_Invoke (_iterator, NW_ADT_Iterator, nextElement) (\
     NW_ADT_IteratorOf (_iterator)))

#define NW_ADT_Iterator_GetElementSize(_iterator) \
  (NW_Object_Invoke (_iterator, NW_ADT_Iterator, getElementSize) (\
     NW_ADT_IteratorOf (_iterator)))

#define NW_ADT_Iterator_GetNextElement(_iterator, _element) \
  (_NW_ADT_Iterator_GetNextElement (\
     NW_ADT_IteratorOf (_iterator), (_element)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_ITERATOR_H */
