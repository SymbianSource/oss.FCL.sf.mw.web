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


#include "nw_hed_documentroot.h"
//R->ul #include "urlloader_urlloaderint.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_metainformation_metaelementhandleri.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

// Forward references:
TBrowserStatusCode
NW_XHTML_metaElementHandler_processHttpEquivPragmaNocache (NW_XHTML_ContentHandler_t* contentHandler);


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_metaElementHandler_processHttpEquivCacheControl (NW_Ucs2* content,
                                                          NW_XHTML_ContentHandler_t* contentHandler)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (content != NULL);

  if (NW_Str_StricmpConst(content, "no-cache") == 0) {
    /* Equivalent to pragma/no-cache so call the same method. */
    status = NW_XHTML_metaElementHandler_processHttpEquivPragmaNocache(contentHandler);
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_metaElementHandler_processHttpEquivExpires (NW_Ucs2* content, 
                                                     NW_XHTML_ContentHandler_t* contentHandler)
{
  
  /* Note about return status: invalid syntax is ignored and returned as
     success. Invalid syntax and content will result in the page expiring immediately.
     The only error that is tracked and returned is out-of-memory.
  */

  contentHandler->metaCacheExpires = NW_TRUE;
  contentHandler->metaExpiresDate = NW_Str_Newcpy(content);

  if(contentHandler->metaExpiresDate == NULL)
	{
	return KBrsrOutOfMemory;
	}
    
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_metaElementHandler_processHttpEquivPragmaNocache (NW_XHTML_ContentHandler_t* contentHandler)
{
  contentHandler->metaNoCache = NW_TRUE;
  return KBrsrSuccess;

}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_metaElementHandler_processHttpEquivPragma (NW_Ucs2* content,
                                                    NW_XHTML_ContentHandler_t* contentHandler)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (content != NULL);

  if (NW_Str_StricmpConst(content, "no-cache") == 0) {
    status = NW_XHTML_metaElementHandler_processHttpEquivPragmaNocache(contentHandler);
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_metaElementHandler_processHttpEquivRefresh (NW_Ucs2* content,
                                                     NW_XHTML_ContentHandler_t* contentHandler)
{
  NW_Ucs2* p1;      // ptr to part 1 of content string
  NW_Ucs2* p2;      // ptr to part 2 of content string
  NW_Ucs2* s;       // generic string ptr
  NW_Int32 secs;    // number of seconds before refresh/redirect
  NW_Uint8 loadReason;
  NW_Ucs2* url = NULL;
  NW_Text_t* urlText = NULL;
  NW_Text_t* resolvedUrl = NULL;
  const NW_Ucs2* currentUrlStorage = NULL;
  NW_Uint8 sFreeNeeded = NW_FALSE;
  NW_Text_Length_t charCount;

  NW_ASSERT (content != NULL);

  /* Note about return status: invalid syntax is ignored and returned as
     success.  The only error that is tracked and returned is out-of-memory.
  */

  /* Parse the contents, pulling out the number of seconds and the URL. Syntax:
        n;URL=http://www.website.com/index.html
     where n represents number of seconds to wait before refresh/redirect.

     The syntax is somewhat loose, using Microsoft I.E. v6.0 as the benchmark.
     If the delay is there at all then some sort of refresh will occur.  If a
     delay is specified and no URL specified or the URL syntax is invalid then
     it is treated as an 'implied' refresh to the same page.

     The parsing is destructive, meaning the string value of content will be 
     modified and re-used.  The original pointer is kept so that the memory
     can be properly deallocated.
  */

  /* Split the string into two parts: part1 -- the seconds, part2 -- the URL. */
  p2 = NW_Str_Strchr(content, ';');
  if (p2 != NULL) {
    *p2++ = 0; // replace ';' with null
  }

  /* Check syntax of seconds and convert to numeric value. */
  p1 = NW_Str_TrimInPlace(content, NW_Str_Both);
  if (*p1 == 0) {
    return KBrsrSuccess;     // invalid syntax
  }
  s = p1;
  /* Must be at least one valid digit. */
  if (!NW_Str_Isdigit(*s++)) {
    return KBrsrSuccess;   // invalid syntax
  }
  secs = NW_Str_Atoi(p1);

  /* Any non-digits before the semicolon invalidates the URL in order to mimic
     Microsoft I.E. behavior. */
  while (*s) {
    if (!NW_Str_Isdigit(*s++)) {
      p2 = NULL;
      break;
    }
  }

  /* Check syntax of URL part and extract just the URL. */
  if (p2 != NULL) {
    p2 = NW_Str_TrimInPlace(p2, NW_Str_Both);
    /* Must contain "url", case-insenstive. */
    if (NW_Str_StrnicmpConst(p2, "url", 3) == 0) {
      p2 += 3;
      p2 = NW_Str_TrimInPlace(p2, NW_Str_Start);
      /* Must contain "=". */
      if (NW_Str_StrnicmpConst(p2, "=", 1) == 0) {
        p2 += 1;
        /* Trim is used instead of TrimInPlace in order to get a new string
           object. */
        url = NW_Str_Trim(p2, NW_Str_Start);
        /* If empty string, free it, indicating use of implied URL back to self. */
        if (NW_Str_Strlen(url) == 0) {
          NW_Str_Delete(url);
          url = NULL;
        }
      }
    }
  }

  /* The loadReason determines whether or not a history buffer entry is created
     for the URL.  Assume standard doc load, which creates a history buffer
     entry. */
  loadReason = NW_HED_UrlRequest_Reason_DocLoad;

  NW_TRY (status) {

    currentUrlStorage = NW_HED_UrlRequest_GetRawUrl(
    NW_HED_ContentHandler_GetUrlRequest(NW_HED_ContentHandlerOf(contentHandler)));
    NW_THROW_ON_NULL(currentUrlStorage, status, KBrsrUnexpectedError);

    /* Resolve the URL. */
    if (url == NULL) {
      /* If null, then refresh back to self and do not create a history buffer
         entry. */
      loadReason = NW_HED_UrlRequest_Reason_ShellReload;
      resolvedUrl = (NW_Text_t*)NW_Text_UCS2_New((NW_Ucs2*)currentUrlStorage,
                                                 NW_Str_Strlen(currentUrlStorage),
                                                 NW_Text_Flags_Copy | NW_Text_Flags_Aligned | NW_Text_Flags_NullTerminated);
      NW_THROW_OOM_ON_NULL(resolvedUrl, status);
    } else {
      /* If not null, then resolve the URL. */
      urlText = (NW_Text_t*)NW_Text_UCS2_New(url, NW_Str_Strlen(url), 0);
      NW_THROW_OOM_ON_NULL(urlText, status);
      status = NW_HED_ContentHandler_ResolveURL(contentHandler,
                                                urlText,
                                                &resolvedUrl);
      _NW_THROW_ON_ERROR(status);
      /* ResolveURL function destructs urlText iff success. */
      urlText = NULL;

      /* Compare resolved URL to current page.  If the same, do not create a
         history buffer entry. */
      s = (NW_Ucs2*)NW_Text_GetUCS2Buffer(resolvedUrl,
                                          NW_Text_Flags_Aligned,
                                          &charCount,
                                          &sFreeNeeded);
      NW_THROW_OOM_ON_NULL(s, status);
      if (NW_Str_Stricmp(s, currentUrlStorage) == 0) {
          loadReason = NW_HED_UrlRequest_Reason_ShellReload;
      }
    }

    /* If the delay is zero do not create a history buffer entry. */
    if (secs == 0) {
      loadReason = NW_HED_UrlRequest_Reason_ShellReload;
    }

    /* Pass the data on to the content handler object, which will take it
       from there. */
    status = NW_XHTML_ContentHandler_MetaRefreshInitialize(contentHandler,
                                                           secs,
                                                           resolvedUrl,
                                                           loadReason);
    _NW_THROW_ON_ERROR(status);
    /* MetaRefreshInitialize takes ownership of resolvedUrl iff success. */
    resolvedUrl = NULL;
  }
  NW_CATCH (status) {
    if (status != KBrsrOutOfMemory) {
      status = KBrsrSuccess;
    }
  }
  NW_FINALLY {
    if (url != NULL) {
      NW_Str_Delete(url);
    }
    if (urlText != NULL) {
      NW_Object_Delete(urlText);
    }
    if (resolvedUrl != NULL) {
      NW_Object_Delete(resolvedUrl);
    }
    if (sFreeNeeded){
      NW_Str_Delete(s);
    }
    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_metaElementHandler_Class_t NW_XHTML_metaElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_metaElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_metaElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_metaElementHandler_ProcessEvent
  },
  { /* NW_XHTML_metaElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_metaElementHandler_t NW_XHTML_metaElementHandler = {
  { { &NW_XHTML_metaElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_metaElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                         NW_XHTML_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode)
{
  NW_Ucs2* httpEquiv = NULL;
  NW_Ucs2* content = NULL;

  NW_REQUIRED_PARAM(elementHandler);

  /* The only supported META attribute is 'http-equiv' and of the http-equiv
     directives, only the 'cache-control (for 'no-cache'), 'expires',
     'pragma' (for 'no-cache') and 'refresh' are supported.  All other META
     tags are ignored.  Syntax:
      <META HTTP-EQUIV="cache-control" CONTENT="no-cache">
      <META HTTP-EQUIV="expires" CONTENT="Wed, 09 Aug 2000 08:21:57 GMT">
        where the date must conform to RFC1123
      <META HTTP-EQUIV="pragma" CONTENT="NO-CACHE">
      <META HTTP-EQUIV="refresh" CONTENT="n;URL=http://www.website.com/index.html">
        where n represents number of seconds to wait before refresh/redirect

     Note about return status: in general success is returned since unsupported
     or missing attributes and/or bad attribute values just cause the META tag to
     be ignored.  The only error that is tracked and returned is out-of-memory.
  */

  NW_TRY (status) {

    /* Find the 'http-equiv' attribute and get its string value returned as:
       malloced, UCS-2, aligned, null terminated. */
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                                  elementNode, 
                                                  NW_XHTML_AttributeToken_http_equiv,
                                                  &httpEquiv);
    _NW_THROW_ON_ERROR (status);


    /* All of the supported http-equiv directives have a second attribute
       called 'content'. Find the 'content' attribute and get its string
       value returned as: malloced, UCS-2, aligned, null terminated. */
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                                  elementNode, 
                                                  NW_XHTML_AttributeToken_content,
                                                  &content);
    _NW_THROW_ON_ERROR (status);


    /* Check to see if the http-equiv attribute is one of the supported
       http-equiv directives and process each accordingly. */
    if (NW_Str_StricmpConst(httpEquiv, "cache-control") == 0) {
      status = NW_XHTML_metaElementHandler_processHttpEquivCacheControl(content, contentHandler);
      _NW_THROW_ON_ERROR (status);
    } else if (NW_Str_StricmpConst(httpEquiv, "expires") == 0) {
      status = NW_XHTML_metaElementHandler_processHttpEquivExpires(content, contentHandler);
      _NW_THROW_ON_ERROR (status);
    } else if (NW_Str_StricmpConst(httpEquiv, "pragma") == 0) {
      status = NW_XHTML_metaElementHandler_processHttpEquivPragma(content, contentHandler);
      _NW_THROW_ON_ERROR (status);
    } else if (NW_Str_StricmpConst(httpEquiv, "refresh") == 0) {
      status = NW_XHTML_metaElementHandler_processHttpEquivRefresh(content,
                                                                   contentHandler);
      _NW_THROW_ON_ERROR (status);
    } else {
      NW_FINISHED;
    }
  }
  NW_CATCH (status) {
    if (status != KBrsrOutOfMemory) {
      status = KBrsrSuccess;
    }
  }
  NW_FINALLY {
    NW_Str_Delete(httpEquiv);
    NW_Str_Delete(content);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_metaElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  /* invoke our superclass for completion */
  return NW_XHTML_ElementHandler_Class.NW_XHTML_ElementHandler.
    createBoxTree (elementHandler, contentHandler, elementNode, parentBox);
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_XHTML_metaElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_LMgr_Box_t* box,
                                           NW_Evt_Event_t* event)
{
  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(contentHandler);
  NW_REQUIRED_PARAM(elementNode);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(event);

  /* by default we don't handle the event */
  return 0;
}
