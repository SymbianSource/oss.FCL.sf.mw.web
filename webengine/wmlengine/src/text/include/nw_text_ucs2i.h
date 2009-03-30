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


#ifndef NW_TEXT_UCS2I_H
#define NW_TEXT_UCS2I_H


#include "NW_Text_FixedWidthI.h"
#include "nw_text_ucs2.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
NW_Ucs4
_NW_Text_UCS2_GetCharacterAt (const NW_Text_Abstract_t* abstractText,
                              NW_Text_Length_t index);

NW_TEXT_EXPORT
NW_Ucs2*
_NW_Text_UCS2_GetUCS2Buffer (const NW_Text_Abstract_t* textAbstract,
                             NW_Uint16 flags,
                             NW_Text_Length_t* charCount,
                             NW_Bool* freeNeeded);


NW_Text_Length_t
_NW_Text_UCS2_GetSpaceAfter (const NW_Text_Abstract_t* abstractText,
                             NW_Text_Length_t index);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_UCS2I_H */
