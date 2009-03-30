/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_msg_messagedispatcheri.h"
#include "BrsrStatusCodes.h"

/* -------------------------------------------------------------------------- *
    class definition
 * ---------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- */
const
NW_Msg_MessageDispatcher_Class_t NW_Msg_MessageDispatcher_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Core_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Msg_MessageDispatcher	 */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
//#pragma message(__FILE__ " TODO: Implement isBrowserRunning without non-const static variable")

const NW_Msg_MessageDispatcher_t NW_Msg_MessageDispatcher = {
  { &NW_Msg_MessageDispatcher_Class },
  
/*  { NW_FALSE } */
};

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Msg_MessageDispatcher_DispatchMessage (const NW_Msg_MessageDispatcher_t* thisObj,
                                          NW_Msg_Message_t* message, 
                                          MMessageListener* listener)
{
  NW_Msg_t* msg = NULL;
  NW_Msg_Address_t myAddress;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (message != NULL);
  NW_ASSERT (listener != NULL);

  (void)thisObj;
  
  NW_TRY (status) {
    /* get our address */
    status = NW_Msg_GetThisReceiver (&myAddress);
    NW_ASSERT (status == KBrsrSuccess);

    /* construct the message */
    msg = NW_Msg_New (0, myAddress, myAddress, NW_MSG_MSG_DSP_HANDLE_MESSAGE);
    NW_THROW_OOM_ON_NULL (msg, status);
    
    msg->data = message;

    /* set the message's listener */
    NW_Msg_Message_SetListener (message, listener);

    /* send the message */
    status = NW_Msg_Send (msg);
    NW_THROW_ON_ERROR (status);

    msg = NULL;
  }

  NW_CATCH (status) {
    NW_Msg_Delete (msg);
  }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Msg_MessageDispatcher_MessageHandler (NW_Msg_t* msg)
{
  NW_Msg_Message_t* message = NULL;
  MMessageListener* listener;

  NW_ASSERT (msg != NULL);

#ifdef NEEDS_SYMBIAN_IMPLEMENTATION	
   /* get our address */	 
  if(!NW_Msg_MessageDispatcher_GetBrowserState(&NW_Msg_MessageDispatcher)) {
    NW_Msg_Delete (msg);
    regturn kBrsrFailure;
  }
#endif
	
  /* get the message */
  message = NW_Msg_MessageOf (msg->data);

  /* dispatch the message to the listener */
  listener = NW_Msg_Message_GetListener (message);

  listener->HandleMessage (message);

  /* clean up */
  NW_Msg_Delete (msg);
  return KBrsrSuccess;
}
