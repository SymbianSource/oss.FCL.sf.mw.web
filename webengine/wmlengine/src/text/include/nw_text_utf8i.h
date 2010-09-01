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


#ifndef NW_TEXT_UTF8I_H
#define NW_TEXT_UTF8I_H


#include "NW_Text_VariableWidthI.h"
#include "nw_text_utf8.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_UTF8_ReadCharacter (NW_Text_VariableWidth_t* textVariableWidth,
                             NW_Int32 offset,
                             NW_Ucs4* character,
                             NW_Uint32* numBytes);

NW_Text_Length_t
_NW_Text_UTF8_GetSpaceAfter (const NW_Text_Abstract_t* abstractText,
                             NW_Text_Length_t index);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_UTF8I_H */
