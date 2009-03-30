/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Utility for converting cxml parser status codes to browser
*     status codes
*     
*
*/


#ifndef NWX_STATUSCODECONVERT_H
#define NWX_STATUSCODECONVERT_H

#include "BrsrStatusCodes.h"
#include <nwx_status.h>

#ifdef __cplusplus
extern "C" {
#endif

TBrowserStatusCode StatusCodeConvert( NW_Status_t cxml_status );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
