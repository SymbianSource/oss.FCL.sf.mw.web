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


#include "nwx_defs.h"
#include "nwx_mem.h"
#include "nwx_string.h"
#include "nwx_msg_api.h"
#include "scr_msgs.h"
#include "urlloader_urlresponse.h"
#include "urlloader_urlloaderint.h"
#include "nwx_script_handler.h"
#include "nwx_logger.h"
#include "nwx_ctx.h"
#include "scr_core.h"
#include "scr_api.h"
#include "scr_context.h"
#include "scr_crypto.h"
#include "nwx_generic_dlg.h"
#include "nwx_osu.h"
#include "nwx_http_defs.h"
#include "nwx_time.h"
#include "BrsrStatusCodes.h"

/* Interval to suspend thread to allow other applications to run. */
#define SUSPEND_MICROSECONDS 100000

typedef struct {
  NW_Msg_Address_t address;     /* Script Interpreter */
  NW_Msg_Address_t wmlBrowser;  /* WML Browser */
  NW_Msg_Address_t loader;      /* URL Loader */
  NW_Msg_t         *finishMsg;  /* msg alloc'ed at start */
} Script_Context_t; 



/*************************** Global Data *****************************
      NOTES: For now we will create a single global variable to
      hold all of the context for the component. It will be
      accessed with the getThis() function call
*/

/************************ Access Functions **********************/
static Script_Context_t *script_getThis(void) 
{
  return (Script_Context_t*) NW_Ctx_Get(NW_CTX_SCRIPT, 0);
}

static void script_setThis(Script_Context_t *thisObj) 
{ 
  NW_Ctx_Set(NW_CTX_SCRIPT, 0, thisObj );
}

static void script_notifyError(NW_Byte status) { (void) ScriptAPI_finish(status, 0);}


/*********************** WIM messages handler *************************
      NOTES: This function receives messages that WIM operations
      completed, and feeds the messages back to the state machine.
*/

/************************ Access Functions **********************/
//lint --e{752} not referenced
TBrowserStatusCode NW_Script_Wimiresp(NW_Msg_t *msg);


/*****************************************************************
   Incoming message handlers. Parse the data meaningful to the CoreScript
   out of the message, and call the appropriate CoreScript callback
   function
*****************************************************************/

/************************************************************************
  Function: NW_Script_Startreq
  Purpose: Handles the NW_MSG_SCRIPT_START_REQ. Initializes the script
  interpreter and sends a acknowledgement.
  Parameters: msg - start script message
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Startreq(NW_Msg_t *msg)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_StartScript_Req_t *req;
  Script_Context_t *thisObj;

  if (msg == NULL) {
    return KBrsrBadInputParam;
  }
  req = (NW_StartScript_Req_t *)msg->data;
  if (req == NULL) {
    return KBrsrBadInputParam;
  }
  msg->data = NULL;/* freed by the scripting engine */

  thisObj = script_getThis();
  if(thisObj == NULL) {
    thisObj = (Script_Context_t*) NW_Mem_Malloc(sizeof(Script_Context_t));
    if (thisObj == NULL)
    {
      status = KBrsrOutOfMemory;
    }
    else
    {
      NW_Mem_memset(thisObj, 0, sizeof(Script_Context_t));
      /* Get our address and the address of all the servers  */
      NW_Msg_LookupReceiverByName(NW_SCRIPT_SERVER, &thisObj->address);
      NW_Msg_LookupReceiverByName(NW_HTTP_LOADER, &thisObj->loader);
      NW_Msg_LookupReceiverByName(NW_BROWSER_CORE, &thisObj->wmlBrowser);
      thisObj->finishMsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                                 NW_MSG_UA_FINISHNODLG_REQ);
      if (thisObj->finishMsg == NULL) {
        status = KBrsrOutOfMemory;
      }
      else {
        /*  Initialize the script libaries*/
        initialize_core_script();

        /* Now store the new context */
        script_setThis(thisObj);
      }
    }
  }

  /* 
   * Allocate a message will will be used on script completion.
   * This will be used up in the method "NW_Script_Finishnodialog"
   */
  if (thisObj != NULL && thisObj->finishMsg == NULL) {
    thisObj->finishMsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                                 NW_MSG_UA_FINISHNODLG_REQ);
    if (thisObj->finishMsg == NULL) {
      status = KBrsrOutOfMemory;
    }
  }

  /* Send response message back to the browser */
  NW_Msg_Reinit(msg, msg->tid,
                       msg->src, /* Send it back to where it came from */
                       msg->dest,
                       NW_MSG_SCRIPT_START_RESP,
                       status,
                       NULL);
  if (NW_Msg_Send(msg) != KBrsrSuccess) {
    NW_LOG0(NW_LOG_LEVEL1, "Script : Could not send message");
    NW_Msg_Delete(msg); msg = NULL;
    NW_StartScript_Req_DeepFree(req);
    return KBrsrFailure; 
  }

  if (status == KBrsrOutOfMemory)/* stop further processing if we run out of memory */
  {
    NW_StartScript_Req_DeepFree(req);
    return KBrsrOutOfMemory;
  }

  if (BRSR_STAT_IS_SUCCESS(status)) {
    set_ctx_state(ScriptAPI_getCurrentContext(), RUNNING);
    CoreScript_InitiateScript(req->url, req->postfields,
                                    req->referer, req->content,
                                    req->contentLen, req->charset);
  }
   
  /* Note: the referrer and the URL are freed by the script interpreter */
  NW_Mem_Free(req->content); req->content = NULL;
  NW_StartScript_Req_Free(req); 
  return status;
}

/************************************************************************
  Function: NW_Script_Urlloadresp
  Purpose: Handles the url load response.
  Parameters: status - status of url load
              transId - the transaction id
              loadContext - context for callback (should be NULL)
              resp - loader response
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Urlloadresp(TBrowserStatusCode status, NW_Uint16 transId, NW_Int32 chunkIndex,
                                  void *loadContext, NW_Url_Resp_t *resp)
{
  NW_Bool   noResponse = NW_FALSE;
  NW_Bool   result     = NW_FALSE;
  context_t *ctx       = ScriptAPI_getCurrentContext();

  NW_REQUIRED_PARAM(transId);
  NW_REQUIRED_PARAM(loadContext);

  // buffer response chunks until the last chunk arrives with
  // chunkIndex equals to -1.
  // then pass it to the script handler
  if( chunkIndex != -1 )
    {
    // buffer partial responses
    // pointer to the buffer is passed as loadContext
    NW_Buffer_t* partialResponseBuffer;

    partialResponseBuffer = (NW_Buffer_t*)loadContext;

    NW_ASSERT( resp != NULL );

    NW_Buffer_AppendBuffers( partialResponseBuffer, resp->body );
    }
  else
    {
    // take buffer as response body
    if( status == KBrsrSuccess && resp != NULL )
      {
      // last chunk does not contain any data
      NW_Buffer_Free( resp->body );
      resp->body = (NW_Buffer_t*)loadContext;
      }
    else
      {
      // if the load fails then release the buffer
      // free partialResponseBuffer
      NW_Buffer_Free( (NW_Buffer_t*)loadContext );
      }
    
    if (!resp || !resp->uri || !resp->body) 
      {
    noResponse = NW_TRUE;
  }
  /* Load string callback will process a non-success wsp_status */
  if (get_ctx_state(ctx) == LOADSTRING_REQ && noResponse == NW_FALSE)
  {
    result = CoreScript_LoadString_CB(status, resp->httpStatus, 
                                      resp->body->data, resp->body->length,
                                      resp->contentTypeString, resp->charset);
  }
  else if (noResponse == NW_TRUE || !BRSR_STAT_IS_SUCCESS(status) ||
           (resp->httpStatus != Success))
  { 
    set_error_code(SCR_ERROR_UNABLE_TO_LOAD_COMPILATION_UNIT);
    if (ctx->cs) {
      delete_call_str(ctx);
    }
  }
  else if (0 == NW_Asc_stricmp((char*)HTTP_application_vnd_wap_wmlscriptc_string, 
							   (char*)resp->contentTypeString)) {
    result = CoreScript_LoadScript_CB(status, resp->uri, resp->body->data, 
                                      resp->body->length, resp->charset);
  }

  if (result == NW_FALSE)
  {
    if (get_error_code() == SCR_ERROR_NO_ERROR)
    {
      set_error_code(SCR_ERROR_VERIFICATION_FAILED);
    }
    script_notifyError(get_error_code());
  } 
  }
  /* We can delete the response: scripting engine copies the content */
  UrlLoader_UrlResponseDelete(resp); 
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Goresp
  Purpose: Handles the NW_MSG_SCRIPT_GO_REQ. Restarts the script interpreter.
  Parameters: msg - go response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Goresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg != NULL);
  status = msg->status;

  NW_Msg_Delete(msg);

  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    CoreScript_Go_CB(status);
  }
  return KBrsrSuccess;
}


/* 
   Incoming message handlers. Parse the data meaningful to the CoreScript
   out of the message, and call the appropriate CoreScript callback
   function 
*/
/************************************************************************
  Function: NW_Script_Getvarresp
  Purpose: Handles the NW_MSG_COREBROWSER_GETVAR_RESP.
  Parameters: msg - get var response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Getvarresp(NW_Msg_t *msg)
{
  NW_ScriptServerVarInf_t *req;

  NW_ASSERT(msg);
  NW_ASSERT(script_getThis());

  req = (NW_ScriptServerVarInf_t *)msg->data;
  msg->data = NULL;
  NW_ASSERT(req);

  if (msg->status == KBrsrOutOfMemory) {

    /* fatal errors -- for now just out of mem is fatal. */
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    CoreScript_GetVar_CB(msg->status, req->name, req->value);
  }
  
  NW_Script_VarInf_Free(req);
  NW_Msg_Delete(msg);
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Setvarresp
  Purpose: Handles the NW_MSG_COREBROWSER_SETVAR_RESP.
  Parameters: msg - set var response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Setvarresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);
  
  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    CoreScript_SetVar_CB(status);
  }
  
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Loadprevresp
  Purpose: Handles the NW_MSG_COREBROWSER_LOADPREV_RESP.
  Parameters: msg - load prev response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Loadprevresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);

  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    CoreScript_Prev_CB(status);
  }
  
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Refreshresp
  Purpose: Handles the NW_MSG_COREBROWSER_REFRESH_RESP.
  Parameters: msg - refresh response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Refreshresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);

  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    CoreScript_Refresh_CB(status);
  }
  
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Newcontextresp
  Purpose: Handles the NW_MSG_COREBROWSER_NEWCONTEXT_RESP.
  Parameters: msg - new context response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Newcontextresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);

  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    CoreScript_NewContext_CB(status);
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Dialogalertresp
  Purpose: Handles the NW_MSG_UA_DLGALERT_RESP.
  Parameters: msg - dialog alert response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Dialogalertresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;
  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);

  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    NW_WmlsDialog_AlertResp();
  }

  return KBrsrSuccess;
}
/************************************************************************
  Function: NW_Script_Notifyerrorresp
  Purpose: Handles the NW_MSG_UA_NOTIFYERROR_RESP.
  Parameters: msg - finish response.
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Notifyerrorresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;
  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);
  if (status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    NW_WmlsDialog_NotifyerrorResp();
  }

  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Dialogconfirmresp
  Purpose: Handles the NW_MSG_UA_DLGCONFIRM_RESP.
  Parameters: msg - dialog confirm response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Dialogconfirmresp(NW_Msg_t *msg)
{
  NW_ScriptServerConfirmReq_t *data = NULL;         /* confirm message */  
  
  NW_ASSERT(msg);
  NW_ASSERT(script_getThis() != NULL);

  data = (NW_ScriptServerConfirmReq_t *)msg->data;
  msg->data = NULL;

  if (msg->status == KBrsrOutOfMemory) {
    /* fatal errors -- for now just out of mem is fatal. */
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    /* success or nonfatal error -- keep processing script */
    NW_ASSERT(data != NULL);
    NW_WmlsDialog_ConfirmResp(data->response);
  }
  NW_Script_Confirm_Req_Free (data);
  NW_Msg_Delete(msg);
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Dialogpromptresp
  Purpose: Handles the NW_MSG_UA_DLGPROMPT_RESP.
  Parameters: msg - dialog prompt response
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Dialogpromptresp(NW_Msg_t *msg)
{
  NW_Ucs2 *data;
  NW_ASSERT(msg);
  NW_ASSERT(script_getThis() != NULL);

  data = (NW_Ucs2 *)msg->data;
  if (msg->status == KBrsrOutOfMemory) {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  } else {
    NW_WmlsDialog_PromptResp(data);
    if (data) NW_Mem_Free(data);
  }

  NW_Msg_Delete(msg);
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Exitreq
  Purpose: Handles the NW_MSG_SCRIPT_EXIT_REQ. Clears the script
  context and sends an acknowledgement.
  Two alternatives for ExitReq behavior:
   1) CoreScript should control calling ExitResp, probably after it
      terminates the running script 
   2) BrowserHandler immediately send the ExitResp back to browser,
      then let CoreScript terminate the running script 
  Parameters: msg - exit request
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Exitreq(NW_Msg_t *msg)
{
  context_t *ctx = ScriptAPI_getCurrentContext();

  NW_ASSERT(script_getThis() != NULL);
  NW_ASSERT(msg != NULL);
 
  
  /* We need to cancel outstanding loader requests when exiting or Script.
  s*/
  //UrlLoader_CancelAll();
  if (was_calling(ctx)) {
    delete_call_str(ctx);
  }
  clear_context(ctx);

  /* Send response message back to the browser */
  NW_Msg_Reinit(msg, msg->tid,
                       msg->src, /* Send it back to where it came from */
                       msg->dest,
                       NW_MSG_SCRIPT_EXIT_RESP,
                       KBrsrSuccess,
                       NULL);
  if (NW_Msg_Send(msg) != KBrsrSuccess) {
    NW_LOG0(NW_LOG_LEVEL1, "Script : Could not send message");
    NW_Msg_Delete(msg);
    return KBrsrFailure; 
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Resumereq
  Purpose: Handles the NW_MSG_SCRIPT_RESUME_REQ.
  Parameters: msg - resume request
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Resumereq(NW_Msg_t *msg)
{
  TBrowserStatusCode status;
  NW_ASSERT(msg != NULL);

  status = msg->status;
  NW_Msg_Delete(msg);

  NW_ASSERT(script_getThis() != NULL);

  if (BRSR_STAT_IS_SUCCESS(status))
  {
    CoreScript_Resume();
  }
  else
  {
    /* abort the script interpreter */
    script_notifyError(SCR_ERROR_USER_ABORT);
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Finishnodialogresp
  Purpose: Handles the NW_MSG_UA_FINISHNODLG_RESP.
  Parameters: msg - finish response.
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Finishnodialogresp(NW_Msg_t *msg)
{
  context_t *ctx = ScriptAPI_getCurrentContext();

  /* ignoring the message status and clearing the script context */
  NW_ASSERT(msg != NULL);

  if (was_calling(ctx)) {
    delete_call_str(ctx);
  }
  clear_context(ctx);
  NW_Msg_Delete(msg);
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Suspendresp
  Purpose: Handles the NW_MSG_SCRIPT_SUSPEND_RESP.
  Parameters: msg - suspend response.
  Return Values:
  KBrsrSuccess
*************************************************************************/
TBrowserStatusCode NW_Script_Suspendresp(NW_Msg_t *msg)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg);
  status = msg->status;
  NW_Msg_Delete(msg);

  if (BRSR_STAT_IS_SUCCESS(status))
  {
    CoreScript_Resume();
  }

  return KBrsrSuccess;
}

/*****************************************************************
   Outgoing message handlers.
*****************************************************************/

/************************************************************************
  Function: NW_Script_Getvar
  Purpose: Sends a request to the WML Browser for the value of a variable.
  Parameters:  name - the name of the variable
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
**************************************************************************/

TBrowserStatusCode NW_Script_Getvar(NW_Ucs2* name)
{
  NW_Msg_t *cmsg = NULL;
  NW_ScriptServerVarInf_t *data;
  Script_Context_t *thisObj;

  NW_ASSERT(name != NULL);

  thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);

  data = NW_Script_VarInf_New(name, NULL);
  if (data == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_COREBROWSER_GETVAR_REQ);
  if (cmsg == 0)
  {
    NW_Script_VarInf_Free(data);
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg->status = KBrsrSuccess;
  cmsg->data = data;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Script_VarInf_Free(data);
    cmsg->data = NULL;
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Setvar
  Purpose: Sends a request to the WML Browser to set the variable value.
  Parameters:  name - the name of the variable
                value - value of the variable
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
**************************************************************************/

TBrowserStatusCode NW_Script_Setvar(NW_Ucs2* name, NW_Ucs2* value)
{
  NW_Msg_t *cmsg = NULL;
  NW_ScriptServerVarInf_t *data;
  Script_Context_t *thisObj;

  NW_ASSERT(name != NULL);

  thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);

  data = NW_Script_VarInf_New(name, value);
  if (data == NULL)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_COREBROWSER_SETVAR_REQ);
  if (cmsg == 0)
  {
    NW_Script_VarInf_Free(data);
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg->data = data;
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Script_VarInf_Free(data);
    cmsg->data = NULL;
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_LoadPrev
  Purpose: Sends a request to the WML Browser to execute a <PREV> task.
  Parameters: 
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
**************************************************************************/
TBrowserStatusCode NW_Script_Loadprev(void)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_COREBROWSER_LOADPREV_REQ);
  if (cmsg == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1, "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Refresh
  Purpose: Sends a request to the WML Browser to execute a <REFRESH> task.
  Parameters: 
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
**************************************************************************/
TBrowserStatusCode NW_Script_Refresh(void)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_COREBROWSER_REFRESH_REQ);
  if (cmsg == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1, "Scripting Engine : Could not send Refresh message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Newcontext
  Purpose: Sends a request to the WML Browser to start a new context.
  Parameters: 
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Newcontext(void)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_COREBROWSER_NEWCONTEXT_REQ);
  if (cmsg == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Dialogalert
  Purpose: Sends a request to the User agent to display a alert dialog.
  Parameters: 
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Dialogalert(const NW_Ucs2 *message)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;
  NW_Ucs2 *data;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);
  NW_ASSERT(message);

  data = NW_Str_Newcpy(message);
  if (data == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_UA_DLGALERT_REQ);
  if (cmsg == 0)
  {
    NW_Str_Delete(data);
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg->data = data;
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Str_Delete(data);
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Dialogconfirm
  Purpose: Sends a request to the User agent to display a confirm dialog.
  Parameters: message - message to be displayed
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Dialogconfirm(const NW_Ucs2 *message, const NW_Ucs2 *yesStr, 
                                    const NW_Ucs2 *noStr)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t* thisObj;
  NW_ScriptServerConfirmReq_t *data;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);
  NW_ASSERT(message);

  data = NW_Script_Confirm_Req_New(message, yesStr, noStr);
  if (data == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
 
  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_UA_DLGCONFIRM_REQ);
  if (cmsg == NULL) {
    NW_Script_Confirm_Req_Free(data);
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg->data = data;
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Script_Confirm_Req_Free(data);
    cmsg->data = NULL;
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Dialogprompt
  Purpose: Sends a request to the User agent to display a prompt dialog.
  Parameters: message - the message to be displayed
              defaultInput - the default input value
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Dialogprompt(const NW_Ucs2 *message, const NW_Ucs2 *defaultInput)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t* thisObj;
  NW_ScriptServerDlgPromptReq_t* data;

  thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);/* script interpreter should be initialized */

  data = NW_Script_DlgPrompt_Req_New(message, defaultInput);
  if (data == 0)/* stop processing */
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_UA_DLGPROMPT_REQ);
  if (cmsg == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg->data = data;
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Script_DlgPrompt_Req_Free(data);
    cmsg->data = NULL;
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Load
  Purpose: Requests the URl loader to load the Url.
  Parameters: url - the url to load
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Load(NW_Ucs2 *url)
{
  NW_Uint16 tid = 0;/* dont care about transaction ids */
  TBrowserStatusCode status;
  NW_Buffer_t* partialResponseBuffer;
  
  NW_ASSERT(url != NULL);

  /* Do the load. When the loader finishes it invokes the
     NW_Script_Urlloadresp() function with a callback context that is 
     there in NW_Url_StartLoad call. Currently loadContext is NW_SCRIPT_SERVER.
     It should be some other value that is meant only for script and no
     other applicaiton would be using it.
  */
 
  // this partialResponseBuffer is being used as a buffer in 
  // the response callback function
  partialResponseBuffer = NW_Buffer_New( 1 ); 
  status = UrlLoader_Request(url, NULL, NULL, 0, NW_URL_METHOD_GET, &tid,
      (void*)partialResponseBuffer, (NW_Url_RespCallback_t*)NW_Script_Urlloadresp,
      NW_CACHE_NORMAL, NW_DEFAULTHEADERS_ALL, NW_UrlRequest_Type_Any, NW_FALSE,  NW_TRUE,
      NW_HED_UrlRequest_Reason_DocLoad, NULL);

  if (status != KBrsrSuccess && status != KBrsrHedContentDispatched && status != KBrsrScriptErrorUserAbort)
  {
    NW_Buffer_Free( partialResponseBuffer );
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return status;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Go
  Purpose: Sends a request to the WML Browser to execute a <go> task.
  Parameters: url - the url to load
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Go(NW_Ucs2 *url)
{
  NW_Msg_t *cmsg = NULL;
  NW_Ucs2 *data;
  Script_Context_t *thisObj;

  thisObj = script_getThis();
  NW_ASSERT(url);
  NW_ASSERT(thisObj);

  data = (NW_Ucs2*) NW_Mem_Malloc((NW_Str_Strlen(url) + 1) * sizeof(NW_Ucs2));
  if (data == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  NW_Str_Strcpy(data, url);
  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_COREBROWSER_GO_REQ);
  if (cmsg == 0)
  {
    NW_Mem_Free(data);
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }

  cmsg->data = data;
  cmsg->status = KBrsrSuccess;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Mem_Free(data);
    cmsg->data = NULL;
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Notifyerror
  Purpose: Sends a request to the User agent to display a error.
  Parameters: 
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Notifyerror(TBrowserStatusCode status)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_UA_NOTIFYERROR_REQ);
  if (cmsg == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg->status = status;
  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Finishnodialog
  Purpose: Sends a request to User agent indicating that the script
      interpreter has completed.  NOTE!!  script_NotifyError() is already
      on the call stack, sooo, if we run out of memory, DON'T MAKE A
      RECURSIVE CALL to script_NotifyError().
      
  Parameters: 
  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Finishnodialog(NW_Byte status, NW_Ucs2 *msg)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;
  NW_ScriptServerFinishReq_t *data = NULL;
  TBrowserStatusCode errorCode;

  thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);
  
  if (thisObj->finishMsg != NULL){
    cmsg = thisObj->finishMsg;
    thisObj->finishMsg = NULL;
  }
  cmsg->status = KBrsrSuccess;
  switch (status) /* map the script interpreter internal status codes to TBrowserStatusCode */
  {
    case SCR_ERROR_NO_ERROR:
      errorCode = KBrsrSuccess;
      break;
    case SCR_ERROR_VERIFICATION_FAILED:
      errorCode = KBrsrScriptBadContent;
      break;
    case SCR_ERROR_FATAL_LIBRARY_FUNCTION_ERROR:
      errorCode = KBrsrScriptFatalLibFuncError;
      break;
    case SCR_ERROR_INVALID_FUNCTION_ARGUMENTS:
      errorCode = KBrsrScriptInvalidFuncArgs;
      break;
    case SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND:
    case SCR_ERROR_UNABLE_TO_LOAD_COMPILATION_UNIT:
      errorCode = KBrsrScriptCardNotInDeck;
      break;
    case SCR_ERROR_ACCESS_VIOLATION:
      errorCode = KBrsrScriptNoAccess;
      break;
    case SCR_ERROR_STACK_UNDERFLOW:
      errorCode = KBrsrScriptErrorStackUnderflow;
      break;
    case SCR_ERROR_STACK_OVERFLOW:
      errorCode = KBrsrScriptErrorStackOverflow;
      break;
    case SCR_ERROR_OUT_OF_MEMORY:
      errorCode = KBrsrOutOfMemory;
      break;

    case SCR_ERROR_USER_EXIT:
      errorCode = KBrsrScriptErrorUserExit;
      break;
    case SCR_ERROR_USER_ABORT:
      errorCode = KBrsrScriptErrorUserAbort;
      break;
    default:
      /* assertion to catch unmapped status codes. */
      NW_ASSERT(0);
      errorCode = KBrsrFailure;
  }
  data = NW_Script_Finish_Req_New(errorCode, msg);
 
  cmsg->data = data;

  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    if (data) NW_Script_Finish_Req_Free(data);
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1,
            "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Script_Suspend
  Purpose: Sends a request to script interpreter to yield. 
  Parameters: 

  Return Values:
  KBrsrSuccess
  KBrsrFailure
  KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_Suspend(void)
{
  NW_Msg_t *cmsg = NULL;
  Script_Context_t *thisObj;

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  /* Suspend thread briefly to allow other applications to run. */
  NW_Osu_Wait(SUSPEND_MICROSECONDS);

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_SCRIPT_SUSPEND_REQ);
  if (cmsg == 0)
  {
    script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
    return KBrsrOutOfMemory;
  }
  cmsg->status = KBrsrSuccess;

  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
    NW_LOG0(NW_LOG_LEVEL1, "Scripting Engine : Could not send message");
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}


/********************************/
/******** WTA Functions *********/
/********************************/

/***************************************************************
 * Function: NW_Script_wp_makeCall 
 * Purpose:  Sends a request to the Browser
 ***************************************************************/

TBrowserStatusCode NW_Script_wp_makeCall(NW_Ucs2 *number)
{
  NW_Msg_t          *msg = NULL;
  NW_Ucs2           * url = NULL;
  Script_Context_t  *thisObj;
  const char* const makeCallUrlPrefix = "wtai://wp/mc;";
  NW_Opaque_t       *result = NULL;
  TBrowserStatusCode       status = KBrsrSuccess;
  
  NW_LOG0(NW_LOG_LEVEL5, "SCRIPT : NW_Script_wp_makeCall\n");

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  url = (NW_Ucs2*) NW_Mem_Malloc((NW_Asc_strlen(makeCallUrlPrefix) + NW_Str_Strlen(number) + 1) * sizeof(NW_Ucs2));
  msg = NW_Msg_New(1, thisObj->address, thisObj->address, NW_MSG_SCRIPT_WP_MAKECALL_RESP);  
  result = (NW_Opaque_t*) NW_Mem_Malloc( sizeof( NW_Opaque_t ) );

  if (url != NULL && msg != NULL && result != NULL)
  {
    result->type = NW_OPAQUE_VALUE_TYPE_STRING;
    result->value.s = NULL;

    msg->status = KBrsrSuccess;
    msg->data = result;

    NW_Str_StrcpyConst(url, makeCallUrlPrefix);
    NW_Str_Strcat(url, number);
    status = NW_Script_Load(url);
    /* The WTAI loader should copy the URL string */
    NW_Mem_Free(url);
    url = NULL;
    if (status == KBrsrHedContentDispatched)
    {
        status = KBrsrSuccess;
    }
  }
  else
  {
    status =  KBrsrOutOfMemory;
  }

  if (status == KBrsrSuccess)
  {
      status = NW_Msg_Send(msg);
  }

  if (status != KBrsrSuccess)
  {
    NW_Mem_Free( result );
    NW_Mem_Free( url );
    NW_Mem_Free( msg );
  }
  return status;
}

/***************************************************************
 * Function: NW_Script_wp_sendDTMF 
 * Purpose:  Sends a request to the Browser
 ***************************************************************/

TBrowserStatusCode NW_Script_wp_sendDTMF(NW_Ucs2 *dtmf)
{
  NW_Msg_t          *msg = NULL;
  NW_Ucs2           * url = NULL;
  Script_Context_t  *thisObj;
  const char* const sendDtmfUrlPrefix = "wtai://wp/sd;";
  NW_Opaque_t       *result = NULL;
  TBrowserStatusCode       status = KBrsrSuccess;
  
  NW_LOG0(NW_LOG_LEVEL5, "SCRIPT : NW_Script_wp_sendDTMF\n");

  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  url = (NW_Ucs2*) NW_Mem_Malloc((NW_Asc_strlen(sendDtmfUrlPrefix) + NW_Str_Strlen(dtmf) + 1) * sizeof(NW_Ucs2));
  msg = NW_Msg_New(1, thisObj->address, thisObj->address, NW_MSG_SCRIPT_WP_SENDDTMF_RESP);  
  result = (NW_Opaque_t*) NW_Mem_Malloc( sizeof( NW_Opaque_t ) );

  if (url != NULL && msg != NULL && result != NULL)
  {
    result->type = NW_OPAQUE_VALUE_TYPE_STRING;
    result->value.s = NULL;

    msg->status = KBrsrSuccess;
    msg->data = result;

    NW_Str_StrcpyConst(url, sendDtmfUrlPrefix);
    NW_Str_Strcat(url, dtmf);
    status = NW_Script_Load(url);
    /* The WTAI loader should copy the URL string */
    NW_Mem_Free(url);
    url = NULL;
    if (status == KBrsrHedContentDispatched)
    {
        status = KBrsrSuccess;
    }
  }
  else
  {
    status =  KBrsrOutOfMemory;
  }

  if (status == KBrsrSuccess)
  {
      status = NW_Msg_Send(msg);
  }

  if (status != KBrsrSuccess)
  {
    NW_Mem_Free( result );
    NW_Mem_Free( url );
    NW_Mem_Free( msg );
  }
  return status;
}

/***************************************************************
 * Function: NW_Script_wp_addPBEntry 
 * Purpose:  Sends a request to the Browser
 ***************************************************************/

TBrowserStatusCode NW_Script_wp_addPBEntry(NW_Ucs2 *number, NW_Ucs2 *name)
{
  NW_Msg_t          *msg = NULL;
  NW_Ucs2           * url = NULL;
  Script_Context_t  *thisObj;
  const char* const pbEntryUrlPrefix = "wtai://wp/ap;";
  const char* const numberNameSeparator = ";";
  NW_Opaque_t       *result = NULL;
  TBrowserStatusCode       status = KBrsrSuccess;
  
  NW_LOG0(NW_LOG_LEVEL5, "SCRIPT : NW_Script_wp_PBEntry\n");
  
  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  url = (NW_Ucs2*) NW_Mem_Malloc((NW_Asc_strlen(pbEntryUrlPrefix) + NW_Str_Strlen(number) +
      NW_Str_Strlen(name) + 2) * sizeof(NW_Ucs2));
  msg = NW_Msg_New(1, thisObj->address, thisObj->address, NW_MSG_SCRIPT_WP_ADDPBENTRY_RESP);  
  result = (NW_Opaque_t*) NW_Mem_Malloc( sizeof( NW_Opaque_t ) );

  if (url != NULL && msg != NULL && result != NULL)
  {
    result->type = NW_OPAQUE_VALUE_TYPE_STRING;
    result->value.s = NULL;

    msg->status = KBrsrSuccess;
    msg->data = result;

    NW_Str_StrcpyConst(url, pbEntryUrlPrefix);
    NW_Str_Strcat(url, number);
    NW_Str_StrcatConst(url, numberNameSeparator);
    NW_Str_Strcat(url, name);
    status = NW_Script_Load(url);
    /* The WTAI loader should copy the URL string */
    NW_Mem_Free(url);
    url = NULL;
    if (status == KBrsrHedContentDispatched)
    {
        status = KBrsrSuccess;
    }
  }
  else
  {
    status =  KBrsrOutOfMemory;
  }

  if (status == KBrsrSuccess)
  {
      status = NW_Msg_Send(msg);
  }

  if (status != KBrsrSuccess)
  {
    NW_Mem_Free( result );
    NW_Mem_Free( url );
    NW_Mem_Free( msg );
  }
  return status;
}

/***************************************************************
 * Function: NW_Script_nokia_locationInfo
 * Purpose:  Sends a request to the Browser
 ***************************************************************/

TBrowserStatusCode NW_Script_nokia_locationInfo(NW_Ucs2 *url, 
                                         NW_Ucs2 *ll_format,
                                         NW_Ucs2 *pn_format)
{
  NW_Msg_t          *cmsg = NULL;
  Script_Context_t  *thisObj = NULL;
  void              **args = NULL;
  TBrowserStatusCode       status = KBrsrSuccess;
  
  thisObj = script_getThis();
  NW_ASSERT(thisObj);

  args = (void**)NW_Mem_Malloc(sizeof(void*) * 3);
  if (args != NULL) {
    args[0] = (void*)url;
    args[1] = (void*)ll_format;
    args[2] = (void*)pn_format;
  } else {
    status = KBrsrOutOfMemory;
    goto failure;
  }

  cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                    NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_REQ);  
  if (cmsg == NULL) {
    status = KBrsrOutOfMemory;
    goto failure;
  }

  cmsg->status = KBrsrSuccess;
  cmsg->data = args;

  if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
    NW_Msg_Delete( cmsg );
    NW_LOG0(NW_LOG_LEVEL1, "Scripting Engine : Could not send message");
    status = KBrsrFailure;
    goto failure;
  }
  return KBrsrSuccess;

failure:
  NW_Mem_Free( url );
  NW_Mem_Free( ll_format );
  NW_Mem_Free( pn_format );
  NW_Mem_Free( args );

  script_notifyError(SCR_ERROR_OUT_OF_MEMORY);
  return status;

}

/************************************************
 * WTAI Response Handler
 * 
 * This handler is called as a response to every
 * WTAI request.
 ************************************************/

TBrowserStatusCode NW_Script_wtaiResp(NW_Msg_t *msg)
{
  NW_LOG0(NW_LOG_LEVEL5, "SCRIPT : NW_Script_wtaiResp\n"); 
  NW_ASSERT(msg != NULL);

  switch(msg->type)
  {
    case NW_MSG_SCRIPT_WP_MAKECALL_RESP:
    case NW_MSG_SCRIPT_WP_SENDDTMF_RESP:
    case NW_MSG_SCRIPT_WP_ADDPBENTRY_RESP:
    {
      NW_Opaque_t *result = (NW_Opaque_t*) msg->data;
      NW_ASSERT(result != NULL);
      CoreScript_Wtai_CB(result);
      break;
    }
    default:
        NW_ASSERT(0);
  }
  NW_Msg_Delete(msg);

  return KBrsrSuccess;
}



/***********************************/
/******** Crypto Functions *********/
/***********************************/

/************************************************************************
  Function: NW_Script_CryptoSignTextErrorCleanup
  Purpose: Cleanup in case of an error 
  Parameters: params - The pointer to free
              status - The error code to display to the user

  Return Values: none
*************************************************************************/
void NW_Script_CryptoSignTextErrorCleanup(SignTextParams_t* params, 
                                          TBrowserStatusCode status)
{
#ifndef PROFILE_NO_WTLS  
  NW_Msg_t *msg;
  Script_Context_t* thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);/* script interpreter should be initialized */
  NW_ScrCrypto_SignTextParamsFree(params);
  if ((msg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser , NW_MSG_UA_DISPLAYERROR_REQ))!=NULL)
  {
    msg->status = status;
    NW_Msg_Send(msg);
  }
  CoreScript_SignText_CB(status, NULL, 0);
#else
  NW_REQUIRED_PARAM(status);
  NW_REQUIRED_PARAM(params);
  CoreScript_SignText_CB(KBrsrSuccess, NULL, 0);

#endif /*PROFILE_NO_WTLS*/
}

/************************************************************************
  Function: NW_Script_CryptoSignTextStart
  Purpose: Sends a request to script proxy to display the text to sign. 
  Parameters: 

  Return Values: KBrsrSuccess , KBrsrFailure or KBrsrOutOfMemory
*************************************************************************/
TBrowserStatusCode NW_Script_CryptoSignTextStart(NW_Ucs2 *stringToSign, NW_Int32 option, 
                                    NW_Int32 keyIdType, NW_Byte *keyIdByte, NW_Int32 keyIdLen)
{
#ifndef PROFILE_NO_WTLS  
  NW_Msg_t *cmsg = NULL;
  Script_Context_t* thisObj;
  SignTextParams_t* params = NULL;
  NW_Dlg_Prompt_t* data = NULL;
  TBrowserStatusCode status = KBrsrSuccess;
  const NW_Bool contDo=NW_FALSE;
  thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);/* script interpreter should be initialized */
  do 
  {
    if (stringToSign == NULL || NW_Str_Strlen(stringToSign) <1)
    {
      status = KBrsrScriptInvalidFuncArgs;
      break;
    }
    if (keyIdType < SignatureKeyIdentifierNone || keyIdType > SignatureKeyIdentifierTrustedKeyHash)
    {
      status = KBrsrScriptInvalidFuncArgs;
      break;
    }
    if (option < 0 || option > (SignatureIncludeContent | SignatureIncludeKeyHash | SignatureIncludeCertificate))
    {
      status = KBrsrScriptInvalidFuncArgs;
      break;
    }
    if (keyIdType != SignatureKeyIdentifierNone && (keyIdByte == NULL || keyIdLen == 0))
    {
      status = KBrsrScriptInvalidFuncArgs;
      break;
    }

    params = NW_ScrCrypto_SignTextParamsNew(SignStateInit, stringToSign, option, keyIdType, 
      keyIdByte, keyIdLen, 0, NULL, NULL, NULL);
    if (params != NULL )
    {
      if ((data = NW_Dlg_PromptInit(NULL,stringToSign,UI_OPTION_TEXT_TO_SIGN, SIGN_BUTTON_INDEX,  
      CANCEL_BUTTON_INDEX, 0, params)) != NULL)
      {
        cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                      NW_MSG_UA_DIALOGPROMPT_REQ);
      }
    }
    if (cmsg == NULL)
    {
      status = KBrsrOutOfMemory;
    }
    else
    {
      cmsg->data = data;
      cmsg->status = KBrsrSuccess;
      if (NW_Msg_Send(cmsg) != KBrsrSuccess) {
        cmsg->data = NULL;
        NW_Msg_Delete(cmsg);
        NW_LOG0(NW_LOG_LEVEL1,
              "Scripting Engine : Could not send message");
        status = KBrsrFailure;
      }
    }
  }
  while(contDo);
  if (status != KBrsrSuccess)
  {
    NW_Script_CryptoSignTextErrorCleanup(params, status);
    NW_Mem_Free(data);
    NW_Mem_Free(cmsg);
  }
  return KBrsrSuccess;
#else /*PROFILE_NO_WTLS*/
  NW_REQUIRED_PARAM(keyIdLen);
  NW_REQUIRED_PARAM(keyIdByte);
  NW_REQUIRED_PARAM(keyIdType);
  NW_REQUIRED_PARAM(option);
  NW_REQUIRED_PARAM(stringToSign);

  /* errorCleanup() will push invalid_val on the stack */
  /* and restart the interprete() loop. just return SUCCESS */
  /* and since signtext is async current execution of */
  /* interprete() loop will terminate */
  NW_Script_CryptoSignTextErrorCleanup(NULL, KBrsrFailure);

  return KBrsrSuccess;
#endif /*PROFILE_NO_WTLS*/
}

//#pragma message("TODO:: nwx_script_handler.c, enable SignText")
#if 0
#ifndef PROFILE_NO_WTLS
static NW_Bool signStateInit(SignTextParams_t* params, void* Ctx)
{
  NW_Dlg_ListSelect_t* listSelectData=NULL;
  NW_Dlg_Prompt_t* promptData=NULL;
  NW_Ucs2** labels=NULL;
  NW_Bool cont=NW_TRUE;
  const NW_Bool contDo=NW_FALSE;
  NW_Uint8 i;
  TBrowserStatusCode status=KBrsrSuccess;
  NW_Msg_t *cmsg = NULL;
  Script_Context_t* thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);/* script interpreter should be initialized */
  NW_ASSERT(Ctx!=NULL);
  promptData = (NW_Dlg_Prompt_t*)Ctx;
  do
  {
    if (promptData->selectedButton == 0)
    {
      /* The user chose to sign the text */
      switch (params->keyIdType)
      {
       case SignatureKeyIdentifierNone:
        status = NW_Wim_GetCertificateListByWIM(NULL, NW_WIMI_CU_Client,
                                                &params->certNum, &params->certificatesList);
        break;
       case SignatureKeyIdentifierUserKeyHash:
        {
          status = NW_Wim_GetCertificateListByKeyHash(params->keyIdByte ,
                                                      NW_WIMI_CU_Client, &params->certNum, &params->certificatesList);
          break;
        }
       case SignatureKeyIdentifierTrustedKeyHash:
        status = NW_Wim_GetCertificateListByCAKeyHash(params->keyIdByte ,
                                                      NW_WIMI_CU_Client, &params->certNum,
                                                      &params->certificatesList);
        break;
       default:
          /* signText got invalid parameter.
             Already checked parameters before, so should not get here*/
             NW_ASSERT(NW_FALSE);
             NW_Script_CryptoSignTextErrorCleanup(params,KBrsrScriptInvalidFuncArgs);
             cont=NW_FALSE;
             break;
      }
      if (status != KBrsrSuccess)
      {
        NW_Script_CryptoSignTextErrorCleanup(params, status);
        cont=NW_FALSE;
        break;
      }
      /* Get only non-repudiation keys */
      if ((status = NW_Wim_GetCertListByKeyUsage(&(params->certificatesList),
                                                 &(params->certNum), NW_PKCS15_KU_sign)) != KBrsrSuccess)
      {
        NW_Script_CryptoSignTextErrorCleanup(params, status);
        cont=NW_FALSE;
        break;
      }
      
      /*
       * if there are no matching certificates, abort.
       * if there is one matching certificate, continue.
       * if there are multiple matching certificates, let the user select which one to use.
       */
       if (params->certNum == 0)
      {
        NW_Script_CryptoSignTextErrorCleanup(params,KBrsrCryptoNoCert);
        cont=NW_FALSE;
      } else {
   
        {
          if ((status = NW_Wim_GetLabelsFromRef(params->certNum,
                                                params->certificatesList, &labels)) != KBrsrSuccess )
          {
            if (labels == NULL)
            {
              params->certNum = 0;
            }
            break;
          }

          listSelectData = NW_Dlg_ListSelectInit(NULL, NULL, UI_OPTION_CERT_LIST_SIGN_TEXT, params->certNum, labels,
                                                 SELECT_BUTTON_INDEX,  CANCEL_BUTTON_INDEX,
                                                 0, 0, params);
          params->state=SignStateGetSelectedCert;

          cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                            NW_MSG_UA_DIALOGLISTSELECT_REQ);
          if (listSelectData == NULL || cmsg == NULL)
          {
            status = KBrsrOutOfMemory;
            break;
          }
          cmsg->data = listSelectData;
          cmsg->status = KBrsrSuccess;
          if (NW_Msg_Send(cmsg) != KBrsrSuccess)
          {
            status = KBrsrFailure;
            NW_LOG0(NW_LOG_LEVEL1, "Scripting Engine : Could not send message");
          }
          cont=NW_FALSE;
        } 

        if (status !=KBrsrSuccess)
        {
          for (i=0;i<params->certNum;i++)
          {
            NW_Mem_Free(labels[i]);
          }
          NW_Mem_Free(labels);
          NW_Mem_Free(listSelectData);
          listSelectData=NULL;
          cmsg->data = NULL;
          NW_Msg_Delete(cmsg);
          cmsg=NULL;
          cont=NW_FALSE;
          NW_Script_CryptoSignTextErrorCleanup(params, status);
          break;
        }
      }
    }
    else
    {
      NW_ScrCrypto_SignTextParamsFree(params);
      CoreScript_SignText_CB(KBrsrCryptoUserCancel, NULL, 0);
      cont=NW_FALSE;
    }
  }
  while (contDo);
  NW_Mem_Free(promptData);
  promptData = NULL;
  return cont;
}

static NW_Bool signStateGetSelectedCert(SignTextParams_t* params, void* Ctx)
{
  NW_Dlg_ListSelect_t* listSelectData=NULL;
  NW_Ucs2** labels=NULL;
  NW_Bool cont=NW_TRUE;
  NW_Uint8 i;
  NW_ASSERT(Ctx!=NULL);
  listSelectData = (NW_Dlg_ListSelect_t*)Ctx;
  if (listSelectData->selectedButton == 0) /* The user chose the sign button, not the cancel */
  {
    params->selectedCertRef= params->certificatesList[listSelectData->selectedItem];
    params->certificatesList[listSelectData->selectedItem] = NULL;
    NW_Wim_free_RefList(params->certificatesList);
    params->certificatesList=NULL;
    params->certNum=0;
    params->state = SignStateGetPIN;
  }
  else
  {
    NW_ScrCrypto_SignTextParamsFree(params);
    CoreScript_SignText_CB(KBrsrCryptoUserCancel, NULL, 0);
    cont=NW_FALSE;
  }
  labels = listSelectData->items;
  for (i=0;i<listSelectData->itemsCount;i++)
  {
    NW_Mem_Free(labels[i]);
  }
  NW_Mem_Free(labels);                
  NW_Mem_Free(listSelectData);
  return cont;
}

static void signStateGetPIN(SignTextParams_t* params)
{
  NW_Dlg_InputPrompt_t* inputPromptData=NULL;
  const NW_Bool contDo=NW_FALSE;
  TBrowserStatusCode status=KBrsrSuccess;
  NW_Msg_t *cmsg = NULL;
  void* PINRef=NULL;
  void* WIMRef=NULL;
  void* refList[3] = {NULL, NULL, NULL}; /* reference lists are NULL terminated */
  NW_Ucs2** labelList = NULL;
  NW_Ucs2* tmp;

  NW_Ucs2* input=NW_Mem_Malloc((MAX_PIN_LEN+1)*sizeof(NW_Ucs2));
  Script_Context_t* thisObj = script_getThis();
  NW_ASSERT(thisObj != NULL);/* script interpreter should be initialized */
  do
  {
    if (input == NULL)
    {
      status = KBrsrOutOfMemory;
      break;
    }
    input[0]=0;
    if ((status = NW_Wim_GetCertInfo(params->selectedCertRef, &WIMRef, 
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) != KBrsrSuccess)
      break;

    if ((status = NW_Wim_GetPINRefFromCertRef(params->selectedCertRef, &PINRef)) != KBrsrSuccess)
      break;

    /* Get the WIM and PIN label */
    refList[0]=PINRef;
    refList[1]=WIMRef;
    if ((status = NW_Wim_GetLabelsFromRef(2,refList,&labelList))!=KBrsrSuccess)
    {
      break;
    }
    inputPromptData = NW_Dlg_InputPromptInit(labelList[1], labelList[0], NULL, NULL, UI_OPTION_ENTER_PIN_SIGN_TEXT, 
      NW_TRUE, NW_TRUE,
      ENTER_BUTTON_INDEX,CANCEL_BUTTON_INDEX,
      MAX_PIN_LEN+1,input,0,params);
    params->state=SignStateGetSignature;
    cmsg = NW_Msg_New(1, thisObj->address, thisObj->wmlBrowser,
                      NW_MSG_UA_DIALOGINPUTPROMPT_REQ);
    if (inputPromptData == NULL || cmsg == NULL)
    {
      status = KBrsrOutOfMemory;
      break;
    }
    cmsg->data = inputPromptData;
    cmsg->status = KBrsrSuccess;
    if (NW_Msg_Send(cmsg) != KBrsrSuccess) 
    {
      status = KBrsrFailure;
      NW_Mem_Free(inputPromptData);
      cmsg->data = NULL;
      (void) NW_Msg_Delete(cmsg);
      NW_LOG0(NW_LOG_LEVEL1,
              "Scripting Engine : Could not send message");
    }
  }
  while (contDo);
  NW_Wim_free_Ref(refList[0]);
  NW_Wim_free_Ref(refList[1]);

  if (status !=KBrsrSuccess)
  {
    if (inputPromptData != NULL)
    {
      NW_Mem_Free(inputPromptData->input);
      tmp = (NW_Ucs2*)inputPromptData->title;
      NW_Mem_Free(tmp);
      tmp = (NW_Ucs2*)inputPromptData->prompt;
      NW_Mem_Free(tmp);
      NW_Mem_Free(inputPromptData);
    }
    if (cmsg != NULL)
    {
      cmsg->data = NULL;
      NW_Msg_Delete(cmsg);
      cmsg=NULL;
    }
    NW_Script_CryptoSignTextErrorCleanup(params, status);
  }
  NW_Mem_Free(labelList);
}

static void signStateGetSignature(SignTextParams_t* params, void* Ctx)
{
  NW_Dlg_InputPrompt_t* inputPromptData=NULL;
  const NW_Bool contDo=NW_FALSE;
  NW_Uint8 i;
  TBrowserStatusCode status=KBrsrSuccess;
  NW_Ucs2* tmp;
  SignedContent *signedContent = NULL;
  AuthenticatedAttribute *authenticatedAttribute = NULL;
  NW_Time_Tm_t *universalTime = NULL;
  NW_Uint16 authenticatedAttributeLen = 14; /* we only support GMT time */
  NW_Uint8 *pTime;

  NW_ASSERT(Ctx!=NULL);
  inputPromptData = (NW_Dlg_InputPrompt_t*)Ctx;
  if (inputPromptData->selectedButton == 0) 
  {
    NW_Byte *utf8Buf = NW_Mem_Malloc(NW_Str_Strlen(params->stringToSign) * 3);
    NW_Byte* p=utf8Buf;
    NW_Ucs2 *s = params->stringToSign;
    /* PKCS1 hashed content block type format */
    NW_Byte hashedData[] = {
      0x30, 0x21, 
        0x30, 0x09, 
          0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 
          0x05, 0x00,
        0x04, 0x14,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      
    };
    /* PKCS7 hashed content type format */
    NW_Byte pkcs7 [] = { 
     0x31, 0x5d, 
        0x30, 0x18, 
          0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x03,
            0x31, 0x0b,
              0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x01,
        0x30, 0x1c,
          0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x05,
            0x31,0x0f,
              0x17, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                                        0x00, 0x00, 0x00, 0x5a,
        0x30, 0x23,
          0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x04,
            0x31,0x16,
              0x04, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
/* Get the time here */

    NW_Byte *inputPIN=NULL;
    NW_Uint16 inputPINLen=0;
    NW_SignResp_t *signResp=NULL;
    authenticatedAttribute = NW_Mem_Malloc(authenticatedAttributeLen);
    signedContent = NW_Mem_Malloc(sizeof(SignedContent));

    do
    {
      if (authenticatedAttribute == NULL || signedContent == NULL)
      {
        status = KBrsrOutOfMemory;
        break;
      }
      signedContent->authenticatedAttribute = authenticatedAttribute;
      params->signedContent = signedContent;
      
      authenticatedAttribute->attributeType = SignatureAuthenticatedAttributeGmtUtcTime;
      /* This is GMT time */
      universalTime = NW_Time_CvtToTimeTm(NW_Time_GetTime());
      pTime = (NW_Uint8*)authenticatedAttribute->gmtUtcTime;
      pTime += NW_Reader_WriteUcs2CharBuff(universalTime->Year,pTime);
      pTime += NW_Reader_WriteUcs2CharBuff(universalTime->Month,pTime);
      pTime += NW_Reader_WriteUcs2CharBuff(universalTime->Day,pTime);
      pTime += NW_Reader_WriteUcs2CharBuff(universalTime->Hour,pTime);
      pTime += NW_Reader_WriteUcs2CharBuff(universalTime->Minute,pTime);
      pTime += NW_Reader_WriteUcs2CharBuff(universalTime->Second,pTime);

      NW_Time_Tm_Delete(universalTime);
      universalTime=NULL;
      NW_Mem_memcpy(&pkcs7[45], (NW_Uint8*)authenticatedAttribute->gmtUtcTime,
        authenticatedAttributeLen - 2);
      
      if (utf8Buf != NULL)
      {
        while (*s) {
          if (*s == '\n') 
            continue;
          p += NW_Reader_WriteUtf8CharBuff(*s,p);
          s++;
        }
      }
      else
      {
        status = KBrsrOutOfMemory;
        break;
      }
      if ((status = NW_Wim_Hash(NW_WIMI_HA_SHA1 , (NW_Uint16) (p - utf8Buf), utf8Buf, 
        &pkcs7[75])) != KBrsrSuccess)
      {
        break;
      }

      if ((status = NW_Wim_Hash(NW_WIMI_HA_SHA1 , (NW_Uint16) (95), pkcs7, 
        &hashedData[15])) != KBrsrSuccess)
      {
        break;
      }      
      
      if ((status = NW_Wim_GetKeyRefFromCertRef(params->selectedCertRef,
        &params->selectedKeyRef)) != KBrsrSuccess)
      {
          break;
      }
      inputPIN = (NW_Byte*) NW_Str_CvtToAscii(inputPromptData->input);
      inputPINLen = NW_UINT16_CAST( NW_Str_Strlen(inputPromptData->input) );
      if (inputPIN == NULL)
      {
        status = KBrsrOutOfMemory;
        break;
      }
      signResp = NW_Mem_Malloc(sizeof(NW_SignResp_t));
      if (signResp == NULL)
      {
        status = KBrsrOutOfMemory;
        break;
      }
      signResp->senderInfo.sender = NW_SCRIPT_SERVER;
      signResp->senderInfo.context = params;
      signResp->senderInfo.callback = NW_Script_Wimiresp;

      if ((status = NW_Wim_SignReq(signResp, hashedData, sizeof(hashedData),
        params->selectedKeyRef, inputPINLen, inputPIN)) != KBrsrSuccess)
      {
        break;
      }
      params->state = SignStateBuildSignature;
    }
    while(contDo);
    if (status !=KBrsrSuccess)
    {
      NW_Mem_Free(utf8Buf);
      if (inputPIN!=NULL)
      {
        /* Erase the PIN from memory */
        for (i=0;i<inputPINLen;i++)
        {
          inputPIN[i]=0;
        }
        NW_Mem_Free(inputPIN);
      }
      NW_Mem_Free(signResp);
      NW_Script_CryptoSignTextErrorCleanup(params, status);
    }
    NW_Mem_Free(utf8Buf);
  }
  else
  {
    NW_ScrCrypto_SignTextParamsFree(params);
    CoreScript_SignText_CB(KBrsrCryptoUserCancel, NULL, 0);
  }
  /* Erase the PIN from memory */
  for (i=0;i<MAX_PIN_LEN;i++)
  {
    inputPromptData->input[i]=0;
  }
  NW_Mem_Free(inputPromptData->input);
  tmp = (NW_Ucs2*)inputPromptData->title;
  NW_Mem_Free(tmp);
  tmp = (NW_Ucs2*)inputPromptData->prompt;
  NW_Mem_Free(tmp);
  inputPromptData->input=NULL;
  NW_Mem_Free(inputPromptData);
}

static NW_Bool signStateBuildSignature(SignTextParams_t* params, void* Ctx)
{
  NW_SignResp_t *signResp = (NW_SignResp_t*)Ctx;
  TBrowserStatusCode status=KBrsrSuccess;
  NW_Uint8 signerInfosCount=0;
  NW_Uint8 i;
  const NW_Bool contDo = NW_FALSE;
  NW_Bool cont = NW_TRUE;
  Signature *signature = NULL;
  SignerInfo **signerInfo = NULL;
  ContentInfo *contentInfo = NULL;
  SignedContent *signedContent = NULL;
	NW_Byte *keyId;
  NW_Uint16 keyIdLen;
	NW_Uint8 type;
  NW_Uint16 certLen;
  NW_CertResp_t* certReq = NULL;
  NW_ASSERT(params!=NULL);
  NW_ASSERT(Ctx!=NULL);
  do
  {
    status = NW_Wim_ConvertErrors(signResp->status);
    if (status != KBrsrSuccess)
    {
      break;
    }
    signature = NW_Mem_Malloc(sizeof(Signature));
    contentInfo = NW_Mem_Malloc(sizeof(ContentInfo));
    signedContent = params->signedContent;

    if (signature == NULL || contentInfo == NULL || 
      signedContent == NULL)
    {
      status = KBrsrOutOfMemory;
      break;
    }
    if ((status = NW_Wim_GetCertInfo(params->selectedCertRef, NULL, NULL, NULL,
      &keyId, &keyIdLen, NULL, NULL, NULL, &type, &certLen, NULL )) != KBrsrSuccess)
    {
      break;
    }
    signature->algorithm = SignatureAlgorithmRsaSha1;
    signature->signLen = signResp->signatureLen;
    signature->sign = NW_Mem_Malloc(signResp->signatureLen);
    if (signature->sign == NULL)
    {
      status = KBrsrOutOfMemory;
      break;
    }
    NW_Mem_memcpy(signature->sign, signResp->signature, signResp->signatureLen);
    signature->signLen = signResp->signatureLen;
    if (params->option == 0)
    {
      signerInfosCount = 1;
    }
    else
    {
      if (params->option & SignatureIncludeKeyHash)
        signerInfosCount ++;
      if (params->option & SignatureIncludeCertificate)
        signerInfosCount ++;
    }

    signerInfo = NW_Mem_Malloc(signerInfosCount * sizeof(SignerInfo*));
    if (signerInfo == NULL)
    {
      status = KBrsrOutOfMemory;
      break;
    }
    for (i=0;i<signerInfosCount;i++)
    {
      signerInfo[i] = NW_Mem_Malloc(sizeof(SignerInfo));
      if (signerInfo[i] == NULL)
      {
        status = KBrsrOutOfMemory;
        break;
      }
    }
    if (status != KBrsrSuccess)
      break;

    i = 0;

    if (params->option == 0)
    {
      signerInfo[i]->signerInfoType = 0;
      signerInfo[i]->signerInfoLen = 0;
      signerInfo[i]->signerInfo = NULL;
    }
    if (params->option & SignatureIncludeKeyHash)
    {
      signerInfo[i]->signerInfoType = 1;
      signerInfo[i]->signerInfoLen = keyIdLen;
      signerInfo[i]->signerInfo = keyId;
      keyId = NULL;
      i++;
    }
    NW_Mem_Free(keyId);
    if (params->option & SignatureIncludeCertificate)
    {
      switch (type)
      {
        case WTLSCert:
          signerInfo[i]->signerInfoType = SignatureCertificateTypeWtls;
          break;
        case X509Cert:
          signerInfo[i]->signerInfoType = SignatureCertificateTypeX509;
          break;
        case X968Cert:
          signerInfo[i]->signerInfoType = SignatureCertificateTypeX968;
          break;
        case CertURL:
          signerInfo[i]->signerInfoType = SignatureCertificateTypeURL;
          break;
      }
      signerInfo[i]->signerInfoLen = certLen;
      signerInfo[i]->signerInfo = NULL;
    }
    /* currently, only text content type is defined */
    contentInfo->contentType = SignatureContentTypeText;
    contentInfo->contentEncoding = HTTP_utf_8;
    contentInfo->contentPresent = (NW_Bool)((params->option & SignatureIncludeContent) ?
      NW_TRUE : NW_FALSE);
    if (contentInfo->contentPresent)
    {
      NW_Byte* p;
      NW_Ucs2 *s = params->stringToSign;
      contentInfo->content = NW_Mem_Malloc(NW_Str_Strlen(params->stringToSign) * 3);
      p = contentInfo->content;

      if (contentInfo->content != NULL)
      {
        while (*s) {
          if (*s == '\n' )
            continue;
          p += NW_Reader_WriteUtf8CharBuff(*s,p);
          s++;
        }
        contentInfo->contentLen = (NW_Uint16)(p - (NW_Byte*)contentInfo->content);
      }
      else
      {
        status = KBrsrOutOfMemory;
        break;
      }
    }
    else
    {
      contentInfo->contentLen = 0;
      contentInfo->content=NULL;
    }

    signedContent->version = 1;
    signedContent->signature = signature;
    signedContent->signerInfoCount = signerInfosCount;
    signedContent->signerInfos = signerInfo;
    signedContent->contentInfo = contentInfo;
    params->signedContent = signedContent;

    if (params->option & SignatureIncludeCertificate)
    {
      certReq = NW_Mem_Malloc(sizeof(NW_CertResp_t));
      if (certReq == NULL)
      {
        status = KBrsrOutOfMemory;
        break;
      }
      certReq->senderInfo.sender = NW_SCRIPT_SERVER;
      certReq->senderInfo.context = params;
      certReq->senderInfo.callback = NW_Script_Wimiresp;
      status = NW_Wim_CertificateReq(certReq, params->selectedCertRef);
      if (status!=KBrsrSuccess)
      {
        break;
      }
      params->state = SignStateAddCertificateContent;
      cont = NW_FALSE;
    }
    else
    {
      params->state = SignStateFinish;
    }
  }
  while(contDo);
  if (status != KBrsrSuccess)
  {
    cont = NW_FALSE;
    NW_Mem_Free(signature);
    for (i=0;i<signerInfosCount;i++)
    {
      NW_Mem_Free(signerInfo[i]);
    }
    NW_Mem_Free(signerInfo);
    if (contentInfo != NULL)
    {
      NW_Mem_Free(contentInfo->content);
      NW_Mem_Free(contentInfo);
    }
    NW_Mem_Free(signedContent);
    params->signedContent = NULL; /* Do not free twice */
    NW_Mem_Free(certReq);
    NW_Script_CryptoSignTextErrorCleanup(params, status);
  }
  NW_Mem_Free(signResp);
  return cont;
}

static NW_Bool signStateAddCertificateContent(SignTextParams_t* params, void* Ctx)
{
  NW_Bool cont=NW_TRUE;
  const NW_Bool contDo=NW_FALSE;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_CertResp_t *certResp = (NW_CertResp_t*)Ctx;
  NW_ASSERT(Ctx != NULL);
  do
  {
    status = NW_Wim_ConvertErrors(certResp->status);
    if (status != KBrsrSuccess)
    {
      break;
    }
    if (params->signedContent->signerInfos[params->signedContent->signerInfoCount - 1]->signerInfoLen != certResp->certLen)
    {
      status = KBrsrOutOfMemory;
      break;
    }
    params->signedContent->signerInfos[params->signedContent->signerInfoCount - 1]->signerInfo = certResp->cert;
    params->state = SignStateFinish;
  }
  while(contDo);
  if (status != KBrsrSuccess)
  {
    NW_Script_CryptoSignTextErrorCleanup(params, status);
    cont=NW_FALSE;
  }
  NW_Mem_Free(certResp);
  return cont;
}

static void signStateFinish(SignTextParams_t* params)
{
  NW_Byte* signedText;
  NW_Uint32 signedTextLen;
  NW_Uint16 i;
  NW_Uint16 signatureLen=0;
  NW_Uint16 signerInfoLen[4];/* no more than 4 defferent infos */
  NW_Uint16 signerInfosLen=2; /* 2 is for size */
  NW_Uint16 contentInfoLen=0;
  NW_Uint16 authenticatedAttributeLen = 14; /* we only support GMT time */
  NW_ASSERT(params!=NULL);
  /* calculate length of buffer */

  signatureLen = 1;/* 1 for the algorithm */
  if (params->signedContent->signature->algorithm != 0)
  {
    signatureLen = (NW_Uint16)(signatureLen + params->signedContent->signature->signLen + 2);/* 2 for the size */
  }

  for (i=0;i<params->signedContent->signerInfoCount;i++)
  {
    signerInfoLen[i]=1; /* 1 for is the type */
    if (params->signedContent->signerInfos[i]->signerInfoType != 0)
    {
      signerInfoLen[i] = (NW_Uint16)(signerInfoLen[i] + params->signedContent->signerInfos[i]->signerInfoLen);
      switch (params->signedContent->signerInfos[i]->signerInfoType)
      {
        case SignatureCertificateTypeWtls:
        case SignatureCertificateTypeX509:
        case SignatureCertificateTypeX968:
          signerInfoLen[i] += 2; 
          /* 2 is for size  */
          break;
        case SignatureCertificateTypeURL:
          signerInfoLen[i] += 1; /* 1 is for size */
          break;
        default:
          break;
      }
    }
    signerInfosLen = (NW_Uint16)(signerInfosLen + signerInfoLen[i] );
  }

  contentInfoLen = 4; /* 4 bytes for flags: content type, encoding and present */
  if (params->signedContent->contentInfo->contentPresent)
  {
    contentInfoLen = (NW_Uint16)(contentInfoLen + params->signedContent->contentInfo->contentLen + 2); /* 2 is for size */
  }

  /* 1 for the version */
  signedTextLen= 1 + signatureLen + signerInfosLen + contentInfoLen + 
    authenticatedAttributeLen;
  signedText = NW_Mem_Malloc(signedTextLen);
  if (signedText != NULL)
  {
    /* fill the buffer */
    NW_Byte *p=signedText;
    *p = 1; /* version number */
    p++;
    *p = params->signedContent->signature->algorithm;
    p++;
    if (params->signedContent->signature->algorithm != 0)
    {
      p += NW_Reader_WriteUcs2CharBuff(params->signedContent->signature->signLen,p);
      NW_Mem_memcpy(p,params->signedContent->signature->sign,params->signedContent->signature->signLen);
      p += params->signedContent->signature->signLen;
    }
    p += NW_Reader_WriteUcs2CharBuff((NW_Uint16)(signerInfosLen-2),p); /* size not included */
    for (i=0;i<params->signedContent->signerInfoCount;i++)
    {
      *p = params->signedContent->signerInfos[i]->signerInfoType;
      p++;
      switch (params->signedContent->signerInfos[i]->signerInfoType)
      {
        case SignatureIdKeyHash:
          params->signedContent->signerInfos[i]->signerInfoLen=20; /* 20 is the size of a SHA1 hash */
          break;
        case SignatureCertificateTypeWtls:
        case SignatureCertificateTypeX509:
        case SignatureCertificateTypeX968:
          p += NW_Reader_WriteUcs2CharBuff(params->signedContent->signerInfos[i]->signerInfoLen,p);
          break;
        case SignatureCertificateTypeURL:
          *p = (NW_Uint8)(params->signedContent->signerInfos[i]->signerInfoLen);
          p++;
          break;
        default:
          break;
      }
      NW_Mem_memcpy(p,params->signedContent->signerInfos[i]->signerInfo,
        params->signedContent->signerInfos[i]->signerInfoLen);
      p += params->signedContent->signerInfos[i]->signerInfoLen;
    }
    *p = (NW_Uint8)SignatureContentTypeText;
    p++;
    p += NW_Reader_WriteUcs2CharBuff(params->signedContent->contentInfo->contentEncoding,p);
    *p = params->signedContent->contentInfo->contentPresent;
    p++;
    if (params->signedContent->contentInfo->contentPresent)
    {
      p += NW_Reader_WriteUcs2CharBuff(params->signedContent->contentInfo->contentLen,p);
      NW_Mem_memcpy(p,params->signedContent->contentInfo->content,
        params->signedContent->contentInfo->contentLen);
      p += params->signedContent->contentInfo->contentLen;            
    }
    *p = (NW_Uint8)(authenticatedAttributeLen - 1); /* size of authenticated attribute , 
                                           without the length */
    p++;
    *p = params->signedContent->authenticatedAttribute->attributeType;
    p++;
    NW_Mem_memcpy(p,params->signedContent->authenticatedAttribute->gmtUtcTime,
      authenticatedAttributeLen - 2);/* size of authenticated attribute , 
                                        without the length and type */

    NW_ScrCrypto_SignTextParamsFree(params);
    CoreScript_SignText_CB(KBrsrSuccess, signedText, signedTextLen);
  }
  else
  {
    NW_Script_CryptoSignTextErrorCleanup(params, KBrsrOutOfMemory);
  }
}
#endif /*PROFILE_NO_WTLS*/

/************************************************************************
  Function: NW_Script_CryptoSignTextContinue
  Purpose: Continue processing of SignText 
  Parameters: 

  Return Values: none
*************************************************************************/
void NW_Script_CryptoSignTextContinue(SignTextParams_t* params, void* Ctx)
{
#ifndef PROFILE_NO_WTLS  
  NW_Bool cont=NW_TRUE;
  while (cont)
  {
    switch (params->state) 
    {
      case SignStateInit:
      {
        cont = signStateInit( params, Ctx);
        Ctx = NULL;
        break;
      }
      case SignStateGetSelectedCert:
      {
        cont = signStateGetSelectedCert(params, Ctx);
        Ctx = NULL;
        break;
      }
      case SignStateGetPIN:
      {
        signStateGetPIN(params);
        cont = NW_FALSE;
        break;
      }
      case SignStateGetSignature:
        signStateGetSignature(params, Ctx);
        Ctx = NULL;
        cont=NW_FALSE;
        break;
      case SignStateBuildSignature:
      {
        cont = signStateBuildSignature(params, Ctx);
        Ctx = NULL;
        break;
      }
      case SignStateAddCertificateContent:
      {
        cont = signStateAddCertificateContent(params, Ctx);
        Ctx = NULL;
        break;
      }
      case SignStateFinish:
      {
        signStateFinish(params);
        cont = NW_FALSE;
        break;
      }
      default:
        NW_ASSERT(NW_FALSE); /* Should not be here. All states should be covered */
        NW_Script_CryptoSignTextErrorCleanup(params,KBrsrScriptFatalLibFuncError);
        cont=NW_FALSE;
        break;
    }/* switch */
  }/* while cont */
#else
  NW_REQUIRED_PARAM(Ctx);
  NW_REQUIRED_PARAM(params);

#endif /*PROFILE_NO_WTLS*/
}


TBrowserStatusCode NW_Script_Genericdialoglistselectresp(NW_Msg_t *msg)
{
  NW_Dlg_ListSelect_t* data;
  NW_ASSERT(msg != NULL);
  NW_ASSERT(msg->data != NULL);
  data = (NW_Dlg_ListSelect_t*)msg->data;
  NW_ASSERT(data->context!=NULL);
  NW_Msg_Delete(msg);
  NW_Script_CryptoSignTextContinue((SignTextParams_t*)data->context , data);
  return KBrsrSuccess;
}

TBrowserStatusCode NW_Script_Genericdialogpromptresp(NW_Msg_t *msg)
{
  NW_Dlg_Prompt_t* data;
  NW_ASSERT(msg != NULL);
  NW_ASSERT(msg->data != NULL);
  data = (NW_Dlg_Prompt_t*)msg->data;
  NW_Msg_Delete(msg);
  NW_Script_CryptoSignTextContinue((SignTextParams_t*)data->context , data);
  return KBrsrSuccess;
}

TBrowserStatusCode NW_Script_Genericdialoginputpromptresp(NW_Msg_t *msg)
{
  NW_Dlg_InputPrompt_t* data;
  NW_ASSERT(msg != NULL);
  NW_ASSERT(msg->data != NULL);
  data = (NW_Dlg_InputPrompt_t*)msg->data;
  NW_Msg_Delete(msg);
  NW_Script_CryptoSignTextContinue((SignTextParams_t*)data->context , data);
  return KBrsrSuccess;
}


TBrowserStatusCode NW_Script_Wimiresp(NW_Msg_t *msg)
{
  SignTextParams_t* params;
  NW_SenderInfo_t * senderInfo;
  NW_ASSERT(msg->data!=NULL);
  senderInfo = (NW_SenderInfo_t *) msg->data;
  NW_Msg_Delete(msg);
  NW_ASSERT(senderInfo!=NULL);
  params = senderInfo->context;
  NW_ASSERT(params!=NULL);
  NW_Script_CryptoSignTextContinue(params, senderInfo);
  return KBrsrSuccess;
}
#endif
