/*
* Copyright (c) 1999 Nokia Corporation and/or its subsidiary(-ies).
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
    
  File name:  nwx_msg_config_win.c
  Part of: Messaging  
  Version: V1.0
  Description: 
  The NW_Msg_Startup routine is used to start the messaging system.
  It uses the NW_MSG_MAP macro defined in rb_msg_map.h to initialize
  an array of Message Handlers and Message Dispatchers.

******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_defs.h"
#include "nwx_ctx.h"
#include "nwx_msg_api.h"
#include "nwx_msg_types.h"
#include "nwx_http_header.h"
#include "wml_wae.h"
#include "nwx_mem.h"
#include "nwx_psq.h"
#include "nwx_msg_dsp_api_epoc32.h"
#include "nwx_msg_map_epoc32.h"
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

/* 
** TODO: This struct is duplicated in nwx_msg_epoc32.c, which is down
**       right evil.  To resolve this, it should be put in a shared-private 
**       header file.
*/
/*lint --e{754} local structure member not referenced */
typedef struct {
  NW_Ds_DynamicArray_t *receivers;      /* receivers know to this guy */
  NW_Uint16            nextRID;         /* next receiver ID */
  NW_Uint16            nextDID;         /* next dispatcher ID */
  NW_Msg_Dispatcher_t  **g_dispatchers; /* NW_Msg_Startup allocs ; NW_Msg_Shutdown frees */
} Msg_Context_t;


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

/* The single Windows WND proc dispatcher */
//extern NW_Msg_Dispatcher_t *g_wndproc_dsp;

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*********************************************************
**  Name:   NW_Msg_Startup
**  Description:  Rainbow Messaging startup. Must be called 
**                before any calls to the messaging system.
**  Parameters:   qName - Message queue name
**  Return Value: KBrsrSuccess or KBrsrOutOfMemory
**********************************************************/
TBrowserStatusCode NW_Msg_Startup(const char* qName) 
{
  Msg_Context_t *ctx = (Msg_Context_t *) NW_Ctx_Get(NW_CTX_MSG_API,0);
  NW_Uint16  numHandlers;
  NW_Uint16  numDispatchers;
  NW_Int16   i,k;
  NW_Msg_Address_t address;
  NW_Msg_Dispatcher_t  **g_dispatchers = NULL;
  TBrowserStatusCode status;

  /* The NW_MSG_MAP... macros will initialize the rb_map[] array of
     message handlers and message dispatchers */
  NW_Msg_MapEntry_t  rb_map[] = NW_MSG_MAP_CLI;

  numHandlers = NW_MSG_NUM_HANDLERS(rb_map);

  numDispatchers  = NW_MSG_NUM_DISPATCHERS;

  /* Create an array to hold all of the dispatchers */
  g_dispatchers = (NW_Msg_Dispatcher_t**) NW_Mem_Malloc(numDispatchers * sizeof(NW_Msg_Dispatcher_t *));
  /* Save pointer to dispatcher for Free'ing later in NW_Msg_Shutdown */
  ctx->g_dispatchers = g_dispatchers; 
  
  if (g_dispatchers == NULL)
    return KBrsrOutOfMemory;

  for (i = 0; i < numDispatchers; i++) {
    g_dispatchers[i] = NULL;
  }
  /* Loop thru the list of handlers and start message dispatchers as needed */
  for (i = 0; i < numHandlers; i++) 
  {
    /* If the dispatcher doesn't exist, start it */
    if (g_dispatchers[rb_map[i].threadIndex] == NULL) 
    {
      g_dispatchers[rb_map[i].threadIndex] = NW_Msg_NewDispatcher(rb_map[i].dspType, qName);
      if (g_dispatchers[rb_map[i].threadIndex] == NULL) {
        for (k = 0; k <= i; k++)
        {
          if (g_dispatchers[rb_map[k].threadIndex] != NULL)
          {
           if (g_dispatchers[rb_map[k].threadIndex]->queue != NULL)
            {
                NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]->queue->signal);
        // we can't cleanup mutex NW_Osu_DeleteMutex(), so it will be a memory leak in the low memory situation
                NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]->queue);
            }
            NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]);
            g_dispatchers[rb_map[k].threadIndex] = NULL;
          }
        }
        NW_Mem_Free(g_dispatchers);
        return KBrsrOutOfMemory;
      }
      //if (rb_map[i].dspType == NW_MSG_WNDPROC && g_wndproc_dsp == NULL)
      //g_wndproc_dsp = g_dispatchers[rb_map[i].threadIndex];
    }
    status = NW_Msg_DispatcherRegister(g_dispatchers[rb_map[i].threadIndex], &rb_map[i]);
    if (status == KBrsrOutOfMemory)
    {
        for (k = 0; k <= i; k++)
        {
          if (g_dispatchers[rb_map[k].threadIndex] != NULL)
          {
           if (g_dispatchers[rb_map[k].threadIndex]->queue != NULL)
            {
                NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]->queue->signal);
                NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]->queue);
            }
            NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]);
            g_dispatchers[rb_map[k].threadIndex] = NULL;
          }
        }
      NW_Mem_Free(g_dispatchers);
      return KBrsrOutOfMemory;
    }
  }

  /* Loop thru the list of handlers lookup the address of each reciever. Doing
     this now will make certain that all the receivers are known in the message
     api layer before the first message is sent. */
  for (i = 0; i < numHandlers; i++) {
    status = NW_Msg_LookupReceiverByName(rb_map[i].name, &address);
    if (status == KBrsrOutOfMemory)
    {
      // TO DO: call function which will cleanup handler NW_Msg_LookupReceiverByName()
      for (k = 0; k < i; k++)
      {
        if (g_dispatchers[rb_map[k].threadIndex] != NULL)
        {
          if (g_dispatchers[rb_map[k].threadIndex]->queue != NULL)
          {
                NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]->queue->signal);
                NW_Mem_Free(g_dispatchers[k]->queue);
          }
          NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]->handlers);
          NW_Mem_Free(g_dispatchers[rb_map[k].threadIndex]);
          g_dispatchers[rb_map[k].threadIndex] = NULL;
        }
      }
      NW_Mem_Free(g_dispatchers);
      return KBrsrOutOfMemory;
    }
  }

  return KBrsrSuccess;
}


/*********************************************************
**  Name:   NW_Msg_Shutdown
**  Description:  Rainbow Messaging Shutdown.
**                Free's all memory allocated in NW_Msg_Startup
**  Parameters:   none 
**  Return Value: none 
**********************************************************/
void NW_Msg_Shutdown() 
{
    Msg_Context_t *ctx = (Msg_Context_t *) NW_Ctx_Get(NW_CTX_MSG_API,0);
    NW_Uint16  count,i;
    NW_Msg_Dispatcher_t  **g_dispatchers = NULL;
    NW_Msg_Dispatcher_t  *dsp = NULL;
    NW_Msg_Handler_t *tmp,*tmp2;

    if (ctx == NULL)
      return;

    g_dispatchers = ctx->g_dispatchers; 
    if (g_dispatchers != NULL)
    {

      /* Loop thru the list of handlers and free message dispatchers */
      dsp = g_dispatchers[0];
    
      if (dsp && dsp->queue)
      {
          NW_Psq_Delete(dsp->queue);
      }
      if (dsp && dsp->handlers)
      {
        tmp = dsp->handlers;
        while (tmp->next != NULL)
        {
            tmp2 = (NW_Msg_Handler_t*) tmp->next;
            NW_Mem_Free(tmp); 
            tmp = tmp2;
        }
        NW_Mem_Free(tmp); 
      }

      NW_Mem_Free(g_dispatchers[0]);
      g_dispatchers[0] = NULL;
      NW_Mem_Free(g_dispatchers);
      g_dispatchers = NULL;

    }
    // Free up all the Message Receivers allocated in nwx_msg_epoc32.c
    count = NW_Ds_DarGetCount(ctx->receivers);
    for (i=0 ; i < count ; i++)
    {
        NW_Mem_Free(NW_Ds_DarGetElement(ctx->receivers, i));
    }
    NW_Ds_DarDelete(ctx->receivers);

    NW_Mem_Free(ctx);
    ctx = NULL;
    NW_Ctx_Set(NW_CTX_MSG_API,0, NULL);
}
