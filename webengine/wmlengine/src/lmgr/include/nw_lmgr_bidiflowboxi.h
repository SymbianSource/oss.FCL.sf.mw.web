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


#ifndef NW_BIDIFLOWBOXI_H
#define NW_BIDIFLOWBOXI_H

#include "nw_lmgr_flowboxi.h"
#include "nw_lmgr_bidiflowbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Protected methods */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_BidiFlowBox_HandleInline(NW_LMgr_FlowBox_t* flow, 
                                 NW_LMgr_Box_t* box,
                                 NW_LMgr_FormatContext_t* context);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_BidiFlowBox_HandleInlineContainer(NW_LMgr_FlowBox_t* flow, 
                                          NW_LMgr_ContainerBox_t* container,
                                          NW_LMgr_FormatContext_t* context);

/* LRFlow box overrides */

NW_LMGR_EXPORT TBrowserStatusCode
_NW_LMgr_BidiFlowBox_FormatChildren(NW_LMgr_FormatBox_t* format, 
                                    NW_LMgr_ContainerBox_t *whose,
                                    NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT TBrowserStatusCode
_NW_LMgr_BidiFlowBox_FlowInline(NW_LMgr_FlowBox_t* flow, 
                                NW_LMgr_Box_t* box,
                                NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT TBrowserStatusCode
_NW_LMgr_BidiFlowBox_FlowBlock(NW_LMgr_FlowBox_t* flow, 
                               NW_LMgr_Box_t* box,
                               NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT TBrowserStatusCode
_NW_LMgr_BidiFlowBox_ListItem(NW_LMgr_FlowBox_t* flow, 
                              NW_LMgr_Box_t* item,
                              NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT TBrowserStatusCode
_NW_LMgr_BidiFlowBox_Float(NW_LMgr_FlowBox_t* flow, 
                            NW_LMgr_Box_t* box,
                            NW_LMgr_FormatContext_t* context);
NW_LMGR_EXPORT TBrowserStatusCode
_NW_LMgr_BidiFlowBox_NewLine(NW_LMgr_FlowBox_t* flow,
                             NW_LMgr_FormatContext_t* context,
                             NW_GDI_Metric_t newLine,
							 NW_Bool indentNewLine);

NW_LMGR_EXPORT TBrowserStatusCode
NW_LMgr_BidiFlowBox_Flush(NW_LMgr_FlowBox_t* flow,
                        NW_LMgr_FormatContext_t* context, NW_Bool addLineSpacing);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_BIDIFLOWBOXI_H */
