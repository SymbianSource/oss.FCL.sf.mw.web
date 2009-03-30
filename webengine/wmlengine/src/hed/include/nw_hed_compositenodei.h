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


#ifndef _NW_HED_CompositeNodeI_h_
#define _NW_HED_CompositeNodeI_h_

#include "nw_object_aggregatei.h"
#include "nw_hed_icompositenode.h"
#include "nw_hed_compositenode.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ICompositeNode_Class_t NW_HED_CompositeNode_ICompositeNode_Class;
NW_HED_EXPORT const NW_Object_Class_t* const NW_HED_CompositeNode_InterfaceList[];

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
void
_NW_HED_CompositeNode_Destruct (NW_Object_Aggregate_t* aggregate);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_AddChild (NW_HED_CompositeNode_t* compositeNode,
                                NW_HED_DocumentNode_t* childNode,
                                void* key);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_ICompositeNode_RemoveChild (NW_HED_ICompositeNode_t* icompositeNode,
                                                  NW_HED_DocumentNode_t* childNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeNode_ICompositeNode_GetChildren (NW_HED_ICompositeNode_t* icompositeNode,
                                                  NW_ADT_Iterator_t** childIterator);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_HED_CompositeNode_AddChild(_compositeNode, _child, _key) \
  (NW_Object_Invoke (_compositeNode, NW_HED_CompositeNode, addChild) ( \
     NW_HED_CompositeNodeOf (_compositeNode), \
     NW_HED_DocumentNodeOf (_child), (_key)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_CompositeNodeI_h_ */
