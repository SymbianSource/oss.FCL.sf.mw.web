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
**   File: wml_scrproxy.h
**   Subsystem Name: WAE User Agent
**   Purpose:  This module is responsible for coordinating the WML Script Engine
**   with the WML Interpreter and the User Interface.
**************************************************************************/
#ifndef WML_SCRPROXY_H
#define WML_SCRPROXY_H

/*
** Includes
*/
#include "nw_scrproxy.h"
#include "nwx_generic_dlg.h"
#include "nw_wtai_api.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/*
** Type Declarations
*/
/* interface to the script interpreter */
typedef struct {
  /* request to run a script. transfer ownership of the url, referer, and content
  to the script server */
  TBrowserStatusCode (*run) (NW_Ucs2 *url, NW_Ucs2 *postfields,
                     NW_Ucs2 *referer, NW_Byte *content,
                     const NW_Uint32 len, const NW_Http_CharSet_t charset);

  /* request to abort the running script */
  TBrowserStatusCode (*abort) (void);
} NW_ScriptSrvApi_t;


/*
** Global Function Declarations
*/
/*************** interface from the WAE Browser ***************/

/* dispose the script proxy */
/* this function is not being used any where.
TBrowserStatusCode NW_ScrProxy_Dispose(NW_ScrProxy_t *scrProxy);
*/

/* script interpreter is not responding */
TBrowserStatusCode NW_ScrProxy_ScrNotRespondingError(NW_ScrProxy_t *scrProxy);

/* check if any script is still running */
TBrowserStatusCode NW_ScrProxy_IsScriptRunning(NW_ScrProxy_t *scrProxy,
                                        NW_Bool *result);

/* acknowledgement for a script start */
TBrowserStatusCode NW_ScrProxy_RunScriptAck(NW_ScrProxy_t *scrProxy,
                                     const TBrowserStatusCode status);

/* script has completed */
TBrowserStatusCode NW_ScrProxy_Finish(NW_ScrProxy_t *scrProxy,
                               const TBrowserStatusCode status,
                               const NW_Ucs2 *message);

/* acknowledgement for a script abort */
TBrowserStatusCode NW_ScrProxy_AbortAck(NW_ScrProxy_t *scrProxy,
                                 const TBrowserStatusCode status);

/* alert the user with a dialog */
TBrowserStatusCode NW_ScrProxy_DlgAlert(NW_ScrProxy_t *scrProxy, 
                                 const NW_Ucs2 *message,
                                 void  *callbackCtx,
                                 NW_ScrProxy_DlgAlertCB_t *callback);

TBrowserStatusCode NW_ScrProxy_NotifyError(NW_ScrProxy_t *scrProxy, 
                                 const TBrowserStatusCode status);

/* confirm with the user with a dialog */
TBrowserStatusCode NW_ScrProxy_DlgConfirm(NW_ScrProxy_t *scrProxy,
                                  const NW_Ucs2 *message, 
                                  const NW_Ucs2 *yesMessage,
                                  const NW_Ucs2 *noMessage,
                                  void  *callbackCtx,
                                  NW_Dlg_ConfirmCB_t *callback);

/* prompt the user with a dialog */
TBrowserStatusCode NW_ScrProxy_DlgPrompt(NW_ScrProxy_t *scrProxy,
                                  const NW_Ucs2 *message,
                                  const NW_Ucs2 *defaultInput,
                                  void  *callbackCtx,
                                  NW_ScrProxy_DlgPromptCB_t *callback);

/* prompt the user with a generic dialog */
TBrowserStatusCode NW_ScrProxy_GenericDlgListSelect(NW_ScrProxy_t *scrProxy, NW_Dlg_ListSelect_t *data,
                                             void* callbackCtx, NW_Dlg_ListSelectCB_t* callback);


/* prompt the user with a generic dialog */
TBrowserStatusCode NW_ScrProxy_GenericDlgPrompt(NW_ScrProxy_t *scrProxy, NW_Dlg_Prompt_t *data,
                                         void* callbackCtx, NW_Dlg_PromptCB_t* callback);

/* prompt the user with a generic dialog */
TBrowserStatusCode NW_ScrProxy_GenericDlgInputPrompt(NW_ScrProxy_t *scrProxy, NW_Dlg_InputPrompt_t *data,
                                              void* callbackCtx, NW_Dlg_InputPromptCB_t* callback);


/* acknowledgement for a script exit */
TBrowserStatusCode NW_ScrProxy_GetVar(NW_ScrProxy_t *scrProxy,
                              const NW_Ucs2 *var, 
                              NW_Ucs2 **ret_string);

/* set a variable */
TBrowserStatusCode NW_ScrProxy_SetVar(NW_ScrProxy_t *scrProxy, 
                              const NW_Ucs2 *var, 
                              const NW_Ucs2 *value);

/* execute a wml prev task on the wml interpreter */
TBrowserStatusCode NW_ScrProxy_LoadPrev(NW_ScrProxy_t *scrProxy);

/* execute a newcontext on the wml interpreter */
TBrowserStatusCode NW_ScrProxy_NewContext(NW_ScrProxy_t *scrProxy);

/* refresh the wml interpreter */
TBrowserStatusCode NW_ScrProxy_Refresh(NW_ScrProxy_t *scrProxy);

/* execute a load url on the wml interpreter */
TBrowserStatusCode NW_ScrProxy_LoadUrl(NW_ScrProxy_t *scrProxy,
                                const NW_Ucs2 *url,
                                const NW_Ucs2 *param);

/* Query Browser app for Suspend/Resume of Script */
TBrowserStatusCode NW_ScrProxy_BrowerApp_QueryForSuspRes(NW_ScrProxy_t *scrProxy);
/* WTAI functions */

/* public library */
TBrowserStatusCode NW_ScrProxy_wp_makeCall(NW_ScrProxy_t *scrProxy, 
                                    NW_Ucs2 *number);
TBrowserStatusCode NW_ScrProxy_wp_sendDTMF(NW_ScrProxy_t *scrProxy, 
                                    NW_Ucs2 *dtmf);
TBrowserStatusCode NW_ScrProxy_wp_addPBEntry(NW_ScrProxy_t *scrProxy, 
                                    NW_Ucs2 *number,
                                      NW_Ucs2 *name);

/* not yet supported */

#if 0

/* voice call library */
TBrowserStatusCode NW_ScrProxy_wvc_setup     (NW_ScrProxy_t *scrProxy, 
                                      NW_Ucs2 *number, NW_Bool mode, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wvc_accept    (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, NW_Bool *mode, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wvc_release   (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wvc_sendDTMF  (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, NW_Ucs2 *dtmf, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wvc_callStatus(NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, NW_Ucs2 *field, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wvc_list      (NW_ScrProxy_t *scrProxy, 
                                      NW_Bool returnFirst, 
                                      NW_Opaque_t *result);


/* network text library */
TBrowserStatusCode NW_ScrProxy_wnt_send      (NW_ScrProxy_t *scrProxy, 
                                      NW_Ucs2 *address, NW_Ucs2 *text, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wnt_list      (NW_ScrProxy_t *scrProxy, 
                                      NW_Bool returnFirst, NW_Int32 type, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wnt_remove    (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wnt_getFieldValue (void *ctx, 
                                      NW_Opaque_t *msgHandle, NW_Ucs2 *field, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wnt_markAsRead(NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);


/* phone book library */
TBrowserStatusCode NW_ScrProxy_wpb_write     (NW_ScrProxy_t *scrProxy, 
                                      NW_Int32 index, NW_Ucs2 *number, NW_Ucs2 *name, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wpb_search    (NW_ScrProxy_t *scrProxy, 
                                      NW_Ucs2 *field, NW_Ucs2 *value, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wpb_remove    (NW_ScrProxy_t *scrProxy, 
                                      NW_Int32 index, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wpb_getFieldValue (void *ctx, 
                                      NW_Int32 index, NW_Ucs2 *field, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wpb_change    (NW_ScrProxy_t *scrProxy, 
                                      NW_Int32 index, NW_Ucs2 *field, NW_Ucs2 *value, 
                                      NW_Opaque_t *result);


/* call log library */
TBrowserStatusCode NW_ScrProxy_wcl_dialled   (NW_ScrProxy_t *scrProxy, 
                                      NW_Bool returnFirst, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wcl_missed    (NW_ScrProxy_t *scrProxy, 
                                      NW_Bool returnFirst, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wcl_received  (NW_ScrProxy_t *scrProxy, 
                                      NW_Bool returnFirst, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wcl_getFieldValue(NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *logHandle, NW_Ucs2 *field, 
                                      NW_Opaque_t *result);


/* miscellaneous library */
TBrowserStatusCode NW_ScrProxy_wm_setIndicator  (NW_ScrProxy_t *scrProxy, 
                                          NW_Int32 type, NW_Int32 newState, 
                                          NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wm_endcontext (NW_ScrProxy_t *scrProxy,
                                       NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wm_getProtection (NW_ScrProxy_t *scrProxy,  
                                          NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_wm_setProtection (NW_ScrProxy_t *scrProxy,  
                                          NW_Bool mode, 
                                          NW_Opaque_t *result);


/* is136 specific library */ 
TBrowserStatusCode NW_ScrProxy_is136_sendFlash  (NW_ScrProxy_t *scrProxy,  
                                          NW_Opaque_t *handle, NW_Ucs2 *flash, 
                                          NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_is136_sendAlert  (NW_ScrProxy_t *scrProxy,  
                                          NW_Opaque_t *handle, NW_Ucs2 *alert, 
                                          NW_Opaque_t *result);


/* gsm specific library */
TBrowserStatusCode NW_ScrProxy_gsm_hold      (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_retrieve  (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_transfer  (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *Bhandle, NW_Opaque_t *Chandle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_deflect   (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, NW_Ucs2 *number, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_multiparty(NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_separate  (NW_ScrProxy_t *scrProxy,
                                      NW_Opaque_t *handle,
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_sendUSSD  (NW_ScrProxy_t *scrProxy, 
                                      NW_Ucs2 *message, NW_Int32 codingScheme, 
                                      NW_Int32 type, NW_Int32 transactionID, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_gsm_netinfo   (NW_ScrProxy_t *scrProxy, 
                                      NW_Int32 type, 
                                      NW_Opaque_t *result);


/* pdc specific library */
TBrowserStatusCode NW_ScrProxy_pdc_hold      (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_pdc_retrieve  (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_pdc_transfer  (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, NW_Ucs2 *number, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_pdc_deflect   (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, NW_Ucs2 *dest, 
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_pdc_multiparty(NW_ScrProxy_t *scrProxy,
                                      NW_Opaque_t *result);

TBrowserStatusCode NW_ScrProxy_pdc_separate  (NW_ScrProxy_t *scrProxy, 
                                      NW_Opaque_t *handle, 
                                      NW_Opaque_t *result);


#endif /* if 0 */

/* Nokia proprietary */
TBrowserStatusCode NW_ScrProxy_nokia_locationInfo(NW_ScrProxy_t *scrProxy, 
                                           NW_Ucs2 *url,
                                           NW_Ucs2 *ll_format,
                                           NW_Ucs2 *pn_format);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */


#endif  /* WML_SCRPROXY_H */
