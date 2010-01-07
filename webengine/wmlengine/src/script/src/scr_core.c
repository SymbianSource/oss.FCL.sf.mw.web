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

#include "scr_core.h"
#include "scr_conv.h"
#include "scr_api.h"
#include "scr_srv.h"
#include "scr_reader.h"
#include "scr_params.h"
#include "scr_inter.h"

#include "scr_lang.h"
#include "scr_float.h"
#include "scr_strlib.h"
#include "scr_url.h"
#include "scr_browser.h"
#include "scr_dialog.h"
#include "scr_debug.h"
#include "scr_wta.h"
#include "scr_crypto.h"
#include "nwx_url_utils.h"

#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"

static NW_Ucs2* Base64Encod(NW_Int32, NW_Byte * );

/* Base64Table contains the 64 printable characters */
/* const char Base64Table[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";*/
/* static NW_Ucs2 *ConvertByteToUcs2String(NW_Int32 length, NW_Byte *info); *//*not used anywhere*/
#define BASE64_LINE_LEN 76

/* HexadecimalTable contains 16 hex characters */
/*const char HexadecimalTable [16] = "0123456789abcdef";*/
void populate_lib_fcns(void);



void initialize_core_script(void)
{
  populate_lib_fcns();
  return;
}

/****************************************************************************

  Name: UniqueBaseValue

  Description:  Creates a value to be used as the base value 
                for the check-sum calculation 

  Parameters:   Data, length of the data
  Algorithm :   Uses frame stack pointer to create "unique" offsets to the data
                buffer. The frame stack is allocated once Wml invokes Wmls.
                It will not change at least until Wml would invoke Wmls next time.
                The first four bytes of the pointer value are used to fetch
                bytes from the data buffer. If the value of a particular 
                fetched byte is greater that the data buffer size then
                the offset is calculated by applying the modulus operator to
                the byte value.
  Return Value: unique base value

******************************************************************************/
static NW_Uint32 UniqueBaseValue(void *data, NW_Uint32 len)
{
  NW_Int32  i;
  frstack_t *fs;
  NW_Byte   buf[sizeof(fs)];
  NW_Uint32 val  = len;
  NW_Byte   *ptr = (NW_Byte *)data;
  context_t *ctx = ScriptAPI_getCurrentContext();

  NW_ASSERT(ctx && data && len > 1);
  fs = get_ctx_frstack(ctx);
  NW_ASSERT(fs);

  NW_Mem_memcpy(&buf[0], &fs, sizeof(fs));
  for (i=0; i < (NW_Int32) sizeof(fs); i++) {
    val += ptr[buf[i]%len];
  }
  return val;
}

/****************************************************************************

  Name: BuildCheckSum

  Description:  Calculates check-sum by adding shifted (0,8,16,24 times) bytes 
                from the data buffer.
                The value that is used as the base for additions is supplied
                by the UniqueBaseValue() function

  Parameters:   Data, length of the data

  Return Value: check-sum for the data buffer

****************************************************************************/
static NW_Uint32 BuildCheckSum(void *data, NW_Uint32 len)
{
  NW_Uint32 i;
  NW_Uint32 locSum;
  NW_Byte   *ptr = (NW_Byte *) data;

  locSum = UniqueBaseValue(data, len);
  for (i=0; i < len; i++) {
    NW_Uint32 tmp = ptr[i];
    locSum += (tmp << ( (i%sizeof(NW_Uint32)) *8));
  }
  return locSum;
}

/* Helper function for CoreScript_InitiateScript() */
static NW_Byte InitiateScriptErrChk(NW_Byte error)
{
  NW_Byte ec;

  ec = get_error_code();
  if (ec == SCR_ERROR_NO_ERROR) {
    ec = error;
    set_error_code(ec);
  }

  return ec;
}


/* transfer ownership of url and referer to the script interpreter */
void CoreScript_InitiateScript(NW_Ucs2 *url,
                               NW_Ucs2 *postfields,
                               NW_Ucs2 *referer,
                               NW_Byte *content, 
                               const NW_Uint32 contentLength,
                               const NW_Http_CharSet_t charset)
{
  NW_Ucs2     *frag     = NULL;
  NW_Ucs2     *funcName = NULL;
  NW_Ucs2     *tmpFrag;
  NW_Byte     funcId    = 0;
  NW_Byte     ec        = SCR_ERROR_NO_ERROR;
  function_t  *f;
  NW_Int32    args;
  NW_Reader_t reader;

  do {
    TBrowserStatusCode status;
    NW_Uint32   chkSum;
    context_t   *ctx = ScriptAPI_getCurrentContext();
    NW_ASSERT(ctx);

    NW_Reader_InitializeBuffReader(&reader, content, contentLength, charset);

    /*
    ** read_script does a clear_url_context() call which effectively erases
    ** the possibility of returning to our previous state.  This should be
    ** postponed till we verify we can load the entire new script, however
    ** it is not convenient.
    */

    if (!read_script(&reader, referer, url)) {
      ec = InitiateScriptErrChk(SCR_ERROR_VERIFICATION_FAILED);
      break;
    }

    init_frames();

    if (!create_estack()) {
      ec = InitiateScriptErrChk(SCR_ERROR_OUT_OF_MEMORY);
      break;
    }

    status = NW_Url_GetFragId(url, &frag, NW_FALSE);
    if (status == KBrsrOutOfMemory) {
      ec = InitiateScriptErrChk(SCR_ERROR_OUT_OF_MEMORY);
      break;
    }

    if (frag == NULL) {
      ec = InitiateScriptErrChk(SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND);
      break;
    }

    /* Before use: Unescape the fragment */
    tmpFrag = frag;
    frag = NW_Url_ConvertFileFromEscapeURL(tmpFrag);
    NW_Str_Delete(tmpFrag);
    if (frag == NULL) {
      ec = InitiateScriptErrChk(SCR_ERROR_OUT_OF_MEMORY);
      break;
    }

    funcName = read_func_name(frag);
    if (funcName  == NULL) {
      ec = InitiateScriptErrChk(SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND);
      break;
    }

    if (!find_function_index(ctx, funcName, &funcId)) {
      ec = InitiateScriptErrChk(SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND);
      break;
    }

    chkSum = BuildCheckSum(content, contentLength);
    if (!push_frame(url, postfields, chkSum, funcId, referer)) {
      ec = InitiateScriptErrChk(SCR_ERROR_STACK_OVERFLOW);
      break;
    }

    f    = get_func(funcId);
    args = no_of_args(f);

    if (!parse_argument_list(frag + NW_Str_Strlen(funcName), args))
    {
      /* NW_Wmls_PopFrame() properly frees referer and url */
      NW_Wmls_PopFrame();
      url     = NULL;
      referer = NULL;
      
      ec = InitiateScriptErrChk(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
      break;
    }

    /* return at end of script function will call NW_Wmls_PopFrame() */
    /* which deletes url & referer, so down below don't free url & */
    /* referer unless we've gotten an error */
    interprete();
    
    /* Only time we change ec in this loop is prior to break statement */
    /* this comparison avoids a warning about const condition */
  } while (ec != SCR_ERROR_NO_ERROR);

  NW_Str_Delete(frag);
  NW_Str_Delete(funcName);
  if (ec != SCR_ERROR_NO_ERROR)
  {
    /*
    ** if we got here, we failed to properly load the script and therefore
    ** need to take down any parts of the script we managed to read before
    ** encountering an error.  We would LIKE to have saved all the previous
    ** context till we finished loading all the sections of the new script,
    ** however for now just be content to make sure we don't leak.
    */
    context_t *ctx = ScriptAPI_getCurrentContext();
    clear_url_context(ctx);
    NW_Str_Delete(url);
    NW_Str_Delete(referer);
    ScriptAPI_finish(ec, get_error_msg() ); 
  }

  return;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   CoreScript_SignText_CB
    Input   :   NW_Int32, NW_Byte*, NW_Int32
    Output  :   void
    Purpose :   This function is the callback function for the crypto_signText()
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Crypto Lib Callback */
void CoreScript_SignText_CB(TBrowserStatusCode status, NW_Byte* Text, NW_Int32 length)
{
  
    NW_Ucs2 *ustr;
    str_t str;
    val_t val;

    /* Note: ucs22str() returns NULL if out-of-mem and            */
    /* new_str_val_and_free() returns new_invalid_val() if a NULL */
    /* is passed into it. This is the same behavior as old code.  */ 
    if (status == KBrsrCryptoUserCancel) {
        ustr = NW_Str_CvtFromAscii("error:userCancel");
    } else if (status == KBrsrCryptoNoCert) {
        ustr = NW_Str_CvtFromAscii("error:noCert");
    } else if (status == KBrsrSuccess) {
        ustr = Base64Encod(length,Text);/* Encoding the KeyIdType as Base64 */
    } else {
        ustr = NULL;
    }

    str = ucs22str(ustr);
    if (ustr) NW_Str_Delete(ustr);
    val = new_str_val_and_free(&str);
    push_estack_and_free(&val);
    
    interprete();
    return;
}


/* The following functions are CoreScript's callbacks from the runtime
   environment's messaging mechanism. Each callback is the response to
   one of the above messages to the CoreBrowser. */
void CoreScript_GetVar_CB(TBrowserStatusCode status, NW_Ucs2 *var_name, NW_Ucs2 *var_value)
{
  str_t str;
  val_t val;

  NW_ASSERT((var_name == NULL) || (var_name != NULL));

	NW_REQUIRED_PARAM(var_name);
  /* continue with the script execution */
  if ((status == KBrsrSuccess) && (var_value != NULL))
  {
    str = ucs22str(var_value);
    val = new_str_val_and_free(&str);
  }
  else
  {
    val = new_empty_str_val();
  }

  push_estack_and_free(&val);
  interprete();
  return;
}


void CoreScript_SetVar_CB(TBrowserStatusCode status)
{
  NW_ASSERT(status == KBrsrSuccess);

	NW_REQUIRED_PARAM(status);
  /* for the time being - always succeed */
  push_estack(new_bool_val(NW_TRUE));

  /* continue with the script execution */
  interprete();
  return;
}


void CoreScript_Go_CB(TBrowserStatusCode status)
{
  if (status == KBrsrSuccess) {
    push_estack(new_empty_str_val());
  } else {
    push_estack(new_invalid_val());
  }
  
  /* continue with the script execution */
  interprete();
  return;
}


void CoreScript_Prev_CB(TBrowserStatusCode status)
{
  if (status == KBrsrSuccess) {
    push_estack(new_empty_str_val());
  } else {
    push_estack(new_invalid_val());
  }
  
  /* continue with the script execution */
  interprete();
  return;
}


void CoreScript_Refresh_CB(TBrowserStatusCode status)
{

  NW_REQUIRED_PARAM(status);
  /*
  * WMLBrowser.Refresh() is  a NOOP for the WML Interepreter. 
  * As we dont support refresh we push a invalid value on the stack
  * and continue interpreting. [WMLScriptLibs, 12.7]
  */
  push_estack(new_invalid_val());

  /* continue with the script execution */
  interprete();
  return;
}


void CoreScript_NewContext_CB(TBrowserStatusCode status)
{
  if (status == KBrsrSuccess) {
    push_estack(new_empty_str_val());
  } else {
    push_estack(new_invalid_val());
  }
  
  /* continue with the script execution */
  interprete();
  return;
}


NW_Bool CoreScript_CallingFunctionIndex(NW_Byte *findex)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  function_t *f;
  NW_Ucs2 *fname;
  NW_Bool retval;
  /* we get the function and argument information from the call_str structure in scr_context */

  fname  = get_ctx_fname(ctx);
  retval = find_function_index(ctx, fname, findex);

  if (!retval)
  {
    set_error_code(SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND);
    return NW_FALSE;
  }

  f = get_func(*findex);
  NW_ASSERT(f);
  
  /* function not found is a runtime error !! */
  /* mismatch of args is a runtime error !! */
  if(get_ctx_args_no(ctx) != no_of_args(f))
  {
    set_error_code(SCR_ERROR_INVALID_FUNCTION_ARGUMENTS);
    return NW_FALSE;
  }
  return NW_TRUE;
}


/* Helper function for CoreScript_LoadScript_CB() */
static NW_Bool LoadScriptCleanup(context_t *ctx, NW_Uint8 error)
{
  delete_call_str(ctx);
  set_error_code(error);
  return NW_FALSE;
}


/* The caller can free the content and the url */
/* We now make a local copy when necessary */
NW_Bool CoreScript_LoadScript_CB(TBrowserStatusCode status, 
                                 NW_Ucs2     *url,
                                 NW_Byte     *content, 
                         const NW_Uint32     contentLength,
                     const NW_Http_CharSet_t charset)
{
  context_t   *ctx;
  ctx_state_e st;
  NW_Reader_t reader;
  NW_Ucs2     *referer = NULL;
  NW_Ucs2     *urlcopy = NULL;

  NW_ASSERT(status == KBrsrSuccess);

  NW_REQUIRED_PARAM(status);
  ctx = ScriptAPI_getCurrentContext();
  st = get_ctx_state(ctx);

  NW_Reader_InitializeBuffReader(&reader, content, contentLength, charset);

  referer = get_curr_referer();


  if ((st == URL_REQ) || (st == RELOAD))
  {
    NW_Byte     findex;
    NW_Uint32   chkSum;

    chkSum = BuildCheckSum(content, contentLength);
    if (st == RELOAD && get_curr_chkSum() != chkSum) {
      return LoadScriptCleanup(ctx, SCR_ERROR_VERIFICATION_FAILED);
    }


    if (!read_script(&reader, referer, url)) {
      return LoadScriptCleanup(ctx, SCR_ERROR_VERIFICATION_FAILED);
    }
    if (st == URL_REQ)
    {
      if (!CoreScript_CallingFunctionIndex(&findex)) {
        return LoadScriptCleanup(ctx, SCR_ERROR_VERIFICATION_FAILED);
      }

      /* Script interpreter is going to free the url */
      urlcopy = NW_Str_Newcpy(url);
      if (urlcopy == NULL) {
        return LoadScriptCleanup(ctx, SCR_ERROR_OUT_OF_MEMORY);
      }

      /* urlcopy in this case already has postfields included */
      if (!push_frame(urlcopy, NULL, chkSum, findex, referer)) {
        NW_Str_Delete(urlcopy);
        return LoadScriptCleanup(ctx, SCR_ERROR_STACK_OVERFLOW);
      }
      delete_call_str(ctx);
    }

    interprete();
    return NW_TRUE;
  }

  /* we should not have gotten here ... */
  return LoadScriptCleanup(ctx, SCR_ERROR_UNABLE_TO_LOAD_COMPILATION_UNIT);
}


/* Helper function for CoreScript_LoadString_CB() */
static NW_Bool LoadStringCleanup(NW_Uint32 httpStatus)
{
  val_t val;

  val = new_int_val((httpStatus));
  push_estack_and_free(&val);
  interprete();
  return NW_TRUE;
}


NW_Bool CoreScript_LoadString_CB(TBrowserStatusCode status, 
                                 NW_Uint32 httpStatus,
                                 NW_Byte *content,
                                 const NW_Uint32 contentLength,
                                 const NW_Http_ContentTypeString_t contentTypeString,
                                 const NW_Http_CharSet_t charset)
{
  NW_Reader_t reader;
  NW_Int32  strLen;
  val_t val;
  str_t str;
  context_t *ctx = ScriptAPI_getCurrentContext();
  
  NW_ASSERT(status == KBrsrSuccess || status == KBrsrHttpStatus);

	NW_REQUIRED_PARAM(status);
  if (get_ctx_state(ctx) != LOADSTRING_REQ) {
    set_error_code(SCR_ERROR_UNABLE_TO_LOAD_COMPILATION_UNIT);
    return NW_FALSE;
  }

  if (httpStatus != Success)
  {
    return LoadStringCleanup(httpStatus);
  }

  /* these 2 content types are added to accept header in CreateHttpHeaders */
  /* if we request a uri whose content type doesn't match the file, then we */
  /* normally get NotFound as the http error code.  Since we didn't really */
  /* want these two types in our accept header, then we WOULD have gotten */
  /* NotFound as our responseCode */
  if ((0 == NW_Asc_stricmp((char*)HTTP_application_vnd_wap_wmlc_string, (char*)contentTypeString)) ||
      (0 == NW_Asc_stricmp((char*)HTTP_application_vnd_wap_wmlscriptc_string, (char*)contentTypeString)))
  {
    /* Got a content type that does not match the content type
       requested.  Return an integer error code. */
    return LoadStringCleanup(NotAcceptable);
  }

  NW_Reader_InitializeBuffReader(&reader, content, contentLength, charset);
  
  strLen = NW_Reader_GetStrCharCount(&reader, reader.readerSize);

  if ( strLen <= 0 ) {
    /* TODO -- verify this is correct error code */
    return LoadStringCleanup(NotAcceptable);
  }

  str = new_str(strLen, NULL);
  if (str == NULL)
  {
    set_error_code(SCR_ERROR_OUT_OF_MEMORY);
    return NW_FALSE;
  }

  if (!NW_Reader_ReadChars(&reader, str_buf_ptr(str), reader.readerSize))
  {
    free_str(str);
    /* TODO -- verify this is correct error code */
    return LoadStringCleanup(NotAcceptable);
  }

  val = new_str_val_and_free(&str);
  push_estack_and_free(&val);

  interprete();
  return NW_TRUE;
}


void CoreScript_Resume(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  ctx_state_e s = get_ctx_state(ctx);

  if (s == SUSPEND)
  {
    set_ctx_state(ctx, RUNNING);
    interprete();
  }

  return;
}


void  NW_WmlsDialog_PromptResp(NW_Ucs2 *input)
{
  str_t str;
  val_t val;

  /* continue with the script execution */
  if (input)
  {
    str = ucs22str(input);
    val = new_str_val_and_free(&str);
  }
  else
  {
    val = new_empty_str_val();
  }

  push_estack_and_free(&val);
  interprete();

  return;
}


void  NW_WmlsDialog_ConfirmResp(NW_Bool result)
{
  push_estack(new_bool_val(result));
  interprete();

  return;
}

void  NW_WmlsDialog_NotifyerrorResp(void)
{
  NW_Byte errCode = get_error_code();

  if (errCode == SCR_ERROR_NO_ERROR)
  {
    push_estack(new_empty_str_val());
    interprete();
  }
  else 
  {
    /* we return from displaying error message */
    ScriptAPI_finishNoDialog(errCode, NULL);
  }

  return;
}

void  NW_WmlsDialog_AlertResp(void)
{
  NW_Byte errCode = get_error_code();

  if (errCode == SCR_ERROR_NO_ERROR)
  {
    push_estack(new_empty_str_val());
    interprete();
  }
  else 
  {
    /* we return from displaying error message */
    ScriptAPI_finishNoDialog(errCode, NULL);
  }

  return;
}


void set_error_code(NW_Byte b)
{
  context_t *ctx = ScriptAPI_getCurrentContext();

  set_ctx_error_code(ctx, b);
  return;
}


NW_Byte get_error_code(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();
  
  return get_ctx_error_code(ctx);
}


void set_error_msg(NW_Ucs2* msg)
{
  context_t *ctx = ScriptAPI_getCurrentContext();

  set_ctx_error_msg(ctx, msg);
  return;
}


NW_Ucs2* get_error_msg(void)
{
  context_t *ctx = ScriptAPI_getCurrentContext();

  return get_ctx_error_msg(ctx);
}


NW_Byte get_function_index(NW_Uint16 lib_index,NW_Byte func_index)
{
  switch (lib_index)
  {
    /* The crypto library is the only library whose indexes do not start from 0 */
    case CRYPTO_LIB_ID:
      switch (func_index)
      {
        case 16:
          func_index = 0;
          break;
      }
      break;
  }
  return func_index;
}
 



#define CHECK_FUNC_INDEX(index,limit) \
if ((index) < (limit)) {return NW_TRUE;} else {return NW_FALSE;}

NW_Bool is_valid_reference(NW_Uint16 lib_index, NW_Byte func_index)
{
  switch (lib_index) {
   case LANG_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_lang_lib())
    
   case FLOAT_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_float_lib())
    
   case STRING_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_string_lib())
    
   case URL_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_url_lib())
    
   case BROWSER_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_browser_lib())
    
   case DIALOG_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_dialog_lib())

  /* Added: For Crypto Library. By araman 06/08/00 4PM */
   case CRYPTO_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_crypto_lib())
    
#ifdef WAE_DEBUG
   case DEBUG_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_debug_lib())
#endif

   case WP_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_wp_lib())

   case NOKIA_LIB_ID:
    CHECK_FUNC_INDEX(func_index, size_nokia_lib())

   default:
    return (NW_FALSE);
  }
}


void populate_lib_fcns(void)
{
  lib_function_table_t *lib_funcs = ScriptAPI_get_lib_table();

  populate_lang_lib(lib_funcs);
  populate_float_lib(lib_funcs);
  populate_string_lib(lib_funcs);
  populate_url_lib(lib_funcs);
  populate_browser_lib(lib_funcs);
  populate_dialog_lib(lib_funcs);
  populate_crypto_lib(lib_funcs);  /* Added: For Crypto Library */
  populate_wta_lib(lib_funcs);

#ifdef WAE_DEBUG
  populate_debug_lib(lib_funcs);
#endif
}


NW_Bool call_lib_func(NW_Uint16 lib_index, NW_Byte func_index, NW_Bool *async)
{
  lib_function_table_t *lib_funcs = ScriptAPI_get_lib_table();
  NW_Int32 index = ScriptAPI_get_lib_index(lib_index);
  
  func_index = get_function_index(lib_index,func_index);

  if (!is_valid_reference(lib_index, func_index))
  {
    set_error_code(SCR_ERROR_FATAL_LIBRARY_FUNCTION_ERROR);
    return NW_FALSE;
  }

  *async = (*lib_funcs)[index][func_index].is_async;
  return (*lib_funcs)[index][func_index].fp();
}


/*******************************************************************
 *
 * WTAI handling
 *
 *******************************************************************/

void CoreScript_Wtai_CB(NW_Opaque_t *result)
{
  str_t str;
  val_t val;
  
  NW_ASSERT(result != NULL);
  if ( result->type == NW_OPAQUE_VALUE_TYPE_INVALID ) {
    push_estack(new_invalid_val());
  } else {
    switch (result->type) {
    case NW_OPAQUE_VALUE_TYPE_INT32:
      push_estack(new_int_val (result->value.i));
      break;
    case NW_OPAQUE_VALUE_TYPE_STRING:
      str = ucs22str(result->value.s);
      NW_Str_Delete(result->value.s);
      val = new_str_val_and_free(&str);
      push_estack_and_free(&val);
      break;
    case NW_OPAQUE_VALUE_TYPE_BOOL:
      push_estack(new_bool_val (result->value.b));
      break;
    case NW_OPAQUE_VALUE_TYPE_FLOAT:
    case NW_OPAQUE_VALUE_TYPE_INVALID:
    default:
      push_estack(new_invalid_val());
      break;
    }
  }
  NW_Mem_Free( result );
  
  interprete();
}


void base64Encode(NW_Byte* inBuf, NW_Int32 len, NW_Ucs2* outBuf)
{
  static const NW_Byte alphabet[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  switch (len)
  {
    case 1:
      outBuf[0] = alphabet[((inBuf[0] >> 2) & 0x3F)];
      outBuf[1] = alphabet[((inBuf[0] << 4) & 0x30)];
      outBuf[2] = outBuf[3] = (NW_Ucs2)'=';
      break;
    case 2:
      outBuf[0] = alphabet[((inBuf[0] >> 2) & 0x3F)];
      outBuf[1] = alphabet[(((inBuf[0] << 4) & 0x30) | ((inBuf[1] >> 4) & 0x0F))];
      outBuf[2] = alphabet[((inBuf[1] << 2) & 0x3C)];
      outBuf[3] = (NW_Ucs2)'=';
      break;
    case 3:
      outBuf[0] = alphabet[((inBuf[0] >> 2) & 0x3F)];
      outBuf[1] = alphabet[(((inBuf[0] << 4) & 0x30) | ((inBuf[1] >> 4) & 0x0F))];
      outBuf[2] = alphabet[(((inBuf[1] << 2) & 0x3C) | ((inBuf[2] >> 6) & 0x03))];
      outBuf[3] = alphabet[(inBuf[2] & 0x3F)];
      break;
    default:
      NW_ASSERT(NW_FALSE);
  }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Name    :   Base64Encod
    Input   :   NW_Int32, NW_Byte*
    Output  :   NW_Ucs2*
    Purpose :   This function does the Base64 encoding of the KeyIdtype(which is a NW_Byte *) 
                and returns the encoded data as NW_Ucs2*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
NW_Ucs2* Base64Encod(NW_Int32 length, NW_Byte *bufToEnc) 
{
  NW_Uint32 retLength=0;
  NW_Ucs2 *encoded_data = NULL;
  if (length != 0)
  {
    retLength = ((((length + 2) / 3) * 4) * (BASE64_LINE_LEN + 1)) / BASE64_LINE_LEN + 1;
    encoded_data = (NW_Ucs2*) NW_Mem_Malloc(retLength * sizeof(NW_Ucs2));
    if (encoded_data == NULL)
      retLength = 0;
    else
    {
      NW_Uint32 i;
      NW_Int32 iIn;
      NW_Int32 iOut;
      NW_Ucs2 encoded[4];
 
      for (iIn=0,iOut=0;iIn<length;iIn+=3)
      {
        base64Encode(bufToEnc+iIn, (iIn +3 <= length) ? 3 : length-iIn, encoded);
        for (i=0 ; i<4 ; i++)
{ 
          encoded_data[iOut++] = encoded[i];
          if ((iOut % (BASE64_LINE_LEN+1)) == 0) /* iOut is at least 1 */
          {
            encoded_data[iOut++] = (NW_Ucs2)'\r';
          }
        }
      }
      encoded_data[iOut] = (NW_Ucs2)0;
    }
  }
/*  
   NW_Int32 i=0;
   NW_Int32 check = 0;
   NW_Int32 b64byte[4]; 
   NW_Int32 Outindex = 0;
   NW_Ucs2 *encoded_data = NULL;
   check = length%3;
   if(length == 0)
     return NULL;

   if(check == 0)
   {
     encoded_data = NW_Mem_Malloc(((length*4/3) + 1)*sizeof(NW_Ucs2));
   }
   else if(check == 1)
   {
     encoded_data = NW_Mem_Malloc((((length+2)*4/3) + 1)*sizeof(NW_Ucs2));
   }
   else if(check == 2)
   {
     encoded_data = NW_Mem_Malloc((((length+1)*4/3) + 1)*sizeof(NW_Ucs2));
   }
   while (length>2)
   { 
     b64byte[0]=buftoenc[i]>>2; 
     b64byte[1]=((buftoenc[i]&3)<<4)|(buftoenc[i+1]>>4); 
     b64byte[2]=((buftoenc[i+1]&0x0F)<<2)|(buftoenc[i+2]>>6); 
     b64byte[3]=buftoenc[i+2]&0x3F; 
     encoded_data[Outindex]=Base64Table[b64byte[0]]; 
     encoded_data[Outindex+1]=Base64Table[b64byte[1]]; 
     encoded_data[Outindex+2]=Base64Table[b64byte[2]]; 
     encoded_data[Outindex+3]=Base64Table[b64byte[3]]; 
     length-=3;
     Outindex+=4;
     i+=3;
   }  
   if(check == 0)
   {
     encoded_data[Outindex]= '\0';
   }
   else if(check == 2)
   {
     b64byte[0]=buftoenc[i]>>2;
     b64byte[1]=((buftoenc[i]&3)<<4)|(buftoenc[i+1]>>4);
     b64byte[2]=((buftoenc[i+1]&0x0F)<<2);
     encoded_data[Outindex]=Base64Table[b64byte[0]]; 
     encoded_data[Outindex+1]=Base64Table[b64byte[1]]; 
     encoded_data[Outindex+2]=Base64Table[b64byte[2]];
     encoded_data[Outindex+3]='=';
     encoded_data[Outindex+4]= '\0';
   }
   else
   {
     b64byte[0]=buftoenc[i]>>2;
     b64byte[1]=((buftoenc[i]&3)<<4);
     encoded_data[Outindex]=Base64Table[b64byte[0]]; 
     encoded_data[Outindex+1]=Base64Table[b64byte[1]]; 
     encoded_data[Outindex+2]='=';
     encoded_data[Outindex+3]='=';
     encoded_data[Outindex+4]='\0';
   }
*/
   return encoded_data;
} 

