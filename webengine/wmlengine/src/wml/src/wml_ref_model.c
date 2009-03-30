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
    $Workfile: wml_ref_model.c $

    Purpose:

  Class: WmlBrowser

  Browser's reference model processing logic for inter-card
  traversal

*/

#include "wml_task.h"
#include "wml_history.h"
#include "wml_url_utils.h"
#include "wml_api.h"
#include "wml_elm_attr.h"

#include "nwx_memseg.h"
#include "nwx_string.h"
#include "nwx_msg_api.h"
#include "nwx_url_utils.h"
//#include "nwx_mailto_loader.h"
//#include "nwx_wtai_loader.h"
#include "BrsrStatusCodes.h"

static TBrowserStatusCode ProcessVarElements(NW_Wml_t *obj, NW_Bool useTempVars, NW_Wml_Element_t* task_el);

static void VarListFree(NW_Wml_VarList_t *varList, NW_Mem_Segment_Id_t mem_segment);

static NW_Wml_VarList_t *CpyVarList(NW_Wml_VarList_t *varList, NW_Mem_Segment_Id_t mem_segment);



static TBrowserStatusCode HandleNewContext(NW_Wml_t *obj);

static TBrowserStatusCode ApplyInternalVariables(NW_Wml_t *obj);

static TBrowserStatusCode SetTempVar(NW_Wml_VarList_t *paramList, NW_Mem_Segment_Id_t mem_segment,
                              NW_Ucs2 *name, NW_Ucs2 *value);

static TBrowserStatusCode CanNavigate(NW_Wml_t *thisObj, NW_Wml_Element_t *el);

/*
 * RETURN KBrsrSuccess
 *      KBrsrFailure - does not imply a fatal error
 *                 - no task element is found
 *      KBrsrOutOfMemory
 *      KBrsrMalformedUrl - malformed URL encountered
 *      NW_STAT_* - propagates TBrowserStatusCode error from the other calls
 */
TBrowserStatusCode NW_Wml_HandleTaskEvent(NW_Wml_t *thisObj, NW_Wml_Element_t* el)
{
  NW_Wml_Element_t task_el;
  NW_Ucs2 *target_url = 0;
  NW_Ucs2 *extension_val = NULL;
  NW_Bool task_found = NW_FALSE;
  NW_DeckDecoderIter_t iter;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Url_Schema_t ret_scheme;
  TBrowserStatusCode schema_status = KBrsrSuccess;
  NW_Bool display = NW_FALSE;

  /* first check if we can find href attribute (A element) */
  status = NW_Wml_GetHrefAttr(thisObj, el, &target_url);
  if (status != KBrsrSuccess && status != KBrsrFailure)
  {
    /* FAILURE does not imply a fatal error */
    return status;
  }

  /* if no url, then perhaps a dialog should be presented to the user */
  if (target_url)
  {
    NW_Wml_ElType_e el_type;
    /* check if we are allowed to navigate. */
    status = NW_Wml_CanNavigateToUri();
    if (status != KBrsrSuccess) {
      NW_Mem_Free(target_url);
      return status;
    }

    /* check the file extension */
    extension_val = NW_Url_GetUrlFileExtension(target_url);

    /*
       There is no special handling for mailto and wtai here.
       The default scheme dispatcher will dispatch these schemes
     */
    thisObj->curr_task = GO_TASK;

    //To get the element type
    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);
    if ( el_type == A_ELEMENT )
    {
    //pass the element type. Otherwise it will crash.
    status = NW_Wml_FetchUrl(thisObj, target_url, el);
    }
    else
    {
    /* thisObj parameter - no GO element involved */
    status = NW_Wml_FetchUrl(thisObj, target_url, 0);
    }
    NW_Mem_Free(target_url);
    return status; /* _NOT_ KBrsrSuccess; */
  }

  /* Check if we are allowed to navigate. */
  status = CanNavigate(thisObj, el);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* look for a task element */
  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, el) != KBrsrSuccess)
    return KBrsrFailure;

  while ((task_found == NW_FALSE) && NW_DeckDecoderIter_GetNextElement(&iter, &task_el))
  {
    NW_Wml_ElType_e el_type;
    NW_Bool running = NW_FALSE;

    NW_DeckDecoder_GetType(thisObj->decoder, &task_el, &el_type);

    if (el_type == GO_ELEMENT)
    {
      task_found = NW_TRUE;
      thisObj->curr_task = GO_TASK;

      /* GetHrefAttr & FetchUrl handle 12.5.1.2 */
      status = NW_Wml_GetHrefAttr(thisObj, &task_el, &target_url);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      {
        /* FAILURE does not imply a fatal error */
        return status;
      }

      /* This function handles 12.5.1 */
      status = ProcessVarElements(thisObj, NW_TRUE, &task_el);
      if ((status != KBrsrSuccess) && (status != KBrsrFailure))
      {
        return status;
      }

      /*
       * if no url, then perhaps a dialog should be presented to
       * the user
       */
      if (target_url)
      {
        /*
           There is no special handling for mailto and wtai here.
           The default scheme dispatcher will dispatch these schemes
        */
        status = NW_Wml_FetchUrl(thisObj, target_url, &task_el);

		     //If there is a WTAI or callendar in an oneventforward, the card must be displayed again
  	    //after responding to the dialog query to make the call or else the browser will "jam".
        schema_status = NW_Url_GetSchemeNoValidation(target_url, &ret_scheme);
        //val = NW_Str_StrcmpConst(NW_Url_GetUrlFileExtension(target_url), WAE_URL_CALENDAR_EXT);

        if( (extension_val != NULL) &&
            (0 == NW_Str_StrcmpConst(NW_Url_GetUrlFileExtension(extension_val), WAE_URL_CALENDAR_EXT)
            || 0 == NW_Str_StrcmpConst(NW_Url_GetUrlFileExtension(extension_val), WAE_URL_JPG_EXT)))
            {
            display = NW_TRUE;
            }
        if ( schema_status ==  KBrsrSuccess && (ret_scheme == NW_SCHEMA_WTAI || display == NW_TRUE))
		          {
            schema_status = NW_Wml_DisplayCard(thisObj, NW_TRUE);

            if (schema_status != KBrsrSuccess)
                {
		              return schema_status;
                }
		          return KBrsrSuccess;
		          }

        NW_Mem_Free(target_url);
        if (status != KBrsrSuccess && status != KBrsrFailure)
        {
          return status;
        }
      }
      else
      {
        return KBrsrMalformedUrl;
      }
    }
    else if (el_type == PREV_ELEMENT)
    {
      task_found = NW_TRUE;
      thisObj->curr_task = PREV_TASK;

      /* This function handles 12.5.2.1 */
      status = ProcessVarElements(thisObj, NW_TRUE, &task_el);
      if ((status != KBrsrSuccess) && (status != KBrsrFailure))
      {
        return status;
      }

      status = NW_Wml_FetchPrevUrl(thisObj);
      if (status == KBrsrFailure)
      {
        /*
         * FAILURE means the history stack is empty.
         * Must unset variables
         */
        NW_Wml_ClearTaskVariables(thisObj);
		status = KBrsrHedContentDispatched;
		return status;
      }
      else if (status != KBrsrSuccess)
      {
        return status;
      }
    }
    else if (el_type == REFRESH_ELEMENT)
    {
      NW_Uint32 dummy = 0;
      task_found = NW_TRUE;
      thisObj->curr_task = REFRESH_TASK;

      status = (TIMER_API->isRunning)(thisObj->browser_app, &running);
      if (status == KBrsrSuccess && running == NW_TRUE)
      {
        status = NW_Wml_SetTimerKey(thisObj, WAE_TIMER_FROM_TIMER, &dummy);
        if (status != KBrsrSuccess && status != KBrsrFailure)
        {
          /* FAILURE does not imply a fatal error */
          return status;
        }
      }
      /* This function handles 12.5.4.1 */
      status = ProcessVarElements(thisObj, NW_TRUE, &task_el);
      if ((status != KBrsrSuccess) && (status != KBrsrFailure))
      {
        return status;
      }

      /*
       * WML Spec 12.5.4.2
       * apply navigation task variable settings to browser content
       */
      (void) NW_Wml_ApplyTaskVariables(thisObj);

      /* Set Input and Select element NAME and INAME variables */
      if ((status = ApplyInternalVariables(thisObj)) == KBrsrOutOfMemory)
        return status;

      status = NW_Wml_SetTimerKey(thisObj, WAE_TIMER_USE_DEFAULT, &dummy);

      /* KBrsrSuccess implies timer is present.
       * KBrsrFailure is not a fatal error
       */
      if ((status != KBrsrSuccess) && (status != KBrsrFailure)) {
        /* Currently this would be true only in the case of KBrsrOutOfMemory */
        return status;
      }

      /*
       * WML Spec 12.5.4.4
       * display card via the MMI interface
       * don't call NW_WmlPanel_UiRefresh() - this card hasn't been
       * presented yet
       */
      status = NW_Wml_DisplayCard(thisObj, NW_TRUE);
      if (status != KBrsrSuccess)
      {
        return status;
      }

      /* TODO: This has to change to reflect the new logic of
       * setting the timer after the page is refreshed vk - 01/22
       */
      status = NW_Wml_HandleTimer(thisObj);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      {
        /* FAILURE does not imply a fatal error */
        return status;
      }
    }
    else if (el_type == NOOP_ELEMENT)
    {
      /* no processing */
      task_found = NW_TRUE;
      thisObj->curr_task = NOOP_TASK;
    }
    /* we ignore any other element type */
  }

  return KBrsrSuccess;
}


/*
   ProcessGo supports the Reference Processing Behavior - Inter-card
   Navigation defined in WML Specification 12.5.1.5 - 12.5.1.10,
     5. handle NEWCONTEXT
     6. apply navigation task variable assignments to browser context
     7. push destination card onto history stack
     8. handle ONENTERFORWARD
     9. handle TIMER element
    10. display card via the MMI interface
 *
 * RETURN KBrsrSuccess
 *      KBrsrFailure - does not imply a fatal error
 *      KBrsrOutOfMemory
 *      NW_STAT_* - propagates TBrowserStatusCode error from the other calls
 */
TBrowserStatusCode NW_Wml_ProcessGo(NW_Wml_t *obj, NW_Ucs2 *response_url)
{
  TBrowserStatusCode status;
  NW_Uint32 dummy = 0;

  /* timers haven't been started yet. */
  NW_Wml_SetTimerProcessing(obj, NW_FALSE);


  /* WML Spec 12.5.1.5
   *
   * apply navigation task variable settings to browser content
   */
  (void) NW_Wml_ApplyTaskVariables(obj);

  /* WML Spec 12.5.1.6
   *
   * check for card's new_context state and clear browser history
   * and var_list if neccesary
   */
  if ((status = HandleNewContext(obj)) != KBrsrSuccess)
    return status;

  /* Set Input and Select element NAME and INAME variables */
  if ((status = ApplyInternalVariables(obj)) == KBrsrOutOfMemory)
    return status;

  /* WML Spec 12.5.1 #7.
   * The destination card was pushed already into the history stack by
   * the DocumentRoot.UpdateHistory
   */
  if ((status = NW_Wml_HistorySetResponseUrl(obj, response_url)) != KBrsrSuccess)
    return status;

  /* WML Spec 12.5.1.8
   *
   * handle ONENTERFORWARD intrinsic event;
   */
  status = NW_Wml_HandleIntrinsicEvents(obj, ONENTERFORWARD);

  /* If status is failure then no ONENTERFORWARD existed.  KBrsrSuccess
  ** means ONENTERFORWARD existed and was executed, so content will be
  ** specified by result of ONENTERFORWARD.  All other errors should terminate
  ** processing this deck.
  */
  if (status != KBrsrFailure)
  {
    status = NW_Wml_DisplayCard(obj, NW_FALSE);
    return status;
  }

  /* Check to see if there is a ONTIMER event specified if so populate the
   * key variable.The timer will be set after the card has been displayed
   */
  status = NW_Wml_SetTimerKey(obj, WAE_TIMER_USE_DEFAULT, &dummy);

  /* KBrsrSuccess implies timer is present.
   * KBrsrFailure is not a fatal error
   */
  if ((status != KBrsrSuccess) && (status != KBrsrFailure)) {
    /* Currently this would be true only in the case of KBrsrOutOfMemory */
    (void)NW_Wml_HistoryToPrev(obj, NULL);
    return status;
  }

  /* WML Spec 12.5.1.10
   *
   * display card via the MMI interface
   */
  status = NW_Wml_DisplayCard(obj, NW_FALSE);
  if (status != KBrsrSuccess) {
    (void)NW_Wml_HistoryToPrev(obj, NULL);
  }

  return status;
}


/*
   ProcessPrev supports the Reference Processing Behavior - Inter-card
   Navigation defined in WML Specification 12.5.2.5 - 12.5.2.8,
     5. apply navigation task variable assignments to browser context
     6. handle ONENTERBACKWARD
     7. handle TIMER element
     8. display card via the MMI interface
 *
 * RETURN KBrsrSuccess
 *      KBrsrFailure - does not imply a fatal error
 *      NW_STAT_* - propagates TBrowserStatusCode error from the other calls
 *
 */
TBrowserStatusCode NW_Wml_ProcessPrev(NW_Wml_t *obj, NW_Ucs2 *response_url)
{
  TBrowserStatusCode status;
  TBrowserStatusCode savedStatus = KBrsrSuccess;
  NW_Uint32 dummy;

  /* timers havent been started yet. */
  NW_Wml_SetTimerProcessing(obj, NW_FALSE);

  /* WML Spec 12.5.2.5
   *
   * apply navigation task variable settings to browser content
   */
  (void) NW_Wml_ApplyTaskVariables(obj);

  /* Set Input and Select element NAME and INAME variables */
  if ((status = ApplyInternalVariables(obj))
      == KBrsrOutOfMemory)
    return status;

  /* WML Spec 12.5.1.7
   *
   * push destination card onto history stack.
   * In this case, we just need to save the current response Url.
   */

  if ((status = NW_Wml_HistorySetResponseUrl(obj, response_url)) != KBrsrSuccess)
    return status;

  /* WML Spec 12.5.2.6
   *
   * handle ONENTERBACKWARD intrinsic event
   */
  status = NW_Wml_HandleIntrinsicEvents(obj, ONENTERBACKWARD);
  if ((status == KBrsrSuccess) || (status == KBrsrOutOfMemory))
  {
    /* things get subtle here */
    return status;
  }
  else if(status != KBrsrFailure)
  {
    savedStatus = status;
  }

  /* Check to see if there is a ONTIMER event specified if so populate the
   * key variable.The timer will be set after the card has been displayed
   */
  status = NW_Wml_SetTimerKey(obj, WAE_TIMER_USE_DEFAULT, &dummy);

  /* KBrsrSuccess implies timer is present.
   * KBrsrFailure is not a fatal error
   */
  if ((status != KBrsrSuccess) && (status != KBrsrFailure)) {

   /* Currently this would be true only in the case of KBrsrOutOfMemory */
   if (savedStatus != KBrsrSuccess) {
     return savedStatus;
   } else {
     return status;
   }

  }

  /* WML Spec 12.5.2.8
   *
   * display card via the MMI interface
   */
  if ((status = NW_Wml_DisplayCard(obj, NW_FALSE)) != KBrsrSuccess) {
    if (savedStatus != KBrsrSuccess) {
      return savedStatus;
    } else {
      return status;
    }
  }

  return savedStatus;
}

/*
   ProcessRefresh supports the Reference Processing Behavior - Inter-card
   Navigation defined in WML Specification 12.5.4.2 - 12.5.4.4,
     2. apply navigation task variable assignments to browser context
     3. handle TIMER element
     4. display card via the MMI interface
 *
 * RETURN KBrsrSuccess
 *        KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_ProcessRefresh(NW_Wml_t *obj)
{
  TBrowserStatusCode status;
  NW_Wml_t* thisObj = (NW_Wml_t*) obj;
  NW_Uint32 dummy;
  NW_Bool running = NW_FALSE;

  /* WML Spec 12.5.4.2
   *
   * apply navigation task variable settings to browser content
   */
  (void) NW_Wml_ApplyTaskVariables(thisObj);

  /* Set Input and Select element NAME and INAME variables */
  if ((status = ApplyInternalVariables(obj)) == KBrsrOutOfMemory)
    return status;

  /*
   * WML Spec 12.5.4.3 handle TIMER element per the
   * Refresh's specific requirements in 11.7
   */
  status = (TIMER_API->isRunning)(thisObj->browser_app, &running);
  if (status == KBrsrSuccess && running == NW_TRUE)
  {
    status = NW_Wml_SetTimerKey(thisObj, WAE_TIMER_FROM_TIMER, &dummy);
    if (status != KBrsrSuccess && status != KBrsrFailure)
      /* FAILURE does not imply a fatal error */
      return status;
  }

  /* Check to see if there is a ONTIMER event specified if so populate the
   * key variable.The timer will be set after the card has been displayed
   */
  status = NW_Wml_SetTimerKey(obj, WAE_TIMER_USE_DEFAULT, &dummy);

  /* KBrsrSuccess implies timer is present.
   * KBrsrFailure is not a fatal error
   */
  if ((status != KBrsrSuccess) && (status != KBrsrFailure)) {
    /* Currently this would be true only in the case of KBrsrOutOfMemory */
    return status;
  }

  /* WML Spec 12.5.4.4
   *
   * display card via the MMI interface
   */
  status = (DISPLAY_API->refresh)(thisObj->browser_app);

  return status;
}


/*
 * apply navigation task variable settings to browser content
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure - param_list is null or empty
 */
TBrowserStatusCode NW_Wml_ApplyTaskVariables(NW_Wml_t* thisObj)
{
  NW_Wml_Var_t *v;

  if (thisObj->param_list == 0)
    return KBrsrFailure;

  if (NW_WmlVar_IsListEmpty(thisObj->param_list))
    return KBrsrFailure;

  /*
   * If dup var, replace value. else, add to list
   *
   * NOTE: within this loop, all variables in param_list are
   * cleared.
   */
  while ((v = NW_WmlVar_PopFirst(thisObj->param_list)) != NULL)
  {
    if (!NW_WmlVar_Replace(thisObj->var_list, v))
      NW_WmlVar_AddVarToList(thisObj->var_list, v);
  }

  return KBrsrSuccess;
}


/*
 * If the context contains task (temporary) variables, clear them
 *
 * Assume the memory allocated for these variables is part
 * of thisObj->mem_segment_general and will be free'd when thisObj->mem_segment_general
 * is free'd.
 */
void NW_Wml_ClearTaskVariables(NW_Wml_t *thisObj)
{
  if (thisObj == 0 || thisObj->param_list == 0)
    return;

  while (NW_WmlVar_PopFirst(thisObj->param_list) != NULL)
  {
    /* Loop through the vars and clear them */
    ;
  }
}

/*
 * Implements the Reference Processing Behavior -
 * 9.5.2 The Prev Element with setvar's.
 * The function is invoked when HistoryShell Previous is requested
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure
 */
TBrowserStatusCode NW_Wml_ProcessSetvarElements(NW_Wml_t *thisObj)
{
  NW_Wml_Element_t *el;
  NW_Wml_Element_t task_el;
  NW_Int32         i;
  TBrowserStatusCode      nwStatus = KBrsrSuccess;

  NW_ASSERT (thisObj != NULL);

  if (thisObj->prev_task_id >= 0) {
    if ((i = NW_Wml_FindEl(thisObj, thisObj->prev_task_id)) != -1) {
      el = thisObj->card_els[i];
      if (NW_DeckDecoder_FirstContainedElement(thisObj->decoder, el, &task_el)) {
        nwStatus = ProcessVarElements(thisObj, NW_TRUE, &task_el);
      }
    }
    else {
      nwStatus = KBrsrFailure;
    }
  }
  return nwStatus;
}

/*
 * Implements the Reference Processing Behavior -
 * Inter-card Navigation defined in WML Specification 12.5.1.1,
 * 12.5.2.1, and 12.5.4.1; specifically.
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - var element not found (not fatal)
 *         KBrsrOutOfMemory
 *         NW_STAT_*
 */
static TBrowserStatusCode ProcessVarElements(NW_Wml_t *thisObj, NW_Bool useTempVars, NW_Wml_Element_t *task_el)
{
  NW_Wml_Element_t el;
  TBrowserStatusCode      status;
  NW_Ucs2          *n = NULL;
  NW_Ucs2          *v = NULL;
  NW_Wml_VarList_t *mergedList = NULL;
  NW_DeckDecoderIter_t iter;

  /* "ClearTaskVariables" MOVED HERE to *ALWAYS* clear the task variables.
   * 10/27/2000, jac.
   *
   * The context should not contain any old task/temporary vars
   * but if it does (e.g. an error occurred and the variables
   * were not cleared), clear them now.
   */
  NW_Wml_ClearTaskVariables(thisObj);

  NW_THROWIF_ERROR((status = NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, task_el)));

  while (NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    if ((status = NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type))
      == KBrsrOutOfMemory) {
      NW_THROW_ERROR();
    }

    if (el_type == SETVAR_ELEMENT)
    {
      /* get the NAME attribute */
      if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, NAME_ATTR,
           thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &n)) == KBrsrOutOfMemory) {
        NW_THROW_ERROR();
      }

      /* get the VALUE attribute */
      if (useTempVars) {
        if (mergedList == NULL)
          NW_THROWIF_MEM((mergedList = CpyVarList(thisObj->var_list,
                                                  NW_MEM_SEGMENT_MANUAL)));

        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, VALUE_ATTR,
          mergedList, NW_MEM_SEGMENT_MANUAL, &v)) == KBrsrOutOfMemory) {
          NW_THROW_ERROR();
        }

        if ((status = SetTempVar(mergedList, NW_MEM_SEGMENT_MANUAL, n, v))
            == KBrsrOutOfMemory) {
          NW_THROW_ERROR();
        }
        status = SetTempVar(thisObj->param_list, thisObj->mem_segment_general, n, v);
      }
      else {
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, VALUE_ATTR,
                thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &v)) == KBrsrOutOfMemory) {
          NW_THROW_ERROR();
        }
        status = NW_Wml_SetVar(thisObj, n, v);
      }

      /* FAILURE does not imply a fatal error */
      if (status != KBrsrSuccess && status != KBrsrFailure) {
        NW_THROW_ERROR();
      }

      NW_Str_Delete(n);
      NW_Str_Delete(v);
      n = v = NULL;
    }
  }

  status = KBrsrSuccess;
  NW_THROW_ERROR();

NW_CATCH_MEM
  status = KBrsrOutOfMemory;

NW_CATCH_ERROR
  VarListFree(mergedList, NW_MEM_SEGMENT_MANUAL);
  NW_Str_Delete(n);
  NW_Str_Delete(v);

  return status;
}


/*
 * NewContext is a generic function to reset the CoreBrowser's
 * current context
 *
 * RETURN: KBrsrSuccess
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode HandleNewContext(NW_Wml_t* thisObj)
{
  NW_Wml_Element_t card_el;

  if (!NW_DeckDecoder_GetCardElement(thisObj->decoder, &card_el))
    return KBrsrFailure;

  if (NW_DeckDecoder_HasAttributeWithValue (thisObj->decoder, &card_el, NEWCONTEXT_ATTR,
      WAE_ASC_TRUE_STR, thisObj->var_list))
    NW_Wml_NewContext(thisObj);

  return KBrsrSuccess;
}



/*
 * apply NAME and INAME attr variables inside card's INPUT and
 * SELECT elements
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure - does not imply a fatal error
 *                        - no card element
 *        NW_STAT_* - propagates TBrowserStatusCode error from the other calls
 */
static TBrowserStatusCode ApplyInternalVariables(NW_Wml_t* thisObj)
{
  NW_Wml_Element_t card_el;

  if (!NW_DeckDecoder_GetCardElement(thisObj->decoder, &card_el))
    return KBrsrFailure;

  return NW_Wml_ApplyContainedInternalVariables(thisObj, &card_el);
}


/*
 * Add the given name/value pair to the "temporary" variable
 * list - the thisObj->param_list variable list.
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - if name = null
 *                         - returned by NW_WmlVar_AddToList if name = null
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode SetTempVar(NW_Wml_VarList_t *paramList, NW_Mem_Segment_Id_t mem_segment,
                              NW_Ucs2 *name, NW_Ucs2 *value)
{
   NW_Wml_Var_t *v;
   TBrowserStatusCode  status = KBrsrFailure;

   NW_ASSERT(paramList);

   if (name) {
     v = NW_WmlVar_Get(paramList, name);

     if (v) {
       status = NW_WmlVar_ReplaceValue(v, value, mem_segment);
     }
     else {
       status = NW_WmlVar_AddToList(paramList, name, value, mem_segment);
     }
   }
   return status;
}

/*
 * The function frees memory alocated for a Wml variable list. It does nothing if
 * the manual memory segment is used.
 */
static void VarListFree(NW_Wml_VarList_t *varList, NW_Mem_Segment_Id_t mem_segment)
{
  if (mem_segment == NW_MEM_SEGMENT_MANUAL)
  {
    NW_Wml_Var_t *pVar = NULL;
    NW_Wml_Var_t tmpVar;

    while ((pVar = NW_WmlVar_Next(varList, pVar)) != NULL) {
      NW_Str_Delete(pVar->name);
      NW_Str_Delete(pVar->val);
      tmpVar.next = pVar->next;
      NW_Mem_Free(pVar);
      pVar = &tmpVar;
    }
  }
  return;
}

/*
 * The function creates a copy of a Wml variable list in
 * a specified memory segment. The function returns:
 * NULL pointer in out memory conditions
 */
static NW_Wml_VarList_t *CpyVarList(NW_Wml_VarList_t *varList, NW_Mem_Segment_Id_t mem_segment)
{
  NW_Wml_Var_t     *pVar = NULL;
  NW_Wml_VarList_t *retList;

  retList = NW_WmlVar_NewList(mem_segment);
  if (retList)
  {
    while ((pVar = NW_WmlVar_Next(varList, pVar)) != NULL)
    {
      TBrowserStatusCode status;
      if (!pVar->name || !pVar->val) {
        continue;
      }
      status = NW_WmlVar_AddToList(retList, pVar->name, pVar->val, mem_segment);
      if (status == KBrsrOutOfMemory) {
        VarListFree(retList, mem_segment);
        retList = NULL;
        break;
      }
      else {
        NW_ASSERT(status == KBrsrSuccess);
      }
    }
  }
  return retList;
}
static TBrowserStatusCode CanNavigate(NW_Wml_t *thisObj, NW_Wml_Element_t *el)
{
  NW_Wml_Element_t      task_el;
  NW_Wml_ElType_e       el_type;
  NW_DeckDecoderIter_t  iter;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(el != NULL);

  /* look for a task element */
  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, el) != KBrsrSuccess) {
    return KBrsrFailure;
  }

  while (NW_DeckDecoderIter_GetNextElement(&iter, &task_el))
  {
    NW_DeckDecoder_GetType(thisObj->decoder, &task_el, &el_type);
    /* The go and prev tasks result in navigation to a new url. */
    if (el_type == GO_ELEMENT || el_type == PREV_ELEMENT)
    {
      /* check if we are allowed to navigate. */
      return NW_Wml_CanNavigateToUri();
    }
  }
  return KBrsrSuccess;
}
