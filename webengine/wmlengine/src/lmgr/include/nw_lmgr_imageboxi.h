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


#ifndef NW_IMAGEBOXI_H
#define NW_IMAGEBOXI_H

#include "nw_lmgr_mediaboxi.h"
#include "nw_lmgr_imagebox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Image box overrides */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImageBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_ImageBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImageBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus);
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImageBox_GetMinimumContentSize(NW_LMgr_Box_t* box,
                                        NW_GDI_Dimension2D_t *size);

TBrowserStatusCode
_NW_LMgr_ImageBox_Resize (NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context);


TBrowserStatusCode
_NW_LMgr_ImageBox_Constrain(NW_LMgr_Box_t *box,
                       NW_GDI_Metric_t constraint);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_IMAGEBOXI_H */
