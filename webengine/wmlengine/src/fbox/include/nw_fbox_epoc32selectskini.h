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


#ifndef NW_FBOX_EPOC32SELECTSKINI_H
#define NW_FBOX_EPOC32SELECTSKINI_H

#include "nw_fbox_selectskini.h"
#include "nw_fbox_epoc32selectskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT  
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp);

NW_FBOX_EXPORT  
void
_NW_FBox_Epoc32SelectSkin_Destruct(NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_SetActive(NW_FBox_SelectSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_Draw(NW_FBox_Skin_t* skin,
                            NW_LMgr_Box_t* box,
                            CGDIDeviceContext* deviceContext,
                            NW_Uint8 hasFocus);
/**
* Gets the baseline for the content of this box.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aLmgrBox Ptr to the box to which this skin belongs.
* @return Baseline value.
*/
NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_Epoc32SelectSkin_GetBaseline(
    NW_FBox_Epoc32SelectSkin_t* aThisObj,
    NW_LMgr_Box_t* aLmgrBox );

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_GetSize(NW_FBox_Skin_t* skin,
                               NW_LMgr_Box_t* box,
                               NW_GDI_Dimension3D_t* size);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_DrawBorder(NW_FBox_ButtonBoxSkin_t* thisObj,
                                  CGDIDeviceContext* deviceContext,
                                  NW_Uint8 hasFocus);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_AddOption(NW_FBox_SelectSkin_t* selectSkin,
                                    NW_FBox_OptionBox_t* optionBox,
                                    NW_LMgr_Box_t** styleBox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_AddOptGrp(NW_FBox_SelectSkin_t* selectSkin,
                                    NW_FBox_OptGrpBox_t* optgrpBox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_DetailsDialog(NW_FBox_SelectSkin_t* skin);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_RefreshText(NW_FBox_SelectSkin_t* skin);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_Split(NW_FBox_SelectSkin_t* selectSkin,
                                NW_GDI_Metric_t space,
                                NW_LMgr_Box_t* *splitBox,
                                NW_Uint8 flags);



#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_EPOC32INPUTSKINI_H */
