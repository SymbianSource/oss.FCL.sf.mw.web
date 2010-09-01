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


#ifndef NW_WMLSCRIPT_CONTENTHANDLERI_H
#define NW_WMLSCRIPT_CONTENTHANDLERI_H

#include "nw_hed_contenthandleri.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_WmlScript_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp);

extern
void
_NW_WmlScript_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject);

extern
TBrowserStatusCode
_NW_WmlScript_ContentHandler_Initialize (NW_HED_DocumentNode_t* documentNode,
                                         TBrowserStatusCode aInitStatus);

extern
TBrowserStatusCode
_NW_WmlScript_ContentHandler_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                         NW_LMgr_Box_t** boxTree);

extern
void
_NW_WmlScript_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, 
                                      NW_Bool aDestroyBoxTree);

extern
void
_NW_WmlScript_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode);

extern
const NW_Text_t*
_NW_WmlScript_ContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler);

extern
TBrowserStatusCode
_NW_WmlScript_ContentHandler_Cancel (NW_HED_DocumentNode_t* documentNode,
				     NW_HED_CancelType_t cancelType);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WMLSCRIPT_CONTENTHANDLERI_H */
