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


#ifndef SCR_CORE_H
#define SCR_CORE_H

#include "scr_defs.h"
#include "scr_srv.h"
#include "scr_msgs.h"
#include "nw_wae_reader.h"
#include "nwx_http_header.h"
#include "BrsrStatusCodes.h"



typedef NW_Bool lib_function(void);

typedef struct {
  NW_Byte       arg_size;  /* no of args */
  lib_function  *fp;
  NW_Bool       is_async;
} lib_function_t;


#define NO_OF_BROWSER_FUNCS   7
#define NO_OF_DEBUG_FUNCS     3 
#define NO_OF_CRYPTO_FUNCS    1
#define NO_OF_DIALOG_FUNCS    3
#define NO_OF_FLOAT_FUNCS     8
#define NO_OF_LANG_FUNCS      15
#define NO_OF_STRING_FUNCS    16
#define NO_OF_URL_FUNCS       14
#define NO_OF_NOKIA_FUNCS     1

typedef const lib_function_t *lib_function_table_t[NO_OF_STD_LIBS];

void initialize_core_script(void);

/* transfer ownership of url and referer to the script interpreter. */
void CoreScript_InitiateScript(NW_Ucs2 *url,
                               NW_Ucs2 *postfields,
                               NW_Ucs2 *referer,
                               NW_Byte *content, 
                               const NW_Uint32 contentLength,
                               const NW_Http_CharSet_t charset);

void CoreScript_Suspend(void);
void CoreScript_GetVar_CB(TBrowserStatusCode status, NW_Ucs2 *var_name, NW_Ucs2 *var_value);
void CoreScript_SetVar_CB(TBrowserStatusCode status);
void CoreScript_Go_CB(TBrowserStatusCode status);
void CoreScript_Prev_CB(TBrowserStatusCode status);
void CoreScript_NewContext_CB(TBrowserStatusCode status);
void CoreScript_Refresh_CB(TBrowserStatusCode status);
 
/* Crypto Lib Callback */
void CoreScript_SignText_CB(TBrowserStatusCode status, NW_Byte* Text, NW_Int32 length);

/* transfer ownership of content to the script interpreter. */
NW_Bool CoreScript_LoadScript_CB(TBrowserStatusCode status, 
                                 NW_Ucs2 *url,
                                 NW_Byte *content, 
                                 const NW_Uint32 contentLength,
                                 const NW_Http_CharSet_t charset);

/* transfer ownership of content to the script interpreter. */
NW_Bool CoreScript_LoadString_CB(TBrowserStatusCode status,
                                 NW_Uint32 httpStatus,
                                 NW_Byte *content,
                                 const NW_Uint32 contentLength,
                                 const NW_Http_ContentTypeString_t contentTypeString,
                                 const NW_Http_CharSet_t charset);
  
void CoreScript_Resume(void);

void  NW_WmlsDialog_PromptResp(NW_Ucs2 *input);
void  NW_WmlsDialog_ConfirmResp(NW_Bool result);
void  NW_WmlsDialog_AlertResp(void);
void  NW_WmlsDialog_NotifyerrorResp(void);


void CoreScript_Wtai_CB(NW_Opaque_t *result);

void set_error_code(NW_Byte b);
NW_Byte get_error_code(void);

void set_error_msg(NW_Ucs2* msg);
NW_Ucs2* get_error_msg(void);

NW_Bool call_lib_func(NW_Uint16 lib_index, NW_Byte func_index, NW_Bool *async);

#endif /* SCR_CORE_H */



