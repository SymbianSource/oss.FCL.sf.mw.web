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


#ifndef NW_ABSTRACTTEXTBOXI_H
#define NW_ABSTRACTTEXTBOXI_H

#include "nw_lmgr_mediaboxi.h"
#include "nw_lmgr_abstracttextbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Text box overrides */

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);
NW_LMGR_EXPORT
void
_NW_LMgr_AbstractTextBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Split(NW_LMgr_Box_t* box, 
                       NW_GDI_Metric_t space, 
                       NW_LMgr_Box_t** splitBox,
                       NW_Uint8 flags);

NW_LMGR_EXPORT 
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_GetMinimumContentSize(NW_LMgr_Box_t* box,
                                       NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
NW_Bool
_NW_LMgr_AbstractTextBox_HasTrailingWS(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Draw (NW_LMgr_Box_t* box,
                       CGDIDeviceContext* deviceContext,
                       NW_Uint8 hasFocus);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_TextBox_Split(NW_LMgr_Box_t* box, 
                       NW_GDI_Metric_t space, 
                       NW_LMgr_Box_t** splitBox,
                       NW_Uint8 flags);
NW_LMGR_EXPORT
NW_GDI_Metric_t
_NW_LMgr_AbstractTextBox_GetBaseline (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_AbstractTextBox_TrimDisplayText (NW_LMgr_AbstractTextBox_t* textBox, 
                                         NW_Text_t* displayText);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ABSTRACTTEXTBOXI_H */
