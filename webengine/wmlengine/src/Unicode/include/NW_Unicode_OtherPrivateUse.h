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


#ifndef _NW_Unicode_OtherPrivateUse_h_
#define _NW_Unicode_OtherPrivateUse_h_




#include "NW_Unicode_Category.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_OtherPrivateUse_Class_s NW_Unicode_OtherPrivateUse_Class_t;
typedef struct NW_Unicode_OtherPrivateUse_s NW_Unicode_OtherPrivateUse_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_OtherPrivateUse_ClassPart_s {
  void** unused;
} NW_Unicode_OtherPrivateUse_ClassPart_t;
  
struct NW_Unicode_OtherPrivateUse_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Unicode_Category_ClassPart_t NW_Unicode_Category;
  NW_Unicode_OtherPrivateUse_ClassPart_t NW_Unicode_OtherPrivateUse;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Unicode_OtherPrivateUse_s {
  NW_Unicode_Category_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Unicode_OtherPrivateUse_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Unicode_OtherPrivateUse))

#define NW_Unicode_OtherPrivateUseOf(object) \
  (NW_Object_Cast (object, NW_Unicode_OtherPrivateUse))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_UNICODE_EXPORT const NW_Unicode_OtherPrivateUse_Class_t NW_Unicode_OtherPrivateUse_Class;
NW_UNICODE_EXPORT const NW_Unicode_OtherPrivateUse_t NW_Unicode_OtherPrivateUse;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
#define NW_Unicode_IsOtherPrivateUse(_code) \
  (NW_Unicode_Category_IsMemberOf (&NW_Unicode_OtherPrivateUse, (_code)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Unicode_OtherPrivateUse_h_ */
