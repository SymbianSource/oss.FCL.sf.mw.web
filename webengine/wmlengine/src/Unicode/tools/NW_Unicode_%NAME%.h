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


#ifndef _NW_Unicode_%NAME%_h_
#define _NW_Unicode_%NAME%_h_

#include "NW_Unicode_Category.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_%NAME%_Class_s NW_Unicode_%NAME%_Class_t;
typedef struct NW_Unicode_%NAME%_s NW_Unicode_%NAME%_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Unicode_%NAME%_ClassPart_s {
  void** unused;
} NW_Unicode_%NAME%_ClassPart_t;
  
struct NW_Unicode_%NAME%_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Unicode_Category_ClassPart_t NW_Unicode_Category;
  NW_Unicode_%NAME%_ClassPart_t NW_Unicode_%NAME%;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Unicode_%NAME%_s {
  NW_Unicode_Category_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Unicode_%NAME%_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Unicode_%NAME%))

#define NW_Unicode_%NAME%Of(object) \
  (NW_Object_Cast (object, NW_Unicode_%NAME%))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Unicode_%NAME%_Class_t NW_Unicode_%NAME%_Class;
extern const NW_Unicode_%NAME%_t NW_Unicode_%NAME%;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
#define NW_Unicode_Is%NAME%(_code) \
  (NW_Unicode_Category_IsMemberOf (&NW_Unicode_%NAME%, (_code)))

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _NW_Unicode_%NAME%_h_ */
