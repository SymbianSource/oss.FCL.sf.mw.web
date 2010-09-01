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
    $Workfile: wml_select_ui_state.c $

    Purpose:

        Class: WmlBrowser

        Main browser's select ui state logic
*/

#include "wml_api.h"
#include "wml_task.h"
#include "wml_elm_attr.h"

#include "nwx_memseg.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*lint -save -e571 Suspicious cast*/

static TBrowserStatusCode SetSelectUIState(NW_Wml_t *thisObj, NW_Wml_Element_t* el);

static TBrowserStatusCode SetOptStateToFirstItem(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el);

static TBrowserStatusCode SetOptStatesUsingIName(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* iname_var_value, NW_Wml_Select_Type_e type);

static TBrowserStatusCode SetOptStatesUsingName(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* name_var_value, NW_Wml_Select_Type_e type);

static TBrowserStatusCode MatchOptionIndex(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Int32  option_count_to_match, NW_Wml_Element_t **ret_el);

static TBrowserStatusCode MatchOptionValueUsingElArray(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* name, NW_Wml_Element_t **ret_el);


/* 
 * sets the UI state for each select element in a card using 
 * the select elements NAME and/or INAME variables 
 *
 * RETURN KBrsrSuccess
 *      KBrsrFailure - propagated from NW_DeckDecoder_GetType
 *                 - the card has no elements
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_SetCardUIState(NW_Wml_t* thisObj)
{
  NW_Int32  i = 0;
  NW_Wml_Element_t *el = 0;
  TBrowserStatusCode status;

  if (!thisObj->card_els[0]) return KBrsrFailure;
  
  do
  {
    NW_Wml_ElType_e el_type;

    el = thisObj->card_els[i];
  
    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    if (el_type == SELECT_ELEMENT) {
      if ((status = SetSelectUIState(thisObj, el)) != KBrsrSuccess)
        return status;
    }

  } while (thisObj->card_els[++i]);

  return KBrsrSuccess;
}


/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode SetSelectUIState(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Ucs2 
      *name = 0,               /* NAME attribute */
      *value = 0,              /* VALUE attribute */
      *iname = 0,              /* INAME attribute */
      *ivalue = 0,             /* IVALUE attribute */
      *iname_value = 0,        /* value of the iname attribute's variable */
      *name_value = 0;         /* value of the name attribute'svariable */
  NW_Wml_Select_Type_e type = SINGLE_SELECT;

  /* 
   * get INAME variable name and value 
   */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, select_el, INAME_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &iname)) 
      == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if (iname) 
  {
    if ((status = NW_Wml_GetVar(thisObj, iname, &iname_value))
        == KBrsrOutOfMemory)
      NW_THROW_ERROR();
  }

  /* 
   * get NAME variable name and value 
   */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, select_el, NAME_ATTR,
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &name))
      == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if (name) 
  {
    if ((status = NW_Wml_GetVar(thisObj, name, &name_value)) 
        == KBrsrOutOfMemory)
      NW_THROW_ERROR();
  }

  /* 
   * get select type: single or multiple 
   */
  if (NW_DeckDecoder_HasAttributeWithValue (thisObj->decoder, select_el, MULTIPLE_ATTR,
      WAE_ASC_TRUE_STR, thisObj->var_list))
    type = MULTIPLE_SELECT;

  /* process all possible cases in precedence order */


  /* 
   * if iname variable exists, parse it and 
   * update UI select/option state 
   */
  if (iname)
  {
    status = SetOptStatesUsingIName(thisObj, select_el, iname_value, type);
  }

  /* 
   *  else if name variable exists, parse it 
   *  and update UI select/option state 
   */
  else if (name)
  {
    status = SetOptStatesUsingName(thisObj, select_el, name_value, type);
  }
  /* 
   * else if IVALUE is defined (but no name or iname), use it 
   */ 
  else if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, select_el, IVALUE_ATTR, 
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &ivalue)) != KBrsrFailure)
  {
    if (status == KBrsrSuccess)
      status = SetOptStatesUsingIName(thisObj, select_el, ivalue, type);
  }

  /* 
   * else if VALUE is defined (but no name or iname), use it 
   */ 
  else if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, select_el, VALUE_ATTR, 
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &value)) != KBrsrFailure)
  {
    if (status == KBrsrSuccess)
      status = SetOptStatesUsingName(thisObj, select_el, value, type);
  }

  /* 
   * else if multi-select is defined, then no options are pre-selected 
   */ 
  else if (type == MULTIPLE_SELECT)
  {
    status = NW_WmlInput_InitOptionValues(thisObj, select_el);
  }

  /* 
   * else if single-select is defined, then choose first option 
   */ 
  else
  {
    status = SetOptStateToFirstItem(thisObj, select_el);
  }

  if (status != KBrsrOutOfMemory) 
    status = KBrsrSuccess;

  /* fall through */

NW_CATCH_ERROR
  NW_Str_Delete(name);
  NW_Str_Delete(value);
  NW_Str_Delete(iname);
  NW_Str_Delete(ivalue);
  NW_Str_Delete(iname_value);
  NW_Str_Delete(name_value);

  return status;
}


/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode SetOptStateToFirstItem(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el)
{
  NW_Int32  i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t *el = 0;
  TBrowserStatusCode status;

  /* loop thru card elements starting at select_el */
  do
  {
    NW_Wml_ElType_e el_type;
  
    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app,
             NW_UINT16_CAST(el->id), NW_TRUE)) != KBrsrSuccess)
          return status;
        keep_looping = NW_FALSE;
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
       * */
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
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode SetOptStatesUsingIName(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* iname_var_value, NW_Wml_Select_Type_e type)
{
  NW_Wml_Element_t* matched_option_el = 0;
  TBrowserStatusCode status;
  NW_Ucs2* start = iname_var_value, *delimiter_position = 0;

  /* initialize all options to NW_FALSE */
  if ((status = NW_WmlInput_InitOptionValues(thisObj, select_el)) 
      != KBrsrSuccess)
    return status;

  if (iname_var_value == NULL) {
    return KBrsrSuccess;
  }

  /* handle the delimiter seperated values in the iname value list */
  while (type == MULTIPLE_SELECT && 
        ((delimiter_position = NW_Str_Strchr(start, ';')) != NULL))
  {
    *delimiter_position = 0;
    matched_option_el = 0;

    if ((status = MatchOptionIndex(thisObj, select_el, NW_Str_Atoi(start), 
        &matched_option_el)) != KBrsrSuccess)
      return status;

    if (matched_option_el)
    {
      if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app, 
          NW_UINT16_CAST(matched_option_el->id), NW_TRUE)) != KBrsrSuccess)
        return status;
    }
    *delimiter_position = ';';
    start = delimiter_position + 1;
  } 

  /* handle the last value in the iname value list */
  matched_option_el = 0;

  if ((status = MatchOptionIndex(thisObj, select_el, NW_Str_Atoi(start), 
      &matched_option_el)) != KBrsrSuccess)
    return status;

  /* Reset status */
  status = KBrsrSuccess;

  if (matched_option_el)
  {
    if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app, 
        NW_UINT16_CAST(matched_option_el->id), NW_TRUE)) != KBrsrSuccess)
      return status;
  }
  else if (type == SINGLE_SELECT)
    status = SetOptStateToFirstItem(thisObj, select_el);

  return status;
}

/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode SetOptStatesUsingName(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* name_var_value, NW_Wml_Select_Type_e type)
{
  NW_Wml_Element_t* matched_option_el = 0;
  TBrowserStatusCode status;
  NW_Ucs2* start = name_var_value, *delimiter_position = 0;

  /* initialize all options to NW_FALSE */ 
  if ((status = NW_WmlInput_InitOptionValues(thisObj, select_el)) 
      != KBrsrSuccess)
    return status;

  if (name_var_value == NULL) {
    return KBrsrSuccess;
  }

  /* handle the delimiter seperated values in the name value list */
  while (type == MULTIPLE_SELECT && 
      ((delimiter_position = NW_Str_Strchr(start, ';')) != NULL))
  {
    *delimiter_position = 0;
    matched_option_el = 0;

    if ((status = MatchOptionValueUsingElArray(thisObj, select_el, 
        start, &matched_option_el)) != KBrsrSuccess)
      return status;

    if (matched_option_el)
    {
      if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app, 
          NW_UINT16_CAST(matched_option_el->id), NW_TRUE)) != KBrsrSuccess)
        return status;
    }
    *delimiter_position = ';';
    start = delimiter_position + 1;
  }

  /* handle the last value in the name value list */
  matched_option_el = 0;

  if ((status = MatchOptionValueUsingElArray(thisObj, select_el, 
      start, &matched_option_el)) != KBrsrSuccess)
    return status;

  /* Reset status */
  status = KBrsrSuccess;

  if (matched_option_el)
  {
    if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app, 
        NW_UINT16_CAST(matched_option_el->id), NW_TRUE)) != KBrsrSuccess)
      return status;
  }
  else if (type == SINGLE_SELECT)
    status = SetOptStateToFirstItem(thisObj, select_el);

  return status;
}


/*
 * RETURN KBrsrSuccess
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode MatchOptionIndex(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Int32  option_count_to_match, NW_Wml_Element_t **ret_el)
{
  NW_Int32  option_count = 0, i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t *el = 0;

  do
  {
    NW_Wml_ElType_e el_type;

    el = thisObj->card_els[i];
  
    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        if (++option_count == option_count_to_match)
        {
          *ret_el = el;
          keep_looping = NW_FALSE;
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
 *      KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlInput_InitOptionValues(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el)
{
  NW_Int32  i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t *el = 0;
  TBrowserStatusCode status;

  /* loop thru card elements starting at first element after select_el */
  do
  {
    NW_Wml_ElType_e el_type;

    el = thisObj->card_els[i];
    if(!el)
        return KBrsrWmlbrowserBadContent;
  
    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);
    
    switch (el_type)
    {
      case OPTION_ELEMENT:
        if ((status = (DISPLAY_API->setOptState)(thisObj->browser_app, 
            NW_UINT16_CAST(el->id), NW_FALSE)) != KBrsrSuccess)
          return status;
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
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode MatchOptionValueUsingElArray(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* name, NW_Wml_Element_t **ret_el)
{
  NW_Int32  i = NW_Wml_FindEl(thisObj, NW_UINT16_CAST(select_el->id)) + 1;
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t *el = 0;
  TBrowserStatusCode status;
  NW_Ucs2 *option_value = 0;

  do
  {
    NW_Wml_ElType_e el_type;
  
    el = thisObj->card_els[i];

    NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:

        /* get OPTION element value */ 
        option_value = NULL;
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, VALUE_ATTR, 
            thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value))
            == KBrsrOutOfMemory)
          return status;

        if (option_value && !NW_Str_Strcmp(name, option_value))
        {
          *ret_el = el;
          keep_looping = NW_FALSE;
        }

        NW_Str_Delete(option_value);
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

/*lint -restore*/
