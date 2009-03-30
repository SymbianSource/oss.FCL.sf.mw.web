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


#ifndef _nwx_script_handler_h
#define _nwx_script_handler_h

#ifdef __cplusplus
extern "C" {
#endif

/* incoming messages */
TBrowserStatusCode NW_Script_Startreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Getvarresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Setvarresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Loadurlresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Loadprevresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Refreshresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Newcontextresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Exitreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Resumereq(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Dialogalertresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Notifyerrorresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Dialogconfirmresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Dialogpromptresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Urlloadresp(TBrowserStatusCode status, NW_Uint16 transId, NW_Int32 chunkIndex,
                                  void *loadContext, NW_Url_Resp_t *resp);
TBrowserStatusCode NW_Script_Goresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Finishnodialogresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Suspendresp(NW_Msg_t *msg);

/* outgoing messages */
TBrowserStatusCode NW_Script_Getvar(NW_Ucs2 *name);
TBrowserStatusCode NW_Script_Setvar(NW_Ucs2 *name, NW_Ucs2 *value);
TBrowserStatusCode NW_Script_Loadprev(void);
TBrowserStatusCode NW_Script_Refresh(void);
TBrowserStatusCode NW_Script_Newcontext(void);
TBrowserStatusCode NW_Script_Finish(void);
TBrowserStatusCode NW_Script_Dialogalert(const NW_Ucs2 *message);
TBrowserStatusCode NW_Script_Dialogconfirm(const NW_Ucs2 *message, const NW_Ucs2 *yesStr,
                                    const NW_Ucs2 *noStr);
TBrowserStatusCode NW_Script_Dialogprompt(const NW_Ucs2 *message, const NW_Ucs2 *defaultInput);
TBrowserStatusCode NW_Script_Load(NW_Ucs2 *url);
TBrowserStatusCode NW_Script_Go(NW_Ucs2 *url);
TBrowserStatusCode NW_Script_Notifyerror(TBrowserStatusCode status);
TBrowserStatusCode NW_Script_Finishnodialog(NW_Byte status, NW_Ucs2 *msg);
TBrowserStatusCode NW_Script_Suspend(void);

/* wtai declarations */

/* public library */
TBrowserStatusCode NW_Script_wp_makeCall(NW_Ucs2 *number);
TBrowserStatusCode NW_Script_wp_sendDTMF(NW_Ucs2 *dtmf);
TBrowserStatusCode NW_Script_wp_addPBEntry(NW_Ucs2 *number, NW_Ucs2 *name);

/* Nokia proprietary */
TBrowserStatusCode NW_Script_nokia_locationInfo(NW_Ucs2 *url, 
                                         NW_Ucs2 *ll_format,
                                         NW_Ucs2 *pn_format);

TBrowserStatusCode NW_Script_CryptoSignTextStart(NW_Ucs2 *stringToSign, NW_Int32 option, 
                                    NW_Int32 keyIdType, NW_Byte *keyIdByte, NW_Int32 keyIdLen);


TBrowserStatusCode NW_Script_wtaiResp(NW_Msg_t *msg);

#ifdef __cplusplus
} 
#endif /* extern "C" */

#endif /*_nwx_script_handler_h*/

/* END here  */
