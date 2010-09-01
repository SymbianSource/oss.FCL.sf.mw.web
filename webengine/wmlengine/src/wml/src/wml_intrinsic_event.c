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


/*****************************************************************
**    File name:  wml_intrinsic_event.c
**    Part of: WML Interpreter
**    Description:  This file contains functions to handle the 
      ONETERFORWARD, ONENTERBACKWARD and ONTIMER Intrinsic Events
      per [WML].
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "wml_task.h"
#include "wml_url_utils.h"
#include "wml_api.h"
#include "wml_elm_attr.h"

#include "nwx_mem.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

static 
TBrowserStatusCode Wml_HandleTemplateIntrinsicEvents(NW_Wml_t* obj, 
                                              NW_Wml_Intrinsic_e intrinsic_event);
static 
TBrowserStatusCode Wml_HandleCardIntrinsicEvents(NW_Wml_t* obj,
                                          NW_Wml_Intrinsic_e intrinsic_event);
static 
TBrowserStatusCode Wml_HandleIntrinsicEvents(NW_Wml_t *obj,
                                      NW_Wml_Element_t *el,
                                      NW_Wml_Intrinsic_e intrinsic_event);

/*****************************************************************

  Name: Wml_HandleCardIntrinsicEvents()

  Description:  Searches for the card element, and if found invokes the
                Wml_HandleIntrinsicEvents function.

  Parameters:   obj              -
                intrinsic_event  - intrinsic event
 
  Return:
          KBrsrSuccess - if the event is found
          KBrsrFailure - if the event is not found (NOT a fatal error)
                          - no card element.
          KBrsrOutOfMemory
          NW_STAT_* - propagates other errors

******************************************************************/
static 
TBrowserStatusCode Wml_HandleCardIntrinsicEvents(NW_Wml_t* obj,
                                          NW_Wml_Intrinsic_e intrinsic_event)
{
  NW_Wml_Element_t  card_el;
  NW_Wml_t          *thisObj = obj;

  if (!NW_DeckDecoder_GetCardElement(thisObj->decoder, &card_el))
    return KBrsrFailure;
  return Wml_HandleIntrinsicEvents(thisObj, &card_el, intrinsic_event);
}


/*****************************************************************

  Name: Wml_HandleTemplateIntrinsicEvents()

  Description:  Searches for the template element, and if found invokes the
                Wml_HandleIntrinsicEvents function.

  Parameters:   obj              -
                intrinsic_event  - intrinsic event
 
  Return:
          KBrsrSuccess - if the event is found
          KBrsrFailure - if the event is not found (NOT a fatal error)
                          - no template element
                          - not an intrinsic event
                          - bad card element
          KBrsrOutOfMemory
          NW_STAT_* - other errors

******************************************************************/
static 
TBrowserStatusCode Wml_HandleTemplateIntrinsicEvents(NW_Wml_t* obj, 
                                              NW_Wml_Intrinsic_e intrinsic_event)
{
  TBrowserStatusCode       status;
  NW_Wml_Element_t  templ_el;
  NW_Wml_t          *thisObj = obj;

  if ((status = NW_DeckDecoder_GetTemplateElement(thisObj->decoder, &templ_el)) 
    != KBrsrSuccess) {
    return status;
  }
  return Wml_HandleIntrinsicEvents(thisObj, &templ_el, intrinsic_event);
}

/*****************************************************************

  Name: Wml_HandleIntrinsicEvents()

  Description:  Handle intrinsic events defined on the requested element.
  1. Look for (ONENTERFORWARD/ONENTERBACKWARD/ONTIMER) attribute value URL 
  on the element.If found, fetch the URL.
  2. Search for any contained elements of type ONEVENT on the element. If 
  found the task defined on the element is executed.
  
  Note: Intrinsic events are only defined at the card/template level.
    
  Parameters:   obj              -
                el               - card (or) template element
                intrinsic_event  - intrinsic events
 
  Return:
        KBrsrSuccess - if the event is found
        KBrsrFailure - if the event is not found (NOT a fatal error)
                        - not an intrinsic event
                        - bad element
        KBrsrOutOfMemory
        NW_STAT_* - propagates other errors

******************************************************************/
static 
TBrowserStatusCode Wml_HandleIntrinsicEvents(NW_Wml_t *obj,
                                      NW_Wml_Element_t *el,
                                      NW_Wml_Intrinsic_e intrinsic_event)
{
  NW_Wml_t          *thisObj = obj;
  NW_Ucs2           *url = 0, *target_url = 0;
  NW_Wml_Element_t  el_iterator;
  NW_Bool           found = NW_FALSE;
  NW_Wml_Attr_t     attr_token = UNKNOWN_ATTR;
  const char        *type_str = NULL;
  TBrowserStatusCode       status;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(el != NULL);
   /* known event? */
  NW_ASSERT((intrinsic_event == ONENTERFORWARD) ||
            (intrinsic_event == ONENTERBACKWARD) ||
            (intrinsic_event == ONTIMER));
  
  switch (intrinsic_event) {
  
    case ONENTERFORWARD:
      attr_token = ONENTERFORWARD_ATTR;
      type_str = WAE_ASC_ONENTERFORWARD_STR;
      break;

    case ONENTERBACKWARD:
      attr_token = ONENTERBACKWARD_ATTR;
      type_str = WAE_ASC_ONENTERBACKWARD_STR;
      break;

    case ONTIMER:
      attr_token = ONTIMER_ATTR;
      type_str = WAE_ASC_ONTIMER_STR;
      break;

    case ONCLICK:
    default:
      break;
  }
  NW_ASSERT(type_str != NULL);
  NW_ASSERT((attr_token == ONENTERFORWARD_ATTR) || 
            (attr_token == ONENTERBACKWARD_ATTR) ||
            (attr_token == ONTIMER_ATTR));

  /* First, check for ON*** card attribute */
  status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, attr_token,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &url);

  /* FAILURE does not imply a fatal error */
  if (status != KBrsrSuccess && status != KBrsrFailure) {
    return status;
  }

  if (url) {

    /* get absolute url */
    status = NW_Wml_GetFullUrl(thisObj, url, &target_url);
    NW_Str_Delete(url);

    if (status == KBrsrSuccess)
    {
      /* if no url, then perhaps a dialog should be presented to the user */
      if (target_url) {
        /* clean up the display */
        if ((status = (DISPLAY_API->cleanUp)(thisObj->browser_app)) == KBrsrSuccess) {
          thisObj->curr_task = GO_TASK;
           
          if( (intrinsic_event == ONENTERFORWARD) ||(intrinsic_event ==ONENTERBACKWARD)) {
            thisObj->IntraDeck_OefCounter++;  /* Intra-deck On-enter-forward*/            
            thisObj->oefNavigation = NW_TRUE;
          }
          status = NW_Wml_FetchUrl(thisObj, target_url, 0);/* no GO element */
        }
        NW_Str_Delete(target_url);
        if (status != KBrsrSuccess) {
          return status;
        }
      }
      found = NW_TRUE;
    }
    else if(status == KBrsrFailure) /* fall through */
    {
      /* FAILURE does not imply a fatal error */
      found = NW_TRUE;
    }
    else
    {
      return status;
    }
  } else { /* Second, check for ONEVENT element of type onenter_tag */
    NW_DeckDecoderIter_t iter;

    found = NW_FALSE;
    if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, el) != KBrsrSuccess)
      return KBrsrFailure;
  
    while ((found == NW_FALSE) && NW_DeckDecoderIter_GetNextElement(&iter, &el_iterator)) 
    {
      NW_Wml_ElType_e el_type;
  
      NW_DeckDecoder_GetType(thisObj->decoder, &el_iterator, &el_type);

      if (el_type == ONEVENT_ELEMENT) {
        if( (intrinsic_event == ONENTERFORWARD) ||(intrinsic_event ==ONENTERBACKWARD)) {
          thisObj->IntraDeck_OefCounter++;  /* Intra-deck On-enter-forward*/            
          thisObj->oefNavigation = NW_TRUE;
        }
        if (NW_DeckDecoder_HasAttributeWithValue (thisObj->decoder, &el_iterator, TYPE_ATTR,
            type_str, thisObj->var_list)) {
          /* clean up the display */
          if ((status = (DISPLAY_API->cleanUp)(thisObj->browser_app)) != KBrsrSuccess)
          {
            return status;
          }
          status = NW_Wml_HandleTaskEvent(thisObj, &el_iterator);
          if (status != KBrsrSuccess && status != KBrsrFailure)
          {
            /* FAILURE does NOT imply a fatal error */
            return status;
          }
          found = NW_TRUE;
        }
      }
      /* Note: Check "found" variable first because event processing can change 
         content which could make GetNextElement call invalid.  jwild 2000.09.29. */
    }
  }

  NW_ASSERT((status == KBrsrSuccess) || (status == KBrsrFailure));

  if (found == NW_TRUE)
    return KBrsrSuccess;
  else
    return KBrsrFailure;
}

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
/*****************************************************************

  Name: NW_Wml_HandleIntrinsicEvents()

  Description:  HandleIntrinsicEvents  generic function, searches for the card event 
    tag (ONENTERFORWARD or ONENTERBACKWARD) as either an attribute of the card element
    or a contained ONEVENT element of TYPE "ONENTER*". 
   
    1. If the CARD element contains a card-level Intrinsic Event
      (i.e. the event is an attribute of the CARD), execute the event
    2. Else, if the CARD contains an ONEVENT element, execute the ONEVENT's task;
    3. Else if the deck contains a TEMPLATE element and the TEMPLATE contains
       an Intrinsic Event as an attribute, execute the event;
    4. Else if the deck contains a TEMPLATE element and the TEMPLATE contains
       an ONEVENT element, execute the ONEVENT's task

  Parameters:   obj              -
                intrinsic_event  - intrinsic events
 
  Return:
          KBrsrSuccess - if the event is found
          KBrsrFailure - if the event is not found (NOT a fatal error)
          KBrsrOutOfMemory
          NW_STAT_* - propagates other errors

******************************************************************/
TBrowserStatusCode NW_Wml_HandleIntrinsicEvents(NW_Wml_t* obj, NW_Wml_Intrinsic_e intrinsic_event)
{
  NW_Wml_t    *thisObj = obj;
  TBrowserStatusCode status;
  
  /* card level events override the deck level events */
  status = Wml_HandleCardIntrinsicEvents(thisObj, intrinsic_event);  
  if (status == KBrsrFailure)
  {
    /* not a fatal error, look for template events */
    status = Wml_HandleTemplateIntrinsicEvents(thisObj, intrinsic_event);
    /* propogate errors to the caller */
    if (status != KBrsrSuccess && status != KBrsrFailure)
    {
      return status;
    }
  }
  else if(status != KBrsrSuccess)
  {
    /* propogate errors to the caller */
    return status;
  }

  NW_ASSERT((status == KBrsrSuccess) || (status == KBrsrFailure));
  
  /* the browser state will be reflected in the curr_task variable */
  if ((status == KBrsrSuccess) && (thisObj->curr_task != NOOP_TASK)) {
    /*return (DISPLAY_API->destroyCard)(thisObj->browser_app);*/
    return KBrsrSuccess;
  }
  return KBrsrFailure;
}
