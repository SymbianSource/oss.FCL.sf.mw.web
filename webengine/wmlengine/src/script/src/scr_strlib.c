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
    $Workfile: scr_strlib.c $

    Purpose:

        This file implements the ScriptServer functionality for the String standard 
        library. The String standard library contains a set of string functions.
        A String is an array of UNICODE characters.  Each of the characters has 
        an index.  The first character in a string has an index of 0.  The length
        of the string is the number of characters in the array.

 */

#include "scr_strlib.h"
#include "scr_api.h"
#include "scr_srv.h"
#include "scr_estack.h"
#include "scr_conv.h"
#include "scr_core.h"
#include "nwx_string.h"
#include "nwx_math.h"


static NW_Bool GetValAsIntAndFree (val_t *vptr, NW_Int32 *iptr)
{
  NW_Bool result;
  NW_Float32 fval;

  result = val2int(*vptr, iptr);
  if (!result) {
    result = val2float(*vptr, &fval);
    if (result) {
      *iptr = NW_Math_truncate(fval);
    }
  }

  free_val(*vptr);
  return result;
}



NW_Byte size_string_lib(void)
{
  return NO_OF_STRING_FUNCS;
}



static NW_Bool string_length(void)
{
  val_t v1;
  val_t res;
  NW_Bool result;
  str_t str = NULL;

  v1 = pop_estack();
  result = val2str_and_free(&v1, &str);

  if (result) {
    res = new_int_val(str_len(str));
  } else {
    res = new_invalid_val();
  }

  free_str(str);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_isEmpty(void)
{
  val_t v1;
  val_t res;
  NW_Bool result;
  str_t str = NULL;

  v1 = pop_estack();
  result = val2str_and_free(&v1, &str);

  if (result) {
    res = new_bool_val(NW_BOOL_CAST(str_len(str) == 0));
  } else {
    res = new_invalid_val();
  }

  free_str(str);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_charAt(void)
{
  val_t v1, v2;
  val_t res;
  NW_Int32  index;
  NW_Bool result1, result2;
  str_t res_str = NULL;
  str_t str = NULL;

  v1 = pop_estack();
  v2 = pop_estack();

  result1 = GetValAsIntAndFree(&v1, &index);
  result2 = val2str_and_free(&v2, &str);

  if (result1 && result2) {
    if ( (index < 0) || (index >= str_len(str)) ) {
      res_str = new_str(0, NULL);
    } else {
      res_str = scr_substr(str, index, 1);
    }
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_subString(void)
{
  val_t v1, v2, v3;
  val_t res;
  NW_Bool result1, result2, result3;
  NW_Int32  istart, ilen;
  str_t str1 = NULL;
  str_t res_str = NULL;

  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = GetValAsIntAndFree(&v2, &istart);
  result3 = GetValAsIntAndFree(&v3, &ilen);

  if (result1 && result2 && result3) {
    res_str = scr_substr(str1, istart, ilen);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_find(void)
{
  val_t v1, v2;
  val_t res;
  NW_Bool result1, result2;
  str_t str1 = NULL;
  str_t str2 = NULL;

  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = val2str_and_free(&v2, &str2);

  /* if len(result2)==0 we must return INVALID as per [WAP194] 9.5 */
  if (result1 && result2 && str_len(str2)!=0 ) {
    res = new_int_val(str_find(str1, str2, 0));
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_replace(void)
{
  val_t v1, v2, v3;
  val_t res;
  NW_Bool result1, result2, result3;
  str_t str1 = NULL;
  str_t str2 = NULL;
  str_t str3 = NULL;
  str_t res_str = NULL;

  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = val2str_and_free(&v2, &str2);
  result3 = val2str_and_free(&v3, &str3);

  if (result1 && result2 && result3)
  {
    res_str = str_replace(str1, str2, str3);
    res = new_str_val_and_free(&res_str);
  } else {
    res= new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  free_str(str3);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_elements(void)
{
  val_t v1, v2;
  val_t res;
  NW_Bool result1, result2;
  str_t str1 = NULL;
  str_t str2 = NULL;
  NW_Ucs2 wc;

  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = val2str_and_free(&v2, &str2);

  if (result1 && result2 && (str_len(str2) > 0))
  {
    wc = scr_charat(str2, 0);
    if (str_is_empty(str1)) {
      res = new_int_val(1);
    } else {
      res = new_int_val(str_times(str1, wc) + 1);
    }
  } else {
    res= new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_elementAt(void)
{
  val_t v1, v2, v3;
  val_t res;
  NW_Int32  index;
  str_t str1 = NULL;
  str_t str2 = NULL;
  str_t res_str = NULL;
  NW_Bool result1, result2, result3;
  NW_Ucs2 separator;

  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = GetValAsIntAndFree(&v2, &index);
  result3 = val2str_and_free(&v3, &str2);

  if (result1 && result2 && result3 && (str_len(str2) > 0)) {
    separator = scr_charat(str2, 0);
    res_str = str_elementat(str1, index, separator);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_removeAt(void)
{
  val_t v1, v2, v3;
  val_t res;
  NW_Int32  index;
  str_t str1 = NULL;
  str_t str2 = NULL;
  str_t res_str = NULL;
  NW_Bool result1, result2, result3;
  NW_Ucs2 separator;

  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = GetValAsIntAndFree(&v2, &index);
  result3 = val2str_and_free(&v3, &str2);

  if (result1 && result2 && result3 && (str_len(str2) > 0)) {
    separator = scr_charat(str2, 0);
    res_str = str_removeat(str1, index, separator);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_replaceAt(void)
{
  val_t v1, v2, v3, v4;
  val_t res;
  NW_Int32  index;
  str_t str1 = NULL;
  str_t str2 = NULL;
  str_t str3 = NULL;
  str_t res_str = NULL;
  NW_Bool result1, result2, result3, result4;
  NW_Ucs2 separator;

  v4 = pop_estack();
  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = val2str_and_free(&v2, &str2);
  result3 = GetValAsIntAndFree(&v3, &index);
  result4 = val2str_and_free(&v4, &str3);


  if (result1 && result2 && result3 && result4 && (str_len(str3) > 0)) {
    separator = scr_charat(str3, 0);
    res_str = str_replaceat(str1, str2, index, separator);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  free_str(str3);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_insertAt(void)
{
  val_t v1, v2, v3, v4;
  val_t res;
  NW_Int32  index;
  str_t str1 = NULL;
  str_t str2 = NULL;
  str_t str3 = NULL;
  str_t res_str = NULL;
  NW_Bool result1, result2, result3, result4;
  NW_Ucs2 separator;

  v4 = pop_estack();
  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = val2str_and_free(&v2, &str2);
  result3 = GetValAsIntAndFree(&v3, &index);
  result4 = val2str_and_free(&v4, &str3);


  if (result1 && result2 && result3 && result4 && (str_len(str3) > 0)) {
    separator = scr_charat(str3, 0);
    res_str = str_insertat(str1, str2, index, separator);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  free_str(str3);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_squeeze(void)
{
  val_t v1;
  val_t res;
  str_t str1 = NULL;
  str_t res_str = NULL;
  NW_Bool result1;

  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);

  if (result1)   {
    res_str = str_squeeze(str1);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_trim(void)
{
  val_t v1;
  val_t res;
  str_t str1 = NULL;
  str_t res_str = NULL;
  NW_Bool result1;

  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);

  if (result1)   {
    res_str = str_trim(str1);
    res = new_str_val_and_free(&res_str);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_compare(void)
{
  val_t v1, v2;
  val_t res;
  str_t str1 = NULL;
  str_t str2 = NULL;
  NW_Bool result1, result2;
  NW_Int32 len1, len2;
  NW_Int32 i, limit, value;

  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);
  result2 = val2str_and_free(&v2, &str2);

  if (result1 && result2) {
    len1 = str_len(str1);
    len2 = str_len(str2);
    limit = NW_MIN(len1, len2);
    value = len1 - len2;

    for(i = 0; i < limit; i++)
    {
      NW_Ucs2 c1 = scr_charat(str1, i);
      NW_Ucs2 c2 = scr_charat(str2, i);

      if (c1 != c2) {
        value = c1 - c2;
        break;
      }
    }

    if (value != 0) {
      value = (value > 0) ? 1:-1;
    }
    res = new_int_val(value);
  } else {
    res = new_invalid_val();
  }

  free_str(str1);
  free_str(str2);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_toString(void)
{
  val_t v1;
  val_t res;
  str_t str1 = NULL;
  NW_Bool result1;

  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);

  if (result1) {
    res = new_str_val_and_free(&str1);
  } else if (ScriptVarType(v1) == VAL_TYPE_INVALID) {
    NW_Ucs2 invalid_str[8] = { 'i', 'n', 'v', 'a', 'l', 'i', 'd', 0 };
    str_t s = ucs22str(invalid_str);
    res = new_str_val_and_free(&s);
  } else {
    return NW_FALSE;
  }

  free_str(str1);
  push_estack_and_free(&res);
  return NW_TRUE;
}



static NW_Bool string_format(void)
{
  val_t v1, v2;
  val_t res;
  str_t str1 = NULL;
  str_t res_str = NULL;
  NW_Bool result1;
  NW_Bool retval = NW_FALSE;

  v2 = pop_estack();
  v1 = pop_estack();

  result1 = val2str_and_free(&v1, &str1);

  if (result1) {
    NW_Ucs2 *f_str = str2ucs2(str1);
    if (f_str) {
      res_str = scr_format(f_str, v2, &retval);
      if (retval) {
        res = new_str_val_and_free(&res_str);
      }
      NW_Str_Delete(f_str);
    }
  }
  free_val(v2); 

  if (!retval) {
    res = new_invalid_val();
  }

  free_str(str1); 
  push_estack_and_free(&res);
  return NW_TRUE;
}



/*  functions in String library */
static const lib_function_t string_lib_table[NO_OF_STRING_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          string_length,            NW_FALSE },
  {   1,          string_isEmpty,           NW_FALSE },
  {   2,          string_charAt,            NW_FALSE },
  {   3,          string_subString,         NW_FALSE },
  {   2,          string_find,              NW_FALSE },
  {   3,          string_replace,           NW_FALSE },
  {   2,          string_elements,          NW_FALSE },
  {   3,          string_elementAt,         NW_FALSE },
  {   3,          string_removeAt,          NW_FALSE },
  {   4,          string_replaceAt,         NW_FALSE },
  {   2,          string_insertAt,          NW_FALSE },
  {   1,          string_squeeze,           NW_FALSE },
  {   1,          string_trim,              NW_FALSE },
  {   2,          string_compare,           NW_FALSE },
  {   1,          string_toString,          NW_FALSE },
  {   2,          string_format,            NW_FALSE }
};

void populate_string_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(STRING_LIB_ID);
  (*table)[i] = string_lib_table;

  return;
}
