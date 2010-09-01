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


/*****************************************************************
**    File name:  AppServicesImpl.h
**    Part of: HED
**    Description:  Provides an interface to WTA functionality.
******************************************************************/

#ifndef NW_HED_APPSERVICESIMPL_H
#define NW_HED_APPSERVICESIMPL_H

#include "nwx_defs.h"
#include "nwx_string.h"
#include "nw_scrproxy.h"
#include "nw_hed_appservices.h"
#include "nwx_generic_dlg.h"
#include "nw_wae.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
/*
**-------------------------------------------------------------------------
**  Function declarations
**-------------------------------------------------------------------------
*/

/* TODO: add any other relevant api declarations */

/* error functions called by the WML Browser */
TBrowserStatusCode NW_UI_NotifyError(void *wae, const TBrowserStatusCode status,
                              void *callback_ctx,
                              NW_ErrorCB_t *callback);
TBrowserStatusCode NW_UI_HttpError(void *wae, const NW_WaeHttpStatus_t httpError,
                            const NW_Ucs2 *uri,
                            void *callback_ctx,
                            NW_ErrorCB_t *callback);

/* dialog api called by the script/authentication */
void NW_UI_DialogListSelect ( NW_Dlg_ListSelect_t *data,
                              void* callbackCtx,
                              NW_Dlg_ListSelectCB_t *callback );

void NW_UI_DialogPrompt ( NW_Dlg_Prompt_t* data,
                          void* callbackCtx, 
                          NW_Dlg_PromptCB_t *callback );

void NW_UI_DialogInputPrompt ( NW_Dlg_InputPrompt_t *data,
                               void* callbackCtx, 
                               NW_Dlg_InputPromptCB_t *callback );

void NW_UI_DlgAlert (void *wae, 
                     const NW_Ucs2 *message,
                     void  *callback_ctx,
                     NW_ScrProxy_DlgAlertCB_t *callback);

void NW_UI_DlgConfirm (void  *wae,
                       const NW_Ucs2 *message, 
                       const NW_Ucs2 *yesMessage,
                       const NW_Ucs2 *noMessage,
                       void  *callback_ctx,
                       NW_Dlg_ConfirmCB_t *callback);

void NW_UI_DlgPrompt (void *wae,
                      const NW_Ucs2 *message,
                      const NW_Ucs2 *defaultInput,
                      void  *callback_ctx,
                      NW_ScrProxy_DlgPromptCB_t *callback);


/*PictureViewer */
void NW_UI_PictureView_InitPVList(void *ctx);
void NW_UI_PictureView_AddItem(NW_Uint32* image);
 
/* SoftKey rountines */ 
void NW_UI_SoftKey_SetLeftKey(NW_SoftKeyText_t softKeyText);
void NW_UI_SoftKey_SetRightKey(NW_SoftKeyText_t softKeyText);

/* WTAI declarations */
TBrowserStatusCode NW_HED_wp_makeCall       (void *ctx,               /* context */ 
                                      const NW_Ucs2 *number);  /* arguments */
TBrowserStatusCode NW_HED_wp_sendDTMF       (void *ctx, 
                                      const NW_Ucs2 *dtmf);
TBrowserStatusCode NW_HED_wp_addPBEntry     (void *ctx, 
                                      const NW_Ucs2 *number, const NW_Ucs2 *name);
TBrowserStatusCode NW_HED_nokia_locationInfo(void *ctx, 
                                      NW_Ucs2 *url,
                                      NW_Ucs2 *ll_format,
                                      NW_Ucs2 *pn_format,
                                      NW_Http_Header_t *header);
/* URLLoadProgress API routines */
TBrowserStatusCode NW_UI_StopContentLoad(void *ctx,const NW_Ucs2* urlname);
TBrowserStatusCode NW_UI_StartContentLoad(void *ctx,const NW_Ucs2* urlname);
TBrowserStatusCode NW_UI_UrlLoadProgressOn(void *wae, NW_Uint16 transactionId);

/* User Redirection Response API routines */
void NW_UI_GetUserRedirectionData (void *wae,
                                   const NW_Ucs2 *originalUrl,
                                   const NW_Ucs2 *redirectedUrl,
                                   void  *callbackCtx,
                                   NW_Wae_UserRedirectionCB_t *callback);

/* Select List API declarations */
void NW_UI_SelectList_Cleanup(void);

/* Script Suspend/Resume Query Declaration */
TBrowserStatusCode NW_UI_ScriptSuspResQuery(void *ctx);

/* Mailto API */
TBrowserStatusCode NW_UI_SendEmailRequest(const NW_Ucs2 *url);

TBrowserStatusCode NW_UI_BrowserApp_Exit(void *ctx);

TBrowserStatusCode NW_UI_QuitSelect(void *ctx, TBrowserStatusCode status, NW_Dlg_Confirm_Result_t res);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_APPSERVICESIMPL_H */

