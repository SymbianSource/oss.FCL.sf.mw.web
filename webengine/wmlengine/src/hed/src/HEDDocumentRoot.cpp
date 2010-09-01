/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include <stddef.h>
#include <Uri16.h>

#include "nw_hed_documentrooti.h"
#include "nw_hed_historyentry.h"
#include "nw_hed_urlrequest.h"
#include "nw_hed_urlresponse.h"
#include "nw_hed_treevisitor.h"
#include "nw_hed_appservices.h"
#include "nw_hed_inumbercollector.h"
#include "nw_adt_resizablevector.h"
#include "nw_adt_mapiterator.h"
#include "nw_adt_singleiterator.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nw_wml1x_epoc32contenthandler.h"
#include "nw_hed_iimageviewer.h"
#include "nw_hed_compositecontenthandleri.h"
#include "urlloader_urlloaderint.h"

#include "LMgrObjectBoxOOCi.h"
#include "HedLoadObserver.h"

#include "nwx_string.h"
#include "nw_evlog_api.h"
#include "nwx_logger.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"
#include "HEDDocumentListener.h"

#include "WmlControl.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_PartialLoad_ValidityCheck( NW_HED_DocumentRoot_t* thisObj,
                                  NW_HED_DocumentNode_t** owner,
                                  TBrowserStatusCode loadStatus,
                                  NW_Url_Resp_t* response,
                                  NW_HED_UrlRequest_t* urlRequest )
  {
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( owner != NULL );
  NW_ASSERT( urlRequest != NULL );

  // TODO remove this wmlscript HACK 
  //
  // Wml-scripts can be executed from a <go> tag -- go's usually replace 
  // the top-level document, but in this case we want to handle it as an
  // embedded resource.  Because of this we must change the url-request
  // reason to NW_HED_UrlRequest_Reason_DocLoadChild.
  if( response != NULL && response->contentTypeString != NULL )
    {
    if( NW_Asc_stricmp( (char*)HTTP_application_vnd_wap_wmlscriptc_string, 
	  		     		  (char*)response->contentTypeString ) == 0 )
      {
      NW_HED_UrlRequest_SetReason( urlRequest, NW_HED_UrlRequest_Reason_DocLoadChild );
      // set the owner back to wml content handler
      NW_ASSERT( thisObj->childNode != NULL );
      // wml script onwed by wml ch
      if( NW_Object_IsInstanceOf( thisObj->childNode, &NW_Wml1x_Epoc32ContentHandler_Class ) == NW_TRUE )
        {
        *owner = thisObj->childNode;
        }
      else
        {
        status = KBrsrWmlbrowserBadContentType;
        }
      }
    }  
  if( loadStatus != KBrsrSuccess ) 
    {
    // we failed to load the page and will not be creating a new box tree.
    // The reason for the load was doing Back - since before issuing request
    // we pushed the history back, we now move it forward. This is not the right
    // way to do things and should be cleaned up in future
    if( ( NW_HED_UrlRequest_GetReason( urlRequest ) == NW_HED_UrlRequest_Reason_ShellPrev ) || 
        ( NW_HED_UrlRequest_GetReason( urlRequest ) == NW_HED_UrlRequest_Reason_DocPrev ) )
      {
      NW_HED_HistoryStack_SetCurrent( thisObj->historyStack, NW_HED_HistoryStack_Direction_Next );
      }
    }
  // if a top-level request is cancelled we don't need to do anything else 
  return status;
  }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_DocumentRoot_PushNewHistoryEntry (NW_HED_DocumentRoot_t* thisObj, 
                                         NW_HED_ContentHandler_t* contentHandler,
                                         NW_HED_UrlRequest_t* newReq,
                                         NW_HED_UrlResponse_t* newResp)
  {
  NW_HED_HistoryEntry_t* newEntry;
  TBrowserStatusCode status = KBrsrSuccess;
  const NW_Ucs2* url = NW_HED_UrlRequest_GetRawUrl( newReq );
  TUriParser16 uriParser;
  const TText* linkUrl = NULL;
  NW_Text_t* textUrl = NULL;

  // remove username:password information from the URL
  if (url)
    {
    linkUrl = url;

    TPtrC urlDes (linkUrl, User::StringLength(linkUrl));

    User::LeaveIfError(uriParser.Parse(urlDes));

    if ( uriParser.IsPresent( EUriUserinfo ) )
      {
      // Remove the username and password from the URI
      CUri16* newUri = CUri16::NewLC( uriParser );

      newUri->RemoveComponentL( EUriUserinfo );
    
      const TText* newUrl = newUri->Uri().UriDes().Ptr();

      textUrl = NW_Text_New(HTTP_iso_10646_ucs_2, (void*)newUrl, newUri->Uri().UriDes().Length(), 
                            NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy);

      // delete the old url from the request
      NW_Object_Delete (newReq->url);
      
      // assign the new url to the request
      newReq->url = textUrl;
      CleanupStack::PopAndDestroy( newUri ); // newUri
      }
    }

  if (contentHandler != NULL) 
    {
    newEntry = NW_HED_ContentHandler_CreateHistoryEntry (contentHandler,
							 newReq, newResp);
    }
  else 
    {
    newEntry = NW_HED_HistoryEntry_New (newReq, newResp);
    }
  
  if (newEntry) 
    {
    NW_Ucs2 *responseUrl = NULL;

    status = NW_HED_HistoryStack_PushEntry (thisObj->historyStack, newEntry);
    NW_ASSERT (status == KBrsrSuccess);

    /* We don't want to current entry in the stack to have an invalid url.*/
    responseUrl = (NW_Ucs2 *)NW_HED_UrlResponse_GetRawUrl(newResp);
    if (responseUrl == NULL || NW_Str_Strlen(responseUrl) == 0) 
      {
      
      NW_HED_HistoryStack_SetCurrent (thisObj->historyStack, 
                                      NW_HED_HistoryStack_Direction_Previous);
      }
    
    if(url != NULL)
      {
      NW_ASSERT(thisObj->appServices != NULL);
      NW_EvLog_Log( &(thisObj->appServices->evLogApi), NW_EvLog_HISTORY_PUSH,
		    url );
      }
    }
  else 
    {
    status = KBrsrOutOfMemory;
    }

  return status;
  }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_DocumentRoot_UpdateHistory (NW_HED_DocumentRoot_t* thisObj,
                                   NW_HED_ContentHandler_t* contentHandler,
                                   NW_HED_UrlRequest_t* urlRequest,
                                   NW_Url_Resp_t* response)
{
  NW_HED_UrlResponse_t* newResp = NULL;
  NW_Url_Resp_t* tempResponse = NULL;


  /* get the new and current url-responses */ 
  if (response == NULL) {
    tempResponse = UrlLoader_UrlResponseNew(NW_HED_UrlRequest_GetRawUrl (urlRequest),
    NULL, 0, NULL, NULL, NULL, NW_FALSE, NULL, NULL, 0, NULL, 0, 0, 0, 0);

    NW_THROWIF_MEM (tempResponse);
  
    response = tempResponse;
  }

  if (contentHandler == NULL) {
    newResp = NW_HED_UrlResponse_New (response);
  }
  else {
    newResp = NW_HED_ContentHandler_NewUrlResponse (contentHandler, response);
  }

  NW_THROWIF_MEM (newResp);

  /* this is no longer needed */
  if (tempResponse != NULL) {
    UrlLoader_UrlResponseDelete (tempResponse);
    tempResponse = NULL;
  }

  /* if the request is non-history-based "push" a new history-entry */
  // urlRequest->loadType == NW_HED_UrlRequest_Reason_ShellReload is a temporary
  // fix on the Meta refresh issue when the meta starts a request with ShellReload type
  // and that request returns a different response url ("redirection") to what we already have in
  // the history list. In this case we need to update the history entry with the
  // redirected url otherwise no relative url will work.
  if ((NW_HED_UrlRequest_IsHistoricRequest (urlRequest) == NW_FALSE || 
       urlRequest->reason == NW_HED_UrlRequest_Reason_ShellReload )) 
    {
    NW_HED_HistoryEntry_t* currentEntry;
    currentEntry = NW_HED_HistoryStack_GetEntry( thisObj->historyStack, NW_HED_HistoryStack_Direction_Current );

    if( !currentEntry || !NW_HED_UrlRequest_Equal( currentEntry->urlRequest,urlRequest ) )
      {
      // this is a new request. let's push it to the history stack.
      NW_THROWIF_ERROR( NW_HED_DocumentRoot_PushNewHistoryEntry( thisObj, contentHandler, urlRequest, newResp ) );
      }
    else if( currentEntry )
      {
      // check if the response url has been changed. - redirection case from www.x.com to www2.x.com
      if( currentEntry->urlResponse )
        {
        if( !NW_HED_UrlResponse_Equal( currentEntry->urlResponse, newResp ) )
          {
          // If currentEntry's URL is same as referrer, clear referer before deleting.
          const NW_Text_t* referrerUrl = NW_HED_DocumentRoot_GetDocumentReferrer( thisObj );
          if ( referrerUrl )
            {
            if ( referrerUrl->storage == currentEntry->urlResponse->rawUrl )
              {
              NW_HED_DocumentRoot_SetDocumentReferrer( thisObj, NULL );
              }
            }
          // update response entry
          NW_Object_Delete( currentEntry->urlResponse );
          currentEntry->urlResponse = newResp;
          }
        else
          {
          // free response entry if we do not add it to the history stack
          NW_Object_Delete( newResp );
          }
        }
      }
    }

  /* 
  ** We are navigating in history or preforming a reload.  So we don't need to 
  ** create a new history entry.
  */
  else 
    {
    /* 
    ** TODO we should probably replace the urlResponse found in the history
    ** entry with this one.  The question is how do we know which history 
    ** entry contains the response we want to update?
    */
    NW_Object_Delete (newResp);
    }

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_Object_Delete (newResp);
  UrlLoader_UrlResponseDelete(tempResponse);

  return KBrsrFailure;

NW_CATCH_MEM
  NW_Object_Delete (newResp);
  UrlLoader_UrlResponseDelete(tempResponse);

  return KBrsrOutOfMemory;
}


/* ------------------------------------------------------------------------- */
/*****************************************************************

  Name:         NW_HED_DocumentRoot_CreateTopLevelContentHandler

  Description:  Creates content handler for a given top level context

  Parameters:   See below

  Return Value: loadStatus, *contentHandler

*****************************************************************/
static 
TBrowserStatusCode
NW_HED_DocumentRoot_CreateTopLevelContentHandler(NW_HED_DocumentRoot_t* thisObj,
                                                 NW_HED_DocumentNode_t* owner,
                                                 NW_Url_Resp_t* response,
                                                 NW_HED_UrlRequest_t* urlRequest,
                                                 NW_HED_ContentHandler_t** newContentHandler)
    {
    NW_HED_ContentHandler_t* contentHandler = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    
    // parameter assertion block 
    NW_ASSERT(urlRequest != NULL);
    NW_ASSERT(response != NULL);
    NW_ASSERT(response->contentTypeString != NULL);
    NW_ASSERT(NW_HED_UrlRequest_IsTopLevelRequest(urlRequest) != NW_FALSE);
    
    // Check if we want to create a top-level content handler
    if ((NW_Asc_stricmp((char*)HTTP_text_vnd_wap_wmlscript_string, 
                        (char*)response->contentTypeString) == 0)) 
        {
        // Script cannot be at top level
        status = KBrsrScriptBadContent;
        }
        
    // Try to create a content handler for     
    if (status == KBrsrSuccess)
        {
        // create content handler
        contentHandler = NW_HED_MimeTable_CreateContentHandler(thisObj->mimeTable,
                                                               owner,
                                                               response->contentTypeString,
                                                               urlRequest, response, NW_TRUE);
        // we do support the content-type so initialize the newly created
        // content-handler
        if (contentHandler != NULL) 
            {
            NW_HED_HistoryEntry_t* currentEntry;
            NW_Bool deleteBoxTree = NW_TRUE;
            
            thisObj->documentListener->NewDocument (thisObj, urlRequest, deleteBoxTree);
            // update the history 
            NW_HED_DocumentRoot_UpdateHistory(thisObj, contentHandler, urlRequest, response);
            
            // associate content handler with current history entry 
            currentEntry = NW_HED_HistoryStack_GetEntry(thisObj->historyStack, 
                                                        NW_HED_HistoryStack_Direction_Current);
            
            NW_HED_ContentHandler_SetAssociatedHistoryEntry(contentHandler, currentEntry);
            
            // delete old certInfo, store the new certInfo
            NW_Mem_Free(thisObj->certInfo);
            thisObj->certInfo = NULL;
            if (response != NULL)
                {
                thisObj->certInfo = response->certInfo;
                // take ownership
                response->certInfo = NULL;
                }

            // set childnode. current childnode is saved as previous at 
            // MHEDDocumentListener::NewDocument
            thisObj->childNode = NW_HED_DocumentNodeOf( contentHandler );
            }
        else
            {
            // if this is null we don't support the content-type of the response
            status = KBrsrWmlbrowserBadContentType;
            }
        }

    *newContentHandler = contentHandler;
    
    return status;
    }

/* ------------------------------------------------------------------------- */
static
NW_Bool
NW_HED_DocumentRoot_OnLoadError (NW_HED_DocumentRoot_t* thisObj,
                                 NW_Int16 errorClass,
                                 NW_WaeError_t error)
{
   return thisObj->documentListener->LoadEnd (thisObj, errorClass, error);
  /* Be careful adding code after this line, since we may already exitted the browser */

}

/* ------------------------------------------------------------------------- */
static
void
NW_HED_DocumentRoot_HandleLoadError (NW_HED_DocumentRoot_t* thisObj,
                                     NW_HED_DocumentNode_t* owner,
                                     const NW_HED_UrlRequest_t *urlRequest,
                                     NW_Int16 errorClass,
                                     TBrowserStatusCode error)
{
  (void) urlRequest;

  /* 
  ** only handle errors generated by the document-root or the 
  ** current content handler.
  */
  if ((owner == NW_HED_DocumentNodeOf (thisObj)) || 
      (owner == NW_HED_DocumentNodeOf (thisObj->childNode))) {
    thisObj->documentErrorClass = errorClass;
    thisObj->documentError = error;
    (void)NW_HED_DocumentNode_Cancel (thisObj, NW_HED_CancelType_Error);
  }
}

/* ------------------------------------------------------------------------- */
static
void
NW_HED_DocumentRoot_HandleIntraPageNavError (NW_HED_DocumentRoot_t* thisObj,
                                             NW_HED_DocumentNode_t* owner,
                                             const NW_HED_UrlRequest_t *urlRequest,
                                             NW_Int16 errorClass,
                                             TBrowserStatusCode error)
{
  (void) urlRequest;

  /* 
  ** only handle errors generated by the document-root or the 
  ** current content handler.
  */
  if ((owner == NW_HED_DocumentNodeOf (thisObj)) || 
      (owner == NW_HED_DocumentNodeOf (thisObj->childNode))) {
    thisObj->documentErrorClass = errorClass;
    thisObj->documentError = error;
    (void)NW_HED_DocumentNode_Cancel (thisObj, NW_HED_CancelType_Error);
  }
}

/* ------------------------------------------------------------------------- */
static
NW_Bool
NW_HED_DocumentRoot_HandleSteadyStateError (NW_HED_DocumentRoot_t* thisObj,
                                            NW_HED_DocumentNode_t* owner,
                                            const NW_HED_UrlRequest_t *urlRequest,
                                            NW_Int16 errorClass,
                                            TBrowserStatusCode error)
{
  (void) owner;
  (void) urlRequest;

  /* simply show the error */
  return thisObj->documentListener->ReportError (thisObj, errorClass, (NW_WaeError_t) error, NULL);
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_DocumentRoot_GetRestoreHistoryDirection (NW_HED_DocumentRoot_t* thisObj,
                                                NW_HED_HistoryStack_Direction_t* restoreDir)
{
  NW_HED_HistoryEntry_t* entry = NULL;
  const NW_HED_UrlRequest_t* urlReq = NULL;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Uint8 reason;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
  NW_ASSERT (restoreDir);

  /* get the reason the current load was issued */
  NW_THROWIF_NULL (entry = NW_HED_HistoryStack_GetEntry (thisObj->historyStack,
                                        NW_HED_HistoryStack_Direction_Current));
  NW_THROWIF_NULL (urlReq = NW_HED_HistoryEntry_GetUrlRequest (entry));

  reason = NW_HED_UrlRequest_GetReason (urlReq);

  /* 
  ** given the reason for the load that just failed determine the history
  ** direction that would "restore" it.  If the reason was either restore-next
  ** or restore-prev then the "restored" page failed for some reason and we
  ** should do nothing.
  */

  switch (reason)
  {
    case NW_HED_UrlRequest_Reason_ShellPrev:
    case NW_HED_UrlRequest_Reason_DocPrev:
		  *restoreDir = NW_HED_HistoryStack_Direction_RestoreNext;
		  break;

    case NW_HED_UrlRequest_Reason_DocLoad:
    case NW_HED_UrlRequest_Reason_ShellLoad:
    case NW_HED_UrlRequest_Reason_ShellNext:
		  *restoreDir = NW_HED_HistoryStack_Direction_RestorePrevious;
		  break;
    
    case NW_HED_UrlRequest_Reason_ShellReload:
    case NW_HED_UrlRequest_Reason_Undefined:
    case NW_HED_UrlRequest_Reason_DocLoadChild:
		  status = KBrsrFailure;
		  break;

	  case NW_HED_UrlRequest_Reason_RestorePrev:
    case NW_HED_UrlRequest_Reason_RestoreNext:
		  status = KBrsrFailure;
		  break;
  }

  return status;

NW_CATCH_ERROR
  status = KBrsrFailure;
  return status;
}


/* ------------------------------------------------------------------------- */
static
NW_Bool
NW_HED_DocumentRoot_RestorePrev (NW_HED_DocumentRoot_t* thisObj,
                                 NW_Int16 errorClass,
                                 NW_WaeError_t error)
{
  TBrowserStatusCode status;
  NW_HED_HistoryStack_Direction_t restoreDir(NW_HED_HistoryStack_Direction_Current);

  /* avoid 'unreferenced formal parameter' warnings */
  (void) errorClass;
  (void) error;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* load the previous page from history */
  status =
    NW_HED_DocumentRoot_GetRestoreHistoryDirection (thisObj, &restoreDir);
  if (status == KBrsrSuccess) {
      (void) NW_HED_DocumentRoot_HistoryLoad (thisObj, restoreDir);
    }

  return NW_FALSE;
}

/* ------------------------------------------------------------------------- */
static
void
NW_HED_DocumentRoot_HandlePageLoadStarted (NW_HED_DocumentRoot_t* thisObj)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* reset the documentStatus and set the loading flag */
  thisObj->documentErrorClass = BRSR_STAT_CLASS_NONE;
  thisObj->documentError = KBrsrSuccess;
  thisObj->isLoading = NW_TRUE;
  thisObj->chunkIndex = 0;
}

/* ------------------------------------------------------------------------- */
static
void
NW_HED_DocumentRoot_HandlePageLoadCompleted (NW_HED_DocumentRoot_t* thisObj)
{
  NW_Bool cancel = NW_FALSE;
  NW_Int16 errorClass;
  NW_WaeError_t error;
  NW_Bool exitBrowser = NW_FALSE;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* reset the loading flag */
  thisObj->isLoading = NW_FALSE;

  /* reset the cancel flag and store the status for further use */
  if (thisObj->isCancelling == NW_TRUE) {
    cancel = thisObj->isCancelling;
    thisObj->isCancelling = NW_FALSE;    
  }

  /* reset the documentStatus */
  errorClass = thisObj->documentErrorClass;
  error = thisObj->documentError;

  thisObj->documentErrorClass = BRSR_STAT_CLASS_NONE;
  thisObj->documentError = KBrsrSuccess;
 
  /*
  ** it is possible for a page to load embedded resources while 
  ** performing intra-page navigation (this can happen in wml while navigating 
  ** between cards that contain images).  When all of the embedded resources 
  ** are loaded this method will be called.  In this case we simply need to 
  ** call HandleIntraPageNavigationCompleted.
  */
  if (thisObj->isIntraPageNav > 0) {
    (void) NW_HED_DocumentRoot_HandleIntraPageNavigationCompleted (thisObj);
  }

  /* if the page loaded without errors... */
  else if (error == KBrsrSuccess) {
    /* 
    ** tell the content handler that all of it loads are done so that it
    ** can start tasks that were postponed until the document was completely loaded.
    */
    if(thisObj->childNode) {
    NW_HED_DocumentNode_AllLoadsCompleted (thisObj->childNode);
  }
  }

  /* 
  ** else if cancelled... Currently the thought is to treat cancel just 
  ** like any other error (which is handled below).  One could add a 
  ** block here to handle it differently.
  */

  /* else the page failed to load... */
  else {
    if (errorClass != BRSR_STAT_CLASS_GENERAL ||
        error != KBrsrHedContentDispatched) {
      exitBrowser = thisObj->documentListener->ReportError (thisObj, errorClass, error, NW_HED_DocumentRoot_OnLoadError);
    }
    if (exitBrowser == NW_TRUE) {
      return;
    }

    if (thisObj->childNode == NULL) {
      return;
    }
  }

  /* if cancel mechanism was in progress load the most recent urlRequest 
     if it exists*/
  if (cancel == NW_TRUE)  {

    if (thisObj->nextUrlRequestToLoad != NULL) {
      NW_HED_UrlRequest_t *urlRequest;

      urlRequest = thisObj->nextUrlRequestToLoad;
      thisObj->nextUrlRequestToLoad = NULL;
			NW_HED_DocumentRoot_StartRequest (thisObj, NW_HED_DocumentNodeOf(thisObj),
        urlRequest, NULL );
    }  
  } 
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_DocumentRoot_InitContexts (NW_HED_DocumentRoot_t* thisObj)
{
  NW_Int8 id;
  NW_HED_Context_t* globalContext = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
  
  thisObj->contextMap = NULL;

  /* create the map */
  NW_THROWIF_NULL (thisObj->contextMap = NW_ADT_ResizableMap_New (sizeof (NW_Int8),
      sizeof (NW_HED_Context_t*), 2, 1));

  /* create and add the global context */
  NW_THROWIF_NULL (globalContext = NW_HED_Context_New());

  id = NW_HED_DocumentRoot_ContextGlobalId;
  NW_THROWIF_ERROR (NW_HED_DocumentRoot_AddContext (thisObj, globalContext, &id));

  return KBrsrSuccess;

NW_CATCH_ERROR
  NW_Object_Delete (thisObj->contextMap);
  thisObj->contextMap = NULL;

  NW_Object_Delete (globalContext);

  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
static
void
NW_HED_DocumentRoot_ShutdownContexts (NW_HED_DocumentRoot_t* thisObj)
{
  NW_ADT_MapIterator_t mapIterator;
  void **key;
  void**  valuePtr;
  TBrowserStatusCode status;
  NW_ADT_Iterator_t *iterator;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* delete each context */
  status = NW_ADT_MapIterator_Initialize(&mapIterator,thisObj->contextMap);
  if (status!= KBrsrSuccess){
    return;
  }
  iterator = NW_ADT_IteratorOf(&mapIterator);
  while (NW_ADT_Iterator_HasMoreElements (iterator)) {
    NW_HED_Context_t* context;

    key = NW_ADT_Iterator_NextElement (iterator);
    NW_ASSERT (key != NULL);

    /* fetch a pointer to the value from the map Returns NULL if item
       doesn't exist. */
    valuePtr = NW_ADT_Map_GetUnsafe (thisObj->contextMap, key);

    /* use byte-wise copy to get the unaligned data at valuePtr into
       the context pointer */
    if (valuePtr != NULL)
    {
      (void) NW_Mem_memcpy (&context, valuePtr, sizeof context);
    }
    else
    {
      /* no context exists for the given key */
      context = NULL;
    }

    NW_Object_Delete (context);
  }


  /* delete the map */
  NW_Object_Delete (thisObj->contextMap);
  thisObj->contextMap = NULL;

  return;

}

/* ------------------------------------------------------------------------- */
const
NW_HED_DocumentRoot_Class_t NW_HED_DocumentRoot_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_HED_DocumentNode_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                 */
    /* interfaceList                  */ NW_HED_DocumentRoot_InterfaceList
  },
  { /* NW_Object_Dynamic              */
    /* instanceSize                   */ sizeof (NW_HED_DocumentRoot_t),
    /* construct                      */ _NW_HED_DocumentRoot_Construct,
    /* destruct                       */ _NW_HED_DocumentRoot_Destruct
  },
  { /* NW_HED_DocumentNode            */
    /* cancel                         */ _NW_HED_DocumentRoot_Cancel,
    /* partialLoadCallback	          */ _NW_HED_DocumentRoot_PartialLoadCallback,
    /* initialize                     */ _NW_HED_DocumentNode_Initialize,
    /* nodeChanged                    */ _NW_HED_DocumentRoot_NodeChanged,
    /* getBoxTree                     */ _NW_HED_DocumentRoot_GetBoxTree,
    /* processEvent                   */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                    */ _NW_HED_DocumentRoot_HandleError,
    /* suspend                        */ _NW_HED_DocumentRoot_Suspend,
    /* resume                         */ _NW_HED_DocumentRoot_Resume,
    /* allLoadsCompleted              */ _NW_HED_DocumentRoot_AllLoadsCompleted,
    /* intraPageNavigationCompleted   */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus                      */ _NW_HED_DocumentRoot_LoseFocus,
    /* GainFocus                      */ _NW_HED_DocumentRoot_GainFocus,
    /* handleLoadComplete             */ _NW_HED_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_DocumentRoot            */
    /* documentDisplayed              */ _NW_HED_DocumentRoot_DocumentDisplayed
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const NW_HED_DocumentRoot_InterfaceList[] = {
  &NW_HED_DocumentRoot_ICompositeNode_Class,
  &NW_HED_DocumentRoot_ILoadRecipient_Class,
  &NW_HED_DocumentRoot_ILoaderListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const NW_HED_ICompositeNode_Class_t NW_HED_DocumentRoot_ICompositeNode_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_HED_ICompositeNode_Class,
    /* querySecondary        */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary   */
    /* offset                */ offsetof (NW_HED_DocumentRoot_t,
					  NW_HED_ICompositeNode)
  },
  { /* NW_HED_ICompositeNode */
    /* removeChild           */ _NW_HED_DocumentRoot_ICompositeNode_RemoveChild,
    /* getChildren           */ _NW_HED_DocumentRoot_ICompositeNode_GetChildren
  }
};

/* ------------------------------------------------------------------------- */
const
NW_HED_ILoadRecipient_Class_t NW_HED_DocumentRoot_ILoadRecipient_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_HED_ILoadRecipient_Class,
    /* queryInterface        */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface   */
    /* offset                */ offsetof (NW_HED_DocumentRoot_t,
					  NW_HED_ILoadRecipient)
  },
  { /* NW_HED_ILoadRecipient */
    /* processPartialLoad    */ _NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad
  }
};

/* ------------------------------------------------------------------------- */
const
NW_HED_ILoaderListener_Class_t NW_HED_DocumentRoot_ILoaderListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_HED_ILoaderListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_HED_DocumentRoot_t,
					   NW_HED_ILoaderListener)
  },
  { /* NW_HED_ILoaderListener */
    /* isLoading              */ _NW_HED_DocumentRoot_ILoaderListener_IsLoading,
    /* loadProgressOn         */ _NW_HED_DocumentRoot_ILoaderListener_LoadProgressOn,
    /* loadProgressOff        */ _NW_HED_DocumentRoot_ILoaderListener_LoadProgressOff
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argList)
{
  NW_HED_DocumentRoot_t* thisObj = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_HED_DocumentRoot_Class));
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (dynamicObject);

  NW_TRY (status) {
    /* invoke our superclass constructor */
    status = _NW_HED_DocumentNode_Construct (dynamicObject, argList);
    if (status != KBrsrSuccess) {
      return status;
    }

    /* initialize the member variables */
    thisObj->mimeTable = va_arg (*argList, NW_HED_MimeTable_t*);
    NW_ASSERT (thisObj->mimeTable != NULL);
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj->mimeTable,
                                       &NW_HED_MimeTable_Class));

    thisObj->appServices = va_arg (*argList, NW_HED_AppServices_t*);
    NW_ASSERT (thisObj->appServices != NULL);

    thisObj->browserApp_Ctx = va_arg(*argList, void*);

/* Let's be pessimistic and assume that function calls below will fail.. */
  status = KBrsrFailure;

    thisObj->loadList = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New (sizeof (NW_HED_DocumentRoot_LoadContext_t*),
                                  5, 10);
    NW_THROW_OOM_ON_NULL (thisObj->loadList, status);
    thisObj->loader = NW_HED_Loader_New ();
    NW_THROW_OOM_ON_NULL (thisObj->loader, status);
    status =
      NW_HED_Loader_SetLoaderListener (thisObj->loader,
                                       &thisObj->NW_HED_ILoaderListener);

    thisObj->historyStack = NW_HED_HistoryStack_New ();
    NW_THROW_OOM_ON_NULL (thisObj->historyStack, status);

    status = NW_HED_DocumentRoot_InitContexts (thisObj);
    NW_THROW_ON_ERROR (status);

    /*this may not be the best place for this - vk*/
    thisObj->isCancelling = NW_FALSE;
    thisObj->certInfo = NULL;
  }

  NW_CATCH (status) {
    NW_HED_DocumentRoot_ShutdownContexts (thisObj);
    NW_Object_Delete (thisObj->historyStack);
    NW_Object_Delete (thisObj->loader);
    NW_Object_Delete (thisObj->loadList);
  }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (dynamicObject);

  NW_Mem_Free(thisObj->certInfo);

  /* delete our dynamic members */
  NW_Object_Delete (thisObj->historyStack);
  NW_Object_Delete (thisObj->childNode);
  NW_Object_Delete (thisObj->loader);

  if(thisObj->loadList != NULL)
      {
      NW_ADT_Vector_Metric_t size;
      NW_ADT_Vector_Metric_t i;

      size =  NW_ADT_Vector_GetSize(thisObj->loadList);
      for (i =0; i <size; i++)
          {
          NW_HED_DocumentRoot_LoadContext_t* loadContext;

          loadContext = *(NW_HED_DocumentRoot_LoadContext_t**)
          NW_ADT_Vector_ElementAt (thisObj->loadList, i);
          NW_Mem_Free(loadContext);
          loadContext = NULL;
          }
      NW_Object_Delete(thisObj->loadList);
      }

 

  NW_HED_DocumentRoot_ShutdownContexts (thisObj);
}

/* ------------------------------------------------------------------------- *
   NW_HED_DocumentNode methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_Cancel (NW_HED_DocumentNode_t* /*documentNode*/,
			     NW_HED_CancelType_t /*cancelType*/)
{
/*  NW_HED_DocumentRoot_t* thisObj;
  
  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
				     &NW_HED_DocumentRoot_Class));
  
  // for convenience 
  thisObj = NW_HED_DocumentRootOf (documentNode);
  
  //   If there are requests in the request list and if the isCancelling
  //   flag is not already set,set the isCancelling flag to indicate
  //   that the cancel mechanism is in progress. Call
  //   NW_HED_Loader_CancelAllLoads to cancel all outstanding requests
  if (!thisObj->isCancelling) {
    if (NW_HED_Loader_GetRequestListSize (thisObj->loader) > 0) {
      thisObj->isCancelling = NW_TRUE;
      NW_HED_Loader_CancelAllLoads (thisObj->loader); 
    }
  }
  
  if (thisObj->childNode != NULL) {
    (void)NW_HED_DocumentNode_Cancel (thisObj->childNode, cancelType);
  }
*/  
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_PartialLoadCallback(NW_HED_DocumentNode_t* documentNode,
                                   TBrowserStatusCode loadStatus,
                                   NW_Int32 chunkIndex,
                                   NW_HED_UrlRequest_t* urlRequest,
                                   NW_Url_Resp_t* response,
                                   NW_Uint16 transactionId,
                                   void* context )
  {
  NW_REQUIRED_PARAM( context );
  NW_REQUIRED_PARAM( transactionId );
  NW_REQUIRED_PARAM( urlRequest );
  NW_REQUIRED_PARAM( loadStatus );

  NW_TRY( status ) 
    {
    NW_HED_DocumentRoot_t* thisObj;
    thisObj = NW_HED_DocumentRootOf( documentNode );

    // keep track of chunk
    thisObj->chunkIndex = chunkIndex;

    // first chunk
    if( chunkIndex == 0 )
      {  
      status = NW_HED_ContentHandler_PartialNextChunk( 
        NW_HED_ContentHandlerOf( thisObj->childNode ), chunkIndex, response, context );
      _NW_THROW_ON_ERROR( status );
      }
    // further chunk
    else if( chunkIndex != -1 )
      {        
      status = NW_HED_ContentHandler_PartialNextChunk( 
        NW_HED_ContentHandlerOf( thisObj->childNode ), chunkIndex, response, context );
      _NW_THROW_ON_ERROR( status );
      }
    // the last chunk. 
    else 
      {
      NW_LOG0( NW_LOG_LEVEL2, "Performance measurement: top-level content loaded" );
      // initialize the content handler
      status = NW_HED_DocumentNode_Initialize( thisObj->childNode, loadStatus );
      // Notify the document listener that we've loaded a new document 
      (void)thisObj->documentListener->LoadEnd (thisObj, BRSR_STAT_CLASS_GENERAL, status);
      // dispatched content means load success
      if( status == KBrsrHedContentDispatched ) 
        {
        status = KBrsrSuccess;
        }
      // last response is not taken by the content handler, 
      // let's release it right here
      UrlLoader_UrlResponseDelete( response );

      _NW_THROW_ON_ERROR( status );
      }
    }
  NW_CATCH( status ) 
    {
    }
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_NodeChanged (NW_HED_DocumentNode_t* documentNode,
                                  NW_HED_DocumentNode_t* childNode)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* deal with the 'unreferenced formal parameter' warning */
  (void) childNode;

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);

  /* notify our listener */
  (void)thisObj->documentListener->DocumentChanged( thisObj, ((CActive::TPriority)( CActive::EPriorityLow + 2 )) );

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                 NW_LMgr_Box_t** boxTree)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);
  if(thisObj->childNode == NULL)
    {
    return KBrsrFailure;
    }
  /* we simply allow our single child to create its box tree within the
     supplied parent box */
  return NW_HED_DocumentNode_GetBoxTree (thisObj->childNode, boxTree);
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_HandleError (NW_HED_DocumentNode_t* documentNode,
                                  const NW_HED_UrlRequest_t *urlRequest,
                                  NW_Int16 errorClass,
                                  NW_WaeError_t error)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
				     &NW_HED_DocumentRoot_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);

  if (NW_HED_DocumentRoot_IsLoading(thisObj) == NW_TRUE) {
    NW_HED_DocumentRoot_HandleLoadError (thisObj, documentNode, urlRequest,
                                         errorClass, (TBrowserStatusCode) error);
  }
  else if (NW_HED_DocumentRoot_IsIntraPageNav(thisObj) > 0) {
    NW_HED_DocumentRoot_HandleIntraPageNavError (thisObj, documentNode,
                                                 urlRequest, errorClass,
                                                 (TBrowserStatusCode) error);
  }
  else {
    NW_HED_DocumentRoot_HandleSteadyStateError (thisObj, documentNode,
                                                urlRequest, errorClass,
                                                (TBrowserStatusCode) error);
  }
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree)
  {
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);

  /* we simply suspend our one child */
  if( thisObj->childNode != NULL ) 
    {
    NW_HED_DocumentNode_Suspend ( thisObj->childNode, aDestroyBoxTree );
    }
  thisObj->childNode = NULL;
  }

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_Resume (NW_HED_DocumentNode_t* documentNode)
  {
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf( documentNode );
 
  if (thisObj->childNode != NULL) 
    {
    /* resume the content handler to cause it to reactivate */
    NW_HED_DocumentNode_Resume ( thisObj->childNode );
    }  
  /* notify our listener */
  (void)thisObj->documentListener->DocumentChanged( thisObj, ((CActive::TPriority)( CActive::EPriorityLow + 2 )) );
  }

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);

  /* we simply tell our one child */
  if (thisObj->childNode != NULL) {
    NW_HED_DocumentNode_AllLoadsCompleted (thisObj->childNode);
  }
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_LoseFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);

  /* we losefocus for our one child */
  if (thisObj->childNode != NULL) {
    NW_HED_DocumentNode_LoseFocus (thisObj->childNode);
  }
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_GainFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_DocumentRoot_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_DocumentNode_Class));

  /* for convenience */
  thisObj = NW_HED_DocumentRootOf (documentNode);

  /* we gainfocus our one child */
  if (thisObj->childNode != NULL) {
    NW_HED_DocumentNode_GainFocus (thisObj->childNode);
  }
}

/* ------------------------------------------------------------------------- *
   NW_HED_ICompositeNode method implemtations
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_ICompositeNode_RemoveChild (NW_HED_ICompositeNode_t* icompositeNode,
                                                 NW_HED_DocumentNode_t* childNode)
{
  NW_HED_DocumentRoot_t* thisObj;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (icompositeNode,
                                     &NW_HED_DocumentRoot_ICompositeNode_Class));

  /* for convenience */
  thisObj = (NW_HED_DocumentRoot_t*)NW_Object_Interface_GetImplementer (icompositeNode);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
 
  /* only remove the node if it actually matches */
  if (NW_HED_DocumentNodeOf (thisObj->childNode) != childNode) {
    return KBrsrNotFound;
  }

  thisObj->childNode = NULL;
  childNode->parent = NULL;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_ICompositeNode_GetChildren (NW_HED_ICompositeNode_t* icompositeNode,
                                                 NW_ADT_Iterator_t** childIterator)
{
  NW_HED_DocumentRoot_t* thisObj;
  NW_ADT_SingleIterator_t* iterator;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (icompositeNode,
                                     &NW_HED_DocumentRoot_ICompositeNode_Class));

  /* for convenience */
  thisObj = (NW_HED_DocumentRoot_t*)NW_Object_Interface_GetImplementer (icompositeNode);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* instantiate the iterator */
  iterator = NW_ADT_SingleIterator_New (&thisObj->childNode,
                                        sizeof (thisObj->childNode));
  if (iterator == NULL) {
    return KBrsrOutOfMemory;
  }
  *childIterator = (NW_ADT_Iterator_t*) iterator;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   ILoadRecipient implementation
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad( NW_HED_ILoadRecipient_t* thisObj,
                                                        TBrowserStatusCode loadStatus,
                                                        NW_Uint16 transactionId,
                                                        NW_Int32 chunkIndex,
                                                        NW_Url_Resp_t* response,
                                                        NW_HED_UrlRequest_t* urlRequest,
                                                        void* clientData )
  {
  TBrowserStatusCode failedStatus = KBrsrSuccess;
  NW_HED_DocumentRoot_t* documentRoot;
  NW_HED_DocumentRoot_LoadContext_t* loadContext = NULL;
  NW_ADT_Vector_Metric_t index;

  NW_TRY( status ) 
    {
    // parameter assertion block
    NW_ASSERT( thisObj != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( thisObj,
                                       &NW_HED_DocumentRoot_ILoadRecipient_Class ) );
    NW_ASSERT( urlRequest != NULL );
    NW_ASSERT( clientData != NULL );

    // for convenience
    documentRoot = (NW_HED_DocumentRoot_t*)
      NW_Object_QueryInterface (thisObj, &NW_HED_DocumentRoot_Class);
    loadContext = (NW_HED_DocumentRoot_LoadContext_t*) clientData;

    NW_LOG4( NW_LOG_LEVEL2, "_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad START trans=%d chunkIndex=%d status=%d, loadStatus=%d", 
      transactionId, chunkIndex, status, loadStatus );

    // find the loadContext on our load list - if it isn't there, the load has
    // been deleted and should not be processed 
    index = NW_ADT_Vector_GetElementIndex( documentRoot->loadList, &loadContext );
    if( index != NW_ADT_Vector_AtEnd )
      {
      // some general checks
      status = NW_HED_PartialLoad_ValidityCheck( documentRoot, &loadContext->owner, 
        loadStatus, response, urlRequest );
      _NW_THROW_ON_ERROR( status );
      _NW_THROW_ON_ERROR( loadStatus );

      // Report each chunk to the load observer -- except the last one.
      if (loadContext->loadObserver && response)
        {
        if (chunkIndex >= 0)
          {
          loadContext->loadObserver->ChunkReceived(transactionId, chunkIndex, *response, 
              *urlRequest, clientData);
          }
        }

      if( chunkIndex == 0 )
        {
        NW_HED_ContentHandler_t* newContentHandler = NULL;
        // first chunk        
        if( NW_HED_UrlRequest_IsTopLevelRequest( urlRequest ) == NW_TRUE )
          {
          // create content handler
           status = NW_HED_DocumentRoot_CreateTopLevelContentHandler( documentRoot, loadContext->owner, 
            response, urlRequest, &newContentHandler );
          _NW_THROW_ON_ERROR( status );
          }
        // pass the content to the owner
        status = NW_HED_DocumentNode_PartialLoadCallback( loadContext->owner, loadStatus, chunkIndex, 
          urlRequest, response, transactionId, loadContext->clientData );
        // NW_HED_DocumentNode_PartialLoadCallback takes the ownership of response
        response = NULL;

        _NW_THROW_ON_ERROR( status );      
        // destroy prev document if the first chunk is all set unless
        // the new document is an unknown content handler or a wml ch ( onenterbackward)
        if( newContentHandler )
          {
          if( NW_Object_IsClass( newContentHandler, &NW_Wml1x_Epoc32ContentHandler_Class ) != NW_TRUE )
          {
          documentRoot->documentListener->DestroyPreviousDocument();
          }
        }
        }
      // further chunks
      else if( chunkIndex != -1 )
        {
        // page continues    
        status = NW_HED_DocumentNode_PartialLoadCallback( loadContext->owner, loadStatus, chunkIndex, 
          urlRequest, response, transactionId, loadContext->clientData );
        // NW_HED_DocumentNode_PartialLoadCallback takes the ownership of response
        response = NULL;

        _NW_THROW_ON_ERROR( status );      
        }
      // last chunk
      else
        {
        NW_Bool isInHistoryStack;
        // remove the loadContext from the list
        TBrowserStatusCode tempStatus = NW_ADT_DynamicVector_RemoveAt( documentRoot->loadList, index );
        NW_ASSERT( tempStatus == KBrsrSuccess );

        // Report the successful handling of the resource.
        if (loadContext->loadObserver)
          {
          if (response && (response->method == NW_URL_METHOD_HEAD))
            {
            loadContext->loadObserver->HeadCompleted(transactionId, *response, 
                *urlRequest, clientData);
            }
          else
            {
            loadContext->loadObserver->LoadCompleted(BRSR_STAT_CLASS_GENERAL, KBrsrSuccess, 
                transactionId, *urlRequest, clientData);
            }
          }

        // Check to see if urlRequest is in the HistoryStack now.  This is
        // necessary because a WML card could indicate new context in which
        // case the HistoryStack will be cleared.  If it's ever in the stack
        // then the stack owns it; otherwise, this method owns it and needs
        // to deallocate it when done with it. Only top-level-requests are
        // added to the HistoryStack so only those have to be checked against
        // the stack.
        if( NW_HED_UrlRequest_IsTopLevelRequest( urlRequest ) )
          {
          isInHistoryStack = NW_HED_HistoryStack_IsUrlinHistory( documentRoot->historyStack, urlRequest );
          }
        else
          {
          isInHistoryStack = NW_FALSE;
          }

          // final chunk. it does not have any body though.
          status = NW_HED_DocumentNode_PartialLoadCallback( loadContext->owner, loadStatus, chunkIndex, 
            urlRequest, response, transactionId, loadContext->clientData );
          // NW_HED_DocumentNode_PartialLoadCallback takes the ownership of response
          response = NULL;

          _NW_THROW_ON_ERROR( status );      

        // urlrequest is no longer needed and may need to be deleted. It gets
        // deleted if it's not, or was never, in the HistoryStack.
        if( !isInHistoryStack )
          { 
          NW_Object_Delete( urlRequest );
          }
        // free the loadContext 
        NW_Mem_Free( loadContext );
        loadContext = NULL;
        }
      }
    else
      {
      // no associated entry found
      NW_THROW_STATUS( status, KBrsrFailure );
      }
    }
  NW_CATCH( status ) 
    { 
    NW_Int16 errorClass = BRSR_STAT_CLASS_GENERAL;

    // Report the failed handling of the resource to the load observer.
    if (loadContext && loadContext->loadObserver)
      {
      TBrowserStatusCode stat;
      NW_Int16 statClass;

      if( (loadStatus == KBrsrHttpStatus) && (response != NULL) )
        {
        stat = response->httpStatus;
        statClass = BRSR_STAT_CLASS_HTTP;
        }
      else
        {
        stat = status;
        statClass = BRSR_STAT_CLASS_GENERAL;
        }

      loadContext->loadObserver->LoadCompleted(statClass, stat, transactionId, *urlRequest, clientData);
      }

    // any chunk can fail and any chunk can cancel itself by failing.
    NW_LOG4( NW_LOG_LEVEL2, "_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad Chunk failed  trans=%d chunkIndex=%d status=%d, loadStatus=%d", 
     transactionId, chunkIndex, status, loadStatus );
  
    // in case of top level request. 
    if( NW_HED_UrlRequest_IsTopLevelRequest( urlRequest ) == NW_TRUE  )
      {
      // handle http errors
      if( loadStatus != KBrsrSuccess )
        {
        failedStatus = loadStatus;

        // all http errors (except cancel)
        // remove current top level trans from the load list as HandlerError puts a cancellAll call on 
        // all transactions in this list.
        NW_HED_Loader_RemoveTransFromLoadList( documentRoot->loader, transactionId );
        // report top level http error

        if( loadStatus == KBrsrHttpStatus && response != NULL )
          {
          // set loadStatus to the actual error number.
          loadStatus = response->httpStatus;
          errorClass = BRSR_STAT_CLASS_HTTP;
          }
        if (loadContext && loadContext->owner)
            {
            NW_HED_DocumentNode_HandleError( loadContext->owner, urlRequest, errorClass, (NW_WaeError_t)loadStatus );
            }
        }
      // non-http errors
      else if( status != KBrsrSuccess )
        {
        failedStatus = status;

        NW_LOG0( NW_LOG_LEVEL2, "_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad non http error" );
        // We need to cancel any top level transaction that fails due to bad content etc
        // the cancel reason must be KBrsrFailure so that when the cancel comes back from 
        // stack we know that the cancel has been initiated by us (we need to differentiate the cancel from
        // network or the user cancel)

        // status must be set here 'cause if this is the only oustanding request, then the cancel 
        // calls NW_HED_ILoaderListener_IsLoading( FALSE ) at loader.c which ends up calling handleError with ok status.
        // if we don't set error status here, then HandleError does not get it.
        // comment continues right below
        documentRoot->documentErrorClass = BRSR_STAT_CLASS_GENERAL;
        documentRoot->documentError = (NW_WaeError_t)status;

        NW_HED_Loader_CancelLoad( documentRoot->loader, transactionId, KBrsrFailure );
        // part2: if there are outstanding request then let handleError take care of the rest.
        if( documentRoot->isLoading == NW_TRUE )
          {
          if (loadContext && loadContext->owner)
              {
              // report top level error
              NW_HED_DocumentNode_HandleError( loadContext->owner, urlRequest, BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t)status );
              }
          }
        // set status to KBrsrCancelled so that the stack can be aware that this
        // transaction has already been cancelled.
        status = KBrsrCancelled;
        }
      // The new content handler has to be initialized even if it fails at some point no matter whether
      // the failing is due to a user cancel or an internal error (for example: bad content).
      // However, if the new content handler fails on the very first chunk, then we take the
      // previous top level content handler back, and that one does not need any initialization.
      // If link is broken case happens then we don't even create a new content handler so
      // there is nothing to initialize.  The content handler is passed the failedStatus and it can
      // decide for itself whether to create the partial document or not.
      if( chunkIndex != 0 && documentRoot->childNode )
        {
        NW_HED_ContentHandler_t* topLevelContentHander = NW_HED_ContentHandlerOf( documentRoot->childNode );
        if( topLevelContentHander->initialized == NW_FALSE )
          {
          TBrowserStatusCode initStatus;
          initStatus = NW_HED_DocumentNode_Initialize( documentRoot->childNode, failedStatus );
          NW_LOG0( NW_LOG_LEVEL2, "_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad Initialization passed" );
          if ( initStatus != KBrsrSuccess && initStatus != KBrsrWmlbrowserBadContent)
              {
              NW_HED_DocumentRoot_RestorePrev( documentRoot, BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t)initStatus );
              }
          }
        }
      }
    // non top level errors
    else
      {
      NW_LOG0( NW_LOG_LEVEL2, "_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad non top level error" );
      // handle http errors
      if( loadStatus != KBrsrSuccess )
        {
        // report top level http error

        if( loadStatus == KBrsrHttpStatus && response != NULL )
          {
          // set loadStatus to the actual error number.
          loadStatus = response->httpStatus;
          errorClass = BRSR_STAT_CLASS_HTTP;
          }
        // notify the owner of the embedded load complete
        if (loadContext && loadContext->owner)
            {
            NW_HED_DocumentNode_HandleLoadComplete( loadContext->owner, urlRequest, errorClass, loadStatus,
              transactionId, loadContext->clientData );
            }
        }
      // non-http errors
      else 
        {
        // notify the owner of the embedded load complete
        if (loadContext && loadContext->owner)
            {
            NW_HED_DocumentNode_HandleLoadComplete( loadContext->owner, urlRequest, BRSR_STAT_CLASS_GENERAL, status, 
              transactionId, loadContext->clientData );
            }
        // no need either to report the error or to cancel transactions.
        // ZAL: comment: http ok status failed ->cancel trans
        if( chunkIndex != -1  && status != KBrsrSuccess )
          {
          NW_HED_Loader_CancelLoad( documentRoot->loader, transactionId, KBrsrFailure );
          // set status to KBrsrCancelled so that the stack can be aware of that this
          // transaction has already been cancelled.
          status = KBrsrCancelled;
          }
        else
          {
          status = KBrsrSuccess ;
          }
        }
      }
    index = NW_ADT_Vector_GetElementIndex( documentRoot->loadList, &loadContext );
    // delete both request and response
    if( index != NW_ADT_Vector_AtEnd )
      {
      // pop the request off the docroot loadlist
      TBrowserStatusCode tempStatus = NW_ADT_DynamicVector_RemoveAt( documentRoot->loadList, index );
      NW_ASSERT( tempStatus == KBrsrSuccess );

      // In other cases UrlRequest is deleted by the HistoryEntry
      if( NW_HED_HistoryStack_IsUrlinHistory( documentRoot->historyStack, urlRequest ) != NW_TRUE ) 
        {
        NW_Object_Delete( urlRequest );
        }
      }
    // release response. note that a content handler might have released the
    // response structure but then it must be NULL at this point.
    UrlLoader_UrlResponseDelete( response );
    // free the loadContext
    if (loadContext!=NULL)
        {
        NW_Mem_Free( loadContext );
        loadContext = NULL;
        }
    }
  NW_FINALLY 
    {
    NW_LOG4( NW_LOG_LEVEL2, "_NW_HED_DocumentRoot_ILoadRecipient_ProcessPartialLoad END trans=%d chunkIndex=%d status=%d, loadStatus=%d", 
      transactionId, chunkIndex, status, loadStatus );
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   ILoaderListener implementation
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_ILoaderListener_IsLoading (NW_HED_ILoaderListener_t* loaderListener,
                                                NW_HED_Loader_t* loader,
                                                NW_Bool state)
{
  NW_HED_DocumentRoot_t* thisObj;

  NW_REQUIRED_PARAM(loader);

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (loaderListener,
                                     &NW_HED_ILoaderListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (loader, &NW_HED_Loader_Class));

  /* obtain the implementer */
  thisObj = (NW_HED_DocumentRoot_t*)NW_Object_Interface_GetImplementer (loaderListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  if (state == NW_TRUE) {
    /* notify the MHEDDocumentListener */
    if (thisObj->isLoading == NW_FALSE && thisObj->documentListener != NULL) {
      (void)thisObj->documentListener->IsActive (thisObj, state);
      NW_HED_DocumentRoot_HandlePageLoadStarted (thisObj);
    }
  }
  else {
    /* notify the MHEDDocumentListener */
    if (thisObj->isLoading == NW_TRUE && thisObj->documentListener != NULL) {
      (void)thisObj->documentListener->IsActive (thisObj, state);
      NW_HED_DocumentRoot_HandlePageLoadCompleted (thisObj);
      /* Be careful adding code after this line, since we may already exit 
         the browser */
    }
  }
}

/* ------------------------------------------------------------------------- *
   NW_HED_DocumentRoot methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

void*
NW_HED_DocumentRoot_GetCertInfo (NW_HED_DocumentRoot_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
  return thisObj->certInfo;
}


/* ------------------------------------------------------------------------- */
const NW_Text_t*
NW_HED_DocumentRoot_GetTitle (NW_HED_DocumentRoot_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* the DocumentRoot does not store a title - it always returns it's child
     title (if there is one) */
  if (thisObj->childNode != NULL && thisObj->documentError != KBrsrWmlbrowserBadContent) {
    return NW_HED_ContentHandler_GetTitle (thisObj->childNode);
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
const NW_Text_t*
NW_HED_DocumentRoot_GetDocumentReferrer(NW_HED_DocumentRoot_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  return thisObj->referrerUrl;
}


void
NW_HED_DocumentRoot_SetDocumentReferrer(NW_HED_DocumentRoot_t* thisObj, const NW_Text_t* referrerUrl)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  thisObj->referrerUrl = (NW_Text_t*)referrerUrl;
}



TBrowserStatusCode
NW_HED_DocumentRoot_StartRequest_tId2( NW_HED_DocumentRoot_t* thisObj,
                                       const NW_HED_UrlRequest_t* aUrlRequest,
                                       void* aLoadRecipient,
                                       void* aClientData,
                                       NW_Uint16* aTransactionId, 
                                       void* aLoadContext )
    {
    TBrowserStatusCode status;

    status =
      NW_HED_Loader_StartRequest( thisObj->loader, 
                                  aUrlRequest, 
                                  (NW_HED_ILoadRecipient_t*) aLoadRecipient,
				                  aClientData, aTransactionId, aLoadContext );

    return status;
    }

/* ------------------------------------------------------------------------- */
/* upon success the ownership of urlRequest is passed to this method         */
TBrowserStatusCode
NW_HED_DocumentRoot_StartRequest_tId (NW_HED_DocumentRoot_t* thisObj,
                                  NW_HED_DocumentNode_t* owner,
                                  const NW_HED_UrlRequest_t* urlRequest,
                                  void* clientData , NW_Uint16* aTransactionId,
                                  void* aLoadContext)
{
  NW_HED_DocumentRoot_LoadContext_t* loadContext = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (owner, &NW_HED_DocumentNode_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (urlRequest, &NW_HED_UrlRequest_Class));

  if (urlRequest && urlRequest->url)
    NW_LOG1( NW_LOG_OFF, "NW_HED_DocumentRoot_StartRequest_tId START: %s", urlRequest->url->storage );

  NW_TRY (status) {
    NW_HED_ILoadRecipient_t* loadRecipient;
    void** entry;

    /* terminate all outstanding load requests */
    if (NW_HED_UrlRequest_IsTopLevelRequest (urlRequest)) {
      NW_LOG0( NW_LOG_LEVEL2, "Performance measurement: top level load is requested" );
      (void) NW_HED_DocumentNode_Cancel (thisObj, NW_HED_CancelType_NewRequest);
      thisObj->isTopLevelRequest = NW_TRUE;
    }
    else {
      thisObj->isTopLevelRequest = NW_FALSE;
    }

    /* switch from image map view if we need to */
    thisObj->appServices->browserAppApi.SwitchFromImgMapViewIfNeeded();

    /* if a cancel is in progress */
    if (thisObj->isCancelling == NW_TRUE) {
    
      /* if the new request is a top level request cache it so that we can load
         it after the cancel is over provided it is not already cached */
      if ((NW_HED_UrlRequest_IsTopLevelRequest (urlRequest)) &&
          (thisObj->nextUrlRequestToLoad != urlRequest)) {
      
        /* delete the old request if not historic */
        if ((thisObj->nextUrlRequestToLoad != NULL) &&
            (NW_HED_UrlRequest_IsHistoricRequest (thisObj->nextUrlRequestToLoad) == NW_FALSE)) {
  
          NW_Object_Delete (thisObj->nextUrlRequestToLoad);
          thisObj->nextUrlRequestToLoad = NULL;
        }
      
        /* cache the top level request */
        thisObj->nextUrlRequestToLoad = NW_HED_UrlRequestOf (urlRequest);
        NW_THROW_SUCCESS (status);
      }

      /* if not a top level request delete the request as we do not cache it */
      else if (NW_HED_UrlRequest_IsTopLevelRequest (urlRequest) != NW_TRUE) {
        NW_Object_Delete (NW_HED_UrlRequestOf (urlRequest));
        NW_THROW_SUCCESS (status);
      }
    
      /* this is a top level request already in the cache */
      NW_THROW_SUCCESS (status);
    }  
    /* let the current content handler try to handle the request first */
    if ((thisObj->childNode != NULL) &&
        (NW_HED_ContentHandler_HandleRequest (thisObj->childNode,
					      (NW_HED_UrlRequest_t*) urlRequest) == KBrsrSuccess)) {
      NW_THROW_SUCCESS (status);
    }

    /* allocate and initialize the LoadContext */
    loadContext = (NW_HED_DocumentRoot_LoadContext_t*)
      NW_Mem_Malloc (sizeof (*loadContext));
    NW_THROW_OOM_ON_NULL (loadContext, status);
    loadContext->owner = owner;
    loadContext->loadObserver = NULL;
    loadContext->clientData = clientData;
    loadContext->virtualRequest = NW_FALSE;

    /* submit the load request */
    loadRecipient = (NW_HED_ILoadRecipient_t*)
      NW_Object_QueryInterface (thisObj, &NW_HED_ILoadRecipient_Class);
    status =
      NW_HED_Loader_StartRequest (thisObj->loader, urlRequest, loadRecipient,
				  loadContext, &loadContext->transactionId, aLoadContext );
    if( status == KBrsrCancelled )
      {
      // status == KBrsrCancelled means virtual request
      loadContext->virtualRequest = NW_TRUE;
      //pay attention on changing the status value TSW id: MVIA-72EJ6A
      status = KBrsrSuccess;
      }
    _NW_THROW_ON_ERROR (status);

    /* append the loadContext to our list of outstanding loads */
    entry = NW_ADT_DynamicVector_InsertAt (thisObj->loadList, &loadContext,
                                           NW_ADT_Vector_AtEnd);    
    /*NW_ASSERT (entry != NULL);*/
    if (entry == NULL) {      
      status =
        NW_HED_Loader_DeleteLoad (thisObj->loader, loadContext->transactionId,NULL);
      _NW_THROW_ON_ERROR( status );
    }
    if(aTransactionId )
      {
      *aTransactionId = loadContext->transactionId;
      }

  }

  NW_CATCH (status) {
    NW_Mem_Free (loadContext);
    loadContext = NULL;
  }

  NW_FINALLY {
    NW_LOG0( NW_LOG_OFF, "NW_HED_DocumentRoot_StartRequest_tId End" );
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
/* upon success the ownership of urlRequest is passed to this method         */
TBrowserStatusCode
NW_HED_DocumentRoot_StartRequest (NW_HED_DocumentRoot_t* thisObj,
                                  NW_HED_DocumentNode_t* owner,
                                  const NW_HED_UrlRequest_t* urlRequest,
                                  void* clientData)
  {
  return NW_HED_DocumentRoot_StartRequest_tId(thisObj,owner,urlRequest,
                                           clientData , NULL);
  }
/* ------------------------------------------------------------------------- */
/* The caller retains ownership of url                                       */
TBrowserStatusCode
NW_HED_DocumentRoot_StartLoad (NW_HED_DocumentRoot_t *thisObj,
                               NW_HED_DocumentNode_t *owner,
                               const NW_Text_t       *url,
                               NW_Uint8              reason,
                               void                  *clientData,
                               NW_Uint8 loadType,
                               NW_Cache_Mode_t cacheMode)
  {
  NW_HED_UrlRequest_t* urlRequest = NULL;

  NW_LOG0( NW_LOG_OFF, "NW_HED_DocumentRoot_StartLoad START" );

  NW_TRY (status) {
    /* instantiate the url request structure */
    urlRequest =
      NW_HED_UrlRequest_New ((NW_Text_t*) url, NW_URL_METHOD_GET, NULL,
			     NULL, reason, NW_HED_UrlRequest_LoadNormal, loadType );
    NW_THROW_OOM_ON_NULL (urlRequest, status);

    NW_HED_UrlRequest_SetCacheMode (urlRequest, cacheMode);
		
	/* issue the load request */
    status = NW_HED_DocumentRoot_StartRequest (thisObj, owner, urlRequest,
                                               clientData );
    _NW_THROW_ON_ERROR (status);
  }

  NW_CATCH (status) {
    NW_Object_Delete (urlRequest);
  }

  NW_FINALLY {
    NW_LOG1( NW_LOG_OFF, "NW_HED_DocumentRoot_StartLoad END, status: %x", status );
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_HistoryLoad (NW_HED_DocumentRoot_t           *thisObj,
                                 NW_HED_HistoryStack_Direction_t direction)
{
  TBrowserStatusCode status;
  const NW_HED_UrlRequest_t   *urlRequest;
  const NW_HED_HistoryEntry_t *entry;
  NW_Cache_Mode_t             cacheMode = NW_CACHE_NORMAL;
  NW_Uint8                    reason;
  NW_Uint8                    method;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* get the url in the given direction */
  entry = NW_HED_HistoryStack_GetEntry (thisObj->historyStack, direction);
  if (entry != NULL)
      {
      if (!NW_HED_HistoryStack_ValidateEntry(entry))
          {
          entry = NULL;
          }
      }
  if (entry == NULL) {
    switch(direction) {
      case NW_HED_HistoryStack_Direction_Current:
        status = KBrsrHedNoCurrentHistEntry;
        break;

      case NW_HED_HistoryStack_Direction_Previous:
        status = KBrsrHedNoPreviousHistEntry;
        break;

      case NW_HED_HistoryStack_Direction_Next:
        status = KBrsrHedNoNextHistEntry;
        break;

      default:
        status = KBrsrFailure;
    }

    return status;
  }

  urlRequest = NW_HED_HistoryEntry_GetUrlRequest (entry);
  NW_ASSERT (urlRequest);

  method = NW_HED_UrlRequest_GetMethod(urlRequest);
  if (method == NW_URL_METHOD_POST)
  {
	 if ((status = thisObj->appServices->securityNotes.AboutToLoadPage(ERepostConfirmation)) != KBrsrSuccess)
    {
      return status;
    }
  }

  /* set the reason and cache mode */
  if (direction == NW_HED_HistoryStack_Direction_Previous) {
    reason = NW_HED_UrlRequest_Reason_ShellPrev;
    cacheMode = NW_CACHE_HISTPREV;
  }
  else if (direction == NW_HED_HistoryStack_Direction_Next) {
    reason = NW_HED_UrlRequest_Reason_ShellNext;
  }
  else if (direction == NW_HED_HistoryStack_Direction_Current) {
    reason = NW_HED_UrlRequest_Reason_ShellReload;
    cacheMode = NW_CACHE_NOCACHE;
  }
  else if (direction == NW_HED_HistoryStack_Direction_RestorePrevious) {
    reason = NW_HED_UrlRequest_Reason_RestorePrev;
  }
  else if (direction == NW_HED_HistoryStack_Direction_RestoreNext) {
    reason = NW_HED_UrlRequest_Reason_RestoreNext;
  }
  else {
    NW_ASSERT (0); /* this is an error */
    reason = NW_HED_UrlRequest_Reason_Undefined;
  }

  if( NW_HED_UrlRequest_GetMethod ((NW_HED_UrlRequest_t*) urlRequest) == NW_URL_METHOD_DATA )
      {
      NW_HED_UrlRequest_SetMethod ((NW_HED_UrlRequest_t*) urlRequest, NW_URL_METHOD_GET);
      }
  NW_HED_UrlRequest_SetReason ((NW_HED_UrlRequest_t*) urlRequest, reason);
  NW_HED_UrlRequest_SetCacheMode ((NW_HED_UrlRequest_t*) urlRequest,
				  cacheMode);
  /* update the history to point to the new current entry.  It is
     important that this is done BEFORE StartRequest -- to support
     intra-doc navigation. */
  NW_HED_HistoryStack_SetCurrent(thisObj->historyStack, direction);
  
  /* load the url */
  status =
    NW_HED_DocumentRoot_StartRequest (thisObj, NW_HED_DocumentNodeOf (thisObj),
                                      urlRequest, NULL );

  /* event log function, Back or Forward */
  if (status == KBrsrSuccess) {
    if (direction == NW_HED_HistoryStack_Direction_Previous) {
      NW_ASSERT(thisObj->appServices != NULL);
      NW_EvLog_Log(&(thisObj->appServices->evLogApi), NW_EvLog_HISTORY_FORWARD,
		   NULL);
    }
    else if (direction == NW_HED_HistoryStack_Direction_Next) {
      NW_ASSERT(thisObj->appServices != NULL);
      NW_EvLog_Log(&(thisObj->appServices->evLogApi), NW_EvLog_HISTORY_BACK,
		   NULL);
    }
  }
  
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_SetLoadObserver (NW_HED_DocumentRoot_t* thisObj,
                                     NW_Uint16 aTransationId,
                                     MHEDLoadObserver* aLoadObserver)
  {
  NW_HED_DocumentRoot_LoadContext_t* loadContext = NULL;
  NW_ADT_Vector_Metric_t i;
  NW_ADT_Vector_Metric_t size;

  size = NW_ADT_Vector_GetSize(thisObj->loadList);
  for (i = 0; i < size; i++) 
    {
    TBrowserStatusCode  status;

    loadContext = NULL;
    status = NW_ADT_Vector_GetElementAt(thisObj->loadList, i, &loadContext);

    if ((status == KBrsrSuccess) && (loadContext->transactionId == aTransationId)) 
      {
      loadContext->loadObserver = aLoadObserver;
      return KBrsrSuccess;
      }
    }

  return KBrsrNotFound;
  }

/* ------------------------------------------------------------------------- */
void
_NW_HED_DocumentRoot_DeleteRequests (NW_HED_DocumentRoot_t* thisObj,
                                     NW_HED_DocumentNode_t* childNode)
  {
  NW_ADT_Vector_Metric_t index;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (childNode, &NW_HED_DocumentNode_Class));

  /* iterate through the load list canceling all requests belonging to the
     childNode */
  index = NW_ADT_Vector_GetSize (thisObj->loadList);
  while (index-- > 0) 
    {
    TBrowserStatusCode status;
    NW_HED_DocumentRoot_LoadContext_t* loadContext;

    loadContext = NULL;
    status = NW_ADT_Vector_GetElementAt (thisObj->loadList, index, &loadContext);
    NW_ASSERT (status == KBrsrSuccess);
    if (loadContext->owner == childNode) 
      {
      /* cancel the load */
      status =
        NW_HED_Loader_DeleteLoad (thisObj->loader, loadContext->transactionId,
                                  NULL);
      if(status == KBrsrSuccess)
        {
        /* remove the entry from the loadList */
        status = NW_ADT_DynamicVector_RemoveAt (thisObj->loadList, index);
        NW_ASSERT (status == KBrsrSuccess);

        /* free the load context */
        NW_Mem_Free (loadContext);
        loadContext = NULL;
        }
      }
    }
  }

/* ------------------------------------------------------------------------- */
const NW_Text_t*
NW_HED_DocumentRoot_GetURL (NW_HED_DocumentRoot_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  if (thisObj->childNode != NULL) {
    return NW_HED_ContentHandler_GetURL (thisObj->childNode);
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
const NW_HED_UrlRequest_t*
NW_HED_DocumentRoot_GetUrlRequest (NW_HED_DocumentRoot_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  if (thisObj->childNode != NULL) {
    return NW_HED_ContentHandler_GetUrlRequest (thisObj->childNode);
  }
  return NULL;
}

/* -------------------------------------------------------------------------
   If *contextId is zero the documentRoot pick a value that is unused at the
   time of the call (starting at the lowest possible number greater than
   100), otherwise the given contextId is used.
   ---------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_AddContext (NW_HED_DocumentRoot_t* thisObj,
                                NW_HED_Context_t* context,
                                NW_Int8 *contextId)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));
  NW_ASSERT (context != NULL);
  NW_ASSERT (contextId != NULL);

  /* if need be pick an id */
  if (contextId == NW_HED_DocumentRoot_ContextPickId) {
    NW_Int8 id = 99; /* TODO change this to a define and move with
                        other context-id defs */

    do {
      id++;
      context = NW_HED_DocumentRoot_GetContext (thisObj, id);
    } while (context != NULL);

    *contextId = id;
  }

  /* add the context */
  return NW_ADT_Map_Set (thisObj->contextMap, contextId, &context);
}

/* -------------------------------------------------------------------------
   Removes the context from the document.  Note the context is not freed
   by this method.
   ---------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_RemoveContext (NW_HED_DocumentRoot_t* thisObj,
                                   NW_Int8 contextId)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  return NW_ADT_Map_Remove (thisObj->contextMap, &contextId);
}

/* -------------------------------------------------------------------------
   Returns the context of the given contextId
   ---------------------------------------------------------------------- */
NW_HED_Context_t*
NW_HED_DocumentRoot_GetContext (NW_HED_DocumentRoot_t* thisObj,
                                NW_Int8 contextId)
{
  NW_HED_Context_t* context;
  void**  valuePtr;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* attempt to lookup the child ContentHandler for the key fetch a
     pointer to the value from the map Returns NULL if item doesn't
     exist. */
  valuePtr = NW_ADT_Map_GetUnsafe (thisObj->contextMap, &contextId);

  /* use byte-wise copy to get the unaligned data at valuePtr into the
     context pointer */
  if (valuePtr != NULL)
  {
    (void) NW_Mem_memcpy (&context, valuePtr, sizeof context);
  }
  else
  {
    /* no context exists for the given key */
    context = NULL;
  }
  return context;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_SetDocumentListener (NW_HED_DocumentRoot_t* thisObj,
                                         MHEDDocumentListener* documentListener)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  thisObj->documentListener = documentListener;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_HandleIntraPageNavigationStarted (NW_HED_DocumentRoot_t* thisObj)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* reset the documentStatus and set the loading flag */
  thisObj->documentErrorClass = BRSR_STAT_CLASS_NONE;
  thisObj->documentError = KBrsrSuccess;
  thisObj->isIntraPageNav++;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_DocumentRoot_HandleIntraPageNavigationCompleted (NW_HED_DocumentRoot_t* thisObj)
{
  NW_Int16 errorClass;
  NW_WaeError_t error;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* reset the loading flag */
  thisObj->isIntraPageNav--;

  /* reset the documentStatus and reset the loading flag */
  errorClass = thisObj->documentErrorClass;
  error = thisObj->documentError;

  thisObj->documentErrorClass = BRSR_STAT_CLASS_NONE;
  thisObj->documentError = KBrsrSuccess;

  /* if the navigation occurred with errors... */
  if (error != KBrsrSuccess) {
    (void)thisObj->documentListener->ReportError (thisObj, errorClass, error, NW_HED_DocumentRoot_RestorePrev);
  }
  
  else {
    if (thisObj->childNode != NULL) {
      (void) NW_HED_DocumentNode_IntraPageNavigationCompleted (thisObj->childNode);
    }
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_CollectNumbers (NW_HED_DocumentRoot_t  *thisObj,
                                     NW_ADT_DynamicVector_t *dynamicVector)
{
  NW_HED_TreeVisitor_t treeVisitor;

  NW_TRY (status) {
    /* initialize the tree visitor */
    status = NW_HED_TreeVisitor_Initialize (&treeVisitor, thisObj);
    NW_THROW_ON_ERROR (status);

    /* iterate over the document tree */
    while (NW_HED_TreeVisitor_HasNext (&treeVisitor)) {
      NW_HED_DocumentNode_t* documentNode;
      NW_HED_INumberCollector_t* numberCollector;
    
      /* get the next document node in the tree */
      (void) NW_HED_TreeVisitor_GetNext (&treeVisitor, &documentNode);

      /* does it implement the INumberCollector interface? */
      numberCollector = (NW_HED_INumberCollector_t*)
        NW_Object_QueryInterface (documentNode, &NW_HED_INumberCollector_Class);
      if (numberCollector == NULL) {
        continue;
      }

      /* invoke the collect method on the interface implementation */
      status =
	NW_HED_INumberCollector_Collect (numberCollector, dynamicVector);
      NW_THROW_ON_ERROR (status);
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    NW_Object_Terminate (&treeVisitor);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_HED_DocumentRoot_GetViewImageList (NW_HED_DocumentRoot_t  *thisObj,
                                       NW_ADT_DynamicVector_t *dynamicVector)
{
  NW_HED_TreeVisitor_t treeVisitor;

  NW_TRY (status) {
    /* initialize the tree visitor */
    status = NW_HED_TreeVisitor_Initialize (&treeVisitor, thisObj);
    NW_THROW_ON_ERROR (status);

    /* iterate over the document tree */
    while (NW_HED_TreeVisitor_HasNext (&treeVisitor)) 
    {
      NW_HED_DocumentNode_t* documentNode;
      NW_HED_IImageViewer_t* imageViewer;
    
      /* get the next document node in the tree */
      (void) NW_HED_TreeVisitor_GetNext (&treeVisitor, &documentNode);

      /* does it implement the IImageViewer interface? */
      imageViewer = (NW_HED_IImageViewer_t*)
        NW_Object_QueryInterface (documentNode, &NW_HED_IImageViewer_Class);
      if (imageViewer == NULL) {
        continue;
      }

      /* invoke the collect method on the interface implementation */
      status =	NW_HED_IImageViewer_GetImage (imageViewer, dynamicVector);
      NW_THROW_ON_ERROR (status);
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    NW_Object_Terminate (&treeVisitor);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_ShowImages(NW_HED_DocumentRoot_t  *thisObj)
{
  NW_HED_TreeVisitor_t treeVisitor;

  NW_TRY (status) {
    /* initialize the tree visitor */
    status = NW_HED_TreeVisitor_Initialize (&treeVisitor, thisObj);
    NW_THROW_ON_ERROR (status);

    /* iterate over the document tree */
    while (NW_HED_TreeVisitor_HasNext (&treeVisitor)) 
    {
      NW_HED_DocumentNode_t* documentNode;
      NW_HED_IImageViewer_t* imageViewer;
    
      /* get the next document node in the tree */
      (void) NW_HED_TreeVisitor_GetNext (&treeVisitor, &documentNode);

      /* does it implement the IImageViewer interface? */
      imageViewer = (NW_HED_IImageViewer_t*)
        NW_Object_QueryInterface (documentNode, &NW_HED_IImageViewer_Class);
      if (imageViewer == NULL) {
        continue;
      }

      /* invoke the collect method on the interface implementation */
      status =	NW_HED_IImageViewer_ShowImages(imageViewer);
      NW_THROW_ON_ERROR (status);
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    NW_Object_Terminate (&treeVisitor);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_HED_DocumentRoot_t*
NW_HED_DocumentRoot_New (NW_HED_MimeTable_t* mimeTable, 
                         NW_HED_AppServices_t* appServices,
                         void* browserAppCtx)
{
  return (NW_HED_DocumentRoot_t*)
    NW_Object_New (&NW_HED_DocumentRoot_Class, NULL, mimeTable, 
                   appServices, browserAppCtx);
}
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoaderListener_LoadProgressOn (NW_HED_ILoaderListener_t *loaderListener,
                                           NW_Uint16 transactionId)
  {
  NW_HED_DocumentRoot_t* thisObj;

    /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (loaderListener,
                                     &NW_HED_ILoaderListener_Class));
  
  /* obtain the implementer */
  thisObj = (NW_HED_DocumentRoot_t*)NW_Object_Interface_GetImplementer (loaderListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* notify the MHEDDocumentListener */
  if (thisObj->documentListener != NULL)
    {
     (void)thisObj->documentListener->LoadProgressOn (transactionId);
    }
  
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_ILoaderListener_LoadProgressOff (NW_HED_ILoaderListener_t *loaderListener,
                                                      NW_Uint16 transactionId)
  {
  NW_HED_DocumentRoot_t* thisObj;

    /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (loaderListener,
                                     &NW_HED_ILoaderListener_Class));
  
  /* obtain the implementer */
  thisObj = (NW_HED_DocumentRoot_t*)NW_Object_Interface_GetImplementer (loaderListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* notify the MHEDDocumentListener */
  if (thisObj->documentListener != NULL)
    {
     (void)thisObj->documentListener->LoadProgressOff (transactionId);
    }
  
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_DocumentRoot_DocumentDisplayed (NW_HED_DocumentRoot_t* thisObj)
{
  TBrowserStatusCode status = KBrsrSuccess;
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_DocumentRoot_Class));

  /* Notify the content handler that the contents have been displayed.
     This is necessary to support features like XHTML META refresh. */
  if (thisObj->childNode != NULL &&
    NW_Object_IsInstanceOf (thisObj->childNode, &NW_HED_CompositeContentHandler_Class) ) {
    status =  NW_HED_CompositeContentHandler_DocumentDisplayed (thisObj->childNode);
  }

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_DocumentRoot_IsContextSwitchPending (NW_HED_DocumentRoot_t* thisObj)
{
  // If a top-level load is in progress and it's still on the first chunk then
  // a context switch is pending.
  if (thisObj->isLoading && thisObj->isTopLevelRequest && (thisObj->chunkIndex == 0)) {
    return NW_TRUE;
  }
  // A cancel in progress on a top-level load after the first chunk implies a
  // context switch.
  if (thisObj->isCancelling && thisObj->isTopLevelRequest && (thisObj->chunkIndex != 0)) {
    return NW_TRUE;
  }
  return NW_FALSE;
}

/* ------------------------------------------------------------------------- */
void
NW_HED_DocumentRoot_SetHistoryControl(NW_HED_DocumentRoot_t* thisObj, CWmlControl* aWmlControl)
	{
  	SetOssHistoryControl( thisObj->historyStack, aWmlControl );
	}
