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
    $Workfile: scr_estack.c $

    Purpose:

       This file implements the execution stack for the ScriptServer Virtual machine. 
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

#include "scr_api.h"
#include "scr_core.h"
#include "nwx_mem.h"

/* for the time being the maximum size is 20 we can make extendable
  stack later on */

NW_Bool create_estack(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  estack_t *es = (estack_t *)NW_Mem_Malloc(sizeof(estack_t));

  if (!es)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }

  es->size = 0;
  es->capacity = MAX_EXPR_DEPTH;

  es->ops = (val_t *)NW_Mem_Malloc(es->capacity * sizeof(val_t));
  if (!es->ops)
  {
    NW_Mem_Free(es);
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }


  NW_Mem_memset(es->ops, 0, es->capacity * sizeof(val_t));
  set_ctx_estack(ctx, es);
  return NW_TRUE;
}


/* Copy the val_t to the estack */
void push_estack(val_t v)
{
  val_t temp;

  temp = dup_val(v);
  push_estack_and_free(&temp);
  return;
}


/* Transfer ownership of the val_t to the estack */
void push_estack_and_free(val_t *vptr)
{
  context_t *ctx;
  estack_t *es;

  NW_ASSERT(vptr != NULL);
  NW_ASSERT(ScriptVarType(*vptr) != VAL_TYPE_UNINITIALIZED);

  ctx = ScriptAPI_getCurrentContext();
  NW_ASSERT(ctx);

  es = get_ctx_estack(ctx);
  NW_ASSERT(es);

  if (es->size >= es->capacity)
  {
    set_error_code(SCR_ERROR_STACK_OVERFLOW);
    return;
  }

  es->ops[es->size++] = *vptr;
  vptr->type     = VAL_TYPE_UNINITIALIZED;
  vptr->value.s  = NULL;
  return;
}



val_t pop_estack(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  estack_t *es = get_ctx_estack(ctx);

  if(es->size > 0) {
    return es->ops[--es->size];
  } else {
    set_error_code(SCR_ERROR_STACK_UNDERFLOW);
    return new_invalid_val();
  }
}

void pop_estack_and_free() {
  val_t val;
  val = pop_estack();
  if (!IS_INVALID(val)) {
    free_val(val);
  }
  return;
}

NW_Bool is_estack_empty(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  estack_t *es = get_ctx_estack(ctx);
  NW_ASSERT(es);
  return NW_BOOL_CAST(es->size == 0);
}

void free_estack(estack_t *st)
{
  NW_Int32  i;

  for(i = 0; i < st->size; i++)
  {
    if (ScriptVarType(st->ops[i]) != VAL_TYPE_UNINITIALIZED) free_val(st->ops[i]);
  }
  NW_Mem_Free(st->ops);
}

