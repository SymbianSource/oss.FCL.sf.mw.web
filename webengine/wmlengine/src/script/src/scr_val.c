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
    $Workfile: scr_val.c $

     Purpose:
        
          This file implements values
 */


#include "scr_val.h"
#include "scr_conv.h"
#include "scr_core.h"
#include "scr_estack.h" /* 1/17/01 */

#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_ctx.h"
#include "nwx_math.h"
#include "nwx_float.h"

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
  This function pops off the stack a val_t and converts it to ucs2
  and returns the status.

  Depending upon the value of variable "trimFlag", leading and
  trailing whitespace may be trimmed.

  ***NOTE*** that this function FREE's parameter "v" and if an
  ERROR is detected, frees "ustr" as well.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static NW_Bool retrieveUcs2Val(NW_Ucs2 **ustr, NW_Bool trimFlag )
{
  val_t v = uninitialize_val();

  v = pop_estack();

  if (ScriptVarType(v) == VAL_TYPE_INVALID) {
    push_estack(new_invalid_val());
    free_val(v);
    return NW_FALSE;
  }

  if (trimFlag)
  {
    *ustr = val2ucs2_trim(v);
  }
  else
  {
    *ustr = val2ucs2(v);
  }
  free_val(v);

  if (ustr == NULL) {
    push_estack(new_invalid_val());
  }

  return NW_TRUE;
}



/*
**-------------------------------------------------------------------------
**  External Prototypes
**-------------------------------------------------------------------------
*/
extern NW_Int32   get_int(val_t);
extern NW_Bool    get_bool(val_t);
extern NW_Float32 get_float(val_t);



NW_Ucs2* GetUcs2Val(void)
{
  NW_Ucs2 *ustr = 0;
  if(retrieveUcs2Val(&ustr, NW_TRUE )) {
    return ustr;
  }
  else {
    return NULL;
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
   Name    :   GetUcs2Val_notrim
   Input   :   void
   Output  :   bool
   Purpose :   This version of the above does NOT strip leading and trailing
               whitespace.
   
               This function pops off the stack a val_t and converts it to ucs2
               and returns the status.

               ***NOTE*** that this function FREE's parameter "v" and if an
               ERROR is detected, frees "ustr" as well.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
NW_Ucs2* GetUcs2Val_notrim(void)
{
  NW_Ucs2 *ustr = 0;
  if(retrieveUcs2Val(&ustr, NW_FALSE )) {
    return ustr;
  }
  else {
    return NULL;
  }
} 


/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   GetIntVal
    Input   :   void
    Output  :   bool
    Purpose :   This function pops off the stack a val_t and converts it to NW_Int32
                and returns the status
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
NW_Bool GetIntVal(NW_Int32 *value)
{
  NW_Bool status = NW_FALSE;
  val_t v = uninitialize_val();
 
  v = pop_estack();

  if (ScriptVarType(v) == VAL_TYPE_INVALID) 
  {
    push_estack(new_invalid_val());
    free_val(v);
    return NW_FALSE; /* Error */
  }

  status = val2int(v, value);
  if(status == NW_FALSE)
  {
    push_estack(new_invalid_val());
    free_val(v);
    return NW_FALSE; /* Error */
  }
  free_val(v);

  return NW_TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   GetBoolVal
    Input   :   void
    Output  :   bool
    Purpose :   This function pops off the stack a val_t and converts it to NW_Bool
                and returns the status
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
NW_Bool GetBoolVal(NW_Bool *value)
{
  val_t v = uninitialize_val();
 
  v = pop_estack();

  if (ScriptVarType(v) == VAL_TYPE_INVALID) 
  {
    push_estack(new_invalid_val());
    free_val(v);
    return NW_FALSE; /* Error */
  }

  *value = get_bool(v);
  free_val(v);

  return NW_TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   GetFloatVal
    Input   :   void
    Output  :   Float
    Purpose :   This function pops off the stack a val_t and converts it to NW_Bool
                and returns the status
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
NW_Bool GetFloatVal(NW_Float32 *value)
{
  val_t v = uninitialize_val();
 
  v = pop_estack();

  if (ScriptVarType(v) == VAL_TYPE_INVALID) 
  {
    push_estack(new_invalid_val());
    free_val(v);
    return NW_FALSE; /* Error */
  }

  *value = get_float(v);
  free_val(v);

  return NW_TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   GetOpaqueVal
    Input   :   void
    Output  :   bool
    Purpose :   This function pops off the stack a val_t and converts it to NW_
                and returns the status
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
NW_Opaque_t* GetOpaqueVal(void)
{
  NW_Opaque_t *opaque;
  NW_Int32 i;
  NW_Bool b;
  NW_Float32 f;
  
  val_t v = uninitialize_val();
  v = pop_estack();

  /* allocate OpaqueVal */
  opaque = (NW_Opaque_t *) NW_Mem_Malloc( sizeof( NW_Opaque_t ) );
  if ( opaque == NULL ) {
    return NULL;
  }

  switch (ScriptVarType(v)) {
  case VAL_TYPE_STR:
    opaque->value.s = GetUcs2Val();
    opaque->type = NW_OPAQUE_VALUE_TYPE_STRING;
    break;
  case VAL_TYPE_INT:
    if (!GetIntVal(&i)) {
      NW_Mem_Free( opaque );
      return NULL; /* Error */
    }
    opaque->value.i = i;
    opaque->type = NW_OPAQUE_VALUE_TYPE_INT32;
    break;
  case VAL_TYPE_BOOL:
    if (!GetBoolVal(&b)) {
      NW_Mem_Free( opaque );
      return NULL; /* Error */
    }
    opaque->value.b = b;
    opaque->type = NW_OPAQUE_VALUE_TYPE_BOOL;
    break;
  case VAL_TYPE_FLOAT:
    if (!GetFloatVal(&f)) {
      NW_Mem_Free( opaque );
      return NULL; /* Error */
    }
    opaque->value.f = f;
    opaque->type = NW_OPAQUE_VALUE_TYPE_FLOAT;
    break;
  case VAL_TYPE_INVALID:
    NW_Mem_Free( opaque );
    free_val(v);
    return NULL; /* Error */
  case VAL_TYPE_UNINITIALIZED:
    /* case added to supress gcc warning, leaving same behavior.*/
    break;
  }

  free_val(v);

  return opaque;
}

val_t new_int_val(NW_Int32  i)
{
  val_t val_ret;
  val_ret.type = VAL_TYPE_INT;
  val_ret.value.i = i;
  return val_ret;
}

val_t new_float_val(NW_Float32 f)
{
  val_t val_ret;
  val_ret.type = VAL_TYPE_FLOAT;

  /* If we got "underflow" return 0 per WMLScript 6.2.7.2 */
     if ((f < SCR_MIN_FLOAT) && (f > -SCR_MIN_FLOAT))
   {
       f = 0.0;
   }
  val_ret.value.f = f;
  return val_ret;
}


val_t new_str_val(str_t s)
{
  val_t val_ret;
  
  if (!s) /*if the pointer to the string is zero, return invalid type???*/
  {
    val_ret.type = VAL_TYPE_INVALID;
    val_ret.value.i = 0;
    return val_ret;
  }

  if (str_is_empty(s))
  {
    return new_empty_str_val();
  }
  else
  {
    val_ret.value.s = dup_str(s);
    val_ret.type = VAL_TYPE_STR;
    return val_ret;
  }
}



/* Transfer ownership of string to a new val_t */
val_t new_str_val_and_free(str_t *sptr)
{
  val_t val_ret;
  
  if (!*sptr) /*if the pointer to the string is zero, return invalid type???*/
  {
    val_ret.type    = VAL_TYPE_INVALID;
    val_ret.value.i = 0;
    return val_ret;
  }

  val_ret.value.s = *sptr;
  val_ret.type    = VAL_TYPE_STR;
  *sptr           = NULL;
  return val_ret;
}


val_t new_empty_str_val(void)
{
  val_t val_ret;
  val_ret.type = VAL_TYPE_STR;    /*For empty string, the value field is set to 0*/
  val_ret.value.i = 0;    
  return val_ret;
}

val_t new_bool_val(NW_Bool b)
{
  val_t val_ret;
  val_ret.type = VAL_TYPE_BOOL;
  val_ret.value.b = b;
  return val_ret;
}

val_t new_invalid_val(void)
{
  val_t val_ret;
  val_ret.type = VAL_TYPE_INVALID;
  val_ret.value.i = 0;
  return val_ret;
}

NW_Int32  get_int(val_t v)
{
 
  if(v.type == VAL_TYPE_INT)
  {
    return v.value.i;
  }
  
  set_error_code(SCR_ERROR_VERIFICATION_FAILED);
  return 0;  /* TODO - should have better error return */
  
}

NW_Float32 get_float(val_t v)
{
  if(v.type == VAL_TYPE_FLOAT)
    return v.value.f;
  
  set_error_code(SCR_ERROR_VERIFICATION_FAILED);
  return 0;  /* TODO - should have better error return */
  
}

NW_Bool get_bool(val_t v)
{

  if(v.type == VAL_TYPE_BOOL)
    return v.value.b;
    
  set_error_code(SCR_ERROR_VERIFICATION_FAILED);
  return NW_FALSE;  /* TODO - should have better error return */
}

str_t get_str(val_t v)
{
  if(v.type == VAL_TYPE_STR)
  {
    if(v.value.i == 0)  /*if empty string*/
    {
         return new_str(0, 0);
    }
    return dup_str(v.value.s);
  }
  
  set_error_code(SCR_ERROR_VERIFICATION_FAILED);
  return (str_t)0;  /* TODO - should have better error return */
}

SCR_VAL_TYPE ScriptVarType(val_t v)
{
  return v.type;
}

/* make sure it copies the string */
val_t dup_val(val_t v)
{

  if (v.type == VAL_TYPE_INT) 
     return new_int_val(get_int(v));
  if (v.type == VAL_TYPE_FLOAT) 
     return new_float_val(get_float(v));
  if (v.type == VAL_TYPE_BOOL) 
     return new_bool_val(get_bool(v));
  if (v.type == VAL_TYPE_STR) 
  { 
    str_t s = get_str(v);
    val_t res = new_str_val(s);
    free_str(s);
    return res;
  }

return new_invalid_val();
}

NW_Bool val2bool(val_t v, NW_Bool *res)
{

  if (v.type == VAL_TYPE_STR) {
    str_t s = get_str(v);
    *res = NW_BOOL_CAST(!str_is_empty(s));
    free_str(s);
    return NW_TRUE;
  }

  if (v.type == VAL_TYPE_INT) {
    *res = NW_BOOL_CAST(get_int(v) != 0);
    return NW_TRUE;
  }

  if (v.type == VAL_TYPE_FLOAT) {
    *res = NW_BOOL_CAST(get_float(v) != 0.0);
    return NW_TRUE;
  }

  if (v.type == VAL_TYPE_BOOL) {
    *res = get_bool(v);
    return NW_TRUE;
  }

  return NW_FALSE;
}


NW_Bool val2float(val_t v, NW_Float32 *f)
{
  if (IS_STR(v))
  {
    NW_Bool consumeAll;
    str_t s = get_str(v);
    NW_Bool retval =  str2float(s, f, &consumeAll);
    free_str(s);
    return NW_BOOL_CAST(retval && consumeAll);
  }

  else if (IS_FLOAT(v)) {
    *f = get_float(v);
    return NW_TRUE;
  }

  else if (IS_INT(v)) {
    *f = (NW_Float32)get_int(v);
    return NW_TRUE;
  }

  else if (IS_BOOL(v)) {
    /* explicitly map NW_TRUE -> 1 and NW_FALSE -> 0 */
    *f = (get_bool(v) ? (NW_Float32)1 : (NW_Float32)0);
    return NW_TRUE;
  }

  return NW_FALSE;
}


NW_Ucs2 *val2ucs2(val_t v)
{
  NW_Ucs2 *res = 0;
  str_t s = 0;

  if (!val2str(v, &s)) return 0;

  res = str2ucs2(s); /* creates copy */
  if (!res) return 0;
  free_str(s);
  return res;
}


/* similar to val2ucs2 - remove white spaces in the beginning and in the end */
NW_Ucs2 *val2ucs2_trim(val_t v)
{
  NW_Ucs2 *res = 0;
  str_t s = 0;
  NW_Uint32  start, end;

  if (!val2str(v, &s)) return 0;

  res = str2ucs2(s); /* creates copy */
  if (!res) return 0;
  free_str(s);

  start = 0;
  end = NW_Str_Strlen(res);

  while (NW_Str_Isspace(res[start])) start++;
  while ((end != 0) && NW_Str_Isspace(res[end - 1])) end--;


  if (start > end)  /* only white spaces */
  {
    NW_Ucs2 *trimmed_res = (NW_Ucs2*)NW_Mem_Malloc(sizeof(NW_Ucs2));
    if (trimmed_res == NULL)
    {
      NW_Mem_Free(res);
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      return NULL;
    }

    trimmed_res[0] = '\0';
    NW_Mem_Free(res);
    return trimmed_res;
  }
  else if ((start > 0) || end < NW_Str_Strlen(res)) /* some white spaces */
  {
    NW_Ucs2 *trimmed_res = NW_Str_Substr(&res[start], 0, (end - start));
    if (trimmed_res == NULL)
    {
      NW_Mem_Free(res);
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      return NULL;
    }

    NW_Mem_Free(res);
    return trimmed_res;
  }

  else return res; /* no white spaces */
}


/* Convert a val_t to a string */
NW_Bool val2str(val_t v, str_t *str)
{
  if (IS_STR(v)) {
    *str = get_str(v);
    return NW_TRUE;
  }

  else if (IS_INT(v))
  {
    *str = int2str(get_int(v));
    return NW_BOOL_CAST(*str != 0);
  }

  else if (IS_FLOAT(v)) {
    *str = float2str(get_float(v));
    return NW_BOOL_CAST(*str != 0);
  }

  else if (IS_BOOL(v)) {
    *str = bool2str(get_bool(v));
    return NW_TRUE;
  }

  else return NW_FALSE;
}


/* Convert val_t to string and free the val_t */
NW_Bool val2str_and_free(val_t *vptr, str_t *str)
{
  if (IS_STR(*vptr)) {
    if(vptr->value.s == NULL) {
      /* if empty string */
      *str = new_str(0, 0);
    } else {
      /* transfer ownership of the string */
      *str = vptr->value.s;
    }
    vptr->value.s = NULL;
    vptr->type    = VAL_TYPE_UNINITIALIZED;
    return NW_TRUE;
  }

  else if (IS_INT(*vptr))
  {
    *str = int2str(get_int(*vptr));
    return NW_BOOL_CAST(*str != 0);
  }

  else if (IS_FLOAT(*vptr)) {
    *str = float2str(get_float(*vptr));
    return NW_BOOL_CAST(*str != 0);
  }

  else if (IS_BOOL(*vptr)) {
    *str = bool2str(get_bool(*vptr));
    return NW_TRUE;
  }

  else return NW_FALSE;
}


/* Convert a val_t to an integer */
NW_Bool val2int(val_t v, NW_Int32  *res)
{

  if (IS_STR(v))
  {
    NW_Bool consumeAll;
    str_t s = get_str(v);
    NW_Bool retval =  str2int(s, res, &consumeAll);
    free_str(s);
    return NW_BOOL_CAST(retval && consumeAll);
  }

  else if (IS_BOOL(v)) {
    /* explicitly map NW_TRUE -> 1 and NW_FALSE -> 0 */
    *res = (get_bool(v) ? 1 : 0);
    return NW_TRUE;
  }

  else if (IS_INT(v))  {
    *res = get_int(v);
    return NW_TRUE;
  }

  else return NW_FALSE;
}


/* Free a val_t's storage */
void free_val(val_t v)
{
    
  if (v.type == VAL_TYPE_STR)
  {
    free_str(v.value.s);
  }
  return;
}


/*Use to declare a value val_t*/
val_t uninitialize_val(void)
{
  val_t val_ret;
  val_ret.type = VAL_TYPE_UNINITIALIZED;
  val_ret.value.i = 0;
  return val_ret;
}

/*eliminate this function later*/
NW_Bool set_pointer_msb(void)
{
  return NW_TRUE;
}

