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


#ifndef NW_LMGR_ACTION_H
#define NW_LMGR_ACTION_H

#include "nw_object_dynamic.h"
#include "NW_Text_Abstract.h"
#include "NW_LMgr_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Action_Class_s NW_LMgr_Action_Class_t;
typedef struct NW_LMgr_Action_s NW_LMgr_Action_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
void
(*NW_LMgr_Action_Invoke_t) (NW_LMgr_Action_t* Action);

typedef
const NW_Text_t*
(*NW_LMgr_Action_GetLabel_t) (NW_LMgr_Action_t* Action);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Action_ClassPart_s {
  NW_LMgr_Action_Invoke_t invoke;
  NW_LMgr_Action_GetLabel_t getLabel;
} NW_LMgr_Action_ClassPart_t;

struct NW_LMgr_Action_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Action_ClassPart_t NW_LMgr_Action;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_Action_s {
  NW_Object_Dynamic_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_Action_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_Action))

#define NW_LMgr_ActionOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_Action))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_Action_Class_t NW_LMgr_Action_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_Action_Invoke(_action) \
  (NW_LMgr_Invoke (_action, NW_LMgr_Action, invoke) \
    (_action))

#define NW_LMgr_Action_GetLabel(_action) \
  (NW_LMgr_Invoke (_action, NW_LMgr_Action, getLabel) \
    (_action))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ACTION_H */
