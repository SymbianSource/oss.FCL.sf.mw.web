/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_hed_urlresponsei.h"
#include "nw_text_ucs2.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*
** TODO make subclasses of this to store XHTML and WML1.x relevant response info
*/

/* ------------------------------------------------------------------------- *
   private static methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static void NW_HED_UrlResponse_DeleteUrl (NW_HED_UrlResponse_t *thisObj)
{
  NW_Object_Delete (thisObj->url);

  if (thisObj->ownRawUrl)
    NW_Mem_Free ((NW_Ucs2*) thisObj->rawUrl);

  thisObj->url = NULL;
  thisObj->rawUrl = NULL;
  thisObj->ownRawUrl = NW_FALSE;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_UrlResponse_Class_t NW_HED_UrlResponse_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_UrlResponse_t),
    /* construct               */ _NW_HED_UrlResponse_Construct,
    /* destruct                */ _NW_HED_UrlResponse_Destruct
  },
  { /* NW_HED_UrlResponse       */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_UrlResponse_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_HED_UrlResponse_t* thisObj;
  const NW_Url_Resp_t* response;
  NW_Text_UCS2_t temp;
  const NW_Text_t* url;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_UrlResponse_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_HED_UrlResponseOf (dynamicObject);

  /* set the member variables -- either response or url should be non-NULL */
  response = (const NW_Url_Resp_t*) va_arg (*argp, const NW_Url_Resp_t*);
  url = (const NW_Text_t*) va_arg (*argp, const NW_Text_t*);

  if (response != NULL) {
    if (response->uri == NULL) {
      return KBrsrUnexpectedError;
    }

    /* TODO what should be done if response->redirected is set? */

    NW_Text_UCS2_Initialize (&temp, response->uri, 0, 0);
    thisObj->url = NW_Text_Copy (&temp, NW_TRUE);
    if (!thisObj->url) {
      return KBrsrOutOfMemory;
    }
  } else if (url != NULL) {
    thisObj->url = (NW_Text_t*) url;
  } else {
    status = KBrsrFailure;
  }

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_UrlResponse_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_UrlResponse_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_UrlResponse_Class));

  /* for convenience */
  thisObj = NW_HED_UrlResponseOf (dynamicObject);

  NW_HED_UrlResponse_DeleteUrl (thisObj);
}

/* ------------------------------------------------------------------------- *
   final functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_t*
NW_HED_UrlResponse_GetUrl (const NW_HED_UrlResponse_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->url;
}

/* ------------------------------------------------------------------------- */
const NW_Ucs2*
NW_HED_UrlResponse_GetRawUrl (const NW_HED_UrlResponse_t* thisObj)
{
  NW_Text_Length_t length;
  NW_HED_UrlResponse_t* uReq = (NW_HED_UrlResponse_t*) thisObj;

  NW_ASSERT (thisObj);

  if (uReq->rawUrl != NULL) {
    return uReq->rawUrl;
  }

  /* convert the url to Unicode and get the storage */
  uReq->rawUrl =
    (NW_Ucs2*) NW_Text_GetUCS2Buffer (uReq->url, NW_Text_Flags_Aligned,
                                      &length, &uReq->ownRawUrl);

  return uReq->rawUrl;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_UrlResponse_UpdateUrl (NW_HED_UrlResponse_t* thisObj, const NW_Text_t* newUrl)
{
  NW_Text_t* url;
  TBrowserStatusCode status = KBrsrSuccess;

  url = NW_Text_Copy (newUrl, NW_TRUE);
  if (url) {
    /* delete the old value */
    NW_HED_UrlResponse_DeleteUrl (thisObj);

    thisObj->url = url;
  }
  else {
    status = KBrsrOutOfMemory;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_UrlResponse_Equal (const NW_HED_UrlResponse_t* urlResponse1, 
                          const NW_HED_UrlResponse_t* urlResponse2)
{
  NW_ASSERT (urlResponse1);
  NW_ASSERT (urlResponse2);

  return (NW_Bool) (NW_Text_Compare (urlResponse1->url,
                                     urlResponse2->url) ? NW_FALSE : NW_TRUE);
}

/* ------------------------------------------------------------------------- */
NW_HED_UrlResponse_t*
NW_HED_UrlResponse_Clone (const NW_HED_UrlResponse_t* thisObj)
{
  NW_Text_t*            url = NULL;
  NW_HED_UrlResponse_t* urlResp;

  NW_THROWIF_NULL (url = NW_Text_Copy (thisObj->url, NW_FALSE));
  
  urlResp = (NW_HED_UrlResponse_t*) NW_Object_New (&NW_HED_UrlResponse_Class, NULL, url);
  NW_THROWIF_NULL (urlResp);

  return urlResp;

NW_CATCH_ERROR
  NW_Object_Delete (url);

  return NULL;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_UrlResponse_t*
NW_HED_UrlResponse_New (const NW_Url_Resp_t* response)
{
  NW_ASSERT (response);

  return (NW_HED_UrlResponse_t*) NW_Object_New (&NW_HED_UrlResponse_Class, 
      response, NULL);
}

