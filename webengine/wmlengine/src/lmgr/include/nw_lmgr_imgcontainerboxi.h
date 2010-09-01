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


#ifndef NW_IMGCONTAINERBOXI_H
#define NW_IMGCONTAINERBOXI_H

#include "nw_lmgr_mediaboxi.h"
#include "nw_lmgr_imgcontainerbox.h"
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
_NW_LMgr_ImgContainerBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_ImgContainerBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_Split(NW_LMgr_Box_t* box, NW_GDI_Metric_t space, 
                               NW_LMgr_Box_t* *splitBox, NW_Uint8 flags);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_GetMinimumContentSize (NW_LMgr_Box_t *box,
                                                NW_GDI_Dimension2D_t *size);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_Draw (NW_LMgr_Box_t* box,
                               CGDIDeviceContext* deviceContext,
                               NW_Uint8 hasFocus);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
