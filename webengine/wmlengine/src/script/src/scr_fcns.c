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
    $Workfile: scr_fcns.c $

    Purpose:

        Implements the function pool for the ScriptServer.  WMLReader reads the 
        functions and adds them to the function pool.  The function pool is an 
        array of pointers.  There are as many numbers of functionpool as there 
        are functions in a deck.
 */

#include "scr_fcns.h"
#include "scr_api.h"
#include "scr_core.h"

#include "nwx_mem.h"
#include "nwx_string.h"

NW_Bool create_func_pool(NW_Int32  c)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  funcpool_t *fp = 0;

  if (c <= 0) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return NW_FALSE;
  }

  fp = (funcpool_t *)NW_Mem_Malloc(sizeof(funcpool_t));
  if (!fp)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }
  fp->funcs = (function_t **)NW_Mem_Malloc(c * sizeof(function_t *));
  if (!fp->funcs)
  {
    NW_Mem_Free(fp);
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }
  fp->capacity = c;
  fp->size = 0;
  set_ctx_funcpool(ctx, fp);
  return NW_TRUE;
}


void add_func_fp(function_t *f)
{
  context_t *ctx;
  funcpool_t *fp;

  ctx  = ScriptAPI_getCurrentContext();
  NW_ASSERT(ctx);

  fp = get_ctx_funcpool(ctx);
  NW_ASSERT(fp);
  NW_ASSERT(fp->size < fp->capacity);

  fp->funcs[fp->size++] = f;
}

NW_Bool create_fname_table(NW_Int32  c)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  fname_table_t *fnames = 0;

  NW_ASSERT(ctx);
  if (c <= 0) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return NW_FALSE;
  }
  fnames = (fname_table_t *)NW_Mem_Malloc(sizeof(fname_table_t));
  if (!fnames)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }
  fnames->names = (fname_t *)NW_Mem_Malloc(c * sizeof(fname_t));
  if (!fnames->names)
  {
    NW_Mem_Free(fnames);
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }
  fnames->capacity = c;
  fnames->size = 0;
  set_ctx_fname_table(ctx, fnames);
  return NW_TRUE;
}


void add_fname(NW_Byte index, NW_Ucs2 *name)
{
  context_t *ctx;
  fname_table_t *fnames;

  ctx = ScriptAPI_getCurrentContext();
  NW_ASSERT(ctx);

  fnames = get_ctx_fname_table(ctx);
  NW_ASSERT(fnames);

  NW_ASSERT(fnames->size < fnames->capacity);
  fnames->names[fnames->size].index = index;
  fnames->names[fnames->size].name = name;
  fnames->size++;
}

function_t *get_func(NW_Int32  funcIndex)
{
  context_t *ctx;
  funcpool_t *fp;
  function_t *f = 0;

  ctx = ScriptAPI_getCurrentContext();
  NW_ASSERT(ctx);

  fp = get_ctx_funcpool(ctx);
  NW_ASSERT(fp);

  if ((funcIndex < 0) || (funcIndex > fp->size)) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return NULL;
  }

  f = fp->funcs[funcIndex];
  return f;
}

NW_Byte no_of_local_vars(function_t *f)
{
  if (f == NULL) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return 0;
  }
  return f->nbr_of_local_vars;
}

NW_Byte no_of_args(function_t *f)
{
  if (f == NULL) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return 0;
  }
  return f->nbr_of_args;
}

NW_Uint16 get_code_size(function_t *f)
{
  if (f == NULL) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return 0;
  }
  return f->code_size;
}


NW_Bool fu_get_opcode(NW_Byte func_index, NW_Uint16 offset, NW_Byte *b)
{
  function_t *f = 0;
  context_t *ctx;
  funcpool_t *fp;

  NW_ASSERT(b != NULL);

  ctx = ScriptAPI_getCurrentContext();
  NW_ASSERT(ctx);

  fp = get_ctx_funcpool(ctx);
  NW_ASSERT(fp);

  if (NW_UINT8_CAST(func_index) >= fp->size) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return NW_FALSE;
  }

  f = fp->funcs[func_index];

  if (offset >= f->code_size) {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return NW_FALSE;
  }

  *b = f->code[offset];
  return NW_TRUE;
}

NW_Bool fname_find_function_index(fname_table_t *fnames, NW_Ucs2 *fname, NW_Byte *index)
{
  NW_Int32  i = 0;

  if (!fnames) {
    return NW_FALSE;
  }

  for(; i < fnames->size; i++)
  {
    if (!NW_Str_Strcmp( fnames->names[i].name, fname))
    {
      *index = fnames->names[i].index;
      return NW_TRUE;
    }
  }
  return NW_FALSE;
}

void free_func(function_t *f)
{
  if (f != NULL) {
    NW_Mem_Free(f->code);
  }
}

void free_funcpool(funcpool_t *fp){
  NW_Int32  i;

  NW_ASSERT(fp);

  for(i = 0; i < fp->size; i++)
  {
    free_func(fp->funcs[i]);
    NW_Mem_Free(fp->funcs[i]);
  }
  NW_Mem_Free(fp->funcs);
}

void free_fname(fname_t *fn)
{
  NW_ASSERT(fn);
  NW_Mem_Free(fn->name);
}

void free_fname_table(fname_table_t *fn_table)
{
  NW_Int32  i;

  if (!fn_table) return;

  for (i = 0; i < fn_table->size; i++) {
    free_fname(&fn_table->names[i]);
  }
  NW_Mem_Free(fn_table->names);
}

NW_Bool valid_func_index(NW_Int32  index)
{
  context_t *ctx;
  funcpool_t *fp;

  ctx = ScriptAPI_getCurrentContext();
  NW_ASSERT(ctx);

  fp = get_ctx_funcpool(ctx);
  NW_ASSERT(fp);

  return NW_BOOL_CAST((index >= 0) && (index < fp->size));
}

