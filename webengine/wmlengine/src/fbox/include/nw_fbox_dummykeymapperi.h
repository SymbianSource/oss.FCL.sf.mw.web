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


#ifndef NW_FBOX_DUMMYKEYMAPPERI_H
#define NW_FBOX_DUMMYKEYMAPPERI_H

#include "nw_fbox_keymapperi.h"
#include "nw_fbox_dummykeymapper.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes   
 * ------------------------------------------------------------------------- */
NW_Ucs2
_NW_FBox_DummyKeyMapper_MapKey (NW_FBox_KeyMapper_t* thisObj,
                                NW_FBox_KeyMapper_Mode_t* mode,
                                NW_Ucs2 key,
                                NW_Uint8 *repeated);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_DUMMYKEYMAPPERI_H */
