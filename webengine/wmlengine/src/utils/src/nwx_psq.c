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
    
  File name:  nwx_psq.c
  Part of: Protected Signaled Queue 
  Version: V1.0
  Description: 
    Provides interfaces to a Protected, Signaled, Queue. This is a queue that
  can be have multiple readers and writers running in separate threads.
  When something is added to the queue a signal is set.

******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_psq.h"
#include "nwx_string.h"
#include "nwx_logger.h"
#include "nwx_datastruct.h"
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

/*****************************************************************
**  Name:   NW_Psq_New
**  Description:  Creates a new Psq object.
**  Parameters:   *name - name of queue for debugging  
**  Return Value: a pointer to the new Psq object or NULL
******************************************************************/
NW_Psq_t *NW_Psq_New(const char *name)
{
  NW_Psq_t *que;

  NW_ASSERT(name != NULL);

  que = (NW_Psq_t*) NW_Mem_Malloc(sizeof(NW_Psq_t));
  if (que != NULL) {
    que->head = NULL;
    que->tail = NULL;
    que->length = 0;
    que->signal = (NW_Osu_Signal_t*) NW_Osu_NewSignal(name);
    if (que->signal == NULL)
    {
      NW_Mem_Free(que);
      return NULL;
    }
    que->mutex  = (NW_Osu_Mutex_t*) NW_Osu_NewMutex(name);
    if (que->mutex == NULL)
    {
      NW_Osu_DeleteSignal(que->signal);
      NW_Mem_Free(que);
      return NULL;
    }
    que->name   = NW_Str_CvtFromAscii(name);
    if (que->name == NULL) {
      NW_Osu_DeleteSignal(que->signal);
      NW_Osu_DeleteMutex(que->mutex);
      NW_Mem_Free(que);
      return NULL;
    }
  }
  return que;
}

/*****************************************************************
**  Name:   NW_Psq_Delete
**  Description:  Delete a Psq object
**  Parameters:   *que - a pointer to the queue 
**  Return Value: void
**  Note: caller *must* insure that both signal and mutex
**        are no longer in use prior to deleting the Psq object.
******************************************************************/
void NW_Psq_Delete(NW_Psq_t *que)
{
  NW_ASSERT(que != NULL);

  if (que->signal != NULL)
    NW_Osu_DeleteSignal(que->signal);
  if (que->mutex != NULL)
    NW_Osu_DeleteMutex(que->mutex);
  if (que->name != NULL)
    NW_Str_Delete(que->name);
  NW_Mem_Free(que);

  return;
}

/*****************************************************************
**  Name:   NW_Psq_AddTail
**  Description:  Add a new node to the tail of the queue
**  Parameters:   *que - a pointer to the queue 
**                *node - pointer to the new node 
**  Return Value: void
******************************************************************/
void NW_Psq_AddTail(NW_Psq_t *que, NW_Node_t *node)
{
  NW_ASSERT(que != NULL);
  NW_ASSERT(node != NULL);
  NW_LOG2(NW_LOG_LEVEL5, "addTail on queue %s length %d\n",
          que->name, que->length);

  /* lock the mutex and wait as long as needed */
  NW_Osu_LockMutex(que->mutex);

  /* do the add */
  if (que->head == NULL) {
    que->head = node;
  } else {
    que->tail->next = node;
  }
  que->tail = node;
  que->tail->next = NULL;
  que->length++;

  /* unlock the mutex and set signal */
  NW_Osu_UnlockMutex(que->mutex);
  NW_Osu_NotifySignal(que->signal);
  return;
}

/*****************************************************************
**  Name:   NW_Psq_RemoveHead
**  Description:  Return node from the head of the queue and remove it
**  Parameters:   *que - a pointer to the queue 
**  Return Value: pointer to removed node
******************************************************************/
NW_Node_t *NW_Psq_RemoveHead(NW_Psq_t *que)
{
  NW_Node_t *tmp=NULL;

  NW_ASSERT(que != NULL);
  NW_LOG2(NW_LOG_LEVEL5, "removeHead on queue %s length %d\n",
          que->name, que->length);

  /* lock the mutex and wait as long as needed */
  NW_Osu_LockMutex(que->mutex);

  /* do the remove */
  if (que->head != NULL) {
    tmp = que->head;
    que->head = tmp->next;
    if (que->head == NULL)
      que->tail = NULL;
    que->length--;
  }

  /* unlock the mutex */
  NW_Osu_UnlockMutex(que->mutex);

  /* return the node */
  return tmp;
}

/*****************************************************************
**  Name:   NW_Psq_WaitForAdd
**  Description:  Wait for a node to be added to the queue
**  Parameters:   *que - a pointer to the queue
**                howLong - number mSec to wait or NW_WAIT_FOREVER
**  Return Value: KBrsrSuccess if node is queued, else KBrsrWaitTimeout
******************************************************************/
TBrowserStatusCode NW_Psq_WaitForAdd(const NW_Psq_t *que, const NW_Int32 howLong)
{
  /* NW_LOG1(NW_LOG_ALL, "wait on queue %s\n", que->name); */
  return NW_Osu_WaitOnSignal(que->signal, howLong);
}
