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


#ifndef NW_POSFLOWBOXI_H
#define NW_POSFLOWBOXI_H

#include "nw_lmgr_bidiflowboxi.h"
#include "nw_lmgr_flowboxi.h"
#include "nw_lmgr_posflowbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   virtual methods implementation declarations
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp);
void
_NW_LMgr_PosFlowBox_Destruct (NW_Object_Dynamic_t* dynamicObject);


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Split(NW_LMgr_Box_t* box, 
                           NW_GDI_Metric_t space, 
                           NW_LMgr_Box_t** splitBox,
                           NW_Uint8 flags);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_PostResize(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_GetMinimumContentSize(NW_LMgr_Box_t* box,
                                          NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
NW_Bool
_NW_LMgr_PosFlowBox_HasFixedContentSize(NW_LMgr_Box_t *box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Constrain(NW_LMgr_Box_t* box,
                            NW_GDI_Metric_t constraint);


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Draw (NW_LMgr_Box_t* box,
                           CGDIDeviceContext* deviceContext,
                           NW_Uint8 hasFocus);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Render (NW_LMgr_Box_t* box,
                             CGDIDeviceContext* deviceContext,
                             NW_GDI_Rectangle_t* clipRect,
                             NW_LMgr_Box_t *currentBox,
                             NW_Uint8 flags,
                             NW_Bool* hasFocus,
                             NW_Bool* skipChildren,
                             NW_LMgr_RootBox_t* rootBox );

NW_LMGR_EXPORT
NW_GDI_Metric_t
_NW_LMgr_PosFlowBox_GetBaseline(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_Shift (NW_LMgr_Box_t* box, 
                           NW_GDI_Point2D_t *delta);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
_NW_LMgr_PosFlowBox_Clone (NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_PosFlowBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                    NW_LMgr_FormatContext_t* context);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_POSFLOWBOXI_H */
