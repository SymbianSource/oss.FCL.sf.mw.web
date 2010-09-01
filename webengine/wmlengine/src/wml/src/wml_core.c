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
    $Workfile: wml_core.c $

    Purpose:

        Class: WmlBrowser

        Main browser engine class definition. The exteral environment messages to this
        engine through the constructor, Start, Event, and *Resp methods. The core browser
        object directs the URL Loader to get WML deck and card bytecode. It then processes
        the WML tags and interfaces with an external UI system. The core browser also 
        interfaces with the WML script engine.

*/

#include "wml_core.h"
#include "wml_history.h"
#include "wml_api.h"
#include "wml_url_utils.h"

#include "nwx_memseg.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"

#include "nw_evlog_api.h"
#include "nw_wae.h"
#include "nwx_ctx.h"
#include "BrsrStatusCodes.h"
#include "urlloader_urlloaderint.h"

/* Zero out the WML data structure. */
static void initWmlData( NW_Wml_t* thisObj )
{

  /* thisObj->browser_app = NULL;  don't init because won't be reset. */

  thisObj->curr_task = INVALID_TASK;

  thisObj->var_list = NULL;
  thisObj->param_list = NULL;

  /* Also initializes last_load, history_resp_url, and history_index. */
  NW_Wml_HistoryInit(thisObj);

  thisObj->decoder = NULL;

  thisObj->card_els = NULL;

  thisObj->prev_task_id = -1;
  thisObj->do_type_prev_id = -1;

  thisObj->script_state.load_url = NULL;
  thisObj->script_state.load_url_params = NULL;
  thisObj->script_state.load_header = NULL;
  thisObj->script_state.load_prev = NW_FALSE;
  thisObj->script_state.timer_time = 0;
  thisObj->script_state.timerProcessing = NW_FALSE; 

  thisObj->hasFocus = NW_TRUE; /* Assume start with focus. */

  thisObj->mem_segment_general = NW_MEM_SEGMENT_INVALID; 
  thisObj->mem_segment_card = NW_MEM_SEGMENT_INVALID; 
  thisObj->IntraDeck_OefCounter = 0; /*Reset the counter*/
  thisObj->oefNavigation = NW_FALSE; /*establish navigation initially as non-eof */

  /* thisObj->wml_api = NULL;  Don't init because won't be reset. */

  return;
}

static void freeWmlData( NW_Wml_t* thisObj )
{
  /* Frees var_list and param_list when free thisObj->mem_segment_general. */

  NW_Str_Delete( thisObj->history_resp_url );

  /* Handles null pointer for decoder. */
  NW_DeckDecoder_Delete(thisObj->decoder);

  /* Free card_els and card info. */
  NW_Wml_FreeCardSegment(thisObj);

  /* Free up the WMLScript restart state. */
  NW_Str_Delete(thisObj->script_state.load_url);
  NW_Str_Delete(thisObj->script_state.load_url_params);
  UrlLoader_HeadersFree(thisObj->script_state.load_header);
  
  if ( thisObj->mem_segment_general != NW_MEM_SEGMENT_INVALID )
  {
    NW_Mem_Segment_Free(thisObj->mem_segment_general);
  }

  /* set to a "zero" state. */
  initWmlData( thisObj );    

  return;
}

/* Allocate any initial data structures needed. */
static TBrowserStatusCode allocWmlData( NW_Wml_t* thisObj )
{
  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( thisObj->mem_segment_general == NW_MEM_SEGMENT_INVALID );
  NW_ASSERT( thisObj->var_list == NULL );
  NW_ASSERT( thisObj->param_list == NULL );

  thisObj->mem_segment_general = NW_Mem_Segment_New();
  if (thisObj->mem_segment_general == 0)
    return KBrsrOutOfMemory;

  thisObj->var_list = NW_WmlVar_NewList(thisObj->mem_segment_general);
  if (thisObj->var_list == 0)
  {
    NW_Mem_Segment_Free(thisObj->mem_segment_general);
    thisObj->mem_segment_general = NW_MEM_SEGMENT_INVALID;
    return KBrsrOutOfMemory;
  }

  thisObj->param_list = NW_WmlVar_NewList(thisObj->mem_segment_general);
  if (thisObj->param_list == 0)
  {
    NW_Mem_Segment_Free(thisObj->mem_segment_general);
    thisObj->var_list = NULL;
    thisObj->mem_segment_general = NW_MEM_SEGMENT_INVALID;
    return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}

/******************************************************************************
  Name:
    NW_Wml_GetVersion( NW_Wml_t *thisObj, NW_Uint8 *version )

  Description:
    Retrieves a version identifier set when the WML deck was loaded.  The
    version is saved by "wml_decoder.c" routine,
    NW_DeckDecoder_InitializeAndValidate().
    The format of this NW_Byte follows information in paragraph 5.4 of
    WAP-192-WXBML-20000515.

  Parameters:
    NW_Wml_t *thisObj    - pointer to NW_Wml_t
    NW_Uint8    *version - pointer to NW_Int8 variable in which version is returned

  Return value:
    KBrsrSuccess   obtained version of WBXML saved when deck was loaded.
    KBrsrFailure   wml context not available; "decoder" is not available.
 *****************************************************************************/
TBrowserStatusCode NW_Wml_GetVersion( NW_Wml_t *thisObj, NW_Uint8 *version )
{
  if (thisObj == NULL)
  {
    return KBrsrFailure;
  }
  if (thisObj->decoder == NULL)
  {
    return KBrsrFailure;
  }
  *version = NW_DeckDecoder_GetVersion (thisObj->decoder);
  return KBrsrSuccess;
}


/*
 * RETURN: KBrsrSuccess
           KBrsrBadInputParam
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode NW_Wml_InitTask(NW_Wml_t *thisObj,
                        const NW_WmlApi_t *wmlapi,
                        void *usrAgent,
                        NW_Wml_Task_e  taskValue)
{
  TBrowserStatusCode status;

  if ((thisObj == 0) || 
      (wmlapi == 0) || 
      (wmlapi->display == 0) || 
      (wmlapi->timer == 0))
    return KBrsrBadInputParam;
  
  /* Initialize ALL the member data */
  initWmlData( thisObj );
  status = allocWmlData( thisObj );
  if ( status != KBrsrSuccess ) {
    return status;
  }

  thisObj->browser_app = usrAgent;
  thisObj->curr_task = taskValue;
  thisObj->wml_api = wmlapi;

  return KBrsrSuccess;
}

TBrowserStatusCode NW_Wml_Init(NW_Wml_t *thisObj,
                        const NW_WmlApi_t *wmlapi,
                        void *usrAgent)
{
  return NW_Wml_InitTask(thisObj, wmlapi, usrAgent, NOOP_TASK);
}


TBrowserStatusCode NW_Wml_InitFromShell(NW_Wml_t *thisObj,
                                const NW_Ucs2 *url,
                                const NW_WmlApi_t *wmlapi,
                                void *usrAgent)
{
  TBrowserStatusCode status;
  
  status = NW_Wml_InitTask(thisObj, wmlapi, usrAgent, GO_TASK);
  NW_REQUIRED_PARAM(url);
  return status;
}

#if(0)
/* This function is not being used as of 07/31/2001. */
void NW_Wml_Dispose(NW_Wml_t *thisObj )
{
  NW_ASSERT( thisObj != NULL );
  
  /* Should not be called now that freeWmlData does all cleanup.
     jwild 2001.04.26 */
  NW_ASSERT(NW_FALSE); 

  /* Free allocated heap data*/
  NW_Wml_FreeCardSegment(thisObj);
  NW_Mem_Segment_Free(thisObj->mem_segment_general);
}
#endif

TBrowserStatusCode NW_Wml_Start(NW_Wml_t* thisObj, NW_Ucs2 *initialUrl)
{
  NW_Ucs2 *pStart;
  NW_Ucs2 *pEnd;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Bool running;

  if ((thisObj == NULL) || (initialUrl == NULL)) 
    return KBrsrBadInputParam;
  
  NW_ASSERT(DISPLAY_API != NULL);
  NW_ASSERT(TIMER_API != NULL);

  thisObj->IntraDeck_OefCounter = 0; /*Reset the counter*/
  thisObj->oefNavigation = NW_FALSE;

  /* First check if we can navigate */
  if ((status = NW_Wml_CanNavigateToUri())
    != KBrsrSuccess) {
    return status;
  }
  
  /* 
   * If the deck is running a timer loop and the user tries to 
   * open another page the, timers should be destroyed.
   */
  if ((status = (TIMER_API->isRunning)(thisObj->browser_app, 
                                       &running)) ==
      KBrsrSuccess) 
  {
    if (running) 
    {
      status = (TIMER_API->destroy)(thisObj->browser_app);
    }
  }
  if (status != KBrsrSuccess) {
    return status;
  }

  /* Strip leading & trailing spaces from the Url */
  pStart  = initialUrl;
  pEnd    = NW_Str_Strchr(initialUrl, '\0');
  while (pStart < pEnd) {
    if (NW_Str_Isspace(*pStart)) {
      pStart++;
    } else {
      break;
    }
  }
  while (pStart < pEnd) {
    if (NW_Str_Isspace(*(pEnd - 1))) {
      pEnd--;
    } else {
      break;
    }
  }
  *pEnd = '\0';
  if (pStart != initialUrl) {
    (void)NW_Mem_memmove(initialUrl, pStart, (NW_UINT32_CAST((pEnd - pStart + 1))*sizeof(NW_Ucs2)));
  }

  /*
   * Since this request may be the result of a user-supplied
   * URL and not part of "normal" WML navigation, must create
   * a new context, per WML spec section 10.4.
   *
   * Must also tell the UI to destroy the current card.
   * This could result in the current card being cleared but
   * that's OK - per WML spec section 10.2 which says an
   * implementation "can reset its state to a well-known value"
   * and in this case, we assume this means a clear UI.
   */
  if ((status = (DISPLAY_API->destroyCard)(thisObj->browser_app)) ==
      KBrsrSuccess) 
  {
    thisObj->curr_task = GO_TASK;
    NW_Wml_NewContext(thisObj);
    status = NW_Wml_DoLoadAndSave(thisObj, initialUrl, NW_FALSE, NULL, 
                                  NULL, NULL, NULL, NULL, NW_CACHE_NORMAL);
  }
  return status;
}

TBrowserStatusCode NW_Wml_Exit(NW_Wml_t* thisObj)
{
  NW_ASSERT(thisObj != NULL);
  
  freeWmlData( thisObj );

#ifdef WAE_DEBUG
  find_leaks();
#endif

  return KBrsrSuccess;
}

/*
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - var_name is not found
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_GetVar(NW_Wml_t* thisObj, const NW_Ucs2 *var_name, NW_Ucs2 **ret_string) 
{
   NW_Ucs2 *temp;

   NW_ASSERT(thisObj != NULL);

   temp = NW_WmlVar_GetValue(thisObj->var_list, var_name); 

   if (temp)
   {
     *ret_string = NW_Str_Newcpy(temp);

     if (*ret_string == NULL)
       return KBrsrOutOfMemory;

     return KBrsrSuccess;
   }

   return KBrsrFailure;
}

/*
 * RETURN: NULL     -- no more variables
 *         NON-NULL -- a pointer to a wml variable
 */
NW_Wml_Var_t *NW_Wml_NextVar(NW_Wml_t* thisObj, NW_Wml_Var_t *current) 
{
   NW_ASSERT(thisObj);

   return NW_WmlVar_Next(thisObj->var_list, current);
}

void NW_Wml_UnsetVar(NW_Wml_t* thisObj, NW_Ucs2 *var_name) 
{
   NW_ASSERT(var_name != NULL);
   NW_ASSERT(thisObj != NULL);

   (void)NW_WmlVar_Remove(thisObj->var_list, var_name);
}

/*
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - if name = null
 *              - returned by NW_WmlVar_AddToList if name = null
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_SetVar(NW_Wml_t* thisObj, const NW_Ucs2 *name, 
                          const NW_Ucs2 *value)
{
   TBrowserStatusCode status;
   NW_Wml_Var_t *v = 0;
   NW_WaeUsrAgent_t* wae  = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);

   NW_ASSERT(thisObj != NULL);
   NW_ASSERT( wae != NULL );

   if (name == NULL) 
     return KBrsrFailure;

   v = NW_WmlVar_Get(thisObj->var_list, name);

   if (v) 
     status = NW_WmlVar_ReplaceValue(v, value, thisObj->mem_segment_general);
   else 
     status = NW_WmlVar_AddToList(thisObj->var_list, name, value, 
                                  thisObj->mem_segment_general);
   NW_EvLog_Log( NW_WaeUsrAgent_GetEvLog(wae), NW_EvLog_SET_VAR, name, value );

   return status;
}


TBrowserStatusCode NW_Wml_GainFocus(NW_Wml_t *thisObj)
{
  NW_Bool running = NW_FALSE;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ASSERT(TIMER_API != NULL);

  if (thisObj == NULL)
    return KBrsrBadInputParam;

  thisObj->hasFocus = NW_TRUE;

  /* IF the timer is running, resume it */
  if ((status = (TIMER_API->isRunning)(thisObj->browser_app, &running)) == KBrsrSuccess)
  {
    if (running) status = (TIMER_API->resume)(thisObj->browser_app);
  }
  else if (thisObj->script_state.timer_time != 0)
  {
    status = (TIMER_API->create)(thisObj->browser_app, thisObj->script_state.timer_time);
    thisObj->script_state.timer_time = 0;
    if (status != KBrsrSuccess)
    {
      return status;
    }
  }
  return status;
}

TBrowserStatusCode NW_Wml_LoseFocus(NW_Wml_t *thisObj)
{
  NW_Bool running = NW_FALSE;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ASSERT(TIMER_API != NULL);
  
  if (thisObj == NULL)
    return KBrsrBadInputParam;

  thisObj->hasFocus = NW_FALSE;

  /* if the timer is running, stop it */
  if ((status = (TIMER_API->isRunning)(thisObj->browser_app, &running)) == KBrsrSuccess)
  {
    if (running) status = (TIMER_API->stop)(thisObj->browser_app);
  }
  return status;
}

NW_Mem_Segment_Id_t NW_Wml_GetCardSegment(NW_Wml_t *thisObj)
{
  if ( thisObj->mem_segment_card == NW_MEM_SEGMENT_INVALID )
  {
    thisObj->mem_segment_card = NW_Mem_Segment_New();
    
    NW_ASSERT(thisObj->mem_segment_card != NW_MEM_SEGMENT_INVALID);
  }

  return thisObj->mem_segment_card;
}

TBrowserStatusCode NW_Wml_FreeCardSegment(NW_Wml_t *thisObj)
{
  if ( thisObj->mem_segment_card != NW_MEM_SEGMENT_INVALID )
  {
    NW_Mem_Segment_Free(thisObj->mem_segment_card);
    thisObj->mem_segment_card = NW_MEM_SEGMENT_INVALID;
  }

  return KBrsrSuccess;
}


/*-----------------------------------------------------------------------------------
 * NW_Wml_GetPublicId
 *
 * see WBXML spec 1.2, sections 5.5 and 7.2
 *
 * returns KBrsrSuccess
 *         KBrsrOutOfMemory
 *---------------------------------------------------------------------------------*/
TBrowserStatusCode NW_Wml_GetPublicId(NW_Wml_t *thisObj, NW_Uint32 *publicId)
{
  NW_DeckDecoder_t *decoder = thisObj->decoder;

  return ( NW_DeckDecoder_GetDocumentPublicId(decoder, publicId) );
}

