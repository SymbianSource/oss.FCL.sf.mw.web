/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/***************************************************************************
**   File: nwx_osu_epoc32.h
**   Purpose:   Cleans up all the rainbow memory on exiting.
**************************************************************************/
#ifndef NWX_OSU_EPOC32_H
#define NWX_OSU_EPOC32_H

/*
** Includes
*/
#include "nwx_defs.h"
#include "nwx_osu.h"

#ifdef __cplusplus
extern "C" {
#endif

void NW_Osu_DeleteContext();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_OSU_EPOC32_H */
