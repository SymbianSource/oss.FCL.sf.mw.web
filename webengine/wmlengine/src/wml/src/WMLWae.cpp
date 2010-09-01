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
* Description:  Provides an interface to the browser. This module
*                has mostly glue logic to integrate various components of Rainbow. 
*                It exposes a initialization function that create and intializes
*                the WAE components.
*
*/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_logger.h"
#include "wml_scrproxy.h"
#include "wml_core.h"
#include "nwx_ctx.h"
#include "wml_decoder.h"
#include "nw_loadreq.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"
#include "nw_wml1x_epoc32contenthandler.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"
#include "CReferrerHelper.h"

#include "urlloader_urlloaderint.h"

/*
**-------------------------------------------------------------------------
**  Internal Types
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
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************

  Name: NW_WaeUsrAgent_New()

  Description:  Create and initialize a new wae user agent

  Parameters:   

  Return Value: pointer to the wae user agent

******************************************************************/
NW_WaeUsrAgent_t* NW_WaeUsrAgent_New()
{
  NW_WaeUsrAgent_t *waeUsrAgent = NULL;

  waeUsrAgent = (NW_WaeUsrAgent_t*) NW_Mem_Malloc(sizeof(NW_WaeUsrAgent_t));
  if (waeUsrAgent != NULL) {
    waeUsrAgent->wml          = NULL;
    waeUsrAgent->scrProxy     = NULL;
    waeUsrAgent->authApi      = NULL;
    waeUsrAgent->loadProgApi  = NULL;
    waeUsrAgent->errorApi     = NULL;
    waeUsrAgent->wtaiApi      = NULL;
    waeUsrAgent->uiCtx        = NULL;
    waeUsrAgent->evLogApi     = NULL;
    waeUsrAgent->userRedirection  = NULL;
  }
  return waeUsrAgent;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_Free()

  Description:  Free the Wae user agent. This function will shutdown the
  URL Loader, WML Interpreter, Image Loader and the script proxy.

  Parameters:   pointer to the wae user agent

  Return Value: 

******************************************************************/
void NW_WaeUsrAgent_Free(NW_WaeUsrAgent_t *waeUsrAgent)
{
  if (waeUsrAgent != NULL) {
    if (waeUsrAgent->scrProxy != NULL) {
      NW_ScrProxy_Free(waeUsrAgent->scrProxy);
    }

    NW_Mem_Free(waeUsrAgent);
  }

  return;
}

/************************************************************************
  Function: NW_Wae_UnRegisterWml
  Purpose:  Register the wml interpreter with the wae user agent.

  Parameters:  
  wae           - wae browser
  wml           - pointer to the wml interpreter

  Return Values:Return Values:KBrsrSuccess
                KBrsrOutOfMemory
**************************************************************************/
TBrowserStatusCode NW_Wae_RegisterWml(NW_WaeUsrAgent_t *wae, NW_Wml_t *wml)
{
  NW_ASSERT(wae != NULL);
  NW_ASSERT(wml != NULL);
  wae->wml = wml;

  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Wae_UnRegisterWml
  Purpose:  Unregister the wml interpreter with the wae user agent

  Parameters:  
  wae           - wae browser

  Return Values:KBrsrSuccess
                KBrsrNotFound if not previously registered
**************************************************************************/
TBrowserStatusCode NW_Wae_UnRegisterWml(NW_WaeUsrAgent_t *wae)
{
  if (wae)
  {
      wae->wml = NULL;
  }

  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Wae_RegisterWmlScriptProxy
  Purpose:  Register the script proxy with the wae user agent

  Parameters:  
  wae           - wae browser

  Return Values:KBrsrSuccess
**************************************************************************/
TBrowserStatusCode NW_Wae_RegisterWmlScriptProxy(NW_WaeUsrAgent_t *wae, 
                                          NW_ScrProxy_t *scrProxy)
{
   wae->scrProxy = scrProxy;
   return KBrsrSuccess;
}

/************************************************************************
  Function: NW_Wae_UnRegisterWmlScriptProxy
  Purpose:  UnRegister the script proxy with the wae user agent

  Parameters:  
  wae           - wae browser

  Return Values:KBrsrSuccess
**************************************************************************/
TBrowserStatusCode NW_Wae_UnRegisterWmlScriptProxy(NW_WaeUsrAgent_t *wae)
{
  wae->scrProxy = NULL;
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_WaeUsrAgent_RegisterGenDlg
  Purpose:  Register callback functions for generic dialogs.

  Parameters:  
  wae           - wae browser
  genDlgapi     - generic dialogs api

  Return Values: KBrsrSuccess
**************************************************************************/
TBrowserStatusCode 
NW_WaeUsrAgent_RegisterGenDlg(NW_WaeUsrAgent_t *wae,
                          const NW_GenDlgApi_t *genDlgapi)
{
  wae->genDlgApi = genDlgapi;
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_WaeUsrAgent_UnRegisterGenDlg
  Purpose:  UnRegister callback functions for generic dialogs.

  Parameters:  
  wae           - wae browser

  Return Values: KBrsrSuccess
**************************************************************************/
TBrowserStatusCode 
NW_WaeUsrAgent_UnRegisterGenDlg(NW_WaeUsrAgent_t *wae)
{
  wae->genDlgApi = NULL;
  return KBrsrSuccess;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_GetScript()

  Description:  Get a pointer to the WML Script Proxy. The caller of 
                this function should not free the returned pointer.

  Parameters:   *wae - pointer to the wae browser

  Return Value: pointer to the script proxy
                
******************************************************************/
NW_ScrProxy_t * NW_WaeUsrAgent_GetScript(NW_WaeUsrAgent_t *wae)
{
  if (wae != NULL) {
    return wae->scrProxy;
  }
  return NULL;
}

/*****************************************************************

  Name: NW_Wml_CanNavigateToUri()

  Description:  Is it allowed to navigate to a new card. This behaviour
                is defined by the wml scripts running asynchronously.
                If a script has not completed and the user tries to 
                navigate to a new card, the navigation is canceled.

  Parameters: result  - result from the user.

  Return Value: KBrsrSuccess
                KBrsrWaeNavigationCancelled

******************************************************************/
TBrowserStatusCode NW_Wml_CanNavigateToUri()
{
  TBrowserStatusCode       status = KBrsrSuccess;
  NW_Bool           running;
  NW_WaeUsrAgent_t  *wae;

  wae = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT(wae != NULL);

  /* The script proxy registers dynamically in Rainbow 3.0 */
  if (!wae->scrProxy) return KBrsrSuccess;

  /* check if a script is running */
  if ((status = NW_ScrProxy_IsScriptRunning(wae->scrProxy, &running))
              == KBrsrSuccess) {
    if (running == NW_TRUE) {
      status = KBrsrWaeNavigationCancelled;
    }
  }

  return status;
}

/*****************************************************************

  Name: NW_Wml_DoPrevLoad()

  Description:  Sets History to the previous entry and issues Url load request.
                Returns KBrsrFailure if no previous entry

  Parameters:   Wml Content handler pointer

  Return Value: KBrsrSuccess
                KBrsrFailure
                KBrsrOutOfMemory

******************************************************************/
TBrowserStatusCode NW_Wml_DoPrevLoad(void *wml1xCH)
{
  NW_HED_HistoryEntry_t     *prevEntry;
  NW_HED_DocumentRoot_t     *dr;
  const NW_HED_UrlRequest_t *urlRequest;
  TBrowserStatusCode               status = KBrsrFailure;

  dr = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (wml1xCH);
  prevEntry = NW_HED_HistoryStack_GetEntry(dr->historyStack,
                                  NW_HED_HistoryStack_Direction_Previous);
  if (prevEntry) {
    urlRequest = NW_HED_HistoryEntry_GetUrlRequest(prevEntry);

    if ((KBrsrSuccess == 
        NW_HED_UrlRequest_SetReason((NW_HED_UrlRequest_t*)urlRequest, 
                                    NW_HED_UrlRequest_Reason_DocPrev)) &&
        (KBrsrSuccess == 
        NW_HED_UrlRequest_SetCacheMode((NW_HED_UrlRequest_t*)urlRequest, 
                                       NW_CACHE_HISTPREV))) {
      /* 
      ** update the history to point to the new current entry.  It is important
      ** that this is done BEFORE StartRequest.
      */
      NW_HED_HistoryStack_SetCurrent(dr->historyStack, 
                                     NW_HED_HistoryStack_Direction_Previous);

      /* Do the load */ 
      status = NW_HED_DocumentRoot_StartRequest(dr,
                                    NW_HED_DocumentNodeOf( dr ), // top level request is owned by document root.
                                    urlRequest,
                                    (void *)&NW_Wml1x_Epoc32ContentHandler_Class );
    }
  }
  return status;
}

/*****************************************************************

  Name: NW_Wml_UrlLoad()

  Description:  Load a Url.

  Parameters:   url     - url to load  
                postfields - pointer to postfield data struct,
                             OR NULL, if no postfield data exists

  Return Value: KBrsrSuccess
                KBrsrFailure

******************************************************************/
TBrowserStatusCode NW_Wml_UrlLoad(void          *wml1xCH,
                           const NW_Ucs2 *url, 
                           NW_NVPair_t   *postfields,
                   const NW_Cache_Mode_t cacheMode,
				   NW_Ucs2* referrerUrl)
{
  NW_Text_UCS2_t        *urlObj     = NULL;
  NW_HED_UrlRequest_t   *urlRequest = NULL;
  NW_Ucs2               *req_url    = NULL;
  TBrowserStatusCode           status      = KBrsrSuccess;
  NW_HED_DocumentRoot_t *dr;

  dr = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (wml1xCH);
 
  if ( !NW_NVPair_IsEmpty(postfields) ) {
    status = NW_LoadReq_BuildQuery(url, postfields, &req_url, NULL, NULL);
    if (status != KBrsrSuccess) {
      NW_THROW_ERROR();
    }
  } else {
    req_url = NW_Str_Newcpy(url);
    if (req_url == NULL) {
      status = KBrsrOutOfMemory;
      NW_THROW_ERROR();
    }
  }

  urlObj = NW_Text_UCS2_New(req_url, 0, 0);
  if (!urlObj) {
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }

  urlRequest = NW_HED_UrlRequest_New(NW_TextOf (urlObj),
                                      NW_URL_METHOD_GET, 
                                      NULL, NULL,
                                      NW_HED_UrlRequest_Reason_DocLoad,
                                      NW_HED_UrlRequest_LoadNormal,
                                      NW_UrlRequest_Type_Any);

  if (referrerUrl) {
	  SetReferrerHeader(urlRequest, referrerUrl);
  }

  if (!urlRequest) {
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }

  (void)NW_HED_UrlRequest_SetCacheMode(urlRequest, cacheMode);
  
  
  status = NW_HED_DocumentRoot_StartRequest(dr,
                                NW_HED_DocumentNodeOf( dr ), // top level request is owned by document root.
                                urlRequest,
                                (void *)&NW_Wml1x_Epoc32ContentHandler_Class );
  
  if (status == KBrsrSuccess) {
    urlRequest = NULL;  /* To prevent freeing it when exiting the function */
  }

NW_CATCH_ERROR
  NW_Str_Delete(req_url);
  NW_Object_Delete(urlObj);
  if (urlRequest) {
    NW_Object_Delete (urlRequest);
  }
  return status;
}

/*****************************************************************

  Name: NW_Wml_HttpLoad()

  Description:  Do a HTTP load.

  Parameters:   usrAgent        - wae browser
                *url            - url to load  
                is_method_post  - HTTP method
                referer         - referer to the url
                accept_charset  -
                postfields      - name/value pairs
                enctype         - urlencoded or multipart/form-data

  Return Value: KBrsrSuccess
                KBrsrFailure
                KBrsrUnsupportedFormCharset
                KBrsrOutOfMemory

******************************************************************/
TBrowserStatusCode NW_Wml_HttpLoad(void             *wml1xCH,
                            const NW_Ucs2    *url,
                            NW_Bool          is_method_post, 
                            const NW_Ucs2    *referer, 
                            const NW_Ucs2    *accept_charset,
                            NW_Http_Header_t *header,
                            NW_NVPair_t      *postfields,
                            const NW_Ucs2    *enctype,
                       const NW_Cache_Mode_t cacheMode)
{
  NW_Buffer_t           *body       = NULL;
  NW_Ucs2               *resultUrl  = NULL;
  NW_HED_UrlRequest_t   *urlRequest = NULL;
  NW_Text_UCS2_t        *urlObj     = NULL;
  NW_Uint8              method;
  TBrowserStatusCode           status;
  NW_WaeUsrAgent_t      *wae;
  NW_HED_DocumentRoot_t *dr;

  dr = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (wml1xCH);

  wae = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);

  NW_ASSERT(wae != NULL);
  NW_ASSERT(url != NULL);

  status = NW_LoadReq_Create(url, is_method_post, referer,
                             accept_charset, postfields, enctype,
                             NW_DeckDecoder_GetEncoding (wae->wml->decoder), 
                             &header, &resultUrl, &method, &body, NULL, NULL);

  if (status != KBrsrSuccess) {
    NW_THROW_ERROR();
  }
  
  urlObj = NW_Text_UCS2_New (resultUrl, 0, 0);

  if (!urlObj) {
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }
  urlRequest = NW_HED_UrlRequest_New(NW_TextOf (urlObj),
                                     method, 
                                     header, 
                                     body,
                                     NW_HED_UrlRequest_Reason_DocLoad,
                                     NW_HED_UrlRequest_LoadNormal,
                                     NW_UrlRequest_Type_Any);

  if (!urlRequest) {
    UrlLoader_HeadersFree(header);//R->ul
    NW_Buffer_Free(body);
    status = KBrsrOutOfMemory;
    NW_THROW_ERROR();
  }

  (void)NW_HED_UrlRequest_SetCacheMode(urlRequest, cacheMode);
  /* Do the load */   
  status = NW_HED_DocumentRoot_StartRequest(dr,
                                NW_HED_DocumentNodeOf( dr ), // top level request is owned by document root.
                                urlRequest,
                                (void *)&NW_Wml1x_Epoc32ContentHandler_Class );
  if (status == KBrsrSuccess) {
    urlRequest = NULL;  /* To prevent freeing it when exiting the function */
  }

NW_CATCH_ERROR
  NW_Str_Delete(resultUrl);
  NW_Object_Delete(urlObj);
  if (urlRequest) {
    NW_Object_Delete (urlRequest);
  }
  return status;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_DlgListSelect()
  
  Purpose:  Call the list select dialog box.  

  Parameters:  
  wae           - wae browser
  data          - structure to pass to the dialog
  callbackCtx   - callback context
  callback      - the callback to cal after the dialog box exits

  Return Values:
     KBrsrSuccess
     KBrsrNotImplemented

******************************************************************/
TBrowserStatusCode NW_WaeUsrAgent_DlgListSelect(NW_WaeUsrAgent_t *wae, 
                                         NW_Dlg_ListSelect_t *data,
                                         void *callbackCtx,
                                         NW_Dlg_ListSelectCB_t callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ASSERT(wae != NULL);
  if (wae->genDlgApi != NULL) {
    NW_ASSERT(wae->genDlgApi->DialogListSelect != NULL);
    (wae->genDlgApi->DialogListSelect) (data, callbackCtx, callback);
  } else {
    status = KBrsrNotImplemented;
    NW_ASSERT(0); /* dialog not implemented. */
  }
  return status;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_DlgPrompt()

  Purpose:  Call the prompt dialog box.  

  Parameters:  
  wae           - wae browser
  data          - structure to pass to the dialog
  callbackCtx   - callback context
  callback      - the callback to cal after the dialog box exits

  Return Values:
     KBrsrSuccess
     KBrsrNotImplemented

******************************************************************/
TBrowserStatusCode NW_WaeUsrAgent_DlgPrompt(NW_WaeUsrAgent_t *wae, 
                                     NW_Dlg_Prompt_t *data,
                                     void *callbackCtx,
                                     NW_Dlg_PromptCB_t callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ASSERT(wae != NULL);
  if (wae->genDlgApi != NULL) {
    NW_ASSERT(wae->genDlgApi->DialogPrompt != NULL);
    (wae->genDlgApi->DialogPrompt) (data, callbackCtx, callback);
  } else {
    status = KBrsrNotImplemented;
    NW_ASSERT(0); /* dialog not implemented. */
  }
  return status;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_DlgInputPrompt()

  Purpose:  Call the input prompt dialog box.  

  Parameters:  
  wae           - wae browser
  data          - structure to pass to the dialog
  callbackCtx   - callback context
  callback      - the callback to cal after the dialog box exits

  Return Values:
     KBrsrSuccess
     KBrsrNotImplemented

******************************************************************/
TBrowserStatusCode NW_WaeUsrAgent_DlgInputPrompt(NW_WaeUsrAgent_t *wae, 
                                          NW_Dlg_InputPrompt_t *data,
                                          void *callbackCtx,
                                          NW_Dlg_InputPromptCB_t callback)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ASSERT(wae != NULL);
  if (wae->genDlgApi != NULL) {
    NW_ASSERT(wae->genDlgApi->DialogInputPrompt != NULL);
    (wae->genDlgApi->DialogInputPrompt) (data, callbackCtx, callback);
  } else {
    status = KBrsrNotImplemented;
    NW_ASSERT(0); /* dialog not implemented. */
  }
  return status;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_RegisterError()

  Purpose:  register callbacks for error handling.  

  Parameters:  
  wae           - wae browser
  errorApi      - error api

  Return Values:
     KBrsrSuccess

******************************************************************/
TBrowserStatusCode NW_WaeUsrAgent_RegisterError(NW_WaeUsrAgent_t *wae, 
                                         const NW_ErrorApi_t *errorApi)
{
  NW_ASSERT(wae != NULL);
  NW_ASSERT(errorApi != NULL);
  wae->errorApi = errorApi;
  return KBrsrSuccess;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_RegisterLoadProgress()

  Purpose:  register callbacks for error handling.  

  Parameters:  
  wae           - wae browser
  errorApi      - error api

  Return Values:
     KBrsrSuccess

******************************************************************/
TBrowserStatusCode NW_WaeUsrAgent_RegisterLoadProgress(NW_WaeUsrAgent_t *wae, 
                          const NW_UrlLoadProgressApi_t *loadProgApi)
{
  NW_ASSERT(wae != NULL);
  NW_ASSERT(loadProgApi != NULL);
  wae->loadProgApi = loadProgApi;
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_WaeUsrAgent_RegisterEvLog
  Purpose:  Register event log API.

  Parameters:  
  wae           - wae browser
  evLogApi      - event log api

  Return Values: KBrsrSuccess
**************************************************************************/
TBrowserStatusCode 
NW_WaeUsrAgent_RegisterEvLog(NW_WaeUsrAgent_t *wae,
                          const NW_EvLogApi_t *evLogApi)
{
  NW_ASSERT(wae != NULL);
  NW_ASSERT(evLogApi != NULL);
  wae->evLogApi = evLogApi;
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_WaeUsrAgent_UnRegisterEvLog
  Purpose:  UnRegister event log API.

  Parameters:  
  wae           - wae browser

  Return Values: KBrsrSuccess
**************************************************************************/
TBrowserStatusCode 
NW_WaeUsrAgent_UnRegisterEvLog(NW_WaeUsrAgent_t *wae)
{
  wae->evLogApi = NULL;
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_WaeUsrAgent_RegisterUserRedirection
  Purpose:  Register callback functions for user redirection.

  Parameters:  
  wae           - wae browser
  authapi       - NW_UserRedirectionApi_t

  Return Values: KBrsrSuccess
**************************************************************************/
TBrowserStatusCode 
NW_WaeUsrAgent_RegisterUserRedirection(NW_WaeUsrAgent_t *wae,
                                       const NW_UserRedirectionApi_t *userRedirectionapi)
{
  wae->userRedirection = userRedirectionapi;
  return KBrsrSuccess;
}

/************************************************************************
  Function: NW_WaeUsrAgent_UnRegisterUserRedirection
  Purpose:  UnRegister callback functions for user redirection.

  Parameters:  
  wae           - wae browser

  Return Values: KBrsrSuccess
**************************************************************************/
TBrowserStatusCode 
NW_WaeUsrAgent_UnRegisterUserRedirection(NW_WaeUsrAgent_t *wae)
{
  wae->userRedirection = NULL;
  return KBrsrSuccess;
}

/*****************************************************************

  Name: NW_WaeUsrAgent_GetEvLog

  Description: Return pointer to EvLogApi of wae

  Parameters: wae pointer

  Return Value: EvLogApi of wae

*****************************************************************/
const NW_EvLogApi_t *NW_WaeUsrAgent_GetEvLog(NW_WaeUsrAgent_t *wae)
{
  return (wae->evLogApi);
}
