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
    $Workfile: wml_ui.c $

    Purpose:

        Class: WmlBrowser

        Browser's UI processing implementation for element rendering

   $Task: wml_ui.c $

*/

#include "wml_api.h"
#include "wml_list.h"
#include "wml_task.h"
#include "wml_url_utils.h"
#include "wml_elm_attr.h"

#include "nwx_mem.h"
#include "nwx_memseg.h"
#include "nwx_string.h"
#include "wml_history.h"
#include "BrsrStatusCodes.h"

static TBrowserStatusCode RenderCardElements(NW_Wml_t* thisObj, NW_Wml_Element_t* container, NW_Wml_List_t* card_el_list);

static TBrowserStatusCode RenderTemplateElements(NW_Wml_t* thisObj, NW_Wml_List_t* card_el_list);

static TBrowserStatusCode RenderDoElement(NW_Wml_t* thisObj, NW_Wml_Element_t* do_el, NW_Wml_List_t* card_el_list);

static TBrowserStatusCode RenderAElement(NW_Wml_t* thisObj, NW_Wml_Element_t* do_el, NW_Wml_List_t* card_el_list);

static TBrowserStatusCode CleanContext(NW_Wml_t *thisObj);


static TBrowserStatusCode CleanContext(NW_Wml_t* thisObj)
{
  NW_Wml_VarList_t *WmlVar_NewList = 0;
  NW_Wml_VarList_t *new_param_list = 0;
  NW_Mem_Segment_Id_t MEM_CC_SCOPE = NW_Mem_Segment_New();

  if (!MEM_CC_SCOPE)
    return KBrsrOutOfMemory;

  if (thisObj->var_list)
  {
    WmlVar_NewList = NW_WmlVar_DupList(thisObj->var_list, MEM_CC_SCOPE);
    if (WmlVar_NewList == 0)
    {
      NW_Mem_Segment_Free(MEM_CC_SCOPE);
      return KBrsrOutOfMemory;
    }
  }

  if (thisObj->param_list)
  {
    new_param_list = NW_WmlVar_DupList(thisObj->param_list, MEM_CC_SCOPE);
    if (new_param_list == 0)
    {
      NW_Mem_Segment_Free(MEM_CC_SCOPE);
      return KBrsrOutOfMemory;
    }
  }

  NW_Mem_Segment_Free(thisObj->mem_segment_general);

  thisObj->mem_segment_general = MEM_CC_SCOPE;
  thisObj->var_list = WmlVar_NewList;
  thisObj->param_list = new_param_list;
  thisObj->card_els = NULL;

  return KBrsrSuccess;
}

/*
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - not a card
 *         KBrsrOutOfMemory
 *         NW_STAT_* - propagates other fatal errors
 */
TBrowserStatusCode NW_Wml_DisplayCard(NW_Wml_t* thisObj, NW_Bool cancelRequest)
{
  NW_Uint32  len, i;
  NW_Int32 tmplen;
  NW_Wml_Element_t card_el;
  TBrowserStatusCode status;
  NW_Wml_List_t *card_el_list;
  
  NW_Wml_Element_t* cardElArrayManual;  /* Allocated in MEM_MANUAL. */
  NW_Wml_Element_t* cardElArrayCard;    /* Allocated in MEM_CARD. */
  NW_Wml_Element_t* cardElPtr;

  /* check for user agent callbacks */
  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(DISPLAY_API != NULL);
  NW_ASSERT(TIMER_API != NULL);

  thisObj->IntraDeck_OefCounter = 0; /*Reset the counter*/
  thisObj->oefNavigation = NW_FALSE; /* navigations have completed */

  /* free scratch-pad for processing deck/card/script */
  status = CleanContext(thisObj);
  if (status != KBrsrSuccess)
    return status;

  NW_Wml_FreeCardSegment(thisObj);

  /* WAVE FIX: In case there is no decoder, just return here... */
  /* This will happen in case the coreBrowser finds an error on the very first deck...(BAD Content) */
  if(thisObj->decoder == NULL)
  {
      return KBrsrSuccess;
  }

  /* initialize */
  thisObj->do_type_prev_id = -1;
  thisObj->prev_task_id = -1;
  card_el_list = NW_WmlList_NewEmptyList(NW_Wml_GetCardSegment(thisObj));
  if (!card_el_list)
    return KBrsrOutOfMemory;

  /* cancel all outstanding request if cancelRequest was passed as NW_TRUE */
  if(cancelRequest) {
    status = NW_Wml_CancelAllLoads(thisObj);
  }

  /* create the card */
  if ((status = (DISPLAY_API->createCard)(thisObj->browser_app))
      != KBrsrSuccess)
    return status;/* stop processing */

  /* position to card start */
  if (!NW_DeckDecoder_GetCardElement(thisObj->decoder, &card_el))
    return KBrsrFailure;

  /* set containter to card start */
  /* render displayable elements */
  status = RenderCardElements(thisObj, &card_el, card_el_list);
  if (status != KBrsrSuccess && status != KBrsrFailure)
     /* FAILURE does not imply a fatal condition */
    return status;

  status = RenderTemplateElements(thisObj, card_el_list);
  if (status != KBrsrSuccess && status != KBrsrFailure)
     /* FAILURE does not imply a fatal condition */
    return status;

  /* convert card_el_list to an array */
  tmplen = NW_WmlList_Length(card_el_list);
  len = NW_UINT32_CAST(tmplen);

  /* Redo to free the element list after the element array is
     is created.  Necessary because using too much MEM_CARD for
     Wave.  jwild 2000.08.23 */

  /* Allocate on continuous array for elements and copy from list.
     Possible to have no elements. */
  cardElArrayManual = NULL;
  if (len > 0)
  {
    cardElArrayManual = (NW_Wml_Element_t*)
                        NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t) * len,
                                              NW_MEM_SEGMENT_MANUAL);
    if (cardElArrayManual == NULL)
      return KBrsrOutOfMemory;
  }

  cardElPtr = (NW_Wml_Element_t*) NW_WmlList_First(card_el_list);
  if(cardElArrayManual != NULL)
  {
    for(i = 0; (i < len) && cardElPtr ; i++)
    {
      NW_DeckDecoder_CopyElement(thisObj->decoder, cardElPtr, &cardElArrayManual[i]);
      cardElPtr = (NW_Wml_Element_t*) NW_WmlList_Next(card_el_list);
    }
  }

  /* Now free the element list because all information is in the element array. */
  NW_Wml_FreeCardSegment(thisObj);

  /* Now allocate the element array and array of element array pointers in
     MEM_CARD. Copy over. */
  cardElArrayCard = NULL;
  if ( len > 0 )
  {
    cardElArrayCard = (NW_Wml_Element_t*)
                      NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t) * len,
                                            NW_Wml_GetCardSegment(thisObj));
    if (cardElArrayCard == NULL)
    {
      NW_Mem_Free( cardElArrayManual );
      return KBrsrOutOfMemory;
    }
  }
  
  thisObj->card_els = (NW_Wml_Element_t **)NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t*) * (len + 1),
                                         NW_Wml_GetCardSegment(thisObj));
  if (thisObj->card_els == NULL)
  {
    if ( cardElArrayManual != NULL )
    {
      NW_Mem_Free( cardElArrayManual );
    }
    return KBrsrOutOfMemory;
  }
  
  if(cardElArrayManual != NULL && cardElArrayCard != NULL)
  {
    for(i = 0; i < len; i++)
    {
      NW_DeckDecoder_CopyElement(thisObj->decoder, &cardElArrayManual[i], &cardElArrayCard[i]);
      thisObj->card_els[i] = &cardElArrayCard[i];
    }
  }
  thisObj->card_els[len] = NULL;

  /* Now free the MEM_MANUAL element array. */
  if (cardElArrayManual != NULL)
  {
    NW_Mem_Free( cardElArrayManual );
  }

  status = NW_Wml_SetCardUIState(thisObj);
  if (status != KBrsrSuccess && status != KBrsrFailure)
    /* FAILURE does not imply a fatal condition */
    return status;

  /* display the card */
  if ((status = (DISPLAY_API->showCard)(thisObj->browser_app)) != KBrsrSuccess)
    return status;

  return KBrsrSuccess;
}

/*
 * RETURN: KBrsrSuccess
 *         KBrsrFailure
 *         KBrsrOutOfMemory
 *         NW_STAT_* - propagates other fatal errors
 */
static TBrowserStatusCode RenderCardElements(NW_Wml_t* thisObj,
    NW_Wml_Element_t* container,
    NW_Wml_List_t* card_el_list)
{
  NW_Wml_Element_t *el;
  NW_Wml_ElType_e el_type;
  TBrowserStatusCode status;
  NW_DeckDecoderIter_t iter;

  /* Handle card elements */
  el = (NW_Wml_Element_t*) NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t), NW_Wml_GetCardSegment(thisObj));
  if (!el)
    return KBrsrOutOfMemory;

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, container) != KBrsrSuccess)
    return KBrsrSuccess;

  while (NW_DeckDecoderIter_GetNextElement(&iter, el))
  {
    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case DO_ELEMENT:
      {
        status = RenderDoElement(thisObj, el, card_el_list);
        if (status != KBrsrSuccess && status != KBrsrFailure)
          return status;
        break;
      }

      case A_ELEMENT:
      case ANCHOR_ELEMENT:
      {
        status = RenderAElement(thisObj, el, card_el_list);
        if (status != KBrsrSuccess && status != KBrsrFailure)
          return status;
        break;
      }

      /* simple elements with attributes - add to card_els */
      case INPUT_ELEMENT:
      case BREAK_ELEMENT:
      case IMAGE_ELEMENT:
      case STRING_ELEMENT:
        /* send the element to the display */
        if ((status = (DISPLAY_API->addElement)(thisObj->browser_app,
            el_type, &el->id)) != KBrsrSuccess)
          return status;

        if ((status = NW_WmlList_Append(card_el_list, el))
            == KBrsrOutOfMemory)
          return status;
        break;

      /* recursive elements with attributes - add recursively to card_els */
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
      case SELECT_ELEMENT:
      case P_ELEMENT:
      case FIELDSET_ELEMENT:
      case OPTGRP_ELEMENT:
      case TABLE_ELEMENT:
      case TR_ELEMENT:
      case TD_ELEMENT:
      case OPTION_ELEMENT:
      case PRE_ELEMENT:
      {
        NW_Int16 elemId = 0;
        if ((status = (DISPLAY_API->addElement)(thisObj->browser_app,
            el_type, &el->id)) != KBrsrSuccess)

          return status;

        if ((status = NW_WmlList_Append(card_el_list, el))
            == KBrsrOutOfMemory)
          return status;

        /* recursively call self, then send end element */
        status = RenderCardElements(thisObj, el, card_el_list);
        if (status != KBrsrSuccess && status != KBrsrFailure)
          return status;

        /* ignore the element ids for the end element */
        if ((status = (DISPLAY_API->addElement)(thisObj->browser_app,
            END_ELEMENT, &elemId)) != KBrsrSuccess)
          return status;

        break;
      }

      case UNKNOWN_ELEMENT:
      {
        /*
         * If this unknown element has content, must recurse on it
         * but do not want to send it to the UI
         */
        if (NW_DeckDecoder_HasContent(thisObj->decoder, el))
        {
          status = RenderCardElements(thisObj, el, card_el_list);
          if (status != KBrsrSuccess && status != KBrsrFailure)
            return status;
        }
        break;
      }

      case CARD_ELEMENT:
      case GO_ELEMENT:
      case PREV_ELEMENT:
      case REFRESH_ELEMENT:
      case NOOP_ELEMENT:
      case SETVAR_ELEMENT:
      case ONEVENT_ELEMENT:
      case WML_ELEMENT:
      case HEAD_ELEMENT:
      case TEMPLATE_ELEMENT:
      case END_ELEMENT:
      case TIMER_ELEMENT:
      case ACCESS_ELEMENT:
      case META_ELEMENT:
      case POSTFIELD_ELEMENT:
      default:
        break; /* unknown elements are not going to be rendered */
    }

    el = (NW_Wml_Element_t*) NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t), NW_Wml_GetCardSegment(thisObj));
    if (!el)
      return KBrsrOutOfMemory;
  } 

  return KBrsrSuccess;
}


/* Handle template elements
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - NON-fatal: no template; not a card; etc.
 *         KBrsrOutOfMemory
 *         NW_STAT_* - propagates other fatal errors
 */
static TBrowserStatusCode RenderTemplateElements(NW_Wml_t* thisObj,
    NW_Wml_List_t* card_el_list)
{
  NW_Wml_Element_t *el, container;
  TBrowserStatusCode status;
  NW_DeckDecoderIter_t iter;

  el = (NW_Wml_Element_t*) NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t), NW_Wml_GetCardSegment(thisObj));
  if (!el)
    return KBrsrOutOfMemory;

  if (NW_DeckDecoder_GetTemplateElement(thisObj->decoder, &container) != KBrsrSuccess)
    return KBrsrFailure;

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, &container) != KBrsrSuccess)
    return KBrsrFailure;

  while (NW_DeckDecoderIter_GetNextElement(&iter, el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    if (el_type == DO_ELEMENT)
    {
      NW_Wml_Element_t card_el;
      NW_Ucs2 *task_name = NULL;

      if (!NW_DeckDecoder_GetCardElement(thisObj->decoder, &card_el))
        return KBrsrFailure;
      
      if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, NAME_ATTR,
           thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &task_name)) == KBrsrOutOfMemory )
      {
        return status;
      }
      
      /* if name is not defined, use type as name */
      if (!task_name)
      {
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, TYPE_ATTR,
             thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &task_name)) == KBrsrOutOfMemory )
        {
          return status;
        }
      }
      
      /* if a do element with the same name exists in the deck, the template DO is not displayed */
      if (task_name)
      {
        /* SearchDoElements searches by TYPE if NAME is not available */
        if ((status = NW_DeckDecoder_SearchDoElements(thisObj->decoder,
             thisObj->var_list, &card_el,  NAME_ATTR, task_name)) != KBrsrSuccess )
        {
          status = RenderDoElement(thisObj, el, card_el_list);
          if (status != KBrsrSuccess && status != KBrsrFailure)
            return status;
        }
        
        NW_Str_Delete(task_name);
      }
      else
      {
        status = RenderDoElement(thisObj, el, card_el_list);
        if (status != KBrsrSuccess && status != KBrsrFailure)
          return status;
      }
    }

    el = (NW_Wml_Element_t*) NW_Mem_Segment_Malloc(sizeof(NW_Wml_Element_t), NW_Wml_GetCardSegment(thisObj));
    if (!el)
      return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}


/*
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - NON-fatal
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode RenderDoElement(NW_Wml_t* thisObj,
    NW_Wml_Element_t* do_el,
    NW_Wml_List_t* card_el_list)
{
  NW_Wml_Element_t task_el;
  NW_Wml_ElType_e do_task;
  TBrowserStatusCode status = KBrsrSuccess;

  if (!NW_DeckDecoder_FirstContainedElement(thisObj->decoder, do_el, &task_el))
    return KBrsrSuccess;

  NW_DeckDecoder_GetType(thisObj->decoder, &task_el, &do_task);

  if ((status = (DISPLAY_API->addElement)(thisObj->browser_app, DO_ELEMENT,
      &do_el->id)) != KBrsrSuccess)
    return status;

  /* UI prev softkey processing...
  prev_task_id meaning:
  -1 : initial, no PREV tasks processed
  >= 0 : the id of the only DO or A elem processed which has PREV task
  -2 : more than 1 DO or A elem processed which has PREV task
  do_type_prev_id meaning:
  -1 : initial, no DO processed with type PREV
  >= 0 : the id of the only DO elem processed which has type PREV
  -2 : more than 1 DO elem processed which has type PREV
  */

  /* first check for prev tasks */
  if (do_task == PREV_ELEMENT)
  {
    if (thisObj->prev_task_id == -1)
      thisObj->prev_task_id = do_el->id;
    /*
     * if more than 1 PREV task, then it is misleading to map to
     * a UI construct (like a softkey)
     */
    else if (thisObj->prev_task_id >= 0)
      thisObj->prev_task_id = -2;
  }

  /* second check for prev types */
  if (NW_DeckDecoder_HasAttributeWithValue(thisObj->decoder, do_el, 
      TYPE_ATTR, WAE_ASC_PREV_STR, thisObj->var_list) == NW_TRUE)
  {
    if (thisObj->do_type_prev_id == -1)
      thisObj->do_type_prev_id = do_el->id;
    /*
     * if more than 1 DO has TYPE PREV, then it is misleading
     * to map to a UI construct (like a softkey)
     */
    else if (thisObj->do_type_prev_id >= 0)
      thisObj->do_type_prev_id = -2;
  }

  if ((status = NW_WmlList_Append(card_el_list, do_el)) == KBrsrOutOfMemory)
    return status;

  return KBrsrSuccess;
}


/*
 * RETURN: KBrsrSuccess
 *         KBrsrFailure  - unrecognized element
 *         KBrsrOutOfMemory
 *         NW_STAT_* - propagates other fatal errors
 */
static TBrowserStatusCode RenderAElement(NW_Wml_t* thisObj,
    NW_Wml_Element_t* a_el,
    NW_Wml_List_t* card_el_list)
{
  NW_Wml_Element_t task_el;
  NW_Wml_ElType_e a_type = UNKNOWN_ELEMENT;
  NW_Ucs2 *href = NULL;
  TBrowserStatusCode status;
  NW_Int16 elemid;

  /*
   * Since A and ANCHOR elements appear to mapped to the same
   * enum "A_ELEMENT", we don't know if this is an A or an ANCHOR
   * element.  However, since an A element is required by the WML
   * DTD to have an href attribute, we look for it and if it is
   * found, we assume that the element is an A element and if this
   * attribute is not found, then assume it is an ANCHOR element.
   *
   */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, a_el, HREF_ATTR,
        thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &href)) == KBrsrOutOfMemory)
    return status;

  if (!href)
  {
    NW_DeckDecoderIter_t iter;

    /* Must be an anchor element */
    if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, a_el) != KBrsrSuccess)
      return KBrsrSuccess;

    /* looking for the task element */
    while (NW_DeckDecoderIter_GetNextElement(&iter, &task_el))
    {
      NW_DeckDecoder_GetType(thisObj->decoder, &task_el, &a_type);

      if ((a_type == NOOP_ELEMENT) || (a_type == GO_ELEMENT) ||
          (a_type == PREV_ELEMENT) || (a_type == REFRESH_ELEMENT))
        break;

      else
        a_type = UNKNOWN_ELEMENT;

    } 

    if (a_type == UNKNOWN_ELEMENT)
    {
      return KBrsrSuccess;
    }

    /* a elements for noop tasks are hiden from the UI */
    if (a_type == NOOP_ELEMENT)
    {
      return KBrsrSuccess;
    }
  }
  else
  {
    NW_Mem_Free( href );
  }

  if ((status = (DISPLAY_API->addElement)(thisObj->browser_app, A_ELEMENT,
        &a_el->id)) != KBrsrSuccess)
    return status;

  if ((status = NW_WmlList_Append(card_el_list, a_el)) == KBrsrOutOfMemory)
    return status;

  /* render all text elements */
  status = RenderCardElements(thisObj, a_el, card_el_list);
  if (status != KBrsrSuccess && status != KBrsrFailure)
    return status;

  /* send dummy "end of a" element. the element id is ignored */
  if ((status = (DISPLAY_API->addElement)(thisObj->browser_app, END_ELEMENT,
      &elemid)) != KBrsrSuccess)
    return status;

  return KBrsrSuccess;
}
