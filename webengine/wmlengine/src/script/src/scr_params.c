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
    $Workfile: scr_params.c $

     Purpose: handle parameter parsing in urls
 */

#include "scr_params.h"
#include "scr_context.h"
#include "scr_conv.h"
#include "scr_core.h"

#include "nwx_string.h"
#include "nwx_url_utils.h"
 
#define INVALID_LITERAL     "invalid"
#define BOOL_TRUE_LITERAL   "true"
#define BOOL_FALSE_LITERAL  "false"

#define INVALID_LITERAL_LEN     (sizeof(INVALID_LITERAL)-1)
#define BOOL_TRUE_LITERAL_LEN   (sizeof(BOOL_TRUE_LITERAL)-1)
#define BOOL_FALSE_LITERAL_LEN  (sizeof(BOOL_FALSE_LITERAL)-1)

NW_Bool read_hex_char(NW_Ucs2 *p, NW_Int32  *val)
{
  NW_Int32  i;

  *val = 0;
  for(i = 0; i < 2; i++)
  {
    *val <<= 4;

    if ( ('0' <= p[i]) && (p[i] <= '9') )
      *val += (p[i] - '0');
    else if (('A' <= p[i]) && (p[i] <= 'F') )
      *val += (p[i] - 'A' + 10);
    else if (('a' <= p[i]) && (p[i] <= 'f') )
      *val += (p[i] - 'a' + 10);
    else
      return NW_FALSE;
  }

  return NW_TRUE;
}

NW_Bool read_hex_wchar(NW_Ucs2 *p, NW_Int32  *val)
{
  NW_Int32  i;

  *val = 0;
  for(i = 0; i < 4; i++)
  {
    *val <<= 4;

    if (('0' <= p[i]) && ( p[i] <= '9') )
      *val += (p[i] - '0');
    else if (('A' <= p[i]) && ( p[i] <= 'F'))
      *val += (p[i] - 'A' + 10);
    else if (('a' <= p[i]) && ( p[i] <= 'f'))
      *val += (p[i] - 'a' + 10);
    else
      return NW_FALSE;
  }

  return NW_TRUE;
}


NW_Int32  read_oct_char(NW_Ucs2 *p, NW_Int32  *val)
{
  NW_Int32  i;

  *val = 0;
  for(i = 0; i < 3; i++)
  {
    if ((i == 2) && (p[0] > '3')) break;
    if (('0' <= p[i]) && (p[i] <= '7'))
    {
      *val <<= 3;
      *val += (p[i] - '0');
    }
    else break;
  }

  return i;
}


NW_Ucs2 *read_func_name(NW_Ucs2 *frag)
{
  NW_Int32  i;
  NW_Int32 len;

  if (!NW_Str_Isalpha(frag[0])) return 0;

  len = NW_Str_Strlen(frag);
  for(i=0; i < len; i++) 
  {
    if (NW_Str_Isalnum(frag[i]) || (frag[i] == WAE_URL_UNDER_CHAR)) /* It now allows underscore in func name */
      continue;
    if (frag[i] != '(') {
      return 0;
    } else {
      NW_Ucs2 *res = NW_Str_Substr(frag, 0, i);
      if (res == NULL) {
        set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      }
      return res;
    }
  }
  return NULL;
}


NW_Ucs2 *read_string_literal(NW_Ucs2 **p)
{
  NW_Ucs2 start_char = **p, *res = 0;

  NW_Int32  i, j;

  NW_Int32  len = NW_Str_Strlen(*p);
  res = NW_Str_New(len);
  if (res == NULL) {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NULL;
  }

  for(i=1, j = 0;;)
  {
    if (i >=  len)
    {
      NW_Str_Delete(res);
      return NULL;
    }
    if ((*p)[i] == start_char)
    {
      *p += (i + 1);
      res[j] = '\0';
      return res;
    }

    if ((*p)[i] == '\\')
    {
      i++;
      if (((*p)[i] == '\'') || ((*p)[i] == '\"') || ((*p)[i] == '\\') || ((*p)[i] == '/'))
      {
        res[j++] = (*p)[i];
        i++;
      }
      else if ((*p)[i] == 'b')
      {
        res[j++] = 0x8;
        i++;
      }
      else if ((*p)[i] == 'f')
      {
        res[j++] = 0xC;
        i++;
      }
      else if ((*p)[i] == 'n')
      {
        res[j++] = 0xA;
        i++;
      }
      else if ((*p)[i] == 'r')
      {
        res[j++] = 0xD;
        i++;
      }
      else if ((*p)[i] == 't')
      {
        res[j++] = 0x9;
        i++;
      }
      else if ((*p)[i] == 'x')
      {
        NW_Int32  val;
        if (!read_hex_char(*p + i + 1, &val))
        {
          NW_Str_Delete(res);
          return NULL;
        }
        /* returned character should be ascii */
        res[j++] = NW_UINT8_CAST(val);
        i+=2;
      }
      else if ((*p)[i] == 'u')
      {
        NW_Int32  val;
        if (!read_hex_wchar(*p + i + 1, &val))
        {
          NW_Str_Delete(res);
          return NULL;
        }
        /* returned character should be ascii */
        res[j++] = NW_UINT8_CAST(val);
        i+=4;
      }
      else
      {
        NW_Int32  val = 0;
        NW_Int32  nbytes = read_oct_char(*p + i, &val);
        if (nbytes == 0)
        {
          NW_Str_Delete(res);
          return NULL;
        }
        /* returned character should be ascii */
        res[j++] = NW_UINT8_CAST(val);
        i += nbytes;
      }
    }
    else res[j++] = (*p)[i++];
  }
}


NW_Bool read_numeric_literal(NW_Ucs2 **p, NW_Byte index)
{
  NW_Ucs2 *arg_end = 0, *str = 0;
  str_t s;
  NW_Int32  ival;
  NW_Float32 fval;
  NW_Bool retval = NW_FALSE, consume;

  for(arg_end = *p; (*arg_end != ',') && (*arg_end != ')'); arg_end++) {}

  if (arg_end == *p) return NW_FALSE;
  str = NW_Str_Substr(*p, 0, (arg_end - *p));
  if (!str)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }

  s = ucs22str(str);
  NW_Str_Delete(str);

  if (str2int(s, &ival, &consume) && consume)
  {
    val_t v = new_int_val(ival);
    set_func_var(index, v);
    *p = arg_end;
    free_val(v);
    retval = NW_TRUE;
  }

  else if (str2float(s, &fval, &consume) && consume)
  {
    val_t v = new_float_val(fval);
    set_func_var(index, v);
    *p = arg_end;
    free_val(v);
    retval = NW_TRUE;
  }

  free_str(s);

  return retval;
}


NW_Bool parse_argument(NW_Ucs2 **p, NW_Byte index)
{
  NW_Bool retval = NW_TRUE;

  while(NW_Str_Isspace(**p)) (*p)++;

  if (!NW_Str_StrncmpConst(*p, INVALID_LITERAL, INVALID_LITERAL_LEN))
  {
    set_func_var(index, new_invalid_val());
    *p += INVALID_LITERAL_LEN;
  }

  else if (!NW_Str_StrncmpConst(*p, BOOL_TRUE_LITERAL, BOOL_TRUE_LITERAL_LEN))
  {
    val_t v = new_bool_val(NW_TRUE);
    set_func_var(index, v);
    *p += BOOL_TRUE_LITERAL_LEN;
    free_val(v);
  }

  else if (!NW_Str_StrncmpConst(*p, BOOL_FALSE_LITERAL, BOOL_FALSE_LITERAL_LEN))
  {
    val_t v = new_bool_val(NW_FALSE);
    set_func_var(index, v);
    *p += BOOL_FALSE_LITERAL_LEN;
    free_val(v);
  }

  else if ((**p == '\'') || (**p == '\"'))
  {
    str_t s;
    val_t v;
    NW_Ucs2 *str = read_string_literal(p);
    if (!str) return NW_FALSE;

    s = ucs22str(str);
    v = new_str_val(s);
    set_func_var(index, v);
    free_str(s);
    free_val(v);
    NW_Str_Delete(str);
  }

  else retval = read_numeric_literal(p, index);

  if (retval)
  {
    while(NW_Str_Isspace(**p)) (*p)++;
    return NW_TRUE;
  }
  else return NW_FALSE;
}


NW_Bool parse_argument_list(NW_Ucs2 *params, NW_Int32  args)
{
  NW_Ucs2 *p = params;
  NW_Byte index = 0;

  if (*p++ != '(') return NW_FALSE;

  if (*p == ')') return NW_BOOL_CAST(args == 0); /* check for an empty argument list */

  while(*p && (index < args))
  {
    if (!parse_argument(&p, index++)) return NW_FALSE;
    if (*p == ',') p++;
    else if (*p == ')') return NW_BOOL_CAST(args == index);
    else return NW_FALSE;
  }

  return NW_FALSE;
}

