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


#ifndef _NW_Text_FixedWidthIteratorI_h_
#define _NW_Text_FixedWidthIteratorI_h_

#include "nw_text_iteratori.h"
#include "nw_text_fixedwidthiterator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_FixedWidthIterator_GetNext (NW_Text_Iterator_t* iterator,
                                     NW_Ucs4* character);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_FixedWidthIterator_GetOffset (NW_Text_Iterator_t* iterator,
                                       NW_Uint32* offset);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_FixedWidthIterator_SetIndex (NW_Text_Iterator_t* iterator,
                                      NW_Text_Length_t index);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Text_FixedWidthIteratorI_h_ */
