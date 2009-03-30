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
* Description:  This module has the logic to coordinate the 
*                interaction between the WML Script Engine, WML Interpreter
*                and the user agent ie. typically the GUI.
*                (1) acts as proxy to the script interpreter. All the calls to the script
*                    interpreter are tunneled through this module.
*                (2) invokes the wml interpreter for script requests to the WML Browser 
*                    Library
*                (3) invokes the user agent for the script requests to the Dialog libraries
*                (4) propogates start of script exection and finish to the user agent
*                (5) exposes API's to abort scripts, query the state of the script 
*                    interpreter(idle, busy) .
*
*/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "wml_scrproxy.h"
#include "wml_wae.h"    /* wml script interface */
#include "nwx_generic_dlg.h"
#include "nwx_ctx.h"
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

typedef enum {
  SCR_STATE_INIT,
  SCR_STATE_RUN_REQ,
  SCR_STATE_RUNNING,
  SCR_STATE_DONE,
  SCR_STATE_ABORT_REQ,
  SCR_STATE_ABORTED
} NW_Script_State_t;

struct _NW_ScrProxy_t {
  void                    *ctx;
  const NW_ScriptSrvApi_t *scrSvrApi;
  const NW_Scr_ProgressApi_t   *scrProgressApi;
  const NW_Scr_DialogApi_t     *scrDlgApi;
  NW_NotifyError_t             errApi;
  const NW_WtaiApi_t      *wtaiApi;
  NW_Script_State_t       state;  /* state of the script engine */
  const NW_Scr_WmlBrowserApi_t *scrBrowserApi;
  const NW_Scr_SuspResQueryApi_t *scrSuspResQueryApi;
};

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
static void *ScrProxy_GetUserAgent(NW_ScrProxy_t *scrProxy);
/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

static const NW_ScriptSrvApi_t scriptSrv_Api =
{
  NW_Wml_RunScript,
  NW_Wml_AbortScript,
};

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
static void *ScrProxy_GetUserAgent(NW_ScrProxy_t *scrProxy)
{
  NW_ASSERT(scrProxy != NULL);
  return scrProxy->ctx;
}

static void
ScrProxy_SetScriptApi(NW_ScrProxy_t *scrProxy,
                      const NW_ScriptSrvApi_t *scrSvrApi)
{
   scrProxy->scrSvrApi = scrSvrApi;
}

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
/*****************************************************************

  Name: NW_ScrProxy_New()

  Description:  Create a new script proxy.

  Parameters:  

  Return Value: pointer to the script proxy

******************************************************************/
NW_ScrProxy_t *NW_ScrProxy_New()
{
  NW_ScrProxy_t *proxy = (NW_ScrProxy_t*) NW_Mem_Malloc(sizeof(NW_ScrProxy_t));

  if (proxy != NULL) {
    proxy->scrSvrApi = NULL;
    proxy->scrProgressApi  = NULL;
    proxy->scrDlgApi       = NULL;
    proxy->scrBrowserApi   = NULL;
    proxy->state     = SCR_STATE_INIT;
  }
  return proxy;
}

/*****************************************************************

  Name: NW_ScrProxy_Free()

  Description:  Free the script proxy.

  Parameters:  proxy - pointer to the script proxy

  Return Value: 

******************************************************************/
void NW_ScrProxy_Free(NW_ScrProxy_t *proxy)
{
  if (proxy != NULL) {
    NW_Mem_Free(proxy);
  }
}

/*****************************************************************

  Name: NW_ScrProxy_Initialize()

  Description:  Initialize the script proxy.

  Parameters:   scrProxy  - pointer to the script proxy
                ctx       - context of the caller
                scrBrowserApi  - interface to browser (WML context)
                scrProgressApi - interface to browser (script progress)
                scrDlgApi      - interface to dialogs
                wtaiApi        - interface to WTAI
                errApi         - interface to browser (error notification)

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode 
NW_ScrProxy_Initialize(NW_ScrProxy_t *scrProxy,
                       void *ctx,
                       const NW_Scr_WmlBrowserApi_t *scrBrowserApi,
                       const NW_Scr_ProgressApi_t   *scrProgressApi,
                       const NW_Scr_DialogApi_t     *scrDlgApi,
                       const NW_WtaiApi_t           *wtaiApi,
                       const NW_Scr_SuspResQueryApi_t *scrSuspResQueryApi,
                       NW_NotifyError_t             errApi)
{
  if ((scrProxy != NULL) && (scrBrowserApi != NULL)
    && (scrProgressApi != NULL) && (scrDlgApi != NULL) && (errApi != NULL)) {
    scrProxy->ctx       = ctx;
    scrProxy->scrBrowserApi   = scrBrowserApi;
    scrProxy->scrProgressApi  = scrProgressApi;
    scrProxy->scrDlgApi       = scrDlgApi;
    scrProxy->errApi    = errApi;
    scrProxy->wtaiApi   = wtaiApi;
    scrProxy->state     = SCR_STATE_INIT;
    scrProxy->scrSuspResQueryApi = scrSuspResQueryApi;
    ScrProxy_SetScriptApi(scrProxy, &scriptSrv_Api);
    return KBrsrSuccess;
  }
  return KBrsrBadInputParam;
}

/*****************************************************************

  Name: NW_ScrProxy_RunScript()

  Description:  Run a script. Updates the state machine to indicate 
                that a request to run a script has been made.

  Parameters:   scrProxy  - pointer to the script proxy
                wml       - the wml interpreter that is making the request
                url       - location from which the script is downloaded
                referer   - the referer to the script
                content   - encoded wml script
                len       - lenght of the content
                charset   - charset

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrBusy  - the script engine is busy
******************************************************************/
TBrowserStatusCode 
NW_ScrProxy_RunScript(NW_ScrProxy_t *scrProxy, 
                      NW_Ucs2 *url, NW_Ucs2 *postfields,
                      NW_Ucs2 *referer, NW_Byte *content,
                      const NW_Uint32 len, const NW_Http_CharSet_t charset)
{
  TBrowserStatusCode   ret_status;
  void          *ctx;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  NW_ASSERT(url != NULL);
  NW_ASSERT(content != NULL);
  NW_ASSERT(referer != NULL);
  if ((scrProxy->state == SCR_STATE_RUNNING)
    || (scrProxy->state == SCR_STATE_ABORT_REQ)
    || (scrProxy->state == SCR_STATE_RUN_REQ)) {
    return KBrsrScrproxyScrBusy;
  }
  
  
  NW_ASSERT(scrProxy->scrSvrApi != NULL);

  /* run the script */
  ret_status = scrProxy->scrSvrApi->run(url, postfields, referer, content, len, charset);
  if (ret_status == KBrsrSuccess) {
    scrProxy->state = SCR_STATE_RUN_REQ;

    /* inform the user that we have started a new script */
    if (scrProxy->scrProgressApi != NULL) {
      ctx = ScrProxy_GetUserAgent(scrProxy);
      if ((ret_status = scrProxy->scrProgressApi->start(ctx, url))
        != KBrsrSuccess) {
        return ret_status;
      }
    }
  }
  return ret_status;
}

/*****************************************************************

  Name: NW_ScrProxy_RunScriptAck()

  Description:  Acknowledgement for a run script request.

  Parameters:   scrProxy  - pointer to the script proxy
                status    - status of the script interpreter

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_RunScriptAck(NW_ScrProxy_t *scrProxy,
                                     const TBrowserStatusCode status)
{
  void *ctx;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  /* validate our state */
  NW_ASSERT(scrProxy->state == SCR_STATE_RUN_REQ);
  if (scrProxy->state == SCR_STATE_RUN_REQ) {
    if (status == KBrsrSuccess) {
      scrProxy->state = SCR_STATE_RUNNING;
    } else {
      /* failed to run the script */
      scrProxy->state = SCR_STATE_ABORTED;
      /* script has completed */
      ctx = ScrProxy_GetUserAgent(scrProxy);
      if (scrProxy->scrProgressApi != NULL) {
        return scrProxy->scrProgressApi->finish(ctx, status, NULL);
      }
    }
  }
  return KBrsrSuccess;
}

/*****************************************************************

  Name: NW_ScrProxy_Finish()

  Description:  Script execution has completed.

  Parameters:   scrProxy  - pointer to the script proxy
                status    - status of the script interpreter
                message   - messgae from the script interpreter

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_Finish(NW_ScrProxy_t *scrProxy,
                               const TBrowserStatusCode status,
                               const NW_Ucs2 *message)
{
  void          *ctx;
  TBrowserStatusCode   ret_status = KBrsrSuccess;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  /* validate our state */
  NW_ASSERT(scrProxy->state == SCR_STATE_RUNNING);
  if (scrProxy->state == SCR_STATE_RUNNING) {
    scrProxy->state = SCR_STATE_DONE;
    
    ctx = ScrProxy_GetUserAgent(scrProxy);
    /* inform the user that the script has completed */
    if (scrProxy->scrProgressApi != NULL) {
      ret_status = scrProxy->scrProgressApi->finish(ctx,
                                            status, message);
    }
  }
  return ret_status;
}

/*****************************************************************

  Name: NW_ScrProxy_AbortAck()

  Description:  Acknowledgement from the script interpreter for the 
                abort request.

  Parameters:   scrProxy  - pointer to the script proxy
                status    - status of the script interpreter

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_AbortAck(NW_ScrProxy_t *scrProxy,
                                 const TBrowserStatusCode status)
{
  void          *ctx;
  TBrowserStatusCode   ret_status = KBrsrSuccess;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  /* validate our state */
  NW_ASSERT(scrProxy->state == SCR_STATE_ABORT_REQ);
  if (scrProxy->state == SCR_STATE_ABORT_REQ) {
    scrProxy->state = SCR_STATE_ABORTED;

    ctx = ScrProxy_GetUserAgent(scrProxy);
    /* inform the user that the script has completed */
    if (scrProxy->scrProgressApi != NULL) {
      ret_status = scrProxy->scrProgressApi->finish(ctx,
                                        status, NULL);
    }
  }
  return ret_status;
}

/*****************************************************************

  Name: NW_ScrProxy_DlgAlert()

  Description:  Script library dialog alert.

  Parameters:   scrProxy  - pointer to the script proxy
                message   - message to display to the user

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_DlgAlert(NW_ScrProxy_t *scrProxy,
                                 const NW_Ucs2 *message,
                                 void          *callbackCtx,
                                 NW_ScrProxy_DlgAlertCB_t *callback)
{
  TBrowserStatusCode status = KBrsrSuccess;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  if (scrProxy->state == SCR_STATE_RUNNING) {
    void *ctx = ScrProxy_GetUserAgent(scrProxy);
    if (scrProxy->scrDlgApi != NULL) {
      scrProxy->scrDlgApi->dlgAlert(ctx, message, 
                callbackCtx, callback);
    }
  } else {
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_NotifyError()

  Description:  Script library dialog error notification

  Parameters:   scrProxy  - pointer to the script proxy
                message   - message to display to the user

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_NotifyError(NW_ScrProxy_t *scrProxy,
                                 const TBrowserStatusCode Err)
{
  TBrowserStatusCode status = KBrsrSuccess;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  if (scrProxy->state == SCR_STATE_RUNNING) {
    void *ctx = ScrProxy_GetUserAgent(scrProxy);
    if (scrProxy->errApi != NULL) {
      (void) scrProxy->errApi(ctx, Err, NULL, NULL);
    }
  } else {
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}
/*****************************************************************

  Name: NW_ScrProxy_DlgConfirm()

  Description:  Script library dialog confirm.

  Parameters:   scrProxy    - pointer to the script proxy
                message     - message to display to the user
                yesMessage  - yes message
                noMessage   - no message
                confirm     - user confirmation

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_DlgConfirm(NW_ScrProxy_t   *scrProxy,
                            const NW_Ucs2 *message, 
                            const NW_Ucs2 *yesMessage,
                            const NW_Ucs2 *noMessage,
                            void          *callbackCtx,
                            NW_Dlg_ConfirmCB_t *callback)
{
  TBrowserStatusCode status = KBrsrSuccess;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING) {
    void *ctx = ScrProxy_GetUserAgent(scrProxy);
    if (scrProxy->scrDlgApi != NULL) {
      NW_ASSERT(scrProxy->scrDlgApi->dlgConfirm != NULL);
      scrProxy->scrDlgApi->dlgConfirm(ctx, message,
                yesMessage, noMessage, callbackCtx, callback);
    }
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_DlgPrompt()

  Description:  Script library dialog prompt.

  Parameters:   scrProxy      - pointer to the script proxy
                message       - message to display to the user
                defaultInput  - default input string
                ret_string    - return string

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_DlgPrompt(NW_ScrProxy_t *scrProxy,
                                  const NW_Ucs2 *message,
                                  const NW_Ucs2 *defaultInput,
                                  void          *callbackCtx,
                                  NW_ScrProxy_DlgPromptCB_t *callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING) {
    void *ctx = ScrProxy_GetUserAgent(scrProxy);
    if (scrProxy->scrDlgApi != NULL) {
      scrProxy->scrDlgApi->dlgPrompt(ctx, message, 
                defaultInput, callbackCtx, callback);
    }
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_GenericDlgListSelect()

  Description:  Display a generic dialog box with a prompt only.

  Parameters:   scrProxy      - pointer to the script proxy
                data - holds all the strings to display


  Return Value: 
                KBrsrSuccess
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_GenericDlgListSelect(NW_ScrProxy_t *scrProxy, NW_Dlg_ListSelect_t *data,
                                             void *callbackCtx, NW_Dlg_ListSelectCB_t* callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  void *wae = NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING && wae != NULL) {
    (void) NW_WaeUsrAgent_DlgListSelect((NW_WaeUsrAgent_t*) wae, data, callbackCtx, callback);  
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_GenericDlgPrompt()

  Description:  Display a generic dialog box with a prompt only.

  Parameters:   scrProxy      - pointer to the script proxy
                data - holds all the strings to display


  Return Value: 
                KBrsrSuccess
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_GenericDlgPrompt(NW_ScrProxy_t *scrProxy, NW_Dlg_Prompt_t *data,
                                         void *callbackCtx, NW_Dlg_PromptCB_t* callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  void *wae = NW_Ctx_Get(NW_CTX_WML_CORE, 0);

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING && wae != NULL) {
    (void)NW_WaeUsrAgent_DlgPrompt((NW_WaeUsrAgent_t*) wae, data, callbackCtx, callback);  
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_GenericDlgInputPrompt()

  Description:  Display a generic dialog box with a prompt for user input.

  Parameters:   scrProxy      - pointer to the script proxy
                data - holds all the strings to display


  Return Value: 
                KBrsrSuccess
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_GenericDlgInputPrompt(NW_ScrProxy_t *scrProxy, NW_Dlg_InputPrompt_t *data,
                                         void *callbackCtx, NW_Dlg_InputPromptCB_t* callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  void *wae = NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING && wae != NULL) {
    (void)NW_WaeUsrAgent_DlgInputPrompt((NW_WaeUsrAgent_t*) wae, data, callbackCtx, callback);  
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_GetVar()

  Description:  Get a variable from the wml intepreter.

  Parameters:   scrProxy      - pointer to the script proxy
                var           - variable to query

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_GetVar(NW_ScrProxy_t *scrProxy,
                              const NW_Ucs2 *var, 
                              NW_Ucs2 **ret_string)
{
  TBrowserStatusCode status;
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING) {
    status = scrProxy->scrBrowserApi->getVar(scrProxy->ctx, var, ret_string);
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_SetVar()

  Description:  Set a variable on the wml intepreter.

  Parameters:   scrProxy      - pointer to the script proxy
                var           - new variable
                value         - variable value

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_SetVar(NW_ScrProxy_t *scrProxy, 
                              const NW_Ucs2 *var, 
                              const NW_Ucs2 *value)
{
  TBrowserStatusCode status;
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING) {
    status = scrProxy->scrBrowserApi->setVar(scrProxy->ctx, var, value);
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_LoadPrev()

  Description:  Perform a prev task on the wml interpreter.

  Parameters:   scrProxy      - pointer to the script proxy

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_LoadPrev(NW_ScrProxy_t *scrProxy)
{
  TBrowserStatusCode status;
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  if (scrProxy->state == SCR_STATE_RUNNING) {
    status = scrProxy->scrBrowserApi->prev(scrProxy->ctx);
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_NewContext()

  Description:  Perform a newcontext on the wml interpreter.

  Parameters:   scrProxy      - pointer to the script proxy

  Return Value: 
                KBrsrSuccess
                KBrsrScrproxyScrAbort
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_NewContext(NW_ScrProxy_t *scrProxy)
{
  TBrowserStatusCode status;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  if (scrProxy->state == SCR_STATE_RUNNING)
  {
    status = scrProxy->scrBrowserApi->newContext(scrProxy->ctx);
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_Refresh()

  Description:  Perform a refresh on the wml interpreter.

  Parameters:   scrProxy      - pointer to the script proxy

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_Refresh(NW_ScrProxy_t *scrProxy)
{
  TBrowserStatusCode status;  

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }


  if (scrProxy->state == SCR_STATE_RUNNING) {
    status = scrProxy->scrBrowserApi->refresh(scrProxy->ctx);
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }

  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_LoadUrl()

  Description:  Perform a load url on the wml interpreter.

  Parameters:   scrProxy    - pointer to the script proxy
                url         -  url to load
                param       - ???

  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
                KBrsrOutOfMemory
                KBrsrScrproxyScrAbort
******************************************************************/
TBrowserStatusCode NW_ScrProxy_LoadUrl(NW_ScrProxy_t *scrProxy, const NW_Ucs2 *url, 
                                const NW_Ucs2 *param)
{
  TBrowserStatusCode status;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
  
  if (scrProxy->state == SCR_STATE_RUNNING) {
    status =  scrProxy->scrBrowserApi->go(scrProxy->ctx, url, param, NULL);
  } else {
    /* ignore the request */
    status = KBrsrScrproxyScrAbort;
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_AbortScript()

  Description:  Abort a running script. The script is only aborted
                after we get a abort acknowledgement.

  Parameters:   scrProxy    - pointer to the script proxy
 
  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_AbortScript(NW_ScrProxy_t *scrProxy)
{
  TBrowserStatusCode status = KBrsrSuccess;
  
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  if ((scrProxy->state == SCR_STATE_RUNNING) ||
     (scrProxy->state == SCR_STATE_RUN_REQ)) {

    NW_ASSERT(scrProxy->scrSvrApi != NULL);

    /* abort the script */
    if ((status = scrProxy->scrSvrApi->abort()) == KBrsrSuccess)
    {
      scrProxy->state = SCR_STATE_ABORT_REQ;
    }
  }
  return status;
}

/*****************************************************************

  Name: NW_ScrProxy_IsScriptRunning()

  Description:  export the state of the script interpreter.

  Parameters:   scrProxy    - pointer to the script proxy
                result      - "NW_TRUE" script is running
                              "NW_FALSE" script is not running
 
  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_IsScriptRunning(NW_ScrProxy_t *scrProxy,
                                        NW_Bool *result)
{
  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }
 
  if ((scrProxy->state == SCR_STATE_RUN_REQ) ||
     (scrProxy->state == SCR_STATE_RUNNING) ||
     (scrProxy->state == SCR_STATE_ABORT_REQ)) {
    *result = NW_TRUE;
  } else {
    *result =NW_FALSE;
  }
  return KBrsrSuccess;
}

/*****************************************************************

  Name: NW_ScrProxy_ScrNotRespondingError()

  Description:  script interpreter is not respoinding.

  Parameters:   scrProxy    - pointer to the script proxy
 
  Return Value: 
                KBrsrSuccess
                KBrsrBadInputParam
******************************************************************/
TBrowserStatusCode NW_ScrProxy_ScrNotRespondingError(NW_ScrProxy_t *scrProxy)
{
  
  void *ctx;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  if ((scrProxy->state == SCR_STATE_RUN_REQ) ||
     (scrProxy->state == SCR_STATE_RUNNING) ||
     (scrProxy->state == SCR_STATE_ABORT_REQ)) {
      
    scrProxy->state = SCR_STATE_ABORTED;
  
    ctx = ScrProxy_GetUserAgent(scrProxy);
    /* send a script finish to the user .*/
    if (scrProxy->scrProgressApi != NULL) {
      (void) scrProxy->scrProgressApi->finish(ctx, KBrsrFailure, NULL);
    }
    /* flag an error to the user. */
    if (scrProxy->errApi != NULL) {
      (void) scrProxy->errApi(ctx, KBrsrScrproxyScrNotResponding, NULL, NULL);
    }
  }
  return KBrsrSuccess;
}


/*****************************************************************

  Name: NW_ScrProxy_nokia_locationInfo()

  Description:  see [WAP-170]

  Parameters:   scrProxy      - pointer to the script proxy

  Return Value: 
                KBrsrSuccess
                KBrsrScrproxyScrAbort
******************************************************************/

TBrowserStatusCode NW_ScrProxy_nokia_locationInfo(NW_ScrProxy_t    *scrProxy, 
                                           NW_Ucs2     *url, 
                                           NW_Ucs2     *ll_format,
                                           NW_Ucs2     *pn_format)
{
//#pragma message ("TODO:: wml_scr_proxy.c, fix NW_ScrProxy_nokia_locationInfo")
  NW_REQUIRED_PARAM(scrProxy);  
  NW_REQUIRED_PARAM(url);
  NW_REQUIRED_PARAM(ll_format);
  NW_REQUIRED_PARAM(pn_format);

#if 0
  TBrowserStatusCode status = KBrsrSuccess;
  NW_WtaiApi_t wtaiApi;
  void *ctx;
  NW_Http_Header_t *header = NULL;

  if (scrProxy == NULL) {
    return KBrsrBadInputParam;
  }

  /* If no API in proxy, get the API from the service factory */
  if ( scrProxy->wtaiApi == NULL ) 
  {
    wtaiApi = NW_PlatformServicesFactory_GetScrWtaiService();
  }
  /* If only the method is null, get the API from the service factory */
  else if (scrProxy->wtaiApi->nokia_locationInfo == NULL ) 
  {
    wtaiApi = NW_PlatformServicesFactory_GetScrWtaiService();
  }
  else
  {
    wtaiApi = *(scrProxy->wtaiApi);
  }

  if (scrProxy->state == SCR_STATE_RUNNING) 
  {
    ctx = ScrProxy_GetUserAgent(scrProxy);
    
    if (wtaiApi.nokia_locationInfo != NULL) 
    {
      header = NW_Http_Header_New(NULL, 0, NW_TRUE, NW_TRUE /* use POST */);
      if (header == NULL) 
      {
        return KBrsrOutOfMemory;
      }

      status = (wtaiApi.nokia_locationInfo)(ctx, url, 
                                            ll_format, pn_format,
                                            header);
      if (status != KBrsrSuccess) 
      {
        UrlLoader_HeadersFree(header);
        return status;
      }

      status =  scrProxy->scrBrowserApi->go(scrProxy->ctx, url, NULL, header);
      /* go() makes copies of url & header.  we own header so free it here */
      UrlLoader_HeadersFree(header);
      if (status != KBrsrSuccess) 
      {
        return status;
      }
    }
  } 
  else 
  {
    status = KBrsrScrproxyScrAbort;
  }
  return status;
#endif
  return KBrsrFailure;
}

