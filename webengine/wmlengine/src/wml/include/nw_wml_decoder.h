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


/*
    $Workfile: nw_wml_decoder.h $

    Purpose:

        Class: DeckDecoder

        This class stores WML bytecode for a retrieved URL. The core browser will
        instruct the DeckDecoder to position in a particular card, then it will ask
        the DeckDecoder to deliver WML elements within this card. A reader is used
        by a DeckDecoder object to read and interpret the WML bytecode.

*/

#ifndef NW_WML_DECODER_H
#define NW_WML_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NW_DeckDecoder_s NW_DeckDecoder_t;

typedef struct NW_DeckDecoderIter_s NW_DeckDecoderIter_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_WML_DECODER_H */
