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


#ifndef NW_FORMATBOXI_H
#define NW_FORMATBOXI_H

#include "nw_lmgr_containerboxi.h"
#include "nw_lmgr_formatbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FormatBox_Resize(NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FormatBox_Constrain(NW_LMgr_Box_t* box,
                             NW_GDI_Metric_t constraint);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FormatBox_FormatChildren(NW_LMgr_FormatBox_t* format, 
                                  NW_LMgr_ContainerBox_t *whose,
                                  NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FormatBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                    NW_LMgr_FormatContext_t* context);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FORMATBOXI_H */
