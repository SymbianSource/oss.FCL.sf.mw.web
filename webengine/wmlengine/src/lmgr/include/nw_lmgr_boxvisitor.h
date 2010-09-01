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


#ifndef NW_LMGR_BOXVISITOR_H
#define NW_LMGR_BOXVISITOR_H

#include "nw_object_dynamic.h"
#include "nw_lmgr_box.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_BoxVisitor_Class_s NW_LMgr_BoxVisitor_Class_t;
typedef struct NW_LMgr_BoxVisitor_s NW_LMgr_BoxVisitor_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_BoxVisitor_ClassPart_s {
  void** unused;
} NW_LMgr_BoxVisitor_ClassPart_t;

struct NW_LMgr_BoxVisitor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_BoxVisitor_ClassPart_t NW_LMgr_BoxVisitor;
};

/* ------------------------------------------------------------------------- */
struct NW_LMgr_BoxVisitor_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_LMgr_Box_t* rootBox;
  NW_LMgr_Box_t* currentBox;
};

/* ------------------------------------------------------------------------- */
#define NW_LMgr_BoxVisitor_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_BoxVisitor))

#define NW_LMgr_BoxVisitorOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_BoxVisitor))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_BoxVisitor_Class_t NW_LMgr_BoxVisitor_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_BoxVisitor_GetRootBox(_boxVisitor) \
  (*((NW_LMgr_Box_t* const*) &(_boxVisitor))->rootBox)

#define NW_LMgr_BoxVisitor_GetCurrentBox(_boxVisitor) \
  (*((NW_LMgr_Box_t* const*) &(_boxVisitor))->currentBox)

#define NW_LMgr_BoxVisitor_SetCurrentBox(_boxVisitor, _box) \
  (((_boxVisitor)->currentBox = (_box)), KBrsrSuccess)

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_BoxVisitor_SetRootBox (NW_LMgr_BoxVisitor_t* BoxVisitor,
                               NW_LMgr_Box_t* rootBox);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_BoxVisitor_NextBox (NW_LMgr_BoxVisitor_t* BoxVisitor,
                            NW_Bool* skipChildren);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_BoxVisitor_Initialize (NW_LMgr_BoxVisitor_t* boxVisitor,
                               NW_LMgr_Box_t* rootBox);

NW_LMGR_EXPORT
NW_LMgr_BoxVisitor_t*
NW_LMgr_BoxVisitor_New (NW_LMgr_Box_t* rootBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_BOXVISITOR_H */
