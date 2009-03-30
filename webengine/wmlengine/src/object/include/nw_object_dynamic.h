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


#ifndef NW_OBJECT_DYNAMIC_H
#define NW_OBJECT_DYNAMIC_H

#include "nw_object_base.h"
#include "nwx_defs.h"
#include <stdarg.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Dynamic_Class_s NW_Object_Dynamic_Class_t;
typedef struct NW_Object_Dynamic_s NW_Object_Dynamic_t;

/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Object_Dynamic_Construct_t) (NW_Object_Dynamic_t* object,
                                  va_list* argp);

typedef
void
(*NW_Object_Dynamic_Destruct_t) (NW_Object_Dynamic_t* object);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Dynamic_ClassPart_s {
  NW_Uint16 instanceSize;
  NW_Object_Dynamic_Construct_t construct;
  NW_Object_Dynamic_Destruct_t destruct;
} NW_Object_Dynamic_ClassPart_t;

struct NW_Object_Dynamic_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Dynamic_s {
  NW_Object_Base_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Dynamic_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Object_Dynamic))

#define NW_Object_DynamicOf(_object) \
  (NW_Object_Cast ((_object), NW_Object_Dynamic))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Dynamic_Class_t NW_Object_Dynamic_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
TBrowserStatusCode
NW_Object_Initialize (const NW_Object_Class_t* objClass,
                      NW_Object_t* object, ...);
                      
NW_OBJECT_EXPORT
NW_Object_t*
NW_Object_New (const NW_Object_Class_t* objClass, ...);

NW_OBJECT_EXPORT
void
NW_Object_Terminate (NW_Object_t* object);

NW_OBJECT_EXPORT
void
NW_Object_Delete (NW_Object_t* object);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_DYNAMIC_H */
