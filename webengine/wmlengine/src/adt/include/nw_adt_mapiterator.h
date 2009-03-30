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


#ifndef NW_ADT_MAPITERATOR_H
#define NW_ADT_MAPITERATOR_H

#include "nw_adt_vectoriterator.h"
#include "nw_adt_map.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_MapIterator_Class_s NW_ADT_MapIterator_Class_t;
typedef struct NW_ADT_MapIterator_s NW_ADT_MapIterator_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_MapIterator_ClassPart_s {
  void** unused;
} NW_ADT_MapIterator_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_MapIterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Iterator_ClassPart_t NW_ADT_Iterator;
  NW_ADT_VectorIterator_ClassPart_t NW_ADT_VectorIterator;
  NW_ADT_MapIterator_ClassPart_t NW_ADT_MapIterator;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_MapIterator_s {
  NW_ADT_VectorIterator_t super;

  /* member variables */
  NW_ADT_Map_t* map;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_MapIterator_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_MapIterator))

#define NW_ADT_MapIteratorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_MapIterator))

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_MapIterator_Class_t NW_ADT_MapIterator_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_MapIterator_Initialize (NW_ADT_MapIterator_t* iterator,
                               const NW_ADT_Map_t* map);

NW_ADT_EXPORT
NW_ADT_MapIterator_t*
NW_ADT_MapIterator_New (const NW_ADT_Map_t* map);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_MAPITERATOR_H */
