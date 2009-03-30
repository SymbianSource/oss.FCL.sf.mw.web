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
* Description:  Defines the interface to the messaging dispatching.
*
*/


#ifndef nwx_msg_dsp_h
#define nwx_msg_dsp_h

#ifdef __cplusplus
extern "C" {
#endif
/*
** Type Definitions
*/ 

/******************** Messaging Data Types **********************************/
typedef enum {
	NW_MSG_PSQ_DSP,
	NW_MSG_NATIVE_DSP
} NW_Msg_DispatcherType_t;

#define NW_WNDPROC_MESSAGE (WM_USER + 100)
#define NW_DISPATCHER_TIMER_ID 1200

typedef TBrowserStatusCode NW_Msg_HandlerFunc_t(NW_Msg_t *);

typedef struct NW_Msg_Handler_s {
  struct NW_Msg_Handler_s     *next;      /* put this in a list */
  NW_Msg_Type_t               type;       /* message type */
  NW_Msg_RName_t              name;       /* Receiver Name */
  NW_Msg_HandlerFunc_t        *func;      /* function to handle the message */
} NW_Msg_Handler_t;

typedef struct {
  NW_Msg_DispatcherType_t     type;
  NW_Psq_t                    *queue;     /* queue pointer */
  NW_Osu_Hwnd_t               hwnd;       /* windows handle for WndProc */
  NW_Osu_Signal_t             signal;     /* a signal pointer */
  NW_Msg_Handler_t            *handlers;  /* message handler list */
  NW_Osu_ThreadId_t           thread;     /* Thread the dispatcher will run in */
  NW_Msg_HandlerFunc_t        *tmofunc;   /* The timeout function */
  NW_Int32                    wait;       /* How long to wait for a timeout */
} NW_Msg_Dispatcher_t;

typedef struct {
  NW_Uint16                   threadIndex;/* Which thread to run in */
  NW_Msg_DispatcherType_t     dspType;    /* Type of the dispatcher */
  NW_Msg_RName_t              name;       /* Receiver name */    
  NW_Msg_Type_t               msgType;    /* message type */
  NW_Msg_HandlerFunc_t        *func;      /* message handler */
} NW_Msg_MapEntry_t;



/*
** Global Function Declarations
*/

/******************************* Message Dispatcher Functions ***********************/
/* 1-time initialization function */
TBrowserStatusCode NW_Msg_DispatcherInitialize(const char* qName);
/* create a new message dispatcher */
NW_Msg_Dispatcher_t *NW_Msg_NewDispatcher(const NW_Msg_DispatcherType_t,
                                          const char* qName);
/* register a message map entry with a dispatcher */
TBrowserStatusCode NW_Msg_DispatcherRegister(NW_Msg_Dispatcher_t *dsp, 
                                        const NW_Msg_MapEntry_t *entry);
/* send a message to a dispatcher */
TBrowserStatusCode NW_Msg_DispatcherSend(NW_Msg_Dispatcher_t *dsp, NW_Msg_t *msg);
/* return dispatcher with specified name */
NW_Msg_Dispatcher_t *NW_Msg_LookupDispatcherByName(const NW_Msg_RName_t name);
/* return dispatcher for this thread */
NW_Msg_Dispatcher_t *NW_Msg_GetThisDispatcher();
/* Not Yet Implemented message handler */
TBrowserStatusCode NW_Msg_NYI(NW_Msg_t *msg);
/* start the messaging system (defined in nwx_msg_config_win.c) */
TBrowserStatusCode NW_Msg_Startup(const char *qName);

/* Free all resources of the messaging system */
void NW_Msg_Shutdown(); 
void NW_Msg_DispatcherShutdown();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* nwx_msg_dsp_h */
