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
    $Workfile: scr_ptrstack.c $

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

#include "scr_ptrstack.h"
#include "nwx_mem.h"
#include "scr_core.h"

pstack_t *new_pstack(NW_Int32  cap)
{
  pstack_t *st = (pstack_t *)NW_Mem_Malloc(sizeof(pstack_t));
  if (!st)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }

  st->s = (void **) NW_Mem_Malloc(cap * sizeof(void*));
  if (!st->s)
  {
    NW_Mem_Free(st);
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }
  st->capacity = cap;
  st->size = 0;

  return st;
}

NW_Bool is_pstack_empty(pstack_t *st)
{
  NW_ASSERT(st);
  return NW_BOOL_CAST(st->size == 0);
}

NW_Bool is_pstack_full(pstack_t *st)
{
  NW_ASSERT(st);
  return NW_BOOL_CAST(st->size == st->capacity);
}

void *pop_pstack(pstack_t *st)
{
  NW_ASSERT(st);
  if (is_pstack_empty(st))
  {
    return 0;
  }
  else
  {
    return (st->s[--st->size]);
  }
}

void *peek_pstack(pstack_t *st)
{
  NW_ASSERT(st);
  if (is_pstack_empty(st))
  {
    return 0;
  }
  else
  {
    return (st->s[st->size-1]);
  }
}

void push_pstack(pstack_t *st, void *v)
{
  NW_ASSERT(st);
  NW_ASSERT(!is_pstack_full(st));
  st->s[st->size++] = v;
}

void destroy_pstack(pstack_t *st)
{
  NW_ASSERT(st);
  NW_Mem_Free(st->s);
  st->s = 0;
}

void *el_at_pstack(pstack_t *st, NW_Int32  index)
{
  NW_ASSERT(st);
  NW_ASSERT((0 <= index) && (index < st->capacity));
  return st->s[index];
}

