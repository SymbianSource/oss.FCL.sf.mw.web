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


#ifndef WML_DECODER_H
#define WML_DECODER_H

#include "nwx_http_header.h"
#include "nw_wml_var.h"
#include "nwx_buffer.h"
#include "wml_elements.h"
#include "wml_decoder_iter.h"
#include "nw_wml_defs.h"
#include "nw_nvpair.h"
#include "nw_wml_decoder.h"
#include "nw_wae_reader.h"
#include "nw_wml_core.h"
#include "nw_hed_context.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" {
#endif

/* TODO make this private by moving it to wml_decoder.c */
struct NW_DeckDecoder_s {
    NW_Wml_Element_t card_el;
    NW_Wml_Deck_t*   domDeck;
};


/* deck related */
NW_DeckDecoder_t* NW_DeckDecoder_New (void);

void NW_DeckDecoder_Delete (NW_DeckDecoder_t* thisObj);

TBrowserStatusCode NW_DeckDecoder_InitializeAndValidate (NW_DeckDecoder_t* thisObj, 
                                                  NW_Buffer_t* domBuffer, 
                                                  NW_Http_CharSet_t charset,
                                                  NW_Bool contentWasPlainText);

TBrowserStatusCode NW_DeckDecoder_GetRootElement (const NW_DeckDecoder_t* thisObj, 
                                           NW_Wml_Element_t* root);

NW_Bool NW_DeckDecoder_GetCardElement (const NW_DeckDecoder_t* thisObj, 
                                       NW_Wml_Element_t* elem);

TBrowserStatusCode NW_DeckDecoder_GetTemplateElement (const NW_DeckDecoder_t* thisObj, 
                                               NW_Wml_Element_t* elem);

TBrowserStatusCode NW_DeckDecoder_GetHeadElement (const NW_DeckDecoder_t *thisObj, 
                                           NW_Wml_Element_t *elem);

TBrowserStatusCode NW_DeckDecoder_SetCardByName (NW_DeckDecoder_t *thisObj, 
                                          const NW_Ucs2* cardName);

TBrowserStatusCode NW_DeckDecoder_SetCardByElement (NW_DeckDecoder_t* thisObj, 
                                             const NW_Wml_Element_t* newElement);

TBrowserStatusCode NW_DeckDecoder_GetCardName (const NW_DeckDecoder_t *thisObj, 
                                        NW_Ucs2 **cardName);

TBrowserStatusCode NW_DeckDecoder_GetCardTitle (const NW_DeckDecoder_t* thisObj, 
                                         const NW_Wml_VarList_t* varList, 
                                         NW_Ucs2** title);

TBrowserStatusCode NW_DeckDecoder_GetDocumentPublicId (const NW_DeckDecoder_t* thisObj, 
                                                NW_Uint32* publicId);

NW_Uint8 NW_DeckDecoder_GetVersion (const NW_DeckDecoder_t* thisObj);

NW_Uint16 NW_DeckDecoder_GetEncoding (const NW_DeckDecoder_t* thisObj);


/* element related */
TBrowserStatusCode NW_DeckDecoder_ReadString (const NW_DeckDecoder_t* thisObj, 
                                       const NW_Wml_Element_t* elem, 
                                       const NW_Wml_VarList_t* varList, 
                                       NW_Mem_Segment_Id_t memScope, 
                                       NW_Ucs2** retString);

NW_Bool NW_DeckDecoder_FirstContainedElement (const NW_DeckDecoder_t* thisObj, 
                                              const NW_Wml_Element_t* container, 
                                              NW_Wml_Element_t* elem);

TBrowserStatusCode NW_DeckDecoder_GetType (const NW_DeckDecoder_t* thisObj, 
                                    const NW_Wml_Element_t* elem, 
                                    NW_Wml_ElType_e* type);

TBrowserStatusCode NW_DeckDecoder_SearchDoElements(NW_DeckDecoder_t* thisObj, 
                                            NW_Wml_VarList_t* varList, 
                                            NW_Wml_Element_t* container, 
                                            NW_Wml_Attr_t attribute, 
                                            NW_Ucs2* searchString);

TBrowserStatusCode NW_DeckDecoder_GetPostfieldData (const NW_DeckDecoder_t* thisObj,
                                             const NW_Wml_VarList_t* varList, 
                                             const NW_Wml_Element_t* elem, 
                                             NW_NVPair_t** postfieldsArg);

TBrowserStatusCode NW_DeckDecoder_GetAttribute (const NW_DeckDecoder_t* thisObj, 
                                         const NW_Wml_Element_t* elem, 
                                         NW_Wml_Attr_t attribute, 
                                         const NW_Wml_VarList_t* varList, 
                                         NW_Mem_Segment_Id_t memScope, 
                                         NW_Ucs2** attributeValue);

NW_Bool NW_DeckDecoder_HasAttributeWithValue (const NW_DeckDecoder_t* thisObj, 
                                              const NW_Wml_Element_t* elem,
                                              NW_Wml_Attr_t attribute,
                                              const char* attributeValue,
                                              const NW_Wml_VarList_t* varList);

NW_Bool NW_DeckDecoder_HasContent (const NW_DeckDecoder_t* thisObj, 
                                   const NW_Wml_Element_t* elem);

TBrowserStatusCode NW_DeckDecoder_CopyElement (const NW_DeckDecoder_t* thisObj, 
                                        const NW_Wml_Element_t* srcEl, 
                                        NW_Wml_Element_t* destEl);

NW_HED_Context_t*
NW_DeckDecoder_VarListToContext (const NW_Wml_VarList_t* varList);

TBrowserStatusCode
NW_DeckDecoder_ContextToVarList (const NW_HED_Context_t* context,
                                 NW_Wml_VarList_t* varList,
                                 NW_Mem_Segment_Id_t memScope);

/* deck-iterator related */
TBrowserStatusCode NW_DeckDecoderIter_Initialize (NW_DeckDecoderIter_t *thisObj,
                                           const NW_DeckDecoder_t* decoder,
                                           const NW_Wml_Element_t* container);

TBrowserStatusCode NW_DeckDecoderIter_InitializeAll_Children (NW_DeckDecoderIter_t *thisObj,
                                           const NW_DeckDecoder_t* decoder,
                                           const NW_Wml_Element_t* container);
NW_Bool NW_DeckDecoderIter_GetNextElement (NW_DeckDecoderIter_t* thisObj, 
                                           NW_Wml_Element_t* elem);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WML_DECODER_H */
