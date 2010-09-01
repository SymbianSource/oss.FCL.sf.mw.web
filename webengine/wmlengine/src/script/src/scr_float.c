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
    $Workfile: scr_float.c $

    Purpose:

        This file implements the the Float standard library. The Float 
        library contains a set of arithmetic floating point functions 
        that are frequently used by the applications.
        The implementation of this library is optional and implemented 
        only by devices that can support floating point operations.  If
        Floating-pont operations are not supported all functions in 
        this library must return invalid.
 */

#include "scr_api.h"
#include "scr_srv.h"
#include "scr_float.h"
#include "scr_valexpr.h"
#include "scr_estack.h"
#include "scr_conv.h"
#include "nwx_math.h"
 

/*#define NO_OF_FLOAT_FUNCS 8
static lib_function_t float_funcs[NO_OF_FLOAT_FUNCS];*/

NW_Byte size_float_lib(void)
{
  return NO_OF_FLOAT_FUNCS;
}

static NW_Bool float_int(void)
{
  NW_Float32 f;
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();

  if (val2float(v, &f)) {
    res = new_int_val(NW_Math_truncate(f));
  } else {
    res = new_invalid_val();
  }
  
  push_estack(res);

  free_val(res);
  free_val(v);
  
  return NW_TRUE;
}


static NW_Bool float_floor(void)
{
  NW_Float32 f;
  NW_Int32 i;
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();

  if (val2int(v, &i)) {
    res = new_int_val(i);
  } else if (val2float(v, &f)) {
    res = new_int_val(NW_Math_truncate(NW_Math_floor((f))));
  } else {
    res = new_invalid_val();
  }
  
  push_estack(res);
  free_val(res);
  free_val(v);
  return NW_TRUE;
}


static NW_Bool float_ceil(void)
{
  NW_Float32 f;
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();

  if (val2float(v, &f))
    res = new_int_val(NW_Math_truncate(NW_Math_ceil(f)));

  else res = new_invalid_val();

  push_estack(res);
  free_val(res);
  free_val(v);
  return NW_TRUE;
}



static NW_Bool float_pow(void)
{
  NW_Float32 f1, f2;
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();
  NW_Int32  i = 0;

  v2 = pop_estack();
  v1 = pop_estack();

  if (val2float(v1, &f1) && val2float(v2, &f2))
  {
    if (((f1 == 0.0) && (f2 < 0.0))  ||
        ((f1 < 0.0) && !val2int(v2, &i)))
    {
      res = new_invalid_val();
    }
    else 
    {
      if ( f2 == 0.0 )
      {
        /* Force result to 1 if anything raised to 0 power.
           Necessary because some pow() routines return 0
           instead of 1.  jwild 10/15/1999. */
        /*lint -e{747} Significant prototype coercion*/
        res = new_float_val( 1.0 );
      }
      else
      {
        if ((f1 < 0.0) || val2int(v2, &i)) {
          res = new_float_val((NW_Float32)NW_Math_powint(f1, i));
        } else {
          res = new_float_val((NW_Float32)NW_Math_pow(f1, f2));
        }
      }
    }
  }
  else 
    res = new_invalid_val();

  push_estack(res);
  free_val(res);
  free_val(v1);
  free_val(v2);
  return NW_TRUE;
}


static NW_Bool float_round(void)
{
  NW_Float32 f;
  val_t v = uninitialize_val(), res = uninitialize_val();
  NW_Int32  flr;
  
  v = pop_estack();

  if (val2float(v, &f))
  {
    flr = NW_Math_truncate(NW_Math_floor(f));
    res = (f - flr < 0.5)?new_int_val(flr):new_int_val(flr + 1);
  }
  else res = new_invalid_val();

  push_estack(res);
  free_val(res);
  free_val(v);
  return NW_TRUE;
}


static NW_Bool float_sqrt(void)
{
  NW_Float32 f;
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();

  if (val2float(v, &f))
    if (f >= 0.0)
    {
      /*lint -e{747} Significant prototype coercion*/
      res = new_float_val((NW_Float32)NW_Math_pow(f,0.5));
    }
    else res = new_invalid_val();

  else res = new_invalid_val();

  push_estack(res);
  free_val(res);
  free_val(v);
  return NW_TRUE;
}


static NW_Bool float_maxFloat(void)
{
  val_t v = uninitialize_val();

  v = new_float_val(SCR_MAX_FLOAT);
  push_estack(v);
  free_val(v);
  return NW_TRUE;
}


static NW_Bool float_minFloat(void)
{
  val_t v = uninitialize_val();

  v = new_float_val(SCR_MIN_FLOAT);
  push_estack(v);
  free_val(v);
  return NW_TRUE;
}


/*  functions in Float library */
static const lib_function_t float_lib_table[NO_OF_FLOAT_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          float_int,                NW_FALSE },
  {   1,          float_floor,              NW_FALSE },
  {   1,          float_ceil,               NW_FALSE },
  {   2,          float_pow,                NW_FALSE },
  {   1,          float_round,              NW_FALSE },
  {   1,          float_sqrt,               NW_FALSE },
  {   0,          float_maxFloat,           NW_FALSE },
  {   0,          float_minFloat,           NW_FALSE }
};

void populate_float_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(FLOAT_LIB_ID);
  (*table)[i] = float_lib_table;

  return;
}
