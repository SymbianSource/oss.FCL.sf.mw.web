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


#ifndef NW_FBOX_INLINEINPUTSKINI_H
#define NW_FBOX_INLINEINPUTSKINI_H

#include "nw_fbox_inputskini.h"
#include "nw_fbox_inlineinputskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */


NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_DrawText(NW_FBox_InputSkin_t* skin,
                                    CGDIDeviceContext* deviceContext);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_MoveCursorRight(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_MoveCursorLeft(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_ClearLastChar(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_SetDisplayText(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_SetActive(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputSkin_DetailsDialog(NW_FBox_InputSkin_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INLINEINPUTSKINI_H */
