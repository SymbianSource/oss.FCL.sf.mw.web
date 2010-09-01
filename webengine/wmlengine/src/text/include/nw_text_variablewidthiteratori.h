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


#ifndef _NW_Text_VariableWidthIteratorI_h_
#define _NW_Text_VariableWidthIteratorI_h_

#include "nw_text_iteratori.h"
#include "nw_text_variablewidthiterator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_VariableWidthIterator_Construct (NW_Object_Dynamic_t* dynamic,
                                          va_list* argList);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_VariableWidthIterator_GetNext (NW_Text_Iterator_t* iterator,
                                        NW_Ucs4* character);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_VariableWidthIterator_GetOffset (NW_Text_Iterator_t* iterator,
                                          NW_Uint32* offset);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_VariableWidthIterator_SetIndex (NW_Text_Iterator_t* iterator,
                                         NW_Text_Length_t index);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Text_VariableWidthIteratorI_h_ */
