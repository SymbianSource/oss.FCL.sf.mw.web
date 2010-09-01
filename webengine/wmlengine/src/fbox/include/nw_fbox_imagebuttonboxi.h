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


#ifndef NW_FBOX_ImageButtonBoxI_H
#define NW_FBOX_ImageButtonBoxI_H

#include "nw_fbox_formboxi.h"
#include "nw_lmgr_animatedimageaggregatei.h"
#include "nw_fbox_imagebuttonbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/* ------------------------------------------------------------------------- *
   protected static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_LMgr_IEventListener_Class_t _NW_FBox_ImageButtonBox_IEventListener_Class;
NW_FBOX_EXPORT const NW_LMgr_AnimatedImageAggregate_Class_t _NW_LMgr_AnimatedImageButtonBoxAggregate_Class;
NW_FBOX_EXPORT const NW_Object_Class_t* const _NW_FBox_ImageButtonBox_SecondaryList[];


/* ------------------------------------------------------------------------- *
   protected method prototype(s)
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_ImageButtonBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_ImageButtonBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
NW_FBox_Skin_t*
_NW_FBox_ImageButtonBox_InitSkin(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
NW_FBox_Interactor_t*
_NW_FBox_ImageButtonBox_InitInteractor(NW_FBox_FormBox_t* formBox);

NW_FBOX_EXPORT
NW_GDI_Metric_t
_NW_FBox_ImageButtonBox_GetBaseline (NW_LMgr_Box_t* box);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_ImageButtonBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus);

 /* Event Listener Interface */
NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_ImageButtonBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                     NW_Evt_Event_t* event);
/* Animation aggregate */
NW_FBOX_EXPORT
NW_Image_AbstractImage_t*
_NW_FBox_ImageButtonBox_AnimatedImageAggregate_GetImage (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_ImageButtonBox_Split(NW_LMgr_Box_t* box, 
                              NW_GDI_Metric_t space, 
                              NW_LMgr_Box_t* *splitBox,
                              NW_Uint8 flags);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_ImageButtonBoxI_H */
