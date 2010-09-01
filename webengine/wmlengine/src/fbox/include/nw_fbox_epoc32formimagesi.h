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


#ifndef NW_FBOX_EPOC32FORMIMAGESI_H
#define NW_FBOX_EPOC32FORMIMAGESI_H

#include "nw_object_basei.h"
#include "nw_fbox_epoc32formimages.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
struct NW_FBox_FormImages_Entry_s {
  NW_Bool focus;
  NW_Bool state;
  NW_Uint16 imageId;
  NW_Uint16 maskId;
};

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_EPOC32FORMIMAGESI_H */
