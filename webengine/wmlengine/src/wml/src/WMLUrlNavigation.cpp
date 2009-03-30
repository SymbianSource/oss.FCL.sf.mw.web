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
    $Workfile: wml_url_navigation.c $

    Purpose:

        Class: WmlBrowser

        Browser url navigation logic

*/
#include "nwx_defs.h"
#include "nwx_memseg.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"
#include "wml_elm_attr.h"

#include "wml_task.h"
#include "wml_api.h"
#include "wml_history.h"
#include "wml_url_utils.h"
#include "wml_decoder.h"

#include "nw_hed_documentroot.h"
#include "mvcshell.h"
#include "nw_lmgr_rootbox.h"
#include "nw_wml1x_wml1xcontenthandler.h"
#include "BrsrStatusCodes.h"


static TBrowserStatusCode LoadDeckError(NW_Wml_t* thisObj, NW_DeckDecoder_t *tmp_decoder,
                                  NW_DeckDecoder_t *old_decoder, NW_Ucs2* url);

static TBrowserStatusCode SetTimerKey(NW_Wml_t *wml);
static TBrowserStatusCode GetMethod(NW_Wml_t *thisObj, NW_Wml_Element_t *go_elem, NW_Bool *method_is_post);
static TBrowserStatusCode MakeReqUrl(NW_Wml_t *thisObj, NW_Ucs2 *card_name, NW_Ucs2 **patchedReqUrl);
static TBrowserStatusCode MoveToCard(NW_Wml_t *thisObj, NW_Ucs2 *card_name);
static TBrowserStatusCode ProcessIntraDeckGoTask(NW_Wml_t *thisObj, NW_Ucs2 *reqUrl, NW_Bool updateHistory);
static TBrowserStatusCode ProcessIntraDeckFetchUrlTask(NW_Wml_t *thisObj, NW_Ucs2 *reqUrl, NW_Bool updateHistory);
static TBrowserStatusCode MustReload(NW_Wml_t *thisObj, NW_Wml_Element_t *go_elem, NW_Bool *reload);
static TBrowserStatusCode GetImageUrl(NW_Wml_t *that, NW_Uint16 imgel_id, NW_Wml_Attr_t image_attr,
                                                       NW_Ucs2 **ret_string);

/*------------------------------------------------------------
 * SetTimerKey 
 * If a timer is running, set the TIMER's KEY variable to the
 * remaining time.
 *
 * returns
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *----------------------------------------------------------*/  

static TBrowserStatusCode SetTimerKey(NW_Wml_t *thisObj)
{
  TBrowserStatusCode status;
  NW_Uint32   timer_value = 0;
  NW_Bool     running = NW_FALSE;
  
  NW_ASSERT(thisObj != NULL);

  status = (TIMER_API->isRunning)(thisObj->browser_app,&running);
  if (status != KBrsrSuccess || !running)
    return status;
  
  if ((status = NW_Wml_SetTimerKey(thisObj, WAE_TIMER_FROM_TIMER, &timer_value))
      == KBrsrOutOfMemory)
    return status;
  thisObj->script_state.timer_time = timer_value;

  if ((status = (TIMER_API->stop)(thisObj->browser_app)) != KBrsrSuccess)
    return status;
  (void)(TIMER_API->destroy)(thisObj->browser_app);

  return status;
}

/*---------------------------------------------------------------------------
 * GetMethod 
 *
 * Sets method_is_post true if the method really, truly is 'post'
 *
 * returns
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *-------------------------------------------------------------------------*/
static TBrowserStatusCode GetMethod(NW_Wml_t *thisObj, 
                             NW_Wml_Element_t *go_elem, 
                             NW_Bool  *method_is_post)
{
  NW_ASSERT(thisObj != NULL);

  *method_is_post = NW_FALSE;

  if (go_elem == NULL)
    return KBrsrSuccess;

  *method_is_post = NW_DeckDecoder_HasAttributeWithValue(thisObj->decoder, go_elem, METHOD_ATTR, 
      WAE_ASC_POST_STR, thisObj->var_list);

  return KBrsrSuccess;
}

/*---------------------------------------------------------------------------
 * GetCacheControl 
 *
 * Sets noCache true if the cache-control attrib. value is 'no-cache'
 *
 * returns
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *-------------------------------------------------------------------------*/
static TBrowserStatusCode GetCacheControl(NW_Wml_t         *thisObj, 
                                          NW_Wml_Element_t *go_elem, 
                                          NW_Bool          *noCache)
{
  NW_ASSERT(thisObj != NULL);

  *noCache = NW_FALSE;

  if (go_elem == NULL)
    return KBrsrSuccess;

  *noCache = NW_DeckDecoder_HasAttributeWithValue(thisObj->decoder, go_elem, CACHECONTROL_ATTR, 
      WAE_ASC_NOCACHE_STR, thisObj->var_list);

  return KBrsrSuccess;
}

/*---------------------------------------------------------------------------
 * GetPostfields 
 *
 * Sets postfields to true if a postfields elmement is within the go tag
 *
 * returns
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *-------------------------------------------------------------------------*/
static TBrowserStatusCode GetPostfields(NW_Wml_t         *thisObj, 
                                        NW_Wml_Element_t *go_elem, 
                                        NW_Bool          *isPostField)
{
  TBrowserStatusCode status = KBrsrFailure;
  NW_NVPair_t *postfields     = NULL;

  NW_ASSERT(thisObj != NULL);

  *isPostField = NW_FALSE;

  if (go_elem == NULL)
  {
    return KBrsrSuccess;
  }

  status = NW_DeckDecoder_GetPostfieldData(thisObj->decoder,
              thisObj->var_list, go_elem, &postfields);

  if (postfields != NULL) 
    {
      *isPostField = NW_TRUE;
      NW_NVPair_Delete( postfields );
      postfields = NULL;
   }

  return status;
}

/*---------------------------------------------------------------------
 * MustReload
 *
 * We must reload if there are any postfields, or the method of the
 * go element is 'post'.
 *
 * returns:
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *---------------------------------------------------------------------*/ 
static TBrowserStatusCode MustReload(NW_Wml_t *thisObj, 
                                     NW_Wml_Element_t *go_elem, 
                                     NW_Bool  *reload)
{
  NW_Bool     isPost;
  NW_Bool     noCache;
  NW_Bool     isPostField;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(reload != NULL);

  *reload = NW_FALSE;

  GetPostfields(thisObj, go_elem, &isPostField);
  
  GetMethod(thisObj, go_elem, &isPost);

  GetCacheControl(thisObj, go_elem, &noCache);

  *reload = (NW_Bool) (noCache || isPost || isPostField);
  return KBrsrSuccess;
}

/*----------------------------------------------------------------------------------
 * MakeReqUrl
 *
 * Workaround for PDC Gateway issue -- we remanufacture the full destination from 
 * the history request url, not from the response url, for an intradeck 'go'
 *
 * returns KBrsrOutOfMemory
 *         KBrsrMalformedUrl
 *         KBrsrSuccess
 *---------------------------------------------------------------------------------*/
static TBrowserStatusCode MakeReqUrl(NW_Wml_t *thisObj, 
                              NW_Ucs2  *card_name,
                              NW_Ucs2 **patchedUrlReq)
{
  TBrowserStatusCode status;
  NW_Ucs2     *url;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(patchedUrlReq != NULL);


  status = NW_Wml_HistoryGetRequestUrl(thisObj, &url);
  if (status == KBrsrOutOfMemory)
    return status;

  status = NW_Url_BuildAbsUrl(url, card_name, patchedUrlReq);
  NW_Str_Delete(url);

  return status;
}

/*----------------------------------------------------------------------
 * MoveToCard
 *
 * Positions the deck decoder to the given card, if possible. If not,
 * then the decoder is positioned to the first card in the deck,
 * per WML 1.2, section 12.5.1#4c.
 *
 * Note -- this works ok even if the card_name parameter is NULL.
 *
 * returns
 *    KBrsrSuccess
 *    KBrsrOutOfMemory
 *---------------------------------------------------------------------*/   
static TBrowserStatusCode MoveToCard(NW_Wml_t *thisObj, NW_Ucs2 *card_name)
{
  TBrowserStatusCode status = KBrsrSuccess; 

  NW_ASSERT(thisObj != NULL);
  
  if(thisObj->decoder != NULL)
    status = NW_DeckDecoder_SetCardByName(thisObj->decoder, card_name);
  
  if (status == KBrsrOutOfMemory)
      return status;  

  /* wrong card_name */
  if (status == KBrsrWmlbrowserCardNotInDeck) {
    /* Try using a NULL card_name to enforce WML 12.5.1#4c */
    status = NW_DeckDecoder_SetCardByName(thisObj->decoder, NULL);
  }
  return status;
}

/*--------------------------------------------------------------------------
 * ProcessIntraDeckGoTask
 *
 * NOTE -- the 'GO_TASK' case contains a workaround for a PDC gateway
 * issue.  Specifically the problem is this -- the PDC gateway sometimes
 * sends back a ContentLocation header indicating that the gateway (e.g.,
 * 'krell.uplanet.com') is the location of the redirected content.
 * Since we derive the 'response_url' from the ContentLocation header,
 * and we resolve relative references with respect to the response url,
 * we have a problem.
 *
 * So, in the case of an intradeck 'go', we need to manufacture a 
 * request URL that contains the real (originally requested)
 * deck URL, plus the _requested_ card name (which is part of reqUrl).
 * Then we need to manufacture a response URL that consists of the
 * request URL plus the _actual_ card that we are going to display (this
 * may or may not be the requested card, and it may or may not have a name).
 *
 * Suppose, for example, the current deck was requested as 
 * "http://foo/deck.wml#card1", and the response_url (derived from
 * the ContentLocation header) from the PDC gateway is 
 * "http://foo/krell.uplanet.com#card1".  Suppose further that the next
 * request from that card is to go to "#card2", but there isn't a 
 * 'card2', and that 'main' is the id of the first card in the deck.
 *
 * Then we need to manufacture a request url of "http://foo/deck.wml#card2"
 * and a response url of "http://foo/krell.uplanet.com#main".
 *
 * returns
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *-------------------------------------------------------------------------*/

static TBrowserStatusCode ProcessIntraDeckGoTask(NW_Wml_t *thisObj, 
                                          NW_Ucs2 *url, 
                                          NW_Bool updateHistory)
{
  NW_Ucs2     *cardName    = NULL;
  NW_Ucs2     *responseUrl = NULL;
  NW_Ucs2     *requestUrl  = NULL;
  TBrowserStatusCode status;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(url != NULL);

  status = NW_DeckDecoder_GetCardName(thisObj->decoder, &cardName);
  if (status != KBrsrSuccess && status != KBrsrFailure)
    goto Exit;

  status = NW_Url_BuildAbsUrl(url,cardName,&responseUrl);
  if (status != KBrsrSuccess)
    goto Exit;

  status = MakeReqUrl(thisObj,NW_Url_Fragment(url),&requestUrl);
  if (status != KBrsrSuccess)
    goto Exit;

  /* now create and push a new History entry */
  if (updateHistory == NW_TRUE) {
    status = NW_Wml_HistoryPushIntraDeckLoad(thisObj, requestUrl, responseUrl);
    if (status != KBrsrSuccess && status != KBrsrFailure)
      goto Exit;
  }

  status = NW_Wml_ProcessGo(thisObj,responseUrl);
  if (status != KBrsrSuccess && status != KBrsrFailure)
    goto Exit;      /* FAILURE does not imply a fatal error */
  status = KBrsrSuccess;

Exit:
  NW_Str_Delete(cardName);
  NW_Str_Delete(requestUrl);
  NW_Str_Delete(responseUrl);
  return status;
}

/*-----------------------------------------------------------------------
 * ProcessIntraDeckFetchUrlTask
 *
 * Handles intradeck navigation tasks as indicated by the the 'curr_task'
 * member of the NW_Wml_t object.
 *
 * returns
 *  KBrsrSuccess
 *  anything else is an unrecoverable error
 *---------------------------------------------------------------------*/
static TBrowserStatusCode ProcessIntraDeckFetchUrlTask(NW_Wml_t *thisObj,
                                                NW_Ucs2  *reqUrl, 
                                                NW_Bool updateHistory)
{
  TBrowserStatusCode status;
  NW_Wml1x_ContentHandler_t* thisHandlerObj;

  NW_ASSERT(thisObj != NULL);

  status = NW_Wml_CancelAllLoads(thisObj);
	thisHandlerObj = NW_Wml1x_ContentHandlerOf(thisObj->browser_app);

  switch (thisObj->curr_task)
  {
    case GO_TASK:
      NW_ASSERT(reqUrl != NULL);
      status = ProcessIntraDeckGoTask(thisObj, reqUrl, updateHistory);
      break;

    case PREV_TASK:
      NW_ASSERT(reqUrl != NULL);
      status = NW_Wml_ProcessPrev(thisObj, reqUrl);
    
      if (status != KBrsrSuccess && status != KBrsrFailure)
        break;    /* FAILURE does not imply a fatal error */
      status = KBrsrSuccess;
      break;

    case REFRESH_TASK:
      status = NW_Wml_ProcessRefresh(thisObj);
      
      if (status != KBrsrSuccess && status != KBrsrFailure)
        break;   /* FAILURE does not imply a fatal error */
      status = KBrsrSuccess;
      break;

    case RESTORE_TASK:
      /* TODO do we need to do more to restore/restart a card */
			if(thisHandlerObj->contextSuspended)
			{
				/* apply the global variables to the wml interpreter */
				NW_DeckDecoder_ContextToVarList (thisHandlerObj->contextSuspended, thisHandlerObj->wmlInterpreter.var_list,
						thisHandlerObj->wmlInterpreter.mem_segment_general);
				/* clear the global variable context */
				NW_Object_Delete (thisHandlerObj->contextSuspended);
				thisHandlerObj->contextSuspended = NULL;
			}
      NW_Wml_DisplayCard(thisObj, NW_FALSE);
      status = NW_Wml_HandleTimer(thisObj);

      status = KBrsrSuccess;
      break;
  
    case INVALID_TASK:
    case NOOP_TASK:
    default:
      status = KBrsrFailure;
  }

  return status;
}


static TBrowserStatusCode NW_Wml_HandleIntraPageNavigationStarted (NW_Wml_t *thisObj)
{
  NW_HED_DocumentRoot_t* docRoot;

  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj->browser_app);

  thisObj->outstandingLoadCount = 0;

  NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox(docRoot);
  (void) NW_ADT_DynamicVector_Clear(rootBox->iOutOfViewListeners);

  return NW_HED_DocumentRoot_HandleIntraPageNavigationStarted (docRoot);
}


static TBrowserStatusCode NW_Wml_HandleIntraPageNavigationCompleted (NW_Wml_t *thisObj)
{
  NW_HED_DocumentRoot_t* docRoot;

  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj->browser_app);

  if (thisObj->IntraDeck_OefCounter > MAX_NUMBER_INTRA_OEF) {
    /* we have exceeded the maximunm # of OnEnterForwards allowable for the deck 
       The error will be handled after we process the deck - Vishy05/14/2002
    */
    docRoot->isIntraPageNav--;

    return KBrsrSuccess;
  }
  else {
    return NW_HED_DocumentRoot_HandleIntraPageNavigationCompleted (docRoot);
  }
}

TBrowserStatusCode NW_Wml_HandleIntraDocRequest (NW_Wml_t *thisObj, 
                                          NW_Wml_Task_e task,
                                          NW_Bool historicRequest,
                                          const NW_Ucs2 *url)
{
  TBrowserStatusCode status = KBrsrWmlbrowserCardNotInDeck;
  NW_Bool mustCallCompleted = NW_FALSE;
  NW_Bool cardNotFound = NW_FALSE;
  NW_Wml1x_ContentHandler_t* wmlCH;

  /* set the wml-task */
  thisObj->curr_task = task;

  if(task != RESTORE_TASK)
  {
    wmlCH = NW_Wml1x_ContentHandlerOf(thisObj->browser_app);
	NW_Object_Delete (wmlCH->contextSuspended);
	/* copy the suspended context */
	wmlCH->contextSuspended = NW_DeckDecoder_VarListToContext(wmlCH->wmlInterpreter.var_list);
  }

  /* notify the docRoot that we are performing an intra-document navigation */
  NW_Wml_HandleIntraPageNavigationStarted (thisObj);
  mustCallCompleted = NW_TRUE;

  /* set the card.  The url will be NULL when we are "restoring" a card */
  if (url != NULL) {
    NW_String_t cardName;

    /* if the intra-nav dest card is not found, it is not a fatal error */
    /* see WML-191, section 12.5.1.4 */

    NW_String_initialize (&cardName, NW_Url_Fragment((NW_Ucs2 *) url), HTTP_iso_10646_ucs_2);
    
    if (cardName.storage != NULL) {
      if (!NW_Wml_Deck_GetCardByName (thisObj->decoder->domDeck, &cardName))
      {
        cardNotFound = NW_TRUE;
      }
    }

    /* set the current card */
    NW_THROWIF_ERROR (status = MoveToCard(thisObj, NW_Url_Fragment((NW_Ucs2 *) url)));
  }

  /* show the intra-deck card */
  NW_THROWIF_ERROR (status = ProcessIntraDeckFetchUrlTask(thisObj, (NW_Ucs2 *) url, 
      (NW_Bool) (historicRequest == NW_FALSE)));

  /* notify the docRoot that we are done navigating */
  NW_Wml_HandleIntraPageNavigationCompleted (thisObj);

  if (cardNotFound == NW_TRUE) /* if the requested card was not found - WML-191, 12.5.1.4 is followed, */
  {                            /* and notify user that a nav exception has occured, to let them know. */
    NW_Wml_HandleError (thisObj, BRSR_STAT_CLASS_GENERAL, (NW_Int16) KBrsrWmlbrowserCardNotInDeck);
  }

  /* When we restore a previous document we do not want to start the timers again! - Vishy 07/02/2002 */
  if(task == RESTORE_TASK) {    
    wmlCH = NW_Wml1x_ContentHandlerOf(thisObj->browser_app);
    (void) (wmlCH->wmlInterpreter).wml_api->timer->destroy(wmlCH);
    wmlCH->allowTimer = NW_FALSE;
  }

  /* update the content handler to point to the frag's url-request */
  {
    NW_HED_DocumentNode_t* docNode = NULL;
    NW_HED_DocumentRoot_t* docRoot = NULL;
    NW_HED_HistoryStack_t* history = NULL;
    NW_HED_HistoryEntry_t* entry = NULL;

    docNode = NW_HED_DocumentNodeOf (thisObj->browser_app);
    docRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (docNode);
    history = NW_HED_DocumentRoot_GetHistoryStack (docRoot);

    entry = NW_HED_HistoryStack_GetEntry (history, NW_HED_HistoryStack_Direction_Current);
    
    status = NW_HED_ContentHandler_SetAssociatedHistoryEntry (docNode, entry);
  }

  return status;

NW_CATCH_ERROR
  /*lint -e{774} Significant prototype coercion*/
  if (mustCallCompleted == NW_TRUE) {
    NW_Wml_HandleError (thisObj, BRSR_STAT_CLASS_GENERAL, (NW_Int16) status);
	//We need to set the IntraDeck_OefCounter to 0 so that one card is shown for the infinite loop and also preventing a crash
	//down the road. The bad content error message will be shown through the next function call.
	thisObj->IntraDeck_OefCounter = 0;
    NW_Wml_HandleIntraPageNavigationCompleted (thisObj);

    if (thisObj->IntraDeck_OefCounter > MAX_NUMBER_INTRA_OEF) {

      /* this is a cronic error and we want to propagate this error to the 
        to the top - Vishy 05/14/2002*/
      status  = KBrsrWmlbrowserBadContent; 
    }
    else {
      /* the error is handled above so set status to success */
      status = KBrsrSuccess;
    }
  }

  return status;
}

/*--------------------------------------------------------------------------- 
 * NW_Wml_FetchUrlGeneric
 *
 * This is the same as NW_Wml_FetchUrlGeneric, but allows for more flexibility.
 * It does not rely on a GO element to provide the semantics of the load,
 * but receives these directly in the argument list.
 *
 * RETURN: KBrsrSuccess
 *         NW_FAILURE - does NOT imply a fatal error 
 *                    - card does not have an intrinsic event
 *         KBrsrBadInputParam
 *         KBrsrOutOfMemory
 *---------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_FetchUrlGeneric(
                            NW_Wml_t *obj, 
                            NW_Ucs2 *url_param,
                            NW_Bool is_method_post,
                            NW_Http_Header_t *header,
                            NW_NVPair_t *postfields,
                            NW_Ucs2 *enctype)
{
  NW_Wml_t    *thisObj       = (NW_Wml_t*) obj;
  TBrowserStatusCode status         = KBrsrFailure;
  NW_Ucs2     *patchedReqUrl = NULL;
  NW_Bool     intraDeck      = NW_FALSE;
  NW_Bool     mustReload     = is_method_post;
  NW_Rfc2396_Parts_t parts   = {0};

  if (!NW_Url_GetRfc2396Parts(url_param,&parts))
    return KBrsrMalformedUrl;

  if ((status = SetTimerKey(thisObj)) != KBrsrSuccess)
    return status;
  
  status = NW_Wml_IsCardInCurrentDeck(thisObj, url_param);
  if (status == KBrsrOutOfMemory)
    return status;

  intraDeck = (NW_Bool) (status == KBrsrSuccess);

  /* 
   * If card is in current deck, then WML Spec 12.5.1.3 and 
   * 12.5.1.4 are satisfied: access control and locating the 
   * destination card
   */

  if (!intraDeck || mustReload) {
    thisObj->curr_task = GO_TASK;
    if (intraDeck) {
      status = MakeReqUrl(thisObj, NW_Url_Fragment(url_param), &patchedReqUrl);
      if (status != KBrsrSuccess)
        return status;
      status = NW_Wml_DoLoadAndSave(thisObj, patchedReqUrl, is_method_post,
                                    NULL, NULL, header,
                                    postfields, enctype, NW_CACHE_NORMAL);
      NW_Str_Delete(patchedReqUrl);
    } else {
      status = NW_Wml_DoLoadAndSave(thisObj, url_param, is_method_post,
                                    NULL, NULL, header,
                                    postfields, enctype, NW_CACHE_NORMAL);
    }
  } 
  
  /* intradeck fetch -- no need to reload the deck */
  else {
    status = NW_Wml_HandleIntraDocRequest (thisObj, thisObj->curr_task, NW_FALSE, url_param);

    if (status != KBrsrSuccess) {
      NW_Wml_ClearTaskVariables(thisObj); /* per WML 12.5.5 */
    }
  }

  return status;
}

/*--------------------------------------------------------------------------- 
 * NW_Wml_FetchUrl
 *
 * Navigate to url_param. url_param is NW_MEM_SEGMENT_MANUAL & not owned by 
 * this routine, but short-lived 
 *
 * NOTE: For an intradeck fetch that doesn't require a reload of the deck,
 * if the curr_task is not reload, we clear thisObj->mem_segment_card and
 * clean the memory segment identified by the 'mem_segment_general' field of the wml 
 * object (this flushes out history strings that were orphaned).
 *
 * RETURN: KBrsrSuccess
 *         NW_FAILURE - does NOT imply a fatal error 
 *                    - card does not have an intrinsic event
 *         KBrsrBadInputParam
 *         KBrsrOutOfMemory
 *---------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_FetchUrl(NW_Wml_t *obj, 
                            NW_Ucs2  *url_param, 
                            NW_Wml_Element_t* go_elem)
{
  NW_Wml_t    *thisObj       = (NW_Wml_t*) obj;
  TBrowserStatusCode status         = KBrsrFailure;
  NW_Ucs2     *patchedReqUrl = NULL;
  NW_Bool     intraDeck      = NW_FALSE;
  NW_Bool     mustReload     = NW_FALSE;
  NW_Rfc2396_Parts_t parts   = {0};

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(url_param != NULL);

  if (NW_Url_GetRfc2396Parts(url_param,&parts) != NW_TRUE)
    return KBrsrMalformedUrl;

  if ((status = SetTimerKey(thisObj)) != KBrsrSuccess)
    return status;
  
  status = NW_Wml_IsCardInCurrentDeck(thisObj, url_param);
  if (status == KBrsrOutOfMemory)
    return status;
  intraDeck = (NW_Bool) (status == KBrsrSuccess);

  if(intraDeck && go_elem)
  {
       NW_Wml_ElType_e el_type;
       //Check if it is <a> tag not the <go> or <anchor> tag. Then let
       //cache decide wheather to keep the same page or load the new one.
       //If we keep the same page then cache parameters are not modified.
       //If there is an <a> tag this is wrong because the cache doesn't implement
       //the wml intradeck naviagation, as it is 12.5.1.3, 12.5.1.4
       NW_DeckDecoder_GetType(thisObj->decoder, go_elem, &el_type);
       if(el_type == ANCHOR_ELEMENT)
       {
        intraDeck = NW_FALSE;
        go_elem   = NULL; //Make same that of the original call.
       }
  }

  if ((status = MustReload(thisObj, go_elem, &mustReload)) != KBrsrSuccess)
    return status;

  /* 
   * If card is in current deck, then WML Spec 12.5.1.3 and 
   * 12.5.1.4 are satisfied: access control and locating the 
   * destination card
   */

  if (!intraDeck || mustReload) {
    thisObj->curr_task = GO_TASK;
    /* reset the counter as we are making an external load */ 
    thisObj->IntraDeck_OefCounter = 0;
    if (intraDeck) {
      status = MakeReqUrl(thisObj, NW_Url_Fragment(url_param), &patchedReqUrl);
      if (status != KBrsrSuccess)
        return status;
      status = NW_Wml_MakeLoadRequest(thisObj, patchedReqUrl, go_elem);
      NW_Str_Delete(patchedReqUrl);
    } else {
      status = NW_Wml_MakeLoadRequest(thisObj, url_param, go_elem);
    }
  } 
  
  /* intradeck fetch -- no need to reload the deck */
  else {
  
  /*Now we need to check if a condition for endless loop of intra-OnEnterForward is met. */
    if(thisObj->IntraDeck_OefCounter > MAX_NUMBER_INTRA_OEF) {
		  /*We stop the navigation to prevent endless loop and 
		    notify the user about the error*/
      return KBrsrWmlbrowserBadContent;
    }
    
    status = NW_Wml_HandleIntraDocRequest (thisObj, thisObj->curr_task, NW_FALSE, url_param);

    if (status != KBrsrSuccess) {
      NW_Wml_ClearTaskVariables(thisObj); /* per WML 12.5.5 */
    }
  }

  return status;
}


/*-------------------------------------------------------------------------
 * NW_Wml_FetchPrevUrl
 *
 * Navigate to the url of the history entry before the current one.
 * 
 * Intradeck fetches cause thisObj->mem_segment_card to be cleared, and
 * cause the wml object's mem_segment_general segment to be cleaned.
 * 
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - history stack is empty
 *         KBrsrOutOfMemory
 *------------------------------------------------------------------------*/
 TBrowserStatusCode NW_Wml_FetchPrevUrl(NW_Wml_t *obj)
 {
  NW_Wml_t         *thisObj = (NW_Wml_t*) obj;
  NW_Wml_History_t *prev_card = NULL;
  TBrowserStatusCode      status = KBrsrSuccess;
  NW_Bool          intraDeck = NW_FALSE;
  
  if ((status = SetTimerKey(thisObj)) != KBrsrSuccess)
    return status;

  if ((status = NW_Wml_HistoryPrevCard(thisObj, &prev_card)) == KBrsrOutOfMemory)
    return status;

  /* 
   * If the history stack is empty, then perhaps a dialog should be 
   * presented to the user.  But the spec does not say this is
   * an error condition.
   */
  if (status == KBrsrFailure)
    goto EndFetchPrev;

  if ((status = NW_Wml_IsCardInCurrentDeck(thisObj, prev_card->req_url))
      == KBrsrOutOfMemory)
    goto EndFetchPrev;
  intraDeck = (NW_Bool)(status == KBrsrSuccess);

  /* The 'pop' of the history stack will occur when NW_Wml_ProcessPrev
     is executed.  Otherwise, we run the risk of popping the history
     stack _before_ the load is completed, and then aborting the load.
     The 'current' url would then be lost!
  */

  /* 
   * if card is in current deck, then WML Spec 12.5.2.3 and 12.5.2.4 are
   * satisfied: access control and locating the destination card
   */
  if (intraDeck)
  {
    NW_Wml_Element_t last_card_el;

    NW_DeckDecoder_GetCardElement (thisObj->decoder, &last_card_el);

    if ((status = MoveToCard(thisObj, NW_Url_Fragment(prev_card->req_url))) 
         == KBrsrOutOfMemory) {
      goto EndFetchPrev;
    }

    if (status == KBrsrSuccess)
    {
      status = NW_Wml_HistoryToPrev(thisObj, NULL);
      if (status != KBrsrSuccess) {
        goto EndFetchPrev;
      }

      /* show the prev card */
      status = NW_Wml_HandleIntraDocRequest (thisObj, PREV_TASK, NW_TRUE, prev_card->req_url);

      if (status != KBrsrSuccess && status != KBrsrFailure)
        /* FAILURE does not imply a fatal error */
        goto EndFetchPrev;
    }
    else
    {
      /* card not found - present dialog app */

      /* do a refresh until UI option list bug is fixed */
      thisObj->curr_task = REFRESH_TASK;
      NW_DeckDecoder_SetCardByElement (thisObj->decoder, &last_card_el);
      status = NW_Wml_DisplayCard(thisObj, NW_TRUE);
      if (status != KBrsrSuccess && status != KBrsrFailure)
        /* FAILURE does not imply a fatal error */
        goto EndFetchPrev;
    }
  }
  else
  {
    thisObj->curr_task = PREV_TASK;
    status = NW_Wml_DoPrevLoad(thisObj->browser_app);
  }

EndFetchPrev:
  NW_Wml_HistoryDeleteEntry(prev_card);

  return status;
}

 
/* Helper function for LoadDeck() to cleanup after errors */
static TBrowserStatusCode LoadDeckError(NW_Wml_t* thisObj, NW_DeckDecoder_t *tmp_decoder,
                          NW_DeckDecoder_t *old_decoder, NW_Ucs2* url)
{

  NW_ASSERT(thisObj != NULL);

  /* Restore old deck decoder */
  if (old_decoder != NULL) {
    thisObj->decoder = old_decoder;
  }

  NW_Str_Delete(url);

  /* Clean up variable state, per WML 12.5.5 */
  NW_Wml_ClearTaskVariables(thisObj);

  NW_Ucs2 *card_name = NULL;
  /* redisplay the old deck only if we have one !*/
  if (thisObj->decoder != NULL && (NW_DeckDecoder_GetCardName(thisObj->decoder, &card_name) != KBrsrSuccess)) {
    return NW_Wml_DisplayCard(thisObj, NW_TRUE);
  }
  else
    {
    return NW_Wml_DisplayCard(thisObj, NULL);
    }
}

/*
 * RETURN KBrsrSuccess - deck is loaded OK
 *        KBrsrFailure - deck is not loaded because of a non-fatal
 *                          error like failing access control
 *        KBrsrOutOfMemory
 *        NW_STAT_*       - propagates fatal return value from CB calls
 */

/*
** TODO this function can be simplified now that we load each deck into their
** own Tempest-content-handler.  Since the wml-content-handler has a unique
** NW_Wml_t structure there will NEVER be previous card or deck to restore to
** if a failure is encountered in this function.  LoadDeckError can also be
** simplified.
*/
TBrowserStatusCode NW_Wml_LoadWmlResp(NW_Wml_t *thisObj, 
                               NW_Buffer_t* responseBuffer,
                               NW_Bool contentWasPlainText,
                               NW_Url_Resp_t* response)
{
  NW_Ucs2             *url         = NULL;
  NW_Ucs2             *card_name   = NULL;
  NW_DeckDecoder_t    *old_decoder = thisObj->decoder;
  NW_DeckDecoder_t    *tmp_decoder = NULL;
  TBrowserStatusCode         status;
  TBrowserStatusCode         error;
  NW_Http_CharSet_t   charset;
  NW_Ucs2             *reqUrl;
  NW_Ucs2             *redirectedUrl;

  NW_ASSERT(thisObj != NULL);

  charset = response->charset;

  reqUrl = NW_Str_Newcpy(response->uri);
  if (reqUrl == NULL) {
    return KBrsrOutOfMemory;
  }

  redirectedUrl = reqUrl;

  /* 
   * check for a redirected url, then free. 
   * both url_param and new_url were malloced without using a mem segment
   */
  url = NW_Wml_CheckRedirection(reqUrl, redirectedUrl);
  if (url == NULL)
  {
    (void) LoadDeckError(thisObj, NULL, NULL, url);
    return KBrsrOutOfMemory;
  }
  NW_ASSERT(url != NULL);

  if (redirectedUrl != reqUrl)
    NW_Mem_Free(redirectedUrl);

  NW_Mem_Free(reqUrl);
  /* create a new DeckDecoder, reuse the resp->context wml bytecode */

  tmp_decoder = NW_DeckDecoder_New();
  if (tmp_decoder == NULL)
  {
    (void) LoadDeckError(thisObj, NULL, NULL, url);
    return KBrsrOutOfMemory;
  }

  status = NW_DeckDecoder_InitializeAndValidate(tmp_decoder, responseBuffer, 
      charset, contentWasPlainText);

  /*
  ** tmp_decoder now owns the content, and will free it when tmp_decoder is
  ** freed, or has already freed it in the case of error.
  */

  if (status != KBrsrSuccess) {
    error = status;
    (void) LoadDeckError(thisObj, tmp_decoder, NULL, url);
    return error;
  }

  /* 
   * Temporarily set thisObj->decoder to tmp_decoder so the access
   * check can be done
   */
  thisObj->decoder = tmp_decoder;

  status = NW_Wml_AccessControl(thisObj, url);
  if (status != KBrsrSuccess) {
    if (status == KBrsrOutOfMemory) {
      error = KBrsrOutOfMemory;
    } else {
      error = KBrsrWmlbrowserNoAccess;
    }
    (void) LoadDeckError(thisObj, tmp_decoder, old_decoder, url);
    return error;
  }

  /* non - WML Spec  */
  /* check for card's meta data and perform custom processing */
  if ((status = NW_Wml_HandleMetaElements(thisObj)) == KBrsrOutOfMemory)
  {
    (void) LoadDeckError(thisObj, tmp_decoder, old_decoder, url);
    return status;
  }

  card_name = NW_Str_Strchr(url, '#');
  if (card_name && NW_Str_StrcmpConst(card_name, WAE_ASC_EMPTY_STR))
    card_name++;
  /* this if branch include card_name = NULL */
  status = NW_DeckDecoder_SetCardByName(thisObj->decoder, card_name);
  if (status != KBrsrSuccess) { 
    /* Try using a NULL card_name to enforce WML 12.5.1#4c */
    status = NW_DeckDecoder_SetCardByName(thisObj->decoder, NULL);
    if (status == KBrsrWmlbrowserCardNotInDeck) {
        (void) LoadDeckError(thisObj, tmp_decoder, old_decoder, url);
        return status;
      }
  }

  /* append the card name to the URL */
  if ((status = NW_DeckDecoder_GetCardName(thisObj->decoder, &card_name))
    == KBrsrSuccess) {
    if(card_name) {
      NW_Ucs2 *new_url = NULL;
      status = NW_Url_BuildAbsUrl(url, card_name, &new_url);
      NW_Str_Delete(card_name);
      if (status != KBrsrSuccess) {
        (void) LoadDeckError(thisObj, tmp_decoder, old_decoder, url);
        return status;
      }
      NW_Str_Delete(url);
      url = new_url;
    }
  }
  /* Card not found is not a fatal error */ 
  if (status == KBrsrOutOfMemory) {
    error = status;
    (void) LoadDeckError(thisObj, tmp_decoder, old_decoder, url);
    return error;
  }

  if ( (thisObj->curr_task == GO_TASK) || (thisObj->curr_task == PREV_TASK) )
  {
    /*
     * Temporarily install the previous decoder so the UI can be notified
     * that the old card can be destroyed 
     */ 
    thisObj->decoder = old_decoder;
    if ((status = (DISPLAY_API->destroyCard)(thisObj->browser_app)) 
        != KBrsrSuccess)
    {
      (void) LoadDeckError(thisObj, tmp_decoder, NULL, url);
      return status;
    }
    /* Restore the new decoder */
    thisObj->decoder = tmp_decoder;
  }

  if (thisObj->curr_task == GO_TASK)
  {
    status = NW_Wml_ProcessGo(thisObj, url);
  }
  else if(thisObj->curr_task == PREV_TASK)
  {
    status = NW_Wml_ProcessPrev(thisObj, url);
  }
  else
  {
    status = KBrsrFailure;
  }
  /* free the old decoder as we have loaded the new deck */
  /* Note: NW_DeckDecoder_Delete() handles NULL pointers */
  NW_DeckDecoder_Delete(old_decoder);

  /* Note: NW_Str_Delete() handles NULL pointers */
  NW_Str_Delete(url);

  if (status != KBrsrSuccess) /* display error and cleanup */
  {
    /* blow up the card here as the user agent might have reported errors
    when displaying the card */
    (void) (DISPLAY_API->destroyCard)(thisObj->browser_app);

    /* Clean up variable state, per WML 12.5.5 */
    NW_Wml_ClearTaskVariables(thisObj);
  }

  if (status != KBrsrSuccess)
    {
    NW_DeckDecoder_Delete(thisObj->decoder);
    thisObj->decoder = NULL;
    }
  return status;
}


/* Retrieve the absolute URL for a image element. This function is called when 
 * only SRC exist. 
 * NOTE: The ret_string parameter has to be freed by the caller by invoking 
 *       the function NW_Wml_FreeString
 * RETURN KBrsrSuccess - URL exits
 *        KBrsrBadInputParam - element not found or bad input param
 *        KBrsrOutOfMemory
 *        NW_STAT_*       - propagates fatal return value from CB calls
 */
TBrowserStatusCode NW_Wml_GetImageUrl(NW_Wml_t *thisObj, NW_Uint16 imgel_id, NW_Ucs2 **ret_string)
{
    NW_Wml_Attr_t image_attr;

  image_attr = SRC_ATTR;
  return (GetImageUrl(thisObj, imgel_id, image_attr, ret_string));
}

/* Retrieve the absolute URL for a image element. This function is called when
 * LOCALSRC attribute exist, it should be loaded first. 
 * NOTE: The ret_string parameter has to be freed by the caller by invoking 
 *       the function NW_Wml_FreeString
 * RETURN KBrsrSuccess - URL exits
 *        KBrsrBadInputParam - element not found or bad input param
 *        KBrsrOutOfMemory
 *        NW_STAT_*       - propagates fatal return value from CB calls
 */

TBrowserStatusCode NW_Wml_GetLocalImageUrl(NW_Wml_t *thisObj, NW_Uint16 imgel_id, NW_Ucs2 **ret_string)
{
  NW_Wml_Attr_t image_attr;

  image_attr = LOCALSRC_ATTR;
  return (GetImageUrl(thisObj, imgel_id, image_attr, ret_string));
}


static TBrowserStatusCode GetImageUrl(NW_Wml_t *thisObj, NW_Uint16 imgel_id, NW_Wml_Attr_t image_attr,
                                                NW_Ucs2 **ret_string)
{
  NW_Int32         id  = -1;
  NW_Wml_Element_t *el =  0;
  NW_Wml_ElType_e  el_type;
  TBrowserStatusCode      status = KBrsrFailure;
  NW_Ucs2          *url   = 0;

  if ((thisObj == NULL) || (id = NW_Wml_FindEl(thisObj, imgel_id)) == -1)
    return KBrsrBadInputParam;

  el = thisObj->card_els[id];
  NW_ASSERT(el != NULL);

  NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

  if (el_type == IMAGE_ELEMENT)
  {
    /* get image-attribute */
    if ( (status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, image_attr,
          thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &url)) == KBrsrOutOfMemory )
    {
      return status;
    }
    
    /* Get url*/
    if (url)
    {
      status = NW_Wml_GetFullUrl(thisObj, url, ret_string);
      NW_Str_Delete(url);
    }
  }
  else
  {
    status = KBrsrBadInputParam;
  }
  return status;
}

TBrowserStatusCode
NW_Wml_CancelAllLoads (NW_Wml_t *thisObj)
{
  NW_HED_DocumentNode_t* rootNode;

  rootNode = NW_HED_DocumentNode_GetRootNode (thisObj->browser_app);
  (void) NW_HED_DocumentNode_Cancel (rootNode, NW_HED_CancelType_NewRequest);

  return KBrsrSuccess;
}

void NW_Wml_HandleError (NW_Wml_t *thisObj, NW_Int16 errorClass, NW_Int16 error)
{
  NW_HED_DocumentNode_t* docNode;
  
  docNode = NW_HED_DocumentNodeOf (thisObj->browser_app);

  if (error == KBrsrWmlbrowserCardNotInDeck)
  {
    NW_HED_DocumentRoot_t* docRoot = NULL;
    
    docRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (docNode);
    
    (void) docRoot->appServices->errorApi.
      notifyError (docRoot, (TBrowserStatusCode) error, NULL, NULL);
    return;
  }
  
  NW_HED_DocumentNode_HandleError (docNode, NULL, errorClass, error); 
}
