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

  Subsystem Name: ProtectedSignaledQueue
  Version: V1.0
  Description:    
    Provides interfaces to a Protected, Signaled, Queue. This is a queue that
  can be have multiple readers and writers running in separate threads.
  When something is added to the queue a signal is set.
******************************************************************/
#ifndef NWX_PSQ_H
#define NWX_PSQ_H

#ifdef __cplusplus
extern "C" {
#endif


/*
** Includes
*/

#include "nwx_defs.h"
#include "nwx_datastruct.h"
#include "nwx_osu.h"
#include "BrsrStatusCodes.h"

/*
** Type Definitions
*/ 

typedef struct {
  NW_Node_t       *head;    /* queue head */
  NW_Node_t       *tail;    /* queue tail */
  NW_Osu_Signal_t *signal;  /* signal this when adding */
  NW_Osu_Mutex_t  *mutex;   /* mutex for locking and unlocking */
  NW_Ucs2         *name;    /* name of queue (for debugging only) */
  NW_Uint16          length;   /* number of nodes in queue */
} NW_Psq_t;


/*
** Global Function Declarations
*/

/* create a new Protected Signaled Queue */
NW_Psq_t  *NW_Psq_New(const char *name);

/* delete a Protected Signaled Queue */
void NW_Psq_Delete(NW_Psq_t *que);

/* add a new node to the tail of the queue */
void NW_Psq_AddTail(NW_Psq_t *que, NW_Node_t *node);

/* return node from the head of the queue and remove it */
NW_Node_t *NW_Psq_RemoveHead(NW_Psq_t *que);

/* wait for node to be added to queue */
TBrowserStatusCode NW_Psq_WaitForAdd(const NW_Psq_t *que, const NW_Int32 howLong);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_PSQ_H */
