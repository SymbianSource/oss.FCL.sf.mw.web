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


/*
    $Workfile: wml_url_utils.h $

    Purpose:

        Class: WmlBrowser

        Browser's url utility functions (interface file)

*/

#ifndef WML_URL_UTILS_H
#define WML_URL_UTILS_H

#include "nw_wml_core.h"
//R
//#include "urlloader_urlloaderint.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

TBrowserStatusCode NW_Wml_GetHrefAttr(NW_Wml_t* obj, NW_Wml_Element_t *el, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_Wml_IsCardInCurrentDeck(NW_Wml_t* obj, NW_Ucs2 *full_url);        

TBrowserStatusCode NW_Wml_GetFullUrl(NW_Wml_t* obj, NW_Ucs2 *rel_url, NW_Ucs2 **ret_string);

NW_Ucs2* NW_Wml_CheckRedirection(NW_Ucs2* url_param, NW_Ucs2* redirected_url);

TBrowserStatusCode NW_Wml_MakeLoadRequest(NW_Wml_t *thisObj, 
                                   NW_Ucs2  *url, 
                           NW_Wml_Element_t *go_el);

TBrowserStatusCode NW_Wml_DoLoadAndSave(NW_Wml_t *thisObj,
                                 NW_Ucs2  *req_url,
                                 NW_Bool  is_method_post,
                                 NW_Ucs2  *referer,
                                 NW_Ucs2  *accept_charset,
                        NW_Http_Header_t  *header,
                             NW_NVPair_t  *escaped_postdata,
                                 NW_Ucs2  *enctype,
                         NW_Cache_Mode_t  cacheMode);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* WML_URL_UTILS_H */
