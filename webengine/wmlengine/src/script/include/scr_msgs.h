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


/***************************************************************************
**   File: scr_msgs.h
**   Purpose:  Lists the data that can be moved between the WML Browser and
**              the scripting engine. Also has utility functions for creation
**        and deletion.
**************************************************************************/

#ifndef NWX_SCRIPT_MSGS_H
#define NWX_SCRIPT_MSGS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include "nwx_defs.h"
#include "nwx_http_header.h"
#include "BrsrStatusCodes.h"

/*
** Type Definitions
*/ 

typedef struct {
  NW_Ucs2         *url;
  NW_Ucs2         *postfields;
  NW_Ucs2         *referer;
  NW_Byte         *content;
  NW_Uint32       contentLen;
  NW_Http_CharSet_t charset;
} NW_StartScript_Req_t;

typedef struct {
  NW_Ucs2   *name;
  NW_Ucs2   *value;
} NW_ScriptServerVarInf_t;

typedef struct {
  NW_Ucs2   *name;
} NW_ScriptServerVarCons_t;

typedef struct {
  NW_Ucs2   *message;
  NW_Ucs2   *input;
} NW_ScriptServerDlgPromptReq_t;

typedef struct {
  NW_Ucs2     *message;
  TBrowserStatusCode status;
} NW_ScriptServerFinishReq_t;

typedef struct {
  NW_Ucs2   *message;
  NW_Ucs2   *yesStr;
  NW_Ucs2   *noStr;
  NW_Bool   response;
} NW_ScriptServerConfirmReq_t;

/*
** Global Function Declarations
*/

/* create a new script request */
NW_StartScript_Req_t*
NW_StartScript_Req_New(NW_Ucs2 *url, NW_Ucs2 *postfields, 
                       NW_Ucs2 *referer, NW_Byte *content,
                       const NW_Uint32 len, const NW_Http_CharSet_t charset);

/* free a script request */
void NW_StartScript_Req_Free(NW_StartScript_Req_t *req);

/* free a script request and all contents */
void NW_StartScript_Req_DeepFree(NW_StartScript_Req_t *req);

/* create a new variable request */
NW_ScriptServerVarCons_t* NW_Script_VarCons_New(const NW_Ucs2 *name);

/* free a variable request */
void NW_Script_VarCons_Free(NW_ScriptServerVarCons_t *var);

/* create a new variable request */
NW_ScriptServerVarInf_t* NW_Script_VarInf_New(const NW_Ucs2 *name, 
                                              const NW_Ucs2 *value);

/* set a value for the variable */
TBrowserStatusCode NW_Script_VarInf_SetValue(NW_ScriptServerVarInf_t* varinf, 
                                      NW_Ucs2 *value);

/* free a new varible request */
void NW_Script_VarInf_Free(NW_ScriptServerVarInf_t *varinf);

/* create a new dialog request */
NW_ScriptServerDlgPromptReq_t* NW_Script_DlgPrompt_Req_New(const NW_Ucs2 *message,
                                                           const NW_Ucs2 *input);
/* free a dialog request */
void NW_Script_DlgPrompt_Req_Free(NW_ScriptServerDlgPromptReq_t *dlg);

/* create a new script finish request */
NW_ScriptServerFinishReq_t* NW_Script_Finish_Req_New(TBrowserStatusCode status,
                                                     const NW_Ucs2 *message);

/* free a script finish request */
void NW_Script_Finish_Req_Free(NW_ScriptServerFinishReq_t *data);

/* create a new script confirm request */
NW_ScriptServerConfirmReq_t* NW_Script_Confirm_Req_New(const NW_Ucs2 *message,
                                                       const NW_Ucs2 *yesStr,
                                                       const NW_Ucs2 *noStr);

/* free a script confirm request */
void NW_Script_Confirm_Req_Free(NW_ScriptServerConfirmReq_t *data);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_SCRIPT_MSGS_H */
