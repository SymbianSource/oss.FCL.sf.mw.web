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


#ifndef _NW_Unicode_SeparatorSpace_h_
#define _NW_Unicode_SeparatorSpace_h_




#include "NW_Unicode_Category.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_SeparatorSpace_Class_s NW_Unicode_SeparatorSpace_Class_t;
typedef struct NW_Unicode_SeparatorSpace_s NW_Unicode_SeparatorSpace_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_SeparatorSpace_ClassPart_s {
  void** unused;
} NW_Unicode_SeparatorSpace_ClassPart_t;
  
struct NW_Unicode_SeparatorSpace_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Unicode_Category_ClassPart_t NW_Unicode_Category;
  NW_Unicode_SeparatorSpace_ClassPart_t NW_Unicode_SeparatorSpace;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Unicode_SeparatorSpace_s {
  NW_Unicode_Category_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Unicode_SeparatorSpace_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Unicode_SeparatorSpace))

#define NW_Unicode_SeparatorSpaceOf(object) \
  (NW_Object_Cast (object, NW_Unicode_SeparatorSpace))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_UNICODE_EXPORT const NW_Unicode_SeparatorSpace_Class_t NW_Unicode_SeparatorSpace_Class;
NW_UNICODE_EXPORT const NW_Unicode_SeparatorSpace_t NW_Unicode_SeparatorSpace;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
#define NW_Unicode_IsSeparatorSpace(_code) \
  (NW_Unicode_Category_IsMemberOf (&NW_Unicode_SeparatorSpace, (_code)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Unicode_SeparatorSpace_h_ */
