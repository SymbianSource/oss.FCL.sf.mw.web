/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_LMGR_TRANSIENTPROPERTYLIST_H
#define NW_LMGR_TRANSIENTPROPERTYLIST_H

#include "nw_lmgr_simplepropertylisti.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_TransientPropertyList_Class_s NW_LMgr_TransientPropertyList_Class_t;
typedef struct NW_LMgr_TransientPropertyList_s NW_LMgr_TransientPropertyList_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_TransientPropertyList_ClassPart_s {
  void** unused;
} NW_LMgr_TransientPropertyList_ClassPart_t;

struct NW_LMgr_TransientPropertyList_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_PropertyList_ClassPart_t NW_LMgr_PropertyList;
  NW_LMgr_SimplePropertyList_ClassPart_t NW_LMgr_SimplePropertyList;
  NW_LMgr_TransientPropertyList_ClassPart_t NW_LMgr_TransientPropertyList;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_TransientPropertyList_s {
  NW_LMgr_SimplePropertyList_t super;

  /* member variables */
  NW_LMgr_PropertyList_t* basePropertyList;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_TransientPropertyList_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_TransientPropertyList))

#define NW_LMgr_TransientPropertyListOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_TransientPropertyList))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_TransientPropertyList_Class_t NW_LMgr_TransientPropertyList_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_TransientPropertyList_GetBasePropertyList(_object) \
  (NW_OBJECT_CONSTCAST(NW_LMgr_PropertyList_t*) \
     NW_LMgr_TransientPropertyListOf (_object)->basePropertyList)

#define NW_LMgr_TransientPropertyList_Get(_object, _key, _entry) \
  (_NW_LMgr_SimplePropertyList_Get ( \
     NW_LMgr_SimplePropertyListOf (_object), (_key), (_entry)))

#define NW_LMgr_TransientPropertyList_Set(_object, _key, _entry) \
  (_NW_LMgr_SimplePropertyList_Set ( \
     NW_LMgr_SimplePropertyListOf (_object), (_key), (_entry)))

#define NW_LMgr_TransientPropertyList_Remove(_object, _key) \
  (_NW_LMgr_SimplePropertyList_Remove ( \
     NW_LMgr_SimplePropertyListOf (_object), (_key)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_TransientPropertyList_t*
NW_LMgr_TransientPropertyList_New (NW_Uint16 capacity,
                                   NW_Uint16 increment,
                                   NW_LMgr_PropertyList_t* basePropertyList);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_TRANSIENTPROPERTYLIST_H */
