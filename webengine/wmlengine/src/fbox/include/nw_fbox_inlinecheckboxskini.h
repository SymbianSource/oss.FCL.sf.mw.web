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


#ifndef NW_FBOX_INLINECHECKBOXSKINI_H
#define NW_FBOX_INLINECHECKBOXSKINI_H

#include "nw_fbox_checkboxskini.h"
#include "nw_fbox_inlinecheckboxskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InlineCheckBoxSkin_Draw(NW_FBox_Skin_t* skin,
                                  NW_LMgr_Box_t* box,
                                  CGDIDeviceContext* deviceContext,
                                  NW_Uint8 hasFocus);

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InlineCheckBoxSkin_GetSize(NW_FBox_Skin_t* skin,
                                     NW_LMgr_Box_t* box,
                                     NW_GDI_Dimension3D_t* size);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INLINECHECKBOXSKINI_H */