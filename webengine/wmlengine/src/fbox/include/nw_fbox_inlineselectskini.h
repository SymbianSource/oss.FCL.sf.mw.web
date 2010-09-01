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


#ifndef NW_FBOX_INLINESELECTSKINI_H
#define NW_FBOX_INLINESELECTSKINI_H

#include "nw_fbox_selectskini.h"
#include "nw_fbox_inlineselectskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT  
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp);

NW_FBOX_EXPORT  
void
_NW_FBox_InlineSelectSkin_Destruct(NW_Object_Dynamic_t* dynamicObject);


NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_SetActive(NW_FBox_SelectSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_Draw(NW_FBox_Skin_t* skin,
                            NW_LMgr_Box_t* box,
                            CGDIDeviceContext* deviceContext,
                            NW_Uint8 hasFocus);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_GetSize(NW_FBox_Skin_t* skin,
                               NW_LMgr_Box_t* box,
                               NW_GDI_Dimension3D_t* size);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_DrawBorder(NW_FBox_ButtonBoxSkin_t* thisObj,
                                  CGDIDeviceContext* deviceContext,
                                  NW_Uint8 hasFocus);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_AddOption(NW_FBox_SelectSkin_t* selectSkin,
                                    NW_FBox_OptionBox_t* optionBox,
                                    NW_LMgr_Box_t** stylebox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_AddOptGrp(NW_FBox_SelectSkin_t* selectSkin,
                                    NW_FBox_OptGrpBox_t* optGrpBox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_DetailsDialog(NW_FBox_SelectSkin_t* skin);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineSelectSkin_RefreshText(NW_FBox_SelectSkin_t* skin);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_InlineINPUTSKINI_H */
