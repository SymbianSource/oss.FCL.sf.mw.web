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
**   File: nw_scr_api.h
**   Subsystem Name: WAE User Agent
**   Purpose:  This module is responsible for coordinating the WML Script Engine
**   with the WML Interpreter and the User Interface.
**************************************************************************/
#ifndef NW_SCR_API_H
#define NW_SCR_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include "nw_wtai_api.h"
#include "nwx_http_header.h"
#include "nw_errnotify.h"
#include "nwx_generic_dlg.h"
#include "BrsrStatusCodes.h"

/*
** Type Declarations
*/

/**
 * The context of the script proxy.
 * Created by NW_ScrProxy_New().
 * Destroyed by NW_ScrProxy_Free().
**/
typedef struct _NW_ScrProxy_t         NW_ScrProxy_t;

/**
 * The callback to call when done displaying a script confirm dialog.
**/

//#define NW_Dlg_ConfirmCB_t NW_ScrProxy_DlgConfirmCB_t;
/**
 * The callback to call when done displaying a script alert dialog.
**/
typedef TBrowserStatusCode (NW_ScrProxy_DlgAlertCB_t)    (void *callback_ctx, 
                                                   TBrowserStatusCode status, 
                                                   NW_Dlg_Confirm_Result_t result);

/**
 * The callback to call when done displaying a script prompt dialog.
**/
typedef TBrowserStatusCode (NW_ScrProxy_DlgPromptCB_t)   (void *callback_ctx, TBrowserStatusCode status, 
                                                   const NW_Ucs2 *input, NW_Bool endPressed);

/**
 * The callback functions will be invoked by the script proxy to inform the user agent that a 
 * WML Script has started and completed.
**/
typedef struct {

  /* WML Script NW_Byte code has been downloaded and its about to execute. */
  TBrowserStatusCode (*start)(void *wae, const NW_Ucs2 *url);

  /* script execution has completed. */
  TBrowserStatusCode (*finish)(void *wae, TBrowserStatusCode status,
                        const NW_Ucs2 *message);
} NW_Scr_ProgressApi_t; /* Script progress, UI interface */

/**
 * The callback functions will be invoked by the script proxy to inform the user agent that a dialog
 * has to displayed.
**/
typedef struct {
  
  /* display a alert dialog to the user */
  void (*dlgAlert)(void *wae, const NW_Ucs2 *message,
                  void  *callback_ctx,
                  NW_ScrProxy_DlgAlertCB_t *callback);

  /* display a confirm dialog to the user */
  void (*dlgConfirm)(void *wae,
                    const NW_Ucs2 *message, 
                    const NW_Ucs2 *yesMessage,
                    const NW_Ucs2 *noMessage,
                    void  *callback_ctx,
                    NW_Dlg_ConfirmCB_t *callback);

  /* display a prompt dialog to the user */
  void (*dlgPrompt)(void *wae,
                    const NW_Ucs2 *message,
                    const NW_Ucs2 *defaultInput,
                    void  *callback_ctx,
                    NW_ScrProxy_DlgPromptCB_t *callback);
} NW_Scr_DialogApi_t; /* Script dialog libraries, UI interface */



typedef struct {

  /* retreive a variable */
  TBrowserStatusCode (*getVar) (void *ctx, const NW_Ucs2 *var, NW_Ucs2 ** ret_string);

  /* set a variable */
  TBrowserStatusCode (*setVar) (void *ctx, const NW_Ucs2 *var, const NW_Ucs2 *value);

  TBrowserStatusCode (*go) (void *ctx, const NW_Ucs2 *url, const NW_Ucs2 *param,
                     NW_Http_Header_t *header);

  TBrowserStatusCode (*prev) (void *ctx);

  TBrowserStatusCode (*newContext) (void *ctx);

  TBrowserStatusCode (*refresh) (void *ctx);

} NW_Scr_WmlBrowserApi_t; /* WMLBrowser Library Api */

typedef struct {
  
  /* script suspend/resume api */
  TBrowserStatusCode (*scriptSuspResQuery) (void *ctx);

} NW_Scr_SuspResQueryApi_t;

/*
** Global Function Declarations
*/

NW_ScrProxy_t *NW_ScrProxy_New();

void NW_ScrProxy_Free(NW_ScrProxy_t *proxy);

/* initialize the script proxy */
TBrowserStatusCode 
NW_ScrProxy_Initialize(NW_ScrProxy_t *scrProxy, void *ctx,
                       const NW_Scr_WmlBrowserApi_t *scrBrowserApi, 
                       const NW_Scr_ProgressApi_t   *scrProgressApi,
                       const NW_Scr_DialogApi_t     *scrDlgApi,
                       const NW_WtaiApi_t           *wtaiApi,
                       const NW_Scr_SuspResQueryApi_t *scrSuspResQueryApi,
                       NW_NotifyError_t             errApi);

/* run a script */
TBrowserStatusCode
NW_ScrProxy_RunScript(NW_ScrProxy_t *scrProxy,
                      NW_Ucs2 *url, NW_Ucs2 *postfields, NW_Ucs2 *referer,
                      NW_Byte *content, const NW_Uint32 len,
                      const NW_Http_CharSet_t charset);

/**
* This function abort's the currently running WML Script.
**/
TBrowserStatusCode NW_ScrProxy_AbortScript(NW_ScrProxy_t *scrProxy);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NW_SCR_API_H */
