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


#ifndef XHTML_TABLEELEMENTHANDLERI_H
#define XHTML_TABLEELEMENTHANDLERI_H

#include "nw_xhtml_elementhandleri.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
_NW_XHTML_tableElementHandler_Initialize (const NW_XHTML_ElementHandler_t* aElementHandler,
                                          NW_XHTML_ContentHandler_t* aContentHandler,
                                          NW_DOM_ElementNode_t* aElementNode);

extern
TBrowserStatusCode
_NW_XHTML_tableElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* aElementHandler,
                                            NW_XHTML_ContentHandler_t* aContentHandler,
                                            NW_DOM_ElementNode_t* aElementNode,
                                            NW_LMgr_ContainerBox_t* aParentBox);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* XHTML_TABLEELEMENTHANDLERI_H */
