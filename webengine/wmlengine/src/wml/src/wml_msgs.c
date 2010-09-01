/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Message handlers for the wml interpreter.
*
*/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/

#include "nwx_logger.h"
#include "nwx_ctx.h"
#include "nwx_msg_api.h"
#include "nwx_generic_dlg.h"
#include "scr_msgs.h"
#include "wml_wae.h"
#include "wml_scrproxy.h"
#include "BrsrStatusCodes.h"


/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/
static TBrowserStatusCode Wml_MsgSendResponse(NW_Msg_t *msg, NW_Msg_Type_t msgType,
                                   TBrowserStatusCode status, void *data);
static NW_ScrProxy_t * Wml_GetScriptProxy(void);

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
static TBrowserStatusCode Wml_MsgSendResponse(NW_Msg_t *msg, NW_Msg_Type_t msgType,
                                   TBrowserStatusCode status, void *data)
{
  TBrowserStatusCode ret_status;
  NW_ASSERT(msg != NULL);

  NW_Msg_Reinit(msg, msg->tid,
                msg->src, /* Send it back to where it came from */
                msg->dest,
                msgType,
                status,
                data);

  if ((ret_status = NW_Msg_Send(msg)) != KBrsrSuccess) {
    NW_LOG0(NW_LOG_LEVEL1, "WML Interpreter : Could not send message");
  }
  return ret_status;
}

/************************************************************************
  Function: Wml_GetScriptProxy
  Purpose:  returns the NW_ScrProxy_t for the current browser context
  Parameters:  none

  Return Values:  NW_ScrProxy_t or NULL if script handler unregistered
**************************************************************************/
static NW_ScrProxy_t * Wml_GetScriptProxy(void)
{
  NW_WaeUsrAgent_t  *browser;
  NW_ScrProxy_t     *scrProxy;

  browser = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT(browser != NULL);

  scrProxy = NW_WaeUsrAgent_GetScript(browser);

  return scrProxy;
}

/************************************************************************
  Function: NW_Wml_DlgConfirmResp
  Purpose: Sends a confirm response to the Script Interpreter.
  Parameters:  ctx        - message
               status     - user agent status 
               result     - user confirmation

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory
**************************************************************************/
static TBrowserStatusCode NW_Wml_DlgConfirmResp(void *ctx, 
                                         TBrowserStatusCode status, 
                                         NW_Dlg_Confirm_Result_t result)
{
  NW_Msg_t                    *msg = (NW_Msg_t  *)ctx;
  NW_ScriptServerConfirmReq_t *data = (NW_ScriptServerConfirmReq_t* )msg->data;
  NW_ScrProxy_t               *scrProxy;

  NW_ASSERT(msg->type == NW_MSG_UA_DLGCONFIRM_REQ); /* bad context pointer */
  NW_ASSERT(data != NULL);

  if (status ==  KBrsrOutOfMemory || result == DIALOG_CONFIRM_END) {
    NW_Script_Confirm_Req_Free(data);
    NW_Msg_Delete(msg);  
    return status;
  }

  switch (result) { 
  case DIALOG_CONFIRM_YES:
    data->response = NW_TRUE;
    break;
  case DIALOG_CONFIRM_NO:
    data->response = NW_FALSE;
    break;
  default:
    NW_ASSERT(0);
  }

  if ((status = Wml_MsgSendResponse(msg, NW_MSG_UA_DLGCONFIRM_RESP,
                         status, data)) != KBrsrSuccess) {
    NW_Script_Confirm_Req_Free(data);
    NW_Msg_Delete(msg);  
    /* inform the proxy that we were not able to send the message */
    scrProxy = Wml_GetScriptProxy();

    if (scrProxy == NULL) {     /* check if script is aborted */
      status = KBrsrScrproxyScrAbort;
    }
    else {
      status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
    }
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_DlgPromptResp
  Purpose: Sends a prompt response to the Script Interpreter.
  Parameters:  ctx        - message
               status     - user agent status 
               input      - user input
               endPressed - 

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory
**************************************************************************/
static TBrowserStatusCode NW_Wml_DlgPromptResp(void *ctx, 
                                        TBrowserStatusCode status, 
                                        const NW_Ucs2 *input,
                                        NW_Bool endPressed)
{
  NW_Msg_t      *msg = (NW_Msg_t  *)ctx;
  NW_Ucs2     *inputCpy = NULL;
  NW_ScrProxy_t *scrProxy;

  NW_ASSERT(msg->type == NW_MSG_UA_DLGPROMPT_REQ); /* bad context pointer */
  
  if (status == KBrsrOutOfMemory || endPressed) {
    NW_Msg_Delete(msg);
    return status;
  }

  if ((status == KBrsrSuccess) && (input != NULL)) {
    inputCpy = NW_Str_Newcpy(input);
    if (inputCpy == NULL) {
      status = KBrsrOutOfMemory;
    }
  }

  if ((status = Wml_MsgSendResponse(msg, NW_MSG_UA_DLGPROMPT_RESP,
                         status, inputCpy)) != KBrsrSuccess) {
    NW_Str_Delete(inputCpy);
    NW_Msg_Delete(msg);
     /* inform the proxy that we were not able to send the message */
    scrProxy = Wml_GetScriptProxy();

    if (scrProxy == NULL) {     /* check if script is aborted */
      status = KBrsrScrproxyScrAbort;
    }
    else {
      status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
    }
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_DlgAlertResp
  Purpose: Sends a alert response to the Script Interpreter.
  Parameters:  ctx        - message
               status     - user agent status 

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory
**************************************************************************/
static TBrowserStatusCode NW_Wml_DlgAlertResp(void *ctx, 
                                       TBrowserStatusCode status,
                                       NW_Dlg_Confirm_Result_t result)
{
  NW_Msg_t      *msg = (NW_Msg_t  *)ctx;
  NW_ScrProxy_t               *scrProxy;

  NW_ASSERT(msg != NULL);
  NW_ASSERT(msg->type == NW_MSG_UA_DLGALERT_REQ); /* bad context pointer */

  if (status == KBrsrOutOfMemory || result == DIALOG_CONFIRM_END) {
    NW_Msg_Delete(msg);
    return status;
  }

  
  if ((status = Wml_MsgSendResponse(msg, NW_MSG_UA_DLGALERT_RESP,
                         status, NULL)) != KBrsrSuccess) {
    NW_Msg_Delete(msg);
    /* inform the proxy that we were not able to send the message */
    scrProxy = Wml_GetScriptProxy();

    if (scrProxy == NULL) {     /* check if script is aborted */
      status = KBrsrScrproxyScrAbort;
    }
    else {
      status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
    }
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Genericdialoglistselectresp
  Purpose: Sends a generic listselect response to the Script Interpreter.
  Parameters:  data - response
               ctx  - message

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory
**************************************************************************/
static TBrowserStatusCode NW_Wml_Genericdialoglistselectresp(NW_Dlg_ListSelect_t* data, void* ctx)
{
  NW_Msg_t      *msg = (NW_Msg_t  *)ctx;
  TBrowserStatusCode status;
  NW_ScrProxy_t *scrProxy;

  NW_ASSERT(msg->type == NW_MSG_UA_DIALOGLISTSELECT_REQ); /* bad context pointer */
  msg->data=data;

  if ((status = NW_Msg_Reply(msg, NW_MSG_UA_DIALOGLISTSELECT_RESP)) != KBrsrSuccess) {
    NW_Msg_Delete(msg);
     /* inform the proxy that we were not able to send the message */
    scrProxy = Wml_GetScriptProxy();

    if (scrProxy == NULL) {     /* check if script is aborted */
      status = KBrsrScrproxyScrAbort;
    }
    else {
      status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
    }
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Genericdialogpromptresp
  Purpose: Sends a generic prompt response to the Script Interpreter.
  Parameters:  data - response
               ctx  - message

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory
**************************************************************************/
static TBrowserStatusCode NW_Wml_Genericdialogpromptresp(NW_Dlg_Prompt_t* data, void* ctx)
{
  NW_Msg_t      *msg = (NW_Msg_t  *)ctx;
  TBrowserStatusCode status;
  NW_ScrProxy_t *scrProxy;

  NW_ASSERT(msg->type == NW_MSG_UA_DIALOGPROMPT_REQ); /* bad context pointer */
  msg->data=data;

  if ((status = NW_Msg_Reply(msg, NW_MSG_UA_DIALOGPROMPT_RESP)) != KBrsrSuccess) {
    NW_Msg_Delete(msg);
     /* inform the proxy that we were not able to send the message */
    scrProxy = Wml_GetScriptProxy();

    if (scrProxy == NULL) {     /* check if script is aborted */
      status = KBrsrScrproxyScrAbort;
    }
    else {
      status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
    }
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Genericdialoginputpromptresp
  Purpose: Sends a generic input prompt response to the Script Interpreter.
  Parameters:  data - response
               ctx  - message

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory
**************************************************************************/
static TBrowserStatusCode NW_Wml_Genericdialoginputpromptresp(NW_Dlg_InputPrompt_t* data, void* ctx)
{
  NW_Msg_t      *msg = (NW_Msg_t  *)ctx;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ScrProxy_t *scrProxy;

  NW_ASSERT(msg->type == NW_MSG_UA_DIALOGINPUTPROMPT_REQ); /* bad context pointer */


  msg->data=data;

  if ((status = NW_Msg_Reply(msg, NW_MSG_UA_DIALOGINPUTPROMPT_RESP)) != KBrsrSuccess) {
    NW_Msg_Delete(msg);
     /* inform the proxy that we were not able to send the message */
    scrProxy = Wml_GetScriptProxy();

    if (scrProxy == NULL) {     /* check if script is aborted */
      status = KBrsrScrproxyScrAbort;
    }
    else {
      status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
    }
  }
  return status;
}

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/************************************************************************
  Function: NW_Wml_Dialogalertreq
  Purpose: Handler for NW_MSG_UA_DLGALERT_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing dialog alert data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Dialogalertreq(NW_Msg_t *msg)
{
  TBrowserStatusCode       status;
  NW_Ucs2         *alertmsg;   /* message to display to the user */
  NW_ScrProxy_t *scrProxy;

  NW_LOG0(NW_LOG_LEVEL5, "WML : NW_Wml_Dialogalertreq\n");

  NW_ASSERT(msg != NULL);
  alertmsg = (NW_Ucs2* )msg->data; msg->data = NULL;

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else {
    status = NW_ScrProxy_DlgAlert(scrProxy, alertmsg, msg, NW_Wml_DlgAlertResp);
  }

  NW_Str_Delete(alertmsg);
  
  if (status != KBrsrSuccess) {
    NW_Msg_Delete(msg);
  }
  return status;
}
/************************************************************************
  Function: NW_Wml_Notifyerrorreq
  Purpose: Handler for NW_MSG_UA_NOTIFYERROR_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing dialog alert data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Notifyerrorreq(NW_Msg_t *msg)
{
  TBrowserStatusCode       status;
  NW_ScrProxy_t *scrProxy;

  NW_LOG0(NW_LOG_LEVEL5, "WML : NW_Wml_Notifyerrorreq\n");

  NW_ASSERT(msg != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else {
    status = NW_ScrProxy_NotifyError(scrProxy,msg->status);
  }

  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
  if (status != KBrsrScrproxyScrAbort) 
  {
    /* return the response to the scripting engine */
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_UA_NOTIFYERROR_RESP,
        status, NULL))
    {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }
  
  /* clean up (1)if we failed to send the message (2)the script has aborted */
  if (status != KBrsrSuccess) {
    /* free the incoming message */
    NW_Msg_Delete(msg);
  }
  return status;
}


/************************************************************************
  Function: NW_Wml_Dialogconfirmreq
  Purpose: Handler for NW_MSG_UA_DLGCONFIRM_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing dialog confirm data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Dialogconfirmreq(NW_Msg_t *msg)
{
  TBrowserStatusCode                 status;
  NW_ScriptServerConfirmReq_t *data = NULL;         /* confirm message */
  NW_ScrProxy_t               *scrProxy;

  NW_ASSERT(msg != NULL);
  data = (NW_ScriptServerConfirmReq_t* )msg->data;
  NW_ASSERT(data != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else {
    status = NW_ScrProxy_DlgConfirm(scrProxy, data->message, data->yesStr,
                         data->noStr, msg, NW_Wml_DlgConfirmResp);
  }

  if (status != KBrsrSuccess) {
    NW_Script_Confirm_Req_Free(data);
    NW_Msg_Delete(msg);
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Dialogpromptreq
  Purpose: Handler for NW_MSG_UA_DLGPROMPT_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing dialog prompt data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Dialogpromptreq(NW_Msg_t *msg)
{
  TBrowserStatusCode                   status;
  NW_ScriptServerDlgPromptReq_t *data = NULL;
  NW_ScrProxy_t                 *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_ScriptServerDlgPromptReq_t* )msg->data; 
  msg->data = NULL;
  NW_ASSERT(data != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    status = NW_ScrProxy_DlgPrompt(scrProxy, data->message, data->input, 
                                   msg, NW_Wml_DlgPromptResp);
  }
  NW_Script_DlgPrompt_Req_Free(data);

  if (status != KBrsrSuccess)
  {
    NW_Msg_Delete(msg);
  }
  return status;
}


/************************************************************************
  Function: NW_Wml_Genericdialoglistselectreq
  Purpose: Handler for NW_MSG_UA_DIALOGLISTSELECT_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing generic dialog listselect data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Genericdialoglistselectreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_Dlg_ListSelect_t *data = NULL;
  NW_ScrProxy_t   *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_Dlg_ListSelect_t* )msg->data; 
  msg->data = NULL;
  NW_ASSERT(data != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else {
    status = NW_ScrProxy_GenericDlgListSelect(scrProxy, data, msg, 
                                              NW_Wml_Genericdialoglistselectresp);
  }

  if (status != KBrsrSuccess) {
    NW_Msg_Delete(msg);
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Genericdialogpromptreq
  Purpose: Handler for NW_MSG_UA_DIALOGPROMPT_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing generic dialog prompt data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Genericdialogpromptreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_Dlg_Prompt_t *data = NULL;
  NW_ScrProxy_t   *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_Dlg_Prompt_t* )msg->data; 
  msg->data = NULL;
  NW_ASSERT(data != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    status = NW_ScrProxy_GenericDlgPrompt(scrProxy, data, msg, NW_Wml_Genericdialogpromptresp);
  }

  if (status != KBrsrSuccess)
  {
    NW_Msg_Delete(msg);
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Genericdialoginputpromptreq
  Purpose: Handler for NW_MSG_UA_DIALOGINPUTPROMPT_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing generic dialog input prompt data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Genericdialoginputpromptreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_Dlg_InputPrompt_t *data = NULL;
  NW_ScrProxy_t   *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_Dlg_InputPrompt_t* )msg->data; 
  msg->data = NULL;
  NW_ASSERT(data != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    status = NW_ScrProxy_GenericDlgInputPrompt(scrProxy, data, msg, NW_Wml_Genericdialoginputpromptresp);
  }

  if (status != KBrsrSuccess) {
    NW_Msg_Delete(msg);
  }
  return status;
}



/************************************************************************
  Function: NW_Ua_Finishnodialogreq
  Purpose: Handler for NW_MSG_UA_FINISHNODLG_REQ messages.
           Invokes the script proxy and sends the response to the 
           script interpreter.
  Parameters:  msg - pointer to message containing script status and message
                     on completion.

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Finishnodialogreq(NW_Msg_t *msg)
{
  TBrowserStatusCode                 status;
  NW_ScriptServerFinishReq_t  *data = NULL; 
  NW_ScrProxy_t               *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_ScriptServerFinishReq_t* )msg->data; 
  msg->data = NULL;

  scrProxy = Wml_GetScriptProxy();

  if (data != NULL)
  {
  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else 
  {
    status = NW_ScrProxy_Finish(scrProxy, data->status, data->message);
  }

  NW_Script_Finish_Req_Free(data);
  }
  else
  {
    status = KBrsrOutOfMemory;
    NW_ScrProxy_Finish(scrProxy, status, NULL);
  }

  
  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
  if (status != KBrsrScrproxyScrAbort) 
  {
    /* return the response to the scripting engine */
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_UA_FINISHNODLG_RESP,
        status, NULL))
    {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }
  
  /* clean up (1)if we failed to send the message (2)the script has aborted */
  if (status != KBrsrSuccess) {
    /* free the incoming message */
    NW_Msg_Delete(msg);
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_Scriptstartresp
  Purpose: Handler for NW_MSG_SCRIPT_START_RESP messages.
           Invokes the script proxy.

  Parameters:  msg -  pointer to message containing acknowledgement for
                      start script request.

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Scriptstartresp(NW_Msg_t *msg)
{
  TBrowserStatusCode         status;
  NW_ScrProxy_t       *scrProxy;

  NW_ASSERT(msg != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    status = NW_ScrProxy_RunScriptAck(scrProxy, msg->status);
  }

  NW_Msg_Delete(msg);

  return status;
}

/************************************************************************
  Function: NW_Wml_Scriptexitresp
  Purpose: Handler for NW_MSG_SCRIPT_EXIT_RESP messages.
           Invokes the script proxy.

  Parameters:  msg -  pointer to message containing acknowledgement for
                      script exit request.
  
  Return Values:
  TBrowserStatusCode - always returns KBrsrSuccess
**************************************************************************/
TBrowserStatusCode NW_Wml_Scriptexitresp(NW_Msg_t *msg)
{
  TBrowserStatusCode         status;
  NW_ScrProxy_t       *scrProxy;

  NW_ASSERT(msg != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    status = NW_ScrProxy_AbortAck(scrProxy, msg->status);
  }

  NW_Msg_Delete(msg);

  return status;
}

/************************************************************************
  Function: NW_Wml_Getvarreq
  Purpose: Handler for NW_MSG_COREBROWSER_GETVAR_REQ message.
           Invokes the script proxy and sends the response to the 
           script interpreter.
  Parameters:  
  msg - msg data contains a pointer to a variable name
  
  Return Values:
  TBrowserStatusCode - KBrsrSuccess
              - KBrsrOutOfMemory
              - KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Getvarreq(NW_Msg_t *msg)
{
  TBrowserStatusCode             status;
  NW_ScriptServerVarInf_t *data;
  NW_Ucs2               *value = NULL;
  NW_ScrProxy_t           *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_ScriptServerVarInf_t*)msg->data; 
  msg->data = NULL;
  NW_ASSERT(data != NULL);
  NW_ASSERT(data->name != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else if ((status = NW_ScrProxy_GetVar(scrProxy, data->name, &value))
    == KBrsrSuccess) {
    status = NW_Script_VarInf_SetValue(data, value);
  }

  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
  if (status != KBrsrScrproxyScrAbort) {
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_COREBROWSER_GETVAR_RESP, 
      status, data)) {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }

  /* clean up (1)if we failed to send the message (2)the script has aborted */
  NW_Msg_Delete(msg);
  NW_Script_VarInf_Free(data);
  return status;
}

/************************************************************************
  Function: NW_Wml_Setvarreq
  Purpose: Handles the NW_MSG_COREBROWSER_SETVAR_REQ messages.
           Invokes the script proxy and sends the response to the 
           script interpreter.
           Note : When variables are set on the WML Interpreter
           the GUI is not refreshed.
  Parameters:  msg -  pointer to message containing pointer to the
                        variable name

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Setvarreq(NW_Msg_t *msg)
{
  TBrowserStatusCode             status;
  NW_ScriptServerVarInf_t *data;
  NW_ScrProxy_t           *scrProxy;

  NW_ASSERT(msg != NULL);

  data = (NW_ScriptServerVarInf_t*)msg->data; 
  msg->data = NULL;
  NW_ASSERT(data != NULL);
  
  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    /* Note: KBrsrFailure does not imply a fatal error ??? */
    status = NW_ScrProxy_SetVar(scrProxy, data->name, data->value);
  }
  NW_Script_VarInf_Free(data);

  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
  if (status != KBrsrScrproxyScrAbort) {
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_COREBROWSER_SETVAR_RESP, 
      status, NULL)) {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }
  
  /* clean up (1)if we failed to send the message (2)the script has aborted */
  NW_Msg_Delete(msg);
  return status;
}

/************************************************************************
  Function: NW_Wml_Loadprevreq
  Purpose: Handler for NW_MSG_COREBROWSER_LOADPREV_REQ messages.
           Invokes the script proxy and sends the response to the 
           script interpreter.

  Parameters:  msg -  pointer to message 

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Loadprevreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_ScrProxy_t   *scrProxy;

  NW_ASSERT(msg != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else if ((status = NW_ScrProxy_LoadPrev(scrProxy)) != KBrsrScrproxyScrAbort) {
  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_COREBROWSER_LOADPREV_RESP,
      status, NULL)) {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }

  /* clean up (1)if we failed to send the message (2)the script has aborted */
  NW_Msg_Delete(msg);
  return status;
}

/************************************************************************
  Function: NW_Wml_Newcontextreq
  Purpose: Handler for NW_MSG_COREBROWSER_NEWCONTEXT_REQ messages.
           Invokes the script proxy and sends the response to the 
           script interpreter.
  Parameters:  msg -  pointer to message 

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Newcontextreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_ScrProxy_t   *scrProxy;

  NW_ASSERT(msg != NULL);

  scrProxy = Wml_GetScriptProxy();
  
  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else if ((status = NW_ScrProxy_NewContext(scrProxy)) != KBrsrScrproxyScrAbort) {
  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_COREBROWSER_NEWCONTEXT_RESP, 
      status, NULL)) {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }
  
  /* clean up (1)if we failed to send the message (2)the script has aborted */
  NW_Msg_Delete(msg);
  return status;
}

/************************************************************************
  Function: NW_Wml_Goreq
  Purpose: Handler for NW_MSG_COREBROWSER_GO_REQ messages.
           Invokes the script proxy and sends the response to the 
           script interpreter.
  Parameters:  msg -  pointer to message 

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Goreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_ScrProxy_t   *scrProxy;
  NW_Ucs2       *url;

  NW_ASSERT(msg != NULL);
  url = (NW_Ucs2 *)msg->data;
  
  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL)
  {
    /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else
  {
    status = NW_ScrProxy_LoadUrl(scrProxy, url, NULL);
  }
  NW_Str_Delete(url);

  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
  if (status != KBrsrScrproxyScrAbort) {
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_COREBROWSER_GO_RESP,
      status, NULL)) {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }
  
  /* clean up (1)if we failed to send the message (2)the script has aborted */
  NW_Msg_Delete(msg);
  return status;
}


/************************************************************************
  Function: NW_Wml_Refreshreq
  Purpose: Handler for NW_MSG_COREBROWSER_REFRESH_REQ messages.
           Invokes the script proxy and sends the response to the 
           script interpreter.
  Parameters:  msg -  pointer to message 

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_Refreshreq(NW_Msg_t *msg)
{
  TBrowserStatusCode     status;
  NW_ScrProxy_t   *scrProxy;

  NW_ASSERT(msg != NULL);

  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else if ((status = NW_ScrProxy_Refresh(scrProxy)) != KBrsrScrproxyScrAbort) {
  /* 
   * send a response message to the scipt interpreter only if the script is 
   * still running ie. not aborted 
   */
    if (KBrsrSuccess == Wml_MsgSendResponse(msg, NW_MSG_COREBROWSER_REFRESH_RESP,
      status, NULL)) {
      return KBrsrSuccess;
    }
    status = NW_ScrProxy_ScrNotRespondingError(scrProxy);
  }
   
  /* clean up (1)if we failed to send the message (2)the script has aborted */
  NW_Msg_Delete(msg);
  return status;
}

/*****************************************************************

  Name: Wae_RunScript()

  Description:  Send a message to the script engine to run 
                a script. Note : the caller has to free the url,
                referer and content if the function returns a status
                code other than KBrsrSuccess.

  Parameters:   url     - location of the script.
                referer - the script invoker
                content - wml script
                len     - length of the content
                charset - encoding

  Return Value: KBrsrSuccess
                KBrsrOutOfMemory

******************************************************************/
TBrowserStatusCode
NW_Wml_RunScript(NW_Ucs2 *url, NW_Ucs2 *postfields,
                 NW_Ucs2 *referer, NW_Byte *content,
                 const NW_Uint32 len, const NW_Http_CharSet_t charset)
{
  TBrowserStatusCode           status;
  NW_StartScript_Req_t  *scrReq;
  NW_Msg_t              *cmsg = NULL;
  NW_Msg_Address_t      wmlMsgAddr, scriptMsgAddr;
  
  NW_ASSERT(url != NULL);
  NW_ASSERT(content != NULL);

  scrReq = NW_StartScript_Req_New(url, postfields, referer, content, len, charset);
  if (scrReq == NULL) {
    return KBrsrOutOfMemory;
  }

  /* get the messaging address of wml and script interpreter */
  status = NW_Msg_LookupReceiverByName(NW_BROWSER_CORE, &wmlMsgAddr);
  if (status == KBrsrSuccess) {
    status = NW_Msg_LookupReceiverByName(NW_SCRIPT_SERVER, &scriptMsgAddr);
    if (status == KBrsrSuccess) {
      /* TODO: add transaction support */
      cmsg = NW_Msg_New(1, wmlMsgAddr, scriptMsgAddr, NW_MSG_SCRIPT_START_REQ);
      if (cmsg == NULL) {
        status = KBrsrOutOfMemory;
      } else {
        cmsg->data = scrReq;
        /* Send the message to the script interpreter */
        status= NW_Msg_Send(cmsg);
      }
    }
  }

  /* clean up */
  if (status != KBrsrSuccess) {
    NW_StartScript_Req_Free(scrReq);
    NW_Msg_Delete(cmsg);
  }
  return status;
}

/*****************************************************************

  Name: NW_Wml_AbortScript()

  Description:  Send a message to the script engine to abort the script.

  Parameters:  

  Return Value: KBrsrSuccess
                KBrsrFailure

******************************************************************/
TBrowserStatusCode NW_Wml_AbortScript()
{
  TBrowserStatusCode           status;
  NW_Msg_t              *cmsg = NULL;
  NW_Msg_Address_t      wmlMsgAddr, scriptMsgAddr;


  /* get the messaging address of wml and script interpreter */
  status = NW_Msg_LookupReceiverByName(NW_BROWSER_CORE, &wmlMsgAddr);
  if (status == KBrsrSuccess) {
    status = NW_Msg_LookupReceiverByName(NW_SCRIPT_SERVER, &scriptMsgAddr);
    if (status == KBrsrSuccess) {
      /* TODO: add transaction support */
      cmsg = NW_Msg_New(1, wmlMsgAddr, scriptMsgAddr, NW_MSG_SCRIPT_EXIT_REQ);
      if (cmsg == NULL) {
        status = KBrsrOutOfMemory;
      } else {
        /* Send the message to the script interpreter */
        status = NW_Msg_Send(cmsg);
      }
    }
  }

  /* clean up */ 
  if (status != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
  }
  return status;
}

/************************************************************************
  Function: NW_Wml_nokia_locationInfo
  Purpose: Handler for NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_REQ messages.
           Unpacks the message and invokes the script proxy. The script 
           proxy is responsible for sending a response to this message.
  Parameters:  msg - pointer to message containing dialog alert data

  Return Values:  KBrsrSuccess or KBrsrOutOfMemory or
                  KBrsrScrproxyScrAbort
**************************************************************************/
TBrowserStatusCode NW_Wml_nokia_locationInfoReq(NW_Msg_t *msg)
{
  TBrowserStatusCode       status;
  void              **args;
  NW_Ucs2           *url;
  NW_Ucs2           *ll_format;
  NW_Ucs2           *pn_format;
  NW_ScrProxy_t     *scrProxy;

  NW_LOG0(NW_LOG_LEVEL5, "WML : NW_Wml_nokia_locationInfoReq\n");

  /* decode the args */
  NW_ASSERT(msg != NULL);
  NW_ASSERT(msg->data != NULL);
  args = (void**)msg->data; 
  url       = (NW_Ucs2*) args[0];
  ll_format = (NW_Ucs2*) args[1];
  pn_format = (NW_Ucs2*) args[2];

  /* free the argument array, so the response can come in its place */
  NW_Mem_Free( msg->data );
  msg->data = NULL;

  /* get the proxy */
  scrProxy = Wml_GetScriptProxy();

  if (scrProxy == NULL) {     /* check if script is aborted */
    status = KBrsrScrproxyScrAbort;
  }
  else {
    status = NW_ScrProxy_nokia_locationInfo(scrProxy, 
                                            url, ll_format, pn_format);
  }
  if (status != KBrsrSuccess) {
    goto failure;
  }

  /* send a reply to the Script engine */
  status = NW_Msg_Reply( msg, NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_RESP );

  if (status != KBrsrSuccess) {
    status = NW_ScrProxy_ScrNotRespondingError(Wml_GetScriptProxy());
    goto failure;
  }

  /* Free only the format args; the browser now owns the URL.
     The message body will be deleted by the Script message handler */
  NW_Str_Delete( url );
  NW_Str_Delete( ll_format );
  NW_Str_Delete( pn_format );
  return status;

failure:
  NW_Msg_Delete( msg );
  NW_Str_Delete( url );
  NW_Str_Delete( ll_format );
  NW_Str_Delete( pn_format );
  return status;
}


