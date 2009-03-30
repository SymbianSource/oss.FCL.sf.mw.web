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
**   File: nwx_cleanup_epoc32.h
**   Purpose:   Cleans up all the rainbow memory on exiting.
**************************************************************************/
#ifndef NWX_CLEANUP_EPOC32_H
#define NWX_CLEANUP_EPOC32_H

/*
** Includes
*/
#include "nwx_defs.h"
#include "BrsrStatusCodes.h"

TBrowserStatusCode NW_Cleanup();

#endif  /* NWX_CLEANUP_EPOC32_H */
