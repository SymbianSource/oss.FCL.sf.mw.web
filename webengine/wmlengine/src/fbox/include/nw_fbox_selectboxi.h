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


#ifndef NW_FBOX_SELECTBOXI_H
#define NW_FBOX_SELECTBOXI_H

#include "nw_fbox_formboxi.h"
#include "nw_fbox_selectbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_SelectBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
NW_Uint16
_NW_FBox_SelectBox_GetHeight(NW_FBox_SelectBox_t* box);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectBox_SetDisplayText(NW_FBox_SelectBox_t* thisObj,
                                NW_Bool bufferModificationOn);

NW_FBOX_EXPORT
NW_FBox_Skin_t*
_NW_FBox_SelectBox_InitSkin(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
NW_FBox_Interactor_t*
_NW_FBox_SelectBox_InitInteractor(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectBox_Reset (NW_FBox_FormBox_t* box);

NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_SelectBox_GetBaseline (NW_LMgr_Box_t* box);


NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_SelectBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                NW_Evt_Event_t* event);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectBox_Split(NW_LMgr_Box_t* thisObj,
                         NW_GDI_Metric_t space,
                         NW_LMgr_Box_t* *splitBox,
                         NW_Uint8 flags);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INPUTBOXI_H */
