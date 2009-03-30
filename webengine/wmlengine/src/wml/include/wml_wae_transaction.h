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
**   File: wml_wae_transaction.h
**   Subsystem Name: WAE User Agent
**   Purpose:  Provides interface to transaction
**************************************************************************/
#ifndef WML_WAE_TRANSACTION_H
#define WML_WAE_TRANSACTION_H

/*
** Includes
*/

/*
** Type Declarations
*/

typedef enum {
  NW_WAE_TRANSACTION_STATE_INIT,
  NW_WAE_TRANSACTION_STATE_PENDING,
  NW_WAE_TRANSACTION_STATE_CANCELLED,
  NW_WAE_TRANSACTION_STATE_COMPLETE
} NW_WaeTransactionState_t;

typedef struct _NW_WaeTransaction_t NW_WaeTransaction_t;
struct _NW_WaeTransaction_t {
  NW_Uint16                     tid;    /* transaction id */
  NW_WaeTransactionState_t   state;  /* state of the transaction */
};

/*
** Global Function Declarations
*/
/* initialize the transaction */
void NW_WaeTransaction_Initialize(NW_WaeTransaction_t *trans);

/* get the transaction identifier */
NW_Uint16 NW_WaeTransaction_GetId(const NW_WaeTransaction_t *trans);

/* set the transaction identifier */
void NW_WaeTransaction_SetId(NW_WaeTransaction_t *trans, NW_Uint16 id);

/* get the state of the transaction */
NW_WaeTransactionState_t 
NW_WaeTransaction_GetState(const NW_WaeTransaction_t *trans);

/* set the state of the transaction */
void NW_WaeTransaction_SetState(NW_WaeTransaction_t *trans,
                                NW_WaeTransactionState_t state);

#endif  /* WML_WAE_TRANSACTION_H */
