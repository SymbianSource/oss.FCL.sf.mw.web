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


#include "nw_hed_contenthandleri.h"

#include "nw_hed_documentrooti.h" // Should we need to include this??
#include "nw_hed_documentroot.h"
#include "nw_hed_historystack.h"
#include "nw_hed_historyentry.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_imagech_epoc32contenthandler.h"

#include "nwx_url_utils.h"
#include "BrsrStatusCodes.h"

//R
#include "urlloader_urlresponse.h"

/* ------------------------------------------------------------------------- */
static void NW_HED_ContentHandler_DestroyBoxTree(NW_HED_ContentHandler_t* thisObj)
{
  NW_LMgr_ContainerBox_t* parentBox = NULL;

  if(thisObj->boxTree == NULL) {
    return;
  }

  parentBox = NW_LMgr_Box_GetParent(thisObj->boxTree);
  
  if((parentBox != NULL) && (NW_Object_Core_GetClass(parentBox) == &NW_LMgr_RootBox_Class)) {
    NW_LMgr_RootBox_BoxTreeDestroyed(NW_LMgr_RootBoxOf(parentBox));
  }

  NW_Object_Delete (thisObj->boxTree);
  thisObj->boxTree = NULL;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_ContentHandler_Class_t NW_HED_ContentHandler_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_HED_DocumentNode_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_ContentHandler_t),
    /* construct               */ _NW_HED_ContentHandler_Construct,
    /* destruct                */ _NW_HED_ContentHandler_Destruct
  },
  { /* NW_HED_DocumentNode     */
    /* cancel                  */ _NW_HED_DocumentNode_Cancel,
    /* partialLoadCallback	   */ NULL,
    /* initialize              */ _NW_HED_ContentHandler_Initialize,
    /* nodeChanged             */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree              */ _NW_HED_ContentHandler_GetBoxTree,
    /* processEvent            */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError             */ _NW_HED_DocumentNode_HandleError,
    /* suspend                 */ _NW_HED_ContentHandler_Suspend,
    /* resume                  */ _NW_HED_DocumentNode_Resume,
    /* allLoadsCompleted       */ _NW_HED_ContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted   */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus               */ _NW_HED_ContentHandler_LoseFocus,
    /* gainFocus               */ _NW_HED_ContentHandler_GainFocus,
    /* handleLoadComplete      */ _NW_HED_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler   */
    /* partialNextChunk        */ _NW_HED_ContentHandler_PartialNextChunk,
    /* getTitle                */ NULL,
    /* getUrl                  */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl              */ _NW_HED_ContentHandler_ResolveURL,
    /* createHistoryEntry      */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse          */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree           */ NULL,
    /* handleRequest           */ _NW_HED_ContentHandler_HandleRequest,
    /* featureQuery            */ _NW_HED_ContentHandler_FeatureQuery,
    /* responseComplete        */ _NW_HED_ContentHandler_ResponseComplete
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  TBrowserStatusCode status;
  NW_HED_ContentHandler_t* thisObj;

  /* invoke our superclass constructor */
  status = _NW_HED_DocumentNode_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  /* for convenience */
  thisObj = NW_HED_ContentHandlerOf( dynamicObject );

  thisObj->initialized = NW_FALSE;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_ContentHandler_t* thisObj;

  /* for convenience */
  thisObj = NW_HED_ContentHandlerOf (dynamicObject);

  // destroy response
  UrlLoader_UrlResponseDelete( thisObj->response );

  /* delete our dynamic members */ 
  NW_HED_ContentHandler_DestroyBoxTree (thisObj);

  /* release our association with our history-entry */
  NW_HED_ContentHandler_SetAssociatedHistoryEntry (thisObj, NULL);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_PartialNextChunk( NW_HED_ContentHandler_t* thisObj,
                                         NW_Int32 chunkIndex,
                                         NW_Url_Resp_t* response,
                                         void* context)
  {
  NW_REQUIRED_PARAM(context);

  NW_TRY( status ) 
    {
    NW_ASSERT( thisObj != NULL );
    NW_ASSERT( response != NULL );

    // first chunk
    if( chunkIndex == 0 )
      {
      // save response
      thisObj->response = 
        UrlLoader_UrlResponseNew( response->uri,
                                  response->body->data, response->body->length,
                                  response->headers, response->contentTypeString,
                                  response->contentLocationString, response->noStore, response->boundary, response->lastModified,
                                  response->charset, response->charsetString, response->httpStatus,
                                  response->method, response->transId,
                                  response->contentLength);
      NW_THROW_OOM_ON_NULL( thisObj->response, status );
      // add this manually.
      thisObj->response->certInfo = response->certInfo ;

      // uri is copied
      // response->uri = NULL;
      response->body->data = NULL;
      response->body->length = 0;
      response->headers = NULL;
      response->contentTypeString = NULL;
	  response->contentLocationString = NULL;
      response->boundary = NULL;
      response->lastModified = NULL;
      response->certInfo = NULL;
      response->charsetString = NULL;
      }
    // further chunks
    else if( chunkIndex != -1 )
      {
      // append new chunk 
      status = NW_Buffer_AppendBuffers( thisObj->response->body, response->body );
      _NW_THROW_ON_ERROR( status );
      }
    else
      {
      // this function musn't be called with the last partial response as it has no body.
      // Initialize must be called instead
      NW_ASSERT( NW_TRUE );
      }
    }
  NW_CATCH( status ) 
    {
    }
  NW_FINALLY 
    {
    // free response
    UrlLoader_UrlResponseDelete( response );
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_GetBoxTree (NW_HED_DocumentNode_t* documentNode,
                                   NW_LMgr_Box_t** boxTree)
{
  NW_HED_ContentHandler_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_HED_ContentHandler_Class));

  // for convenience 
  thisObj = NW_HED_ContentHandlerOf (documentNode);

  // if we haven't created a box tree yet, do so now 
  *boxTree = NULL;
  if( !thisObj->boxTree ) 
    {
    status = NW_HED_ContentHandler_CreateBoxTree (thisObj, &thisObj->boxTree);
    }
  // return the box tree 
  *boxTree = thisObj->boxTree;
  return status;
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_HED_ContentHandler_Initialize( NW_HED_DocumentNode_t* documentNode,
  TBrowserStatusCode aInitStatus )
  {
  NW_HED_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT( NW_Object_IsInstanceOf( documentNode, &NW_HED_ContentHandler_Class ) );
  
  /* for convenience */
  thisObj = NW_HED_ContentHandlerOf( documentNode );

  thisObj->initialized = NW_TRUE;

  // invoke our superclass method
  return( NW_HED_DocumentNode_Class.NW_HED_DocumentNode.initialize(
      documentNode, aInitStatus ) );
  }

/* ------------------------------------------------------------------------- */
void
_NW_HED_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, 
                                NW_Bool aDestroyBoxTree)
{
  NW_HED_ContentHandler_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_HED_ContentHandler_Class));

  /* invoke our superclass method */
  _NW_HED_DocumentNode_Suspend (documentNode, aDestroyBoxTree);

  /* for convenience */
  thisObj = NW_HED_ContentHandlerOf (documentNode);

  /* delete our dynamic members */ 
  if (aDestroyBoxTree)
      {
      NW_HED_ContentHandler_DestroyBoxTree (thisObj);
      }
	  	
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_ContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) documentNode;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_HED_ContentHandler_Class));
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_ContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_ContentHandler_t* thisObj = NULL;
  NW_LMgr_RootBox_t* rootBox;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_HED_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_HED_ContentHandlerOf (documentNode);

  /* there is nothing to do if there is no box tree */
  if (thisObj->boxTree == NULL) {
    return;
  }

  rootBox = NW_LMgr_Box_GetRootBox (thisObj->boxTree);
  if (rootBox != NULL) {
    NW_LMgr_RootBox_LoseFocus (rootBox);
  }
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_ContentHandler_t* thisObj = NULL;
  NW_LMgr_RootBox_t* rootBox;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode, &NW_HED_ContentHandler_Class));

  /* for convenience */
  thisObj = NW_HED_ContentHandlerOf (documentNode);

  /* there is nothing to do if there is no box tree */
  if (thisObj->boxTree == NULL) {
    return;
  }

  rootBox = NW_LMgr_Box_GetRootBox (thisObj->boxTree);  
  if (rootBox != NULL) {
    NW_LMgr_RootBox_GainFocus (rootBox);
  }
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
_NW_HED_ContentHandler_CreateHistoryEntry (NW_HED_ContentHandler_t* thisObj, 
                                           NW_HED_UrlRequest_t* urlRequest, 
                                           NW_HED_UrlResponse_t* urlResponse)
{
  (void) thisObj;

  return NW_HED_HistoryEntry_New (urlRequest, urlResponse);
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
_NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry (NW_HED_ContentHandler_t* thisObj,
                                                        const NW_Text_t* requestUrl,
                                                        const NW_Text_t* responseUrl)
{
  NW_HED_DocumentRoot_t* docRoot = NULL;
  NW_HED_HistoryStack_t* history = NULL;
  NW_HED_HistoryEntry_t* entry = NULL;
  NW_HED_UrlRequest_t* urlReq = NULL;
  NW_HED_UrlResponse_t *urlResp = NULL;
  NW_HED_HistoryEntry_t* newEntry = NULL;
  const NW_Text_t* title;
  NW_Ucs2* storage = NULL;
  static const NW_Ucs2 empty[] = { '\0' };
  static const NW_Ucs2 space[] = { ' ', '\0' };

  /* get current entry */
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
  NW_THROWIF_NULL (docRoot);
  NW_THROWIF_NULL (history = NW_HED_DocumentRoot_GetHistoryStack (docRoot));
  NW_THROWIF_NULL (entry = NW_HED_HistoryStack_GetEntry (history, 
      NW_HED_HistoryStack_Direction_Current));

  /* clone the entry */
  NW_THROWIF_NULL (newEntry = NW_HED_HistoryEntry_Clone (entry));

  /* set the new urls */
  NW_THROWIF_NULL (urlReq = (NW_HED_UrlRequest_t*) NW_HED_HistoryEntry_GetUrlRequest (newEntry));
  NW_THROWIF_ERROR (NW_HED_UrlRequest_UpdateUrl (urlReq, requestUrl));

  NW_THROWIF_NULL (urlResp = (NW_HED_UrlResponse_t*) NW_HED_HistoryEntry_GetUrlResponse (newEntry));
  NW_THROWIF_ERROR (NW_HED_UrlResponse_UpdateUrl (urlResp, responseUrl));

  /* Save the title of page to history */
  title = NW_HED_DocumentRoot_GetTitle (docRoot);

  if (title != NULL)
    {
    NW_Uint8 freeNeeded;
    NW_Text_Length_t length;
    
    storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (title, 0, &length, &freeNeeded);
    if (!freeNeeded)
      {
      storage = NW_Str_Newcpy (storage);
      }
    if(storage == NULL)
      {
      storage = (NW_Ucs2*) empty;
      }

    NW_HED_HistoryEntry_SetPageTitle(newEntry, storage);
    
    if (storage != empty)
      {
      NW_Mem_Free (storage);
      storage=NULL;
      }
    }
  /* No title, let's leave it empty */
  else
    {
    NW_HED_HistoryEntry_SetPageTitle(newEntry, (NW_Ucs2*) space);
    }


  return newEntry;

NW_CATCH_ERROR
  NW_Object_Delete (newEntry);
  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_HED_UrlResponse_t*
_NW_HED_ContentHandler_NewUrlResponse (NW_HED_ContentHandler_t* thisObj,
                                       NW_Url_Resp_t* response)
{
  (void) thisObj;

  return NW_HED_UrlResponse_New (response);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_HandleRequest (NW_HED_ContentHandler_t* thisObj,
                                      NW_HED_UrlRequest_t* urlRequest)
{
  (void) thisObj;
  (void) urlRequest;

  /* simply return KBrsrFailure, meaning that we couldn't act on the request */
  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_FeatureQuery (NW_HED_ContentHandler_t* thisObj,
                                     NW_HED_ContentHandler_Feature_t featureId)
{
  (void) thisObj;
  (void) featureId;

  /* simply return KBrsrFailure, meaning that no support for the feature */
  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_t*
_NW_HED_ContentHandler_GetURL (NW_HED_ContentHandler_t* thisObj)
{
  //lint --e{550} Warning -- Symbol 'status' not accessed
  const NW_HED_UrlResponse_t* urlResponse;
  const NW_Text_t* docUrl = NULL;
  urlResponse = NW_HED_ContentHandler_GetUrlResponse (thisObj);

  if (urlResponse != NULL) {
    docUrl = NW_HED_UrlResponse_GetUrl (urlResponse);
  }

    return docUrl;
}

/* ------------------------------------------------------------------------- */
const NW_HED_UrlRequest_t*
_NW_HED_ContentHandler_GetUrlRequest(NW_HED_ContentHandler_t* thisObj)
{
  const NW_HED_UrlRequest_t* urlReq = NULL;
  NW_ASSERT(thisObj);

  if (thisObj->associatedHistoryEntry != NULL) {
    urlReq = NW_HED_HistoryEntry_GetUrlRequest (thisObj->associatedHistoryEntry);
  }

  return urlReq;
}

/* ------------------------------------------------------------------------- */
const NW_HED_UrlResponse_t*
_NW_HED_ContentHandler_GetUrlResponse(NW_HED_ContentHandler_t* thisObj)
{
  const NW_HED_UrlResponse_t* urlResp = NULL;
  NW_ASSERT(thisObj);

  if (thisObj->associatedHistoryEntry != NULL) {
    urlResp = NW_HED_HistoryEntry_GetUrlResponse (thisObj->associatedHistoryEntry);
  }

  return urlResp;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_SetAssociatedHistoryEntry (NW_HED_ContentHandler_t* thisObj, 
                                                 NW_HED_HistoryEntry_t* entry)
{
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_ContentHandler_Class));

  /* we don't need to do anything if associate the same entry */
  if (thisObj->associatedHistoryEntry == entry) {
    return KBrsrSuccess;
  }

  /* release the old history-entry */
  if (thisObj->associatedHistoryEntry != NULL) {
    NW_HED_HistoryEntry_Release (thisObj->associatedHistoryEntry);
    thisObj->associatedHistoryEntry = NULL;
  }

  /* reserve and hold on to the new history entry */
  if (entry != NULL) {
    status = NW_HED_HistoryEntry_Reserve (entry);

    if (status == KBrsrSuccess) {
      thisObj->associatedHistoryEntry = entry;
    }
  }

  return status;
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
_NW_HED_ContentHandler_GetAssociatedHistoryEntry (NW_HED_ContentHandler_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_ContentHandler_Class));

  return thisObj->associatedHistoryEntry;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContentHandler_ResolveURL (NW_HED_ContentHandler_t* thisObj,
                                   NW_Text_t* url, NW_Text_t** retUrl)
{
  const NW_Ucs2* urlStorage;
  NW_Uint8 urlFreeNeeded;
  const NW_Ucs2* baseStorage;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Ucs2* absStorage;
  NW_Text_UCS2_t* absURL;
  NW_Bool isRelative;

  NW_ASSERT(retUrl != NULL);
  *retUrl = NULL;
  /* all URL's are used by the Rainbow loader system as Unicode */
  urlStorage =
    NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Aligned, NULL, &urlFreeNeeded);

  /* if the supplied URL is absolute we simply return that */
  if (KBrsrSuccess != NW_Url_IsRelative (urlStorage, &isRelative))
  {
    if (urlFreeNeeded) {
      NW_Mem_Free ((NW_Ucs2*) urlStorage);
    }
    return KBrsrFailure;
  }

  if (!isRelative) {
    if (urlFreeNeeded) {
      NW_Mem_Free ((NW_Ucs2*) urlStorage);
    }
    *retUrl = url;
    return status;
  }

  /* ok - its a relative URL, we need to resolve that - first thing is to get
     the storage of the base URL */
  baseStorage = NW_HED_UrlRequest_GetRawUrl (NW_HED_ContentHandler_GetUrlRequest (thisObj));

  /* construct the absolute URL */
  status = NW_Url_RelToAbs (baseStorage, urlStorage, &absStorage);
  if (status != KBrsrSuccess) {
    if (urlFreeNeeded) {
      NW_Mem_Free ((NW_Ucs2*) urlStorage);
    }
    return status;
  }

  /* cleanup */
  if (urlFreeNeeded) {
    NW_Mem_Free ((NW_Ucs2*) urlStorage);
  }

  absURL = NW_Text_UCS2_New (absStorage, 0, NW_Text_Flags_TakeOwnership);
  if (absURL == NULL) {
    return KBrsrOutOfMemory;
  }
  NW_Object_Delete (url);
  *retUrl = NW_TextOf (absURL);
  return KBrsrSuccess;
}


TBrowserStatusCode
_NW_HED_ContentHandler_ResponseComplete( NW_HED_ContentHandler_t* thisObj,
                                         NW_Url_Resp_t* response,
                                         void* context)
{
  NW_REQUIRED_PARAM(thisObj);
  NW_REQUIRED_PARAM(response);
  NW_REQUIRED_PARAM(context);
  return KBrsrSuccess;
}
