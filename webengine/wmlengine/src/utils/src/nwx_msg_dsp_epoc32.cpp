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


/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_defs_symbian.h"
#include "nwx_defs.h"
#include "nwx_ctx.h"
#include "nwx_psq.h"
#include "nwx_msg_api.h"
#include "nwx_datastruct.h"
#include "nwx_osu.h"
#include "nwx_logger.h"
#include "nwx_msg_dsp_api_epoc32.h"
#include "nwx_string.h"
#include "util_cdispatchereng.h"
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
typedef struct {
  CDispatcherEng* ipCDispatcherEng;
} NW_DspEng_Struct;

typedef NW_DspEng_Struct* NW_DspEng_t;

typedef struct {
  NW_Ds_DynamicArray_t *dispatchers;     /* dispatchers know to this guy */
  NW_Osu_Hwnd_t        hwnd;             /* Window handle for WndProc disp */
  NW_DspEng_t          DspEng;    /* Pointer to struct encapslate a C++ class */
} Msg_DspContext_Struct;

typedef Msg_DspContext_Struct Msg_DspContext_t;

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
static Msg_DspContext_t *MsgDsp_GetThis();
static void MsgDsp_SetThis(Msg_DspContext_t *cxt);
static TBrowserStatusCode NW_Msg_DispatcherStart(void *);

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

static Msg_DspContext_t *MsgDsp_GetThis()
{
  return (Msg_DspContext_t *)NW_Ctx_Get(NW_CTX_MSG_DSP,0);
}

static void MsgDsp_SetThis(Msg_DspContext_t *ctx)
{
	NW_Ctx_Set(NW_CTX_MSG_DSP, 0, ctx);
};



/*********************************************************
**  Name:   NW_Msg_DispatcherStart
**  Description:  Dispatcher start routine.
**  Parameters:   *data - message dispatcher to start
**  Return Value: nominally, KBrsrSuccess (never returns)
**********************************************************/
static TBrowserStatusCode NW_Msg_DispatcherStart(void *data)
{
  NW_Msg_Dispatcher_t *dsp  = (NW_Msg_Dispatcher_t *)data;
  Msg_DspContext_t* cxt;

  NW_ASSERT (dsp != NULL);
  cxt = MsgDsp_GetThis();
  if (cxt == NULL)
    return KBrsrOutOfMemory;

  if (!(cxt->DspEng->ipCDispatcherEng->IsDspContext()))
    cxt->DspEng->ipCDispatcherEng->SetDspContext(dsp);
  /* Signal that we have started */
  //NW_Osu_NotifySignal(dsp->signal);

  cxt->DspEng->ipCDispatcherEng->RequestEvent();
 
  return KBrsrSuccess;
}


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*********************************************************
**  Name:   NW_Msg_DispatcherInitialize
**  Description:  Creates a new messaging dispatching system.
**                There will be only one of these per process. 
**                It will know about all of the dispatchers in 
**                the process.
**  Parameters:   qName : Message queue name
**  Return Value: KBrsrSuccess or KBrsrOutOfMemory
**********************************************************/
TBrowserStatusCode NW_Msg_DispatcherInitialize(const char* qName)
{
  Msg_DspContext_t *ctx;
  
  /* Get a pointer to the context. If it is null
     create a new one and store it */
  ctx = MsgDsp_GetThis();
  if (ctx == NULL) 
  {
    ctx = (Msg_DspContext_t *)NW_Mem_Malloc(sizeof(Msg_DspContext_t));
    if (ctx == NULL)
    {
      return KBrsrOutOfMemory;
    }
    ctx->DspEng = (NW_DspEng_Struct*)NW_Mem_Malloc(sizeof(NW_DspEng_Struct));
    if (ctx->DspEng == NULL)
    {
      NW_Mem_Free(ctx);
      MsgDsp_SetThis(NULL);
      return KBrsrOutOfMemory;
    }

    ctx->DspEng->ipCDispatcherEng = new CDispatcherEng(CActive::EPriorityUserInput);
    if (ctx->DspEng->ipCDispatcherEng == NULL)
    {
      NW_Mem_Free(ctx->DspEng);
      ctx->DspEng = NULL;
      NW_Mem_Free(ctx);
      MsgDsp_SetThis(NULL);
      return KBrsrOutOfMemory;
    }

    ctx->dispatchers = NW_Ds_DarNew_DefaultHeap(3);
    if (ctx->dispatchers == NULL) 
    {
      delete ctx->DspEng->ipCDispatcherEng;
      NW_Mem_Free(ctx->DspEng);
      ctx->DspEng = NULL;
      NW_Mem_Free(ctx);
      MsgDsp_SetThis(NULL);
      return KBrsrOutOfMemory;
    }
    ctx->hwnd = NULL;
    MsgDsp_SetThis(ctx);

    /* Now call the startup routine */
    TBrowserStatusCode status = NW_Msg_Startup(qName) ;
    if (status == KBrsrOutOfMemory)
    {
      NW_Ds_DarDelete (ctx->dispatchers);
      ctx->dispatchers = NULL;
      delete ctx->DspEng->ipCDispatcherEng;
      NW_Mem_Free(ctx->DspEng);
      ctx->DspEng = NULL;
      NW_Mem_Free(ctx);
      MsgDsp_SetThis(NULL);
      return KBrsrOutOfMemory;
    }
  }

  return KBrsrSuccess;
}

/*********************************************************
**  Name:   NW_Msg_NewDispatcher
**  Description:  Creates and initializes a new message dispatcher
**  Parameters:   type - dispatcher type
**  Return Value: pointer to new dispatcher or NULL
**********************************************************/
NW_Msg_Dispatcher_t *NW_Msg_NewDispatcher(const NW_Msg_DispatcherType_t type,
                                          const char* qName)
{
  Msg_DspContext_t *ctx = MsgDsp_GetThis();
  NW_Msg_Dispatcher_t *tmp;

  NW_ASSERT (ctx != NULL);
  NW_ASSERT (type == NW_MSG_PSQ_DSP || type == NW_MSG_NATIVE_DSP);

  /* Allocate a dispatcher structure */
  tmp = (NW_Msg_Dispatcher_t *)NW_Mem_Malloc(sizeof(NW_Msg_Dispatcher_t));
  if (tmp != NULL) {
    tmp->type     = type;
    tmp->handlers = NULL;
    tmp->hwnd     = NULL;
    tmp->tmofunc  = NULL;
    tmp->wait     = NW_WAIT_FOREVER;

    /* Not used in EPOC implementation */
    tmp->signal   = NULL;

    switch (type) {
     case NW_MSG_PSQ_DSP:
      tmp->queue = NW_Psq_New(qName);
      if (tmp->queue != NULL) {
          NW_Msg_DispatcherStart(tmp);
          tmp->thread = NW_Osu_GetCurrentThreadId();
      }
      else {
        NW_Mem_Free(tmp);
        return NULL;
      }
      break;
     case NW_MSG_NATIVE_DSP:
      /* If we are using a WndProc dispatcher, just save the thread for */
      /* later use in GetThisDispatcher calls */
      tmp->thread = NW_Osu_GetCurrentThreadId();
      tmp->hwnd = ctx->hwnd;
      break;
     default:
      NW_Mem_Free(tmp);
      return NULL;
    }

    /* Add the new dispatcher to the context */
    if (NW_Ds_DarAppend(ctx->dispatchers, tmp) != KBrsrSuccess) {
      NW_Mem_Free(tmp);
      return NULL;
    }
  }

  return tmp;
}

/*********************************************************
**  Name:   NW_Msg_DispatcherRegister
**  Description:  Registers a message handler with a dispatcher.
**  Parameters:   *dsp - pointer to the dispatcher
**                *entry - pointer to the message map entry
**  Return Value: KBrsrSuccess or KBrsrOutOfMemory
**********************************************************/
TBrowserStatusCode NW_Msg_DispatcherRegister(NW_Msg_Dispatcher_t *dsp,
                                      const NW_Msg_MapEntry_t *entry)
{
  NW_Msg_Handler_t *handler;
  NW_Msg_Handler_t *tmp;

  NW_ASSERT (dsp != NULL);
  NW_ASSERT (entry != NULL);

  /* Create a handler structure */
  handler = (NW_Msg_Handler_t*)NW_Mem_Malloc(sizeof(NW_Msg_Handler_t));
  if (handler == NULL)
    return KBrsrOutOfMemory;

  handler->next = NULL;
  handler->type = entry->msgType;
  handler->name = entry->name;
  handler->func = entry->func;

  /* If the first one, just set it */
  if (dsp->handlers == NULL) {
    dsp->handlers = handler;
  } else {
    /* locate the last one in the last */
    tmp = dsp->handlers;
    while (tmp->next != NULL)
      tmp = (NW_Msg_Handler_t *)tmp->next;
    tmp->next = handler;
  }
  return KBrsrSuccess;
}

/*********************************************************
**  Name:   NW_Msg_DispatcherSend
**  Description:  Sends a message to a dispatcher.
**  Parameters:   *dsp - pointer to the dispatcher
**                *msg - pointer to the message
**  Return Value: KBrsrSuccess or KBrsrUnexpectedError
**********************************************************/
TBrowserStatusCode NW_Msg_DispatcherSend(NW_Msg_Dispatcher_t *dsp, NW_Msg_t *msg)
{
  Msg_DspContext_t *ctx = MsgDsp_GetThis();

  if (ctx == NULL)
    return KBrsrOutOfMemory;

  NW_ASSERT (dsp != NULL);
  NW_ASSERT (msg != NULL);
  NW_ASSERT (dsp->type == NW_MSG_PSQ_DSP || dsp->type == NW_MSG_NATIVE_DSP);

  switch (dsp->type) {
   case NW_MSG_PSQ_DSP:
    NW_Psq_AddTail(dsp->queue, (NW_Node_t *)msg);
    ctx->DspEng->ipCDispatcherEng->RequestEvent();
    break;
   case NW_MSG_NATIVE_DSP:
    break;
   default:
    return KBrsrUnexpectedError;
  }

  return KBrsrSuccess;
}

/*********************************************************
**  Name:   NW_Msg_LookupDispatcherByName
**  Description:  Lookup the message dispatcher from the name.
**  Parameters:   name - name known to sender
**  Return Value: pointer to the dispatcher or NULL
**********************************************************/
NW_Msg_Dispatcher_t *NW_Msg_LookupDispatcherByName(const NW_Msg_RName_t name)
{
  Msg_DspContext_t *ctx = MsgDsp_GetThis();
  NW_Msg_Handler_t *handler;
  NW_Msg_Dispatcher_t *dsp;
  NW_Int16 i, j;

  /* Go through the list of dispatchers and find the one requested */
  if (ctx != NULL) {
    j = NW_Ds_DarGetCount(ctx->dispatchers);
    for (i = 0; i < j; i++)  {
      dsp = (NW_Msg_Dispatcher_t *)NW_Ds_DarGetElement(ctx->dispatchers, i);
      handler = dsp->handlers;
      while (handler != NULL) {
        if (handler->name == name)
          return dsp;
        else
          handler = (NW_Msg_Handler_t *)handler->next;
      }
    }
  }

  return NULL;
}

/*********************************************************
**  Name:   NW_Msg_GetThisDispatcher
**  Description:  Get the message dispatcher for current thread.
**  Parameters:   none
**  Return Value: pointer to the dispatcher or NULL
**********************************************************/
NW_Msg_Dispatcher_t *NW_Msg_GetThisDispatcher()
{
  Msg_DspContext_t *ctx = MsgDsp_GetThis();
  NW_Osu_ThreadId_t thread = NW_Osu_GetCurrentThreadId();
  NW_Msg_Dispatcher_t *dsp;
  NW_Uint16 i, j;

  /* Go through the list of dispatchers and find the one requested */
  if (ctx != NULL) {
    j = NW_Ds_DarGetCount(ctx->dispatchers);
    for (i = 0; i < j; i++)  {
      dsp = (NW_Msg_Dispatcher_t *)NW_Ds_DarGetElement(ctx->dispatchers, i);
      if (dsp->thread == thread)
        return dsp;
    }
  }

  return NULL;
}

/*********************************************************
**  Name:   NW_Msg_NYI
**  Description:  Default message handler for unimplemented types.
**  Parameters:   *msg - pointer to the message
**  Return Value: KBrsrSuccess
**********************************************************/
TBrowserStatusCode NW_Msg_NYI(NW_Msg_t *msg)
{
  NW_REQUIRED_PARAM(msg);
  NW_ASSERT (msg != NULL);

  NW_LOG1(NW_LOG_LEVEL5, "NYI Message Handler %d\n", msg->type);

  return KBrsrSuccess;
}

/*********************************************************
**  Name:   NW_Msg_DispatcherShutdown()
**  Description:  Rainbow Message Dispatcher Shutdown.
**                Free's all memory allocated in NW_Msg_DispatcherInitialize()
**  Parameters:   none 
**  Return Value: none 
**********************************************************/
void NW_Msg_DispatcherShutdown() 
{
    // Free up Dispatching System from NW_Msg_DispatcherInitialize()
    Msg_DspContext_t *dsp_ctx = MsgDsp_GetThis();
    if (dsp_ctx == NULL)
      return;

    dsp_ctx->DspEng->ipCDispatcherEng->Cancel();
    delete dsp_ctx->DspEng->ipCDispatcherEng;
    NW_Mem_Free(dsp_ctx->DspEng);
    NW_Ds_DarDelete(dsp_ctx->dispatchers);
    NW_Mem_Free(dsp_ctx);
    dsp_ctx = NULL;
    NW_Ctx_Set(NW_CTX_MSG_DSP,0, NULL);
}
