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
    $Workfile: scr_context.h $

    Purpose:

        This file is used by the scr_context.h.  It defines the scr_context 
        structure and the prototypes for all the functions used by the ScriptServer
        context.

 */

#ifndef SCR_CONTEXT_H
#define SCR_CONTEXT_H

#include "scr_estack.h"
#include "scr_frstack.h"
#include "scr_fcns.h"
#include "scr_constpool.h"
#include "nwx_http_header.h"

typedef enum 
{ 
  RUNNING,
  IDLE,
  SUSPEND,
  URL_REQ,
  RELOAD,
  LOADSTRING_REQ
} ctx_state_e;


typedef struct {

  NW_Ucs2 *fname;
  NW_Uint8 args;
} call_str;
  
typedef struct {

  call_str *cs;

  estack_t *es;
  frstack_t *fs;

  constpool_t *cp;
  funcpool_t *fp;
  fname_table_t *fn;

  NW_Uint16 access_path;
  NW_Uint16 access_domain;
  NW_Bool access_public;

  NW_Byte errorCode;
  ctx_state_e state;
  NW_Http_ContentTypeString_t loadStringMime;
  NW_Ucs2* msg;
  
  NW_Uint16 suspend_counter;
  
} context_t;

/* clear the context associated with the application */
void clear_url_context(context_t *ctx);

constpool_t *get_ctx_constpool(context_t *ctx);
void set_ctx_constpool(context_t *ctx, constpool_t *cp);

funcpool_t *get_ctx_funcpool(context_t *ctx);
void set_ctx_funcpool(context_t *ctx, funcpool_t *fp);

fname_table_t *get_ctx_fname_table(context_t *ctx);
void set_ctx_fname_table(context_t *ctx, fname_table_t *fn);

estack_t *get_ctx_estack(context_t *ctx);
void set_ctx_estack(context_t *ctx, estack_t *es);

frstack_t *get_ctx_frstack(context_t *ctx);
void set_ctx_frstack(context_t *ctx, frstack_t *fs);

NW_Bool set_ctx_call_str(context_t *ctx, NW_Ucs2 *fname, NW_Byte args);

NW_Uint8 get_ctx_args_no(context_t *ctx);
NW_Ucs2 *get_ctx_fname(context_t *ctx);

NW_Bool was_calling(context_t *ctx);
void delete_call_str(context_t *ctx);

void clear_context(context_t *ctx);

void set_ctx_state(context_t *ctx, ctx_state_e state);
ctx_state_e get_ctx_state(context_t *ctx);

void set_ctx_error_code(context_t *ctx, NW_Byte b);
NW_Byte get_ctx_error_code(context_t *ctx);

void set_ctx_error_msg(context_t *ctx, NW_Ucs2* msg);
NW_Ucs2* get_ctx_error_msg(context_t *ctx);

NW_Bool find_function_index(context_t *ctx, NW_Ucs2 *fname, NW_Byte *index);

NW_Bool should_suspend(context_t *ctx);

#endif  /* SCR_CONSTPOOL_H */
