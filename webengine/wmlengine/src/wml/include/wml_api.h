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


#ifndef WML_API_H
#define WML_API_H

#include "nw_wml_api.h"
#include "wml_decoder.h"
//R
//#include "urlloader_urlloaderint.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* interface to the loaders */
TBrowserStatusCode NW_Wml_HttpLoad(void    *wml1xCH,
                      const NW_Ucs2 *url,
                            NW_Bool is_method_post,
                      const NW_Ucs2 *referer,
                      const NW_Ucs2 *accept_charset,
                   NW_Http_Header_t *header,
                        NW_NVPair_t *postfields,
                      const NW_Ucs2 *enctype,
              const NW_Cache_Mode_t cacheMode);

TBrowserStatusCode NW_Wml_UrlLoad(void     *wml1xCH,
                      const NW_Ucs2 *url, 
                        NW_NVPair_t *postfields,
              const NW_Cache_Mode_t cacheMode, 
			  NW_Ucs2* referrerUrl);

/* Request previous card load as the result of executing a PREV task */
TBrowserStatusCode NW_Wml_DoPrevLoad(void *wml1xCH);

/* interface to control navigation events.
   Note: WML Interpreter invokes this function when ever it is navigating to 
   a new URL. If this function returns a result "KBrsrSuccess", navigation 
   is performed,  for all other values the navigation is canceled.
*/
TBrowserStatusCode NW_Wml_CanNavigateToUri(void);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* WML_API_H */
