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
    $Workfile: scr_url.c $

    Purpose:

        This file implements the ScriptServer functionality for the URL standard 
        library. The URL standard library contains a set of functions for 
        handling both absolute and relative URLs.  The general syntax is:
          <scheme>://<host>:<port>/path;<params>?<queury>#<fragment>
 */

#include "scr_url.h"
#include "scr_frstack.h"
#include "scr_estack.h"
#include "scr_core.h"
#include "scr_conv.h"
#include "scr_api.h"
#include "scr_srv.h" 

//#include "nwx_http_header_utils.h"
#include "nwx_url_utils.h"

#include "nwx_mem.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"


/*#define NO_OF_URL_FUNCS 14
static lib_function_t url_funcs[NO_OF_URL_FUNCS];*/

/*#define BREAK_DO { failed = NW_TRUE; break; }*/

static NW_Ucs2* GetValidUrl(void)
{
  NW_Ucs2 *ustr = 0;
  
  ustr = GetUcs2Val();
  if (ustr != NULL) {
    if (NW_Url_IsValid(ustr) == NW_FALSE) {
      push_estack(new_invalid_val());
      NW_Str_Delete(ustr);
      return NULL;
    }
  }

  return ustr;
}

static NW_Bool ReturnStrVal(TBrowserStatusCode status, NW_Ucs2* res_ustr, NW_Bool res_valid)
{
  val_t res = uninitialize_val();
  str_t res_str = 0;

  if (status == KBrsrOutOfMemory) {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    if (res_ustr != NULL) {
      NW_Str_Delete(res_ustr);
      res_ustr = NULL;
    }
  }

  if (!res_valid) {
    NW_Str_Delete(res_ustr);
    push_estack(new_invalid_val());
    return NW_FALSE;
  } else if (res_ustr == NULL) {
    push_estack(new_empty_str_val());
    return NW_FALSE;
  }

  res_str = ucs22str(res_ustr);
  NW_Str_Delete(res_ustr);
  if (res_str == NULL) {
    /*lint -e{774} Significant prototype coercion*/
    if (res_valid) {
      push_estack(new_empty_str_val());
    } else {
      push_estack(new_invalid_val());
    }
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }
  
  res = new_str_val(res_str);
  free_str(res_str);

  push_estack(res);
  free_val(res);
  
  return NW_TRUE;
}


/*****************************************************************

  Name: IsValidLoadStringContentType(void)

  Description:  Checks for valid loadString content type argument.
                It must begin with "text/" and contain no whitespace
                or comma's.

  Return Value: NW_TRUE if valid, NW_FALSE otherwise.

******************************************************************/
static NW_Bool IsValidLoadStringContentType ( char* contentType )
{
  char* charPtr;
  const char *textPrefix = "text/";

  if ( contentType == NULL ) {
    return NW_FALSE;
  }
   
  /* Check that the string begins with "text/". */
  if (NW_Asc_strnicmp(contentType, textPrefix,
                      NW_Asc_strlen(textPrefix)) != 0)
  {
    /* Does not begin with "text/".  Fail */
    return NW_FALSE;
  }

  /* Fail if string contains any whitespace or a comma. */
  charPtr = contentType;
  while ( *charPtr ) {
    NW_Ucs2 c;

    c = *charPtr;

    if ( NW_Str_Isspace( c ) )
      return NW_FALSE;

    if ( c == WAE_URL_COMMA_CHAR )
      return NW_FALSE;

    charPtr++;
  }

  return NW_TRUE;
}

NW_Byte size_url_lib(void)
{
  return NO_OF_URL_FUNCS;
}

static NW_Bool url_isValid(void)
{
  val_t res = uninitialize_val();
  NW_Ucs2 *ustr = 0;
  
  ustr = GetUcs2Val();
  if (ustr == NULL) {
    return NW_FALSE;
  }

  res = new_bool_val(NW_Url_IsValid(ustr));
  NW_Str_Delete(ustr);

  push_estack(res);
  free_val(res);
  
  return NW_TRUE;
}

static NW_Bool url_getScheme(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }

  status = NW_Url_GetSchemeStr(ustr, &res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}

static NW_Bool url_getHost(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }
    
  status = NW_Url_GetHost(ustr, &res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}

static NW_Bool url_getPort(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  NW_Bool valid_port;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }
    
  status = NW_Url_GetPort(ustr, &valid_port, &res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, valid_port);
}


static NW_Bool url_getPath(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }
    
  status = NW_Url_GetPathNoParam(ustr, &res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}


static NW_Bool url_getParameters(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }
    
  status = NW_Url_GetParameters(ustr, &res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}

static NW_Bool url_getQuery(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }
    
  status = NW_Url_GetQuery(ustr, &res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}

static NW_Bool url_getFragment(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status;
  
  ustr = GetValidUrl();
  if (ustr == NULL) {
    return NW_FALSE;
  }
    
  status = NW_Url_GetFragId(ustr, &res_ustr, NW_FALSE);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}

static NW_Bool url_getBase(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  TBrowserStatusCode status = KBrsrSuccess;

  ustr = get_curr_url();
  /* should always have a current url, but just in case somebody stomps */
  /* on the url pointer in the frame... */
  if (ustr == NULL) {
    NW_ASSERT(ustr != NULL);
    res_ustr = NULL;
  } else {
    status = NW_Url_GetBase(ustr, &res_ustr);
  }

  return ReturnStrVal(status, res_ustr, NW_TRUE);
}

static NW_Bool url_getReferer(void)
{
  TBrowserStatusCode status;
  str_t res_str = 0;
  NW_Ucs2 *relURI = NULL;
  NW_Ucs2 *referer = get_curr_referer();
  val_t res = uninitialize_val();

  if (referer)
  {
    status = NW_Url_AbsToRel(get_curr_url(), referer, &relURI);
    if (status == KBrsrOutOfMemory)
    {
      set_error_code(SCR_ERROR_OUT_OF_MEMORY);
      return NW_FALSE;
    }

    res_str = ucs22str(relURI);
    res = new_str_val(res_str);
    free_str(res_str);
  }
  else res = new_empty_str_val();

  push_estack(res);

  free_val(res);
  if (relURI) {
    NW_Mem_Free(relURI);
  }
  
  return NW_TRUE;
}

static NW_Bool url_resolve(void)
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val();
  NW_Ucs2 *base_url = 0, *emb_url = 0, *res_url = 0;
  TBrowserStatusCode status;
  NW_Bool isvalid;

  v2 = pop_estack();
  v1 = pop_estack();

  if ((ScriptVarType(v1) == VAL_TYPE_INVALID) || (ScriptVarType(v2) == VAL_TYPE_INVALID) )
  {
    push_estack(new_invalid_val());
    free_val(v1);
    free_val(v2);
    return NW_FALSE;
  }

  base_url = val2ucs2_trim(v1);
  emb_url  = val2ucs2_trim(v2);
  free_val(v1);
  free_val(v2);

  if (!base_url || ! emb_url)
  {
    /* Note: NW_Str_Delete() handles null pointers */
    NW_Str_Delete(base_url);
    NW_Str_Delete(emb_url);
    return NW_TRUE;
  }

  status = NW_Url_RelToAbs(base_url, emb_url, &res_url);
  NW_Str_Delete(base_url);
  NW_Str_Delete(emb_url);

  isvalid = NW_BOOL_CAST(res_url != NULL);

  return ReturnStrVal(status, res_url, isvalid);
}

static NW_Bool url_escapeString(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  NW_Int32  len;
  NW_Bool isvalid;
  TBrowserStatusCode status = KBrsrSuccess;
  
  ustr = GetUcs2Val();
  if (ustr == NULL) {
    return NW_FALSE;
  }

  len = NW_Url_EscapeStringLen(ustr);
  res_ustr = NW_Str_New(len);
  if (res_ustr == NULL){
    status = KBrsrOutOfMemory;
  }

  isvalid = NW_Url_EscapeString(ustr, res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, isvalid);
}

static NW_Bool url_unescapeString(void)
{
  NW_Ucs2 *ustr = 0, *res_ustr = 0;
  NW_Int32  len;
  NW_Bool isvalid;
  TBrowserStatusCode status = KBrsrSuccess;
  
  ustr = GetUcs2Val();
  if (ustr == NULL) {
    return NW_FALSE;
  }

  len = NW_Url_UnEscapeStringLen(ustr);
  res_ustr = NW_Str_New(len);
  if (res_ustr == NULL){
    status = KBrsrOutOfMemory;
  }

  isvalid = NW_Url_UnEscapeString(ustr, res_ustr);
  NW_Str_Delete(ustr);

  return ReturnStrVal(status, res_ustr, isvalid);
}

static NW_Bool url_loadString(void)
{
  val_t v1 = uninitialize_val(), v2 = uninitialize_val();
  NW_Ucs2 *url = NULL;
  char *mimetype = 0;
  str_t mimetype_str = 0;
  NW_Bool status = NW_FALSE;

  /*lint --e{764} No cases, only default*/
  switch (status)
  {
   default:  
    v2 = pop_estack();
    v1 = pop_estack();

    if ((ScriptVarType(v1) == VAL_TYPE_INVALID) || (ScriptVarType(v2) == VAL_TYPE_INVALID))
    {
      status = NW_FALSE;
      break;
    }

    url = val2ucs2_trim(v1);

    if (!url)
    {
      status = NW_FALSE;
      break;
    }

    if (!val2str(v2, &mimetype_str))
    {
      status = NW_FALSE;
      break;
    }

    mimetype = str2ascii(mimetype_str);

    if ( ! IsValidLoadStringContentType( mimetype ) )
    {
      status = NW_FALSE;
      break;
    }

    if (!NW_Url_IsValid(url))
    {
      status = NW_FALSE;
      break;
    }

    /* Leave url until get response, jwild */
    status = ScriptAPI_loadString(url, mimetype);
  }
  
  NW_Mem_Free(url);
  NW_Mem_Free(mimetype);
  free_str(mimetype_str);
  free_val(v1);
  free_val(v2); 
  
  if (!status)
    {
      push_estack(new_invalid_val());
      return NW_FALSE;
    }
  else
    return NW_TRUE;
}


/*  functions in Url library */
static const lib_function_t url_lib_table[NO_OF_URL_FUNCS] =
{ /*  arg_size,   func_ptr,                 is_async  */
  {   1,          url_isValid,              NW_FALSE },
  {   1,          url_getScheme,            NW_FALSE },
  {   1,          url_getHost,              NW_FALSE },
  {   1,          url_getPort,              NW_FALSE },
  {   1,          url_getPath,              NW_FALSE },
  {   1,          url_getParameters,        NW_FALSE },
  {   1,          url_getQuery,             NW_FALSE },
  {   1,          url_getFragment,          NW_FALSE },
  {   0,          url_getBase,              NW_FALSE },
  {   0,          url_getReferer,           NW_FALSE },
  {   2,          url_resolve,              NW_FALSE },
  {   1,          url_escapeString,         NW_FALSE },
  {   1,          url_unescapeString,       NW_FALSE },
  {   2,          url_loadString,           NW_TRUE  }
};

void populate_url_lib(lib_function_table_t *table)
{
  NW_Int32 i = ScriptAPI_get_lib_index(URL_LIB_ID);
  (*table)[i] = url_lib_table;

  return;
}
