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


#ifndef NW_STATICTABLECELLBOXI_H
#define NW_STATICTABLECELLBOXI_H

#include "nw_lmgr_bidiflowboxi.h"
#include "nw_lmgr_statictablecellbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                      va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_StaticTableCellBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Draw (NW_LMgr_Box_t* box,
                                  CGDIDeviceContext* deviceContext,
                                  NW_Uint8 hasFocus);
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_PostResize(NW_LMgr_Box_t* box);


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Render (NW_LMgr_Box_t* box,
                                    CGDIDeviceContext* deviceContext,
                                    NW_GDI_Rectangle_t* clipRect,
                                    NW_LMgr_Box_t* currentBox,
                                    NW_Uint8 flags, 
                                    NW_Bool* hasFocus,
                                    NW_Bool* skipChildren,
                                    NW_LMgr_RootBox_t* rootBox );
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                             NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Float(NW_LMgr_FlowBox_t* flow, 
                                  NW_LMgr_Box_t* box,
                                  NW_LMgr_FormatContext_t* context);
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_NewLine(NW_LMgr_FlowBox_t* flow, 
                                    NW_LMgr_FormatContext_t* context,
                                    NW_GDI_Metric_t delta,
									NW_Bool indentNewLine);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_STATICTABLECELLBOXI_H */


