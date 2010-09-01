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


#ifndef NW_FBOX_CLEARACTION_H
#define NW_FBOX_CLEARACTION_H

#include "nw_lmgr_action.h"
#include "nw_text_ucs2.h"
#include "NW_FBox_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_ClearAction_Class_s NW_FBox_ClearAction_Class_t;
typedef struct NW_FBox_ClearAction_s NW_FBox_ClearAction_t;

typedef struct NW_FBox_InputBox_s NW_FBox_InputBox_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_ClearAction_ClassPart_s {
  void** unused;
} NW_FBox_ClearAction_ClassPart_t;

struct NW_FBox_ClearAction_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Action_ClassPart_t NW_LMgr_Action;
  NW_FBox_ClearAction_ClassPart_t NW_FBox_ClearAction;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_FBox_ClearAction_s {
  NW_LMgr_Action_t super;

  NW_Text_UCS2_t* label;
  const NW_FBox_InputBox_t* ownerBox;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_ClearAction_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_ClearAction))

#define NW_FBox_ClearActionOf(_object) \
  (NW_Object_Cast (_object, NW_FBox_ClearAction))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_ClearAction_Class_t NW_FBox_ClearAction_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

NW_FBox_ClearAction_t*
NW_FBox_ClearAction_New (NW_FBox_InputBox_t* inputBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Tempest_FBox_Action_h */
