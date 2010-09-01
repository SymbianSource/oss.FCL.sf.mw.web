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
    $Workfile: wml_history.c $

    Purpose:

        Class: WmlBrowser

        Maintain browser's history state logic
*/

#include "wml_history.h"

#include "nwx_string.h"
#include "nwx_http_header.h"
#include "nw_evlog_api.h"
#include "nw_wae.h"
#include "nwx_ctx.h"
#include "nw_hed_documentroot.h"
#include "BrsrStatusCodes.h"
#include <MemoryManager.h>

/*------------------------------------------------------------------------
 * StringCopy -- makes a copy of src string, puts its address in *dest,
 * and returns true iff the copy was successful (failure means we ran
 * out of memory).  If src is NULL, the 'copy' just consists of a copy
 * of the NULL pointer, and is considered successful.
 -----------------------------------------------------------------------*/
static NW_Bool StringCopy(NW_Ucs2 **dest, NW_Ucs2 *src)
{
  if (src) {
    *dest = NW_Str_Newcpy(src);
    if (*dest != NULL) {
      return NW_TRUE;
    } else {
      return NW_FALSE;
    }
  } else {
    *dest = NULL;
    return NW_TRUE;
  }
}

/* Create a new history entry object */
static NW_Wml_History_t *Wml_HistoryNewEntry(NW_Ucs2 *req_url)
{
  NW_Wml_History_t *entry;

  /* Note: NULL args are acceptable */
  entry = (NW_Wml_History_t*) NW_Mem_Malloc(sizeof(NW_Wml_History_t));
  if (entry != NULL) {
    if (StringCopy(&(entry->req_url), req_url) == NW_TRUE) {
       return entry;
    } else {
      NW_Str_Delete(entry->req_url);
      entry->req_url = NULL;
    }
    NW_Mem_Free(entry);
  }

  return NULL;
}

/* Creates a copy of a Wml History entry using information from HED history.
   Returns a pointer to the new entry or NULL when failed.*/
static NW_Wml_History_t *HEDtoWmlHistoryEntry(NW_HED_HistoryEntry_t *entry)
{
  NW_Ucs2          *reqUrl;
  NW_Wml_History_t *dest;

  reqUrl = (NW_Ucs2 *)NW_HED_UrlRequest_GetRawUrl(entry->urlRequest);
  dest = Wml_HistoryNewEntry(reqUrl);
  return (dest);
}

/* Delete a history entry object */
void NW_Wml_HistoryDeleteEntry(NW_Wml_History_t *entry)
{
  if (entry != NULL) {
    NW_Str_Delete(entry->req_url);
    entry->req_url = NULL;
    NW_Mem_Free(entry);
  }
  return;
}


/*-------------------------------------------------------------------------
 * NW_Wml_HistoryInit -- initialize the history stack
 ------------------------------------------------------------------------*/
void NW_Wml_HistoryInit(NW_Wml_t *wml)
{
  NW_ASSERT(wml != NULL);

  wml->history_resp_url = NULL;
  return;
}


/*--------------------------------------------------------------------------
 * NW_Wml_HistoryClear -- clear all history entries, but preserve the
 *                        current entry
 -------------------------------------------------------------------------*/

void NW_Wml_HistoryClear(NW_Wml_t *wml)
{
  NW_HED_DocumentRoot_t *dr;
  NW_HED_HistoryStack_t *history;
  NW_HED_HistoryEntry_t *entry;

  NW_ASSERT(wml != NULL);

  dr = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (wml->browser_app);

  history = NW_HED_DocumentRoot_GetHistoryStack (dr);
  if (history == NULL) {
    return;
  }
  /* remove the current entry from the history */
  entry = NW_HED_HistoryStack_RemoveEntry (history, NW_HED_HistoryStack_Direction_Current); 
  if (entry == NULL) {
    return;
  }

  /* delete all of the remaining entries */
  NW_HED_HistoryStack_DeleteAll (history);

  /* add the current entry back */
  NW_HED_HistoryStack_PushEntry (history, entry);
}


/*---------------------------------------------------------------------------
 * NW_Wml_HistoryForIntradeckLoad
 *
 * Crates a new entry in the global history to reflect intradeck load
 * -------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_HistoryPushIntraDeckLoad(NW_Wml_t *wml,
                                            NW_Ucs2  *reqUrl, 
                                            NW_Ucs2  *respUrl)
{
  NW_HED_DocumentRoot_t *dr;
  NW_HED_ContentHandler_t *thisObj;
  NW_HED_HistoryEntry_t *newEntry = NULL;
  NW_Text_UCS2_t        requestUrl;
  NW_Text_UCS2_t        responseUrl;
  TBrowserStatusCode           status = KBrsrOutOfMemory;

  NW_ASSERT(wml != NULL);
  NW_ASSERT(reqUrl != NULL);
  NW_ASSERT(respUrl != NULL);

  /* get the document-root and content-handler */
  dr = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (wml->browser_app);
  NW_THROWIF_NULL (dr);
  NW_THROWIF_NULL (thisObj = NW_HED_ContentHandlerOf (wml->browser_app));

  /* wrap the urls as NW_Text_t */
  NW_THROWIF_ERROR (status = NW_Text_UCS2_Initialize (&requestUrl, reqUrl, NW_Str_Strlen (reqUrl), 0));
  NW_THROWIF_ERROR (status = NW_Text_UCS2_Initialize (&responseUrl, respUrl, NW_Str_Strlen (respUrl), 0));

  /* create the new entry */
  NW_THROWIF_NULL (newEntry = NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry (thisObj, 
      &requestUrl, &responseUrl));

  /* push the new entry on the history stack */
  NW_THROWIF_ERROR (status = NW_HED_HistoryStack_PushEntry (dr->historyStack, newEntry));

  /* update the content handler to point to the associated history entry */
  NW_THROWIF_ERROR (status = NW_HED_ContentHandler_SetAssociatedHistoryEntry (thisObj, newEntry));

  return KBrsrSuccess; 

NW_CATCH_ERROR
  
  NW_Object_Delete (newEntry);
  return status;
}


/*-------------------------------------------------------------------------- 
 * NW_Wml_HistoryToPrev
 *  RETURN: KBrsrSuccess
 *          KBrsrFailure
 *          KBrsrOutOfMemory
 * Returns the current history entry in NW_Wml_History_t structure (when wmlEntry!=0). 
 * Return (1)value KBrsrSuccess indicates a successful move to the previous 
 * entry in the HED history and (2)wmlEntry when requested. 
 * KBrsrFailure indicates an absence of the previous entry.
 * KBrsrOutOfMemory is returned when not enough memory to create wmlEntry
 *
 * Note -- the 'wmlEntry' argument may be NULL, indicating that the caller 
 * only wants to move to the previous entry.
 *------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_HistoryToPrev(NW_Wml_t *wml, NW_Wml_History_t **wmlEntry) 
{
  NW_HED_DocumentRoot_t *dr;
  TBrowserStatusCode           nwStatus = KBrsrSuccess;

  NW_ASSERT(wml != NULL);

  dr = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (wml->browser_app);

  if (wmlEntry)
  {
    NW_HED_HistoryEntry_t *HEDhistEntry;
    HEDhistEntry = NW_HED_HistoryStack_GetEntry (dr->historyStack,
                             NW_HED_HistoryStack_Direction_Current);
    NW_ASSERT(HEDhistEntry);

    *wmlEntry = HEDtoWmlHistoryEntry(HEDhistEntry);
    if (*wmlEntry == NULL) {
      nwStatus = KBrsrOutOfMemory;
    }
  }

  if (nwStatus == KBrsrSuccess) {
    nwStatus = NW_HED_HistoryStack_SetCurrent(dr->historyStack,
                           NW_HED_HistoryStack_Direction_Previous);
  }
  return nwStatus; 
}


/*---------------------------------------------------------------------
 * sets the history_resp_url member of NW_Wml_t.  This is separate from
 * SetLastLoad, because at the time SetLastLoad is called, we don't
 * know what the response url will be (could be redirected).
 */
TBrowserStatusCode NW_Wml_HistorySetResponseUrl(NW_Wml_t* wml, NW_Ucs2 *resp_url)
{
  NW_ASSERT(wml != NULL);
  NW_ASSERT(resp_url != NULL);

  NW_Str_Delete(wml->history_resp_url);
  wml->history_resp_url = NW_Str_Newcpy(resp_url);

  if (wml->history_resp_url == NULL) {
    return KBrsrOutOfMemory;
  } else {
    return KBrsrSuccess;
  }
}

/*----------------------------------------------------------------------
 * Gets the 'req_url member of top-of-stack, and makes a new copy of it
 * in the mem_scope given.  The address is return in *ret_string; if the req_url
 * was null, *ret_string is also null.
 ---------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_HistoryGetRequestUrl(NW_Wml_t *wml,
                                        NW_Ucs2 **req_url) 
{
  NW_Ucs2               *url;
  NW_HED_DocumentRoot_t *dr;
  NW_HED_HistoryEntry_t *currEntry;
  TBrowserStatusCode           nwStatus = KBrsrSuccess;

  NW_ASSERT(wml != NULL);
  NW_ASSERT(req_url != NULL);
  *req_url = NULL;

  dr = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (wml->browser_app);

  currEntry = NW_HED_HistoryStack_GetEntry (dr->historyStack,
                         NW_HED_HistoryStack_Direction_Current);
  NW_ASSERT(currEntry);

  url = (NW_Ucs2 *)NW_HED_UrlResponse_GetRawUrl(currEntry->urlResponse);
  if (url) {
    *req_url = NW_Str_Newcpy(url);
    if (!*req_url) {
      nwStatus = KBrsrOutOfMemory;
    }
  }
  return nwStatus;
}

/*----------------------------------------------------------------------
 * Gets the history_resp_url member of NW_Wml_t, and makes a new copy of it
 * in the mem_scope given.  The address is return in *resp_url; if the 
 * response_url was null, *resp_url is also null.
 ---------------------------------------------------------------------*/
 
TBrowserStatusCode NW_Wml_HistoryGetResponseUrl(NW_Wml_t *wml,
                                         NW_Ucs2  **resp_url) 
{
  NW_ASSERT(wml != NULL);
  NW_ASSERT(resp_url != NULL);

  if (wml->history_resp_url  != NULL) {
    *resp_url = NW_Str_Newcpy(wml->history_resp_url);
    if (*resp_url == NULL)
      return KBrsrOutOfMemory;
  } else {
    *resp_url = NULL;
  }

  return KBrsrSuccess;
}

/*----------------------------------------------------------------------
 * Gets the responseUrl member of the previous-history-entry, and makes a new copy of it
 * in the mem_scope given.  The address is return in *resp_url; if the 
 * response_url was null, *resp_url is also null.
 ---------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_HistoryGetPrevResponseUrl(NW_Wml_t *wml,
                                             NW_Ucs2  **resp_url)
{
  NW_Ucs2               *url;
  NW_HED_DocumentRoot_t *dr;
  NW_HED_HistoryEntry_t *currEntry;

  NW_ASSERT(wml != NULL);
  NW_ASSERT(resp_url != NULL);
  *resp_url = NULL;

  dr = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (wml->browser_app);

  currEntry = NW_HED_HistoryStack_GetEntry (dr->historyStack,
                         NW_HED_HistoryStack_Direction_Previous);
  if (currEntry == NULL) {
    return KBrsrFailure;
  }

  url = (NW_Ucs2 *)NW_HED_UrlResponse_GetRawUrl(currEntry->urlResponse);
  if (url) {
    *resp_url = NW_Str_Newcpy(url);
    if (!*resp_url) {
      return KBrsrOutOfMemory;
    }
  }
  return KBrsrSuccess;
}


/*-------------------------------------------------------------------------------
 * NW_Wml_HistoryPrevCard
 * 
 * RETURN: KBrsrSuccess -- there is a previous card, and you got info about it
 *         KBrsrFailure -- there is no previous card, and you didn't get much
 *         KBrsrOutOfMemory -- you're out of memory
 *
 * Note -- you get a mutated copy of the history-stack entry in NW_Wml_History_t format.
 * Any non-null string pointers are replaced by pointers to brand new copies of the
 * original strings, in the mem_scope of your choice.
 ------------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_HistoryPrevCard(NW_Wml_t          *wml, 
                                   NW_Wml_History_t **entry)
{
  NW_HED_DocumentRoot_t *dr;
  NW_HED_HistoryEntry_t *prevEntry;
  TBrowserStatusCode           nwStatus = KBrsrSuccess;

  NW_ASSERT(wml != NULL);
  NW_ASSERT(entry != NULL);

  dr = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (wml->browser_app);

  prevEntry = NW_HED_HistoryStack_GetEntry(dr->historyStack,
                                  NW_HED_HistoryStack_Direction_Previous);
  if (prevEntry == NULL) {
    nwStatus = KBrsrFailure;
    *entry = NULL;
  }
  else {
    *entry = HEDtoWmlHistoryEntry(prevEntry);
    if (*entry == NULL) {
      nwStatus = KBrsrOutOfMemory;
    }
  }
  return nwStatus;
}
