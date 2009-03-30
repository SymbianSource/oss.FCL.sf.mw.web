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
    $Workfile: scr_handle.c $

    Purpose:

       This file contains all functions needed to handle every opcode that is 
       defined in the ScriptServer Specification.  Each function operates on the 
       operand stack and the execution stack as described in the 
       ScriptServer Specification section 9.6.2.1.  
 */

#include "scr_handle.h"
#include "scr_estack.h"
#include "scr_core.h"
#include "scr_context.h"
#include "scr_valexpr.h"


#include "nwx_url_utils.h"
#include "nwx_mem.h"
#include "scr_api.h"
#include "BrsrStatusCodes.h"

NW_Bool handle_call_url_common(NW_Int32  uindex, NW_Int32  findex, NW_Byte args);

void handle_jump_fw_s(NW_Byte offset) 
{
  NW_ASSERT(offset < 32); /*OK to leave*/
  jump_opcode(offset + 1);
}

void handle_jump_fw() 
{
  NW_Byte b;
  if (!get_next_opcode_byte(&b))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }
  jump_opcode(b + 1);
}

void handle_jump_fw_w() 
{
  NW_Uint16 s;

  if (!get_next_opcode_int16(&s))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }
  jump_opcode(s + 1);
}

void handle_jump_bw_s(NW_Byte offset) 
{
  NW_ASSERT(offset < 32);
  jump_opcode(-offset);
}

void handle_jump_bw() 
{
  NW_Byte b;
  if (!get_next_opcode_byte(&b))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }
  jump_opcode(-b - 1);
}

void handle_jump_bw_w() 
{  
  NW_Uint16 s;

  if (!get_next_opcode_int16(&s))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }
  jump_opcode(-s - 2); 
}

void handle_tjump_fw_s(NW_Byte offset) 
{
  val_t v = uninitialize_val();
  NW_Bool b = NW_FALSE;

  NW_ASSERT(offset < 32);
  v = pop_estack();

  if (!val2bool(v, &b))
  {
    if (ScriptVarType(v) == VAL_TYPE_INVALID)
      b = NW_FALSE;
    else NW_ASSERT(NW_FALSE);
  }
  
  if (!b) jump_opcode(offset + 1);  /* on true to jump*/
  else jump_opcode(1); /* or not to jump*/

  free_val(v);
}

void handle_tjump_fw() 
{  
  val_t v = uninitialize_val();
  NW_Bool bo = NW_FALSE;
  NW_Byte b;

  if (!get_next_opcode_byte(&b))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = pop_estack();

  if (!val2bool(v, &bo))
  {
    if (ScriptVarType(v) == VAL_TYPE_INVALID)
      bo = NW_FALSE;
    else NW_ASSERT(NW_FALSE);
  }

  if (!bo) jump_opcode(b + 1);
  else jump_opcode(1); 

  free_val(v);
}

void handle_tjump_fw_w() 
{
  val_t v = uninitialize_val();
  NW_Bool b = NW_FALSE;
 
  NW_Uint16 s;

  if (!get_next_opcode_int16(&s))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = pop_estack();

  if (!val2bool(v, &b))
  {
    if (ScriptVarType(v) == VAL_TYPE_INVALID)
      b = NW_FALSE;
    else NW_ASSERT(NW_FALSE);
  }

  if (!b) jump_opcode(s + 1);
  else jump_opcode(1); 
  free_val(v);
}

void handle_tjump_bw() 
{
  NW_Byte b;
  val_t v = uninitialize_val();
  NW_Bool bo = NW_FALSE;
  
  v = pop_estack();

  if (!val2bool(v, &bo))
  {
    if (ScriptVarType(v) == VAL_TYPE_INVALID)
      bo = NW_FALSE;
    else NW_ASSERT(NW_FALSE);
  }

  if (!get_next_opcode_byte(&b))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  if (!bo) jump_opcode(-b-1);
  else jump_opcode(1);
  free_val(v);
}

void handle_tjump_bw_w() 
{ 
  NW_Uint16 s;
  val_t v = uninitialize_val();
  NW_Bool b = NW_FALSE;
  
  v = pop_estack();  

  if (!val2bool(v, &b))
  {
    if (ScriptVarType(v) == VAL_TYPE_INVALID)
      b = NW_FALSE;
    else NW_ASSERT(NW_FALSE);
  }

  if (!get_next_opcode_int16(&s))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  if (!b) jump_opcode(-s-2);
  else jump_opcode(1);
  free_val(v);
}

/* when hadling opcodes of type CALL_* the 
  position must be incremented by 1 before pushing 
  the new frame
  */
void handle_call_s(NW_Byte findex) 
{
  NW_Byte b;

  NW_ASSERT(findex < 8);

  if (!get_next_opcode_byte(&b))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  if (!push_frame(0, 0, 0, findex, 0))
  {
    set_error_code(SCR_ERROR_STACK_OVERFLOW);
    return;
  }
}

void handle_call() 
{  
  NW_Byte findex;

  if (!get_next_opcode_byte(&findex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  jump_opcode(1);

  if (!push_frame(0, 0, 0, findex, 0))
  {
    set_error_code(SCR_ERROR_STACK_OVERFLOW);
    return;
  }
}

NW_Bool handle_call_lib_s(NW_Byte findex, NW_Bool *async) 
{
  NW_Bool res;
  NW_Byte lindex;

  NW_ASSERT(findex < 8);  /* invalid index */
  if (!get_next_opcode_byte(&lindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (findex > 7 ) 
  {
    return NW_FALSE;
  }

  jump_opcode(1);

  res = call_lib_func(lindex, findex, async);

  return res;
}

NW_Bool handle_call_lib(NW_Bool *async) 
{
  NW_Bool res;

  NW_Byte findex,  lindex;

  if (!get_next_opcode_byte(&findex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (!get_next_opcode_byte(&lindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  jump_opcode(1);

  res = call_lib_func(lindex, findex, async);

  return res;
}

NW_Bool handle_call_lib_w(NW_Bool *async) 
{
  NW_Bool res;

  NW_Byte findex;
  NW_Uint16 lindex;

  if (!get_next_opcode_byte(&findex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (!get_next_opcode_int16(&lindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }


  jump_opcode(1);

  res = call_lib_func(lindex, findex, async);

  return res;
}

void handle_load_var_s(NW_Byte vindex) 
{ 
  val_t v = uninitialize_val();

  if (vindex > 31) /*uchar*/
  {
    NW_ASSERT(NW_FALSE);
    return;
  }
  
  v = get_func_var(vindex); 
  push_estack(v);

  jump_opcode(1);
}

void handle_load_var() 
{  
  val_t v = uninitialize_val();
  NW_Byte index;

  if (!get_next_opcode_byte(&index))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = get_func_var(index);
  push_estack(v);
  
  jump_opcode(1);
}

void handle_store_var_s(NW_Byte vindex) 
{ 
  val_t v = uninitialize_val();

  if (vindex > 15)
  {
    NW_ASSERT(NW_FALSE);
    return;
  }

  v = pop_estack();
  set_func_var(vindex, v);
  jump_opcode(1);
  free_val(v);
}

void handle_store_var() 
{ 
  val_t v = uninitialize_val();
  NW_Byte index;

  if (!get_next_opcode_byte(&index))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = pop_estack(); 
  set_func_var(index, v); 
  jump_opcode(1);
  free_val(v);

}

void handle_load_const_s(NW_Byte cindex) 
{  
  val_t v = uninitialize_val();
  
  if (cindex > 15)
  {
    NW_ASSERT(NW_FALSE);
    return;
  }

  if (!valid_const_index(cindex))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = get_const(cindex);
  push_estack(v);

  jump_opcode(1);
}

void handle_load_const() 
{
  val_t v = uninitialize_val();
  NW_Byte cindex;

  if (!get_next_opcode_byte(&cindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  if (!valid_const_index(cindex))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = get_const(cindex);

  push_estack(v);

  jump_opcode(1);
}

void handle_load_const_w() 
{
  NW_Uint16 cindex;
  val_t v = uninitialize_val();

  if (!get_next_opcode_int16(&cindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  
  if (!valid_const_index(cindex))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = get_const(cindex);
  push_estack(v);

  jump_opcode(1);
}

void handle_incr_var_s(NW_Byte vindex) 
{  
  val_t v = uninitialize_val(), res = uninitialize_val();

  NW_ASSERT( vindex < 8 );

  v = get_func_var(vindex);
  res = incr_val(v);
  set_func_var(vindex, res);

  jump_opcode(1);

  free_val(res);
}

void handle_incr_var() 
{  
  val_t v = uninitialize_val(), res = uninitialize_val();
  NW_Byte vindex;
  
  if (!get_next_opcode_byte(&vindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = get_func_var(vindex);
  res = incr_val(v);
  set_func_var(vindex, res);

  jump_opcode(1);

  free_val(res);
}

void handle_decr_var() 
{  
  val_t v = uninitialize_val(), res = uninitialize_val();
  NW_Byte vindex;
  
  if (!get_next_opcode_byte(&vindex))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v = get_func_var(vindex);  
  res = decr_val(v);
  set_func_var(vindex, res);

  jump_opcode(1);

  free_val(res);
}

void handle_incr() 
{  
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();  
  res = incr_val(v);
  push_estack(res);
  
  jump_opcode(1);
  free_val(v);
  free_val(res);
}

void handle_decr() 
{ 
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();  
  res = decr_val(v);
  push_estack(res);
  
  jump_opcode(1);
  free_val(v);
  free_val(res);
}

void handle_add_asg() 
{ 
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();
  NW_Byte index;

  v1 = pop_estack();

  if (!get_next_opcode_byte(&index))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v2 = get_func_var(index);
  res = add_vals(v2, v1);
  set_func_var(index, res);

  jump_opcode(1);

  free_val(v1);
  free_val(res);
}

void handle_sub_asg() 
{  
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();
  NW_Byte index;

  v1 = pop_estack();

  if (!get_next_opcode_byte(&index))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return;
  }

  v2 = get_func_var(index);
  res = sub_vals(v2, v1);
  set_func_var(index, res);
  
  jump_opcode(1);

  free_val(v1);
  free_val(res);
}

void handle_uminus() 
{ 
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();  
  res = uminus_val(v);
  push_estack(res);

  jump_opcode(1);

  free_val(v);
  free_val(res);
  
}

void handle_add() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  

  res = add_vals(v1, v2);
  push_estack(res);
  
  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}


void handle_sub() 
{  
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = sub_vals(v1, v2);
  push_estack(res);
  
  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_mul() 
{  
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = mul_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_div() 
{  
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = div_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_idiv() 
{  
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = idiv_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_rem() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();

  res = rem_vals(v1, v2);
  push_estack(res);
  
  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_b_and() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();
  res = bit_and_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_b_or() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();
  res = bit_or_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_b_xor() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();
  res = bit_xor_vals(v1, v2);
  push_estack(res);
  
  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_lshift() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = lshift_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_rsshift() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = rsshift_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_rszshift() 
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();
  v1 = pop_estack();
  res = rszshift_vals(v1, v2);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}


void handle_comp(NW_Byte op) 
{  
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), res = uninitialize_val();

  v2 = pop_estack();  
  v1 = pop_estack();  
  res = comp_vals(v1, v2, op);
  push_estack(res);

  jump_opcode(1);

  free_val(v1);
  free_val(v2);
  free_val(res);
}

void handle_l_not() 
{  
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();  
  res = l_not_val(v);
  push_estack(res);  

  jump_opcode(1);

  free_val(v);
  free_val(res);
}

void handle_b_not() 
{  
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();  
  res = b_not_val(v);
  push_estack(res);

  jump_opcode(1);

  free_val(v);
  free_val(res);
}

void handle_pop() 
{  
  val_t v = uninitialize_val();

  v = pop_estack();
  jump_opcode(1);

  free_val(v);
}

void handle_load_const_int(NW_Int32  i) 
{  
  val_t v = uninitialize_val();

  v = new_int_val(i); /* 99 is junk it is used only for small ints */
  
  push_estack(v);
  jump_opcode(1);

  free_val(v);
}


void handle_load_const_bool(NW_Bool b) 
{  
  val_t v = uninitialize_val();

  v = new_bool_val(b);

  push_estack(v);
  jump_opcode(1);

  free_val(v);
}

void handle_load_const_es() 
{  
  val_t v = uninitialize_val();

  v = new_empty_str_val();

  push_estack(v);
  jump_opcode(1);

  free_val(v);
}

void handle_load_invalid() 
{  
  val_t v = uninitialize_val();

  v = new_invalid_val();

  push_estack(v);
  jump_opcode(1);

  free_val(v);
}
  

void handle_typeof() 
{
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();
  res = new_int_val(ScriptVarType(v));
  push_estack(res);

  jump_opcode(1);

  free_val(v);
  free_val(res);
}

void handle_isvalid() 
{
  val_t v = uninitialize_val(), res = uninitialize_val();

  v = pop_estack();
  res = new_bool_val(NW_BOOL_CAST(ScriptVarType(v) != VAL_TYPE_INVALID));
  push_estack(res);

  jump_opcode(1);

  free_val(v);
  free_val(res);
}


void handle_tobool(void)
{
  val_t v = uninitialize_val();
  NW_Bool b;

  v = pop_estack();

  if (val2bool(v, &b)) push_estack(new_bool_val(b));
  else push_estack(new_invalid_val()); 

  jump_opcode(1);

  free_val(v);
}

void handle_scand(void)
{
  val_t v = uninitialize_val();
  NW_Bool b;

  v = pop_estack();

  if (val2bool(v, &b))
    if (b) push_estack(new_bool_val(NW_TRUE));
    else
    {
      push_estack(new_bool_val(NW_FALSE));
      push_estack(new_bool_val(NW_FALSE));
    }
  else
  {
    push_estack(new_invalid_val());
    push_estack(new_bool_val(NW_FALSE));
  }

  jump_opcode(1);

  free_val(v);
}

void handle_scor(void)
{
  val_t v = uninitialize_val();
  NW_Bool b;
    
  v = pop_estack();

  if (val2bool(v, &b))
    if (b)
    {
      push_estack(new_bool_val(NW_TRUE));
      push_estack(new_bool_val(NW_FALSE));
    }
    else push_estack(new_bool_val(NW_TRUE));
  else 
  {
    push_estack(new_invalid_val());
    push_estack(new_bool_val(NW_FALSE));
  }


  jump_opcode(1);

  free_val(v);
}

NW_Bool handle_call_url(void)
{
  NW_Byte uindex, findex, args;

  if (!get_next_opcode_byte(&uindex) ||
      !get_next_opcode_byte(&findex) ||
      !get_next_opcode_byte(&args))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  return handle_call_url_common(uindex, findex, args);
}

NW_Bool handle_call_url_w(void)
{
  NW_Uint16 uindex, findex;
  NW_Byte args;

  if (!get_next_opcode_int16(&uindex) ||
      !get_next_opcode_int16(&findex) ||
      !get_next_opcode_byte(&args))
  {    
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  return handle_call_url_common(uindex, findex, args);
}


NW_Bool handle_call_url_common(NW_Int32  uindex, NW_Int32  findex, NW_Byte args)
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val();
  NW_Ucs2 *url = 0;
  NW_Ucs2 *fname = 0;
  NW_Bool retval = NW_TRUE;
  context_t *ctx = ScriptAPI_getCurrentContext();
  NW_Bool isRelative;

  do {
    retval=NW_FALSE;
    if (!valid_const_index(uindex))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      break;
    }

    v1 = get_const(uindex);

    NW_ASSERT(ScriptVarType(v1) == VAL_TYPE_STR);

    url = val2ucs2(v1);

    if (!url) 
    {
       set_error_code(SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND);
       break;
    }

    if (KBrsrSuccess != NW_Url_IsRelative(url, &isRelative))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      NW_Mem_Free(url);
      break;
    }
    if (isRelative)
    {
      TBrowserStatusCode status = KBrsrSuccess;
      NW_Ucs2 *tmp;

      status = NW_Url_RelToAbs(get_curr_url(), url, &tmp);
      if (status == KBrsrOutOfMemory)
      {
        set_error_code(SCR_ERROR_OUT_OF_MEMORY);
        NW_Mem_Free(url);
        break;
      }
      NW_Mem_Free(url);
      url = tmp;
    }
    /* make sure the url is valid - validation error */

    if (!valid_const_index(findex))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      break;
    }

    v2 = get_const(findex);
    fname = val2ucs2(v2);
    /* make sure that fname is valid */

    if (!fname) 
    {
       set_error_code(SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND);
       break;
    }

    if (!set_ctx_call_str(ctx, fname, args)) break;

    if (!ScriptAPI_load(url))		
        break;

    jump_opcode(1);
    retval=NW_TRUE;
  } while (ctx == NULL); /* always NW_FALSE => no warn const conditional expr */

  if (url) NW_Mem_Free(url);
  return retval;
}

