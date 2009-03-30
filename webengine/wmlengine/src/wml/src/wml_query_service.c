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
    $Workfile: wml_query_service.c $

    Purpose:

        Class: WmlBrowser

        Handles data query requests for the Browser (implementation)
*/

#include "wml_task.h"
#include "wml_elements.h"
#include "wml_history.h"
#include "wml_url_utils.h"
#include "wml_elm_attr.h"

#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - attribute not found
 *       KBrsrBadInputParam - bad input parameters
 *                 - el_id is bad
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_GetAttribute(NW_Wml_t *thisObj, NW_Uint16 el_id, NW_Wml_Attr_t tag, NW_Ucs2 **ret_string)
{
  NW_Wml_Element_t *el;
  NW_Wml_ElType_e  el_type;
  TBrowserStatusCode      status;
  
  if ((status = NW_Wml_GetElementType(thisObj, el_id, &el_type, &el)) != KBrsrSuccess)
    return status;

  if (tag == STR_VALUE)
  {
    if (el_type == STRING_ELEMENT)
    {
      status = NW_DeckDecoder_ReadString(thisObj->decoder, el, thisObj->var_list, NW_MEM_SEGMENT_MANUAL, ret_string);
      if (status == KBrsrOutOfMemory)
        return status;
    }
    else if ((el_type == OPTION_ELEMENT) || (el_type == A_ELEMENT) || (el_type == ANCHOR_ELEMENT))
    {
      NW_Wml_Element_t string_el;
      NW_Wml_ElType_e el_type_2;

      if (!NW_DeckDecoder_FirstContainedElement(thisObj->decoder, el, &string_el)) 
        return KBrsrFailure;

      NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type_2);

      if (el_type != STRING_ELEMENT) 
        return KBrsrFailure;

      status = NW_DeckDecoder_ReadString(thisObj->decoder, el, thisObj->var_list, NW_MEM_SEGMENT_MANUAL, ret_string);
      if (status == KBrsrOutOfMemory)
        return status;
    }
    else
    {
      return KBrsrFailure;
    }
  }
  else
  {
    status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, tag,
                                         thisObj->var_list, NW_MEM_SEGMENT_MANUAL, ret_string);
    if (status == KBrsrOutOfMemory)
      return status;
 
    /* This is a bit of a hack to put this here.  However I want to make sure
    WML and the UI see the same behavior for the invalid format values.
    Return NULL pointer for empty or invalid format.  This will be treated
    like no format or *M in WML. jwild 2000.10.10 */

    if ( (tag == FORMAT_ATTR) && (*ret_string != NULL) &&
         (! NW_WmlInput_ValidateFormat(*ret_string)) )
    {
      NW_Mem_Free(*ret_string);
      *ret_string = NULL;
    }
  }
  
  return KBrsrSuccess;
}

/* 
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - el_id is bad
 *                 - element is not an input element
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_GetElementType(NW_Wml_t *thisObj, NW_Uint16 el_id, NW_Wml_ElType_e *el_type, NW_Wml_Element_t **el)
{
  NW_Int32    id = -1;
  NW_Wml_ElType_e     elTypeLocal;

  if (thisObj == NULL)
    return KBrsrBadInputParam;

  id = NW_Wml_FindEl(thisObj, el_id);
  if (id == -1)
    return KBrsrFailure;

  *el = thisObj->card_els[id];

  NW_DeckDecoder_GetType(thisObj->decoder, *el, &elTypeLocal);

  *el_type = elTypeLocal;
  return KBrsrSuccess;
}

#if 0 /* no longer used 12/5/2001 */
/* 
 * this is probably useless. the PE should just call 
 *  GetAttribute on the INPUT elems KEY attr 
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - el_id is bad
 *                 - element is not an input element
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_GetDefault(NW_Wml_t *thisObj, NW_Uint16 el_id, NW_Ucs2 **ret_string)
{
  NW_Wml_Element_t *el;
  NW_Ucs2          *key_var_name = 0;
  TBrowserStatusCode      status;
  NW_Wml_ElType_e  el_type = UNKNOWN_ELEMENT;

  if ((status = NW_Wml_GetElementType(thisObj, el_id, &el_type, &el)) != KBrsrSuccess)
    return status;

  if (el_type == INPUT_ELEMENT)
  {
    status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, NAME_ATTR, 
        thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &key_var_name);
    if (status == KBrsrOutOfMemory)
      return KBrsrOutOfMemory;

    if (key_var_name)
    {
      NW_Ucs2 *temp = 0;
      temp = NW_WmlVar_GetValue(thisObj->var_list, key_var_name);
      if (temp)
      {
        *ret_string = NW_Str_Newcpy(temp);
        if (*ret_string == 0)
          return KBrsrOutOfMemory;
      }
    }
    /* Note: NW_Str_Delete() handles null pointers */
    NW_Str_Delete(key_var_name);
  }
  else
  {
    return KBrsrFailure; /* bad request */
  }
  return KBrsrSuccess;
}
#endif

/*
 * RETURN: KBrsrSuccess
 *       KBrsrOutOfMemory
 *       KBrsrBadInputParam - Bad input parameter
 */
TBrowserStatusCode NW_Wml_GetCurrURL(NW_Wml_t *thisObj, NW_Ucs2 **ret_string)
{
  NW_ASSERT(thisObj);
  /* Caller frees memory */
  return NW_Wml_HistoryGetResponseUrl(thisObj, ret_string);
}
  
/*
 * RETURN: KBrsrSuccess
 *       KBrsrOutOfMemory
 *       KBrsrBadInputParam - Bad input parameter
 */
TBrowserStatusCode NW_Wml_GetCurrReqURL(NW_Wml_t *thisObj, NW_Ucs2 **ret_string)
{
  NW_ASSERT(thisObj);
  /* Caller frees memory */
  return NW_Wml_HistoryGetRequestUrl(thisObj, ret_string);
}
  
  
NW_Int32  NW_Wml_FindEl(NW_Wml_t *thisObj, NW_Uint16 id)
{
  NW_Uint16 i = 0;
  NW_Wml_Element_t *el;

  if ( (thisObj == NULL) || (thisObj->card_els == NULL) ) {
#if 0
    /*    NW_ASSERT(NW_FALSE); */
#endif
    return -1;
  }

  /* assume rendered_els is an array of n NW_Wml_Element_t items */
  for (i = 0;;i++)
  {
    el = thisObj->card_els[i];
    if (!el) 
      return -1;
    if (el->id == id) 
      return i;
  }

  /* NEVER executed ...
  NW_ASSERT(NW_FALSE);
  return -1;
  */
}

#if 0 /* no longer used 12/5/2001 */
/***************************************************************************
 * Name:          NW_Wml_GetPrevId
 * Description:   returns the previous_task id of the WML object
 * Parameters:    thisObj -- pointer to NW_Wml_t structure
 *                id -- pointer to id to be returned
 * Algorithm:     If either argument is NULL, the value 
 *                KBrsrBadInputParam is returned immediately.
 *                
 *                If the 'prev_task_id' field is greater than or equal to 0, 
 *                that value is placed in *id, and KBrsrSuccess is returned.
 *                
 *                Otherwise, there is either (a) no previous task id, or
 *                (b) more than one 'prev' task.  In either case the id
 *                value is set to -1, and KBrsrFailure is returned. 
 * Return value:  KBrsrSuccess
 *                KBrsrFailure
 *                KBrsrBadInputParam
 ***************************************************************************/
TBrowserStatusCode  NW_Wml_GetPrevId(NW_Wml_t *thisObj, NW_Int16 *id)
{
  NW_ASSERT(thisObj);
  NW_ASSERT(id);

  if (thisObj->prev_task_id >= 0)
  {
    *id = thisObj->prev_task_id;
    return KBrsrSuccess;
  } else {
    *id = -1;
    return KBrsrFailure;
  }
}
#endif

/***************************************************************************
 * Name:          NW_Wml_HasOnpick
 *
 * Description:   returns KBrsrSuccess if all of the following are true:
 *	                 - element is of type OPTION_ELEMENT
 *                   - element contains an onpick attribute or contains an 
 *                     onevent with an onpick type
 *                Otherwise, it returns a failure code 
 *
 * Parameters:    thisObj -- pointer to NW_Wml_t structure
 *                el_id -- element id to check for onpick status 
 *
 * Return value:  KBrsrSuccess
 *                KBrsrFailure
 *                KBrsrOutOfMemory
 ***************************************************************************/
TBrowserStatusCode NW_Wml_HasOnpick(NW_Wml_t *thisObj, NW_Uint16 el_id )
{
  NW_Wml_Element_t *el = 0;
  TBrowserStatusCode      status;
  NW_Wml_ElType_e  el_type;
	
  if ((status = NW_Wml_GetElementType(thisObj, el_id, &el_type, &el)) != KBrsrSuccess)
    return status;

  status = KBrsrFailure;
  if (el_type == OPTION_ELEMENT)
  {
    status = NW_Wml_HasOptionIntrinsicEvents(thisObj, el, NW_FALSE);
  }
  
  return status;
}


