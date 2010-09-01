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
    $Workfile: scr_lang.c $

    Purpose:

        This file implements the ScriptServer functionality for the Lang standard library.
        The Lang standard library contains a set of functions that are closely related 
        to the ScriptServer language core.
 */

#include "scr_lang.h"
#include "scr_srv.h"
#include "scr_core.h"
#include "scr_conv.h"
#include "scr_api.h"

#include "nwx_string.h"
#include "nwx_math.h"

/*#define NO_OF_LANG_FUNCS 15
static lib_function_t lang_funcs[NO_OF_LANG_FUNCS];*/

NW_Byte size_lang_lib(void)
{
  return NO_OF_LANG_FUNCS;
}


static NW_Bool lang_abs(void)
{
  NW_Int32  i;
  NW_Float32 f;
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();

  if (val2int(v, &i))
    res = new_int_val(NW_Math_abs(i));

  else if (val2float(v, &f))
    res = new_float_val((NW_Float32)NW_Math_fabs(f));

  else res = new_invalid_val();

  push_estack(res);

  free_val(v);
  free_val(res);
  return NW_TRUE;
}


/* I try to avoid casting of NW_Int32  to NW_Float32 to prevent rounding errors */
static NW_Bool lang_min(void)
{
  NW_Float32 f1, f2;
  NW_Int32  i1, i2;
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();
  v1 = pop_estack();

  if (val2int(v1, &i1)) {
    if (val2int(v2, &i2)) {
      res = new_int_val(NW_MIN(i1, i2));
    } else if (val2float(v2, &f2)) {
      if (i1 <= f2) {
        res = new_int_val(i1);
      } else {
        res = new_float_val(f2);
      }
    } else {
      res = new_invalid_val(); /* cannot convert v2 to NW_Int32  or NW_Float32 */
    }
  } else if (val2float(v1, &f1)) {
    if (val2int(v2, &i2)) {
      if (f1 <= i2) {
        res = new_float_val(f1);
      } else {
        res = new_int_val(i2);
      }
    } else if (val2float(v2, &f2)) {
      res = new_float_val(NW_MIN(f1, f2));
    } else {
      res = new_invalid_val(); /* cannot convert v2 to NW_Float32 or NW_Int32  */
    }
  } else {
    res = new_invalid_val(); /* cannot convert v1 to NW_Float32 or NW_Int32  */
  }
  push_estack(res);

  free_val(v1);
  free_val(v2);
  free_val(res);
  return NW_TRUE;
}


/* I try to avoid casting of NW_Int32  to NW_Float32 to prevent rounding errors */
static NW_Bool lang_max(void)
{
  NW_Float32 f1, f2;
  NW_Int32  i1, i2;
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();
  v1 = pop_estack();

  if (val2int(v1, &i1)) {
    if (val2int(v2, &i2)) {
      res = new_int_val(NW_MAX(i1, i2));
    } else if (val2float(v2, &f2)) {
      if (i1 >= f2) {
        res = new_int_val(i1);
      } else {
        res = new_float_val(f2);
      }
    } else {
      res = new_invalid_val(); /* cannot convert v2 to NW_Int32  or NW_Float32 */
    }
  } else if (val2float(v1, &f1)) {
    if (val2int(v2, &i2)) {
      if (f1 >= i2) {
        res = new_float_val(f1);
      } else {
        res = new_int_val(i2);
      }
    } else if (val2float(v2, &f2)) {
      res = new_float_val(NW_MAX(f1, f2));
    } else {
      res = new_invalid_val(); /* cannot convert to NW_Int32  or NW_Float32 */
    }
  } else {
    res = new_invalid_val(); /* cannot convert v1 to NW_Float32 or NW_Int32  */
  }

  push_estack(res);

  free_val(v1);
  free_val(v2);
  free_val(res);
  return NW_TRUE;
}

static NW_Bool lang_parseInt(void)
{
  val_t v = uninitialize_val(), res = uninitialize_val();
  str_t str = 0;
  NW_Int32  ival;
  NW_Bool consumeAll;

  v = pop_estack();

  if ((val2str(v, &str)) && str2int(str, &ival, &consumeAll))
    res = new_int_val(ival);
  else res = new_invalid_val();

  push_estack(res);

  free_str(str);
  free_val(v);
  free_val(res);
  return NW_TRUE;
}

static NW_Bool lang_parseFloat(void)
{
  val_t v = uninitialize_val(), res = uninitialize_val();
  str_t str = 0;
  NW_Float32 fval;
  NW_Bool consumeAll;

  v = pop_estack();

  if ((val2str(v, &str)) && str2float(str, &fval, &consumeAll))
    res = new_float_val(fval);
  else res = new_invalid_val();

  push_estack(res);

  free_str(str);
  free_val(v);
  free_val(res);
  return NW_TRUE;
}


/* How can we get invalid ? */
static NW_Bool lang_isInt(void)
{
  val_t v = uninitialize_val(), res = uninitialize_val();
  str_t str = 0;

  v = pop_estack();

  if (ScriptVarType(v) != VAL_TYPE_INVALID)
  {
    NW_Int32  ival;
    NW_Bool bval, consumeAll;

    bval = NW_BOOL_CAST((val2str(v, &str)) && str2int(str, &ival, &consumeAll));
    res = new_bool_val(bval);
  }
  else res = new_invalid_val();

  push_estack(res);

  free_str(str);
  free_val(v);
  free_val(res);
  return NW_TRUE;
}


static NW_Bool lang_isFloat(void)
{
  val_t v = uninitialize_val(), res = uninitialize_val();
  str_t str = 0;

  v = pop_estack();

  if (ScriptVarType(v) != VAL_TYPE_INVALID)
  {
    NW_Float32 fval;
    NW_Bool bval, consumeAll;

    bval = NW_BOOL_CAST((val2str(v, &str)) && str2float(str, &fval, &consumeAll));
    res = new_bool_val(bval);
  }
  else res = new_invalid_val();

  push_estack(res);

  free_str(str);
  free_val(v);
  free_val(res);
  return NW_TRUE;
}


static NW_Bool lang_maxInt(void)
{
  val_t res = uninitialize_val();

  res = new_int_val(SCR_MAX_INT);

  push_estack(res);

  free_val(res);
  return NW_TRUE;
}


static NW_Bool lang_minInt(void)
{
  val_t res = uninitialize_val();

  res = new_int_val(SCR_MIN_INT);

  push_estack(res);

  free_val(res);
  return NW_TRUE;
}


static NW_Bool lang_float(void)
{
  val_t res = uninitialize_val();

  res = new_bool_val(NW_TRUE);

  push_estack(res);

  free_val(res);
  return NW_TRUE;
}


static NW_Bool lang_exit(void)
{
  val_t v = uninitialize_val();

  /* what to do with this return value ??? */
  v = pop_estack();

  /*while (!is_frstack_empty()) NW_Wmls_PopFrame();*/
  /* free_all_frames(); */ /* possible double deallocation. fix moved from Wave */
  
  /* Just set the error for later reporting. */
  /*ScriptAPI_finish(SCR_ERROR_USER_EXIT, 0); */
  set_error_code( SCR_ERROR_USER_EXIT );
  free_val(v);
  return NW_TRUE;
}


static NW_Bool lang_abort(void)
{
  val_t v = uninitialize_val();
  NW_Ucs2 *str = 0;

  /* what to do with this return value ??? */
  v = pop_estack();

  if (ScriptVarType(v) == VAL_TYPE_INVALID) {
    str = NW_Str_CvtFromAscii("invalid");
    if (str == NULL) {
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    }
  } else {
    str = val2ucs2(v);
  }

 /* free_all_frames(); */  /* possible double deallocation. Fix moved from Wave */
  
  /* Set error code for later reporting. */
  /* ScriptAPI_finish(SCR_ERROR_USER_ABORT, str); */
  set_error_code( SCR_ERROR_USER_ABORT );
  set_error_msg( str );  /* Don't free str.  Freed later when context cleared. */

  free_val(v);
  return NW_TRUE;
}


static NW_Bool lang_random(void)
{
  NW_Int32  i, limit;
  NW_Float32 f;
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();

  if (val2int(v, &i)) limit = i;
  else if (val2float(v, &f)) limit = NW_Math_truncate(f);
  else limit = -1;

  /* spec wmls 7.13 says 0->limit inclusive */
  if (limit > 0) res = new_int_val(NW_Math_rand()%(limit+1));
  else if (limit < 0) res = new_invalid_val();
  else res = new_int_val(0);

  push_estack(res);

  free_val(v);
  free_val(res);
  return NW_TRUE;
}


static NW_Bool lang_seed(void)
{
  NW_Int32  i, seed;
  NW_Float32 f;
  val_t v = uninitialize_val();

  v = pop_estack();

  if (val2int(v, &i)) seed = i;
  else if (val2float(v, &f)) seed = NW_Math_truncate(f);
  else seed = -1;

  if (seed >= 0)
  {
    NW_Math_srand(seed);
    push_estack(new_empty_str_val());
  }
  else push_estack(new_invalid_val());

  free_val(v);
  return NW_TRUE;
}


static NW_Bool lang_characterSet(void)
{
  val_t res = uninitialize_val();

  res = new_int_val(ScriptAPI_character_set());

  push_estack(res);

  free_val(res);
  return NW_TRUE;
}


/*  functions in Lang library */
static const lib_function_t lang_lib_table[NO_OF_LANG_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          lang_abs,                 NW_FALSE },
  {   2,          lang_min,                 NW_FALSE },
  {   2,          lang_max,                 NW_FALSE },
  {   1,          lang_parseInt,            NW_FALSE },
  {   1,          lang_parseFloat,          NW_FALSE },
  {   1,          lang_isInt,               NW_FALSE },
  {   1,          lang_isFloat,             NW_FALSE },
  {   0,          lang_maxInt,              NW_FALSE },
  {   0,          lang_minInt,              NW_FALSE },
  {   0,          lang_float,               NW_FALSE },
  {   1,          lang_exit,                NW_FALSE },
  {   1,          lang_abort,               NW_FALSE },
  {   1,          lang_random,              NW_FALSE },
  {   1,          lang_seed,                NW_FALSE },
  {   0,          lang_characterSet,        NW_FALSE }
};

void populate_lang_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(LANG_LIB_ID);
  (*table)[i] = lang_lib_table;

  return;
}
