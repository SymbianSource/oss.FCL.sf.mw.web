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


#ifndef NW_STATICTABLEBOXI_H
#define NW_STATICTABLEBOXI_H

#include "nw_lmgr_formatboxi.h"
#include "nw_lmgr_statictablebox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */




/* ------------------------------------------------------------------------- *
   private macros
 * ------------------------------------------------------------------------- */
#define TABLE_CONTEXT_GET(_ctx, _col, _row) \
  ((_ctx)->cellBoxes[(_row) * ((_ctx)->numCols) + (_col)])
#define TABLE_CONTEXT_SET(_ctx, _col, _row, _cell) \
  (((_ctx)->cellBoxes[(_row) * ((_ctx)->numCols) + (_col)]) = (_cell))

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_StaticTableBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Constrain(NW_LMgr_Box_t* box,
                                  NW_GDI_Metric_t constraint);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Resize(NW_LMgr_Box_t* tableBox,  NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableBox_PostResize(NW_LMgr_Box_t* tableBox);


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Draw (NW_LMgr_Box_t* box,
                              CGDIDeviceContext* deviceContext,
                              NW_Uint8 hasFocus);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_STATICTABLEBOXI_H */
