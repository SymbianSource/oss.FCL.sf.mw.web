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
    $Workfile: wml_option_event.c $

    Purpose:

        Class: WmlBrowser

        Maintain browser's option_event state logic
   
   $Option_Event: wml_option_event.c $

*/

#include "wml_task.h"
#include "wml_api.h"
#include "wml_url_utils.h"
#include "wml_elm_attr.h"

#include "nwx_memseg.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"


/*lint -save -e571 Suspicious cast*/

static TBrowserStatusCode HandleOptionIntrinsicEvents(NW_Wml_t *thisObj, NW_Wml_Element_t* option_el);


/* funcs used with NW_Wml_HandleOptionEvent */
static TBrowserStatusCode CalculateNameSize(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Int32  *ret_size);

static TBrowserStatusCode CalculateINameSize(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Int32  *ret_size);

static TBrowserStatusCode BuildNameValue(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* new_key_value);

static TBrowserStatusCode BuildINameValue(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* new_key_value);


/* 
 * handle user input to the Select element 
 * this func needs to be broken up 
 *
 * RETURN KBrsrSuccess
 *      KBrsrFailure - card has no select element
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_HandleOptionEvent(NW_Wml_t *obj, NW_Wml_Element_t* option_el)
{
  NW_Wml_t *thisObj = obj;
  NW_Wml_Element_t *el = 0, *select_el = 0;
  NW_Bool searching;
  NW_Int32  i;
  NW_Uint16 option_count = 0;
  typedef enum {WAE_SELECTION, WAE_DESELECTION} wml_select_mode_e;
  wml_select_mode_e mode;
  NW_Wml_Select_Type_e type = SINGLE_SELECT;
  TBrowserStatusCode status = KBrsrSuccess;

  NW_Ucs2 *key_var_name = NULL;
  NW_Ucs2 *ikey_var_name = NULL;
  NW_Ucs2 *option_value = NULL;
  NW_Ucs2 *new_key_value = NULL;
  NW_Ucs2 *new_ikey_value = NULL;
  
  /*
  * find the select element and option index associated with the
  * option event
  */
  searching = NW_TRUE;
  i = 0;
  do
  {
    NW_Wml_ElType_e el_type;

    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    if (el_type == SELECT_ELEMENT)
    {
      select_el = el;
      option_count = 0;
    }
    else if (el_type == OPTION_ELEMENT)
    {
      option_count++;
      if (el->id == option_el->id)
        searching = NW_FALSE;
    }
  } while (searching && thisObj->card_els[++i]);

  if (!select_el)
    NW_THROW_ERROR();


  /* get NAME variable name */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, select_el, NAME_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &key_var_name)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* get INAME variable name */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, select_el, INAME_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &ikey_var_name)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* check if multiple selection, default is false */
  if (NW_DeckDecoder_HasAttributeWithValue (thisObj->decoder, select_el, MULTIPLE_ATTR,
      WAE_ASC_TRUE_STR, thisObj->var_list))
    type = MULTIPLE_SELECT;

  /* get OPTION element value */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, option_el, VALUE_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* determine whether event is a SELECTION or a DESELECTION */
  /* for now, single select can't deselect */
  /* note that the UI has already set the option state */
  mode = WAE_SELECTION;
  if (type == MULTIPLE_SELECT)
  {
    NW_Bool st;
    if ((status = (DISPLAY_API->getOptState)(thisObj->browser_app, 
        NW_UINT16_CAST(option_el->id), &st)) != KBrsrSuccess)
      NW_THROW_ERROR();

    if (!st) 
      mode = WAE_DESELECTION;
  }

  switch (type)
  {
    case SINGLE_SELECT:

      /* initialize all options to NW_FALSE */
      if ((status = NW_WmlInput_InitOptionValues(thisObj, select_el)) != KBrsrSuccess)
        NW_THROW_ERROR();

      switch (mode)
      {
        case WAE_SELECTION:

          /* set shared ui option state */
          if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app,
              NW_UINT16_CAST(el->id), NW_TRUE)) != KBrsrSuccess)
            NW_THROW_ERROR();

          /* 
           * set NAME and/or INAME variables.  Note that option_value
           * may be null if the option element has no value attribute
           */
          if (key_var_name && option_value)
          {
            status = NW_Wml_SetVar(thisObj, key_var_name, option_value);
            /* FAILURE does not imply a fatal error */
            if (status != KBrsrSuccess && status != KBrsrFailure)
              NW_THROW_ERROR();
          }

          if (ikey_var_name)
          {
            NW_Ucs2 temp[16];
            (void)NW_Str_Itoa(option_count, temp);
            status = NW_Wml_SetVar(thisObj, ikey_var_name, temp);
            /* FAILURE does not imply a fatal error */
            if (status != KBrsrSuccess && status != KBrsrFailure)
              NW_THROW_ERROR();
          }

          /* handle ONEVENT and ONCLICK on SELECTION only */
          status = HandleOptionIntrinsicEvents(thisObj, option_el);
          if (status == KBrsrOutOfMemory)
            NW_THROW_ERROR();

          break;

        case WAE_DESELECTION:
          if (key_var_name)
            NW_Wml_UnsetVar(thisObj, key_var_name);

          if (ikey_var_name)
            NW_Wml_UnsetVar(thisObj, ikey_var_name);

        break;
      }

      break; /* SINGLE_SELECT */

    case MULTIPLE_SELECT:
      /* build key & ikey variables */
      if (key_var_name)
      {
        NW_Int32  key_size = 0;

        if ((status = CalculateNameSize(thisObj, select_el, &key_size)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();

        if (key_size)
        {
          new_key_value = NW_Str_SegNew(NW_UINT32_CAST(key_size), NW_MEM_SEGMENT_MANUAL);
          if (!new_key_value)
          {
            status = KBrsrOutOfMemory;
            NW_THROW_ERROR();
          }

          if ((status = BuildNameValue(thisObj, select_el, new_key_value)) == KBrsrOutOfMemory)
            NW_THROW_ERROR();

          status = NW_Wml_SetVar(thisObj, key_var_name, new_key_value);
          /* FAILURE does not imply a fatal error */
          if (status != KBrsrSuccess && status != KBrsrFailure)
            NW_THROW_ERROR();
        }
        /* set key_var to null if no options selected */
        else
          NW_Wml_UnsetVar(thisObj, key_var_name);
      }

      if (ikey_var_name)
      {
        NW_Int32  ikey_size;

        if ((status = CalculateINameSize(thisObj, select_el, &ikey_size)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();

        if (ikey_size)
        {
          new_ikey_value = NW_Str_SegNew(NW_UINT32_CAST(ikey_size), NW_MEM_SEGMENT_MANUAL);
          if (!new_ikey_value)
          {
            status = KBrsrOutOfMemory;
            NW_THROW_ERROR();
          }

          if ((status = BuildINameValue(thisObj, select_el, new_ikey_value)) == KBrsrOutOfMemory)
            NW_THROW_ERROR();

          status = NW_Wml_SetVar(thisObj, ikey_var_name, new_ikey_value);
          /* FAILURE does not imply a fatal error */
          if (status != KBrsrSuccess && status != KBrsrFailure)
            NW_THROW_ERROR();
        }
        /* set ikey_var to null if no options selected */
        else
          NW_Wml_UnsetVar(thisObj, ikey_var_name);
      }

      /* do OPTION events for both SELECTION and DESELECTION */
      status = HandleOptionIntrinsicEvents(thisObj, option_el);
      if (status == KBrsrOutOfMemory)
        NW_THROW_ERROR();

    break; /* MULTIPLE_SELECT */
  }

  if (status == KBrsrFailure)
  {
    status = KBrsrSuccess; /* failure in the preceding code does not mean broken */
  }

  /* fall through */

NW_CATCH_ERROR
  NW_Str_Delete(key_var_name);
  NW_Str_Delete(ikey_var_name);
  NW_Str_Delete(option_value);
  NW_Str_Delete(new_key_value);
  NW_Str_Delete(new_ikey_value);

  return status;
}


/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode CalculateNameSize(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Int32  *ret_size)
{
  NW_Int32  i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1, 
         option_value_count = 0, 
         size = 0;
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t *el = 0;
  TBrowserStatusCode status;

  *ret_size = 0;

  /* loop thru card elements starting at select_el */
  do
  {
    NW_Wml_ElType_e el_type;
    NW_Bool st = NW_FALSE;

    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        if ((status = (DISPLAY_API->getOptState)(thisObj->browser_app, NW_UINT16_CAST(el->id), 
              &st)) != KBrsrSuccess)
        {
          return status;
        }
        if (st)
        {
          /* get OPTION element value */
          NW_Ucs2 *option_value = NULL;

          if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, VALUE_ATTR,
              thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value)) == KBrsrOutOfMemory)
            return status;

          if (option_value)
          {
            size += NW_Str_Strlen(option_value);
            option_value_count++;

            NW_Str_Delete(option_value);
          }
        }
        break;

      case STRING_ELEMENT:
      case OPTGRP_ELEMENT:
        break;

      /* 
       * option element may contain emphasis, tab, or break elements 
       * - skip over 
       */
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
        break;

      /* 
       * image, A, and break elems can be contained inside a select 
       * via %flow contained within an emph elem 
       */
      case A_ELEMENT:
      case ANCHOR_ELEMENT:
      case IMAGE_ELEMENT:
      case BREAK_ELEMENT:
        break;

      default:
        keep_looping = NW_FALSE;
        break;
    }
    
  } while (keep_looping && thisObj->card_els[++i]);

  if(option_value_count)
  {
    NW_Uint32 tmplen = WAE_ASC_STRLEN(WAE_ASC_SEMICOLON_STR);
    NW_ASSERT(size);
    /* option values are seperated by delimiters */
    size += ((option_value_count * NW_INT32_CAST(tmplen)) - 1);
  }

  *ret_size = size;

  return KBrsrSuccess;
}


/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode CalculateINameSize(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Int32  *ret_size)
{
  NW_Int32         i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Int32         option_count = 0; 
  NW_Uint32        size = 0;
  NW_Bool          keep_looping = NW_TRUE;
  NW_Bool          found_first_option = NW_FALSE;
  TBrowserStatusCode      status;
  NW_Wml_Element_t *el = 0;

  *ret_size = 0;

  /* loop thru card elements starting at select_el */
  do
  {
    NW_Wml_ElType_e el_type;
    NW_Bool st = NW_FALSE;

    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        option_count++;
        if ((status = (DISPLAY_API->getOptState)(thisObj->browser_app, NW_UINT16_CAST(el->id), &st)) 
                        != KBrsrSuccess)
          return status;

        if (st)
        {
          /* don't count semicolon for first selected option */
          if (!found_first_option)
          {
            /* add null ucs2 char once only */
            size = sizeof(NW_Ucs2);

            /* one NW_Ucs2 */
            if (option_count < 10)
              size += sizeof(NW_Ucs2);

            /* two ucs2_chars */
            if (option_count >= 10 && option_count < 100)
              size += (sizeof(NW_Ucs2) * 2);

            /* three ucs2_chars. yikes - this shouldn't happen */
            if (option_count >= 100)
              size += (sizeof(NW_Ucs2) * 3);

            found_first_option = NW_TRUE;
          }

          /* add the preceeding semicolon to size count */
          else
          {
            /* one NW_Ucs2 plus semicolon */
            if (option_count < 10)
              size += (sizeof(NW_Ucs2) * 2);

            /* two ucs2_chars plus semicolon */
            if (option_count >= 10 && option_count < 100)
              size += (sizeof(NW_Ucs2) * 3);

            /* three ucs2_chars plus semicolon. yikes - this shouldn't happen */
            if (option_count >= 100)
              size += (sizeof(NW_Ucs2) * 4);
          }
        }
        break;

      case STRING_ELEMENT:
      case OPTGRP_ELEMENT:
        break;

      /* 
       * option element may contain emphasis, tab, or break elements 
       * - skip over 
       */
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
        break;

      /* 
       * image, A, and break elems can be contained inside a select 
       * via %flow contained within an emph elem 
       */
      case A_ELEMENT:
      case ANCHOR_ELEMENT:
      case IMAGE_ELEMENT:
      case BREAK_ELEMENT:
        break;

      default:
        keep_looping = NW_FALSE;
       break;
    }
    
  } while (keep_looping && thisObj->card_els[++i]);

  *ret_size = NW_INT32_CAST(size);

  return KBrsrSuccess;
}


/*
 * RETURN KBrsrSuccess
 *        KBrsrOutOfMemory
 */
static TBrowserStatusCode BuildNameValue(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* new_key_value)
{

  NW_Int32  i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Bool keep_looping = NW_TRUE;
  TBrowserStatusCode status;
  NW_Wml_Element_t *el = 0;

  /* loop thru card elements starting at select_el */
  do
  {
    NW_Wml_ElType_e el_type;
    NW_Bool st;

    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        if ((status = (DISPLAY_API->getOptState)(thisObj->browser_app, NW_UINT16_CAST(el->id), &st))
                          != KBrsrSuccess)
        {
          return status;
        }
        if (st)
        {
          /* get OPTION element value */
          NW_Ucs2 *option_value = NULL;

          if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, VALUE_ATTR,
              thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value)) == KBrsrOutOfMemory)
            return status;

          if (option_value)
          {
            /* 
             * add delimiter iff not the first option_value pushed 
             * into new_key_value 
             */
            if (NW_Str_Strlen(new_key_value)) {
              (void)NW_Str_StrcatConst(new_key_value, WAE_ASC_SEMICOLON_STR);
            }
            (void)NW_Str_Strcat(new_key_value, option_value);

            NW_Str_Delete(option_value);
          }
        }
        break;

      case STRING_ELEMENT:
      case OPTGRP_ELEMENT:
        break;

      /* 
       * option element may contain emphasis, tab, or break elements 
       * - skip over 
       */
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
        break;

      /* 
       * image, A, and break elems can be contained inside a select 
       * via %flow contained within an emph elem 
       */
      case A_ELEMENT:
      case ANCHOR_ELEMENT:
      case IMAGE_ELEMENT:
      case BREAK_ELEMENT:
        break;

      default:
        keep_looping = NW_FALSE;
        break;
    }
    
  } while (keep_looping && thisObj->card_els[++i]);

  return KBrsrSuccess;
}


/*
 * RETURN KBrsrSuccess
 *        KBrsrOutOfMemory
 */
static TBrowserStatusCode BuildINameValue(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Ucs2 *new_ikey_value)
{

  NW_Int32  i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Uint16 option_count = 0;
  NW_Bool keep_looping = NW_TRUE;
  TBrowserStatusCode status;
  NW_Wml_Element_t *el = 0;

  /* loop thru card elements starting at select_el */
  do
  {
    NW_Wml_ElType_e el_type;
    NW_Bool st = NW_FALSE;

    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        option_count++;
        if ((status = (DISPLAY_API->getOptState)(thisObj->browser_app, NW_UINT16_CAST(el->id), 
              &st)) != KBrsrSuccess)
        {
          return status;
        }
        if (st)
        {
          NW_Ucs2 ucs2_temp[16];

          /* 
           * add delimiter iff not the first option_value pushed into
           * new_key_value
           */
          if (NW_Str_Strlen(new_ikey_value))
            (void)NW_Str_StrcatConst(new_ikey_value, WAE_ASC_SEMICOLON_STR);

          /* 
           * convert option_count integer to ucs2 string and concat
           * to new_ikey_value
           */
          (void)NW_Str_Itoa(option_count, ucs2_temp);
          (void)NW_Str_Strcat(new_ikey_value, ucs2_temp);
        }
        break;

      case STRING_ELEMENT:
      case OPTGRP_ELEMENT:
        break;

      /* 
       * option element may contain emphasis, tab, or break elements 
       * - skip over 
       */
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
        break;

      /* 
       * image, A, and break elems can be contained inside a select 
       * via %flow contained within an emph elem 
       */
      case A_ELEMENT:
      case ANCHOR_ELEMENT:
      case IMAGE_ELEMENT:
      case BREAK_ELEMENT:
        break;

      default:
        keep_looping = NW_FALSE;
        break;
    }
    
  } while (keep_looping && thisObj->card_els[++i]);

  return KBrsrSuccess;
}



/*
 * Returns KBrsrSuccess or propagates errors from other CB calls
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure -
 *        KBrsrOutOfMemory
 */
static TBrowserStatusCode HandleOptionIntrinsicEvents(NW_Wml_t *thisObj, NW_Wml_Element_t* option_el)
{
  TBrowserStatusCode status;

  status = NW_Wml_HasOptionIntrinsicEvents(thisObj, option_el, NW_TRUE);
  return status;
}


/*
 * execute arg: Set to NW_TRUE if you want to process event
 *              Set to NW_FALSE if you're just checking if event exists, but
 *                don't want to process it
 *
 * Returns KBrsrSuccess or propagates errors from other CB calls
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure -
 *        KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_HasOptionIntrinsicEvents(NW_Wml_t *thisObj, NW_Wml_Element_t* option_el, NW_Bool execute)
{
  TBrowserStatusCode status;

  /* first handle ONCLICK */
  NW_Ucs2 *url = 0;

  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, option_el, ONPICK_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &url)) == KBrsrOutOfMemory)
    return status;

  if (url)
  {
    if (execute == NW_FALSE)
    {
      if ( *url != 0 )
      {
        /* Onpick is not an empty string.  Is an onpick. */
        NW_Mem_Free( url );
        return KBrsrSuccess;
      }
      NW_Mem_Free(url);
    }
    else
    {
      NW_Ucs2 *target_url = 0;
      
      /* check if we are allowed to navigate. */
      status = NW_Wml_CanNavigateToUri();
      if (status != KBrsrSuccess) {
        NW_Mem_Free(url);
        return status;
      }
  
      thisObj->curr_task = GO_TASK;
  
      status = NW_Wml_GetFullUrl(thisObj, url, &target_url);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      {
        /* FAILURE does not imply a fatal error */
        return status;
      }
  
      /* if no url, then perhaps a dialog should be presented to the user */
      if (target_url)
      {
        status = NW_Wml_FetchUrl(thisObj, target_url, 0);
        NW_Mem_Free(target_url);
        if (status != KBrsrSuccess) {
          NW_Mem_Free(url);
          return status;
        }
      }
      NW_Mem_Free(url);
      /* inform caller that an onpick event has been executed */
      if ((status == KBrsrSuccess) || (status == KBrsrFailure))
      {
        return KBrsrWmlbrowserOnPickEx;
      }
    }
  }

  /* then handle ONEVENT */
  else
  {
    NW_Wml_Element_t el;
    NW_DeckDecoderIter_t iter;

    if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, option_el) != KBrsrSuccess)
      return KBrsrFailure;

    while (NW_DeckDecoderIter_GetNextElement(&iter, &el))
    {
      NW_Wml_ElType_e el_type;

      NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

      switch (el_type)
      {
        case ONEVENT_ELEMENT:
        {
          if (NW_DeckDecoder_HasAttributeWithValue (thisObj->decoder, &el, TYPE_ATTR,
              WAE_ASC_ONPICK_STR, thisObj->var_list))
          {
            if (execute == NW_FALSE)
            {
              status = KBrsrSuccess;
            }
            else
            {
              status = NW_Wml_HandleTaskEvent(thisObj, &el);

              /* inform caller that an onpick event has been executed */
              if ((status == KBrsrSuccess) || (status == KBrsrFailure))
              {
                status = KBrsrWmlbrowserOnPickEx;
              }
            }
            return status;
          }
          break;
        }

        case STRING_ELEMENT:
          break;

        /* OPTION may contain any text type */
        case EMPHASIS_ELEMENT:
        case STRONG_ELEMENT:
        case ITALIC_ELEMENT:
        case BOLD_ELEMENT:
        case UNDERLINE_ELEMENT:
        case BIG_ELEMENT:
        case SMALL_ELEMENT:
          break;

        /* 
         * image, A, and break elems can be contained inside a select 
         * via %flow contained within an emph elem 
         */
        case A_ELEMENT:
        case ANCHOR_ELEMENT:
        case IMAGE_ELEMENT:
        case BREAK_ELEMENT:
          break;

        default:
          return KBrsrFailure;
      }
    }
  }

  return KBrsrFailure;
}


/*lint -restore*/
