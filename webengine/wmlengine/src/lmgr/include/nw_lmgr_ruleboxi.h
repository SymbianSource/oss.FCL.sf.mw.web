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


#ifndef NW_RULEBOXI_H
#define NW_RULEBOXI_H

#include "nw_lmgr_mediaboxi.h"
#include "nw_lmgr_rulebox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Rule box overrides */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_RuleBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_RuleBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_RuleBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus);
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_RuleBox_Constrain(NW_LMgr_Box_t* box,
                           NW_GDI_Metric_t constraint);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_RuleBox_Resize(NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_RULEBOXI_H */
