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
    $Workfile: wml_event.c $

    Purpose:

        Class: WmlBrowser

        Browser's event handling implementation
   
   $Event: wml_event.c $

*/

#include "wml_task.h"
#include "wml_elm_attr.h"

#include "nwx_memseg.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"


/*
 * Returns the return value from other CoreBrowser calls
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure 
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_GUIEvent(NW_Wml_t* thisObj, NW_Uint16 id, void *value)
{
  NW_Wml_Element_t *el;
  NW_Int32  i = -1;
  NW_Wml_ElType_e el_type;
  TBrowserStatusCode status;
    
  if (thisObj == NULL)
    return KBrsrBadInputParam;

  if ((i = NW_Wml_FindEl(thisObj, id)) == -1)
    return KBrsrFailure;

  el = thisObj->card_els[i];

  NW_DeckDecoder_GetType(thisObj->decoder, el, &el_type);

  status = KBrsrFailure;

  if (el_type == DO_ELEMENT || el_type == A_ELEMENT || el_type == ANCHOR_ELEMENT)
  {
    status = NW_Wml_HandleTaskEvent(thisObj, el);
  }
  else if (el_type == INPUT_ELEMENT)
  {
    status = NW_Wml_HandleInputEvent(thisObj, el, (NW_Ucs2*) value, NW_TRUE, NULL);
  }
  else if(el_type == OPTION_ELEMENT)
  {
    status = NW_Wml_HandleOptionEvent(thisObj, el);
  } 
  return status;
}


/* 
 * handle user input to the INPUT element 
 *
 * setFlag - if NW_TRUE set/unset the variable.  If NW_FALSE, don't change the variable.
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - attribute not found
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_Wml_HandleInputEvent(NW_Wml_t          *thisObj,
                                    NW_Wml_Element_t  *el,
                                    NW_Ucs2           *value, 
                                    NW_Bool           setFlag,
                                    NW_Bool           *validateOkPtr)
{
  NW_Ucs2*              var_name     = NULL;
  NW_Ucs2*              format       = NULL;
  NW_Ucs2*              emptyok_str  = NULL;
  NW_Ucs2*              maxLengthStr = NULL;
  NW_WmlInput_EmptyOk_t emptyok               = NW_WmlInput_EmptyOk_None;
  NW_Bool               partial_validation_ok = NW_FALSE;
  NW_Bool               validateOk            = NW_FALSE;
  TBrowserStatusCode           status;
  NW_Uint32             publicId;

  status = NW_DeckDecoder_GetDocumentPublicId(thisObj->decoder,&publicId);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* get NAME variable name */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, NAME_ATTR, 
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &var_name)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* get FORMAT attr value */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, FORMAT_ATTR, 
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &format)) == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* get EMPTYOK attr value */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, EMPTYOK_ATTR, 
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &emptyok_str)) 
      == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* get MAXLENGTH attr value */
  if ((status = NW_DeckDecoder_GetAttribute(thisObj->decoder, el, MAXLENGTH_ATTR, 
      thisObj->var_list, NW_MEM_SEGMENT_MANUAL, &maxLengthStr)) 
      == KBrsrOutOfMemory)
    NW_THROW_ERROR();

  /* maxLengthStr set to NULL if attr not found */
  if (maxLengthStr != NULL) {
				NW_Int32 maxLength = -1; /* Initialize to invalid length */
				if(NW_Str_StrIsValidLength(maxLengthStr))
				{
					maxLength = NW_Str_Atoi(maxLengthStr);
				}
		if(maxLength < 0)
		{
      status = KBrsrFailure;
		}
    else if (NW_Str_Strlen(value) > NW_UINT32_CAST(maxLength))
		{
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }
  }

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
  /* 
   * set variable to 0 if it doesn't conform to INPUT FORMAT 
   *  
   * this routine is called when input is complete, hence 
   * partial_validation_ok is NW_FALSE 
   */
  partial_validation_ok = NW_FALSE;

  validateOk = NW_WmlInput_ValidateValue(value, 
                                        format, 
                                        emptyok,
                                        partial_validation_ok,
                                        publicId);

  if (validateOkPtr != NULL)
  {
    *validateOkPtr = validateOk;
  }

  if ( format && 
       !validateOk )
  {
    if(setFlag)
    {
      NW_Wml_UnsetVar(thisObj, var_name);
    }
    else
    {
      status = KBrsrFailure;
      NW_THROW_ERROR();
    }
  }
  else
  {
    if ( setFlag )
    {
      status = NW_Wml_SetVar(thisObj, var_name, value);
      if (status != KBrsrSuccess && status != KBrsrFailure)
      {
       NW_THROW_ERROR();
      }
    }
  }

  status = KBrsrSuccess;
  /* fall through */

NW_CATCH_ERROR
  NW_Str_Delete(var_name);
  NW_Str_Delete(format);
  NW_Str_Delete(emptyok_str);
  NW_Str_Delete(maxLengthStr);

  return status;
}
