/*
* Copyright (c) 1998-2000 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nwx_defs_symbian.h"
#include "nwx_msg_api.h"
#include "nwx_datastruct.h"
#include "nwx_psq.h"
#include "nwx_msg_types.h"
#include "nwx_msg_dsp_api_epoc32.h"
#include "nwx_logger.h"
#include "util_CDispatcherEng.h"
#include "BrsrStatusCodes.h"

/* Active Object ID */
/*#define AO_NONE            0xFF
#define AO_WAPACLI         0x00
#define AO_WAPALAYER       0x01*/
#define AO_WAPMESSAGE      0x02

CDispatcherEng::CDispatcherEng(CActive::TPriority aPriority) 
  :CActive(aPriority) {

  CActiveScheduler::Add(this);
  __DECLARE_NAME(_S("CDispatcherEng"));

  iDsp_cxt = NULL;

}

CDispatcherEng::~CDispatcherEng() {
  Cancel();
}

void CDispatcherEng::DoCancel() {
  // Program execution never gets here as the request is always completed 
  // right after it is issued. See RequestEvent() below.
  }

void CDispatcherEng::RequestEvent() {

  if (!IsActive()) 
  {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, AO_WAPMESSAGE);
  }

}

void CDispatcherEng::RunL() {

  if (iStatus != AO_WAPMESSAGE) {
    // TODO: Somehow the status of this object is
    // corrupted if the program gets here. Unexpected
    // error!!
    return;
  }

  NW_Msg_t *msg;
  NW_Msg_Handler_t *handler;
  TBrowserStatusCode ret;

  msg = (NW_Msg_t *)NW_Psq_RemoveHead(iDsp_cxt->queue);
  if (!msg) return;

  NW_LOG1(NW_LOG_LEVEL3, "Received message %d\n", msg->type);
  handler = iDsp_cxt->handlers;

  while (handler != NULL) {
    if (handler->type == msg->type) {
      ret = handler->func(msg);
      if (ret == KBrsrOutOfMemory)
      {
        User::Leave(ret);
      }
      break;
    } else {
      handler = (NW_Msg_Handler_t *)handler->next;
    }
  }
  // Request another run of this obj to check for queued msg.
  RequestEvent();
}

NW_Bool CDispatcherEng::IsDspContext() {

  if (iDsp_cxt == NULL) return NW_FALSE;
  else
    return NW_TRUE;

}

void CDispatcherEng::SetDspContext(NW_Msg_Dispatcher_t* aDsp_cxt) {

  iDsp_cxt = aDsp_cxt;

}
/******************************************************************************
  RunError

	Overrides default CActive implementation. Called when RunL method leaves.
******************************************************************************/
TInt CDispatcherEng::RunError(TInt /*aError*/)
{
  return KErrNone;
}
