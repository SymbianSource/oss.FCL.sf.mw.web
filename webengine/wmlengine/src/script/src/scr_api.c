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

#include "scr_api.h"
#include "scr_srv.h"
#include "scr_core.h"
#include "scr_conv.h"
#include "scr_crypto.h"

#include "nw_wae_reader.h"

#include "nwx_defs.h"
#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_url_utils.h"
#include "nwx_msg_api.h"
#include "urlloader_urlresponse.h"
#include "urlloader_urlloaderint.h"
#include "nwx_script_handler.h"
#include "nwx_http_defs.h"
#include "nwx_ctx.h"
#include "nw_nvpair.h"

typedef struct {
  context_t ctx;
} Script_Server_t;

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
static NW_Bool genericLoad( const NW_Ucs2* url, char const* mimetype)
{
  void *reqHeader = NULL;
  NW_Uint16        tid  = 0;/* dont care about transaction ids */
  NW_Bool          ret  = NW_TRUE;
  TBrowserStatusCode      status;

  NW_ASSERT(url != NULL);

  reqHeader = UrlLoader_HeadersNew((NW_Uint8*)mimetype, NULL, NULL, NULL, 0, NULL);
  
  /* Add Accept headers */
  if (reqHeader == NULL ) {
    ret = NW_FALSE;
  }
  else {
    /* No default headers must be added later on */

    /* Do the load. When the loader finishes it invokes the
    NW_Script_Urlloadresp function with a NULL callback context */
    
    // this partialResponseBuffer is being used as a buffer in 
    // the response callback function
    NW_Buffer_t* partialResponseBuffer = NW_Buffer_New( 1 );  

    status = UrlLoader_Request(url, reqHeader, NULL, 0, NW_URL_METHOD_GET, &tid,
        (void*)partialResponseBuffer, (NW_Url_RespCallback_t*)NW_Script_Urlloadresp,
        NW_CACHE_NORMAL, NW_DEFAULTHEADERS_CHARSET, NW_UrlRequest_Type_None, NW_FALSE, NW_TRUE,
        NW_HED_UrlRequest_Reason_DocLoad, NULL);

    if (!BRSR_STAT_IS_SUCCESS(status)) {    
      NW_Buffer_Free( partialResponseBuffer );
      ret = NW_FALSE;
    }
  }

  if (ret == NW_FALSE) {
    UrlLoader_HeadersFree(reqHeader);
  }
  return ret;
}

static Script_Server_t *GetScrServerContext(void) 
{
  return (Script_Server_t *)NW_Ctx_Get(NW_CTX_SCR_SERVER, 0);
}

static void SetScrServerContext(Script_Server_t* context) 
{
  NW_Ctx_Set(NW_CTX_SCR_SERVER, 0, context);
}

lib_function_table_t* GetLibFuncTabContext(void) 
{
  return (lib_function_table_t*)NW_Ctx_Get(NW_CTX_SCR_FUNC_TAB, 0);
}

void SetLibFuncTabContext(lib_function_table_t* context) 
{
  NW_Ctx_Set(NW_CTX_SCR_FUNC_TAB, 0, (void*)context);
}

/*************************** Global Data *****************************
      NOTES: For now we will create a single global variable to
      hold all of the context for the component. It will be
      accessed with the getThis() function call
*/

/*TODO: delete when new memory context is tested */
/*static Script_Server_t g_script_server;*/
/*lib_function_t *LIB_FUNCS[NO_OF_STD_LIBS];*/
const NW_Int32 LIB_ID_MAP[NO_OF_STD_LIBS] = {
                                     LANG_LIB_ID,
                                     FLOAT_LIB_ID,
                                     STRING_LIB_ID,
                                     URL_LIB_ID,
                                     BROWSER_LIB_ID,
                                     DIALOG_LIB_ID,
                                     CRYPTO_LIB_ID,
                                     DEBUG_LIB_ID, 
                                     WP_LIB_ID,
                                     NOKIA_LIB_ID
                                   };

/* XML Spec 2.2 CDATA value */
/* Char ::=  #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF] */
/* NOTE: since NW_Ucs2 is only 2 bytes, we can't handle 0x10000-0x10ffff */
NW_Bool CheckVarValue(NW_Ucs2* value)
{
  NW_Ucs2 *pChar = value;

  while (*pChar != 0) {
    if ((*pChar >= 0x20) && (*pChar <= 0xd7ff)) {
      pChar++;
      continue;
    }
    if ((*pChar >= 0xE000) && (*pChar <= 0xfffd)) {
      pChar++;
      continue;
    }
    if ((*pChar == 0x9) || (*pChar == 0xa) || (*pChar == 0xd)) {
      pChar++;
      continue;
    }
    return NW_FALSE;
  }

  return NW_TRUE;
}
  
NW_Bool ScriptAPI_loadString(const NW_Ucs2* url, char *mimetype)
{
  context_t        *ctx = ScriptAPI_getCurrentContext();

  set_ctx_state(ctx, LOADSTRING_REQ);

  return genericLoad(url, mimetype);
}

NW_Bool ScriptAPI_load(NW_Ucs2* url)
{
  context_t*        ctx = ScriptAPI_getCurrentContext();
  char const*       mimetype = (char const*)HTTP_application_vnd_wap_wmlscriptc_string;

  set_ctx_state(ctx, URL_REQ);
  
  return genericLoad(url, mimetype);
}

NW_Bool ScriptAPI_reload(void)
{
  NW_Ucs2   * postfields;
  NW_Ucs2   * fullUrl  = NULL;
  NW_Bool     ret      = NW_FALSE;
  NW_Ucs2   * url      = get_curr_url();
  context_t * ctx      = ScriptAPI_getCurrentContext();
  char const* mimetype = (char const*)HTTP_application_vnd_wap_wmlscriptc_string;

  if (ctx && url)
  {
    postfields = get_curr_postfields();
    if (postfields) {
      if (KBrsrSuccess != NW_Url_AddPostfields(url, postfields, &fullUrl)) {
        goto funcExit;
      }
      url = fullUrl;
    }
    set_ctx_state(ctx, RELOAD);

    ret = genericLoad(url, mimetype);
  }

funcExit:
  NW_Str_Delete(fullUrl);
  return ret;
}

NW_Bool ScriptAPI_getVar(NW_Ucs2* name)
{
  if (!NW_CheckName(name)) {
    return NW_FALSE;
  }
  if (KBrsrSuccess != NW_Script_Getvar(name))
    return NW_FALSE;

  return NW_TRUE;
}

NW_Bool ScriptAPI_setVar(NW_Ucs2* name, NW_Ucs2* value)
{
  if (!NW_CheckName(name) || !CheckVarValue(value)) {
    return NW_FALSE;
  }
  if (KBrsrSuccess != NW_Script_Setvar(name, value))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_go(NW_Ucs2 *url)
{
  if (KBrsrSuccess != NW_Script_Go(url))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_prev(void)
{
  if (KBrsrSuccess !=NW_Script_Loadprev())
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_refresh(void)
{
  if (KBrsrSuccess !=NW_Script_Refresh())
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_newContext(void)
{
  if (KBrsrSuccess != NW_Script_Newcontext())
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_finishNoDialog(NW_Byte status, NW_Ucs2 *msg)
{
  if(KBrsrSuccess != NW_Script_Finishnodialog(status, msg))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_finish(NW_Byte status, NW_Ucs2 *msg)
{
  TBrowserStatusCode nwStatus;

  switch (status)
  {
   case SCR_ERROR_NO_ERROR:
   case SCR_ERROR_USER_EXIT:
   case SCR_ERROR_USER_ABORT:
   case SCR_ERROR_OUT_OF_MEMORY: /* can't display dialog if out of memory */
    ScriptAPI_finishNoDialog(status, msg);
    return NW_TRUE;

   case SCR_ERROR_VERIFICATION_FAILED:
    nwStatus = KBrsrScriptVerificationFailed;
    break;

   case SCR_ERROR_ACCESS_VIOLATION:
    nwStatus = KBrsrScriptAccessViolation;
    break;

   case SCR_ERROR_FATAL_LIBRARY_FUNCTION_ERROR:
    nwStatus = KBrsrScriptFatalLibFuncError;
    break;

   case SCR_ERROR_INVALID_FUNCTION_ARGUMENTS:
    nwStatus = KBrsrScriptInvalidFuncArgs;
    break;

   case SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND:
    nwStatus = KBrsrScriptExternalFunctionNotFound;
    break;

   case SCR_ERROR_UNABLE_TO_LOAD_COMPILATION_UNIT:
    nwStatus = KBrsrScriptUnableToLoadCompilationUnit;
    break;

   case SCR_ERROR_STACK_UNDERFLOW:
    nwStatus = KBrsrScriptErrorStackUnderflow;
    break;

   case SCR_ERROR_STACK_OVERFLOW:
    nwStatus = KBrsrScriptErrorStackOverflow;
    break;

   default:
    nwStatus = KBrsrScriptBadContent;
    break;
  }
  NW_WmlsDialog_Error(nwStatus);
//#pragma message( __FILE__ " TODO: Fix NULL deref in ScriptAPI_finishNoDialog() when called from ScriptAPI_finish()")
// Following line needed for WML Go handling;
//  ScriptAPI_finishNoDialog(status, NULL);
  return NW_TRUE;
}

NW_Bool NW_WmlsDialog_Prompt(const NW_Ucs2 *message, const NW_Ucs2 *defaultInput)
{
  if (KBrsrSuccess != NW_Script_Dialogprompt(message, defaultInput))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool NW_WmlsDialog_Confirm(const NW_Ucs2 *message, const NW_Ucs2 *yes_str, 
                            const NW_Ucs2 *no_str)
{
  if (KBrsrSuccess != NW_Script_Dialogconfirm(message, yes_str, no_str))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool  NW_WmlsDialog_Alert(const NW_Ucs2 *message)
{
  if (KBrsrSuccess != NW_Script_Dialogalert(message))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool  NW_WmlsDialog_Error(const TBrowserStatusCode nwStatus)
{
  if (KBrsrSuccess != NW_Script_Notifyerror(nwStatus))
    return NW_FALSE;
  return NW_TRUE;
}

NW_Bool ScriptAPI_suspend(void)
{
  if (KBrsrSuccess != NW_Script_Suspend())
    return NW_FALSE;
  return NW_TRUE;
}

context_t  *ScriptAPI_getCurrentContext(void)
{
  Script_Server_t* ptr_scr_server = GetScrServerContext();
  if (!ptr_scr_server) { /*have not initialized yet*/ 
    ptr_scr_server = (Script_Server_t*)NW_Mem_Malloc(sizeof(Script_Server_t));
    if (!ptr_scr_server) return NULL;    
    else SetScrServerContext(ptr_scr_server);

    /* Initialize the structure */
    ptr_scr_server->ctx.cs = NULL;
    ptr_scr_server->ctx.es = NULL;
    ptr_scr_server->ctx.fs = NULL;
    ptr_scr_server->ctx.cp = NULL;
    ptr_scr_server->ctx.fp = NULL;
    ptr_scr_server->ctx.fn = NULL;
    ptr_scr_server->ctx.access_path = 0;
    ptr_scr_server->ctx.access_domain = 0;
    ptr_scr_server->ctx.access_public = NW_FALSE;
    ptr_scr_server->ctx.errorCode = 0;
    ptr_scr_server->ctx.state = RUNNING;
    ptr_scr_server->ctx.msg = NULL;

  }
  return &(ptr_scr_server->ctx);
}

lib_function_table_t *ScriptAPI_get_lib_table(void)
{
  /* Initialize table structure if not ready done so*/
  lib_function_table_t *funcTable = GetLibFuncTabContext();
  if (!funcTable) {
    funcTable = (lib_function_table_t *)NW_Mem_Malloc(sizeof(lib_function_table_t));
    /* Memory problem */
    NW_ASSERT(funcTable);
    SetLibFuncTabContext(funcTable);
  }
  return funcTable;
}

NW_Int32 ScriptAPI_get_lib_index(NW_Int32 libId)
{
  NW_Int32 i = 0;
  while ( (i < NO_OF_STD_LIBS) &&
          (LIB_ID_MAP[i] != libId) )
  {
    i++;
  }

  /*lint -e{661} possible out-of-bounds*/
  NW_ASSERT(LIB_ID_MAP[i] == libId);
  return i;
}

/* native character set */
NW_Int32  ScriptAPI_character_set(void)
{
  return 0x3E8;
}

/********************************/
/******** WTA Functions *********/
/********************************/


/* public library */
NW_Bool ScriptAPI_wp_makeCall(NW_Ucs2 *number)
{
  if (KBrsrSuccess != NW_Script_wp_makeCall(number)) {
    return NW_FALSE;
  }
  return NW_TRUE;
}

NW_Bool ScriptAPI_wp_sendDTMF(NW_Ucs2 *dtmf)
{
  if (KBrsrSuccess != NW_Script_wp_sendDTMF(dtmf)) {
    return NW_FALSE;
  }
  return NW_TRUE;
}

NW_Bool ScriptAPI_wp_addPBEntry(NW_Ucs2 *number, NW_Ucs2 *name)
{
  if (KBrsrSuccess != NW_Script_wp_addPBEntry(number, name)) {
    return NW_FALSE;
  }
  return NW_TRUE;
}

/********************************************************
 * NOKIA proprietary library
 * 
 * Name:   locationInfo
 * Input:  NW_Ucs2 *url
 *         NW_Ucs2 *ll_format
 *         NW_Ucs2 *pn_format
 *********************************************************/

NW_Bool ScriptAPI_nokia_locationInfo(NW_Ucs2 *url, 
                                     NW_Ucs2 *ll_format, 
                                     NW_Ucs2 *pn_format)
{
  if (KBrsrSuccess != NW_Script_nokia_locationInfo(url, ll_format, pn_format)) {
    return NW_FALSE;
  }
  return NW_TRUE;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   ScriptAPI_signText
    Input   :   NW_Ucs2*, NW_Int32, NW_Int32, NW_Byte*, NW_Int32
    Output  :   NW_Bool
    Purpose :   Crypto Lib API. Used when the User digitally signs a Text
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

NW_Bool ScriptAPI_signText(NW_Ucs2 *stringToSign, NW_Int32 option, 
                           NW_Int32 keyIdType, NW_Byte *keyIdByte,
                           NW_Int32 keyIdLen)
{
  if (KBrsrSuccess != NW_Script_CryptoSignTextStart(stringToSign, 
                                                       option, 
                                                       keyIdType, 
                                                       keyIdByte, 
                                                       keyIdLen)) {
    return NW_FALSE;
  }

  return NW_TRUE;
}
