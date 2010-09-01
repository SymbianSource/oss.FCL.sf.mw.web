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


#ifndef _NW_HED_ChildIterator_h_
#define _NW_HED_ChildIterator_h_

#include "NW_HED_EXPORT.h"
#include "nw_adt_mapiterator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_HED_ChildIterator

    @scope:       public

    @description: 
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ChildIterator_Class_s NW_HED_ChildIterator_Class_t;
typedef struct NW_HED_ChildIterator_s NW_HED_ChildIterator_t;

/* ------------------------------------------------------------------------- *
   object class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ChildIterator_ClassPart_s {
  void** unused;
} NW_HED_ChildIterator_ClassPart_t;

struct NW_HED_ChildIterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Iterator_ClassPart_t NW_ADT_Iterator;
  NW_ADT_VectorIterator_ClassPart_t NW_ADT_VectorIterator;
  NW_ADT_MapIterator_ClassPart_t NW_ADT_MapIterator;
  NW_HED_ChildIterator_ClassPart_t NW_HED_ChildIterator;
};

/* ------------------------------------------------------------------------- *
   object instance structure
 * ------------------------------------------------------------------------- */
struct NW_HED_ChildIterator_s {
  NW_ADT_MapIterator_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_ChildIterator_GetClassPart(_object) \
  NW_HED_ChildIterator_GetClassPart (_object)

#define NW_HED_ChildIteratorOf(_object) \
  NW_HED_ChildIteratorOf (_object)

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ChildIterator_Class_t NW_HED_ChildIterator_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ChildIterator_Initialize (NW_HED_ChildIterator_t* childIterator,
                                  NW_ADT_Map_t* children);

NW_HED_EXPORT
NW_HED_ChildIterator_t*
_NW_HED_ChildIterator_New (NW_ADT_Map_t* children);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_ChildIterator_Initialize(_childIterator, _children) \
  (_NW_HED_ChildIterator_Initialize ((_childIterator), NW_ADT_MapOf (_children)))

#define NW_HED_ChildIterator_New(_children) \
  (_NW_HED_ChildIterator_New (NW_ADT_MapOf (_children)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_ChildIterator_h_ */
