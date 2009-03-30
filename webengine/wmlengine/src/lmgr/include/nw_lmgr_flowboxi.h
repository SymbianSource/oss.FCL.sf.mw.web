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


#ifndef NW_FLOWBOXI_H
#define NW_FLOWBOXI_H

#include "nw_lmgr_formatboxi.h"
#include "nw_lmgr_flowbox.h"
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
_NW_LMgr_FlowBox_Resize( NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FlowBox_PostResize( NW_LMgr_Box_t* box);


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FlowBox_Format( NW_LMgr_FormatBox_t* format, 
                         NW_LMgr_Box_t* box, 
                         NW_LMgr_FormatContext_t *context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_FlowBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                    NW_LMgr_FormatContext_t* context);
NW_LMGR_EXPORT
NW_GDI_Metric_t
_NW_LMgr_FlowBox_GetBaseline(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
NW_GDI_Metric_t
NW_LMgr_FlowBox_GetLineStart(NW_LMgr_FlowBox_t* flow, 
                             NW_LMgr_FormatContext_t* context,
                             NW_GDI_Metric_t where);
NW_LMGR_EXPORT
NW_GDI_Metric_t
NW_LMgr_FlowBox_GetLineEnd(NW_LMgr_FlowBox_t* flow, 
                           NW_LMgr_FormatContext_t* context,
                           NW_GDI_Metric_t where);

NW_LMGR_EXPORT
NW_GDI_Metric_t
NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBox_t* flow, 
                               NW_LMgr_FormatContext_t* context);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FLOWBOXI_H */
