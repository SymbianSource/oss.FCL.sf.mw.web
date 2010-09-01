/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_FORMBOXI_H
#define NW_FBOX_FORMBOXI_H

#include "nw_lmgr_activeboxi.h"
#include "nw_fbox_formbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FormBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_FormBox_Destruct(NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FormBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FormBox_Resize (NW_LMgr_Box_t* box,NW_LMgr_FormatContext_t* context);

NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_FormBox_GetBaseline (NW_LMgr_Box_t* box);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FormBox_GetMinimumContentSize (NW_LMgr_Box_t* box, NW_GDI_Dimension2D_t* constraint);

NW_FBOX_EXPORT
NW_Bool
_NW_FBox_FormBox_HasFixedContentSize(NW_LMgr_Box_t *box);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_FormBox_Reset (NW_FBox_FormBox_t* box);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_FORMBOXI_H */
