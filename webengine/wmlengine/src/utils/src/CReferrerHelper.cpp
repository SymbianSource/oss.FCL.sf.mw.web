/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CReferrerHelper.h"
#include "urlloader_urlloaderint.h"
#include "nwx_http_defs.h"	// For NW_URL_METHOD_GET
#include "nwx_settings.h"

// -----------------------------------------------------------------------------
// SetReferrerHeader
// Set Referrer Header
// -----------------------------------------------------------------------------
//
void SetReferrerHeader(NW_HED_UrlRequest_t* urlRequest,
					   NW_Ucs2* referrerUrl)
{
	TUrlLoaderScheme urlScheme;
	NW_Text_Length_t urlLength;
	NW_Uint8 urlFreeNeeded = NW_FALSE;
	NW_Ucs2* requestUrl;

	// Get scheme of request url for later use 
	requestUrl = (NW_Ucs2*) NW_Text_GetUCS2Buffer (urlRequest->url, 
								NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
								&urlLength, &urlFreeNeeded);

	urlScheme = UrlLoader_FindUrlScheme(requestUrl);
    if (urlFreeNeeded) 
	{
		NW_Mem_Free(requestUrl);
	}

	// Make sure Schema is valid before going forward
	if (urlScheme == ESchemeInvalid)
	{	
		return;
	}

	// Don't pass referrer header if it is not get requests 
	if (NW_HED_UrlRequest_GetMethod (urlRequest) == NW_URL_METHOD_GET) 
	{	// If yes, then create referrer header
		TUrlLoaderScheme referrerUrlScheme = UrlLoader_FindUrlScheme(referrerUrl);

		if ( referrerUrlScheme != ESchemeInvalid &&
				 !(referrerUrlScheme == ESchemeHttps && urlScheme == ESchemeHttp) )
		{	// No referrer header sent from https to http, othercase send referrer header
			// First check if referrerUrl has fragment in it or not, if so, remove it

  		    NW_Uint32 referrerUrlLength = NW_Str_Strlen(referrerUrl);
			NW_Ucs2* newReferrerUrl = NW_Str_New(referrerUrlLength + 1); 
			if(newReferrerUrl != NULL) 
			{	// Not out of memory, referrer header will be created
				NW_Uint32 i = 0;
				while ( i < referrerUrlLength && referrerUrl[i] != '#')
				{
					newReferrerUrl[i] = referrerUrl[i];
					i++;
				}					
						
				// Terminate the string
				newReferrerUrl[i] = '\0';

				HttpRequestHeaders* referHeader = (HttpRequestHeaders*)UrlLoader_HeadersNew(NULL,
															   NULL,
															   newReferrerUrl,
															   NULL,
															   0,
                                                               NULL);
				urlRequest->header = referHeader;
				// Free newReferrerUrl
				NW_Str_Delete(newReferrerUrl);
				newReferrerUrl = NULL;
			}
			// else: Out of memory, no referrer header sent
		}
	} 
}
