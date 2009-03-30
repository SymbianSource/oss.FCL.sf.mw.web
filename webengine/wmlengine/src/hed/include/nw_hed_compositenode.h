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


#ifndef _NW_HED_CompositeNode_h_
#define _NW_HED_CompositeNode_h_

#include "nw_object_aggregate.h"
#include "nw_hed_icompositenode.h"
#include "nw_adt_map.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_CompositeNode_Class_s NW_HED_CompositeNode_Class_t;
typedef struct NW_HED_CompositeNode_s NW_HED_CompositeNode_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_CompositeNode_AddChild_t) (NW_HED_CompositeNode_t* thisObj,
                                    NW_HED_DocumentNode_t* childNode,
                                    void* key);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_CompositeNode_ClassPart_s {
  NW_HED_CompositeNode_AddChild_t addChild;
} NW_HED_CompositeNode_ClassPart_t;

struct NW_HED_CompositeNode_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_Object_Aggregate_ClassPart_t NW_Object_Aggregate;
  NW_HED_CompositeNode_ClassPart_t NW_HED_CompositeNode;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_CompositeNode_s {
  NW_Object_Aggregate_t super;

  /* interface implementation(s) */
  NW_HED_ICompositeNode_t NW_HED_ICompositeNode;

  /* member variables */
  NW_ADT_Map_t* children;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_CompositeNode_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_CompositeNode))

#define NW_HED_CompositeNodeOf(_object) \
  (NW_Object_Cast (_object, NW_HED_CompositeNode))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_CompositeNode_Class_t NW_HED_CompositeNode_Class;

/* ------------------------------------------------------------------------- *
   public method prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_DocumentNode_t*
_NW_HED_CompositeNode_LookupChild (NW_HED_CompositeNode_t* compositeNode,
                                   void* key);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_DeleteChildren (NW_HED_CompositeNode_t* compositeNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_DeleteChild (NW_HED_CompositeNode_t* compositeNode,
                                   void* key);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_RemoveChild (NW_HED_CompositeNode_t* thisObj,
                                   NW_HED_DocumentNode_t* childNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_GetChildren (NW_HED_CompositeNode_t* thisObj,
                                   NW_ADT_Iterator_t** iterator);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_CompositeNode_LookupChild(_compositeNode, _key) \
  (_NW_HED_CompositeNode_LookupChild (\
     NW_HED_CompositeNodeOf (_compositeNode), (_key)))

#define NW_HED_CompositeNode_DeleteChildren(_compositeNode) \
  (_NW_HED_CompositeNode_DeleteChildren (\
     NW_HED_CompositeNodeOf (_compositeNode)))

#define NW_HED_CompositeNode_DeleteChild(_compositeNode, _key) \
  (_NW_HED_CompositeNode_DeleteChild (\
     NW_HED_CompositeNodeOf (_compositeNode), (_key)))

#define NW_HED_CompositeNode_RemoveChild(_compositeNode, _child) \
  (_NW_HED_CompositeNode_RemoveChild (\
     NW_HED_CompositeNodeOf (_compositeNode), \
     NW_HED_DocumentNodeOf (_child)))

#define NW_HED_CompositeNode_GetChildren(_compositeNode, _iterator) \
  (_NW_HED_CompositeNode_GetChildren (\
     NW_HED_CompositeNodeOf (_compositeNode), (_iterator)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_CompositeNode_h_ */
