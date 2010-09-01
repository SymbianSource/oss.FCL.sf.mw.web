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


#ifndef _NW_Object_Aggregate_h_
#define _NW_Object_Aggregate_h_

#include "nw_object_secondary.h"
#include <stdarg.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_Object_Aggregate

    @scope:       public

    @description: Base class for all aggregates. Aggregates are used to
                  provide a semblence of multiple inheritance. It's built
                  on top of the interface mechanism so it's not true
                  inheritance in the C++ sense but more like Microsoft COM.
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Aggregate_Class_s NW_Object_Aggregate_Class_t;
typedef struct NW_Object_Aggregate_s NW_Object_Aggregate_t;

/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Object_Aggregate_Construct_t) (NW_Object_Aggregate_t* aggregate,
                                    va_list* argp);

typedef
void
(*NW_Object_Aggregate_Destruct_t) (NW_Object_Aggregate_t* aggregate);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Aggregate_ClassPart_s {
  const NW_Object_Class_t* const* secondaryList;
  NW_Object_Aggregate_Construct_t construct;
  NW_Object_Aggregate_Destruct_t destruct;
} NW_Object_Aggregate_ClassPart_t;

struct NW_Object_Aggregate_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_Object_Aggregate_ClassPart_t NW_Object_Aggregate;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Aggregate_s {
  NW_Object_Secondary_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Aggregate_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Object_Aggregate))

#define NW_Object_AggregateOf(_object) \
  (NW_Object_Cast ((_object), NW_Object_Aggregate))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Aggregate_Class_t NW_Object_Aggregate_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_Aggregate_h_ */
