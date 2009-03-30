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
    $Workfile: scr_constpool.c $

    Purpose:

        This file implements the ScriptServer functionality for the constant pool.  The
        constant pool contains all constants defined in the ScriptServer function(s).
 */

#include "scr_constpool.h"

#include "scr_api.h"
#include "scr_core.h"
#include "nwx_mem.h"


constpool_t *create_cp(NW_Int32  size, NW_Int32  charset)
{
  NW_Int32 i;
  constpool_t *cp;
  context_t *ctx = ScriptAPI_getCurrentContext();

  if (size == 0) return 0;

  cp = (constpool_t *)NW_Mem_Malloc(sizeof(constpool_t));
  if (!cp)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }
  cp->vals = (val_t *)NW_Mem_Malloc(size * sizeof(val_t));
  if (!cp->vals)
  {
    NW_Mem_Free(cp);
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return 0;
  }
  for (i=0; i<size; i++) {
    cp->vals[i] = new_invalid_val();
  }

  cp->size    = NW_UINT16_CAST(size);
  cp->charset = NW_UINT16_CAST(charset);

  set_ctx_constpool(ctx, cp);
  return cp;
}


void append_cp_int(NW_Int32  index, NW_Int32  i)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  constpool_t *cp = get_ctx_constpool(ctx);
  val_t v = new_int_val(i);
  NW_ASSERT(cp);
  NW_ASSERT((index < cp->size) && (index >= 0));
  cp->vals[index] = v;
}

void append_cp_float(NW_Int32  index, NW_Float32 f)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  constpool_t *cp = get_ctx_constpool(ctx);
  val_t v = new_float_val(f);
  NW_ASSERT(cp);
  NW_ASSERT((index < cp->size) && (index >= 0));
  cp->vals[index] = v;
}


void append_cp_string(NW_Int32  index, str_t s)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  constpool_t *cp = get_ctx_constpool(ctx);
  val_t v = new_str_val(s);
  NW_ASSERT(cp);
  NW_ASSERT((index < cp->size) && (index >= 0));
  cp->vals[index] = v;
}

val_t get_const(NW_Int32  index)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  constpool_t *cp = get_ctx_constpool(ctx);
  NW_ASSERT(cp);
  /* the range should have been checked by the caller */
  NW_ASSERT((index < cp->size) && (index >= 0));
  return cp->vals[index];
}

NW_Bool valid_const_index(NW_Int32  index)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  constpool_t *cp = get_ctx_constpool(ctx);
  NW_ASSERT(cp);

  return NW_BOOL_CAST((index < cp->size) && (index >= 0));
}

void free_cp(constpool_t *cp)
{
  NW_Int32  i;

  if (cp == NULL) {
    return;
  }

  for(i = 0; i < cp->size; i++) {
    free_val(cp->vals[i]);
  }
  NW_Mem_Free(cp->vals);
}

NW_Bool is_const_string(NW_Int32  index)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  constpool_t *cp = get_ctx_constpool(ctx);
  val_t v;

  NW_ASSERT(cp);
  if ((index >= cp->size) || (index < 0)) {
    return NW_FALSE;
  }

  v = get_const(index);

  return NW_BOOL_CAST(ScriptVarType(v) == VAL_TYPE_STR);
}
