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
    $Workfile: scr_str.c $

     Purpose:
 */

#include "scr_str.h"
#include "scr_conv.h"
#include "scr_core.h"

#include "nwx_mem.h"
#include "nwx_string.h"

/*
** Creates a new empty string. 
** The caller has to delete the returned string with free_str
*/
static str_t new_empty_str(void)
{
  return new_str(0, NULL);
}


/* Create a new length-counted ucs2 string */
str_t new_str(NW_Int32 len, NW_Ucs2 *buf)
{
  str_t res;

  /* The specification for WMLS allows strings of 2^32 characters
   * in length; however the Rainbow architecture puts the length
   * into the first 16 bits of the string. For now, return a
   * fatal error if someone tries to use an absurdly long string.
   */
  if (len > NW_UINT16_MAX)
  {
    set_error_code(SCR_ERROR_FATAL_LIBRARY_FUNCTION_ERROR);
    return NULL;
  }

  res = (str_t) NW_Mem_Malloc((1 + len)*sizeof(NW_Ucs2));
  if (res == NULL)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NULL;
  }

  /* Length is held in the 1st 16-bit char */
  res[0] = NW_UINT16_CAST(len);

  if (len > 0) {
    /* String storage starts with 2nd 16-bit char */
    if (buf != NULL) {
      NW_Mem_memcpy(&res[1], buf, len*sizeof(NW_Ucs2));
    } else {
      NW_Mem_memset(&res[1], 0, len*sizeof(NW_Ucs2));
    }
  }

  return res;
}


/* Free a length-counted ucs2 string */
void free_str(str_t str)
{
  if (str != NULL) {
    NW_Mem_Free(str);
  }
  return;
}


/* Return length of length-counted ucs2 string */
NW_Int32  str_len(str_t str)
{
  NW_ASSERT(str != NULL);

  /* length is held in the first 16-bit char */
  return str[0];
}


/* Return pointer to storage for length-counted ucs2 string */
NW_Ucs2* str_buf_ptr(str_t str)
{
  NW_ASSERT(str != NULL);

  /* String storage starts with 2nd 16-bit char */
  return &str[1];
}


/* Concatinate 2 length-counted ucs2 strings */
str_t str_cat(str_t str1, str_t str2)
{
  str_t res;
  NW_Int32  len1;
  NW_Int32  len2;

  NW_ASSERT(str1);
  NW_ASSERT(str2);

  len1 = str_len(str1);
  len2 = str_len(str2);

  res = new_str((len1 + len2), NULL);
  if (res != NULL) {
    if (len1 > 0) {
      /* String storage starts with 2nd 16-bit char */
      NW_Mem_memcpy(&res[1], &str1[1], len1*sizeof(NW_Ucs2));
    }
    if (len2 > 0) {
      NW_Mem_memcpy(&res[1 + len1], &str2[1], len2*sizeof(NW_Ucs2));
    }
  }

  return res;
}



/* Concatinate 3 length-counted ucs2 strings */
str_t str_cat3(str_t str1, str_t str2, str_t str3)
{
  str_t res;
  NW_Int32  len1, len2, len3;

  if (str1 == NULL || str2 == NULL || str3 == NULL) {
    return NULL;
  }

  len1 = str_len(str1);
  len2 = str_len(str2);
  len3 = str_len(str3);

  res = new_str((len1 + len2 + len3), NULL);
  if (res != NULL) {
    if (len1 > 0) {
      /* String storage starts with 2nd 16-bit char */
      NW_Mem_memcpy(&res[1], &str1[1], len1*sizeof(NW_Ucs2));
    }
    if (len2 > 0) {
      NW_Mem_memcpy(&res[1 + len1], &str2[1], len2*sizeof(NW_Ucs2));
    }
    if (len3 > 0) {
      NW_Mem_memcpy(&res[1 + len1 + len2], &str3[1], len3*sizeof(NW_Ucs2));
    }
  }

  return res;
}


/* Duplicate a length-counted ucs2 string */
str_t dup_str(str_t str)
{
  if (str == NULL) {
    return NULL;
  }
  return new_str(str_len(str), str_buf_ptr(str));
}


/* Create a length-counted ucs2 string from a null-terminated ucs2 string */
str_t ucs22str(NW_Ucs2 *str)
{
  NW_Int32 len;

  if (str == NULL) {
    return new_empty_str();
  }

  len = NW_Str_Strlen(str);
  return new_str(len, str);
}


/* Compare 2 length-counted ucs2 strings */
NW_Int32  str_cmp(str_t str1, str_t str2)
{
  NW_Int32 len, i, len1, len2;
  NW_Int32 value;
  NW_Ucs2 *ptr1, *ptr2;

  if (!str1 || !str2)
  {
    if (str1 > str2) {
      return 1;
    } else if (str2 > str1) {
      return -1;
    } else {
      return 0;
    }
  }

  len1 = str_len(str1);
  len2 = str_len(str2);

  len   = NW_MIN(len1, len2);
  value = len1 - len2;

  ptr1 = str_buf_ptr(str1);
  ptr2 = str_buf_ptr(str2);

  for (i = 0; i < len; i++){
    if (ptr1[i] != ptr2[i]) {
      value = ptr1[i] - ptr2[i];
      break;
    }
  }

  if (value != 0) {
    value = (value > 0) ? 1:-1;
  }
  return value;
}



/* Return the character at the given index */
NW_Ucs2 scr_charat(str_t str, NW_Int32  index)
{
  NW_Ucs2 *ptr;
  NW_Ucs2 ch = 0;
  NW_Int32  len;

  NW_ASSERT(str != NULL);

  len = str_len(str);

  if ((0 <= index) && (index < len)) {
    ptr = str_buf_ptr(str);
    ch = ptr[index];
  } else {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
  }
  return ch;
}


/* Extract a substring from a length-counted ucs2 string */
str_t scr_substr(str_t str, NW_Int32  start, NW_Int32  len)
{
  NW_Int32  slen;
  NW_Ucs2 *ptr;

  if ((str == NULL) || (len <= 0)) {
    return new_empty_str();
  }

  slen = str_len(str);

  start = NW_MAX(start, 0);
  slen  = NW_MIN(len, slen  - start);

  if (slen <= 0) {
    return new_empty_str();
  }

  ptr = str_buf_ptr(str);
  return new_str(slen,  &ptr[start]);
}


/* Find position of 2nd string in 1st string */
NW_Int32  str_find(str_t str1, str_t str2, NW_Int32  start)
{
  NW_Int32  len1, len2, i, j;
  NW_Ucs2 *ptr1, *ptr2;

  if (!str1 || !str2) {
    return -1;
  }

  len1 = str_len(str1);
  len2 = str_len(str2);

  ptr1 = str_buf_ptr(str1);
  ptr2 = str_buf_ptr(str2);
   
  for (i = start; i <= len1 - len2; i++)
  {
    for (j = 0; j < len2; j++)
    {
      if (ptr1[i + j] != ptr2[j]) break;
    }
    if (j == len2) return i;
  }

  return -1;
}



str_t str_replace(str_t str, str_t old_str, str_t newstr)
{
  NW_Int32  start = 0, pos;
  str_t res = 0;
  NW_Int32  lold, lnew;

  if (!old_str || !newstr || !str)
  {
    return 0;
  }

  lold  = str_len(old_str);
  lnew  = str_len(newstr);
  res   = dup_str(str);

  /** returns invalid, if the old string is an empty string **/
  if ( lold == 0) {
    free_str(res);
    return NULL;
  }

  if (res == NULL) {
    return NULL;
  }

  for(; ;)
  {
    pos = str_find(res, old_str, start);

    if (pos == -1) {
      break;
    }
    
    {
      str_t sub1;
      str_t sub2;
      NW_Int32  len = str_len(res);

      sub1 = scr_substr(res, 0, pos);
      sub2 = scr_substr(res, pos + lold, len - pos - lold);
      free_str(res);
      res = str_cat3(sub1, newstr, sub2);
      free_str(sub1);
      free_str(sub2);
      start = pos + lnew;
    }
  }

  return res;
}



NW_Int32  str_times(str_t str, NW_Ucs2 separator)
{
  NW_Int32 len;
  NW_Ucs2 *ptr;
  NW_Int32 i, count = 0;

  NW_ASSERT(str);
  
  len = str_len(str);
  ptr = str_buf_ptr(str);

  for(i = 0; i < len; i++) {
    if (ptr[i] == separator) {
      count++;
    }
  }

  return count;
}



str_t str_elementat(str_t s, NW_Int32  index, NW_Ucs2 separator)
{
  NW_Int32  start = 0, i = 0;
  NW_Int32 len;
  NW_Ucs2 *ptr;

  NW_ASSERT(s);
  
  len = str_len(s);
  ptr = str_buf_ptr(s);

  for (i = 0; (i < len) && (index > 0); i++)
  {
    if (ptr[i] == separator)
    {
      --index;
      start = i + 1;
    }
  }

  for (i = start; i < len; i++)
  {
    if (ptr[i] == separator) break;
  }

  return scr_substr(s, start, i - start);
}



str_t str_removeat(str_t s, NW_Int32  index, NW_Ucs2 separator)
{
  NW_Int32  start = 0, el_len = 0, i = 0;
  NW_Int32 len;
  NW_Ucs2 *ptr;
  str_t res = 0;

  NW_ASSERT(s);

  len = str_len(s);
  ptr = str_buf_ptr(s);

  for (i = 0; (i < len) && (index > 0); i++)
  {
    if (ptr[i] == separator)
    {
      --index;
      start = i + 1;
    }
  }

  for (i = start; i < len; i++)
  {
    if (ptr[i] == separator) break;
  }
  el_len = i - start;

  /* first */
  if (start == 0) {
    res = scr_substr(s, el_len + 1, len);
  }
  /* last */
  else if (start + el_len == len)
  {
    res = scr_substr(s, 0, start - 1);
  }
  else
  {
    str_t sub1 = 0;
    str_t sub2 = 0;

    sub1 = scr_substr(s, 0, start);
    sub2 = scr_substr(s, start + el_len + 1, len);
    NW_ASSERT(sub1);
    NW_ASSERT(sub2);
    res = str_cat(sub1, sub2);
    free_str(sub1);
    free_str(sub2);
  }

  return res;
}



str_t str_replaceat(str_t s, str_t newstr, NW_Int32  index, NW_Ucs2 separator)
{
  NW_Int32  start = 0, el_len = 0, i = 0;
  NW_Int32 len;
  NW_Ucs2 *ptr;
  str_t sub1 = 0;
  str_t sub2 = 0;
  str_t res = 0;

  NW_ASSERT(s);
  NW_ASSERT(newstr);
  
  len = str_len(s);
  ptr = str_buf_ptr(s);

  for(i = 0; (i < len) && (index > 0); i++)
  {
    if (ptr[i] == separator)
    {
      --index;
      start = i + 1;
    }
  }

  for(i = start; i < len; i++)
  {
    if (ptr[i] == separator) break;
  }
  el_len = i - start;

  sub1 = scr_substr(s, 0, start);
  sub2 = scr_substr(s, start + el_len, len);
  NW_ASSERT(sub1);
  NW_ASSERT(sub2);
  
  res = str_cat3(sub1, newstr, sub2);

  free_str(sub1);
  free_str(sub2);

  return res;
  
}



str_t str_insertat(str_t s, str_t newstr, NW_Int32  index, NW_Ucs2 separator)
{
  NW_Int32  start = 0, i = 0;
  NW_Int32 len;
  NW_Ucs2 *ptr;
  str_t sep_str = ucs2ch2str(separator);
  str_t tmpstr = 0;
  str_t res = 0;

  if (!s || !sep_str || !newstr)
  {
    return 0;
  }

  len = str_len(s);
  ptr = str_buf_ptr(s);

  for (i = 0; (i < len) && (index > 0); i++)
  {
    if (ptr[i] == separator)
    {
      --index;
      start = i + 1;
    }
  }

  if ((start == 0) && (i == len) && (len != 0) && (index > 0)) {
    start = len;
  }
  
  if (start == 0)
  {
    str_t empty_str = new_empty_str();
    if (!empty_str) {
      return 0; /* out of memory */
    }
    if (0 != str_cmp(s, empty_str)) {
      tmpstr = str_cat(newstr, sep_str);
    } else {
      tmpstr = dup_str(newstr);
    }
    res = str_cat(tmpstr, s);
    free_str(empty_str);
  }
  else if (i == len)
  {
    NW_ASSERT(NW_FALSE == str_is_empty(s));
    res =  str_cat3(s, sep_str, newstr);
  }
  else
  {
    str_t sub1 = scr_substr(s, 0, start);
    str_t sub2 = scr_substr(s, start, len - start);
    tmpstr = str_cat3(sub1, newstr, sep_str);
    res = str_cat(tmpstr, sub2);
    free_str(sub1);
    free_str(sub2);
  }

  free_str(tmpstr);
  free_str(sep_str);

  return res;
}



str_t str_squeeze(str_t s)
{
  NW_Int32  stringlen, reslen, i;
  NW_Bool blanks = NW_FALSE;
  NW_Ucs2 *ucs2_str;
  str_t res = 0;

  NW_ASSERT(s);
  stringlen = str_len(s);

  ucs2_str = NW_Str_New(stringlen);
  if (!ucs2_str)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  for(i = 0, reslen = 0; i < stringlen; i++)
  {
    NW_Ucs2 wc = scr_charat(s, i);
    if (wc > 32)
    {
      ucs2_str[reslen++] = wc;
      blanks = NW_FALSE;
    }
    else
    {
      if (!blanks) ucs2_str[reslen++] = ' ';
      blanks = NW_TRUE;
    }
  }

  ucs2_str[reslen] = 0;

  res = ucs22str(ucs2_str);

  NW_Str_Delete(ucs2_str);

  return res;
}



str_t str_trim(str_t s)
{
  NW_Int32  from_end = 0, from_start = 0;
  NW_Int32 len;
  NW_Ucs2 *ptr;

  NW_ASSERT(s);

  len = str_len(s);
  ptr = str_buf_ptr(s);

  for(;from_start < len ;from_start++)
    if (ptr[from_start] > 32) break;

  if (from_start == len) return new_empty_str();

  for(;;from_end++)
    if (ptr[len - from_end - 1] > 32) break;

  return scr_substr(s, from_start, len - from_end - from_start);
}



NW_Bool str_is_empty(str_t str)
{
  NW_ASSERT(str);

  return NW_BOOL_CAST(str_len(str) == 0);
}



NW_Bool str_is_alpha(NW_Ucs2 c)
{
  return NW_BOOL_CAST((('a' <= c) && ( c <= 'z')) || (('A' <= c) && (c <= 'Z')));
}



NW_Bool str_is_digit(NW_Ucs2 c)
{
  return NW_BOOL_CAST(('0' <= c) && (c <= '9'));
}


static const char* const reserved_word_table[] =
{
  "access",
  "agent",
  "break",
  "continue",
  "div",
  "div=",
  "domain",
  "else",
  "equiv",
  "extern",
  "for",
  "function",
  "header",
  "http",
  "if",
  "isvalid",
  "meta",
  "name",
  "path",
  "public",
  "return",
  "ScriptVarType",
  "use",
  "user",
  "var",
  "while",
  "url",
  "delete",
  "in",
  "lib",
  "new",
  "null",
  "this",
  "void",
  "with",
  "case",
  "catch",
  "class",
  "const",
  "debugger",
  "default",
  "do",
  "enum",
  "export",
  "extends",
  "finally",
  "import",
  "private",
  "sizeof",
  "struct",
  "super",
  "switch",
  "throw",
  "try"
};


NW_Bool str_is_reserved(str_t str)
{
  NW_Ucs2 *ptr;
  NW_Int32 i, num;

  if (str == NULL) {
    return NW_FALSE;
  }

  ptr = str_buf_ptr(str);
  num = sizeof(reserved_word_table)/sizeof(reserved_word_table[0]);

  for (i = 0; i < num; i++) {
    if (NW_Str_StrcmpConst(ptr, reserved_word_table[i]) == 0) {
      return NW_TRUE;
    }
  }

  return NW_FALSE;
}

