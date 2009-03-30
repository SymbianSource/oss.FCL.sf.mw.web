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
    $Workfile: wml_timer.c $

    Purpose:

        Class: WmlBrowser

        Handles timer element processing logic
*/

#include "wml_task.h"
#include "wml_api.h"
#include "wml_elm_attr.h"
#include "nw_wae.h"
#include "nwx_ctx.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*
* HandleTimer  if destination card contains a TIMER element, start the timer
*
* RETURN KBrsrSuccess
*       KBrsrFailure - propagates CB_SetTimerKey return values
*       KBrsrOutOfMemory
*/
TBrowserStatusCode NW_Wml_HandleTimer(NW_Wml_t* thisObj)
{
  TBrowserStatusCode status;
  NW_Uint32 timeout_period = 0;
  NW_Bool  running;
  NW_ASSERT(TIMER_API != NULL);

  /* done with timer processing */
  NW_Wml_SetTimerProcessing(thisObj, NW_TRUE);

  /* kill the timers if any */
  status = (TIMER_API->isRunning)(thisObj->browser_app, &running);
  if (status == KBrsrSuccess && running == NW_TRUE) {
    
    if ((status = (TIMER_API->stop)(thisObj->browser_app)) != KBrsrSuccess)
      return status;

    if ((status = (TIMER_API->destroy)(thisObj->browser_app)) != KBrsrSuccess)
      return status;
  }

  if ((status = NW_Wml_SetTimerKey(thisObj, WAE_TIMER_USE_DEFAULT, &timeout_period)) != KBrsrSuccess)
    return status;

  /*
  * create and initialize a timer with timeout period. timeout_period
  * must be in tenths of seconds
  */
  if (timeout_period && thisObj->hasFocus)
  {
    if ((status = (TIMER_API->create)(thisObj->browser_app, timeout_period)) != KBrsrSuccess)
      return status;/* stop processing */
  }
  else if (timeout_period)
  {
    /*
    ** when we regain focus, we look at this field to determine what timeout
    ** to use for restarting timers.  since this timer was never created,
    ** timer_time is equal to the original timeout_period.
    */
    thisObj->script_state.timer_time = timeout_period;
  }

  return KBrsrSuccess;
}

/*
 *
 * Changed to allocate in MEM_FUNK instead of MEM_CARD.  MEM_CARD
 * could get filled on lots of OnEnterForwards.  jwild 06/21/2000.
 *
 * RETURN KBrsrSuccess
 *      KBrsrFailure - no card
 *                 - timer value is not a valid integer
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_SetTimerKey(NW_Wml_t* thisObj, NW_Wml_TimerBehavior_t behavior, NW_Uint32 *ret_time)
{
  NW_Wml_Element_t el, card_el;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Int32  timeout_period_int = 0;
  NW_Bool timer_found = NW_FALSE;
  NW_DeckDecoderIter_t iter;

  *ret_time = 0;

  if (thisObj->decoder == NULL || !NW_DeckDecoder_GetCardElement(thisObj->decoder, &card_el))
  {
    return KBrsrFailure;
  }

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, &card_el) != KBrsrSuccess)
  {
    return KBrsrFailure;
  }

  /* allow only one TIMER element per card */
  while ((timer_found == NW_FALSE) && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    /* search for TIMER element in current card. Start a timer if found */
    if (el_type == TIMER_ELEMENT)
    {
      NW_Ucs2 *key_var_name = NULL, *timeout_period = NULL;
      NW_Ucs2 *endptr = 0, *endstr = 0;
      timer_found = NW_TRUE;

      /* get NAME variable name */
      status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, NAME_ATTR,
          thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &key_var_name);

      if (status == KBrsrOutOfMemory)
      {
        return status;
      }

      switch (behavior)
      {
      case WAE_TIMER_USE_DEFAULT:
        if (key_var_name)
        {
          if ((status = NW_Wml_GetVar(thisObj, key_var_name, &timeout_period))
               == KBrsrOutOfMemory)
          {
            NW_Str_Delete(key_var_name);
            NW_Str_Delete(timeout_period);
            return status;
          }
        }

        if (timeout_period)
        {
          /* make sure timeout_period is a positive
           * integer, with no extraneous characters
           */
          endstr = timeout_period + NW_Str_Strlen(timeout_period);
          timeout_period_int = NW_Str_Strtol(timeout_period, &endptr, 10);
          if (endptr != endstr || timeout_period_int < 1)
          {
            NW_Str_Delete(key_var_name);
            NW_Str_Delete(timeout_period);
            return KBrsrFailure;
          }
        }
        
        /* if KEY variable doesn't exist in context,
         * populate it with the TIMER element's DEFAULT
         * value (if DEFAULT attr exists)
         */
        if (!timeout_period || !NW_Str_StrcmpConst(timeout_period, WAE_ASC_EMPTY_STR))
        {
          NW_Str_Delete(timeout_period);
          timeout_period = NULL;

          status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, VALUE_ATTR,
                                               thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &timeout_period);
          if (status == KBrsrOutOfMemory)
          {
            NW_Str_Delete(key_var_name);
            NW_Str_Delete(timeout_period);
            return status;
          }

          if (timeout_period)
          {
          /* make sure timeout_period is a positive
           * integer, with no extraneous characters,
           * handle null timeout_period
           */
            endstr = timeout_period + NW_Str_Strlen(timeout_period);
            timeout_period_int = NW_Str_Strtol(timeout_period, &endptr, 10);
            if (endptr != endstr || timeout_period_int < 1)
            {
              NW_Str_Delete(key_var_name);
              NW_Str_Delete(timeout_period);
              return KBrsrFailure;
            }

            if (key_var_name)
            {
              status = NW_Wml_SetVar(thisObj, key_var_name, timeout_period);
              if (status == KBrsrOutOfMemory)
              {
                NW_Str_Delete(key_var_name);
                NW_Str_Delete(timeout_period);
                return status;
              }
            }
          }
        }
        if (timeout_period_int < 1)
          timeout_period_int = 0;
        break;

      case WAE_TIMER_TIMEOUT:
        if (key_var_name)
        {
          NW_Ucs2 temp_str[16];

          (void)NW_Str_StrcpyConst(temp_str, WAE_ASC_0_STR);
          status = NW_Wml_SetVar(thisObj, key_var_name, temp_str);
          if (status == KBrsrOutOfMemory)
          {
            NW_Str_Delete(key_var_name);
            NW_Str_Delete(timeout_period);
            return status;
          }
        }
        break;

      case WAE_TIMER_FROM_TIMER:
        {
          NW_Ucs2 temp[16];
          NW_Uint32 time_remaining;
          /* get the remaining time in the timer */
          status = (TIMER_API->read)(thisObj->browser_app, &time_remaining);
          if (status != KBrsrSuccess)
          {
            NW_Str_Delete(key_var_name);
            NW_Str_Delete(timeout_period);
            return status;/* stop processing */
          }

          timeout_period_int = time_remaining;

          if (key_var_name)
          {
            (void)NW_Str_Itoa(time_remaining, temp);
            status = NW_Wml_SetVar(thisObj, key_var_name, temp);
            if (status == KBrsrOutOfMemory)
            {
              NW_Str_Delete(key_var_name);
              NW_Str_Delete(timeout_period);
              return status;
            }
          }
          break;
        }
      } /* end behavior switch */

      NW_Str_Delete(key_var_name);
      key_var_name = NULL;
      NW_Str_Delete(timeout_period);
      timeout_period = NULL;
    }

  } 

  *ret_time = timeout_period_int;
  return KBrsrSuccess;
}

/*
* RETURN KBrsrSuccess
*       KBrsrFailure - propagates CB_SetTimerKey return values
*       KBrsrOutOfMemory
*       KBrsrBadInputParam - bad input parameter
*/
TBrowserStatusCode NW_Wml_Timeout(void* obj)
{
  TBrowserStatusCode status;
  NW_Uint32 dummy = 0;
  NW_Wml_t *thisObj = (NW_Wml_t *)obj;
  NW_WaeUsrAgent_t  *browser;

  if (thisObj == NULL)
    return KBrsrBadInputParam;

  /* initialize TIMER element KEY variable's value  */
  if ((status = NW_Wml_SetTimerKey(thisObj, WAE_TIMER_TIMEOUT, &dummy)) ==
      KBrsrOutOfMemory)
    return status;

  /* destroy the timer */
  if ((status = (TIMER_API->destroy)(thisObj->browser_app)) != KBrsrSuccess)
    return status;/* stop processing */


  /* process ONTIMER intrinsic events */
  if ((status = NW_Wml_HandleIntrinsicEvents(thisObj, ONTIMER)) != KBrsrSuccess)
  {
    if (status == KBrsrHedContentDispatched)
	{
      return KBrsrSuccess;
	}
    if (status == KBrsrFailure && thisObj->curr_task == NOOP_TASK)  /* NOOP task is not an error - reset it */
    {
      status = KBrsrSuccess;
    }
    else
    {
      browser = (NW_WaeUsrAgent_t*)NW_Ctx_Get(NW_CTX_WML_CORE, 0);
      if(browser)
      {
        (void) browser->errorApi->notifyError(browser, status, NULL, NULL);
      }
    }
  }
  return status;
}

/*
* Set the timer processing flag for the current card. 
*/
void NW_Wml_SetTimerProcessing(NW_Wml_t* thisObj, NW_Bool timerProcessing)
{
  NW_ASSERT(thisObj != NULL);
  thisObj->script_state.timerProcessing = timerProcessing;
}

/*
* This function returns NW_TRUE if the timer processing (start timers...) has 
* completed for the current card. NW_FALSE means that the timer element has not been
* processed. 
*/
NW_Bool NW_Wml_IsTimerProcessingComplete(NW_Wml_t* thisObj)
{
  NW_ASSERT(thisObj != NULL);
  return thisObj->script_state.timerProcessing;
}
