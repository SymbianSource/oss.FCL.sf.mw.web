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
    $Workfile: scr_browser.c $

    Purpose:

        This file implements the ScriptServer functionality for the WMLBrowser standard library.
        The WMLBrowser library can access different WMLBrowser variables and attributes.

 */
 
#include "scr_browser.h"
#include "scr_conv.h"
#include "scr_api.h"
#include "scr_srv.h" 
#include "nwx_url_utils.h"
#include "urlloader_urlresponse.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

NW_Byte size_browser_lib(void)
{
  return NO_OF_BROWSER_FUNCS;
}

NW_Bool browser_getVar(void) 
{
  NW_Ucs2 *name = 0;
  NW_Bool retVal;

  name = GetUcs2Val();
  if (name == NULL) {
    return NW_FALSE;
  }

  /* name will be freed by the browser app */
  retVal = ScriptAPI_getVar(name);
  if (!retVal) {
    push_estack(new_invalid_val());
  }  

  NW_Mem_Free(name);
  return retVal;
}

static NW_Bool browser_setVar(void)
{
  NW_Ucs2 *name = 0, *value = 0;
  NW_Bool retVal;
  
  value = GetUcs2Val_notrim();
  if (value == NULL) {
    return NW_FALSE;
  }

  name = GetUcs2Val();
  if (name == NULL) {
    NW_Mem_Free(value);
    return NW_FALSE;
  }

  /* name and value will be freed by the browser app */
  retVal = ScriptAPI_setVar(name, value);
  if (!retVal) {
    push_estack(new_invalid_val());
  }

  NW_Mem_Free(value);
  NW_Mem_Free(name);

  return retVal;
}

NW_Bool browser_go(void) 
{
  NW_Ucs2 *url = 0;

  url = GetUcs2Val();
  if (url == NULL) {
    return NW_FALSE;
  }

  /* url and vars will be freed by the browser app */
  ScriptAPI_go(url);
  NW_Str_Delete(url);
  push_estack(new_empty_str_val());

  return NW_TRUE;
}

NW_Bool browser_prev(void) 
{
  ScriptAPI_prev();

  return NW_TRUE;
}

static NW_Bool browser_newContext(void)
{
  ScriptAPI_newContext();

  return NW_TRUE;
}

static NW_Bool browser_getCurrentCard(void)
{
  NW_Ucs2 *ref = 0;

  ref = get_first_referer();

  if (ref)
  {
    TBrowserStatusCode status;
    NW_Ucs2 *base = get_curr_url();
    NW_Ucs2 *res_str = 0;
    str_t s = 0;
    val_t v = uninitialize_val();

    status = NW_Url_AbsToRel(base, ref, &res_str);
    if (status == KBrsrOutOfMemory)
    {
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      return NW_FALSE;
    }

    s = ucs22str(res_str);
    v = new_str_val(s);
    push_estack(v);

    if (res_str) NW_Mem_Free(res_str);
    free_str(s);
    free_val(v);
  }
  else push_estack(new_invalid_val());


  return NW_TRUE;
}

static NW_Bool browser_refresh(void)
{
  ScriptAPI_refresh();

  return NW_TRUE;
}


/*  functions in Browser library */
static const lib_function_t browser_lib_table[NO_OF_BROWSER_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          browser_getVar,           NW_TRUE  },
  {   2,          browser_setVar,           NW_TRUE  },
  {   2,          browser_go,               NW_TRUE  },
  {   1,          browser_prev,             NW_TRUE  },
  {   0,          browser_newContext,       NW_TRUE  },
  {   0,          browser_getCurrentCard,   NW_FALSE },
  {   0,          browser_refresh,          NW_TRUE  }
};

void populate_browser_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(BROWSER_LIB_ID);
  (*table)[i] = browser_lib_table;

  return;
}

