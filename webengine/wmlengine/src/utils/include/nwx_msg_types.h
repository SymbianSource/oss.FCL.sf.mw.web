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

#ifndef NWX_MSG_TYPES_H
#define NWX_MSG_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define NW_WPSA_SERVER   1
#define NW_HTTP_LOADER   2
#define NW_FILE_LOADER   3
#define NW_USER_AGENT    4
#define NW_SCRIPT_SERVER 5
#define NW_BROWSER_CORE  6
#define NW_WTAI_LOADER   7
#define NW_MAILTO_LOADER 8

/* These #defines describe all of the values that can appear
** in the type message field
*/

/* USER Agent Messages */
#define NW_MSG_UA_FINISHNODLG_REQ             120
#define NW_MSG_UA_FINISHNODLG_RESP            121
#define NW_MSG_UA_USERPASSWORD_REQ            122
#define NW_MSG_UA_USERPASSWORD_RESP           123
#define NW_MSG_UA_DLGALERT_RESP               124  
#define NW_MSG_UA_DLGALERT_REQ                125
#define NW_MSG_UA_DLGCONFIRM_RESP             126  
#define NW_MSG_UA_DLGCONFIRM_REQ              127
#define NW_MSG_UA_DLGPROMPT_RESP              128  
#define NW_MSG_UA_DLGPROMPT_REQ               129
#define NW_MSG_UA_NOTIFYERROR_REQ             130
#define NW_MSG_UA_DIALOGLISTSELECT_REQ        131
#define NW_MSG_UA_DIALOGLISTSELECT_RESP       132
#define NW_MSG_UA_DIALOGPROMPT_REQ            133
#define NW_MSG_UA_DIALOGPROMPT_RESP           134
#define NW_MSG_UA_DIALOGINPUTPROMPT_REQ       135
#define NW_MSG_UA_DIALOGINPUTPROMPT_RESP      136
#define NW_MSG_UA_WIMIINIT_RESP               137
#define NW_MSG_UA_DISPLAYERROR_REQ            138
#define NW_MSG_UA_USERREDIRECT_REQ            139
#define NW_MSG_UA_USERREDIRECT_RESP           140
#define NW_MSG_UA_NOTIFYERROR_RESP            141

/* WML Browser Messages */
#define NW_MSG_COREBROWSER_GETVAR_REQ         200
#define NW_MSG_COREBROWSER_GETVAR_RESP        201
#define NW_MSG_COREBROWSER_SETVAR_REQ         202
#define NW_MSG_COREBROWSER_SETVAR_RESP        203
#define NW_MSG_COREBROWSER_LOADURL_REQ        204
#define NW_MSG_COREBROWSER_LOADURL_RESP       205
#define NW_MSG_COREBROWSER_LOADPREV_REQ       206
#define NW_MSG_COREBROWSER_LOADPREV_RESP      207
#define NW_MSG_COREBROWSER_NEWCONTEXT_REQ     208
#define NW_MSG_COREBROWSER_NEWCONTEXT_RESP    209
#define NW_MSG_COREBROWSER_GO_REQ             210
#define NW_MSG_COREBROWSER_GO_RESP            211
#define NW_MSG_COREBROWSER_REFRESH_REQ        212
#define NW_MSG_COREBROWSER_REFRESH_RESP       213

/* Script Messages */
#define NW_MSG_SCRIPT_START_REQ               300
#define NW_MSG_SCRIPT_START_RESP              301
#define NW_MSG_SCRIPT_EXIT_REQ                302
#define NW_MSG_SCRIPT_EXIT_RESP               303
#define NW_MSG_SCRIPT_RESUME_REQ              304
#define NW_MSG_SCRIPT_SUSPEND_REQ             305
#define NW_MSG_SCRIPT_SUSPEND_RESP            306

/* Tempest Messages */
#define NW_MSG_MSG_DSP_HANDLE_MESSAGE         500

/* Script WTAI Messages */
#define NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_REQ  743

#define NW_MSG_SCRIPT_WP_MAKECALL_RESP        750
#define NW_MSG_SCRIPT_WP_SENDDTMF_RESP        751
#define NW_MSG_SCRIPT_WP_ADDPBENTRY_RESP      752

#define NW_MSG_SCRIPT_NOKIA_LOCATIONINFO_RESP 793

#define NW_MSG_WIMI_HASHEDCERT_RESP           907

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_MSG_TYPES_H */
