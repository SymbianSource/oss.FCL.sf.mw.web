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
   $Workfile: scr_inter.c $

    Purpose:

       Loop through each bytecode in a frame that contains the function 
       being executed.
 */

#include "scr_frstack.h"
#include "scr_estack.h"
#include "scr_opcodes.h"
#include "scr_handle.h"
#include "scr_frstack.h"
#include "scr_inter.h"
#include "scr_context.h"
#include "scr_api.h"
#include "scr_core.h"

#include "nwx_string.h"
#include "nwx_url_utils.h"

/* for optimization all the handle function can be later 
   on injected to the interprete to save a function call
 */
void interprete()
{
  NW_Byte opcode, impl_parameter;
  NW_Int32  code_offset;
  context_t *ctx = ScriptAPI_getCurrentContext();

  set_ctx_state(ctx, RUNNING);

  for(;;)
  {
    NW_Byte err_code = get_error_code();
    if (err_code != SCR_ERROR_NO_ERROR)
    {
      ScriptAPI_finish(err_code, get_error_msg());
      return;
    }

    if (should_suspend(ctx))/* should the interpreter yield */
    {
      set_ctx_state(ctx, SUSPEND);
      ScriptAPI_suspend();
      return;
    }

    code_offset = get_code_offset();

    if (code_offset < 0)
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      continue;
    }

    if (code_offset >= get_curr_func_size())
    {
      /* Walked off end of function.  WMLScript 9.4.3 says */
      /* treat as a return of empty string. */
      opcode = RETURN_ES;
    }
    else if (!get_opcode(&opcode))
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      continue;
    }

    parse_opcode(&opcode, &impl_parameter);

    switch(opcode) {

     case JUMP_FW_S:
      handle_jump_fw_s(impl_parameter);
      break;

     case JUMP_FW:
      handle_jump_fw();
      break;

     case JUMP_FW_W:
      handle_jump_fw_w();
      break;

     case JUMP_BW_S:
      handle_jump_bw_s(impl_parameter);
      break;

     case JUMP_BW: /*jump Backward*/
      handle_jump_bw();
      break;

     case JUMP_BW_W: /*jump Backward*/
      handle_jump_bw_w();
      break;

     case TJUMP_FW_S: /*jumpFowrward on false*/
      handle_tjump_fw_s(impl_parameter);
      break;

     case TJUMP_FW: /*jumpFowrward on False*/
      handle_tjump_fw();
      break;

     case TJUMP_FW_W: /*jumpFowrward on false*/
      handle_tjump_fw_w();
      break;

     case TJUMP_BW:
      handle_tjump_bw();
      break;

     case TJUMP_BW_W:
      handle_tjump_bw_w();
      break;

     case CALL_S:

      handle_call_s(impl_parameter);
      break;

     case CALL:
      handle_call();
      break;
      
      /* for the CALL_LIB*: if the called return NW_FALSE or the call
      return NW_TRUE and is assync we return.
      Lang.exit() and Lang.abort() return NW_FALSE.
      Async functions that fail (no need to send/receive a message)
      will return NW_FALSE (e.g. WMLBrowser.go()
      */
     case CALL_LIB_S:
      {
        NW_Bool async;

        if (handle_call_lib_s(impl_parameter, &async) && async) return;
        else  break;
      }

     case CALL_LIB:
      {
        NW_Bool async;

        if (handle_call_lib(&async) && async) return;
        else break;
      }

     case CALL_LIB_W:
      {
        NW_Bool async;
        if (handle_call_lib_w(&async) && async) return;
        else break;
      }

     case CALL_URL_W:
      if(handle_call_url_w())
          return;
      else
          break;
      

     case CALL_URL:
      if(handle_call_url())
          return;
        else
          break;
      
     case LOAD_VAR_S:
      handle_load_var_s((NW_Byte)impl_parameter);
      break;

     case LOAD_VAR:
      handle_load_var();
      break;

     case STORE_VAR_S:
      handle_store_var_s((NW_Byte)impl_parameter);
      break;

     case STORE_VAR:
      handle_store_var();
      break;

     case LOAD_CONST_S:
      handle_load_const_s((NW_Byte)impl_parameter);
      break;

     case LOAD_CONST:
      handle_load_const();
      break;

     case LOAD_CONST_W:
      handle_load_const_w();
      break;

     case INCR_VAR_S:
      handle_incr_var_s((NW_Byte)impl_parameter);
      break;

     case INCR_VAR:
      handle_incr_var();
      break;

     case DECR_VAR:
      handle_decr_var();
      break;

     case INCR:
      handle_incr();
      break;

     case DECR:
      handle_decr();
      break;

     case ADD_ASG:
      handle_add_asg();
      break;

     case SUB_ASG:
      handle_sub_asg();
      break;

     case UMINUS:
      handle_uminus();
      break;

     case ADD:
      handle_add();
      break;

     case SUB:
      handle_sub();
      break;

     case MUL:
      handle_mul();
      break;

     case DIV:
      handle_div();
      break;

     case IDIV:
      handle_idiv();
      break;

     case REM:
      handle_rem();
      break;

     case B_AND:
      handle_b_and();
      break;

     case B_OR:
      handle_b_or();
      break;

     case B_XOR:
      handle_b_xor();
      break;

     case B_LSHIFT:
      handle_lshift();
      break;

     case B_RSSHIFT:
      handle_rsshift();
      break;

     case B_RSZSHIFT:
      handle_rszshift();
      break;

     case EQ:
      handle_comp(EQ);
      break;

     case LE:
      handle_comp(LE);
      break;

     case LT:
      handle_comp(LT);
      break;

     case GE:
      handle_comp(GE);
      break;

     case GT:
      handle_comp(GT);
      break;

     case NE:
      handle_comp(NE);
      break;

     case L_NOT:  /*or bang*/
      handle_l_not();
      break;

     case B_NOT:  /* or tilde*/
      handle_b_not();
      break;

     case POP:
      handle_pop();
      break;

     case CONST_0:
      handle_load_const_int(0);
      break;

     case CONST_1:
      handle_load_const_int(1);
      break;

     case CONST_M1:
      handle_load_const_int(-1);
      break;

     case CONST_ES:
      handle_load_const_es();
      break;

     case CONST_INVALID:
      handle_load_invalid();
      break;

     case CONST_TRUE:
      handle_load_const_bool(NW_TRUE);
      break;

     case CONST_FALSE:
      handle_load_const_bool(NW_FALSE);
      break;

     case TYPEOF:
      handle_typeof();
      break;

     case ISVALID:
      handle_isvalid();
      break;

     case SCAND:
      handle_scand();
      break;

     case SCOR:
      handle_scor();
      break;

     case TOBOOL:
      handle_tobool();
      break;


     case RETURN_ES:

      push_estack(new_empty_str_val());

      /* fall through */
      /*lint -fallthrough*/

     case RETURN:
      {
        NW_Ucs2 *prev_url, *url;

        NW_ASSERT(!is_estack_empty());

        prev_url = NW_Str_Newcpy(get_curr_url());

        NW_Wmls_PopFrame();

        if (is_frstack_empty())
        {
          NW_Str_Delete(prev_url);
          ScriptAPI_finish(SCR_ERROR_NO_ERROR, 0);
          return;
        }

        url = get_curr_url();
        if (NW_Str_Strcmp(url,prev_url)) 
        {
          /*
          ** changed url, so reload previous content.  when URLLoadResp will
          ** return control to the script the previous context will be
          ** restored
          */
          NW_Str_Delete(prev_url);
          clear_url_context(ctx);
          set_ctx_state(ctx, RELOAD);
          ScriptAPI_reload();

          return;
        }

        NW_Str_Delete(prev_url);
        break;
      }

     default:
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
      break;
    }
    
  }
  
  /* The script finishes. Leave error code to be reported. */
  /* ScriptAPI_finish(get_error_code(), 0); */

}


