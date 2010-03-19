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


#ifndef NW_HED_CONTEXT_H
#define NW_HED_CONTEXT_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "nw_string_string.h"
#include "NW_HED_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_Context_Class_s NW_HED_Context_Class_t;
typedef struct NW_HED_Context_s NW_HED_Context_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_Context_ClassPart_s {
  void** unused;
} NW_HED_Context_ClassPart_t;

struct NW_HED_Context_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_Context_ClassPart_t NW_HED_Context;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_Context_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_ADT_DynamicVector_t *vars;
};

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef struct {
  NW_String_t name;
  NW_String_t value;
} NW_HED_Context_Variable_t;


/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_Context_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_Context))

#define NW_HED_ContextOf(_object) \
  (NW_Object_Cast (_object, NW_HED_Context))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_Context_Class_t NW_HED_Context_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_Context_t*
NW_HED_Context_New (void);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Context_Reset (NW_HED_Context_t *thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Context_SetOrAddVariable (NW_HED_Context_t *thisObj, 
                            const NW_String_t *name, 
                            const NW_String_t *value);

NW_HED_EXPORT
const NW_String_t *
NW_HED_Context_GetVariable (const NW_HED_Context_t *thisObj, 
                            const NW_String_t *name);

NW_HED_EXPORT
const NW_HED_Context_Variable_t *
NW_HED_Context_GetEntry (const NW_HED_Context_t *thisObj, 
                         NW_Uint32 index);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Context_RemoveEntry (NW_HED_Context_t *thisObj, 
                            NW_Uint32 index);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_CONTEXT_H */
