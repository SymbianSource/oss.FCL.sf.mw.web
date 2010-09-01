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


#include "scr_conv.h"
#include "scr_api.h"
#include "scr_srv.h"
#include "nwx_mem.h"
#include "nwx_string.h"

#include "nwx_format.h"

#define NO_OF_WP_FUNCS    3  /* (p)ublic */

/*
TODO: WMS commented for now, fix when WTA is ready
static lib_function_t wta_p_funcs[NO_OF_WP_FUNCS];
*/

/* helper functions */

/* public functions */
static NW_Bool wp_makeCall(void)
{
  NW_Bool     result;
  NW_Ucs2     *number = NULL;

  /* convert script variable to "C" variable */
  number = GetUcs2Val();
  if (number == NULL) {
    push_estack(new_invalid_val());
    return NW_FALSE;
  }

  /* compliance with WAP-188, "phone_number"?*/
  if (!NW_Format_IsValidPhoneNumber(number)) {
    push_estack(new_invalid_val());
    NW_Str_Delete(number);
    return NW_FALSE;
  }

  /* call API routine */
  result = ScriptAPI_wp_makeCall(number);

  if (! result) {
    push_estack(new_invalid_val());
    NW_Str_Delete(number);
    return NW_FALSE;
  }
  return NW_TRUE;
}

static NW_Bool wp_sendDTMF(void)
{
  NW_Ucs2   *dtmf = NULL;
  NW_Bool   result;

  /* convert script variable to "C" variable */
  dtmf = GetUcs2Val();
  if (dtmf == NULL) {
    push_estack(new_invalid_val());
    return NW_FALSE;
  }

  /* compliance with WAP-188, "tone_sequence"?*/
  if (!NW_Format_IsValidToneSequence( dtmf )) {
    push_estack(new_invalid_val());
    NW_Str_Delete(dtmf);
    return NW_FALSE;
  }

  /* call API routine */
  result = ScriptAPI_wp_sendDTMF(dtmf);

  if (! result) {
    push_estack(new_invalid_val());
    NW_Str_Delete(dtmf);
    return NW_FALSE;
  }

  return NW_TRUE;
}

static NW_Bool wp_addPBEntry(void)
{
  NW_Ucs2   *number = NULL;
  NW_Ucs2   *name = NULL;
  NW_Bool   result;

  /* convert script variables to "C" variables */
  name = GetUcs2Val();
  if (name == NULL) {
    pop_estack_and_free();
    push_estack(new_invalid_val());
    return NW_FALSE;
  }

  number = GetUcs2Val();
  if (number == NULL) {
    NW_Str_Delete(name);
    push_estack(new_invalid_val());
    return NW_FALSE;
  }

  /* compliance with WAP-188, "phone_number"?*/
  if (!NW_Format_IsValidPhoneNumber(number)) {
    push_estack(new_invalid_val());
    NW_Str_Delete(number);
    NW_Str_Delete(name);
    return NW_FALSE;
  }


  /* call API routine */
  result = ScriptAPI_wp_addPBEntry(number, name);

  if (! result) {
    push_estack(new_invalid_val());
    NW_Str_Delete(number);
    NW_Str_Delete(name);
    return NW_FALSE;
  }

  return NW_TRUE;
}

/* Nokia proprietary library */
static NW_Bool nokia_locationInfo(void)
{
  NW_Bool     result;
  NW_Ucs2     *url = NULL;
  NW_Ucs2     *llformat = NULL;
  NW_Ucs2     *pnformat = NULL;

  pnformat = GetUcs2Val();
  if (pnformat == NULL) {
    pop_estack_and_free();
    pop_estack_and_free();
    push_estack(new_invalid_val());
    return NW_FALSE;
  }

  llformat = GetUcs2Val();
  if (llformat == NULL) {
    pop_estack_and_free();
    push_estack(new_invalid_val());
    NW_Str_Delete(pnformat);
    return NW_FALSE;
  }

  url = GetUcs2Val();
  if (url == NULL) {
    push_estack(new_invalid_val());
    NW_Str_Delete(pnformat);
    NW_Str_Delete(llformat);
    return NW_FALSE;
  }

  /* call API routine */
  result = ScriptAPI_nokia_locationInfo(url,llformat,pnformat);

  if (! result) {
    push_estack(new_invalid_val());
    NW_Str_Delete(url);
    NW_Str_Delete(llformat);
    NW_Str_Delete(pnformat);
    return NW_FALSE;
  }

  return NW_TRUE;
}

/*  functions in WP library */
static const lib_function_t wp_lib_table[NO_OF_WP_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          wp_makeCall,              NW_TRUE  },
  {   1,          wp_sendDTMF,              NW_TRUE  },
  {   2,          wp_addPBEntry,            NW_TRUE  }
};

void populate_wp_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(WP_LIB_ID);
  (*table)[i] = wp_lib_table;

  return;
}

/*  functions in Nokia library */
static const lib_function_t nokia_lib_table[NO_OF_NOKIA_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   3,          nokia_locationInfo,       NW_TRUE  }
};

void populate_nokia_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(NOKIA_LIB_ID);
  (*table)[i] = nokia_lib_table;

  return;
}

/* populate all libraries */
void populate_wta_lib(lib_function_table_t *table)
{
  populate_wp_lib(table);
  populate_nokia_lib(table);
}

NW_Byte size_wp_lib()   /* public */
{
  return NO_OF_WP_FUNCS;
}

NW_Byte size_nokia_lib()   /* public */
{
  return NO_OF_NOKIA_FUNCS;
}

