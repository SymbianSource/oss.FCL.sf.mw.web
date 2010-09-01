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


#ifndef _NW_Text_Iterator_h_
#define _NW_Text_Iterator_h_

#include "NW_Text_EXPORT.h"
#include "nw_object_dynamic.h"
#include "nw_text_types.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/** ----------------------------------------------------------------------- **
    @class:       NW_Text_Iterator

    @scope:       public

    @description: <tbd>
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_Iterator_Class_s NW_Text_Iterator_Class_t;
typedef struct NW_Text_Iterator_s NW_Text_Iterator_t;

typedef struct NW_Text_Abstract_s NW_Text_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Text_Iterator_GetNext_t) (NW_Text_Iterator_t* iterator,
                               NW_Ucs4* character);

typedef
TBrowserStatusCode
(*NW_Text_Iterator_GetOffset_t) (NW_Text_Iterator_t* iterator,
                                 NW_Uint32* offset);

typedef
TBrowserStatusCode
(*NW_Text_Iterator_SetIndex_t) (NW_Text_Iterator_t* iterator,
                                NW_Text_Length_t index);

/* ------------------------------------------------------------------------- *
   object class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_Iterator_ClassPart_s {
  NW_Text_Iterator_GetNext_t getNext;
  NW_Text_Iterator_GetOffset_t getOffset;
  NW_Text_Iterator_SetIndex_t setIndex;
} NW_Text_Iterator_ClassPart_t;

struct NW_Text_Iterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Iterator_ClassPart_t NW_Text_Iterator;
};

/* ------------------------------------------------------------------------- *
   object instance structure
 * ------------------------------------------------------------------------- */
struct NW_Text_Iterator_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_Text_t* text;
  NW_Text_Direction_t direction;
  NW_Text_Length_t index;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_Iterator_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Text_Iterator))

#define NW_Text_IteratorOf(_object) \
  (NW_Object_Cast (_object, NW_Text_Iterator))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_Iterator_Class_t NW_Text_Iterator_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_Iterator_HasMore (NW_Text_Iterator_t* iterator);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Text_Iterator_GetText(_iterator) \
  ((NW_Text_t* const) NW_Text_IteratorOf (_iterator)->text)

#define NW_Text_Iterator_GetDirection(_iterator) \
  ((const NW_Text_Direction_t) NW_Text_IteratorOf (_iterator)->direction)

#define NW_Text_Iterator_SetDirection(_iterator, _direction) \
  (NW_Text_IteratorOf (_iterator)->direction = (_direction), KBrsrSuccess)

#define NW_Text_Iterator_GetIndex(_iterator) \
  ((NW_Text_Length_t) NW_Text_IteratorOf (_iterator)->index)

#define NW_Text_Iterator_SetIndex(_iterator, _index) \
  (NW_Object_Invoke (_iterator, NW_Text_Iterator, setIndex) ( \
     NW_Text_IteratorOf (_iterator), (_index)))

#define NW_Text_Iterator_HasMore(_iterator) \
  (_NW_Text_Iterator_HasMore (NW_Text_IteratorOf (_iterator)))

#define NW_Text_Iterator_GetNext(_iterator, _character) \
  (NW_Object_Invoke (_iterator, NW_Text_Iterator, getNext) ( \
     NW_Text_IteratorOf (_iterator), (_character)))

#define NW_Text_Iterator_GetOffset(_iterator, _offset) \
  (NW_Object_Invoke (_iterator, NW_Text_Iterator, getOffset) ( \
     NW_Text_IteratorOf (_iterator), (_offset)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Text_Iterator_h_ */
