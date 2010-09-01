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
    $Workfile: wml_bind.c $

    Purpose:

        Class: WmlBrowser

        Main browser engine variable binding logic
*/


#include "wml_task.h"

#include "nwx_defs.h"
#include "nwx_memseg.h"
#include "nwx_string.h"
#include "wml_elm_attr.h"
#include "BrsrStatusCodes.h"


/*
 * Forward declarations for static functions
 */
static TBrowserStatusCode BindDefaultVariables(NW_Wml_t *thisObj, NW_Wml_Element_t* el);

static TBrowserStatusCode SetName(NW_Wml_t *thisObj, NW_Wml_Element_t* el, NW_Ucs2 **ret_string);

static TBrowserStatusCode SetIName(NW_Wml_t *thisObj, NW_Wml_Element_t* el, NW_Ucs2 **ret_string);

static TBrowserStatusCode SyncNameValue(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* validated_ivalue, NW_Mem_Segment_Id_t mem_scope, NW_Ucs2 **ret_string);

static TBrowserStatusCode SyncINameValue(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* validated_name, NW_Mem_Segment_Id_t mem_scope, NW_Ucs2 **ret_string);

static TBrowserStatusCode GetOptionValue(NW_Wml_t* thisObj, NW_Wml_Element_t* container_el, NW_Int32  option_to_match, NW_Int32  *option_count, NW_Ucs2 **ret_string);

static TBrowserStatusCode GetOptionValueSize(NW_Wml_t* thisObj, NW_Wml_Element_t* container_el, NW_Int32  option_to_match, NW_Int32  *option_count, NW_Int32  *ret_size);

static TBrowserStatusCode GetOptionValueIndex(NW_Wml_t* thisObj, NW_Wml_Element_t* container_el, NW_Ucs2* name, NW_Int32 * option_count, NW_Int32  *ret_index);


/* 
 * process WML container for INPUT and SELECT elems. 
 * apply their NAME and INAME attr variables to the current browser context
 *
 * RECURSIVE
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - propagated from below
 *         KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_ApplyContainedInternalVariables(NW_Wml_t* thisObj,
                                      NW_Wml_Element_t* container)
{
  NW_Wml_Element_t      el;
  TBrowserStatusCode           status = KBrsrSuccess;
  NW_DeckDecoderIter_t  iter;

  NW_ASSERT(thisObj);
  
  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, container) != KBrsrSuccess)
    return KBrsrSuccess;

  while (NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    if (el_type == INPUT_ELEMENT || el_type == SELECT_ELEMENT)
    {
      status = BindDefaultVariables(thisObj, &el);
      if (status == KBrsrOutOfMemory)
        return status;
    }
    else if(el_type == P_ELEMENT || el_type == FIELDSET_ELEMENT)
    {
      status = NW_Wml_ApplyContainedInternalVariables(thisObj, &el);
      if (status == KBrsrOutOfMemory)
        return status;
    }
    else if (el_type == UNKNOWN_ELEMENT)
    {
      /* 
       * If this unknown element has content, recurse on it 
       */
      if (NW_DeckDecoder_HasContent(thisObj->decoder, &el))
      {
        status = NW_Wml_ApplyContainedInternalVariables(thisObj, &el);
        if (status != KBrsrSuccess && status != KBrsrFailure)
          return status;
      }
    }
  }

  return status;
}


/*
 * RETURN: KBrsrSuccess
 *         KBrsrFailure
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode BindDefaultVariables(NW_Wml_t *thisObj,
    NW_Wml_Element_t* el)
{
  NW_Bool res = NW_FALSE;
  NW_Wml_ElType_e el_type;
  TBrowserStatusCode status;
  NW_Ucs2 *name = NULL;
  NW_Ucs2 *value = NULL;
  NW_Ucs2 *format = NULL;
  NW_Ucs2 *emptyok_str = NULL;
  NW_Ucs2 *value2 = NULL;
  NW_Ucs2 *iname = NULL;
  NW_Ucs2 *ivalue = NULL;
  NW_Ucs2 *multi_str = NULL;
  NW_Ucs2 *validated_value  = NULL;
  NW_Ucs2 *validated_ivalue = NULL;
  NW_Ucs2 *syncd_iname_value= NULL;
  NW_Ucs2 *syncd_name_value = NULL;
  NW_Ucs2 *first_option_value = NULL;

  NW_ASSERT(thisObj);
  NW_ASSERT(thisObj->decoder);

  NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

  /* handle INPUT element */
  if (el_type == INPUT_ELEMENT)
  {
    NW_Uint32 publicId;
    NW_WmlInput_EmptyOk_t emptyok = NW_WmlInput_EmptyOk_None;
    NW_Bool partial_validation_ok;
    
    NW_THROWIF_ERROR(status = NW_DeckDecoder_GetDocumentPublicId(thisObj->decoder,&publicId));
    
    /* initialize name value */
    if ((status = SetName(thisObj, el, &value)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();
      
    /* get NAME variable name */
    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, NAME_ATTR, 
         thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &name)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();
      
    /* get FORMAT attr value */
    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, FORMAT_ATTR,
         thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &format)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();
      
    /* get EMPTYOK attr value */
    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, EMPTYOK_ATTR,
         thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &emptyok_str)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();
      
    emptyok = NW_WmlInput_EmptyOk_None;
    /* if no format is specified, default for format is *M and emptyok=true.  (WML-191, section 11.6.3 - emptyok) */
    if (emptyok_str == NULL && (format == NULL || *format == NULL)) 
    {
      emptyok = NW_WmlInput_EmptyOk_True;
    }
    else
    {
      if ( emptyok_str != NULL) {  
        if (NW_Str_StrcmpConst(emptyok_str, WAE_ASC_TRUE_STR) == 0) {
          emptyok = NW_WmlInput_EmptyOk_True;
        } else if (NW_Str_StrcmpConst(emptyok_str, WAE_ASC_FALSE_STR) == 0) {
          emptyok = NW_WmlInput_EmptyOk_False;
        } 
      }
    }

    /* set variable to 0 if it doesn't conform to INPUT FORMAT */
    partial_validation_ok = NW_FALSE;
    if (format)
    {
      if(!NW_WmlInput_ValidateValue(value, format, emptyok,
                                    partial_validation_ok, publicId))
      {
        /*
        11.6.3 If the name variable contains a value that does not 
        confirm to the input mask, the user agent must unset the 
        value and attempt to initialize the variable with the value 
        attriubute.
        */
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, VALUE_ATTR, 
            thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &value2)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();
          
        if(NW_WmlInput_ValidateValue(value2, format, emptyok,
                                     partial_validation_ok, publicId))
        {
          status = NW_Wml_SetVar(thisObj, name, value2);
          /* FAILURE does not imply a fatal error */
          if (status != KBrsrSuccess && status != KBrsrFailure)
            NW_THROW_ERROR();
        }
        else
        {
          NW_Wml_UnsetVar(thisObj, name);
        }
      }
    }
    res = NW_TRUE;
  }
  /* handle SELECT element */
  else if (el_type == SELECT_ELEMENT)
  {    
    NW_Wml_Select_Type_e type = SINGLE_SELECT;

    /* check if multiple selection, default is false */
    if ((status  = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, MULTIPLE_ATTR, 
        thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &multi_str)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();

    if (multi_str && !NW_Str_StrcmpConst(multi_str, WAE_ASC_TRUE_STR))
      type = MULTIPLE_SELECT;

    /* initialize iname and name values */
    if ((status = SetIName(thisObj, el, &ivalue)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();

    if ((status = SetName(thisObj, el, &value)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();
      
    /* get INAME attribute */
    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, INAME_ATTR, 
        thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &iname)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();
            
    /* get NAME attribute */
    if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, NAME_ATTR, 
        thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &name)) == KBrsrOutOfMemory)
      NW_THROW_ERROR();

    /* validate iname and name values */
    if (ivalue)
    {
      if ((status = NW_WmlSelect_ValidateIName_Value(thisObj, el, ivalue,
           type, NW_MEM_SEGMENT_MANUAL, &validated_ivalue)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();
    }
    if (value)
    {
      if ((status = NW_WmlSelect_ValidateName_Value(thisObj, el, value,
           type, NW_MEM_SEGMENT_MANUAL, &validated_value)) == KBrsrOutOfMemory)
        NW_THROW_ERROR();
    }

    /* case 1: value exists */
    if (validated_value)
    {
      if (name)
      {
        status = NW_Wml_SetVar(thisObj, name, validated_value);
        /* FAILURE does not imply a fatal error */
        if (status != KBrsrSuccess && status != KBrsrFailure)
          NW_THROW_ERROR();
      }
      /* sync iname if iname is declared and not already defined */
      if (iname)
      {
        if ((status = SyncINameValue(thisObj, el, validated_value,
             NW_MEM_SEGMENT_MANUAL, &syncd_iname_value)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();
          
        status = NW_Wml_SetVar(thisObj, iname, syncd_iname_value);
        /* FAILURE does not imply a fatal error */
        if (status != KBrsrSuccess && status != KBrsrFailure)
          NW_THROW_ERROR();
      }
    }
    
    /* case 2: ivalue exists, but value doesn't */
    else if (validated_ivalue)
    {
      if (iname)
      {
        status = NW_Wml_SetVar(thisObj, iname, validated_ivalue);
        /* FAILURE does not imply a fatal error */
        if (status != KBrsrSuccess && status != KBrsrFailure)
          NW_THROW_ERROR();
      }

      /* sync name var if name is declared */
      if (name)
      {
        if ((status = SyncNameValue(thisObj, el, validated_ivalue,
             NW_MEM_SEGMENT_MANUAL, &syncd_name_value)) == KBrsrOutOfMemory)
          NW_THROW_ERROR();

        status = NW_Wml_SetVar(thisObj, name, syncd_name_value);
        /* FAILURE does not imply a fatal error */
        if (status != KBrsrSuccess && status != KBrsrFailure)
          NW_THROW_ERROR();
      } 
    }
    /* case 3: neither ivalue or value exist */
    else
    {
      switch (type)
      {
      case MULTIPLE_SELECT:
        break;

      case SINGLE_SELECT:
        if (name)
        {
          if ((status = NW_WmlSelect_GetFirstOptionValue(thisObj, el, NW_MEM_SEGMENT_MANUAL,
               &first_option_value)) == KBrsrOutOfMemory)
            NW_THROW_ERROR();
            
          if (first_option_value)
          {
            status = NW_Wml_SetVar(thisObj, name, first_option_value);
          }
          else
          {
            /* None of the OPTION elements has the VALUE attribute */
            status = KBrsrSuccess;
          }

          /* FAILURE does not imply a fatal error */
          if (status != KBrsrSuccess && status != KBrsrFailure)
            NW_THROW_ERROR();
        }

        if (iname)
        {
          NW_Ucs2 temp_str[16];

          (void)NW_Str_StrcpyConst(temp_str, WAE_ASC_1_STR);
          status = NW_Wml_SetVar(thisObj, iname, temp_str);
          /* FAILURE does not imply a fatal error */
          if (status != KBrsrSuccess && status != KBrsrFailure)
            NW_THROW_ERROR();
        }
        break;

      default:
        break;
      }
    }    
    res = NW_TRUE;
  }

  if (res == NW_TRUE)
    status = KBrsrSuccess;
  else
    status = KBrsrFailure;

  /* fall through */

NW_CATCH_ERROR
  NW_Str_Delete(name);
  NW_Str_Delete(value);
  NW_Str_Delete(format);
  NW_Str_Delete(emptyok_str);
  NW_Str_Delete(value2);
  NW_Str_Delete(iname);

  if (validated_ivalue != ivalue) {
    NW_Str_Delete(validated_ivalue);
  }

  NW_Str_Delete(ivalue);
  NW_Str_Delete(multi_str);
  NW_Str_Delete(validated_value);
  NW_Str_Delete(syncd_iname_value);
  NW_Str_Delete(syncd_name_value);
  NW_Str_Delete(first_option_value);

  return status;
}


/*
 * This function implements the algorithm described in section
 * 11.5.2.1 - in the description of the name/value attributes.
 *
 * If the given select element's name attribute is defined
 * and it points to a variable that is defined, return the
 * the variable's value via the ret_string parameter.
 *
 * Else if the given select element's name attribute is defined
 * and it points to a variable that is NOT defined, if the
 * select element has a value attribute, a variable is created
 * with the element's name and value attributes and the value
 * of the value attribute is returned via the ret_string parameter.
 *
 * Else set *ret_string to NULL.
 *
 * RETURN: KBrsrSuccess
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode SetName(NW_Wml_t *thisObj, NW_Wml_Element_t* el,
                           NW_Ucs2 **ret_string)
{
  NW_Ucs2     *name = NULL;
  TBrowserStatusCode status;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_string);
  
  /* get NAME attribute */
  status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, NAME_ATTR,
                                    thisObj->var_list, NW_MEM_SEGMENT_MANUAL,
                                    &name);
  if (status == KBrsrOutOfMemory)
    return status;

  *ret_string = NULL;

  if (name)
  {
    if ((status = NW_Wml_GetVar(thisObj, name, ret_string)) 
        == KBrsrOutOfMemory)
    {
      NW_Str_Delete(name);
      return status;
    }
  }

  if (*ret_string == 0)
  {
    /* 
     * The select element's name attribute points to a variable
     * name that is not defined.
     */
    status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, VALUE_ATTR,
                                      thisObj->var_list, NW_MEM_SEGMENT_MANUAL,
                                      ret_string);
    if (status == KBrsrOutOfMemory)
    {
      NW_Str_Delete(name);
      return status;
    }

    if (name && (*ret_string != 0))
    {
      /* Set the name to the select element's value attribute */
      status = NW_Wml_SetVar(thisObj, name, *ret_string);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      {
        /* FAILURE does not imply a fatal error */
        NW_Str_Delete(name);
        return status;
      }
    }
  }

  NW_Str_Delete(name);
  return KBrsrSuccess;
}

/*
 * This function implements the algorithm described in section
 * 11.5.2.1 - in the description of the iname/ivalue attributes.
 *
 * If the given select element's iname attribute is defined
 * and it points to a variable that is defined, return the
 * the variable's value via the ret_string parameter.
 *
 * Else if the given select element's iname attribute is defined
 * and it points to a variable that is NOT defined, if the
 * select element has a ivalue attribute, a variable is created
 * with the element's iname and ivalue attributes and the value
 * of the ivalue attribute is returned via the ret_string parameter.
 *
 * Else set *ret_string to NULL.
 *
 *
 * RETURN: KBrsrSuccess
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode SetIName(NW_Wml_t *thisObj, NW_Wml_Element_t* el,
                            NW_Ucs2 **ret_string)
{
  TBrowserStatusCode status;
  NW_Ucs2     *iname = NULL;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_string);
  
  /* get INAME attribute */
  status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, INAME_ATTR,
                                    thisObj->var_list, NW_MEM_SEGMENT_MANUAL,
                                    &iname);
  if (status == KBrsrOutOfMemory)
    return status;

  *ret_string = NULL;

  if (iname) 
  {
    status = NW_Wml_GetVar(thisObj, iname, ret_string);
    if (status == KBrsrOutOfMemory)
    {
      NW_Str_Delete(iname);
      return status;
    }
  }

  if (*ret_string == 0)
  {
    /* 
     * The select element's iname attribute points to a variable
     * name that is not defined.
     */
    status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, IVALUE_ATTR,
                                      thisObj->var_list, NW_MEM_SEGMENT_MANUAL,
                                      ret_string);
    if (status == KBrsrOutOfMemory)
    {
      NW_Str_Delete(iname);
      return status;
    }

    if (iname && (*ret_string != 0))
    {
      status = NW_Wml_SetVar(thisObj, iname, *ret_string);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      {
        /* FAILURE does not imply a fatal error */
        NW_Str_Delete(iname);
        return status;
      }
    }
  }

  NW_Str_Delete(iname);
  return KBrsrSuccess; 
}

/*
 * RETURN: KBrsrSuccess
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode SyncINameValue(NW_Wml_t* thisObj,
    NW_Wml_Element_t    *select_el,
    NW_Ucs2             *validated_name,
    NW_Mem_Segment_Id_t mem_scope,
    NW_Ucs2             **ret_string)
{
/* large enough to format any 32-bit integer */  
#define TEMP_LENGTH 16
  
  NW_Int32    option_item;
  NW_Int32    count = 0;
  NW_Uint32   size  = 0; 
  NW_Bool     done_processing_name = NW_FALSE;
  NW_Ucs2     *start               = validated_name;
  NW_Ucs2     *delimiter_position  = NULL;
  NW_Ucs2     temp_option_value[TEMP_LENGTH];
  TBrowserStatusCode status;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_string);
  NW_ASSERT(validated_name);
  
  do
  {
    /* get next item in validated_name to match */
    delimiter_position = NW_Str_Strchr(start, ';');
    if (delimiter_position != NULL)
    {
      *delimiter_position = 0;
      status = GetOptionValueIndex(thisObj, select_el, start, &count,
                                      &option_item);
      if (status == KBrsrOutOfMemory)
        return status;
      *delimiter_position = ';';
      start = delimiter_position + 1;
    }

    /* last item in semi-colon delimited list */
    else
    {
      status = GetOptionValueIndex(thisObj, select_el, start, &count,
                                      &option_item);
      if (status == KBrsrOutOfMemory)
        return status;
      done_processing_name = NW_TRUE;
    }

    (void)NW_Str_Itoa(option_item, temp_option_value);
    size += NW_Str_Strlen(temp_option_value);
    count = 0;
    if (!done_processing_name)
      size += 1;
    
  } while (!done_processing_name);

  *ret_string = NW_Str_SegNew(size, mem_scope);
  if (*ret_string == NULL)
    return KBrsrOutOfMemory;
  
  **ret_string = 0;
  start = validated_name;
  done_processing_name = NW_FALSE;

  do
  {
    /* get next item in validated_name to match */
    delimiter_position = NW_Str_Strchr(start, ';');
    if (delimiter_position != NULL)
    {
      *delimiter_position = 0;
      status = GetOptionValueIndex(thisObj, select_el, start, &count,
                                      &option_item);
      if (status == KBrsrOutOfMemory)
        return status;
      *delimiter_position = ';';
      start = delimiter_position + 1;
    } 

    /* last item in semi-colon delimited list */
    else
    {
      status = GetOptionValueIndex(thisObj, select_el, start, &count,
                                      &option_item);
      if (status == KBrsrOutOfMemory)
        return status;
      done_processing_name = NW_TRUE;
    }

    (void)NW_Str_Itoa(option_item, temp_option_value);

    /* add delimiter iff not the first option_value pushed into new_name_value */
    if (NW_Str_Strlen(*ret_string))
      (void)NW_Str_StrcatConst(*ret_string, WAE_ASC_SEMICOLON_STR);
    
    (void)NW_Str_Strcat(*ret_string, temp_option_value);
    count = 0;

  } while (!done_processing_name);

  NW_ASSERT(NW_Str_Strlen(*ret_string) <= size);
  return KBrsrSuccess;
}


/*
 * RETURN: KBrsrSuccess
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode SyncNameValue(NW_Wml_t* thisObj,
    NW_Wml_Element_t    *select_el,
    NW_Ucs2             *validated_ivalue,
    NW_Mem_Segment_Id_t mem_scope,
    NW_Ucs2             **ret_string)
{
  NW_Int32    option_item, option_count;
  NW_Uint32   size = 0;
  NW_Bool     done_processing_iname = NW_FALSE;
  NW_Ucs2     *start                = validated_ivalue;
  NW_Ucs2     *delimiter_position   = NULL;
  NW_Ucs2     *option_value         = NULL;
  TBrowserStatusCode status;

  NW_ASSERT(thisObj);
  NW_ASSERT(validated_ivalue);
  NW_ASSERT(ret_string);
  
  do
  {
    NW_Int32  temp_size;

    /* get next item to match */
    delimiter_position = NW_Str_Strchr(start, ';');
    if (delimiter_position != NULL)
    {
      *delimiter_position = 0;
      option_item = NW_Str_Atoi(start);
      *delimiter_position = ';';
      start = delimiter_position + 1;
    } 

    /* last item in semi-colon delimited list */
    else
    {
      option_item = NW_Str_Atoi(start);
      done_processing_iname = NW_TRUE;
    }

    option_count = 0;
    temp_size = 0;

    if ((status = GetOptionValueSize(thisObj, select_el, option_item,
                                        &option_count, &temp_size))
        == KBrsrOutOfMemory)
    {
      return status;
    }

    if ( (temp_size > 0) && (size > 0) )
    {
      size++;   /* Already an option value,  increment size for ';'. */
    }
    size = temp_size + size;

  } while (!done_processing_iname);

  *ret_string = NW_Str_SegNew(size, mem_scope);
  if (*ret_string == NULL)
  {
    return KBrsrOutOfMemory;
  }

  **ret_string = 0;

  if ( size > 0 )
  {
    start = validated_ivalue;
    done_processing_iname = NW_FALSE;

    do
    {
      /* get next item in to match */
      delimiter_position = NW_Str_Strchr(start, ';');
      if (delimiter_position != NULL)
      {
        *delimiter_position = 0;
        option_item = NW_Str_Atoi(start);
        *delimiter_position = ';';
        start = delimiter_position + 1;
      } 

      /* last item in semi-colon delimited list */
      else
      {
        option_item = NW_Str_Atoi(start);
        done_processing_iname = NW_TRUE;
      }
      option_count = 0;
      if ((status = GetOptionValue(thisObj, select_el, option_item,
                                     &option_count, &option_value))
          == KBrsrOutOfMemory)
        return status;
    
      /* TODO: check that this modification doesn't create problems - KM */
      if ( option_value != NULL )
      {
        /* add delimiter iff not the first option_value pushed
           into ret_string */
        if (*ret_string != NULL && NW_Str_Strlen(*ret_string))
          (void)NW_Str_StrcatConst(*ret_string, WAE_ASC_SEMICOLON_STR);

        (void)NW_Str_Strcat(*ret_string, option_value);
        NW_Str_Delete(option_value);
      }
    } while (!done_processing_iname);
    NW_ASSERT(NW_Str_Strlen(*ret_string) <= size);
  }
  else
  {
    /* size == 0 */
    return KBrsrFailure;
  }

  return KBrsrSuccess;
}

/*
 * RECURSIVE
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - stop recursion
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode GetOptionValue(NW_Wml_t* thisObj,
    NW_Wml_Element_t *container_el,
    NW_Int32         option_to_match,
    NW_Int32         *option_count,
    NW_Ucs2          **ret_string)
{
  NW_Wml_Element_t      el;
  TBrowserStatusCode           status;
  NW_DeckDecoderIter_t  iter;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_string);
  NW_ASSERT(option_count);

  *ret_string = NULL;

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, container_el) != KBrsrSuccess)
    return KBrsrFailure;

  while ((*ret_string == NULL) && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);
    
    if (el_type == OPTION_ELEMENT)
    {
      (*option_count)++;

      if (*option_count == option_to_match)
      {
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, 
             &el, VALUE_ATTR, thisObj->var_list, NW_MEM_SEGMENT_MANUAL, 
             ret_string)) == KBrsrOutOfMemory)
        {
          return status;
        }
      }
    }
    else if (el_type == OPTGRP_ELEMENT)
    {
      /* call self */
      if ((status = GetOptionValue(thisObj, &el, option_to_match,
           option_count, ret_string)) == KBrsrOutOfMemory)
      {
        return status;
      }
    }
  }

  return KBrsrSuccess;
}

/*
 * RECURSIVE
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - element not found - NOT fatal (stops recursion)
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode GetOptionValueSize(NW_Wml_t* thisObj,
    NW_Wml_Element_t* container_el,
    NW_Int32  option_to_match,
    NW_Int32  *option_count,
    NW_Int32  *ret_size)
{
  NW_Wml_Element_t el;
  TBrowserStatusCode status;
  NW_DeckDecoderIter_t iter;

  NW_ASSERT(thisObj);
  NW_ASSERT(ret_size);
  NW_ASSERT(option_count);
  
  *ret_size = 0;

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, container_el) != KBrsrSuccess)
  {
    return KBrsrFailure;
  }

  while ((*ret_size == 0) && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);
    
    if (el_type == OPTION_ELEMENT)
    {
      (*option_count)++;
      if (*option_count == option_to_match)
      {
        /* get OPTION element value */
        NW_Ucs2 *option_value = 0;

        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, &el, VALUE_ATTR, 
            thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value)) == KBrsrOutOfMemory)
          return status;
          
        if (option_value)
        {
          *ret_size = NW_Str_Strlen(option_value);
          NW_Str_Delete(option_value);
        }
      }
    }
    /* call self */
    else if (el_type == OPTGRP_ELEMENT)
    {
      if ((status = GetOptionValueSize(thisObj, &el, option_to_match,
           option_count, ret_size)) == KBrsrOutOfMemory)
        return status;
    }
  }

  return KBrsrSuccess;
}

/*
 * RECURSIVE
 *
 * RETURN: KBrsrSuccess
 *         KBrsrFailure - element not found
 *                         - bad content
 *         KBrsrOutOfMemory
 */
static TBrowserStatusCode GetOptionValueIndex(NW_Wml_t* thisObj,
    NW_Wml_Element_t* container_el,
    NW_Ucs2* name, NW_Int32 * option_count,
    NW_Int32  *ret_index)
{
  NW_Bool keep_looping = NW_TRUE;
  NW_Wml_Element_t el;
  NW_Ucs2 *option_value = 0;
  TBrowserStatusCode status;
  NW_DeckDecoderIter_t iter;

  NW_ASSERT(thisObj);
  NW_ASSERT(option_count);
  NW_ASSERT(ret_index);
  
  *ret_index = 0;

  /* loop thru card elements starting at first element after container_el */
  /* (container is either a select or an option group element) */

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj->decoder, container_el) != KBrsrSuccess)
  {
    return KBrsrFailure;
  }

  while (keep_looping && NW_DeckDecoderIter_GetNextElement(&iter, &el))
  {
    NW_Wml_ElType_e el_type;

    NW_DeckDecoder_GetType(thisObj->decoder, &el, &el_type);

    switch (el_type)
    {
      case OPTION_ELEMENT:

        *option_count += 1;
        /* get OPTION element value */
        if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder,
              &el, VALUE_ATTR, thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &option_value))
            == KBrsrOutOfMemory)
          return status;

        if (!option_value)
          break;

        if (!NW_Str_Strcmp(name, option_value))
        {
          *ret_index = *option_count;
          keep_looping = NW_FALSE;
        }

        NW_Str_Delete(option_value);
        break;

      case OPTGRP_ELEMENT:
        if ((status = GetOptionValueIndex(thisObj, &el, name,
              option_count, ret_index)) == KBrsrOutOfMemory)
          return status;

        if (*ret_index > 0)
          keep_looping = NW_FALSE;

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

  return KBrsrSuccess;
}

