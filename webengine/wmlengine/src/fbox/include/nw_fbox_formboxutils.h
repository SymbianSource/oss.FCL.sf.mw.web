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


#ifndef NW_FBOX_FORMBOXUTILS_H
#define NW_FBOX_FORMBOXUTILS_H

#include "nw_text_ucs2.h"
#include "nw_hed_documentroot.h"
#include "nw_lmgr_box.h"
#include "BrsrTypes.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

extern const NW_Ucs2 NW_FBox_FormBoxUtils_Truncate[];
extern const NW_Ucs2 NW_FBox_FormBoxUtils_TrueStr[];
extern const NW_Ucs2 NW_FBox_FormBoxUtils_FalseStr[];

#define NW_FBox_FormBoxUtils_Truncate_len 3

TBrowserStatusCode 
NW_FBox_FormBoxUtils_GetDisplayLength(CGDIDeviceContext* deviceContext,
																			NW_Text_t* text,
                                      CGDIFont* font,
																			NW_Text_Length_t totalLength,
																			NW_Uint16 width,
																			NW_Text_Length_t* displayLength,
																			NW_Bool isNumber);

NW_HED_DocumentRoot_t *
NW_FBox_FormBoxUtils_GetDocRoot(NW_LMgr_Box_t* box);

NW_Uint16
NW_FBox_FormBoxUtils_GetNumEscapeChars(const NW_Ucs2* format);

void
NW_FBox_FormBoxUtils_FillEscapeChars(const NW_Ucs2* format, NW_Ucs2 *input, 
																		 NW_Ucs2 *retValue, NW_Uint16 length);


void
NW_FBox_FormBoxUtils_StripEscapeChars(const NW_Ucs2* format, NW_Ucs2 *input, 
																			NW_Ucs2 *retValue, NW_Uint16 retValueLen);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_FORMBOXUTILS_H */
