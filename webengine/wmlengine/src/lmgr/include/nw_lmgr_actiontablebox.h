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


#ifndef NW_LMGR_ACTIONTABLEBOX_H
#define NW_LMGR_ACTIONTABLEBOX_H


#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_action.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ActionTableBox_Class_s NW_LMgr_ActionTableBox_Class_t;
typedef struct NW_LMgr_ActionTableBox_s NW_LMgr_ActionTableBox_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ActionTableBox_ClassPart_s {
  void** unused;
} NW_LMgr_ActionTableBox_ClassPart_t;
  
struct NW_LMgr_ActionTableBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_ActiveContainerBox_ClassPart_t NW_LMgr_ActiveContainerBox;
  NW_LMgr_ActionTableBox_ClassPart_t NW_LMgr_ActionTableBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_LMgr_ActionTableBox_s {
  NW_LMgr_ActiveContainerBox_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* actionTable;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ActionTableBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ActionTableBox))

#define NW_LMgr_ActionTableBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ActionTableBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_ActionTableBox_Class_t NW_LMgr_ActionTableBox_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ActionTableBox_NewActionIterator(_actionTableBox) \
  (NW_ADT_VectorIterator_New ( \
    NW_LMgr_ActionTableBoxOf(_actionTableBox)->actionTable, 0, \
    NW_ADT_Vector_AtEnd, NW_ADT_IteratorDirection_Increment))

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_ActionTableBox_t*
NW_LMgr_ActionTableBox_New ();

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ACTIONTABLEBOX_H */
