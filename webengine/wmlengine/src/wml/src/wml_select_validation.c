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
    $Workfile: wml_select_validation.c $

    Purpose:

        Class: WmlBrowser

        Main browser's select variable validation logic
*/

#include "wml_task.h"

#include "nwx_memseg.h"
#include "nwx_string.h"
#include "wml_elm_attr.h"
#include "BrsrStatusCodes.h"


#define WML_SELECT_HIGH_RANGE_CHK_OPT_1 1
#define WML_SELECT_HIGH_RANGE_CHK_OPT_2 0


static TBrowserStatusCode GetNumberSelectOptions(NW_Wml_t *thisObj, NW_Wml_Element_t* select_el, NW_Int32  *ret_int);

static TBrowserStatusCode MatchOptionValueUsingBytecode(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* key);

static TBrowserStatusCode GetCorrectedNameValueLength(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* key_var_value, NW_Wml_List_t* default_option_values, NW_Wml_Select_Type_e type, NW_Int32  *ret_int);

static NW_Ucs2* GetCorrectedNameValue(NW_Ucs2* corrected_value, NW_Wml_List_t* default_option_values);

static NW_Bool SelectValidationDupCheck(NW_Ucs2*, NW_Wml_List_t*);


/*
 * RETURN KBrsrSuccess
 *        KBrsrFailure - NON-fatal
 *        KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlSelect_ValidateIName_Value(
    NW_Wml_t             *thisObj,
    NW_Wml_Element_t     *select_el,
    NW_Ucs2              *ikey_var_value,
    NW_Wml_Select_Type_e type,
    NW_Mem_Segment_Id_t mem_scope,
    NW_Ucs2 **ret_string)
{
  NW_Int32      option_item;
  NW_Uint32     size    = 0;
  NW_Ucs2       *start  = ikey_var_value;
  NW_Ucs2       *end    = 0;
  NW_Ucs2       *endstr = 0;
  NW_Ucs2       *delimiter_position     = 0;
  NW_Ucs2       *validated_option_value = 0;
  NW_Ucs2       *ucs2_temp = 0;
  NW_Int32      max_option_items =0;
  NW_Wml_List_t *default_option_index;
  TBrowserStatusCode   status;
  NW_Bool       corrections_made = NW_FALSE;
  NW_Bool       last_option_item = NW_FALSE;

  default_option_index = NW_WmlList_NewEmptyList(NW_Wml_GetCardSegment(thisObj));

  if (!default_option_index)
  {
    return KBrsrOutOfMemory;
  }

  if ((status = GetNumberSelectOptions(thisObj, select_el, &max_option_items)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  if (!max_option_items) {
    max_option_items = 1;
  }
  
  /* 2 pass algorithm. First pass checks for integers, out of bounds values,
     and dup values while calculating the size of a new, corrected ikey value. 
     The second pass builds the new corrected ikey value. If no corrections
     are necessary, only 1 pass is processed.
  */
  do
  {
    if (!start) {
      break;
    }

    delimiter_position = NW_Str_Strchr(start, ';');
    if (type == MULTIPLE_SELECT && (delimiter_position != NULL))
      *delimiter_position = 0;


    /* skip over non-integer indicies */
    endstr = start + NW_Str_Strlen(start);
    option_item = NW_Str_Strtol(start, &end, 10);
    if (end != endstr)
    {
      corrections_made = NW_TRUE;
    }

    /* low range check - remove values less than one */
    else if (option_item < 1)
    {
      corrections_made = NW_TRUE;
    }
    
    /* high range check
       if value is past upper bound - 1.1 spec is contradictory 
         either 1) it should be removed (pg 38, middle validate() behavior) or
                2) it should set the last entry (pg 39, in next to 
           last bullet "If the iname attribute exists.."
       this implements option (2) for now
    */
#if WML_SELECT_HIGH_RANGE_CHK_OPT_1
    /* implements option 1 */
    else if (option_item > max_option_items)
    {
      corrections_made = NW_TRUE;
    }
#endif

#if WML_SELECT_HIGH_RANGE_CHK_OPT_2
    /* implements option 2 */
    else if (option_item > max_option_items)
    {
      ucs2_temp = NW_Str_SegNew(16, NW_MEM_SEGMENT_MANUAL);

      NW_Str_Itoa(max_option_items, ucs2_temp);

      /* verify this isnt a duplicate */
      if (SelectValidationDupCheck(ucs2_temp, default_option_index))
      {
        corrections_made = NW_TRUE;
      }

      /* 
       * Add item str and semicolon, null terminator comes from 
       * WAE_INAME_DELIMITER_STR 
       */
      else
      {
        size += NW_Str_Strlen(ucs2_temp);
        size += last_option_item ? 0 : WAE_ASC_STRLEN(WAE_ASC_SEMICOLON_STR);
        if ((status = NW_WmlList_Append(default_option_index, (void*) ucs2_temp)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();

        ucs2_temp = NULL;
      }
    }
#endif

    /* "normal" option item */
    else
    {
      /* verify this is not a duplicate */
      if (SelectValidationDupCheck(start, default_option_index))
      {
        corrections_made = NW_TRUE;
      }

      /* add item str and semicolon, remove extra null terminator from calc */
      else
      {
        size += NW_Str_Strlen(start);
        size += last_option_item ? 0 : WAE_ASC_STRLEN(WAE_ASC_SEMICOLON_STR);

        ucs2_temp = NW_Str_SegNewcpy(start, NW_Wml_GetCardSegment(thisObj));
        if (!ucs2_temp)
        {
          status = KBrsrOutOfMemory;
          NW_THROW_ERROR();
        }

        if ((status = NW_WmlList_Append(default_option_index, (void*) ucs2_temp)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();

        ucs2_temp = NULL;
      }
    }

    /* 
     * if not the last option item, reset the delimiter and move the 
     * start pointer 
     */
    if (delimiter_position)
    {
      *delimiter_position = ';';
      start = delimiter_position + 1;
    }

  } while (delimiter_position);
  
  /* 
   * if default_option_index is empty, then set ikey var to the new 
   * default value. This depends on whether the select element is 
   * SINGLE or MULTIPLE selection
   */
  if (!NW_WmlList_Length(default_option_index))
  {
    /* 
     * empty default_option_index and multi selection results in 
     * setting the default option index to 0 
     *
     * note that 0 is a valid value for setting the name or iname 
     * variable value, but it is ignored when seelecting options
     */
    if (NW_DeckDecoder_HasAttributeWithValue (thisObj->decoder, select_el, MULTIPLE_ATTR,
        WAE_ASC_TRUE_STR, thisObj->var_list))
    {
      NW_Ucs2 temp_str[16];

      (void)NW_Str_StrcpyConst(temp_str, WAE_ASC_0_STR);
      if ((status = NW_WmlList_Append(default_option_index, temp_str)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();

      size = WAE_ASC_STRLEN(WAE_ASC_0_STR);
    }
    
    /* 
     * empty default_option_index and single selection results in 
     * setting the default option index to 1
     */
    else
    {
      NW_Ucs2 temp_str[16];

      (void)NW_Str_StrcpyConst(temp_str, WAE_ASC_1_STR);
      if ((status = NW_WmlList_Append(default_option_index, temp_str)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();

      size = WAE_ASC_STRLEN(WAE_ASC_1_STR);
    }

    corrections_made = NW_TRUE;
  }

  if (!corrections_made)
  {
    NW_Str_Delete(ucs2_temp);
    *ret_string = ikey_var_value;
    return KBrsrSuccess;
  }

  /* allocated memory for corrected value - include 1 for null terminator */
  *ret_string = NW_Str_SegNew(size, mem_scope);
  if (*ret_string == 0)
  {
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }

  (void)NW_Mem_memset(*ret_string, '\0', (size + 1) * sizeof(NW_Ucs2));
  
  /* 
   * now process through validated default_option_index list and 
   * build corrected value
   */
  validated_option_value = (NW_Ucs2*) NW_WmlList_First(default_option_index);
  if (validated_option_value != NULL)
  {
    do
    {
      (void)NW_Str_Strcat(*ret_string, validated_option_value);

      /* add delimiter if not last item in list*/
      if (!NW_WmlList_IsLast(default_option_index))
        (void)NW_Str_StrcatConst(*ret_string, WAE_ASC_SEMICOLON_STR);
      validated_option_value = (NW_Ucs2*) NW_WmlList_Next(default_option_index);
    } while (validated_option_value != NULL);
  }

  status = KBrsrSuccess;
  /* fall through */

NW_CATCH_ERROR
  NW_Str_Delete(ucs2_temp);

  return status;
}


/* 
 * verifies that each option item inside the key value is matched to
 * an option's value in the select element. This func will return the
 * first option value if any un-matched values are contained in the 
 * key_var_value
 *
 *
 * RETURN: KBrsrSuccess
 *         KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlSelect_ValidateName_Value(NW_Wml_t* thisObj,
    NW_Wml_Element_t* select_el,
    NW_Ucs2* key_var_value,
    NW_Wml_Select_Type_e type,
    NW_Mem_Segment_Id_t mem_scope,
    NW_Ucs2 **ret_string)
{
  NW_Int32  corrected_value_len = 0;
  TBrowserStatusCode status;
  NW_Wml_List_t *default_option_values = NW_WmlList_NewEmptyList(NW_Wml_GetCardSegment(thisObj));

  if (!default_option_values)
  {
    return KBrsrOutOfMemory;
  }

  /* 
   * first, check if any corrections are needed and, if so, 
   * the corrected value length
   */
  if ((status = GetCorrectedNameValueLength(thisObj, select_el, key_var_value, default_option_values, 
      type, &corrected_value_len)) == KBrsrOutOfMemory)
    return status;

  if (!corrected_value_len)
  {
    *ret_string = NULL;
    return KBrsrSuccess;
  }

  *ret_string = NW_Str_SegNew(NW_UINT32_CAST(corrected_value_len), mem_scope);
  if (*ret_string == 0)
  {
    return KBrsrOutOfMemory;
  }

  (void) GetCorrectedNameValue(*ret_string, default_option_values);

  return KBrsrSuccess;
}


/* 
 * return the value from the first option found given a select element 
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure - NON-fatal
 *        KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlSelect_GetFirstOptionValue(NW_Wml_t* thisObj, 
    NW_Wml_Element_t* select_el, 
    NW_Mem_Segment_Id_t mem_scope, 
    NW_Ucs2 **ret_string)
{
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t el;
  NW_DeckDecoderIter_t iter;
  TBrowserStatusCode status;

  *ret_string = 0;

  /* loop thru card elements starting at first element after select_el */

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, select_el) != KBrsrSuccess)
    return KBrsrFailure;

  while ((keep_looping == NW_TRUE) && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:

        /* get OPTION element value */
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, VALUE_ATTR,
              thisObj->var_list, mem_scope, ret_string))
            == KBrsrOutOfMemory)
          return status;

        if (*ret_string == 0)
          break;

        keep_looping = NW_FALSE;
        break;

      case STRING_ELEMENT:
        break;

      case OPTGRP_ELEMENT:  /*   recursion:   optgroup ::= (optgroup|option)+   */
        status = NW_WmlSelect_GetFirstOptionValue(thisObj,&el,mem_scope,ret_string);
        if (status == KBrsrOutOfMemory)
          return status;

        keep_looping = NW_FALSE;
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

      /* Skip an unknown element */
      case UNKNOWN_ELEMENT:
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

  } 

  return KBrsrSuccess;
}


static NW_Bool SelectValidationDupCheck(NW_Ucs2* value_to_validate, 
    NW_Wml_List_t* default_option_index)
{
  NW_Bool dup_found = NW_FALSE;
  NW_Ucs2* validated_option_value = 0;
  
  /* handle a null value_to_validate */
  if (!value_to_validate)
    return dup_found;

  /* 
   * now process through validated default_option_index list and 
   * build corrected value 
   */
  validated_option_value = (NW_Ucs2*) NW_WmlList_First(default_option_index);
  if (validated_option_value != NULL)
  {
    do
    {
      if (!NW_Str_Strcmp(value_to_validate, validated_option_value))
        dup_found = NW_TRUE;
      validated_option_value = (NW_Ucs2*) NW_WmlList_Next(default_option_index);
    } while (!dup_found && (validated_option_value != NULL));
  }

  return dup_found;
}


/*
 * RECURSIVE
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure - NON-fatal
 *                        - element not supported
 *        KBrsrOutOfMemory
 */
static TBrowserStatusCode GetNumberSelectOptions(NW_Wml_t *thisObj, 
    NW_Wml_Element_t* select_el, 
    NW_Int32  *ret_int)
{
  NW_Int32  option_count = 0;
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t el;
  TBrowserStatusCode status;
  NW_DeckDecoderIter_t iter;

  /* loop thru card elements starting at first element after select_el */

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, select_el) != KBrsrSuccess)
    return KBrsrFailure;

  /* loop thru card elements starting at select_el */
  while ((keep_looping == NW_TRUE) && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;
  
    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:
        option_count++;
        break;

      case OPTGRP_ELEMENT:
        if ((status = GetNumberSelectOptions(thisObj, &el, ret_int)) 
          != KBrsrSuccess) {
          return status;
        }
        option_count += *ret_int;
        break;

      /* 
       * option element may contain strings, emphasis, tab, or 
       * break elements - skip over 
       */
      case STRING_ELEMENT:
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
        break;

      /* Skip an unknown element */
      case UNKNOWN_ELEMENT:
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

      case END_ELEMENT:
        keep_looping = NW_FALSE;
        break;

      /* bad bytecode, raise exception */
      default:
        return KBrsrFailure;
    }

  } 

  *ret_int = option_count;

  return KBrsrSuccess;
}

/*
 * RETURN KBrsrSuccess
 *        KBrsrFailure
 *        KBrsrOutOfMemory
 */
static TBrowserStatusCode GetCorrectedNameValueLength(NW_Wml_t* thisObj,
    NW_Wml_Element_t* select_el,
    NW_Ucs2* key_var_value,
    NW_Wml_List_t* default_option_values,
    NW_Wml_Select_Type_e type,
    NW_Int32  *ret_length)
{
  NW_Ucs2 *start = key_var_value;
  NW_Ucs2 *delimiter_position = 0, *ucs2_temp = 0;
  NW_Int32  corrected_value_len = 0;
  TBrowserStatusCode status;

  *ret_length = 0;

  if (!start)
  {
    /* cases where there is no value to check */
    return KBrsrSuccess;
  }

  /* handle the delimiter separated values */
  do
  {
    if (start[0] == 0)/* check for null terminator */
      break;

    if (type == MULTIPLE_SELECT &&
       ((delimiter_position = NW_Str_Strchr(start, ';')) != NULL))
      *delimiter_position = 0;
    
    /* 
     * check for a match with an option value and, if so, whether 
     * its not a duplicate
     * 
     */
    if ((status = MatchOptionValueUsingBytecode(thisObj, select_el,start))
        == KBrsrOutOfMemory)
      return status;

    if (status == KBrsrSuccess &&
        !SelectValidationDupCheck(start, default_option_values))
    {
      /* set new length */
      if (!corrected_value_len)
        corrected_value_len += NW_Str_Strlen(start);
      else
        /* add 1 for the preceeding semicolon */
        corrected_value_len += (NW_Str_Strlen(start) + 1);

      /* add to default option values list */
      ucs2_temp = NW_Str_SegNewcpy(start, default_option_values->memSegment);
      if (!ucs2_temp)
        return KBrsrOutOfMemory;

      status = NW_WmlList_Append(default_option_values, (void*) ucs2_temp);
      if (status == KBrsrOutOfMemory)
      {
        NW_Str_Delete(ucs2_temp);
        return status;
      }
    }

    if (delimiter_position)
    {
      *delimiter_position = ';';
      start = delimiter_position + 1;
    }
  } while (delimiter_position);

  *ret_length = corrected_value_len;

  return KBrsrSuccess;
}


static NW_Ucs2* GetCorrectedNameValue(NW_Ucs2* corrected_value,
    NW_Wml_List_t* default_option_values)
{
  NW_Ucs2* validated_option_value = 0;

  /* initialize the corrected_value */
  (void)NW_Str_StrcpyConst(corrected_value, WAE_ASC_EMPTY_STR);

  /*  
   * use default_option_values for implementation 
   *
   * now process through validated default_option_index list 
   * and build corrected value 
   */
  validated_option_value = (NW_Ucs2*) NW_WmlList_First(default_option_values);
  if (validated_option_value != NULL)
  {
    do
    {
      (void)NW_Str_Strcat(corrected_value, validated_option_value);
    
      /* add delimiter if not last item in list*/
      if (!NW_WmlList_IsLast(default_option_values))
        (void)NW_Str_StrcatConst(corrected_value, WAE_ASC_SEMICOLON_STR);
      validated_option_value = (NW_Ucs2*) NW_WmlList_Next(default_option_values);
    } while (validated_option_value != NULL);
  }

  return corrected_value;
}


/*
 * RECURSIVE
 *
 * RETURN KBrsrSuccess
 *        KBrsrFailure - NO match found
 *                        - unexpected element
 *        KBrsrOutOfMemory
 */
static TBrowserStatusCode MatchOptionValueUsingBytecode(NW_Wml_t* thisObj, 
    NW_Wml_Element_t* container_el, 
    NW_Ucs2* key) 
{
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t el;
  TBrowserStatusCode status;
  NW_Ucs2 *option_value = NULL;
  NW_Bool match_found = NW_FALSE;
  NW_DeckDecoderIter_t iter;

  /* 
   * loop thru card elements starting at first element after 
   * container_el.  (container is either a select or an 
   * option group element) 
   */

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, container_el) != KBrsrSuccess)
  {
    return KBrsrFailure;
  }

  while ((keep_looping == NW_TRUE) && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;
  
    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:

        /* get OPTION element value */ 
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, 
              &el, VALUE_ATTR, thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value))
            == KBrsrOutOfMemory)
          return status;

        if (!option_value) 
          break;

        if (!NW_Str_Strcmp(key, option_value))
        {
          match_found = NW_TRUE;
          keep_looping = NW_FALSE;
        }

        NW_Str_Delete(option_value);
        option_value = NULL;
        break;

      case OPTGRP_ELEMENT:
        if ((status = MatchOptionValueUsingBytecode(thisObj, &el, key))
             == KBrsrOutOfMemory)
          return status;
        if (status == KBrsrSuccess)
        {
          match_found = NW_TRUE;
          keep_looping = NW_FALSE;
        }
        break;
        
      /* 
       * option element may contain strings, emphasis, tab, or 
       * break elements - skip over 
       */
      case STRING_ELEMENT:
      case EMPHASIS_ELEMENT:
      case STRONG_ELEMENT:
      case ITALIC_ELEMENT:
      case BOLD_ELEMENT:
      case UNDERLINE_ELEMENT:
      case BIG_ELEMENT:
      case SMALL_ELEMENT:
        break;
      
      /* Skip an unknown element */
      case UNKNOWN_ELEMENT:
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

      case END_ELEMENT:
        keep_looping = NW_FALSE;
        break;

      /* unexpected element - present error dialog */
      default:
        return KBrsrFailure;
    }

  } 

  if (match_found) return KBrsrSuccess; else return KBrsrFailure;
}
