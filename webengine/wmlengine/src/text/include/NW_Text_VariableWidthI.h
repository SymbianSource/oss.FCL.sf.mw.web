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


#ifndef NW_TEXT_VariableWidthI_H
#define NW_TEXT_VariableWidthI_H


#include "NW_Text_AbstractI.h"
#include "NW_Text_VariableWidth.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Text_VariableWidth_ReadCharacter(_object, _offset, _character, _numBytes) \
  (NW_Object_Invoke (_object, NW_Text_VariableWidth, readCharacter) ( \
     NW_Text_VariableWidthOf (_object), (_offset), (_character), (_numBytes)))

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_VariableWidth_CreateIterator (NW_Text_Abstract_t* abstractText,
                                       NW_Text_Direction_t direction,
                                       NW_Text_Iterator_t** iterator);

NW_TEXT_EXPORT
NW_Uint32
_NW_Text_VariableWidth_GetStorageSize (const NW_Text_Abstract_t* abstractText);

NW_TEXT_EXPORT
NW_Ucs4
_NW_Text_VariableWidth_GetCharacterAt (const NW_Text_Abstract_t* abstractText,
                                       NW_Text_Length_t index);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_VariableWidthI_H */
