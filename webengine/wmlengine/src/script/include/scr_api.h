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


#ifndef SCR_API_H
#define SCR_API_H

#include "scr_defs.h"
#include "scr_core.h"
#include "scr_context.h"

#include "nw_wml_defs.h"
#include "nwx_msg_api.h"
#include "BrsrStatusCodes.h"





/* Crypto Lib API. Used when the User digitally signs a Text */
NW_Bool ScriptAPI_signText(NW_Ucs2 *stringToSign, NW_Int32 option, NW_Int32 keyIdType, NW_Byte *keyIdByte, NW_Int32 keyIdLen);

/* Ask for the current context */
context_t  *ScriptAPI_getCurrentContext(void);


/*
  Make a request to load a URL. The interperter should exit after making the request.
  If the URL contains WMLScript content - 
  The response should call the callback function CoreScript_LoadScript_CB() 
*/
NW_Bool ScriptAPI_load(NW_Ucs2* url); 

/*
  Make a request to load a URL with a specific mime type. The
  interperter should exit after making the request.
  The return content is not guaranteed to have the specified mimetype.
  The response should call the callback function CoreScript_LoadScript_CB() 
*/
NW_Bool ScriptAPI_loadString(const NW_Ucs2* url, char *mimetype); 


/* Same as ScriptAPI_load called to reload the current script URL */
NW_Bool ScriptAPI_reload(void); 


/*
  Ask the WML browser for a the value of a variable. The interperter should
  exit after making the request.
  The response should call the callback function CoreScript_GetVar_CB().
*/
NW_Bool ScriptAPI_getVar(NW_Ucs2* name);


/*
  Ask the WML browser to set a variable value. The interperter should
  exit after making the request.
  The response should call the callback function CoreScript_SetVar_CB().
*/
NW_Bool ScriptAPI_setVar(NW_Ucs2* name, NW_Ucs2* value);


/*
  Ask the WML browser to execute a <GO> task when The interperter finishes executing the script.
  The interperter should exit after making the request.
  The response should call the callback function CoreScript_Go_CB().
*/
NW_Bool ScriptAPI_go(NW_Ucs2 *url);


/*
  Ask the WML browser to execute a <PREV> task when The interperter finishes executing the script.
  The interperter should  exit after making the request.
  The response should call the callback function CoreScript_Prev_CB().
*/
NW_Bool ScriptAPI_prev(void);


/*
  Ask the WML browser to execute a <REFRESH> task.
  The response should call the callback function CoreScript_Refresh_CB().
*/
NW_Bool ScriptAPI_refresh(void);

/*
  Ask the WML browser to start a new context, The interperter should
  exit after making the request.
  The response should call the callback function CoreScript_Go_CB().
*/
NW_Bool ScriptAPI_newContext(void);


/*
  Ask the WML browser to suspend itself. The interperter should
  exit after making the request.
  The response should call the callback function CoreScript_Resume().

  This function is called priodically to allow processing of pending requests.
*/
NW_Bool ScriptAPI_suspend(void);


/*
  Called when the interperter finishes executing The interperter (status determine if the
  termination was normal or abnormal). The status codes are listed in scr_defs.h
*/
NW_Bool ScriptAPI_finish(NW_Byte status, NW_Ucs2 *msg);
NW_Bool ScriptAPI_finishNoDialog(NW_Byte status, NW_Ucs2 *msg);

/* public library */
NW_Bool ScriptAPI_wp_makeCall(NW_Ucs2 *number);
NW_Bool ScriptAPI_wp_sendDTMF(NW_Ucs2 *dtmf);
NW_Bool ScriptAPI_wp_addPBEntry(NW_Ucs2 *number, NW_Ucs2 *name);

/* Nokia proprietary */
NW_Bool ScriptAPI_nokia_locationInfo(NW_Ucs2 *url, 
                                     NW_Ucs2 *ll_format, 
                                     NW_Ucs2 *pn_format);

/*
  ASK the OS to display a prompt dialog, The interperter should exit after making the request.
  The response should call the callback function NW_WmlsDialog_PromptResp().
*/
NW_Bool NW_WmlsDialog_Prompt(const NW_Ucs2 *message, const NW_Ucs2 *defaultInput);


/*
  Ask the OS to display a confirm dialog, The interperter should exit after making the request.
  The response should call the callback function NW_WmlsDialog_ConfirmResp().
*/
NW_Bool NW_WmlsDialog_Confirm(const NW_Ucs2 *message, const NW_Ucs2 *yes_str, 
                            const NW_Ucs2 *no_str);


/*
  Ask the OS to display an alert dialog, The interperter should exit after making the request.
  The response should call the callback function NW_WmlsDialog_AlertResp().
*/
NW_Bool  NW_WmlsDialog_Alert(const NW_Ucs2 *message);


/*
  Ask the OS to display an error dialog, The interperter should exit after making the request.
  The response should call the callback function NW_WmlsDialog_NotifyerrorResp().
*/
NW_Bool  NW_WmlsDialog_Error(const TBrowserStatusCode nwStatus);


/* Ask from the OS the script libraries table */
lib_function_table_t *ScriptAPI_get_lib_table(void);
NW_Int32 ScriptAPI_get_lib_index(NW_Int32 libId);

/* Added functions to avoid static function table */
lib_function_table_t* GetLibFuncTabContext(void);
void SetLibFuncTabContext(lib_function_table_t* context);

/* native character set */
NW_Int32  ScriptAPI_character_set(void);
#endif
