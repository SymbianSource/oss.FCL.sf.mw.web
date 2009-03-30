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


#ifndef _NW_Text_FixedWidthIterator_h_
#define _NW_Text_FixedWidthIterator_h_

#include "nw_text_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/** ----------------------------------------------------------------------- **
    @class:       NW_Text_FixedWidthIterator

    @scope:       public

    @description: <tbd>
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_FixedWidthIterator_Class_s NW_Text_FixedWidthIterator_Class_t;
typedef struct NW_Text_FixedWidthIterator_s NW_Text_FixedWidthIterator_t;

/* ------------------------------------------------------------------------- *
   object class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_FixedWidthIterator_ClassPart_s {
  void** unused;
} NW_Text_FixedWidthIterator_ClassPart_t;

struct NW_Text_FixedWidthIterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Iterator_ClassPart_t NW_Text_Iterator;
  NW_Text_FixedWidthIterator_ClassPart_t NW_Text_FixedWidthIterator;
};

/* ------------------------------------------------------------------------- *
   object instance structure
 * ------------------------------------------------------------------------- */
struct NW_Text_FixedWidthIterator_s {
  NW_Text_Iterator_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_FixedWidthIterator_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Text_FixedWidthIterator))

#define NW_Text_FixedWidthIteratorOf(_object) \
  (NW_Object_Cast (_object, NW_Text_FixedWidthIterator))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_FixedWidthIterator_Class_t NW_Text_FixedWidthIterator_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Text_FixedWidthIterator_h_ */
