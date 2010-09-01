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


#ifndef NW_FBOX_INPUTBOXI_H
#define NW_FBOX_INPUTBOXI_H

#include "nw_fbox_formboxi.h"
#include "nw_fbox_inputbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InputBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_InputBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
NW_Uint16
_NW_FBox_InputBox_GetHeight(NW_FBox_InputBox_t* box);

NW_FBOX_EXPORT
void
_NW_FBox_InputBox_SetHeight(NW_FBox_InputBox_t* box, NW_Uint16 height);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InputBox_SetDisplayText(NW_FBox_InputBox_t* thisObj,
                                NW_Bool bufferModificationOn);

NW_FBOX_EXPORT
NW_FBox_Skin_t*
_NW_FBox_InputBox_InitSkin(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
NW_FBox_Interactor_t*
_NW_FBox_InputBox_InitInteractor(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InputBox_Reset (NW_FBox_FormBox_t* box);

NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_InputBox_GetBaseline (NW_LMgr_Box_t* box);

extern
TBrowserStatusCode
_NW_FBox_InputBox_ModifyBuffer(NW_FBox_InputBox_t* inputBox, 
                               NW_Ucs2 c, 
                               NW_Uint32 repeated);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InputBox_Split(NW_LMgr_Box_t* thisObj,
                         NW_GDI_Metric_t space,
                         NW_LMgr_Box_t* *splitBox,
                         NW_Uint8 flags);


NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_InputBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                               NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INPUTBOXI_H */
