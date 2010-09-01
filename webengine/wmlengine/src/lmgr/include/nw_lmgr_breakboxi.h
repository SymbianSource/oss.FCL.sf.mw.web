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


#ifndef NW_BREAKBOXI_H
#define NW_BREAKBOXI_H

#include "nw_object_dynamici.h"
#include "nw_lmgr_boxi.h"
#include "nw_lmgr_breakbox.h"
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
_NW_LMgr_BreakBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_BreakBox_Destruct (NW_Object_Dynamic_t* dynamicObject);
  
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_BreakBox_Resize(NW_LMgr_Box_t *box, NW_LMgr_FormatContext_t* context);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_BreakBox_Constrain(NW_LMgr_Box_t *box,
                            NW_GDI_Metric_t constraint);

NW_LMGR_EXPORT
NW_GDI_Metric_t
_NW_LMgr_BreakBox_GetBaseline(NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_BreakBox_Draw (NW_LMgr_Box_t* box,
                   CGDIDeviceContext* deviceContext,
                   NW_Uint8 hasFocus);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif


