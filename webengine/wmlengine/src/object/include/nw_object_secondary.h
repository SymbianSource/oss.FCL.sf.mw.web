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


#ifndef _NW_Object_Secondary_h_
#define _NW_Object_Secondary_h_

#include "nw_object_core.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_Object_Secondary

    @scope:       public

    @description: Base class for all secondary objects.
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Secondary_Class_s NW_Object_Secondary_Class_t;
typedef struct NW_Object_Secondary_s NW_Object_Secondary_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Secondary_ClassPart_s {
  size_t offset;
} NW_Object_Secondary_ClassPart_t;

struct NW_Object_Secondary_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Secondary_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Secondary_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Object_Secondary))

#define NW_Object_SecondaryOf(_object) \
  (NW_Object_Cast ((_object), NW_Object_Secondary))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Secondary_Class_t NW_Object_Secondary_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_Secondary_h_ */
