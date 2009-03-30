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


#include "nw_hed_urlrequesti.h"

#include "nw_text_ucs2.h"
#include "urlloader_urlloaderint.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private static methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static void NW_HED_UrlRequest_DeleteUrl (NW_HED_UrlRequest_t *thisObj)
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
NW_HED_UrlRequest_Class_t NW_HED_UrlRequest_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_UrlRequest_t),
    /* construct               */ _NW_HED_UrlRequest_Construct,
    /* destruct                */ _NW_HED_UrlRequest_Destruct
  },
  { /* NW_HED_UrlRequest       */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_UrlRequest_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_HED_UrlRequest_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_UrlRequest_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_HED_UrlRequestOf (dynamicObject);

  /* set the member variables */
  thisObj->url = (NW_Text_t*) va_arg (*argp, const NW_Text_t*);
  NW_ASSERT (thisObj->url != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->url, &NW_Text_Abstract_Class));
  
  thisObj->url = NW_Text_Copy (thisObj->url, NW_TRUE);
  if (!thisObj->url) {
    status = KBrsrOutOfMemory;
  }
  
  thisObj->method = (NW_Uint8) va_arg (*argp, NW_Uint32);
  thisObj->header = (NW_Http_Header_t*) va_arg (*argp, NW_Http_Header_t*);
  thisObj->body = (NW_Buffer_t*) va_arg (*argp, const NW_Buffer_t*);
  thisObj->reason = (NW_Uint8) va_arg (*argp, NW_Uint32);
  thisObj->loadMode = (NW_HED_UrlRequest_LoadMode_t) va_arg (*argp, NW_HED_UrlRequest_LoadMode_t);
  thisObj->loadType = (NW_Uint8) va_arg (*argp, NW_Uint32);  
  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_UrlRequest_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_UrlRequest_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_UrlRequest_Class));

  /* for convenience */
  thisObj = NW_HED_UrlRequestOf (dynamicObject);

  NW_HED_UrlRequest_DeleteUrl (thisObj);
  UrlLoader_HeadersFree (thisObj->header);
  NW_Buffer_Free (thisObj->body);
}

/* ------------------------------------------------------------------------- *
   final functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_UrlRequest_UpdateUrl (NW_HED_UrlRequest_t* thisObj, const NW_Text_t* newUrl)
{
  NW_Text_t* url;
  TBrowserStatusCode status = KBrsrSuccess;

  url = NW_Text_Copy (newUrl, NW_TRUE);
  if (url) {
    /* delete the old value */
    NW_HED_UrlRequest_DeleteUrl (thisObj);

    thisObj->url = url;
  }
  else {
    status = KBrsrOutOfMemory;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
const NW_Text_t*
NW_HED_UrlRequest_GetUrl (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);
  NW_ASSERT (thisObj->url);

  return thisObj->url;
}

/* ------------------------------------------------------------------------- */
const NW_Ucs2*
NW_HED_UrlRequest_GetRawUrl (const NW_HED_UrlRequest_t* thisObj)
{
  NW_Text_Length_t length;
  NW_HED_UrlRequest_t* uReq = (NW_HED_UrlRequest_t*) thisObj;

  if (uReq->rawUrl != NULL)
    return uReq->rawUrl;

  /* convert the url to Unicode and get the storage */
  uReq->rawUrl = (NW_Ucs2*) NW_Text_GetUCS2Buffer (uReq->url, 
        NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
        &length, &uReq->ownRawUrl);

  return uReq->rawUrl;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
NW_HED_UrlRequest_GetMethod (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->method;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
NW_HED_UrlRequest_GetLoadType (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->loadType;
}

/* ------------------------------------------------------------------------- */
const NW_Http_Header_t*
NW_HED_UrlRequest_GetHeader (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->header;
}

/* ------------------------------------------------------------------------- */
const NW_Buffer_t*
NW_HED_UrlRequest_GetBody (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->body;
}

/* ------------------------------------------------------------------------- */
NW_Cache_Mode_t
NW_HED_UrlRequest_GetCacheMode (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->cacheMode;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_UrlRequest_IsTopLevelRequest (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return (NW_Bool) (thisObj->reason != NW_HED_UrlRequest_Reason_DocLoadChild && thisObj->reason != NW_HED_UrlRequest_Reason_DocLoadHead);
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_UrlRequest_IsHistoricRequest (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  switch (thisObj->reason)
  {
    case NW_HED_UrlRequest_Reason_ShellPrev:
    case NW_HED_UrlRequest_Reason_ShellNext:
    case NW_HED_UrlRequest_Reason_ShellReload:
    case NW_HED_UrlRequest_Reason_DocPrev:
    case NW_HED_UrlRequest_Reason_RestorePrev:
    case NW_HED_UrlRequest_Reason_RestoreNext:
      return NW_TRUE;

    case NW_HED_UrlRequest_Reason_ShellLoad:
    case NW_HED_UrlRequest_Reason_DocLoad:
    case NW_HED_UrlRequest_Reason_DocLoadChild:
    case NW_HED_UrlRequest_Reason_Undefined:
      return NW_FALSE;
  }

  return NW_FALSE;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_UrlRequest_Equal (const NW_HED_UrlRequest_t* urlRequest1, 
    const NW_HED_UrlRequest_t* urlRequest2)
{
  /* TODO update to check more than just the url */
  return (NW_Bool) (NW_Text_Compare (urlRequest1->url,
                                     urlRequest2->url) ? NW_FALSE : NW_TRUE);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_UrlRequest_SetMethod (NW_HED_UrlRequest_t* thisObj, NW_Uint8 method)
{
  NW_ASSERT (thisObj);

  thisObj->method = method;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_UrlRequest_SetReason (NW_HED_UrlRequest_t* thisObj, NW_Uint8 reason)
{
  NW_ASSERT (thisObj);

  thisObj->reason = reason;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_UrlRequest_SetCacheMode (NW_HED_UrlRequest_t* thisObj, NW_Cache_Mode_t cacheMode)
{
  NW_ASSERT (thisObj);

  thisObj->cacheMode = cacheMode;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
NW_HED_UrlRequest_GetReason (const NW_HED_UrlRequest_t* thisObj)
{
  NW_ASSERT (thisObj);

  return thisObj->reason;
}

/* ------------------------------------------------------------------------- */
NW_HED_UrlRequest_t*
NW_HED_UrlRequest_Clone (const NW_HED_UrlRequest_t* thisObj)
{
  NW_Http_Header_t* header = NULL;
  NW_Buffer_t*      body = NULL;
  NW_HED_UrlRequest_t* urlReq;

  if (thisObj->header != NULL)  {
//R->ul    NW_THROWIF_NULL (header = UrlLoader_HeadersCopy (thisObj->header));
  }
   
  if (thisObj->body != NULL)
    {
    NW_THROWIF_NULL (body = NW_Buffer_New (0));
    NW_THROWIF_ERROR (NW_Buffer_CopyBuffers (body, thisObj->body));
    }

  NW_THROWIF_NULL (urlReq = NW_HED_UrlRequest_New (thisObj->url, thisObj->method, 
      header, body, thisObj->reason, thisObj->loadMode, thisObj->loadType));

  return urlReq;

NW_CATCH_ERROR
  UrlLoader_HeadersFree (header);
  NW_Buffer_Free (body);

  return NULL;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_UrlRequest_t*
NW_HED_UrlRequest_New (NW_Text_t* url, NW_Uint8 method,
    NW_Http_Header_t* header, NW_Buffer_t* body, NW_Uint8 reason, NW_HED_UrlRequest_LoadMode_t loadMode, NW_Uint8 loadType)
{
  NW_ASSERT (url);
  
  return (NW_HED_UrlRequest_t*) NW_Object_New (&NW_HED_UrlRequest_Class, 
      url, (NW_Uint32)method, header, body, (NW_Uint32)reason, loadMode, (NW_Uint32)loadType);
}

