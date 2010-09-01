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
* Description:  
*
*/


#ifndef NWX_MSG_MAP_EPOC32_H
#define NWX_MSG_MAP_EPOC32_H


/*
**----------------------------------------------------------------------
** Preprocessor Macro Definitions
**----------------------------------------------------------------------
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"
#include "nwx_script_handler.h"
#include "BrsrStatusCodes.h"

/*************************************************************************
    Function Prototypes for all of the message handlers appear here
*************************************************************************/

/*TBrowserStatusCode NW_Msg_NYI(NW_Msg_t *msg);*/

/*TBrowserStatusCode NW_Script_Notifyerrorresp(NW_Msg_t *msg);*/
TBrowserStatusCode NW_Script_Genericdialoglistselectresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Genericdialogpromptresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Script_Genericdialoginputpromptresp(NW_Msg_t *msg);

/* RME messages */
TBrowserStatusCode NW_Msg_MessageDispatcher_MessageHandler (NW_Msg_t* msg);

/* Script messages for WTA */
TBrowserStatusCode NW_Wml_nokia_locationInfoReq(NW_Msg_t *msg);
TBrowserStatusCode NW_UiRequestCallback(NW_Msg_t *msg);
 
/* WML messages */
TBrowserStatusCode NW_Wml_Scriptexitresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Scriptstartresp(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Getvarreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Setvarreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Loadprevreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Newcontextreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Goreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Refreshreq(NW_Msg_t *msg);

TBrowserStatusCode NW_Wml_Dialogalertreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Notifyerrorreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Dialogconfirmreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Dialogpromptreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Finishnodialogreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Genericdialoglistselectreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Genericdialogpromptreq(NW_Msg_t *msg);
TBrowserStatusCode NW_Wml_Genericdialoginputpromptreq(NW_Msg_t *msg);


#define NW_MSG_NUM_DISPATCHERS 1

/*      message-handler     dispatcher, msg-pump,           component */
#define FILE_LOADER_HDLR    0,          NW_MSG_PSQ_DSP,     NW_FILE_LOADER
#define USER_AGENT_HDLR     0,          NW_MSG_PSQ_DSP,     NW_USER_AGENT
#define CACHE_SERVER_HDLR   0,          NW_MSG_PSQ_DSP,     NW_CACHE_SERVER
#define HTTP_LOADER_HDLR    0,          NW_MSG_PSQ_DSP,     NW_HTTP_LOADER
#define BROWSER_CORE_HDLR   0,          NW_MSG_PSQ_DSP,     NW_BROWSER_CORE
#define SCRIPT_SERVER_HDLR  0,          NW_MSG_PSQ_DSP,     NW_SCRIPT_SERVER
#define WTAI_LOADER_HDLR    0,          NW_MSG_PSQ_DSP,     NW_WTAI_LOADER



/* Define how many message handlers */
#define NW_MSG_NUM_HANDLERS(msg_map) (sizeof(msg_map)/sizeof(msg_map[0]))

/*  message-handler       message-type                        handler-func */
#define NW_MSG_MAP_CLI { \
    {BROWSER_CORE_HDLR,    NW_MSG_MSG_DSP_HANDLE_MESSAGE,       NW_Msg_MessageDispatcher_MessageHandler},  \
    {BROWSER_CORE_HDLR,   NW_MSG_SCRIPT_START_RESP,             NW_Wml_Scriptstartresp}, \
    {BROWSER_CORE_HDLR,   NW_MSG_SCRIPT_EXIT_RESP,              NW_Wml_Scriptexitresp}, \
    {BROWSER_CORE_HDLR,   NW_MSG_COREBROWSER_GETVAR_REQ,        NW_Wml_Getvarreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_COREBROWSER_SETVAR_REQ,        NW_Wml_Setvarreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_COREBROWSER_LOADPREV_REQ,      NW_Wml_Loadprevreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_COREBROWSER_NEWCONTEXT_REQ,    NW_Wml_Newcontextreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_COREBROWSER_GO_REQ,            NW_Wml_Goreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_COREBROWSER_REFRESH_REQ,       NW_Wml_Refreshreq}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_START_REQ,              NW_Script_Startreq}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_EXIT_REQ,               NW_Script_Exitreq}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_COREBROWSER_GETVAR_RESP,       NW_Script_Getvarresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_COREBROWSER_SETVAR_RESP,       NW_Script_Setvarresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_COREBROWSER_LOADPREV_RESP,     NW_Script_Loadprevresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_COREBROWSER_NEWCONTEXT_RESP,   NW_Script_Newcontextresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_COREBROWSER_GO_RESP,           NW_Script_Goresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_COREBROWSER_REFRESH_RESP,      NW_Script_Refreshresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_RESUME_REQ,             NW_Script_Resumereq}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_UA_DLGALERT_RESP,              NW_Script_Dialogalertresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_UA_DLGPROMPT_RESP,             NW_Script_Dialogpromptresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_UA_DLGCONFIRM_RESP,            NW_Script_Dialogconfirmresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_UA_FINISHNODLG_RESP,           NW_Script_Finishnodialogresp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_SUSPEND_REQ,            NW_Script_Suspendresp}, \
    {BROWSER_CORE_HDLR,   NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_REQ, NW_Wml_nokia_locationInfoReq}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_WP_MAKECALL_RESP,       NW_Script_wtaiResp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_WP_SENDDTMF_RESP,       NW_Script_wtaiResp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_WP_ADDPBENTRY_RESP,     NW_Script_wtaiResp}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_RESP,NW_Script_wtaiResp}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_DLGALERT_REQ,               NW_Wml_Dialogalertreq}, \
    {SCRIPT_SERVER_HDLR,  NW_MSG_UA_NOTIFYERROR_RESP,           NW_Script_Notifyerrorresp}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_NOTIFYERROR_REQ,            NW_Wml_Notifyerrorreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_DLGCONFIRM_REQ,             NW_Wml_Dialogconfirmreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_DLGPROMPT_REQ,              NW_Wml_Dialogpromptreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_FINISHNODLG_REQ,            NW_Wml_Finishnodialogreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_DIALOGLISTSELECT_REQ,       NW_Wml_Genericdialoglistselectreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_DIALOGPROMPT_REQ,           NW_Wml_Genericdialogpromptreq}, \
    {BROWSER_CORE_HDLR,   NW_MSG_UA_DIALOGINPUTPROMPT_REQ,      NW_Wml_Genericdialoginputpromptreq}, \
    \
    }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* nwx_msg_map_h */
