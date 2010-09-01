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
    $Workfile: wml_script_message.c $

    Purpose:

        Class: WmlBrowser

        Browser script message functions implementation

*/
   
#include "wml_task.h"
#include "wml_history.h"
#include "wml_url_utils.h"

#include "nwx_mem.h"
#include "nwx_string.h"
#include "nw_evlog_api.h"
#include "nw_wae.h"
#include "nwx_ctx.h"
#include "BrsrStatusCodes.h"
#include "urlloader_urlloaderint.h"

/*
 * RETURN KBrsrSuccess
 *      KBrsrFailure
 *      KBrsrBadInputParam
 *       NW_STAT_* - fatal error
 */
TBrowserStatusCode NW_Wml_ReStart(NW_Wml_t *thisObj, TBrowserStatusCode script_status)
{
  NW_Ucs2 *load_url              = thisObj->script_state.load_url;
  NW_Ucs2 *load_url_params       = thisObj->script_state.load_url_params;
  NW_Http_Header_t *load_header  = thisObj->script_state.load_header;
  NW_Ucs2 *target_url            = NULL;
  NW_Int32  size                 = 0;
  NW_Ucs2 empty[1]               = {0};
  TBrowserStatusCode status             = KBrsrSuccess;

  if (thisObj == NULL)
    goto finish;

  if ( ((script_status == KBrsrSuccess) || ( script_status == KBrsrScriptErrorUserExit))
        && load_url && NW_Str_Strcmp(load_url, empty))
  {
    thisObj->curr_task = GO_TASK;
    if (load_url_params && (load_url_params[0] != '\0'))
    {
      status = NW_WmlVar_ParseList(load_url_params, &size, 
                                   thisObj->mem_segment_general, 
                                   &thisObj->param_list);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      /* Failure does not imply a fatal error */
        goto finish;
    }

    status = NW_Wml_GetFullUrl(thisObj, load_url, &target_url);
    if (status != KBrsrSuccess && status != KBrsrFailure)
    /* FAILURE does not imply a fatal error */
      goto finish;

    /* if no url, then perhaps a dialog should be presented to the user */
    if (target_url && (load_header==NULL))   /* a simple GO */
    {
      status = NW_Wml_FetchUrlGeneric(
                           thisObj, 
                           target_url, 
                           NW_FALSE,    /* go() can only be a get */ 
                           NULL,        /* no header */
                           NULL,        /* no postfields */
                           NULL);       /* no enc_type */

      NW_Str_Delete(target_url);
      if (status != KBrsrSuccess)
        goto finish;
    }
    else if (target_url && load_header) {    /* LOCATIONINFO() */
      status = NW_Wml_FetchUrlGeneric(
                           thisObj, 
                           target_url, 
                           NW_TRUE,     /* locationInfo is a post */ 
                           load_header, /* the header */
                           NULL,        /* blank postfields */
                           NULL);       /* enc_type = NULL, i.e. wml-urlencoded */

      NW_Str_Delete(target_url);

      load_header = NULL;
      thisObj->script_state.load_header = 0;
      
      if (status != KBrsrSuccess)
        goto finish;
    }
  }
  else if (((script_status == KBrsrSuccess) || ( script_status == KBrsrScriptErrorUserExit))
    && thisObj->script_state.load_prev)
  {
    thisObj->curr_task = PREV_TASK;
    status = NW_Wml_FetchPrevUrl(thisObj);
    if (status != KBrsrSuccess && status != KBrsrFailure)
    /* FAILURE does not imply a fatal error */
      goto finish;
  }
  else
  {
    /* Start timer after call to Script */
    if (!NW_Wml_IsTimerProcessingComplete(thisObj))
    {
      /* Current card contains a timer with a timer variable.
         Start timer as usual using the timer variable. */
      status = NW_Wml_HandleTimer(thisObj);
      if (status != KBrsrSuccess && status != KBrsrFailure)
        /* FAILURE does not imply a fatal error */
        goto finish;
    }
    else
    {
      /* Current card does not contain a timer variable. Start
         timer from the saved timer state.  If no timer in card,
         saved timer start is 0. */
      if ((thisObj->script_state.timer_time != 0) && thisObj->hasFocus)
      {
        status = (TIMER_API->create)(thisObj->browser_app, thisObj->script_state.timer_time);
        thisObj->script_state.timer_time = 0;
        if (status != KBrsrSuccess)
          goto finish;
      }
    }

    /* do a refresh until UI option list bug is fixed */
    thisObj->curr_task = REFRESH_TASK;
    status = NW_Wml_DisplayCard(thisObj, NW_FALSE);
    
    if (status != KBrsrSuccess && status != KBrsrFailure)
      /* FAILURE does not imply a fatal error */
      goto finish;
  }

finish:
  /* reset script navigation variables */

  if(thisObj != NULL)
  {
    thisObj->script_state.load_prev = NW_FALSE;
    
    if (load_url != 0)
    {
      NW_Str_Delete(load_url);
      thisObj->script_state.load_url = 0;
    }
    
    if (load_url_params != 0)
    {
      NW_Str_Delete(load_url_params);
      thisObj->script_state.load_url_params = 0;
    }

    if (load_header != 0)
    {
      UrlLoader_HeadersFree(load_header);
      thisObj->script_state.load_header = 0;
    }
  }
  return status;
}

void NW_Wml_LoadPrev(NW_Wml_t *thisObj)
{
  thisObj->script_state.load_prev = NW_TRUE;
  if (thisObj->script_state.load_url)
  {
    NW_Mem_Free(thisObj->script_state.load_url);  
    NW_Mem_Free(thisObj->script_state.load_url_params);  
    thisObj->script_state.load_url = 0;
    thisObj->script_state.load_url_params = 0;
  }
}

void NW_Wml_LoadUrl(NW_Wml_t *thisObj, NW_Ucs2 *url, NW_Ucs2 *param, NW_Http_Header_t *header)
{
  if (thisObj->script_state.load_url)                    /* 11/29/2000 */
  {
    NW_Mem_Free(thisObj->script_state.load_url);  
    NW_Mem_Free(thisObj->script_state.load_url_params);  
  }
  if (thisObj->script_state.load_header)
  {
    UrlLoader_HeadersFree(thisObj->script_state.load_header);
  }
  thisObj->script_state.load_url = url;
  thisObj->script_state.load_url_params = param;
  thisObj->script_state.load_prev = NW_FALSE;
  thisObj->script_state.load_header = header;
}


/* void NewContext() */
void NW_Wml_NewContext(NW_Wml_t *thisObj)
{
  NW_WaeUsrAgent_t* wae  = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);

  NW_Wml_HistoryClear(thisObj);
  NW_WmlVar_Clear(thisObj->var_list);

  NW_ASSERT( wae != NULL );
  NW_EvLog_Log( NW_WaeUsrAgent_GetEvLog(wae), NW_EvLog_VARIABLE_CONTEXT_NEW );
  NW_EvLog_Log( NW_WaeUsrAgent_GetEvLog(wae), NW_EvLog_HISTORY_STACK_CLEAR );
}

/* does a newcontext but leaves the top history entry intact. */
void NW_Wml_NewContextFromScript(NW_Wml_t *thisObj)
{
  NW_Wml_NewContext(thisObj);
}
