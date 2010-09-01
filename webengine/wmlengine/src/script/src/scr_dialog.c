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
    $Workfile: scr_dialog.c $

    Purpose:

        This file implements the ScriptServer functionality for the Dialog standard library.
        The Dialog library contains a set of typical user interface functions.

 */

#include "scr_dialog.h"
#include "scr_conv.h"
#include "scr_api.h"
#include "scr_srv.h" 
#include "nwx_mem.h"


/*#define NO_OF_DIALOG_FUNCS 3
static lib_function_t dialog_funcs[NO_OF_DIALOG_FUNCS];*/

NW_Byte size_dialog_lib(void)
{
  return NO_OF_DIALOG_FUNCS;
}

static NW_Bool dialog_prompt(void)
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val();
  NW_Ucs2 *message = 0, *defaultInput = 0;

  v2 = pop_estack();
  v1 = pop_estack();

  if ((ScriptVarType(v1) == VAL_TYPE_INVALID) || (ScriptVarType(v2) == VAL_TYPE_INVALID) )
  {
    push_estack(new_invalid_val());
    free_val(v1);
    free_val(v2);
    return NW_FALSE;
  }

  message = val2ucs2(v1);
  defaultInput = val2ucs2(v2);

  if (!message || !defaultInput)
  {
    free_val(v1);
    free_val(v2);
    NW_Mem_Free(message);
    NW_Mem_Free(defaultInput);
    return NW_FALSE;
  }


  NW_WmlsDialog_Prompt(message, defaultInput);

  /* ustr1 will be cleaned when the dialog returns */
  NW_Mem_Free(defaultInput);
  NW_Mem_Free(message);
  free_val(v1);
  free_val(v2);

  return NW_TRUE;
}

static NW_Bool dialog_confirm(void)
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val(), v3 = uninitialize_val();
  NW_Ucs2 *message = 0, *yes_str = 0, *no_str = 0;

  v3 = pop_estack();
  v2 = pop_estack();
  v1 = pop_estack();

  if ((ScriptVarType(v1) == VAL_TYPE_INVALID) || (ScriptVarType(v2) == VAL_TYPE_INVALID)
      || (ScriptVarType(v2) == VAL_TYPE_INVALID) )
  {
    push_estack(new_invalid_val());
    free_val(v1);
    free_val(v2);
    free_val(v3);
    return NW_FALSE;
  }

  message = val2ucs2(v1);
  yes_str = val2ucs2(v2);
  no_str = val2ucs2(v3);

  if (!message || !yes_str || !no_str)
  {
    free_val(v1);
    free_val(v2);
    free_val(v3);
    NW_Mem_Free(message);
    if (yes_str) NW_Mem_Free(yes_str);
    if (no_str) NW_Mem_Free(no_str);
    return NW_FALSE;
  }

  NW_WmlsDialog_Confirm(message, yes_str, no_str);

  NW_Mem_Free(message);
  NW_Mem_Free(yes_str);
  NW_Mem_Free(no_str);

  free_val(v1);  free_val(v2);  free_val(v3);

  return NW_TRUE;
}

static NW_Bool dialog_alert(void)
{
  val_t v = uninitialize_val();
  NW_Ucs2 *message = 0;

  v = pop_estack();

  if ((ScriptVarType(v) == VAL_TYPE_INVALID))
  {
    push_estack(new_invalid_val());
    free_val(v);
    return NW_FALSE;
  }

  message = val2ucs2(v);
  if (!message)
  {
    free_val(v);
    return NW_FALSE;
  }

  NW_WmlsDialog_Alert(message);

  free_val(v);

  NW_Mem_Free(message);

  return NW_TRUE;
}


/*  functions in Dialog library */
static const lib_function_t dialog_lib_table[NO_OF_DIALOG_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   2,          dialog_prompt,            NW_TRUE  },
  {   3,          dialog_confirm,           NW_TRUE  },
  {   1,          dialog_alert,             NW_TRUE  }
};

void populate_dialog_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(DIALOG_LIB_ID);
  (*table)[i] = dialog_lib_table;

  return;
}
