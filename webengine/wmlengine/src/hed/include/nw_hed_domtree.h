/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_HED_DOM_TREE_H
#define NW_HED_DOM_TREE_H

#include "nwx_http_header.h"
#include "nwx_buffer.h"

#include <nw_dom_node.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */

typedef struct NW_HED_DomTree_s NW_HED_DomTree_t;


/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

NW_HED_DomTree_t* NW_HED_DomTree_New (NW_Buffer_t* domBuffer,
                                      NW_Http_CharSet_t charset,
                                      NW_Uint32 defaultPublicId,
                                      void* WBXMLEncStrTbl);

TBrowserStatusCode NW_HED_DomTree_Append (NW_HED_DomTree_t** ppDomTree,
                                         NW_Buffer_t* domBuffer,
                                      NW_Http_CharSet_t charset,
                                      NW_Uint32 defaultPublicId,
                                      NW_Uint32 cp_count,
                                      NW_Int32 lastValid,
                                      void* WBXMLEncStrTbl);

void NW_HED_DomTree_Delete (NW_HED_DomTree_t* thisObj);

NW_DOM_Node_t* NW_HED_DomTree_GetRootNode (const NW_HED_DomTree_t* thisObj);

NW_Uint32 NW_HED_DomTree_GetPublicId (const NW_HED_DomTree_t* thisObj);

NW_Uint8 NW_HED_DomTree_GetVersion (const NW_HED_DomTree_t* thisObj);

NW_Uint32 NW_HED_DomTree_GetCharEncoding (const NW_HED_DomTree_t* thisObj);

NW_Buffer_t* NW_HED_DomTree_GetDomBuffer (const NW_HED_DomTree_t* thisObj);

NW_Buffer_t* NW_HED_DomTree_OrphanDomBuffer (NW_HED_DomTree_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_DOM_TREE_H */
