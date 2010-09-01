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


#ifndef NW_FBOX_EPOC32INPUTSKINI_H
#define NW_FBOX_EPOC32INPUTSKINI_H

#include "nw_fbox_inputskini.h"
#include "nw_fbox_epoc32inputskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_Epoc32InputSkin_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_GetSize(NW_FBox_Skin_t* skin,
                           NW_LMgr_Box_t* box,
                           NW_GDI_Dimension3D_t* size);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_DrawText(NW_FBox_InputSkin_t* skin,
                                    CGDIDeviceContext* deviceContext);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_SetDisplayText(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_SetActive(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_SetFocus( 
                        NW_FBox_InputSkin_t* thisObj, NW_Bool focus );

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_DetailsDialog(NW_FBox_InputSkin_t* thisObj);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_Split(NW_FBox_InputSkin_t* thisObj,
                               NW_GDI_Metric_t space,
                               NW_LMgr_Box_t* *splitBox,
                               NW_Uint8 flags);
                               
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_SetVKB(NW_FBox_InputSkin_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_EPOC32INPUTSKINI_H */
