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
*
*/


/*****************************************************************
**    File name:  wml_telephony_api.h
**    Part of: WAE
**    Description:  Provides an interface to Telephony functionality from iHTML/xHTML
******************************************************************/

#ifndef NWX_TELEPHONY_API_H
#define NWX_TELEPHONY_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_defs.h"
#include "nwx_string.h"
#include "nwx_http_header.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/* interface to telephony API */
typedef struct _NW_TelephonyApi_s NW_TelephonyApi_t;

struct _NW_TelephonyApi_s
{
  /* Make Call - a valid number should be sent */
  TBrowserStatusCode (*makeCall)(const NW_Ucs2* number);

  /* add phone book entry */
  TBrowserStatusCode (*addPBEntry)(const NW_Ucs2* number, const NW_Ucs2* name, const NW_Ucs2* email);
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_TELEPHONY_API_H */
