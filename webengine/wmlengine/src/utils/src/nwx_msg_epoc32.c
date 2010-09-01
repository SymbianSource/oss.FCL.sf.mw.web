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
#include "nwx_defs.h"
#include "nwx_ctx.h"
#include "nwx_msg_api.h"
#include "nwx_datastruct.h"
#include "nwx_psq.h"
#include "nwx_msg_types.h"
#include "nwx_msg_dsp_api_epoc32.h"
#include "nwx_logger.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  Function prototype to remove warning.
**-------------------------------------------------------------------------
*/

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

/* 
** TODO: This struct is duplicated in nwx_msg_config_epoc32.c, which is down
**       right evil.  To resolve this, it should be put in a shared-private 
**       header file.
*/
/*lint --e{754} local structure member not referenced */
typedef struct {
  NW_Ds_DynamicArray_t *receivers;       /* receivers know to this guy */
  NW_Uint16            nextRID;          /* next receiver ID */
  NW_Uint16            nextDID;          /* next dispatcher ID */
  NW_Msg_Dispatcher_t  **g_dispatchers;  /* NW_Msg_Startup allocs ; NW_Msg_Shutdown frees */
} Msg_Context_t;

typedef struct {
  NW_Uint32            name;         /* message receiver name */
  NW_Msg_Address_t     address;      /* the address of this receiver */
  NW_Msg_Dispatcher_t  *dispatcher;  /* how will handle the dispatching */
} Msg_Receiver_t;

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
static Msg_Context_t *Msg_GetThis(void);
static void Msg_SetThis(Msg_Context_t *ctx);
static NW_Uint16 Msg_GetNextRID(Msg_Context_t *ctx);
static Msg_Receiver_t *Msg_GetReceiver(const Msg_Context_t *ctx,
                                       const NW_Msg_Address_t addr);

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

static Msg_Context_t *Msg_GetThis()
{
  return (Msg_Context_t*) NW_Ctx_Get(NW_CTX_MSG_API,0);
}

static void Msg_SetThis(Msg_Context_t *ctx)
{
  NW_Ctx_Set(NW_CTX_MSG_API,0,ctx);

}

static NW_Uint16 Msg_GetNextRID(Msg_Context_t *ctx)
{
  return ctx->nextRID++;
}


/*********************************************************
**  Name:   Msg_GetReceiver
**  Description:  Return the receiver for a given address
**  Parameters:   *ctx - pointer to the context
**                address - message address to find
**  Return Value: the matching receiver or NULL
**********************************************************/
static Msg_Receiver_t *Msg_GetReceiver(const Msg_Context_t *ctx,
                                       const NW_Msg_Address_t addr)
{
  NW_Int16 loop, count;
  Msg_Receiver_t *receiver;

  NW_ASSERT(ctx != NULL);

  count = NW_Ds_DarGetCount(ctx->receivers);
  for (loop = 0; loop < count; loop++)  {
    receiver = (Msg_Receiver_t *)NW_Ds_DarGetElement(ctx->receivers, loop);
    if (receiver != NULL) {
      if (receiver->address == addr)
        return receiver;
    }
  }
  return NULL;
}


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
/*!
	\brief Creates a new messaging system. There will be only one of these <br>
	per process. Each messageing system will know about all of the receivers <br>
	registered within the process and all of the message dispatchers.
	\return The  results  are <br>
	KBrsrSuccess - success <br>
	KBrsrOutOfMemory - out of memory <br>
*/

/*********************************************************
**  Name:   NW_Msg_Initialize
**  Parameters:   qName : Name of msg queue
**********************************************************/
TBrowserStatusCode NW_Msg_Initialize(const char* qName)
  {
  Msg_Context_t *ctx;
  TBrowserStatusCode ret; 
  
  /* Get a pointer to the context. If it is null
  create a new one and store it */
  ctx = Msg_GetThis();
  if (ctx == NULL) 
    {
    ctx = (Msg_Context_t*) NW_Mem_Malloc(sizeof(Msg_Context_t));
    if (ctx == NULL)
      {
      Msg_SetThis(ctx);
      return KBrsrOutOfMemory;
      }
    ctx->g_dispatchers = NULL;
    ctx->nextRID = 1;
    ctx->receivers = NW_Ds_DarNew_DefaultHeap(3);
    if (ctx->receivers == NULL)
      {
      NW_Mem_Free(ctx);
      ctx = NULL;
      Msg_SetThis(ctx);
      return KBrsrOutOfMemory;
      }
    Msg_SetThis(ctx);
    }
  
  /* Initialize the Dispatching system also */
  ret = NW_Msg_DispatcherInitialize(qName);
  if (ret == KBrsrOutOfMemory)
    {
    NW_Ds_DarDelete(ctx->receivers);
    ctx->receivers = NULL;
    NW_Mem_Free(ctx);
    ctx =NULL;
    Msg_SetThis(ctx);
    return KBrsrOutOfMemory;
    }
  
  return KBrsrSuccess;
  }

/*********************************************************
**  Name:   NW_Msg_New
**  Description:  Allocate and initialize a new message.
**  Parameters:   tid - transaction id of this message
**                src - source address of the sender
**                dest - destination address
**                type - message type
**  Return Value: pointer to new message or NULL
**********************************************************/
NW_Msg_t *NW_Msg_New(const NW_Uint16 tid, const NW_Msg_Address_t src,
                              const NW_Msg_Address_t dest, const NW_Msg_Type_t type)
{
  NW_Msg_t *msg;

  msg = (NW_Msg_t*) NW_Mem_Malloc(sizeof (NW_Msg_t) );
  if (msg != NULL) {
    msg->next     = NULL;
    msg->dest     = dest;
    msg->src      = src;
    msg->type     = type;
    msg->tid      = tid;
    msg->status   = -1;
    msg->data     = NULL;
  }

  //NW_LOG5(NW_LOG_LEVEL3,
  //        "Msg New: type %d, src %d, dst %d, tid %d, ptr %x",
  //        msg->type, msg->src, msg->dest, msg->tid, msg);

  return msg;
}

/*********************************************************
**  Name:   NW_Msg_Delete
**  Description:  Deletes a message
**  Parameters:   *msg - pointer to the message
**  Return Value: void
**********************************************************/
void NW_Msg_Delete(NW_Msg_t *msg)
{
#ifdef _DEBUG
  NW_Uint32  loop;
  char *p = (char *)msg;
#endif

  if (msg != NULL) {
    NW_LOG6(NW_LOG_LEVEL3,
            "Msg delete: type %d, src %d, dst %d, tid %d, data %x, ptr %x",
            msg->type, msg->src, msg->dest, msg->tid, msg->data, msg);

#ifdef _DEBUG
    for (loop=0; loop< sizeof(NW_Msg_t); loop++, p++) {
      /*lint -e{569} Loss of information (8 bits to 7 bits)*/
      *p = (unsigned char)0xa1;
    }
#endif

    NW_Mem_Free(msg);
  }
  return;
}


/*********************************************************
**  Name:   NW_Msg_Send
**  Description:  Sends a message
**  Parameters:   *msg - pointer to the message
**  Return Value: KBrsrSuccess - if the receiver is found 
	                KBrsrFailure - if the receiver or Destination is not found
**********************************************************/

TBrowserStatusCode NW_Msg_Send(NW_Msg_t *msg)
{
  Msg_Context_t *ctx = Msg_GetThis();
  Msg_Receiver_t *receiver;

  NW_ASSERT(ctx != NULL);
  NW_ASSERT(msg != NULL);

  if (msg->dest < 1)
    return KBrsrFailure;

  /* check for receivers in this dispatcher */
  receiver = Msg_GetReceiver(ctx, msg->dest);
  if (receiver == NULL) {
    /* TODO Add logic for Receiver not in this dispatcher */
    return KBrsrFailure;
  }

  NW_LOG6(NW_LOG_LEVEL3,
          "sendMsg: type %d, src %d, dst %d, tid %d, data %x, ptr %x",
          msg->type, msg->src, msg->dest, msg->tid, msg->data, msg);

  /* Use the dispatcher to send the message */
  NW_Msg_DispatcherSend(receiver->dispatcher, msg);

  return KBrsrSuccess;
}


/*********************************************************
**  Name:   NW_Msg_LookupReceiverByName
**  Description:  Lookup address of receiver from name
**  Parameters:   name - name known by sender
**                *addr - address returned
**  Return Value: KBrsrSuccess or KBrsrUnexpectedError
**                or KBrsrOutOfMemory
**********************************************************/
TBrowserStatusCode NW_Msg_LookupReceiverByName(const NW_Msg_RName_t name,
                                                 NW_Msg_Address_t *addr)
{
  Msg_Context_t *ctx = Msg_GetThis();
  Msg_Receiver_t *receiver;
  NW_Msg_Dispatcher_t *dispatcher;
  NW_Uint16 loop, count;

  NW_ASSERT(ctx != NULL);
  NW_ASSERT(addr != NULL);

  /* Go through the list of receivers and find the one requested */
  count = NW_Ds_DarGetCount(ctx->receivers);
  for (loop = 0; loop < count; loop++)  {
    receiver = (Msg_Receiver_t *)NW_Ds_DarGetElement(ctx->receivers, loop);
    if (receiver != NULL) {
      if (name == receiver->name) {
        *addr = receiver->address;
        return KBrsrSuccess;
      }
    }
  }
  
  /* We haven't send this receiver yet, so go ask the Message
     Dispatching layer about this receiver */
     dispatcher =  NW_Msg_LookupDispatcherByName(name);
     if (dispatcher == NULL) {
      NW_ASSERT(dispatcher == NULL);
      return KBrsrUnexpectedError;
     }

  /* Create a new receiver record and add it */
  receiver = (Msg_Receiver_t*) NW_Mem_Malloc(sizeof(Msg_Receiver_t));
  if (receiver == NULL)
    return KBrsrOutOfMemory;
  receiver->dispatcher  = dispatcher;
  receiver->name        = name;
  receiver->address     = Msg_GetNextRID(ctx);
  if (NW_Ds_DarAppend(ctx->receivers, receiver) != KBrsrSuccess)
  {
    *addr = NULL;
    return KBrsrOutOfMemory;
  }

  /* Return the address */
  *addr = receiver->address;
  return KBrsrSuccess;
  
}

/*********************************************************
**  Name:   NW_Msg_GetThisReceiver
**  Description:  Return address of current receiver
**  Parameters:   *addr - address returned
**  Return Value: KBrsrSuccess or KBrsrUnexpectedError
**                or KBrsrFailure
**********************************************************/

TBrowserStatusCode NW_Msg_GetThisReceiver(NW_Msg_Address_t *addr)
{
  Msg_Context_t *ctx = Msg_GetThis();
  Msg_Receiver_t *receiver;
  NW_Msg_Dispatcher_t *dispatcher;
  NW_Uint16 loop, count;

  NW_ASSERT(ctx != NULL);
  NW_ASSERT(addr != NULL);

  /* First get the This dispatcher */
  dispatcher =  NW_Msg_GetThisDispatcher();
  if (dispatcher == NULL) {
    NW_ASSERT(dispatcher == NULL);
    return KBrsrUnexpectedError;
  }
  
  /* Go thru the list of receivers and find one that uses
     the dispatcher we just found*/
     count = NW_Ds_DarGetCount(ctx->receivers);
     for (loop = 0; loop < count; loop++)  {
      receiver = (Msg_Receiver_t *)NW_Ds_DarGetElement(ctx->receivers, loop);
      if (receiver != NULL) {
        if (dispatcher == receiver->dispatcher) {
          *addr = receiver->address;
          return KBrsrSuccess;
        }
      }
     }

     return KBrsrFailure;
}


/*********************************************************
**  Name:   NW_Msg_Reinit
**  Description:  Reinitializes a message's fields
**  Parameters:   *msg - pointer to the message
**                tid - transaction id of this message
**                dest - destination address
**                src - source address of the sender
**                type - message type
**                status - message status
**                *data - pointer to the message data
**  Return Value: void
**********************************************************/
void NW_Msg_Reinit(NW_Msg_t *msg, const NW_Uint16 tid, const NW_Msg_Address_t dest,
                            const NW_Msg_Address_t src, const NW_Msg_Type_t type,
                            const TBrowserStatusCode status, void *data)
{
  NW_ASSERT(msg != NULL);

  if (msg != NULL) {
    msg->tid    = tid;
    msg->dest   = dest;
    msg->src    = src;
    msg->type   = type;
    msg->status = status;
    msg->data   = data;
  }
  return;
}


/*********************************************************
**  Name:   NW_Msg_Reply
**  Description:  Send a message back to previous source
**  Parameters:   *msg - pointer to the message
**                type - message type
**  Return Value: status
**  Note: if status is not KBrsrSuccess, caller must
**        delete the message data, if any.
**********************************************************/
TBrowserStatusCode NW_Msg_Reply(NW_Msg_t *msg, const NW_Msg_Type_t type)
{
  TBrowserStatusCode status;
  NW_Msg_Address_t temp;

  NW_ASSERT(msg != NULL);

  /* Send this message back to the source address from this receiver address */
  temp      = msg->src;
  msg->src  = msg->dest;
  msg->dest = temp;
  msg->type = type;

  /* Note: Caller must delete the message data (if it is not NULL) if failure */
  status = NW_Msg_Send(msg);
  if (status != KBrsrSuccess) {
    NW_Msg_Delete(msg);
  }

  return status;
}


/*********************************************************
**  Name:   NW_Msg_Forward
**  Description:  Send a message on to another destination
**  Parameters:   *msg - pointer to the message
**                dest - destination address
**                type - message type
**  Return Value: status
**  Note: if status is not KBrsrSuccess, caller must
**        delete the message data, if any.
**********************************************************/
TBrowserStatusCode NW_Msg_Forward(NW_Msg_t *msg, const NW_Msg_Address_t dest, 
                                    const NW_Msg_Type_t type)
{
  TBrowserStatusCode status;

  NW_ASSERT(msg != NULL);

  /* Send this message on to a new destination address from this receiver address */
  msg->src  = msg->dest;
  msg->dest = dest;
  msg->type = type;

  /* Note: Caller must delete the message data (if it is not NULL) if failure */
  status = NW_Msg_Send(msg);
  if (status != KBrsrSuccess) {
    NW_Msg_Delete(msg);
  }

  return status;
}



