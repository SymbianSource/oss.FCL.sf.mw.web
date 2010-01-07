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

#ifndef WML_DECK_H
#define WML_DECK_H

#include "nwx_http_header.h"
#include "nw_wml_var.h"
#include "nw_nvpair.h"

#include "nw_wml_defs.h"
#include "nwx_buffer.h"

#include "nw_hed_domtree.h"
#include "nw_hed_domhelper.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */

typedef struct NW_Wml_Deck_s NW_Wml_Deck_t;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

NW_Wml_Deck_t* NW_Wml_Deck_New (NW_Buffer_t* domBuffer,
                               NW_Http_CharSet_t charset,
                               NW_Bool contentWasPlainText);

void NW_Wml_Deck_Delete (NW_Wml_Deck_t* thisObj);

NW_HED_DomTree_t* NW_Wml_Deck_GetDomTree (const NW_Wml_Deck_t* thisObj);

NW_HED_DomHelper_t* NW_Wml_Deck_GetDomHelper (NW_Wml_Deck_t* thisObj,
                                              const NW_Wml_VarList_t* varList);

NW_DOM_Node_t* NW_Wml_Deck_GetCurrentCard (const NW_Wml_Deck_t* thisObj);

NW_DOM_Node_t* NW_Wml_Deck_GetCardByName (NW_Wml_Deck_t* thisObj, 
                                          const NW_String_t* cardName);

TBrowserStatusCode NW_Wml_Deck_SetCurrentCardByName (NW_Wml_Deck_t* thisObj, 
                                              const NW_String_t* cardName);

void NW_Wml_Deck_SetCurrentCardByElement (NW_Wml_Deck_t* thisObj, 
                                          const NW_DOM_Node_t* element);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* WML_DECK_H */
