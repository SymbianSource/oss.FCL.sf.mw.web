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
    $Workfile: scr_frstack.c $

    Purpose:

       This file implements the frame stack for the ScriptServer Virtual machine. 
       The frame stack stores current runtime values of a routine's formal 
       parameters and local variables, and keep track of intermediate values 
       during expression evaluation.  The ScriptServer Interpreter also uses 
       the runtime stack to keep track of where to return to when the currently 
       executing function returns, and for a function call the return value.  
       The frame stack is allocated at the beginning of the execution of the 
       program.  Each time we call a stack we push a stack frame for that 
       function onto the stack, and we pop up the frame when we return from the 
       function.  A pointer (frame stack pointer) always points to the base 
       (first component) of the current stack frame.  The current stack frame 
       is always the topmost one, and it represents the currently executing 
       routine.

 */

#include "scr_frstack.h"
#include "scr_context.h"
#include "scr_api.h"
#include "scr_core.h"

#include "nwx_mem.h"
#include "nwx_string.h"


frame_t *get_curr_frame(void);
frame_t *get_first_frame(void);
void destroy_all_frames(void);

function_t *get_curr_func(void)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);

  return get_func(frame->func_index);
}

NW_Int16 get_curr_func_size(void)
{
  frame_t *frame = get_curr_frame();
  function_t *f = 0;

  NW_ASSERT(frame);
  f = get_func(frame->func_index);
  NW_ASSERT(f);

  return get_code_size(f);
}

void init_frames(void)
{
  frstack_t *fs = (frstack_t *)new_pstack(MAX_FRAME_SIZE);
  context_t *ctx = ScriptAPI_getCurrentContext();
  set_ctx_frstack(ctx, fs);
}

NW_Bool push_frame(NW_Ucs2 *url, NW_Ucs2 *postfields, NW_Uint32 chkSum, 
                   NW_Int32 func_index, NW_Ucs2 *referer)
{
  context_t  *ctx = ScriptAPI_getCurrentContext();
  NW_Int16   vars = 0, args = 0;  /* Make NW_Byte to avoid compiler warnings. */
  NW_Int32   i;
  frame_t    *frame = 0;
  function_t *f;
  frstack_t  *fs;

  NW_ASSERT(ctx);
  fs = get_ctx_frstack(ctx);
  NW_ASSERT(fs);

  if (!valid_func_index(func_index))
  {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    return NW_FALSE;
  }

  if (is_pstack_full(fs))
  {
    set_error_code(SCR_ERROR_STACK_OVERFLOW);
    return NW_FALSE;
  }

  f = get_func(func_index);
  NW_ASSERT(f != NULL);

  vars = no_of_local_vars(f);
  args = no_of_args(f);

  frame = (frame_t *)NW_Mem_Malloc(sizeof(frame_t));
  if (!frame)
  {
    NW_Mem_Free(frame);
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }
  frame->var_cnt = NW_INT16_CAST(vars + args);

  if (frame->var_cnt > 0)
  {
    frame->vars = (val_t *)NW_Mem_Malloc(frame->var_cnt * sizeof(val_t));
    if (!frame->vars)
    {
      NW_Mem_Free(frame);
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      return NW_FALSE;
    }
  }
  else {
    frame->vars = 0;
  }
  frame->code_offset = 0;
  
  /*
  ** if the new function was called from outside (e.g. from a WML
  ** card) then we take the argument from the stack, otherwise the args
  ** will be set by parse_argument_list
  */
  if (!is_frstack_empty()) {
    for(i = 0; i < args; i++) {
      frame->vars[args - i - 1] = pop_estack();
    }
  } else {
    for(i = 0; i < args; i++) {
      frame->vars[args - i - 1] = uninitialize_val();
    }
  }

  /* local vars and args are pointing to empty var construct*/
  for(i = 0; i < vars; i++) {
    frame->vars[args+i] = new_empty_str_val();
  }

  frame->func_index = NW_UINT8_CAST(func_index);

  if (url)
  {
    if (referer) {
      frame->referer = referer;
    } else {
      frame->referer = get_curr_url();
    }
    frame->url = url;
  }
  else
  {
    NW_ASSERT(!referer);
    NW_ASSERT(!is_frstack_empty());
    NW_ASSERT(!postfields);
    frame->url     = get_curr_url();
    frame->referer = get_curr_referer();
    postfields     = get_curr_postfields();
    chkSum         = get_curr_chkSum();
  }

  frame->postfields = postfields;
  frame->chkSum     = chkSum;
  push_pstack(fs, frame);
 
  return NW_TRUE;
}


void NW_Wmls_PopFrame(void)
{
  frame_t *fr;
  context_t *ctx = ScriptAPI_getCurrentContext();
  frstack_t *fs = get_ctx_frstack(ctx);
  NW_Int32  i, no_locals;

  NW_ASSERT(fs);
  fr = (frame_t *)pop_pstack(fs);
  NW_ASSERT(fr);
 
  /* free variables and args */
  no_locals = fr->var_cnt;
  for(i = 0; i < no_locals; i++) {
    free_val(fr->vars[i]);
  }

  if (fr->vars) {
    NW_Mem_Free(fr->vars);
  }

  /* free url unless the function was called from the same file */

  if (is_frstack_empty())
  {
    NW_Mem_Free(fr->url);
    NW_Mem_Free(fr->referer);
    NW_Mem_Free(fr->postfields);
    fr->url        = NULL;
    fr->referer    = NULL;
    fr->postfields = NULL;

  }
  else
  {
    NW_Ucs2 *new_url = get_curr_url();
    if (fr->url != new_url)
    {
      NW_Mem_Free(fr->url);
      NW_Mem_Free(fr->postfields);
      fr->url        = NULL;
      fr->postfields = NULL;
    }
  }

  NW_Mem_Free(fr);
}

NW_Uint16 get_code_offset(void)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);

  return frame->code_offset;
}


void set_code_offset(NW_Uint16 offset)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);

  frame->code_offset = offset;
}

void jump_opcode(NW_Int32  offset)
{
  frame_t *frame = get_curr_frame();
  function_t *f = 0;

  NW_ASSERT(frame);

  f = get_func(frame->func_index);
  NW_ASSERT(f);

  frame->code_offset = NW_INT16_CAST(frame->code_offset + offset);

  if ((frame->code_offset < 0) || (frame->code_offset > f->code_size)) {
    set_error_code(SCR_ERROR_VERIFICATION_FAILED);
  }
}

NW_Bool get_opcode(NW_Byte *b)
{
  frame_t *frame = get_curr_frame();
  function_t *f = 0;

  NW_ASSERT(frame);
  NW_ASSERT(b != NULL);

  f = get_func(frame->func_index);
  if (!f) {
    return NW_FALSE;
  }

  /* also validate the index */
  return fu_get_opcode(frame->func_index, frame->code_offset, b);
}


/* can be optimize as macro */
NW_Bool get_next_opcode_byte(NW_Byte *b)
{
  jump_opcode(1);
  return get_opcode(b);
}

NW_Bool get_next_opcode_int16(NW_Uint16 *i16)
{
  NW_Byte b1, b2;
  jump_opcode(1);
  if (!get_opcode(&b1)) {
    return NW_FALSE;
  }

  jump_opcode(1);

  if (!get_opcode(&b2)) {
    return NW_FALSE;
  }

  *i16 = NW_UINT16_CAST((b1 << 8) + b2);

  return NW_TRUE;
}


NW_Bool is_frstack_empty(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();

  frstack_t *fs = get_ctx_frstack(ctx);
  NW_ASSERT(fs);
  return is_pstack_empty(fs);
}


val_t get_func_var(NW_Byte i)
{
  frame_t *frame = get_curr_frame();
  function_t *f;
  NW_Int32  var_size, arg_size;

  NW_ASSERT(frame);

  f = get_func(frame->func_index);
  NW_ASSERT(f);

  var_size = no_of_local_vars(f);
  arg_size = no_of_args(f);

  if (NW_UINT8_CAST(i) >= var_size + arg_size)
  {
    set_error_code(SCR_ERROR_STACK_UNDERFLOW);
    return new_invalid_val();
  }

  return frame->vars[i];
}

void set_func_var(NW_Byte i, val_t v)
{
  frame_t *frame = get_curr_frame();
  function_t *f;
  NW_Int32  var_size, arg_size;

  NW_ASSERT(frame);

  f = get_func(frame->func_index);
  NW_ASSERT(f);

  var_size = no_of_local_vars(f);
  arg_size = no_of_args(f);

  if (NW_UINT8_CAST(i) >= var_size + arg_size)
  {
    set_error_code(SCR_ERROR_STACK_UNDERFLOW);
    return;
  }

  free_val(frame->vars[i]);
  frame->vars[i] = dup_val(v); 
}

NW_Ucs2 *get_curr_url(void)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);
  return frame->url;
}

NW_Uint32 get_curr_chkSum(void)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);
  return frame->chkSum;
}

NW_Ucs2 *get_curr_postfields(void)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);
  return frame->postfields;
}

NW_Ucs2 *get_curr_referer(void)
{
  frame_t *frame = get_curr_frame();
  NW_ASSERT(frame);
  return frame->referer;
}

NW_Ucs2 *get_first_referer(void)
{
  frame_t *frame = get_first_frame();
  NW_ASSERT(frame);
  return frame->referer;
}

void free_frstack(frstack_t *st)
{
  free_all_frames();  /*free all frames at the end of NW_Script_Finishnodialogresp, by clear_context */
  destroy_pstack(st);
}

frame_t *get_curr_frame(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  frstack_t *fs = get_ctx_frstack(ctx);
  NW_ASSERT(fs);

  return (frame_t *)peek_pstack(fs);
}

frame_t *get_first_frame(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  frstack_t *fs = get_ctx_frstack(ctx);

  NW_ASSERT(fs);

  return (frame_t *)el_at_pstack(fs, 0);
}

/* frees all the frames in the stack. the current context should be
freed with NW_Wmls_PopFrame before calling this function */
void destroy_all_frames(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  frstack_t *fs = get_ctx_frstack(ctx);
  frame_t *fr = NULL;
  NW_Ucs2 *prev_url = NULL, *prev_referer = NULL;
  NW_Int16 index=0; /* Moved from Wave */

  fr = (frame_t *)peek_pstack(fs);
  if(is_frstack_empty())
  {
    NW_Mem_Free(fr);
    return;
  }

  prev_url = fr->url;
  prev_referer = fr->referer;

  while ((fr = (frame_t *)pop_pstack(fs)) != NULL) {

    if (prev_url != fr->url) {
      NW_Mem_Free(prev_url);
      prev_url = fr->url;
    }

    if (prev_referer != fr->referer) {
      NW_Mem_Free(prev_referer);
      prev_referer = fr->referer;
    }

    /* Commented out inorder to move changes from Wave, following 3 lines. */
	  index = fr->var_cnt; /* index is 0 based */
	  for (index = 0; index < fr->var_cnt; index++) {
		  free_val(fr->vars[index]);
	  }
    
    /* Free the memory */
    if (fr->vars) {
      NW_Mem_Free(fr->vars);
      fr->vars = NULL;
    }

    NW_Mem_Free(fr);
    fr = NULL;
  }

  if (prev_url) {
    NW_Mem_Free(prev_url);
    prev_url = NULL;
  }

  if (prev_referer) {
    NW_Mem_Free(prev_referer);
    prev_referer = NULL;
  }

#if 0
  while (!is_frstack_empty()) {
    fr = (frame_t *)pop_pstack(fs);

    if (!is_frstack_empty() && first) {
      first = NW_FALSE;
      if (fr->url) {
        NW_Mem_Free(fr->url);
        fr->url = NULL;
      }

      if (fr->referer) {
        NW_Mem_Free(fr->referer);
        fr->referer = NULL;
      }
      if (fr) {
        NW_Mem_Free(fr);
        fr = NULL;
      }
    }

    if (fr) {
      NW_Mem_Free(fr);
      fr = NULL;
    }
  }/*end of while (!is_frstack_empty()) */
#endif
  return;
}

/* destroys all the frames in the stack */
void free_all_frames(void)
{

  while (!is_frstack_empty())
  {
    /*deep copy the prev url because NW_Wmls_PopFrame will free the prev_url if there is a context switch*/
    NW_Ucs2 *prev_url = get_curr_url();
    NW_Ucs2 *prev_url_cpy = NULL;
    NW_Ucs2 *url = NULL;

    NW_ASSERT(prev_url);
    prev_url_cpy = NW_Str_Newcpy(prev_url);
    if (prev_url == NULL) {
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      return;
    }

    NW_Wmls_PopFrame();

    if (!is_frstack_empty()) {
      url = get_curr_url();
      NW_ASSERT(url);
      if (!NW_Str_Strcmp(url, prev_url_cpy) == 0) /*are not equal*/
      {
        /* context switch encountered, we cannot use "NW_Wmls_PopFrame" to free the frames
        as the script context is not updated. Note: when a context switch happens the current
        implementation reloads the content from the cache thereby updating the context.
        */
        destroy_all_frames();
      }
    }
    if (prev_url_cpy)
    {
      NW_Mem_Free(prev_url_cpy);
      prev_url_cpy = NULL;
    }
  } /*end of while (!is_frstack_empty()) */
  return;
}
