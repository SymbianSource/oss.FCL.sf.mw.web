/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_Epoc32ImageButtonBoxSkinI_H
#define NW_FBOX_Epoc32ImageButtonBoxSkinI_H

#include "nw_fbox_imagebuttonboxskini.h"
#include "nw_fbox_epoc32imagebuttonboxskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */


NW_FBOX_EXPORT 
TBrowserStatusCode
_NW_FBox_Epoc32ImageButtonBoxSkin_DrawContent(NW_FBox_ImageButtonBoxSkin_t* skin,
                                           NW_LMgr_Box_t* box,
                                           CGDIDeviceContext* deviceContext,
                                           NW_Uint8 hasFocus);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_Epoc32ImageButtonBoxSkinI_H */
