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


#ifndef NW_Text_types_h
#define NW_Text_types_h

#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- */
typedef NW_Uint32 NW_Text_Length_t;

typedef enum NW_Text_Direction_e {
  NW_Text_Direction_Forward  = 1,
  NW_Text_Direction_Backward = -1
} NW_Text_Direction_t;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_Text_types_h */
