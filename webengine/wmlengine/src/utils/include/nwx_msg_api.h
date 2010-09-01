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

  Subsystem Name: Messaging
  Version: V1.0
  Description:
    Provides interfaces to support messaging in a platform independent
  manner. 

  Messages are sent to a unique message Address. A NW_Msg_Address_t is used to 
  identify the receipient of a message. It is an opaque data type.  
  All receivers of messages must have a unique address.

    An address is a 1-way communicate channel. To receive a response from a 
  message the sender will also need to obtain a unique address of it's own 
  and listen for responses. 

  To send a message, the sender must first locate the reciever using the 
  function NW_Msg_LookupReceiver(Name). This id is then made part of 
  a NW_Msg_t structure. Also in that structure will be the senders address
  (assuming a response is desired), and the message itself.

    The RmMsgSend() function is called to send the message to the receiver.
  This function will return KBrsrSuccess or an error code.

     Messages are allocated by the sender and freed by the receiver.

******************************************************************/
#ifndef NWX_MSG_API_H
#define NWX_MSG_API_H

/*
**----------------------------------------------------------------------
** Preprocessor Macro Definitions
**----------------------------------------------------------------------
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
**----------------------------------------------------------------------
** Includes
**----------------------------------------------------------------------
*/
#include "nwx_defs.h"
#include "nwx_msg_types.h"
#include "BrsrStatusCodes.h"

/*
**----------------------------------------------------------------------
** Type Definitions
**----------------------------------------------------------------------
*/ 

/* Messaging Data Types */
typedef NW_Uint16 NW_Msg_Address_t;
typedef NW_Uint16 NW_Msg_Type_t;
typedef NW_Uint16 NW_Msg_RName_t;


typedef struct _NW_Msg_t NW_Msg_t;

struct _NW_Msg_t {
  NW_Msg_t          *next;    /* the next node */
  void              *data;    /* the content of the message */
  NW_Uint16         tid;      /* transaction ID */
  NW_Msg_Address_t  src;      /* source address */
  NW_Msg_Address_t  dest;     /* destination address */
  NW_Msg_Type_t     type;     /* set by the sender */
  TBrowserStatusCode       status;   /* additional status data (see status.h) */
};
 
/*
**----------------------------------------------------------------------
** Global Function Declarations
**----------------------------------------------------------------------
*/

/* 1-time initialization function */
TBrowserStatusCode NW_Msg_Initialize(const char* qName);

/* create a new message */
NW_Msg_t *NW_Msg_New(const NW_Uint16 tid, const NW_Msg_Address_t src,
                     const NW_Msg_Address_t dest, const NW_Msg_Type_t type);

/* delete a message */
void NW_Msg_Delete(NW_Msg_t *msg);

/* send a message */
TBrowserStatusCode NW_Msg_Send(NW_Msg_t *msg);

/* return address of receiver */
TBrowserStatusCode NW_Msg_LookupReceiverByName(const NW_Msg_RName_t name, 
                                        NW_Msg_Address_t *addr); 

/* return address of receiver */
TBrowserStatusCode NW_Msg_GetThisReceiver(NW_Msg_Address_t *addr);

/* reinitialize the fields of a message */
void NW_Msg_Reinit(NW_Msg_t *msg, const NW_Uint16 tid, const NW_Msg_Address_t dest,
                   const NW_Msg_Address_t src, const NW_Msg_Type_t type, 
                   const TBrowserStatusCode status, void *data );

/* Send a message back to previous source */
TBrowserStatusCode NW_Msg_Reply(NW_Msg_t *msg, const NW_Msg_Type_t type);

/* Send a message on to another destination */
TBrowserStatusCode NW_Msg_Forward(NW_Msg_t *msg, const NW_Msg_Address_t dest, 
                           const NW_Msg_Type_t type);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_MSG_API_H */

 
