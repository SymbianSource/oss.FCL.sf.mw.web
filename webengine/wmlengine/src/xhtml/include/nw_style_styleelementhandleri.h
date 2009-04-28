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


#ifndef NW_XHTML_STYLEELEMENTHANDLERI_H
#define NW_XHTML_STYLEELEMENTHANDLERI_H

#include "nw_xhtml_elementhandleri.h"
#include "nw_style_styleelementhandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_XHTML_styleElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode);

extern
TBrowserStatusCode
_NW_XHTML_styleElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_STYLEELEMENTHANDLERI_H */