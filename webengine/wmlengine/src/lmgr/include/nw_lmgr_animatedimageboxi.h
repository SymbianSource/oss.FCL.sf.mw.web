/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_ANIMATEDIMAGEBOXI_H
#define NW_ANIMATEDIMAGEBOXI_H

#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_mediaboxi.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- *
   protected static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_AnimatedImageAggregate_Class_t NW_LMgr_AnimatedImageBox_AnimatedImageAggregate_Class;
NW_LMGR_EXPORT const NW_Object_Class_t* const _NW_LMgr_AnimatedImageBox_SecondaryList[];

/* ------------------------------------------------------------------------- *
   protected method prototype(s)
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AnimatedImageBox_Construct (NW_Object_Dynamic_t* dynamicObject, va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_AnimatedImageBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AnimatedImageBox_Draw (NW_LMgr_Box_t* box, 
                                CGDIDeviceContext* deviceContext,
                                NW_Uint8 hasFocus);

/* Animation aggregate */
NW_LMGR_EXPORT
NW_Image_AbstractImage_t*
_NW_LMgr_AnimatedImageBox_AnimatedImageAggregate_GetImage (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* NW_ANIMATEDIMAGEBOXI_H */
