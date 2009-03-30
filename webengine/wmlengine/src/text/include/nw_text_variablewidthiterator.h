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


#ifndef _NW_Text_VariableWidthIterator_h_
#define _NW_Text_VariableWidthIterator_h_

#include "nw_text_iterator.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/** ----------------------------------------------------------------------- **
    @class:       NW_Text_VariableWidthIterator

    @scope:       public

    @description: <tbd>
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_VariableWidthIterator_Class_s NW_Text_VariableWidthIterator_Class_t;
typedef struct NW_Text_VariableWidthIterator_s NW_Text_VariableWidthIterator_t;

/* ------------------------------------------------------------------------- *
   object class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_VariableWidthIterator_ClassPart_s {
  void** unused;
} NW_Text_VariableWidthIterator_ClassPart_t;

struct NW_Text_VariableWidthIterator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Iterator_ClassPart_t NW_Text_Iterator;
  NW_Text_VariableWidthIterator_ClassPart_t NW_Text_VariableWidthIterator;
};

/* ------------------------------------------------------------------------- *
   object instance structure
 * ------------------------------------------------------------------------- */
struct NW_Text_VariableWidthIterator_s {
  NW_Text_Iterator_t super;

  /* member variables */
  NW_Uint32 offset;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_VariableWidthIterator_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Text_VariableWidthIterator))

#define NW_Text_VariableWidthIteratorOf(_object) \
  (NW_Object_Cast (_object, NW_Text_VariableWidthIterator))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_VariableWidthIterator_Class_t NW_Text_VariableWidthIterator_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Text_VariableWidthIterator_h_ */
