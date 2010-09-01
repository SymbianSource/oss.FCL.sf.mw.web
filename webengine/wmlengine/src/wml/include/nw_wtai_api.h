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

/*****************************************************************
**    File name:  wml_wtai_api.h
**    Part of: WAE
**    Description:  Provides an interface to WTA functionality.
******************************************************************/

#ifndef NWX_WTAI_API_H
#define NWX_WTAI_API_H

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
**  Macros
**-------------------------------------------------------------------------
*/

/* URI Invocation constants */
#define NW_WTAI_PUBLICLIB_URI_STR  (const char*)"wp"
#define NW_WTAI_MAKECALL_URI_STR   (const char*)"mc"
#define NW_WTAI_SENDDTMF_URI_STR   (const char*)"sd"
#define NW_WTAI_ADDPBENTRY_URI_STR (const char*)"ap"

#define NW_WTAI_INVOCATION_ERROR   -200

/* LocationInfo headers */
#define NW_HTTP_LOCINFO_METHOD          (const char*)"X-NOKIA-WAP-LOC-LOCATIONINFO-METHOD"
#define NW_HTTP_LOCINFO_LAT             (const char*)"X-NOKIA-WAP-LOC-LAT"
#define NW_HTTP_LOCINFO_LONG            (const char*)"X-NOKIA-WAP-LOC-LONG"
#define NW_HTTP_LOCINFO_GSM_INFORMATION (const char*)"X-NOKIA-WAP-LOC-GSM-INFORMATION"
#define NW_HTTP_LOCINFO_LL_FORMAT       (const char*)"X-NOKIA-WAP-LOC-LL-FORMAT"
#define NW_HTTP_LOCINFO_PN_FORMAT       (const char*)"X-NOKIA-WAP-LOC-PN_FORMAT"
#define NW_HTTP_LOCINFO_ACTUAL_LL       (const char*)"X-NOKIA-WAP-LOC-ACTUAL-LL"
#define NW_HTTP_LOCINFO_ACCURACY        (const char*)"X-NOKIA-WAP-LOC-ACCURACY"
#define NW_HTTP_LOCINFO_ERROR           (const char*)"X-NOKIA-WAP-LOC-ERROR"

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/* interface to WTAI */
typedef struct _NW_WtaiApi_s NW_WtaiApi_t;

struct _NW_WtaiApi_s
{
  /* public library */
  TBrowserStatusCode (*wp_makeCall)    (void *ctx,               /* context */ 
                                 const NW_Ucs2 *number);  /* arguments */
  TBrowserStatusCode (*wp_sendDTMF)    (void *ctx, 
                                 const NW_Ucs2 *dtmf);
  TBrowserStatusCode (*wp_addPBEntry)  (void *ctx, 
                                 const NW_Ucs2 *number, const NW_Ucs2 *name,
                                 const NW_Ucs2 *email);

  TBrowserStatusCode (*nokia_locationInfo) 
                                (void *ctx, 
                                 NW_Ucs2 *url,
                                 NW_Ucs2 *ll_format,
                                 NW_Ucs2 *pn_format,
                                 NW_Http_Header_t *header);
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_WTAI_API_H */
