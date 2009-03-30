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
    $Workfile: wml_input_validation.c $

    Purpose:

        Class: WmlBrowser

        Main browser's input variable validation logic
*/


#include "wml_task.h"

#include "nwx_string.h"
#include "nw_wml1x_wml_1_3_tokens.h"
#include "NW_Unicode_Category.h"
#include "NW_Unicode_CategoryI.h"
#include "NW_Unicode_LetterLowercase.h"
#include "NW_Unicode_LetterModifier.h"
#include "NW_Unicode_LetterUppercase.h"
#include "NW_Unicode_NumberDecimalDigit.h"
#include "NW_Unicode_SymbolCurrency.h"
#include "NW_Unicode_SymbolModifier.h"
#include "NW_Unicode_SymbolMath.h"
#include "NW_Unicode_SymbolOther.h"
#include "NW_Unicode_PunctuationConnector.h"
#include "NW_Unicode_SeparatorSpace.h"
#include "NW_Unicode_PunctuationDash.h"
#include "NW_Unicode_PunctuationOpen.h"
#include "NW_Unicode_PunctuationClose.h"
#include "NW_Unicode_PunctuationOther.h"
#include "NW_Unicode_LetterOther.h"
#include "NW_Unicode_LetterChina.h"
#include "NW_Unicode_LetterThai.h"

typedef enum {
  ANY_NUMBER_OF_CHARS,
  SPECIFIC_NUMBER_OF_CHARS
} FormatWildcard_t;

/* for INPUT element validation */

static NW_Bool ValidateInputWildcard(NW_Ucs2*, NW_Ucs2*, FormatWildcard_t, NW_Int32 );
static NW_Bool CheckaFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked);
static NW_Bool CheckAFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked);
static NW_Bool ChecknFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked);
static NW_Bool CheckNFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked);
static NW_Bool CheckxFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked);
static NW_Bool CheckXFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked);
static NW_Bool CheckAnyChar(NW_Ucs2* value, NW_Int32  chars_to_check);
static NW_Bool ValidateFormat(const NW_Ucs2 * format, NW_Int32 *numChars);
static NW_Bool IsFormatCode(const NW_Ucs2 ch);
static NW_Bool IsStarF_Format(NW_Ucs2 *format);

NW_Bool NW_WmlInput_ValidateValue(NW_Ucs2* input_value, 
                                  NW_Ucs2* format, 
                                  NW_WmlInput_EmptyOk_t emptyok, 
                                  NW_Bool partial_validation_ok,
                                  NW_Uint32 docPublicId)
{
  NW_Ucs2 *format_cursor = format, *value_cursor = input_value;
  NW_Bool failed_validation = NW_FALSE, done = NW_FALSE;
  NW_Int32 chars_to_check, chars_checked;
  NW_Int32 numChars = 0;

  /* handle empty input cases */
  if (!input_value || input_value[0] == 0)
  {
    if (emptyok == NW_WmlInput_EmptyOk_True) {
      /* Empty string and emptyok == true.  Return valid match. */
      return NW_TRUE;
    } else if (emptyok == NW_WmlInput_EmptyOk_False) {
      /* Empty string and emptyok == false.  Return mismatch. */
      return NW_FALSE;
    } else {
      /* emptyok not set.  Drop through and match against format mask. */
      ;
    }
  }

  if ((format == NULL) || 
      (format[0] == '\0') || 
      (!ValidateFormat(format, &numChars))) {
    /* Either no format or an illegal format.  Nothing to validate
       against.  Everything accepted. */
    return NW_TRUE;
  }

  if ((input_value == NULL) || (input_value[0] == '\0')) {
    if(IsStarF_Format(format)) 
    {
      /* No input value and a "*f" format mask */
      return NW_TRUE;
    }
    else
    {
      /* No input value and a nonempty valid format mask */
      return NW_FALSE;
    }
  }

  if (numChars != -1 && partial_validation_ok == NW_FALSE)
  {
    /* For WML 1.0 and 1.1, the format has a fixed number of characters.
     * If we're validating the entire input and the number of characters
     * are not the same, return failure.  But for WML 1.2, the
     * number of input characters can be from 1 to n (1 <=n <= 9).
     */
    
    NW_Int32 inputLength;

    inputLength = NW_Str_Strlen(input_value);
    
    if (docPublicId == NW_Wml_1_0_PublicId || docPublicId == NW_Wml_1_1_PublicId) /**{
    At WML 1.2 && 1.3, the * number of input characters can be from 1 to n (1 <=n <= 9), and don't 
    need the length of input value is as same as the number get from fomat.
      if (inputLength != numChars)
        return NW_FALSE;
    } else {**/
    if ( (inputLength < 1) || (inputLength > numChars) ) {
        return NW_FALSE;
      }
  }

  while (format_cursor && format_cursor[0] && !failed_validation && !done)
  {
    switch (format_cursor[0])
    {
      case 'A':
        chars_to_check = 1;
        if (CheckAFormat(value_cursor, chars_to_check, &chars_checked))
        {
          format_cursor++;
          value_cursor += chars_checked;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;

      case 'a':
        chars_to_check = 1;
        if (CheckaFormat(value_cursor, chars_to_check, &chars_checked))
        {
          format_cursor++;
          value_cursor += chars_checked;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;

      case 'N':
        chars_to_check = 1;
        if (CheckNFormat(value_cursor, chars_to_check, &chars_checked))
        {
          format_cursor++;
          value_cursor++;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;
 
      case 'n':
        chars_to_check = 1;
        if (ChecknFormat(value_cursor, chars_to_check, &chars_checked))
        {
          format_cursor++;
          value_cursor++;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;

      case 'X':
        chars_to_check = 1;
        if (CheckXFormat(value_cursor, chars_to_check, &chars_checked))
        {
          format_cursor++;
          value_cursor += chars_checked;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;

      case 'x':
        chars_to_check = 1;
        if (CheckxFormat(value_cursor, chars_to_check, &chars_checked))
        {
          format_cursor++;
          value_cursor++;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;

      case 'M':
      case 'm':
        chars_to_check = 1;
        if (CheckAnyChar(value_cursor, chars_to_check))
        {
          format_cursor++;
          value_cursor++;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;

      case '*':
      {
        FormatWildcard_t wildcard = ANY_NUMBER_OF_CHARS;
        format_cursor++;
        if ( value_cursor[0] == 0)
        {
          /* Not more values. OK because * is 0 or more. */
          done = NW_TRUE;
          /* Skip format character if there. */
          if ( format_cursor[0] != 0 )
            format_cursor++;
        }
        else if (ValidateInputWildcard(value_cursor, format_cursor,
                                         wildcard, 0))
        {
          done = NW_TRUE;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;
      }

      case '\\':
        {
          NW_Ucs2 save_format_char, save_value_char;
          format_cursor++;
          save_format_char = format_cursor[1];
          save_value_char = value_cursor[1];
          format_cursor[1] = 0;
          value_cursor[1] = 0;
          if (!NW_Str_Strcmp(format_cursor, value_cursor))
          {
            format_cursor++;
            value_cursor++;
            *format_cursor = save_format_char;
            *value_cursor = save_value_char;
          }
          else
          {
						/* Save the original format back to its value */
            format_cursor++;
            value_cursor++;
            *format_cursor = save_format_char;
            *value_cursor = save_value_char;
            failed_validation = NW_TRUE;
          }
          break;
        }
        
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      {
        /* NW_Ucs2 save_char = *(format_cursor + 1); */
        FormatWildcard_t wildcard = SPECIFIC_NUMBER_OF_CHARS;
        NW_Int32   num_chars;
        NW_Int32   i = 0;
        NW_Ucs2    save_char;

        while (NW_Str_Isdigit(format_cursor[i]))
        {
          i++;
        }
        
        save_char = format_cursor[i];
        format_cursor[i] = 0;
        num_chars = NW_Str_Atoi(format_cursor);

        format_cursor += i;
        *format_cursor = save_char;
        if (ValidateInputWildcard(value_cursor, format_cursor,
                                    wildcard, num_chars))
        {
          format_cursor++;
          /* Bug #
          TODO: JW buddy review this change.

          Problem : the value_cursor was moved by the number of characters
                    defined in the format.
          Fix: For example the value_cursor is "aa" format_cursor = "5a"
               num_chars = "5" the value_cursor cannot be incremented by "5",
               we dont know how big the input string is.
          */
          /*value_cursor += num_chars;*/
          NW_ASSERT(NW_Str_Strlen(value_cursor));
          /*lint -e{666} Expression with side effects passed to repeated parameter in macro*/
          value_cursor += NW_MIN(NW_UINT32_CAST(num_chars), NW_Str_Strlen(value_cursor));
        }
        else
        {
          failed_validation = NW_TRUE;
        }
        break;
      }
      
      default:
        failed_validation = NW_TRUE;
        
    }

    /* eat any wildcard characters left at end */
    if (!value_cursor[0]) {
      NW_Bool wildchar_found = NW_FALSE;
      while (format_cursor[0] == '*') {
        wildchar_found = NW_TRUE;
        format_cursor++;
      }
      /* If these is any wildchar character at this point, it should apprear
         only once and must appear at the end of the format string */
      if(wildchar_found)
      {
        if(format_cursor[0] && IsFormatCode(format_cursor[0]) && 
           !format_cursor[1])
        {
          done = NW_TRUE;
        }
        else
        {
          failed_validation = NW_TRUE;
        }
      }

      if ((!format_cursor[0]) || (NW_Str_Isdigit(format_cursor[0])))  {
        done = NW_TRUE;
      }
    }
    
    /* check for more format chars, but no more value chars */
    if (!done && format_cursor[0] && !value_cursor[0])
    {
      if (partial_validation_ok)
      {
        done = NW_TRUE;
      }
      else
      {
        failed_validation = NW_TRUE;
      }
    } 

    /* check for more value chars, but no more format chars */
    if (!done && value_cursor[0] && !format_cursor[0])
      failed_validation = NW_TRUE;
  }
  return NW_BOOL_CAST(!failed_validation);
}

/*
 * Function to validate the format attribute value of an INPUT element.
 * Note: The UI should validate the format string before trying to parse it.
 * One example where the UI needs to parse the format string is to implement
 * the functionality of preloading the input field.
 * eg:- <input name="date" format="NN\-NN\-\2\0\0\0\0">,
 *          the UI can display this as "__-__-2000"
 * RETURN:  NW_TRUE  - if the format is legal
 *                - if format is null
 *          NW_FALSE - if the format is not legal
 
 */
NW_Bool NW_WmlInput_ValidateFormat(const NW_Ucs2 * format)
{
  NW_Int32 numChars = 0;
  return ValidateFormat(format, &numChars);
}

static NW_Bool ValidateInputWildcard(NW_Ucs2* input_value,
                              NW_Ucs2* format,
                              FormatWildcard_t wildcard,
                              NW_Int32  num_of_chars)
{
  NW_Ucs2 *format_cursor = format, *value_cursor = input_value;
  NW_Bool validation_ok = NW_FALSE;
  NW_Int32 chars_to_check;
  NW_Int32 chars_checked;

  chars_to_check = (wildcard == ANY_NUMBER_OF_CHARS ?
                    NW_Str_Strlen(input_value) : num_of_chars);
  switch (format_cursor[0])
  {
    case 'A':
      if (CheckAFormat(value_cursor, chars_to_check, &chars_checked))
      {
        validation_ok = NW_TRUE;
      }
      break;


    case 'a':
      if (CheckaFormat(value_cursor, chars_to_check, &chars_checked))
      {
        validation_ok = NW_TRUE;
      }
      break;

    case 'N':
      if (CheckNFormat(value_cursor, chars_to_check, &chars_checked))
      {
        validation_ok = NW_TRUE;
      }
      break;

    case 'n':
      if (ChecknFormat(value_cursor, chars_to_check, &chars_checked))
      {
        validation_ok = NW_TRUE;
      }
      break;

    case 'X':
      if (CheckXFormat(value_cursor, chars_to_check, &chars_checked))
      {
        validation_ok = NW_TRUE;
      }
      break;

    case 'x':
      if (CheckxFormat(value_cursor, chars_to_check, &chars_checked))
      {
        validation_ok = NW_TRUE;
      }
      break;

    case 'M':
    case 'm':
      if (CheckAnyChar(value_cursor, chars_to_check))
      {
        validation_ok = NW_TRUE;
      }
      break;


    default:
      /* The format specifier is illegal - the mask is ignored */
      validation_ok = NW_TRUE;
      break;
  }

  return validation_ok;
}


/* Check that 1 to chars_to_check match the WML input "a" format. */
/*entry of any lowercase letter, symbol, or punctuation character. Numeric
characters are excluded.*/

static NW_Bool CheckaFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked)
{
  NW_Int32 i = 0;
  NW_Bool pass = NW_TRUE;
  NW_Ucs4 uniCompare;

  NW_ASSERT(chars_checked);

  *chars_checked = 0;

  if ( (chars_to_check <= 0) ||
       (value == NULL) ||
       (value[0] == 0 ) )
  {
    return NW_FALSE;
  }

  do
  {
    uniCompare = (NW_Ucs4)value[i];
    if ((!(NW_Unicode_IsLetterLowercase(uniCompare))) && 
       (!(NW_Unicode_IsLetterModifier(uniCompare))) &&
       (!(NW_Unicode_IsLetterOther(uniCompare))) &&
       (!(NW_Unicode_IsSymbolCurrency(uniCompare))) &&
       (!(NW_Unicode_IsSymbolMath(uniCompare))) &&
       (!(NW_Unicode_IsSymbolModifier(uniCompare))) &&
       (!(NW_Unicode_IsSymbolOther(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationConnector(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationDash(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOpen(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationClose(uniCompare))) &&
       (!(NW_Unicode_IsSeparatorSpace(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOther(uniCompare))) &&
       (!(NW_Unicode_IsLetterChina(uniCompare))) && 
       (!(NW_Unicode_IsLetterThai(uniCompare)))) 
    {
      pass = NW_FALSE;
    }
    i++;
  } 
  while ((pass == NW_TRUE) && (i < chars_to_check) && value[i]); 
  
  *chars_checked = i;
  return pass;
}


/* Check that 1 to chars_to_check match the WML input "A" format. */
/*entry of any uppercase letter, symbol, or punctuation character. Numeric
characters are excluded**/

static NW_Bool CheckAFormat(NW_Ucs2* value, NW_Int32 chars_to_check, NW_Int32* chars_checked)
{
  NW_Int32 i = 0;
  NW_Bool pass = NW_TRUE;
  NW_Ucs4 uniCompare;
  
  NW_ASSERT(chars_checked);

  *chars_checked = 0;

  if ( (chars_to_check <= 0) ||
       (value == NULL) ||
       (value[0] == 0 ) )
  {
    return NW_FALSE;
  }

  do
  {
    uniCompare = (NW_Ucs4)value[i];
    if ((!(NW_Unicode_IsLetterUppercase(uniCompare))) && 
       (!(NW_Unicode_IsLetterModifier(uniCompare))) &&
       (!(NW_Unicode_IsLetterOther(uniCompare))) &&
       (!(NW_Unicode_IsSymbolCurrency(uniCompare))) &&
       (!(NW_Unicode_IsSymbolMath(uniCompare))) &&
       (!(NW_Unicode_IsSymbolModifier(uniCompare))) &&
       (!(NW_Unicode_IsSymbolOther(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationConnector(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationDash(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOpen(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationClose(uniCompare))) &&
       (!(NW_Unicode_IsSeparatorSpace(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOther(uniCompare))) &&
       (!(NW_Unicode_IsLetterChina(uniCompare))) && 
       (!(NW_Unicode_IsLetterThai(uniCompare)))) 
    {
      pass = NW_FALSE;
    }
    i++;
  } 
  while ((pass == NW_TRUE) && (i < chars_to_check) && value[i]); 

  *chars_checked = i;
  return pass;
}

/* Check that 1 to chars_to_check match the WML input "x" format. */
/* entry of any lowercase letter, numeric character, symbol, or punctuation
character
*/
static NW_Bool CheckxFormat(NW_Ucs2* value, NW_Int32 chars_to_check,
                            NW_Int32* chars_checked)
{
  NW_Int32 i = 0;
  NW_Bool pass = NW_TRUE;
  NW_Ucs4 uniCompare;
  
  NW_ASSERT(chars_checked);

  *chars_checked = 0;

  if ( (chars_to_check <= 0) ||
       (value == NULL) ||
       (value[0] == 0 ) )
  {
    return NW_FALSE;
  }

  do
  {
   uniCompare = (NW_Ucs4)value[i];
   if ((!(NW_Unicode_IsLetterLowercase(uniCompare))) && 
      (!(NW_Unicode_IsLetterOther(uniCompare))) &&
      (!(NW_Unicode_IsLetterModifier(uniCompare))) &&
      (!(NW_Unicode_IsSymbolCurrency(uniCompare))) &&
      (!(NW_Unicode_IsSymbolMath(uniCompare))) &&
      (!(NW_Unicode_IsSymbolModifier(uniCompare))) &&
      (!(NW_Unicode_IsSymbolOther(uniCompare))) &&
      (!(NW_Unicode_IsPunctuationConnector(uniCompare))) &&
      (!(NW_Unicode_IsPunctuationDash(uniCompare))) &&
      (!(NW_Unicode_IsPunctuationOpen(uniCompare))) &&
      (!(NW_Unicode_IsPunctuationClose(uniCompare))) &&
      (!(NW_Unicode_IsSeparatorSpace(uniCompare))) &&
      (!(NW_Unicode_IsPunctuationOther(uniCompare))) &&
      (!(NW_Unicode_IsNumberDecimalDigit(uniCompare))) &&
       (!(NW_Unicode_IsLetterChina(uniCompare))) && 
       (!(NW_Unicode_IsLetterThai(uniCompare)))) 
    {
      pass = NW_FALSE;
    }
    i++;
  } 
  while ((pass == NW_TRUE) && (i < chars_to_check) && value[i]); 

  *chars_checked = i;
  return pass;
}

/* Check that 1 to chars_to_check match the WML input "X" format. */
/* entry of any uppercase letter, numeric character, symbol, or punctuation
character*/

static NW_Bool CheckXFormat(NW_Ucs2* value, NW_Int32 chars_to_check,
                            NW_Int32* chars_checked)
{
  NW_Int32 i = 0;
  NW_Bool pass = NW_TRUE;
  NW_Ucs4 uniCompare;
  

  NW_ASSERT(chars_checked);

  *chars_checked = 0;

  if ( (chars_to_check <= 0) ||
       (value == NULL) ||
       (value[0] == 0 ) )
  {
    return NW_FALSE;
  }

  do
  {
    uniCompare = (NW_Ucs4)value[i];
    if ((!(NW_Unicode_IsLetterUppercase(uniCompare))) && 
       (!(NW_Unicode_IsLetterOther(uniCompare))) &&
       (!(NW_Unicode_IsLetterModifier(uniCompare))) &&
       (!(NW_Unicode_IsSymbolCurrency(uniCompare))) &&
       (!(NW_Unicode_IsSymbolMath(uniCompare))) &&
       (!(NW_Unicode_IsSymbolModifier(uniCompare))) &&
       (!(NW_Unicode_IsSymbolOther(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationConnector(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationDash(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOpen(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationClose(uniCompare))) &&
       (!(NW_Unicode_IsSeparatorSpace(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOther(uniCompare))) &&
       (!(NW_Unicode_IsNumberDecimalDigit(uniCompare))) &&
       (!(NW_Unicode_IsLetterChina(uniCompare))) && 
       (!(NW_Unicode_IsLetterThai(uniCompare)))) 
    {
      pass = NW_FALSE;
    }
    i++;
  } 
  while ((pass == NW_TRUE) && (i < chars_to_check) && value[i]); 

  *chars_checked = i;
  return pass;
}


/* Check that 1 to chars_to_check match the WML input "n" format. */
/* entry of any numeric, symbol, or punctuation character.*/

static NW_Bool ChecknFormat(NW_Ucs2* value, NW_Int32 chars_to_check,
                            NW_Int32* chars_checked)
{
  NW_Int32 i = 0;
  NW_Bool pass = NW_TRUE;
  NW_Ucs4 uniCompare;
  
  NW_ASSERT(chars_checked);

  *chars_checked = 0;

  if ( (chars_to_check <= 0) ||
       (value == NULL) ||
       (value[0] == 0 ) )
  {
    return NW_FALSE;
  }

  do
  {
    uniCompare = (NW_Ucs4)value[i];
    if((!(NW_Unicode_IsSymbolCurrency(uniCompare))) &&
       (!(NW_Unicode_IsSymbolMath(uniCompare))) &&
       (!(NW_Unicode_IsSymbolModifier(uniCompare))) &&
       (!(NW_Unicode_IsSymbolOther(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationConnector(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationDash(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOpen(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationClose(uniCompare))) &&
       (!(NW_Unicode_IsPunctuationOther(uniCompare))) &&
       (!(NW_Unicode_IsNumberDecimalDigit(uniCompare)))) {
        pass = NW_FALSE;
      }
    i++;
  } 
  while ((pass == NW_TRUE) && (i < chars_to_check) && value[i]); 

  *chars_checked = i;
  return pass;
}
/*entry of any numeric character.*/
static NW_Bool CheckNFormat(NW_Ucs2* value, NW_Int32 chars_to_check,
                            NW_Int32* chars_checked)
{
  NW_Int32 i = 0;
  NW_Bool pass = NW_TRUE;

  NW_ASSERT(chars_checked);

  *chars_checked = 0;

  if ( (chars_to_check <= 0) ||
       (value == NULL) ||
       (value[0] == 0 ) )
  {
    return NW_FALSE;
  }

  do
  {
    if (!NW_Unicode_IsNumberDecimalDigit((NW_Ucs4)value[i])) {
      pass = NW_FALSE;
    }
    i++;
  } 
  while (pass && (i < chars_to_check) && value[i]); 

  *chars_checked = i;
  return pass;
}

static NW_Bool CheckAnyChar(NW_Ucs2* value, NW_Int32  chars_to_check)
{
  NW_Int32  i = 0;
  NW_Bool pass = NW_TRUE;

  NW_ASSERT(value);
  
  do
  {
    if (NW_Str_Iscntrl(value[i]))
    {
      pass = NW_FALSE;
    }
    i++;
  }
  while (pass && (i < chars_to_check) && value[i]);

  return pass;
}

/*
 * Given a format string, determine if it is legal.
 *
 * RETURN:  NW_TRUE  - if the format is legal
 *                    - if format is null
 *              NW_FALSE - if the format is not legal
 * 
 * MODIFIED: numChars is set to the number of characters that
 * are allowed in the format or set to -1 if any number of
 * chars are allowed.
 */
static NW_Bool ValidateFormat(const NW_Ucs2 * format, NW_Int32 *numChars)
{
  NW_Bool expectChar = NW_FALSE;
  const NW_Ucs2 * pch;

  NW_ASSERT(numChars != NULL);
  *numChars = 0;

  /* A null or empty format string is legal */
  if ((format == NULL) || (format[0] == 0))
    return NW_TRUE;

  for (pch = format; (*pch) != 0; pch++) 
  {

    switch (*pch)
    {
      case 'A':
      case 'a':
      case 'N':
      case 'n':
      case 'X':
      case 'x':
      case 'M':
      case 'm':
        if (*numChars >= 0)
          (*numChars)++;
        if (expectChar)
        {
          /* 
           * The previous format char was a number or wildcard 
           * thus no other characters are allowed in a legal
           * format string
           */
           if (pch[1] == 0)
            return NW_TRUE;
           else
            return NW_FALSE;
        }
        expectChar = NW_FALSE; 
        break;

      case '\\':
        if (*numChars == -1 || expectChar)
          /* 
           * Was expecting a format char from above but
           * did not get one so the format is not legal
           */
          return NW_FALSE;

        /* Skip this character and the escape */
        (*numChars)+=2;
        pch++;
          
        if ( *pch == 0 )
          return NW_FALSE;  /* Must have another char after "\". */

        break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (*numChars == -1 || expectChar)
          /* Illegal format */
          return NW_FALSE;

        /* 
         * Add the ordinal value of this specifier but assume that
         * one will added for the specifier.
         */
        {
          NW_Ucs2 tmp[2];
          tmp[0] = *pch;
          tmp[1] = 0; 
          *numChars = (*numChars) + NW_Str_Atoi(tmp) - 1;
        }

        expectChar = NW_TRUE; 
        break;

      case '*':
        if (*numChars == -1 || expectChar)
          /* Illegal format */
          return NW_FALSE;

        *numChars = -1;
        expectChar = NW_TRUE; 
        break;

      default:
        /* Illegal format character */
        return NW_FALSE;
    }
  }

  if (expectChar)
    return NW_FALSE;
  else
    return NW_TRUE;
}

static NW_Bool IsFormatCode(const NW_Ucs2 ch)
{
  if((ch == 'A') || (ch == 'a') || (ch == 'M') || (ch == 'm') ||
     (ch == 'X') || (ch == 'x') || (ch == 'N') || (ch == 'n'))
  {
    return NW_TRUE;
  }
  else
  {
    return NW_FALSE;
  }
}

/*****************************************************************

  Name:         IsStarF_Format()

  Description: Checks for "*f" format of WML format attribute

  Parameters: format - In, value of format attribute.

  Return Value: TRUE/FALSE
								TRUE if the format is of "*f" type.

*****************************************************************/
static NW_Bool IsStarF_Format(NW_Ucs2 *format)
{
	if(format[0] != '*')
	{
		return NW_FALSE;
	}
	format++;
	if(((format[0] == 'A') || (format[0] == 'a') || 
		  (format[0] == 'N') || (format[0] == 'n') || 
		  (format[0] == 'X') || (format[0] == 'x') || 
		  (format[0] == 'M') || (format[0] == 'm')) &&
		 (format[1] == 0))
	{
		return NW_TRUE;
	}
	return NW_FALSE;
}
