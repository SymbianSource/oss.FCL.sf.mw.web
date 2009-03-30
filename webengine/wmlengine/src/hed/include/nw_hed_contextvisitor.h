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


#ifndef NW_HED_CONTEXTVISITOR_H
#define NW_HED_CONTEXTVISITOR_H

#include "nw_object_dynamic.h"
#include "nw_hed_context.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- */
typedef struct NW_HED_ContextVisitor_Class_s NW_HED_ContextVisitor_Class_t;
typedef struct NW_HED_ContextVisitor_s NW_HED_ContextVisitor_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_HED_ContextVisitor_ClassPart_s {
  void** unused;
} NW_HED_ContextVisitor_ClassPart_t;

struct NW_HED_ContextVisitor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_ContextVisitor_ClassPart_t NW_HED_ContextVisitor;
};

/* ------------------------------------------------------------------------- */
struct NW_HED_ContextVisitor_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_HED_Context_t* context;
  NW_Uint32 current;
};

/* ------------------------------------------------------------------------- */
#define NW_HED_ContextVisitor_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_HED_ContextVisitor))

#define NW_HED_ContextVisitorOf(_object) \
  (NW_Object_Cast (_object, NW_HED_ContextVisitor))

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ContextVisitor_Class_t NW_HED_ContextVisitor_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
const NW_HED_Context_Variable_t*
NW_HED_ContextVisitor_NextVariable (NW_HED_ContextVisitor_t* thisObj);

NW_HED_EXPORT
void
NW_HED_ContextVisitor_Reset (NW_HED_ContextVisitor_t* thisObj);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
NW_HED_ContextVisitor_Initialize (NW_HED_ContextVisitor_t* thisObj,
                                  const NW_HED_Context_t* context);

NW_HED_EXPORT
NW_HED_ContextVisitor_t*
NW_HED_ContextVisitor_New (NW_HED_Context_t* context);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_CONTEXTVISITOR_H */
