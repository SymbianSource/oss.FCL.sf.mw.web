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


#ifndef _NW_HED_CompositeContentHandlerI_h_
#define _NW_HED_CompositeContentHandlerI_h_

#include "nw_hed_contenthandleri.h"
#include "nw_hed_compositenodei.h"
#include "nw_hed_compositecontenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_CompositeNode_Class_t NW_HED_CompositeContentHandler_CompositeNode_Class;
NW_HED_EXPORT const NW_Object_Class_t* const NW_HED_CompositeContentHandler_SecondaryList[];

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_DetachChildren (NW_HED_CompositeContentHandler_t* compositeContentHandler);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_CompositeNode_AddChild (NW_HED_CompositeNode_t* compositeNode,
                                                        NW_HED_DocumentNode_t* documentNode,
                                                        void* key);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_HED_CompositeContentHandler_DetachChildren(_object) \
  (_NW_HED_CompositeContentHandler_DetachChildren ( \
     NW_HED_CompositeContentHandlerOf (_object)))


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp);
NW_HED_EXPORT
void
_NW_HED_CompositeContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_Cancel (NW_HED_DocumentNode_t* documentNode, 
					NW_HED_CancelType_t cancelType);

NW_HED_EXPORT
void
_NW_HED_CompositeContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree);

NW_HED_EXPORT
void
_NW_HED_CompositeContentHandler_Resume (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_CompositeContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_CompositeContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
void
_NW_HED_CompositeContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_DocumentDisplayed (NW_HED_ContentHandler_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_CompositeContentHandlerI_h_ */
