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


#ifndef NW_FBOX_CHECKBOXI_H
#define NW_FBOX_CHECKBOXI_H

#include "nw_fbox_formboxi.h"
#include "nw_fbox_checkbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_CheckBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
NW_FBox_Skin_t*
_NW_FBox_CheckBox_InitSkin(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
NW_FBox_Interactor_t*
_NW_FBox_CheckBox_InitInteractor(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_CheckBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                               NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_CHECKBOXI_H */
