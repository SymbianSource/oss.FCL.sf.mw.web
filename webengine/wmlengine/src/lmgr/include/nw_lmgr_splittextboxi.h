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


#ifndef NW_SPLITTEXTBOXI_H
#define NW_SPLITTEXTBOXI_H

#include "nw_lmgr_abstracttextboxi.h"
#include "nw_lmgr_splittextbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Text box overrides */

TBrowserStatusCode
_NW_LMgr_SplitTextBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);
void
_NW_LMgr_SplitTextBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_Text_t*
_NW_LMgr_SplitTextBox_GetText (NW_LMgr_AbstractTextBox_t *box);

NW_Text_Length_t
_NW_LMgr_SplitTextBox_GetStart (NW_LMgr_AbstractTextBox_t *box);

NW_Text_Length_t
_NW_LMgr_SplitTextBox_GetLength (NW_LMgr_AbstractTextBox_t *box);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_SPLITTEXTBOXI_H */
