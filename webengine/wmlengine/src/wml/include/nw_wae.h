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
**   File: nw_wae.h
**   Subsystem Name: WAE User Agent
**   Purpose:  Provides interface to the WAE Browser. This module
**   has mostly glue logic that integrates the WML Interpreter with 
**   the URL Loaders
**************************************************************************/
#ifndef NW_WAE_H
#define NW_WAE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
**----------------------------------------------------------------------
** Includes
**----------------------------------------------------------------------
*/
#include "nw_errnotify.h"
#include "nw_scrproxy.h"
#include "nw_wml_core.h"
#include "nw_wml_api.h"
#include "nw_wtai_api.h"
#include "nwx_generic_dlg.h"
#include "nw_evlog_api.h"
#include "wml_wae_transaction.h"
#include "BrsrStatusCodes.h"


/*                                                                          */
/* Spectrum uses the new SIA/OTA Push application, which registers directly */
/* with the WSP stack, rather than the Rainbow OEM Push handlering code     */
/*                                                                          */

/* Define the default port number */
#define NW_DEFAULT_PORT 2948

/*
**----------------------------------------------------------------------
** Type Declarations
**----------------------------------------------------------------------
*/
  
/*
 * ---------------------------------------------------------------------
 */

typedef struct _NW_UrlLoadProgressApi_t     NW_UrlLoadProgressApi_t;
typedef struct _NW_AuthenticationApi_t      NW_AuthenticationApi_t;
typedef struct _NW_GenDlgApi_t              NW_GenDlgApi_t;
typedef struct _NW_WaeUsrAgent_t            NW_WaeUsrAgent_t;
typedef struct _NW_UserRedirectionApi_t     NW_UserRedirectionApi_t;

struct _NW_WaeUsrAgent_t {
  NW_Wml_t                        *wml;             /* WML Interpreter */
  NW_WaeTransaction_t             wmlReq;           /* WML Interpreter loader request */
  NW_ScrProxy_t                   *scrProxy;        /* WML Script Proxy */
  const NW_AuthenticationApi_t    *authApi;         /* Authentication API */
  const NW_GenDlgApi_t            *genDlgApi;       /* Generic dialogs API */
  const NW_UrlLoadProgressApi_t   *loadProgApi;     /* URL Load Progress API */
  const NW_ErrorApi_t             *errorApi;        /* Error API */
  const NW_WtaiApi_t              *wtaiApi;         /* Wtai API */
  const NW_EvLogApi_t             *evLogApi;        /* Event Logging API */
  void                            *uiCtx;           /* GUI Context */
  
  const NW_UserRedirectionApi_t   *userRedirection;  /* User Redirection */
};

/* The callback to call when done displaying a authentication dialog */
typedef TBrowserStatusCode (NW_Wae_AuthenticationCB_t)(void *callback_ctx, 
                                                TBrowserStatusCode status,
                                                const NW_Ucs2 *username, 
                                                const NW_Ucs2 *password,
                                                NW_Bool cancel,
                                                NW_Bool endPressed);

typedef TBrowserStatusCode (NW_Wae_UserRedirectionCB_t)(void *callback_ctx, 
                                                 TBrowserStatusCode status,
                                                 NW_Dlg_Confirm_Result_t resp);

/* Display a authentication dialog to the user */
struct _NW_AuthenticationApi_t {
  void (*getAuthenticationData)(void *wae,
                                void  *callbackCtx,
                                NW_Wae_AuthenticationCB_t *callback);
  void (*getAuthenticationDataFailed)(void *msg,
                                      NW_Dlg_ConfirmCB_t *callback);
}; 

/* Display a authentication dialog to the user */
struct _NW_UserRedirectionApi_t {
  void (*getUserRedirectionData)(void *wae,
                                 const NW_Ucs2 *originalUrl,
                                 const NW_Ucs2 *redirectedUrl,
                                 void  *callbackCtx,
                                 NW_Wae_UserRedirectionCB_t *callback);
}; 

struct _NW_GenDlgApi_t {
  void (*DialogListSelect)(NW_Dlg_ListSelect_t *data, void* callbackCtx, NW_Dlg_ListSelectCB_t* callback);
  void (*DialogPrompt)(NW_Dlg_Prompt_t *data, void* callbackCtx, NW_Dlg_PromptCB_t* callback);
  void (*DialogInputPrompt)(NW_Dlg_InputPrompt_t *data, void* callbackCtx, NW_Dlg_InputPromptCB_t* callback);
  void (*DialogSelectOption)(NW_Dlg_SelectData_t *data, void* callbackCtx, NW_Dlg_SelectCB_t* callback);
  void (*DetailsDialogHashkeyPressed)(NW_Ucs2* input, void* callbackCtx, NW_Dlg_DetailsDlgCB_t* callback);
}; 

/* URL load process API, UI interface*/
struct _NW_UrlLoadProgressApi_t {
  /* start of a URL load ie. request to load a URL has been
  sent to the URL Loader. */
  TBrowserStatusCode (*start)  (void *wae, const NW_Ucs2 *url);
  /* the URL Loader has completed the load. */
  TBrowserStatusCode (*finish) (void *wae, const NW_Ucs2 *url);
  /* Progress status is available */
  TBrowserStatusCode (*load_progress_on) (void *wae, NW_Uint16 transactionId);

}; 

/*
**----------------------------------------------------------------------
** Global Function Declarations
**----------------------------------------------------------------------
*/

/* Create the wae browser */
NW_WaeUsrAgent_t *NW_WaeUsrAgent_New();

/*
 * Free the wae browser. This function will shutdown the
 * URL Loader, WML Interpreter, Image Loader and the script proxy. 
 */
void NW_WaeUsrAgent_Free(NW_WaeUsrAgent_t *wae);



TBrowserStatusCode NW_Wae_RegisterWml(NW_WaeUsrAgent_t *wae, NW_Wml_t *wml);

TBrowserStatusCode NW_Wae_UnRegisterWml(NW_WaeUsrAgent_t *wae);

TBrowserStatusCode NW_Wae_RegisterWmlScriptProxy(NW_WaeUsrAgent_t *wae, 
                                          NW_ScrProxy_t *scrProxy);

TBrowserStatusCode NW_Wae_UnRegisterWmlScriptProxy(NW_WaeUsrAgent_t *wae);


/*                                                                          */
/* Spectrum uses the new SIA/OTA Push application, which registers directly */
/* with the WSP stack, rather than the Rainbow OEM Push handlering code     */
/*                                                                          */

TBrowserStatusCode 
NW_WaeUsrAgent_RegisterGenDlg(NW_WaeUsrAgent_t *wae,
                          const NW_GenDlgApi_t *genDlgApi);

TBrowserStatusCode 
NW_WaeUsrAgent_UnRegisterGenDlg(NW_WaeUsrAgent_t *wae);


/* 
 * Get a pointer to the script proxy. The caller should not free 
 * the returned pointer. 
 */
NW_ScrProxy_t *NW_WaeUsrAgent_GetScript(NW_WaeUsrAgent_t *wae);


/*                                                                          */
/* Spectrum uses the new SIA/OTA Push application, which registers directly */
/* with the WSP stack, rather than the Rainbow OEM Push handlering code     */
/*                                                                          */

TBrowserStatusCode NW_WaeUsrAgent_RegisterError(NW_WaeUsrAgent_t *wae, 
                                         const NW_ErrorApi_t *errorApi);

TBrowserStatusCode NW_WaeUsrAgent_RegisterLoadProgress(NW_WaeUsrAgent_t *wae, 
                                                const NW_UrlLoadProgressApi_t *loadProgApi);


/*                                                                          */
/* Spectrum uses the new SIA/OTA Push application, which registers directly */
/* with the WSP stack, rather than the Rainbow OEM Push handlering code     */
/*                                                                          */

/* Register event log API */
TBrowserStatusCode NW_WaeUsrAgent_RegisterEvLog(NW_WaeUsrAgent_t *wae,
                                         const NW_EvLogApi_t *evLogApi);

/* Unregister event log API */
TBrowserStatusCode NW_WaeUsrAgent_UnRegisterEvLog(NW_WaeUsrAgent_t *wae);

/* Get a pointer to the EvLogApi. */
const NW_EvLogApi_t *NW_WaeUsrAgent_GetEvLog(NW_WaeUsrAgent_t *wae);

TBrowserStatusCode 
NW_WaeUsrAgent_RegisterUserRedirection(NW_WaeUsrAgent_t *wae,
                                       const NW_UserRedirectionApi_t *userRedirectionapi);

TBrowserStatusCode 
NW_WaeUsrAgent_UnRegisterUserRedirection(NW_WaeUsrAgent_t *wae);



TBrowserStatusCode NW_WaeUsrAgent_DlgListSelect(NW_WaeUsrAgent_t *wae, 
                                         NW_Dlg_ListSelect_t *data,
                                         void *callbackCtx,
                                         NW_Dlg_ListSelectCB_t callback);

TBrowserStatusCode NW_WaeUsrAgent_DlgPrompt(NW_WaeUsrAgent_t *wae, 
                                     NW_Dlg_Prompt_t *data,
                                     void *callbackCtx,
                                     NW_Dlg_PromptCB_t callback);

TBrowserStatusCode NW_WaeUsrAgent_DlgInputPrompt(NW_WaeUsrAgent_t *wae, 
                                          NW_Dlg_InputPrompt_t *data,
                                          void *callbackCtx,
                                          NW_Dlg_InputPromptCB_t callback);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NW_WAE_H */
