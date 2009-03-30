/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_XHTML_OBJECT_OBJELEMENTHANDLERI_H
#define NW_XHTML_OBJECT_OBJELEMENTHANDLERI_H

#include "nw_xhtml_elementhandleri.h"
#include "XhtmlObjectElementHandlerOOC.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* GENERATED */

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_XHTML_ObjectElementHandler_Initialize (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode);

extern
TBrowserStatusCode
_NW_XHTML_ObjectElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
        NW_LMgr_ContainerBox_t* parentBox);  

extern
NW_Uint8
_NW_XHTML_ObjectElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
        NW_LMgr_Box_t* aBox, NW_Evt_Event_t* aEvent);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_OBJECT_OBJELEMENTHANDLERI_H */
