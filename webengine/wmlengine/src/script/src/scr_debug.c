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
    $Workfile: scr_debug.c $

    Purpose:
    Propriety Debug library
 */
   
#include "scr_debug.h"
#include "scr_estack.h"
#include "scr_conv.h"
#include "scr_api.h"
#include "scr_core.h"
#include "nwx_mem.h"

/*#define NO_OF_DEBUG_FUNCS 3
static lib_function_t debug_funcs[NO_OF_DEBUG_FUNCS];*/

NW_Byte size_debug_lib(void)
{
  return NO_OF_DEBUG_FUNCS;
}

#ifdef WAE_DEBUG

static FILE *fp = 0;

static NW_Bool debug_openFile(void)
{
  NW_Bool retval = NW_TRUE;
  val_t v1 = 0, v2 = 0;
  str_t str1 = 0;
  str_t str2 = 0;
  
  v2 = pop_estack();
  v1 = pop_estack();

  if (val2str(v1, &str1) && val2str(v2, &str2))
  {
    char *fname = str2ascii(str1);
    char *mode = str2ascii(str2);
    if (fname && mode)
    {
      if (fp) fclose(fp);
      fp = fopen(fname, mode);
      if (!fp) printf("Cannot open file %s with mode %s\n",fname, mode);
      NW_Mem_Free(fname);
      NW_Mem_Free(mode);
    }
    else retval = NW_FALSE;

  }
  else
  {
    NW_ASSERT(NW_FALSE);
    retval = NW_FALSE;
  }

  push_estack(new_empty_str_val());
  free_str(str1);  free_str(str2);
  free_val(v1); free_val(v2);

  return retval;
}

static NW_Bool debug_println(void)
{
  NW_Bool retval = NW_TRUE;
  val_t v = 0;
  str_t str = 0;
  
  v = pop_estack();

  if (val2str(v, &str))
  {
    char *s = str2ascii(str);
    if (s)
    {
      if (fp) fprintf(fp, "%s\n", s);
      else printf("%s\n", s);
      NW_Mem_Free(s);
    }
    else retval = NW_FALSE;
  }
  else if (ScriptVarType(v) == VAL_TYPE_INVALID)
  {
    if (fp) fprintf(fp, "<invalid>\n");
      else printf("<invalid>\n");

  }
  else
  {
    NW_ASSERT(NW_FALSE);
    retval = NW_FALSE;
  }


  push_estack(new_empty_str_val());
  free_str(str);
  free_val(v);

  return retval;
}

static NW_Bool debug_closeFile(void)
{

  if (fp)
  {
    fclose(fp);
    fp = 0;
  }
  push_estack(new_empty_str_val());

  return NW_TRUE;
}

#else
NW_Bool debug_println(void) { return NW_TRUE; }
NW_Bool debug_openFile(void) { return NW_TRUE; }
NW_Bool debug_closeFile(void) { return NW_TRUE; }

#endif

/*  functions in Debug library */
static const lib_function_t debug_lib_table[NO_OF_DEBUG_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          debug_openFile,           NW_FALSE },
  {   1,          debug_println,            NW_FALSE },
  {   0,          debug_closeFile,          NW_FALSE }
};

void populate_debug_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(DEBUG_LIB_ID);
  (*table)[i] = debug_lib_table;

  return;
}
