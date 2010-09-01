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
    $Workfile: scr_context.c $

    Purpose:

        This file implements the functionality needed for the ScriptServer context.  
        The context is made up of:  the application, the execution stack and the
        frame stack.
 */

#include "scr_context.h"
#include "scr_core.h"
#include "nwx_mem.h"

/* interval for the script interpreter to yield. */
#define SUSPEND_INTERVAL  40000

void set_ctx_state(context_t *ctx, ctx_state_e state)
{
  NW_ASSERT(ctx);
  ctx->state = state;
}

ctx_state_e get_ctx_state(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->state;
}

void clear_context(context_t *ctx)
{
  NW_ASSERT(ctx);
  NW_ASSERT(ctx->cs == 0);

  if (ctx->es)
  {
    free_estack(ctx->es);
    NW_Mem_Free(ctx->es);
    ctx->es = 0;
  }

  if (ctx->fs)
  {
    free_frstack(ctx->fs);
    NW_Mem_Free(ctx->fs);
    ctx->fs = 0;
  }

  clear_url_context(ctx);

  ctx->access_path = 0;
  ctx->access_domain = 0;
  ctx->access_public = NW_FALSE;

  ctx->state = IDLE;

  ctx->loadStringMime = 0;
  ctx->errorCode = SCR_ERROR_NO_ERROR;

  ctx->suspend_counter = 0;

  if ( ctx->msg != NULL )
    NW_Mem_Free( ctx->msg );

  ctx->msg = NULL;
}

void clear_url_context(context_t *ctx)
{
  NW_ASSERT(ctx);
  if (ctx->cp)
  {
    free_cp(ctx->cp);
    NW_Mem_Free(ctx->cp);
    ctx->cp = 0;
  }

  if (ctx->fp)
  {
    free_funcpool(ctx->fp);
    NW_Mem_Free(ctx->fp);
    ctx->fp = 0;
  }

  if (ctx->fn)
  {
    free_fname_table(ctx->fn);
    NW_Mem_Free(ctx->fn);
    ctx->fn = 0;
  }
}

constpool_t *get_ctx_constpool(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->cp;
}

void set_ctx_constpool(context_t *ctx, constpool_t *cp)
{
  NW_ASSERT(ctx);
  ctx->cp = cp;
}

funcpool_t *get_ctx_funcpool(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->fp;
}

void set_ctx_funcpool(context_t *ctx, funcpool_t *fp)
{
  NW_ASSERT(ctx);
  ctx->fp = fp;
}

fname_table_t *get_ctx_fname_table(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->fn;
}

void set_ctx_fname_table(context_t *ctx, fname_table_t *fn)
{
  NW_ASSERT(ctx);
  ctx->fn = fn;
}

estack_t *get_ctx_estack(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->es;
}

void set_ctx_estack(context_t *ctx, estack_t *es)
{
  NW_ASSERT(ctx);
  ctx->es = es;
}

frstack_t *get_ctx_frstack(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->fs;
}

void set_ctx_frstack(context_t *ctx, frstack_t *fs)
{
  NW_ASSERT(ctx);
  ctx->fs = fs;
}

NW_Bool set_ctx_call_str(context_t *ctx, NW_Ucs2 *fname, NW_Byte args)
{
  /* cs is freed when calling delete_call_str in URLLoadResp */
  call_str *cs = (call_str*) NW_Mem_Malloc(sizeof(call_str));

  if (!cs)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }

  cs->fname = fname;
  cs->args = args;

  NW_ASSERT(ctx);
  ctx->cs = cs;
  return NW_TRUE;
}

NW_Ucs2 *get_ctx_fname(context_t *ctx)
{
  NW_ASSERT(ctx && ctx->cs);
  return ctx->cs->fname;
}

NW_Uint8 get_ctx_args_no(context_t *ctx)
{
  NW_ASSERT(ctx && ctx->cs);
  return ctx->cs->args;
}


NW_Bool was_calling(context_t *ctx)
{
  NW_ASSERT(ctx);
  return NW_BOOL_CAST(ctx->cs != 0);
}

void delete_call_str(context_t *ctx)
{
  NW_ASSERT(ctx);
  if (ctx->cs) {
    NW_Mem_Free(ctx->cs->fname);
    NW_Mem_Free(ctx->cs);
    ctx->cs = NULL;
  }

}

NW_Bool find_function_index(context_t *ctx, NW_Ucs2 *fname, NW_Byte *index)
{
  NW_ASSERT(ctx);
  return fname_find_function_index(ctx->fn, fname, index);
}

void set_ctx_error_code(context_t *ctx, NW_Byte b)
{
  NW_ASSERT(ctx);
  if (ctx->errorCode == SCR_ERROR_NO_ERROR) {
    ctx->errorCode = b;
  }
}

NW_Byte get_ctx_error_code(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->errorCode;
}

void set_ctx_error_msg(context_t *ctx, NW_Ucs2* msg)
{
  NW_ASSERT(ctx);
  if (ctx->msg == NULL) {
    ctx->msg = msg;
  }
}
NW_Ucs2* get_ctx_error_msg(context_t *ctx)
{
  NW_ASSERT(ctx);
  return ctx->msg;
}

/*
    Function: should_suspend

    Purpose: Calculates whether the script interpreter has run for
             a while. If so, the interpreter should be suspended to
             allow other applications to have a go.

    Parameters:
             ctx: Scripting context

    Returns:
             NW_TRUE if it is time to suspend script interpretation,
             NW_FALSE otherwise
*/
NW_Bool should_suspend(context_t *ctx)
{
  if (++ctx->suspend_counter > SUSPEND_INTERVAL)
  {
    ctx->suspend_counter = 0;
    return NW_TRUE;
  }
  else
  {
    return NW_FALSE;
  }
}
