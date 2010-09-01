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


#ifndef NW_TEXT_ASCIII_H
#define NW_TEXT_ASCIII_H


#include "NW_Text_FixedWidthI.h"
#include "nw_text_ascii.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
NW_Ucs4
_NW_Text_ASCII_GetCharacterAt (const NW_Text_Abstract_t* abstractText,
                               NW_Text_Length_t index);

NW_Text_Length_t
_NW_Text_ASCII_GetSpaceAfter (const NW_Text_Abstract_t* abstractText,
                              NW_Text_Length_t index);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_ASCIII_H */
