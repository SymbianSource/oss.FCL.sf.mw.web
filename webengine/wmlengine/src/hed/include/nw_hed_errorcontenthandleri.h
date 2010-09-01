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


#ifndef NW_HED_ERRORCONTENTHANDLERI_H
#define NW_HED_ERRORCONTENTHANDLERI_H

#include "nw_hed_contenthandleri.h"
#include "nw_hed_errorcontenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ErrorContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp);

NW_HED_EXPORT
void
_NW_HED_ErrorContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_ErrorContentHandler_Initialize (NW_HED_DocumentNode_t* documentNode,
                                        NW_Url_Resp_t* response);

NW_HED_EXPORT
const NW_Text_t*
_NW_HED_ErrorContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler);

NW_HED_EXPORT
NW_LMgr_Box_t*
_NW_HED_ErrorContentHandler_CreateBoxTree (NW_HED_ContentHandler_t* contentHandler);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_ERRORCONTENTHANDLERI_H */
