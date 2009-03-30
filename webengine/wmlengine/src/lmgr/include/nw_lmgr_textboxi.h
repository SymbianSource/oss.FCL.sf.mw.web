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


#ifndef NW_TEXTBOXI_H
#define NW_TEXTBOXI_H

#include "nw_lmgr_abstracttextboxi.h"
#include "nw_lmgr_textbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Text box overrides */

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_TextBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);
NW_LMGR_EXPORT
void
_NW_LMgr_TextBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
NW_Text_t*
_NW_LMgr_TextBox_GetText (NW_LMgr_AbstractTextBox_t *box);

NW_LMGR_EXPORT 
NW_Text_Length_t
_NW_LMgr_TextBox_GetStart (NW_LMgr_AbstractTextBox_t *box);

NW_LMGR_EXPORT
NW_Text_Length_t
_NW_LMgr_TextBox_GetLength (NW_LMgr_AbstractTextBox_t *box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_TextBox_ConserveLeadingWS(NW_LMgr_Box_t* box, NW_Bool conserveWS);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXTBOXI_H */
