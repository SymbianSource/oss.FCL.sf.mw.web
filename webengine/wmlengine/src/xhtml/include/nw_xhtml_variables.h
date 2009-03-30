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


#ifndef NW_XHTML_VARIABLES_H
#define NW_XHTML_VARIABLES_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "nw_text_ucs2.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_Variables_Class_s NW_XHTML_Variables_Class_t;
typedef struct NW_XHTML_Variables_s NW_XHTML_Variables_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_Variables_ClassPart_s {
  void** unused;
} NW_XHTML_Variables_ClassPart_t;

struct NW_XHTML_Variables_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_XHTML_Variables_ClassPart_t NW_XHTML_Variables;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_Variables_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* vector;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_Variables_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_XHTML_Variables))

#define NW_XHTML_VariablesOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_Variables))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_Variables_Class_t NW_XHTML_Variables_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_Variables_Set (NW_XHTML_Variables_t* thisObj,
                        const NW_Text_t* name,
                        const NW_Text_t* value);

NW_Text_t*
NW_XHTML_Variables_Get (const NW_XHTML_Variables_t* thisObj,
                        const NW_Text_t* name);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_VARIABLES_H */
