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


#ifndef _NW_HED_ICompositeNode_h_
#define _NW_HED_ICompositeNode_h_

#include "nw_object_secondary.h"
#include "nw_hed_documentnode.h"
#include "nw_adt_iterator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ICompositeNode_Class_s NW_HED_ICompositeNode_Class_t;
typedef struct NW_HED_ICompositeNode_s NW_HED_ICompositeNode_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_ICompositeNode_RemoveChild_t) (NW_HED_ICompositeNode_t* thisObj,
                                        NW_HED_DocumentNode_t* childNode);

typedef
TBrowserStatusCode
(*NW_HED_ICompositeNode_GetChildren_t) (NW_HED_ICompositeNode_t* thisObj,
                                        NW_ADT_Iterator_t** iterator);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ICompositeNode_ClassPart_s {
  NW_HED_ICompositeNode_RemoveChild_t removeChild;
  NW_HED_ICompositeNode_GetChildren_t getChildren;
} NW_HED_ICompositeNode_ClassPart_t;

struct NW_HED_ICompositeNode_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_HED_ICompositeNode_ClassPart_t NW_HED_ICompositeNode;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_ICompositeNode_s {
  NW_Object_Secondary_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_ICompositeNode_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_ICompositeNode))

#define NW_HED_ICompositeNodeOf(_object) \
  (NW_Object_Cast (_object, NW_HED_ICompositeNode))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ICompositeNode_Class_t NW_HED_ICompositeNode_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_ICompositeNode_RemoveChild(_object, _child) \
  (NW_Object_Invoke (_object, NW_HED_ICompositeNode, removeChild) ( \
     NW_HED_ICompositeNodeOf (_object), NW_HED_DocumentNodeOf (_child)))

#define NW_HED_ICompositeNode_GetChildren(_object, _iterator) \
  (NW_Object_Invoke (_object, NW_HED_ICompositeNode, getChildren) ( \
     NW_HED_ICompositeNodeOf (_object), (_iterator)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_ICompositeNode_h_ */
