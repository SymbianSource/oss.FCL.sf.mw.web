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


#ifndef _NW_HED_TreeVisitorI_h_
#define _NW_HED_TreeVisitorI_h_

#include "nw_object_dynamici.h"
#include "nw_hed_treevisitor.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_TreeVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argList);

NW_HED_EXPORT
void
_NW_HED_TreeVisitor_Destruct (NW_Object_Dynamic_t* dynamicObject);

TBrowserStatusCode
_NW_HED_TreeVisitor_NextSibling (NW_HED_TreeVisitor_t* treeVisitor,
                                 NW_HED_DocumentNode_t* parentNode,
                                 NW_HED_DocumentNode_t* node);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_HED_TreeVisitor_NextSibling(_treeVisitor, _parentNode, _node) \
  (_NW_HED_TreeVisitor_NextSibling ( \
     NW_HED_TreeVisitorOf (_treeVisitor), \
     NW_HED_DocumentNodeOf (_parentNode), \
     NW_HED_DocumentNodeOf (_node)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_TreeVisitorI_h_ */
