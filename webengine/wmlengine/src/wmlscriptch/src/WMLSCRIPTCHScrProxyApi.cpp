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


/****************************************************************
*  WML Script Proxy Callback implementation. This file implements
*  all the wml script proxy callbacks. Most of the functions are 
*  trivial they just invoke the script interfaces on the WML Script 
*  content handler. The dialog libraries and the error api are not
*  currently under a interface; the content handlers should exhibit
*  the same behaviour for these. TODO : hook up the dialog libraries
*  and the error dialog to the generic dialog api.
*****************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/

#include "nwx_string.h"
#include "nwx_logger.h"
#include "nw_errnotify.h"
#include "nwx_http_header.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"
#include "BrsrStatusCodes.h"
#include "MemoryManager.h"

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/
/* WMLS browser library routines called by the script proxy */
static TBrowserStatusCode WmlScrCh_GetVar(void *ctx, const NW_Ucs2 *var,
                                   NW_Ucs2 ** ret_string);

static TBrowserStatusCode WmlScrCh_SetVar(void *ctx, const NW_Ucs2 *var, 
                                   const NW_Ucs2 *value);

static TBrowserStatusCode WmlScrCh_Go(void *ctx, const NW_Ucs2 *url, 
                               const NW_Ucs2 *param, NW_Http_Header_t *header);

static TBrowserStatusCode WmlScrCh_Prev(void *ctx);

static TBrowserStatusCode WmlScrCh_NewContext(void *ctx);

static TBrowserStatusCode WmlScrCh_Refresh(void *ctx);


/* WMLS load progess routines called by the script proxy */
static TBrowserStatusCode
WmlScrCh_ScriptStart(void *ctx, const NW_Ucs2 *url);

static TBrowserStatusCode
WmlScrCh_ScriptComplete(void *ctx, TBrowserStatusCode status,
                        const NW_Ucs2 *message);

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

static const NW_Scr_ProgressApi_t ScrProgressApi = 
{
  WmlScrCh_ScriptStart,
  WmlScrCh_ScriptComplete,
};

static const NW_Scr_WmlBrowserApi_t ScrWmlBrowserApi =
{
  WmlScrCh_GetVar,
  WmlScrCh_SetVar,
  WmlScrCh_Go,
  WmlScrCh_Prev,
  WmlScrCh_NewContext,
  WmlScrCh_Refresh,
};


/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
static TBrowserStatusCode WmlScrCh_ScriptStart(void *ctx, const NW_Ucs2 *url)
{
  NW_WmlScript_ContentHandler_t   *thisObj;
  TBrowserStatusCode                     status = KBrsrSuccess;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);
  if (thisObj->wmlScrListener != NULL) {
    status = NW_WmlsCh_IWmlScriptListener_Start(thisObj->wmlScrListener, url);
  }
  return status;
}


static TBrowserStatusCode
WmlScrCh_ScriptComplete(void *ctx, TBrowserStatusCode status,
                        const NW_Ucs2 *message)
{
  NW_WmlScript_ContentHandler_t   *thisObj;
  TBrowserStatusCode                     ret_status = KBrsrSuccess;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);
  
  if (thisObj->wmlScrListener != NULL) {
    ret_status = NW_WmlsCh_IWmlScriptListener_Finish(thisObj->wmlScrListener,
                                      status, message);
  }
  return ret_status;
}

static TBrowserStatusCode 
WmlScrCh_GetVar(void *ctx, const NW_Ucs2 *var, NW_Ucs2 ** ret_string)
{
  NW_WmlScript_ContentHandler_t   *thisObj;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);

  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  return NW_WmlsCh_IWmlBrowserLib_GetVar(thisObj->wmlBrowserLib, var, ret_string);
}

static TBrowserStatusCode WmlScrCh_SetVar(void *ctx, const NW_Ucs2 *var,
                                   const NW_Ucs2 *value)
{
  NW_WmlScript_ContentHandler_t   *thisObj;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);

  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  return NW_WmlsCh_IWmlBrowserLib_SetVar(thisObj->wmlBrowserLib, var, value);
}

static TBrowserStatusCode WmlScrCh_Go(void *ctx, const NW_Ucs2 *url,
                               const NW_Ucs2 *param,
                               NW_Http_Header_t *header)
{
  NW_WmlScript_ContentHandler_t   *thisObj;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);

  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  return NW_WmlsCh_IWmlBrowserLib_Go(thisObj->wmlBrowserLib, url, param, header);
}

static TBrowserStatusCode WmlScrCh_Prev(void *ctx)
{
  NW_WmlScript_ContentHandler_t   *thisObj;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);

  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  return NW_WmlsCh_IWmlBrowserLib_Prev(thisObj->wmlBrowserLib);
}

static TBrowserStatusCode WmlScrCh_NewContext(void *ctx)
{
  NW_WmlScript_ContentHandler_t   *thisObj;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);

  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  return NW_WmlsCh_IWmlBrowserLib_NewContext(thisObj->wmlBrowserLib);
}

static TBrowserStatusCode WmlScrCh_Refresh(void *ctx)
{
  NW_WmlScript_ContentHandler_t   *thisObj;

  thisObj = NW_WmlScript_ContentHandlerOf(ctx);

  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  return NW_WmlsCh_IWmlBrowserLib_Refresh(thisObj->wmlBrowserLib);
}


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
const NW_Scr_ProgressApi_t* NW_Api_GetScriptProgressCB()
{
  return &ScrProgressApi;
}

const NW_Scr_WmlBrowserApi_t* NW_Api_GetScriptBrowserCB()
{
  return &ScrWmlBrowserApi;
}
