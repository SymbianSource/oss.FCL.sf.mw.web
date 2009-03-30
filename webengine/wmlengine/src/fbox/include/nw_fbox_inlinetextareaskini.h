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


#ifndef NW_FBOX_INLINETEXTAREASKINI_H
#define NW_FBOX_INLINETEXTAREASKINI_H

#include "nw_fbox_textareaskini.h"
#include "nw_fbox_inlinetextareaskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_InlineTextAreaSkin_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_DrawText(NW_FBox_InputSkin_t* thisObj,
                                     CGDIDeviceContext* deviceContext);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_MoveCursorRight(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_MoveCursorLeft(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_ClearLastChar(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_SetDisplayText(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_SetActive(NW_FBox_InputSkin_t* skin);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineTextAreaSkin_DetailsDialog(NW_FBox_InputSkin_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INLINETEXTAREASKINI_H */
