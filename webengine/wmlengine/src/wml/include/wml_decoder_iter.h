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


#ifndef NW_WML_DECODER_ITER_H
#define NW_WML_DECODER_ITER_H

#include "nw_wml_decoder.h"
#include "nw_wml_elements.h"

#include "wml_deck_iter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* typedef struct NW_DeckDecoderIter_s NW_DeckDecoderIter_t; */

struct NW_DeckDecoderIter_s {
  NW_DeckDecoder_t* decoder;
  NW_Wml_Element_t currentElement;

  NW_Wml_DeckIter_t deckIter;
};



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_WML_DECODER_ITER_H */
