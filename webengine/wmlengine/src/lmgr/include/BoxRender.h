/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef BOX_RENDER_H
#define BOX_RENDER_H

#include "nw_gdi_utils.h"
#include "nw_lmgr_box.h"
#include "nw_lmgr_rootbox.h"

#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

#define NO_CHILDREN 0
#define DRAW_CHILDREN 1
#define NOT_DRAW_CHILDREN 2

TBrowserStatusCode
BoxRender_Render (   NW_LMgr_Box_t* box,
                     CGDIDeviceContext* deviceContext,
                     NW_GDI_Rectangle_t* clipRect,
                     NW_LMgr_Box_t* currentBox,
                     NW_Uint8 flags, 
                     NW_Bool hasFocus,
                     NW_LMgr_RootBox_t* rootBox );

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 
