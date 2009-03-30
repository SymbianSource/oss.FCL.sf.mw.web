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


#ifndef NW_MARKUP_ELEMENTHANDLERI_H
#define NW_MARKUP_ELEMENTHANDLERI_H

#include "nw_object_basei.h"
#include "nw_markup_elementhandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_ElementHandler_Initialize (const NW_Markup_ElementHandler_t* thisObj,
                                     NW_Markup_ContentHandler_t* contentHandler,
                                     NW_DOM_ElementNode_t* elementNode);

NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_ElementHandler_CreateBoxTree (const NW_Markup_ElementHandler_t* thisObj,
                                        NW_Markup_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode,
                                        NW_LMgr_ContainerBox_t* parentBox);

NW_MARKUP_EXPORT
NW_Uint8
_NW_Markup_ElementHandler_ProcessEvent (const NW_Markup_ElementHandler_t* elementHandler,
                                       NW_Markup_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* elementNode,
                                       NW_LMgr_Box_t* box,
                                       NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MARKUP_ELEMENTHANDLERI_H */
