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

/*lint -save -esym(1066, NW_Api_GetScriptProgressCB) -esym(1066, NW_Api_GetScriptBrowserCB) */

#include "nw_wmlscriptch_wmlscriptcontenthandleri.h"
#include "nw_wmlscriptch_wmlscriptapi.h"
#include "nw_wae.h"
#include "nwx_ctx.h"
#include "nwx_string.h"
#include <stdarg.h>
#include "BrsrStatusCodes.h"

/*lint -restore */
/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

static NW_Bool ValidateReferer(NW_WmlScript_ContentHandler_t *thisObj,
                            const NW_HED_DocumentNode_t *referer)
{
  thisObj->wmlBrowserLib = (NW_WmlsCh_IWmlBrowserLib_t*) NW_Object_QueryInterface (referer,
                                &NW_WmlsCh_IWmlBrowserLib_Class);
  /* Check if the refering content handler supports the wml browser interface. */
  if (thisObj->wmlBrowserLib == NULL) {
    return NW_FALSE;
  }
  /* this is an optional interface. */
  thisObj->wmlScrListener = (NW_WmlsCh_IWmlScriptListener_t*) NW_Object_QueryInterface (referer,
                                &NW_WmlsCh_IWmlScriptListener_Class);
  return NW_TRUE;
}

static NW_Ucs2 * GetLocation(const NW_HED_DocumentNode_t *documentNode)
{
  const NW_Text_t  *locationObj = NULL;

  locationObj = NW_HED_ContentHandler_GetURL(NW_HED_ContentHandlerOf(documentNode));

  NW_ASSERT(locationObj != NULL);
  return NW_Text_GetUCS2Buffer (locationObj, NW_Text_Flags_Copy, NULL, NULL);
}

static void ApplyVars(NW_WmlScript_ContentHandler_t *thisObj)
{
  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(thisObj->wmlBrowserLib != NULL);
  (void) NW_WmlsCh_IWmlBrowserLib_Init(thisObj->wmlBrowserLib);
  return;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_WmlScript_ContentHandler_Class_t NW_WmlScript_ContentHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_HED_ContentHandler_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize		             */ sizeof (NW_WmlScript_ContentHandler_t),
    /* construct		               */ _NW_WmlScript_ContentHandler_Construct,
    /* destruct			               */ _NW_WmlScript_ContentHandler_Destruct,
  },
  { /* NW_HED_DocumentNode    	       */
    /* cancel                      */ _NW_WmlScript_ContentHandler_Cancel,
    /* partialLoadCallback	       */ NULL,
    /* initialize                  */ _NW_WmlScript_ContentHandler_Initialize,
    /* nodeChanged                 */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                  */ _NW_WmlScript_ContentHandler_GetBoxTree,
    /* processEvent                */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                 */ _NW_HED_DocumentNode_HandleError,
    /* suspend                     */ _NW_WmlScript_ContentHandler_Suspend,
    /* resume                      */ _NW_HED_DocumentNode_Resume,
    /* allLoadsCompleted           */ _NW_HED_ContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted*/ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus                   */ _NW_HED_ContentHandler_LoseFocus,
    /* gainFocus                   */ _NW_WmlScript_ContentHandler_GainFocus,
    /* handleLoadComplete           */ _NW_HED_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler       */
    /* partialNextChunk            */ _NW_HED_ContentHandler_PartialNextChunk,
    /* getTitle                    */ _NW_WmlScript_ContentHandler_GetTitle,
    /* getUrl                      */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl                  */ _NW_HED_ContentHandler_ResolveURL,
    /* createHistoryEntry          */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry     */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse              */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree               */  NULL,
    /* handleRequest               */ _NW_HED_ContentHandler_HandleRequest,
    /* featureQuery                */ _NW_HED_ContentHandler_FeatureQuery
  },
  { /* NW_WmlScript_ContentHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_WmlScript_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                        va_list* argp)
{
  NW_WmlScript_ContentHandler_t     *thisObj;
  NW_HED_UrlRequest_t               *urlRequest;
  TBrowserStatusCode                       status = KBrsrSuccess;
  NW_HED_DocumentRoot_t             *docRoot = NULL;
  NW_WaeUsrAgent_t                  *wae;
  const NW_Text_t                   *urlText;
  

  /* for convenience */
  NW_ASSERT(dynamicObject != NULL);
  thisObj = NW_WmlScript_ContentHandlerOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_HED_ContentHandler_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  
  /* this is embeddable content*/
  NW_HED_ContentHandler_SetIsEmbeddable (dynamicObject, NW_TRUE);
  
  /* create a copy of the url */
  urlRequest = va_arg (*argp, NW_HED_UrlRequest_t*);
  NW_ASSERT (urlRequest);
  NW_ASSERT (NW_Object_IsInstanceOf (urlRequest, &NW_HED_UrlRequest_Class));

  urlText = NW_HED_UrlRequest_GetUrl (urlRequest);
  thisObj->url = NW_Text_GetUCS2Buffer (urlText, NW_Text_Flags_Copy, NULL, NULL);
  
  /* get the DocumentRoot */
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
  NW_ASSERT(docRoot != NULL);
  
  /* create the script proxy */
  thisObj->scrProxy = NW_ScrProxy_New();
  if (thisObj->scrProxy == NULL) {
    return KBrsrOutOfMemory;
  }

  status = NW_ScrProxy_Initialize(thisObj->scrProxy, thisObj,
                                  NW_Api_GetScriptBrowserCB(),
                                  NW_Api_GetScriptProgressCB(),
                                  &docRoot->appServices->scrDlgApi, 
                                  &docRoot->appServices->wtaiApi,
                                  &docRoot->appServices->scriptSuspResApi,
                                  docRoot->appServices->errorApi.notifyError);
  if (status == KBrsrSuccess) {
    wae = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);
    NW_ASSERT(wae != NULL);
    status = NW_Wae_RegisterWmlScriptProxy(wae, thisObj->scrProxy);
  }
  
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_WmlScript_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_WmlScript_ContentHandler_t *thisObj;
  NW_WaeUsrAgent_t              *wae;
  
  /* for convenience */
  thisObj = NW_WmlScript_ContentHandlerOf (dynamicObject);

  /* make sure that we shut down any running scripts */
  TBrowserStatusCode status = NW_ScrProxy_AbortScript(thisObj->scrProxy);
  NW_ASSERT(status == KBrsrSuccess);

  wae = (NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT(wae != NULL);
  status = NW_Wae_UnRegisterWmlScriptProxy(wae);
  NW_ASSERT(status == KBrsrSuccess);

  NW_ScrProxy_Free(thisObj->scrProxy);
  thisObj->scrProxy = NULL;
  
  /*free the copy of the url*/
  NW_Mem_Free ((NW_Ucs2*) thisObj->url);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_WmlScript_ContentHandler_Initialize (NW_HED_DocumentNode_t* documentNode,
    TBrowserStatusCode aInitStatus )
{
  NW_WmlScript_ContentHandler_t     *thisObj;
  NW_Ucs2                           *myLocation = NULL;
  NW_Ucs2                           *postfields = NULL;
  NW_Ucs2                           *refererUrl = NULL;
  NW_HED_DocumentNode_t             *refererCh  = NULL;
  TBrowserStatusCode                       status;
  NW_HED_ContentHandler_t*          contentHandler;

  NW_REQUIRED_PARAM( aInitStatus );

  /* for convenience */
  thisObj = NW_WmlScript_ContentHandlerOf (documentNode);
  contentHandler = NW_HED_ContentHandlerOf( thisObj );

  /* get our location */
  myLocation = NW_Str_Newcpy(thisObj->url);
  if (!myLocation) {
    UrlLoader_UrlResponseDelete (contentHandler->response);
    contentHandler->response = NULL;
    return KBrsrOutOfMemory;
  }
  
  /* get the content handler who loaded us */
  refererCh = NW_HED_DocumentNode_GetParentNode(NW_HED_DocumentNodeOf(thisObj));
  NW_ASSERT(refererCh != NULL);
  
  /* make sure that the refering content handler has implemented the WML Script
  interface */
  if (ValidateReferer(thisObj, refererCh)) {
    /* get the location of the refering content handler */
    refererUrl = GetLocation(NW_HED_DocumentNodeOf(refererCh));
    if (!refererUrl) {
      NW_Str_Delete(myLocation);
      UrlLoader_UrlResponseDelete (contentHandler->response);
      contentHandler->response = NULL;
      return KBrsrOutOfMemory;
    }

    status = NW_Url_GetQuery(myLocation, &postfields);
    if (status != KBrsrSuccess && status != KBrsrFailure) {
      NW_Str_Delete(myLocation);
      NW_Str_Delete(refererUrl);
      UrlLoader_UrlResponseDelete (contentHandler->response);
      contentHandler->response = NULL;
      return status;
    }
    /*
    ** if postfields are in current location then use those, otherwise use
    ** postfields that may have been generated if location is an asp page.
    */
    if (postfields == NULL) {
      status = NW_Url_GetQuery(contentHandler->response->uri, &postfields);
      if (status != KBrsrSuccess && status != KBrsrFailure) {
        NW_Str_Delete(myLocation);
        NW_Str_Delete(refererUrl);
        UrlLoader_UrlResponseDelete (contentHandler->response);
        contentHandler->response = NULL;
        return status;
      }
    } else {
      NW_Str_Delete(postfields);
      postfields = NULL; /* postfields already included in myLocation */
    }
    
    ApplyVars(thisObj);
    /*
    ** run the script; transfer ownership of location, postfields, referer, and content
    ** to the WmlScript Interpreter.
    */
    status = NW_ScrProxy_RunScript(thisObj->scrProxy,
                                   myLocation, postfields, refererUrl,
                                   contentHandler->response->body->data,
                                   contentHandler->response->body->length,
                                   contentHandler->response->charset);
  } else {
    NW_Str_Delete(myLocation);
    status = KBrsrFailure;
    /* TODO: delete objs otherwise deleted when script runs. */
  }
  
  if(status == KBrsrSuccess) {
    /* Release ownership of the buffer and delete the response */
    contentHandler->response->body->data = NULL;
    UrlLoader_UrlResponseDelete (contentHandler->response);
    contentHandler->response = NULL;
  }
  /*
  ** Note: We dont propogate a node changed event as there is nothing to
  ** display at this point.
  */
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_WmlScript_ContentHandler_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                         NW_LMgr_Box_t** boxTree)
{
  NW_REQUIRED_PARAM(documentNode);
  *boxTree = NULL;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_WmlScript_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree)
{
  NW_WmlScript_ContentHandler_t *thisObj = NULL;

  NW_ASSERT(documentNode != NULL);

  /* for convenience */
  thisObj = NW_WmlScript_ContentHandlerOf (documentNode);

  /* make sure that we shut down any running scripts */
  (void) NW_ScrProxy_AbortScript(thisObj->scrProxy);

  /* call our super class to suspend the children */
  NW_HED_ContentHandler_Class.NW_HED_DocumentNode.suspend (documentNode, aDestroyBoxTree);
}

/* ------------------------------------------------------------------------- */
void
_NW_WmlScript_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode)
{
  TBrowserStatusCode status = KBrsrFailure;
  NW_Msg_t              *cmsg = NULL;
  NW_Msg_Address_t      wmlMsgAddr, scriptMsgAddr;
  
    /* get the messaging address of wml and script interpreter */
  status = NW_Msg_LookupReceiverByName(NW_BROWSER_CORE, &wmlMsgAddr);
  if (status == KBrsrSuccess) {
    status = NW_Msg_LookupReceiverByName(NW_SCRIPT_SERVER, &scriptMsgAddr);
    if (status == KBrsrSuccess) {
      /* TODO: add transaction support */
      cmsg = NW_Msg_New(1, wmlMsgAddr, scriptMsgAddr, NW_MSG_SCRIPT_RESUME_REQ);
      if (cmsg == NULL) {
        status = KBrsrOutOfMemory;
      } else {
        /* Send the message to the script interpreter */
        cmsg->status = KBrsrSuccess;
        status= NW_Msg_Send(cmsg);
      }
    }
  }

  /* clean up */
  if (status != KBrsrSuccess) {
    NW_Msg_Delete(cmsg);
  }

  /* call our super class to suspend the children */
  NW_HED_ContentHandler_Class.NW_HED_DocumentNode.gainFocus (documentNode);
}

/* ------------------------------------------------------------------------- */
const NW_Text_t*
_NW_WmlScript_ContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler)
{
  NW_REQUIRED_PARAM(contentHandler);

  /* we dont have a title */
  return (const NW_Text_t *)NULL;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_WmlScript_ContentHandler_Cancel (NW_HED_DocumentNode_t* documentNode,
                                     NW_HED_CancelType_t cancelType)
{
  NW_WmlScript_ContentHandler_t* thisObj;
  void* browserApp = NULL;
  NW_HED_DocumentRoot_t* docRoot = NULL;
 
  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM(cancelType);

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_WmlScript_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_WmlScript_ContentHandlerOf (documentNode);

  /* make sure that we shut down any running scripts */
  (void) NW_ScrProxy_AbortScript(thisObj->scrProxy);
  /* Make sure the wait note panel is removed. */
  browserApp = NW_Ctx_Get(NW_CTX_BROWSER_APP, 0);

  /* get the global context from the docroot */
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(thisObj);

  if(docRoot != NULL) {
    (void) docRoot->appServices->loadProgress_api.finish(browserApp, NULL);
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/*external method to obtain the url from the script content handler*/

const NW_Ucs2* NW_WmlScript_ContentHandler_Get_Url (NW_WmlScript_ContentHandler_t *thisObj) 
{

  NW_ASSERT(thisObj);

  return thisObj->url;
}
