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


#ifndef _NW_Unicode_Category_h_
#define _NW_Unicode_Category_h_

#include "nw_object_core.h"
#include "NW_Unicode_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_Category_Class_s NW_Unicode_Category_Class_t;
typedef struct NW_Unicode_Category_s NW_Unicode_Category_t;

typedef struct NW_Unicode_Category_TableEntry_s NW_Unicode_Category_TableEntry_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_Category_ClassPart_s {
  NW_Uint16 tableSize;
  const NW_Unicode_Category_TableEntry_t* table;
} NW_Unicode_Category_ClassPart_t;
  
struct NW_Unicode_Category_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Unicode_Category_ClassPart_t NW_Unicode_Category;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Unicode_Category_s {
  NW_Object_Core_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Unicode_Category_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Unicode_Category))

#define NW_Unicode_CategoryOf(object) \
  (NW_Object_Cast (object, NW_Unicode_Category))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_UNICODE_EXPORT const NW_Unicode_Category_Class_t NW_Unicode_Category_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_UNICODE_EXPORT
NW_Bool
_NW_Unicode_Category_IsMemberOf (const NW_Unicode_Category_t* category,
				 NW_Ucs4 code);

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
#define NW_Unicode_Category_IsMemberOf(_object, _code) \
  (_NW_Unicode_Category_IsMemberOf (NW_Unicode_CategoryOf (_object), (_code)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Unicode_Category_h_ */
