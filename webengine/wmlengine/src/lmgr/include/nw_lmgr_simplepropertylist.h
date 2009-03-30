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


#ifndef NW_LMGR_SIMPLEPROPERTYLIST_H
#define NW_LMGR_SIMPLEPROPERTYLIST_H

#ifndef __E32DEF_H__
#ifdef NULL
#undef NULL
#endif
#include <e32def.h>
#endif

#include "BrsrStatusCodes.h"
#include "nw_lmgr_propertylist.h"
#include "nw_adt_types.h"
#include "nw_adt_vector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_SimplePropertyList_Class_s NW_LMgr_SimplePropertyList_Class_t;
typedef struct NW_LMgr_SimplePropertyList_s NW_LMgr_SimplePropertyList_t;

/* ------------------------------------------------------------------------- *
   virtual method definitions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_SimplePropertyList_ClassPart_s {
  void** unused;
} NW_LMgr_SimplePropertyList_ClassPart_t;

struct NW_LMgr_SimplePropertyList_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_PropertyList_ClassPart_t NW_LMgr_PropertyList;
  NW_LMgr_SimplePropertyList_ClassPart_t NW_LMgr_SimplePropertyList;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_SimplePropertyList_s {
  NW_LMgr_PropertyList_t super;

  /* member variables */
  void* map;
  
  // cached properties are not added to the property map
  void** iCachedProperty;


  /* backup property list */
  NW_LMgr_PropertyList_t* backupPropList;
  /* DOM node associated with the List */
  NW_Object_t* domNode;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_SimplePropertyList_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_SimplePropertyList))

#define NW_LMgr_SimplePropertyListOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_SimplePropertyList))

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TInt 
NW_LMgr_SimplePropertyList_Count (const NW_LMgr_SimplePropertyList_t* propertyList);

NW_LMGR_EXPORT 
TBrowserStatusCode
NW_LMgr_SimplePropertyList_At (const NW_LMgr_SimplePropertyList_t* propertyList,
                               TInt index,
                               NW_LMgr_PropertyList_Entry_t* entry);

NW_LMGR_EXPORT
TBrowserStatusCode 
NW_LMgr_SimplePropertyList_GetNameAt (const NW_LMgr_SimplePropertyList_t* thisObj,
                                      TInt index,
                                      NW_LMgr_PropertyName_t* name);

TBrowserStatusCode
NW_LMgr_SimplePropertyList_CopyEntireList(NW_LMgr_SimplePropertyList_t* aOldPropList,
                                          NW_LMgr_SimplePropertyList_t** aNewPropList);

TBrowserStatusCode 
NW_LMgr_SimplePropertyList_MapLookup( NW_LMgr_SimplePropertyList_t* thisObj,
                                      NW_LMgr_PropertyName_t key, 
                                      NW_LMgr_PropertyList_Entry_t* entry );

NW_LMGR_EXPORT
TBrowserStatusCode 
NW_LMgr_SimplePropertyList_RemoveWithoutDelete (NW_LMgr_SimplePropertyList_t* thisObj,
                                                NW_LMgr_PropertyName_t key);

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_SimplePropertyList_Class_t NW_LMgr_SimplePropertyList_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_SimplePropertyList_t*
NW_LMgr_SimplePropertyList_New (NW_ADT_Vector_Metric_t capacity,
                                NW_ADT_Vector_Metric_t increment);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_SIMPLEPROPERTYLIST_H */
