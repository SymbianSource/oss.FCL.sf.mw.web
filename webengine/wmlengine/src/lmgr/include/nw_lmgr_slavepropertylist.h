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


#ifndef NW_LMGR_SLAVEPROPERTYLIST_H
#define NW_LMGR_SLAVEPROPERTYLIST_H

#include "nw_lmgr_propertylist.h"
#include "nw_lmgr_box.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_SlavePropertyList_Class_s NW_LMgr_SlavePropertyList_Class_t;
typedef struct NW_LMgr_SlavePropertyList_s NW_LMgr_SlavePropertyList_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_SlavePropertyList_ClassPart_s {
  void** unused;
} NW_LMgr_SlavePropertyList_ClassPart_t;

struct NW_LMgr_SlavePropertyList_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_PropertyList_ClassPart_t NW_LMgr_PropertyList;
  NW_LMgr_SlavePropertyList_ClassPart_t NW_LMgr_SlavePropertyList;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_SlavePropertyList_s {
  NW_LMgr_PropertyList_t super;

  /* member variables */
  NW_LMgr_Box_t* siblingBox;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_SlavePropertyList_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_SlavePropertyList))

#define NW_LMgr_SlavePropertyListOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_SlavePropertyList))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_SlavePropertyList_Class_t NW_LMgr_SlavePropertyList_Class;

/* ------------------------------------------------------------------------- *
   public method prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_Box_t*
_NW_LMgr_SlavePropertyList_GetMasterBox (const NW_LMgr_SlavePropertyList_t* slavePropertyList);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_SlavePropertyList_GetSiblingBox(_object) \
  (NW_OBJECT_CONSTCAST(NW_LMgr_Box_t*) NW_LMgr_SlavePropertyListOf (_object)->siblingBox)

#define NW_LMgr_SlavePropertyList_GetMasterBox(_object) \
  (_NW_LMgr_SlavePropertyList_GetMasterBox (NW_LMgr_SlavePropertyListOf (_object)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT 
NW_LMgr_Box_t*
NW_LMgr_SlavePropertyList_GetFirstBox (const NW_LMgr_SlavePropertyList_t* thisObj);

NW_LMGR_EXPORT
NW_LMgr_SlavePropertyList_t*
NW_LMgr_SlavePropertyList_New (NW_LMgr_Box_t* siblingBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_SLAVEPROPERTYLIST_H */
