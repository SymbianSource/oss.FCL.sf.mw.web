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
    $Workfile: wml_url_utils.c $

    Purpose:

        Class: WmlBrowser

        Browser url utilities logic
*/

#include "wml_decoder.h"
#include "wml_url_utils.h"
#include "wml_history.h"
#include "wml_api.h"
#include "wml_elm_attr.h"
#include "nwx_url_utils.h"
#include "nwx_memseg.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

#include "nwx_settings.h"
#include "urlloader_urlloaderint.h"

/* Note: functionality described below has been removed. This code will not always post back to the same
   card deck. This is a violation of the WML specification but it was decided that it would be better to
   allow posts to the same card deck to support the cases where dynamic web pages post to themselves.

   Use the code change history to see the lines removed.

*/

/* Insofar as postfields and <go> elements are concerned, WML version 1.2 defined that
   a <go> to the same deck with postfields was not allowed. This is used to distinguish
   WBXML decks encoded by the gateways which may adhere to the earlier standard. */


/* 
 * Forward declarations for static functions
 */
 static TBrowserStatusCode GetDeckURL(NW_Wml_t* obj, NW_Ucs2 *full_url, NW_Ucs2 **ret_string);
 
/*
 * Returns KBrsrSuccess 
 *         KBrsrFailure - does NOT imply a fatal error
 *         KBrsrOutOfMemory
 *         KBrsrMalformedUrl
 *         NW_STAT_* - propagates TBrowserStatusCode from other calls
 */
TBrowserStatusCode NW_Wml_GetHrefAttr(NW_Wml_t* obj, NW_Wml_Element_t* el,
                               NW_Ucs2 **ret_string)
{
  NW_Wml_t* thisObj = (NW_Wml_t*) obj;
  NW_Ucs2 *raw_url = NULL;
  NW_Ucs2 *escapedUrl = NULL;
  NW_Uint32 escapeLen;
  TBrowserStatusCode status;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_string);

  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, HREF_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &raw_url)) == KBrsrOutOfMemory)
    return status;

  /* reset status - may be a non-fatal value (e.g. FAILURE) */
  status = KBrsrSuccess;

  /* escape the href value */
//??? RFD: May need to modify this along with the rest of the URL encoding cleanup underway.
  escapeLen = NW_Url_EscapeUnwiseLen(raw_url);

  if ((raw_url != NULL) && (escapeLen != NW_Str_Strlen(raw_url)))
  {
    escapedUrl = NW_Str_New(escapeLen);
    if (escapedUrl == NULL) 
    {
      NW_Str_Delete (raw_url);
      return KBrsrOutOfMemory;
    }

//??? RFD: May need to modify this along with the rest of the URL encoding cleanup underway.
    NW_Url_EscapeUnwise(raw_url, escapedUrl);

    NW_Str_Delete(raw_url);
  }
  else
    escapedUrl = raw_url;

  if (escapedUrl)
  {
    status = NW_Wml_GetFullUrl(thisObj, escapedUrl, ret_string);
    NW_Mem_Free(escapedUrl);
  }

  return status;
}


/* 
 * removing the card part
 *
 * if full_url is passed as NULL, then use the last item in
 * the history list
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - full_url is null and the history stack empty
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode GetDeckURL(NW_Wml_t *obj,
                                     NW_Ucs2 *full_url,
                                     NW_Ucs2 **ret_string)
{
  NW_Wml_t* thisObj = (NW_Wml_t*) obj;
  NW_Ucs2 *last_full_url = 0;
  NW_Ucs2 *end = 0;
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_string);

  status = NW_Wml_HistoryGetResponseUrl(thisObj, &last_full_url);
  if (status == KBrsrOutOfMemory)
     return status;

  if (!full_url)
    full_url = last_full_url;
  
  if (!full_url) {
    /* This can happen (e.g. a script just called WMLBrowser.newContext() ... */
    return KBrsrFailure;
  }
  end = NW_Str_Strrchr(full_url, '#');
  if (end != NULL) {
    *ret_string = NW_Str_SegSubstr(full_url, 0, NW_UINT32_CAST(end - full_url), NW_Wml_GetCardSegment(thisObj));
  } else {
    *ret_string = NW_Str_SegNewcpy(full_url, NW_Wml_GetCardSegment(thisObj));
  }

  NW_Str_Delete(last_full_url);

  if (*ret_string == NULL)
    return KBrsrOutOfMemory;

  return KBrsrSuccess;
}


/* 
 * IsCardInCurrentDeck verifies that the passed url (full_url) is in the same
 * deck as the last card processed by the CoreBrowser.
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - card NOT in current deck
 *                 - no past deck
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_IsCardInCurrentDeck(NW_Wml_t *obj,
                                       NW_Ucs2* full_url)
{
  NW_Wml_t* thisObj = obj;
  NW_Ucs2 *parameter_deck_url = 0;
  NW_Ucs2 *history_deck_url = 0;
  NW_Ucs2 *history_full_url = 0;
  NW_Bool res = NW_FALSE;
  TBrowserStatusCode status;

  NW_ASSERT(thisObj);
  NW_ASSERT(thisObj->decoder);

  /* always false if no current deck decoder is NULL - means no past deck */
#if 0 /* TODO what should replace this call */
  if (!(NW_DeckDecoder_GetReader(thisObj->decoder)))
    return KBrsrFailure;
#endif

  if ((status = GetDeckURL(thisObj, full_url, &parameter_deck_url))
      == KBrsrOutOfMemory)
    return status;


  status = NW_Wml_HistoryGetResponseUrl(thisObj, &history_full_url);
  if (status == KBrsrOutOfMemory)
    return status;

  /* can happen if Prev/Popped back to beginning */
  if (!history_full_url)
    return KBrsrFailure;

  if ((status = GetDeckURL(thisObj, history_full_url,
                                  &history_deck_url)) ==
      KBrsrOutOfMemory)
  {
      NW_Str_Delete(history_full_url);
      return status;
  }

  if (!parameter_deck_url || !history_deck_url)
    res = NW_FALSE;
  else
    res = NW_BOOL_CAST(!NW_Str_Strcmp(parameter_deck_url, history_deck_url));

  NW_Str_Delete(history_full_url);

  if (res == NW_TRUE)
    return KBrsrSuccess;
  else
    return KBrsrFailure;
}


/* 
 * Evaluate whether the passed url is a card name, a relative url, 
 * or an absolute url. In all cases, return the full, absolute url. 
 * The caller is responsible for freeing the returned NW_MEM_SEGMENT_MANUAL memory 
 *
 * Returns KBrsrSuccess 
 *         KBrsrFailure - url_param is null
 *              - no history to get previous url
 *              - does NOT imply a fatal error
 *         KBrsrOutOfMemory
 *         KBrsrMalformedUrl
 *         NW_STAT_* - propagates TBrowserStatusCode from other calls
 */
TBrowserStatusCode NW_Wml_GetFullUrl(NW_Wml_t* thisObj, NW_Ucs2 *url_param,
                                     NW_Ucs2 **ret_string)
    {
    TBrowserStatusCode status;
    NW_Ucs2 *deck_url = 0;
    NW_Ucs2* tempUrl = NULL;
    NW_Url_Schema_t scheme;
    NW_Ucs2* safeUrl = NULL;
    NW_Bool isBrowserScheme = NW_FALSE;
    
    NW_ASSERT(thisObj);
    NW_ASSERT(ret_string);
    NW_ASSERT(url_param);

    tempUrl = url_param;
    
    if ((status = GetDeckURL(thisObj, 0, &deck_url)) == KBrsrOutOfMemory)
        {
        goto clean;
        }
    
    // Handle file URLs and relative URLs whose deck_url has a file schema.
    status = NW_Url_GetScheme(deck_url, &scheme);
    if ((status == KBrsrSuccess) && (scheme == NW_SCHEMA_FILE))
        {
        status = NW_Url_GetSchemeNoValidation(url_param, &scheme);
        if ((status != KBrsrSuccess) || (scheme == NW_SCHEMA_FILE))
            {
            // If getting schema was not successful (status != KBrsrSuccess),
            // assume a relative FILE URL, reset the status, and continue
            // as though the schema is FILE, the same as its deck schema
            // determined above.
            status = KBrsrSuccess;

            // Replace all back-slashes with forward-slashes, hence supporting
            // and converting Windows-style file delimiters to UNIX-style file
            // delimiters.
            (void)NW_Str_Strcharreplace(url_param, '\\', '/');
            tempUrl = NW_Url_ConvertFileToEscapeURL(url_param);
            if (tempUrl == NULL)
                {
                status = KBrsrOutOfMemory;
                goto clean;
                }
            }
        }
//??? RFD: This section needs to be corrected along with the rest of the URL-encoding fixes underway.    
    status = NW_Url_ConvertUnicode(tempUrl, &safeUrl);
    if (status != KBrsrSuccess)
        {
        goto clean;
        }

    if (tempUrl != url_param)
        {
        NW_Str_Delete(tempUrl);
        }
    tempUrl = safeUrl;
    safeUrl = NULL;

    status = NW_Url_IsBrowserScheme(tempUrl, &isBrowserScheme);
    if (status == KBrsrOutOfMemory)
        {
        goto clean;
        }

    /* If and only if the schema is a so-called browser schema, then validate
       the URL. Do NOT validate URLs for other schemas because not all of the
       other schemas use URLs that conform to the RFCs. For example, WTAI URLs
       do not conform; paramters are not encoded and are passed directly in
       UCS2 chars. */
    if (isBrowserScheme)
        {
        if (!NW_Url_IsValid(tempUrl))
            {
            status = KBrsrMalformedUrl;
            goto clean;
            }
        }
    
    status = KBrsrSuccess;  //Reset status.
    /* check if url is a card name only. */
    if (tempUrl[0] == '#')
        {
        if (!deck_url)
            {
            status =  KBrsrFailure;
            goto clean;
            }
        
        *ret_string = NW_Str_Newcat(deck_url, tempUrl);
        if (*ret_string == 0)
            {
            status = KBrsrOutOfMemory;
            goto clean;
            }
        }
    // Check for relative urls; URL should not contain a protocol.
    else if (NW_Str_StrncmpConst(tempUrl, WAE_ASC_HTTP_SLASH_SLASH_STR,  WAE_ASC_STRLEN(WAE_ASC_HTTP_SLASH_SLASH_STR)) &&
             NW_Str_StrncmpConst(tempUrl, WAE_ASC_HTTPS_SLASH_SLASH_STR, WAE_ASC_STRLEN(WAE_ASC_HTTPS_SLASH_SLASH_STR)))
        {
        if (!deck_url)
            {
            status =  KBrsrFailure;
            goto clean;
            }
        status = NW_Url_RelToAbs(deck_url, tempUrl, ret_string);
        }
    /* url parameter is absolute url */
    else
        {
        *ret_string = NW_Str_Newcpy(tempUrl);
        if (*ret_string == 0)
            {
            status = KBrsrOutOfMemory;
            goto clean;
            }
        }
    
clean:
        if (tempUrl != url_param)
            {
            NW_Str_Delete(tempUrl);
            }
        return status;
    }


/* 
 * this function will malloc and set the url state for UrlLoadResp
 *
 * RETURN: string or NULL if malloc for the string fails
 */
NW_Ucs2* NW_Wml_CheckRedirection(NW_Ucs2* url_param, NW_Ucs2* redirected_url)
{
  NW_Ucs2* url = 0;

  /* determine whether url is redirected and set state */
  if (url_param != redirected_url)
  {
    NW_ASSERT(redirected_url);
    url = NW_Str_Newcpy(redirected_url);
  }
  else
  {
    NW_ASSERT(url_param);
    url = NW_Str_Newcpy(url_param);
  }
  return url;
}

/*
 * Returns KBrsrSuccess 
 *         KBrsrBadInputParam
 *         KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_MakeLoadRequest(NW_Wml_t *thisObj, NW_Ucs2 *url,
                                   NW_Wml_Element_t *go_el)
{
  NW_Ucs2     *accept_charset = NULL;
  NW_Ucs2     *referer        = NULL;
  NW_Ucs2     *enctype        = NULL; 
  NW_Ucs2     *refNofrag      = NULL;
  NW_Ucs2     *url_frag       = NULL;
  NW_NVPair_t *postfields     = NULL;
  NW_Ucs2     *top            = NULL;
  NW_Bool     is_method_post  = NW_FALSE;
  NW_Bool     cacheAttr       = NW_FALSE;
  TBrowserStatusCode status   = KBrsrSuccess;
  NW_Uint32   ref_len;
  NW_Uint32   frag_len;

  NW_ASSERT(thisObj);

  if (go_el)
  {
    /* sendrefer attr */
    if (NW_DeckDecoder_HasAttributeWithValue(thisObj->decoder, go_el, 
        SENDREFERER_ATTR, WAE_ASC_TRUE_STR, thisObj->var_list))
    {
      status = NW_Wml_HistoryGetResponseUrl(thisObj, &top);
      NW_THROWIF_ERROR(status);

      /* Spec requires the referer be "smallest" relative URL */
      if ((status = NW_Url_AbsToRel(url, top, &referer)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();

      if (status == KBrsrSuccess)
      {
       NW_Mem_Free(top);
	    }
      else 
	    {
        referer = top;
	    }

      top = NULL;

      if( (status = NW_Url_GetFragId(referer, &url_frag, NW_FALSE)) 
          == KBrsrSuccess )
      {
        frag_len = NW_Str_Strlen(url_frag);
        ref_len = NW_Str_Strlen(referer);
        /*url_frag does not includes # */
        
        refNofrag = (NW_Ucs2*) NW_Mem_Malloc((ref_len - frag_len) * sizeof(NW_Ucs2));
        NW_THROWIF_NULL(refNofrag);

        (void)NW_Str_Strncpy(refNofrag, referer, (ref_len - frag_len));
        refNofrag[ref_len - frag_len -1] = 0;

        NW_Str_Delete(url_frag);
        url_frag = NULL;
      }
			else if(status == KBrsrOutOfMemory)
			{
        NW_THROW_ERROR();
			}
			else if(NW_Url_IsValid(referer))
			{
				refNofrag = NW_Str_Newcpy(referer);
			}
    }

    /* method attr - need to know if the method is "get" or "post" */
    is_method_post = NW_DeckDecoder_HasAttributeWithValue(thisObj->decoder, go_el, 
        METHOD_ATTR, WAE_ASC_POST_STR, thisObj->var_list);

    /* accept-charset attr */
    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder,
          go_el, ACCEPT_CHARSET_ATTR, thisObj->var_list, NW_MEM_SEGMENT_MANUAL,
          &accept_charset)) == KBrsrOutOfMemory) {
      NW_THROW_ERROR();
    }

    // Note: code was removed here. See comment at top of file regarding POST

    if (NW_DeckDecoder_HasAttributeWithValue(thisObj->decoder, go_el, CACHECONTROL_ATTR, 
        WAE_ASC_NOCACHE_STR, thisObj->var_list)) {
      cacheAttr = NW_TRUE;
    }


    if ((status = NW_DeckDecoder_GetPostfieldData(thisObj->decoder,
              thisObj->var_list, go_el, &postfields)) != KBrsrSuccess) {
      NW_THROW_ERROR();
    }

    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder,
         go_el, ENCTYPE_ATTR, thisObj->var_list, NW_MEM_SEGMENT_MANUAL, 
         &enctype)) == KBrsrOutOfMemory) {
      NW_THROW_ERROR();
    }
  }

  status = NW_Wml_DoLoadAndSave(thisObj, url, is_method_post, refNofrag, accept_charset,
      NULL, postfields, enctype, (cacheAttr) ? NW_CACHE_NOCACHE : NW_CACHE_NORMAL);

  /* cleanup */

NW_CATCH_ERROR
  if (top != referer)
  {
    NW_Str_Delete (top);
  }

  NW_Str_Delete(accept_charset);
  NW_Str_Delete(referer);
  NW_Str_Delete(enctype);
  NW_Str_Delete(refNofrag);
  NW_Str_Delete(url_frag);
  NW_NVPair_Delete(postfields);

  return status;
}

/*-------------------------------------------------------------------------------
 * NW_Wml_DoLoadAndSave -- this abstracts out the decision about what kind of load
 * to do, and keeps it localized.
 * 
 * RETURN: 
 *      <whatever HttpLoad or UrlLoad can return>, or 
 *      KBrsrOutOfMemory
 *      
 ------------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_DoLoadAndSave(NW_Wml_t *thisObj,
                                 NW_Ucs2  *req_url,
                                 NW_Bool  is_method_post,
                                 NW_Ucs2  *referer,
                                 NW_Ucs2  *accept_charset,
                        NW_Http_Header_t  *header,
                             NW_NVPair_t  *postfields,
                                 NW_Ucs2  *enctype,
                         NW_Cache_Mode_t  cacheMode)
{ 
  TBrowserStatusCode status;

  /* at this point, we know everything we need to know about what will
   * eventually be pushed onto the history stack; but we don't want it
   * to be pushed until the load is actually completed.  So we set the
   * 'last_load' member of the context with the appropriate information,
   * and the response logic will push it onto the stack for us later.
   */
  if (referer || is_method_post || accept_charset || enctype || header) {
    status = NW_Wml_HttpLoad(thisObj->browser_app, req_url, is_method_post,
                             referer, accept_charset, header, 
                             postfields, enctype, cacheMode);
  }
  else {
	  if (NW_Settings_GetSendReferrerHeader()) {
		      status = NW_Wml_UrlLoad(thisObj->browser_app, req_url, postfields, 
                            cacheMode, thisObj->history_resp_url);
	  }
	  else {		
			status = NW_Wml_UrlLoad(thisObj->browser_app, req_url, postfields, 
                            cacheMode, NULL);
	  }

    /*
    ** the following params are all cleaned up by NW_Wml_HttpLoad().  since
    ** we didn't call NW_Wml_HttpLoad() we must delete them explicitly.
    */
    UrlLoader_HeadersFree(header);
    NW_Str_Delete(referer);
    NW_Str_Delete(accept_charset);
    NW_Str_Delete(enctype);
  }     
  return status;
}

