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


#ifndef NW_FBOX_SELECTSKINI_H
#define NW_FBOX_SELECTSKINI_H

#include "nw_fbox_buttonboxskini.h"
#include "nw_fbox_selectskin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_SelectSkin_Destruct (NW_Object_Dynamic_t* dynamicObject);


NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectSkin_Draw(NW_FBox_Skin_t* skin,
                        NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_SelectSkin_GetSize(NW_FBox_Skin_t* skin,
                           NW_LMgr_Box_t* box,
                           NW_GDI_Dimension3D_t* size);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_SelectSkin_GetSelectBox(_skin) \
  (NW_FBox_SelectBoxOf(NW_FBox_Skin_GetFormBox(_skin)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INPUTSKINI_H */
