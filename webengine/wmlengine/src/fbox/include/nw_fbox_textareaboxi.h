/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_TEXTAREABOXI_H
#define NW_FBOX_TEXTAREABOXI_H

#include "nw_fbox_inputboxi.h"
#include "nw_fbox_textareabox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */


NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_TextAreaBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
NW_Uint16
_NW_FBox_TextAreaBox_GetHeight(NW_FBox_InputBox_t* box);

NW_FBOX_EXPORT
void
_NW_FBox_TextAreaBox_SetHeight(NW_FBox_InputBox_t* box, NW_Uint16 height);

/*
NW_FBOX_EXPORT
NW_Uint16
_NW_FBox_TextAreaBox_GetCols(NW_FBox_TextAreaBox_t* textAreaBox);

NW_FBOX_EXPORT
NW_Uint16
_NW_FBox_TextAreaBox_GetRows(NW_FBox_TextAreaBox_t* textAreaBox);

NW_FBOX_EXPORT
void
_NW_FBox_TextAreaBox_SetCols(NW_FBox_TextAreaBox_t* textAreaBox, NW_Uint16 cols);

NW_FBOX_EXPORT
void
_NW_FBox_TextAreaBox_SetRows(NW_FBox_TextAreaBox_t* textAreaBox, NW_Uint16 rows);
*/

NW_FBOX_EXPORT
NW_FBox_Skin_t*
_NW_FBox_TextAreaBox_InitSkin(NW_FBox_FormBox_t* formBox);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */


#endif  /* NW_FBOX_TEXTAREABOXI_H */
