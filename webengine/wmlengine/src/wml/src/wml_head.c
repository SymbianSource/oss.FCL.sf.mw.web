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
    $Workfile: wml_head.c $

    Purpose:

        Class: WmlBrowser

        Handles head element processing logic
*/

#include "wml_task.h"
#include "wml_history.h"
#include "nwx_url_utils.h"
#include "wml_elm_attr.h"

#include "nwx_mem.h"
#include "nwx_memseg.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"


static void ProcessMetaData(void);


/* 
 * return NW_TRUE if no access conflict, NW_FALSE otherwise 
 *
 * RETURN KBrsrSuccess -> NW_TRUE
 *       KBrsrFailure -> NW_FALSE (access denied - NOT a fatal error)
 *       KBrsrOutOfMemory
 *      NW_STAT_*       -> propagate failures
 */
TBrowserStatusCode NW_Wml_AccessControl(NW_Wml_t* thisObj, NW_Ucs2* url)
{
  NW_Ucs2 *domain = 0, *path = 0, *referring_url = 0;
  NW_Wml_Element_t head_el, el;
  NW_DeckDecoderIter_t iter;
  TBrowserStatusCode status = KBrsrSuccess;

  /* if no head/access element, access is public */
  status = NW_DeckDecoder_GetHeadElement(thisObj->decoder, &head_el);
  if (status != KBrsrSuccess)
  {
    if (status == KBrsrFailure)
      /* The deck has NO head so access is allowed */
      return KBrsrSuccess;
    else
      /* A fatal error ... */
      return status;
  }

  /* if head element has no elements, access is public */
  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, &head_el) != KBrsrSuccess)
    return KBrsrSuccess;

  while (NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    if (el_type == ACCESS_ELEMENT)
    {
      /* if  1) there is an access element
             2) its public attr is not NW_TRUE
             3) there is no prev deck in history to check access
         then fail access control
      */
      status = NW_Wml_HistoryGetPrevResponseUrl(thisObj, &referring_url);
      if (status == KBrsrOutOfMemory)
        return status;

      if (referring_url == NULL)
        return KBrsrFailure;

      /* get domain */
      status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, DOMAIN_ATTR, 
               (NW_Wml_VarList_t *)0, NW_MEM_SEGMENT_MANUAL, &domain);
      if (status == KBrsrOutOfMemory)
      {
        /* Note: NW_Str_Delete() handles NULL pointers */
        NW_Str_Delete(referring_url);
        return status;
      }

      /* get path */
      status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, PATH_ATTR, 
               (NW_Wml_VarList_t *)0, NW_MEM_SEGMENT_MANUAL, &path);
      if (status == KBrsrOutOfMemory)
      {
        /* Note: NW_Str_Delete() handles NULL pointers */
        NW_Str_Delete(referring_url);
        NW_Str_Delete(domain);
        return status;
      }

      /* set defaults if needed */
      if (domain == NULL)
        status = NW_Url_GetHost(url, &domain);
      if (status == KBrsrOutOfMemory || status == KBrsrMalformedUrl)
      {
        /* Note: NW_Str_Delete() handles NULL pointers */
        NW_Str_Delete(referring_url);
        NW_Str_Delete(path);
        NW_Str_Delete(domain);
        return status;
      }

      if (path == NULL) {
        path = NW_Str_NewcpyConst(WAE_URL_PATH_SEP);
      }
      status = NW_Url_AccessOK(domain, path, referring_url, NULL);
      
      /* cleanup */
      /* Note: NW_Str_Delete() handles NULL pointers */
      NW_Str_Delete(referring_url);
      NW_Str_Delete(path);
      NW_Str_Delete(domain);

      /* only one ACCESS element allowed */
      break;

    }

  } 

  return status; 
}


/* 
 * utility 
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - NO meta or head element
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_HandleMetaElements(NW_Wml_t *thisObj)
{
  NW_Ucs2 *http_equiv = 0, *name = 0;
  NW_Ucs2 *content = 0, *scheme = 0;
  NW_Wml_Element_t container, el;
  TBrowserStatusCode status = KBrsrFailure;
  NW_DeckDecoderIter_t iter;

  /* return if no HEAD or HEAD contained elements */
  if (NW_DeckDecoder_GetHeadElement(thisObj->decoder, &container) != KBrsrSuccess)
    return status;

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, &container) != KBrsrSuccess)
    return status;
  
  while (NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    if (el_type == META_ELEMENT)
    {
      http_equiv = NULL;
      name = NULL;
      content = NULL;
      scheme = NULL;

      if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, HTTP_EQUIV_ATTR, 
           (NW_Wml_VarList_t *)0, NW_MEM_SEGMENT_MANUAL, &http_equiv)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();

      if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, NAME_ATTR, 
           (NW_Wml_VarList_t *)0, NW_MEM_SEGMENT_MANUAL, &name)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();
      
      if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, CONTENT_ATTR, 
           (NW_Wml_VarList_t *)0, NW_MEM_SEGMENT_MANUAL, &content)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();
      
      if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, SCHEME_ATTR, 
           (NW_Wml_VarList_t *)0, NW_MEM_SEGMENT_MANUAL, &scheme)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();

      ProcessMetaData();

      NW_Str_Delete(http_equiv);
      http_equiv = NULL;
      NW_Str_Delete(name);
      name = NULL;
      NW_Str_Delete(content);
      content = NULL;
      NW_Str_Delete(scheme);
      scheme = NULL;
    }
  } 

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_Str_Delete(http_equiv);
  NW_Str_Delete(name);
  NW_Str_Delete(content);
  NW_Str_Delete(scheme);

  return status;
}

/*
 * NOTE: to date, this function is a noop.
 *
 * TODO make sure we do not have conflicting charset 
 */
static void ProcessMetaData()
{
}
