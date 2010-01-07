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

#include <e32def.h>

#include "nw_hed_loaderi.h"
#include "nw_adt_resizablevector.h"
#include "nw_text_ucs2.h"
#include "nw_hed_urlrequest.h"
#include "nwx_ctx.h"
#include "nwx_url_utils.h"
#include "urlloader_urlloaderint.h"
#include "nwx_string.h"
#include "nwx_logger.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */

/* Static data not permitted in Epoc. For now it's been moved to the
 * loader context; it would be better off in a class.
 */
#ifdef NOT_DEFINED
  // #ifdef __WINS__
static NW_ADT_DynamicVector_t* NW_HED_Loader_LoadQueue = NULL;
  #pragma message (__FILE__ "32:  Resolve global static data problem for NW_HED_LoaderQueue");
  #else
  //lint --e{752} Info -- local declarator not referenced
//  static const NW_ADT_DynamicVector_t* const NW_HED_Loader_LoadQueue = NULL;
#endif

/* Methods to store & retrieve loader queue from the context.
 * These should be moved out of the context & into a class
 * if possible.
 */
void StoreNW_HED_Loader_LoadQueue(NW_ADT_DynamicVector_t* q);
NW_ADT_DynamicVector_t* GetNW_HED_Loader_LoadQueue(void);

/* ------------------------------------------------------------------------- *
   private static methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static 
NW_Bool
NW_HED_Loader_IsTopLevelRequest (const NW_HED_UrlRequest_t* urlRequest)
{
    return NW_HED_UrlRequest_IsTopLevelRequest(urlRequest);
}

/* ------------------------------------------------------------------------- */
static 
NW_HED_Loader_LoadQEntry_t*
NW_HED_Loader_GetLoadQueueLoadEntry(NW_Uint16 transactionId)
{
  NW_HED_Loader_LoadQEntry_t* entry = NULL;
  NW_ADT_Vector_Metric_t numLoadQEntries;
  NW_ADT_Vector_Metric_t index;
  NW_ADT_DynamicVector_t* loadQueue = NULL;
  NW_Bool found = NW_FALSE;

  loadQueue = GetNW_HED_Loader_LoadQueue();

  if(loadQueue == NULL) {
    return entry;
  }

  /* find the Loader that initiated the load request in our LoadQueue */
  numLoadQEntries = NW_ADT_Vector_GetSize (loadQueue);
  for (index = 0; index < numLoadQEntries; index++) {
    /* get a pointer to the entry in the LoadQueue table */
    entry = (NW_HED_Loader_LoadQEntry_t*)
             NW_ADT_Vector_ElementAt (loadQueue, index);
    NW_ASSERT (entry != NULL);

    /* if we find a match we break out of the loop */
    if (!NW_Mem_memcmp (&entry->transactionId, &transactionId,
                        sizeof (transactionId))) {
      found = NW_TRUE;
      break;
    }
  }
  if( found != NW_TRUE )
    {
    entry = NULL;
    }

  return entry;
}

/* ------------------------------------------------------------------------- */
/*****************************************************************

  Name:         NW_HED_Loader_LocateUrl

  Description:  Locates URL 

  Parameters:   url - url to be located
                transactionId (OUT) - If the url was located, the transaction
                of corresponsing url

  Return Value: KBrsrSuccess
                KBrsrNotFound

*****************************************************************/
static
TBrowserStatusCode
NW_HED_Loader_LocateUrl(NW_Text_t* aUrl, NW_Uint16* aTransactionId,
                        NW_HED_ILoadRecipient_t* aLoadRecipient)
    {
    NW_ADT_DynamicVector_t* loadQueue = NULL;
    NW_HED_Loader_LoadQEntry_t* entry = NULL;
    NW_ADT_Vector_Metric_t numLoadQEntries;
    NW_ADT_Vector_Metric_t index;
    
    *aTransactionId = 0;
    loadQueue = GetNW_HED_Loader_LoadQueue();
    if (loadQueue == NULL)
        {
        return KBrsrNotFound;
        }
    
    numLoadQEntries = NW_ADT_Vector_GetSize (loadQueue);
    for (index = 0; index < numLoadQEntries; index++) 
        {
        NW_HED_Loader_Context_t* context;
        
        // Get a pointer to the entry in the LoadQueue table
    entry = (NW_HED_Loader_LoadQEntry_t*)
             NW_ADT_Vector_ElementAt (loadQueue, index);
        NW_ASSERT (entry != NULL);
        if (entry == NULL)
            {
            // We found an invalid entry, get the next entry
            NW_LOG3(NW_LOG_LEVEL5, "NW_HED_Loader_LocateUrl entry=NULL tId=%d %i of %i",
                    aTransactionId, index, numLoadQEntries);
            return KBrsrNotFound;
            }

        context = (NW_HED_Loader_Context_t*)(entry->clientData);

        // Do we have a valid context->urlRequest->url pointer
        if (context == NULL || context->urlRequest == NULL ||
            context->urlRequest->url == NULL || context->loadRecipient == NULL)
            {
            // We found an invalid entry, get the next entry
            NW_LOG3(NW_LOG_LEVEL5, "NW_HED_Loader_LocateUrl context has a NULL pointer tId=%d %i of %i",
                    aTransactionId, index, numLoadQEntries);
            continue;
            }
        
        // If we find a match we break out of the loop
        if (!NW_Text_Compare(context->urlRequest->url, aUrl) &&
            (context->loadRecipient == aLoadRecipient))
            {
            *aTransactionId = entry->transactionId;
            return KBrsrSuccess;
            }
        }   // end of for-loop

    return KBrsrNotFound;
    }

/* ------------------------------------------------------------------------- */

static
TBrowserStatusCode
NW_HED_Loader_PartialRespCallback( TBrowserStatusCode loadStatus,
                                   NW_Uint16 transactionId,
                                   NW_Int32 chunkIndex,
                                   void* loadContext,
                                   NW_Url_Resp_t* response )
  {
  NW_TRY( status ) 
    {
    NW_HED_Loader_LoadQEntry_t* entry;
    NW_HED_Loader_Context_t* context;
    NW_HED_Loader_t* thisObj = NW_HED_LoaderOf( loadContext );
    NW_ADT_DynamicVector_t* loadQueue = NULL;
    NW_Bool ignoreResponse = NW_FALSE;
    const NW_Text_t* newUrl = NULL;

    NW_ASSERT( thisObj != NULL );
    
    entry = NW_HED_Loader_GetLoadQueueLoadEntry( transactionId );
    
    if( entry != NULL ) 
      {
      // extract the values from the entry 
      (void) NW_Mem_memcpy( &context, &entry->clientData, sizeof( context ) );
      NW_ASSERT( context != NULL );
      
      // pre-pcoessing.

      // if the entry was cancelled in the meantime
      if( entry->loadStatus == KBrsrCancelled ) 
        {
        loadStatus = KBrsrCancelled;
        }
      // or if the transaction was cancelled by the document root
      // due to bad content etc
      else if( entry->loadStatus == KBrsrFailure ) 
        {
        ignoreResponse = NW_TRUE;
        }


	  if(chunkIndex == 0)
	  {
		//R->change the urlRequest with the urlResponse
		newUrl = NW_Text_New (HTTP_iso_10646_ucs_2, response->uri, NW_Str_Strlen(response->uri), 0);
		status = NW_HED_UrlRequest_UpdateUrl (context->urlRequest, newUrl);					

	  }

      // final chunk
      if( chunkIndex == -1 )
        {
		TBrowserStatusCode tempStatus = KBrsrSuccess;
		if (tempStatus) {}; // fix warning
        NW_LOG1(NW_LOG_LEVEL5, "NW_HED_Loader_PartialRespCallback chunkIndex == -1 %d", transactionId );
        // remove the entry from the LoadQueue 
        loadQueue = GetNW_HED_Loader_LoadQueue();
        tempStatus = NW_ADT_DynamicVector_RemoveElement( loadQueue, (void*)entry );
        NW_ASSERT( tempStatus == KBrsrSuccess );

        // remove the transaction from the requestList
        tempStatus = NW_ADT_DynamicVector_RemoveElement ( thisObj->requestList, &transactionId );
        NW_ASSERT( tempStatus == KBrsrSuccess);
        }

      // if the top level transaction was cancelled by the document root due to bad content etc
      // then we should not pass on the cancelled transaction as all associated resources has
      // already been released. 
      if( ignoreResponse == NW_FALSE && (loadStatus != 03 && loadStatus >= 0) )
        {
        // notify the LoadRecipient of the partial load transaction 
        status = NW_HED_ILoadRecipient_ProcessPartialLoad( context->loadRecipient, loadStatus,
          transactionId, chunkIndex, response, context->urlRequest, context->clientData ); 
        }
      else
        {
        UrlLoader_UrlResponseDelete( response );
        }
      
      // post-processing
      if( chunkIndex == -1 )
        {
        // Loading has finished
        // this call send EEventUrlLoadingFinished
        // should be called before NW_HED_ILoaderListener_IsLoading(,,NW_FALSE)
        // where EEventLoadFinished will be sent
        (void)NW_HED_ILoaderListener_LoadProgressOff(thisObj->loaderListener, transactionId);

        // ToDo: Maybe more check is needed here?
        // when testing file://BrCtlTest/test10.htm, 
        // markup load finishes before object request was generated from ObjectElementHandler
        // so EEventLoadFinished is sent too early
        // maybe we can check rootbox->renderingState == NW_LMgr_RootBox_RenderComplete;
        // when test10.htm is loaded onto waplabdc network, the senario above didn't happen
        // let the listener know that we are all set with loading.
        if( NW_ADT_Vector_GetSize( thisObj->requestList ) == 0 &&
            thisObj->loaderListener != NULL ) 
          {
          NW_HED_ILoaderListener_IsLoading( thisObj->loaderListener, thisObj, NW_FALSE );
          }

        // clean-up context
        NW_Mem_Free( context );
        }
      }
    // no valid entry can be found in the load queue
    else
      {      
       NW_LOG2(NW_LOG_LEVEL5, "NW_HED_Loader_PartialRespCallback lurking chunk  transID=%d chunkIndex=%d", transactionId, chunkIndex );
      // clean-up response
      UrlLoader_UrlResponseDelete( response );
      status = KBrsrSuccess;
      }
    _NW_THROW_ON_ERROR(status);
    }
  NW_CATCH (status) 
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
OssPartialRespCallback( TBrowserStatusCode aLoadStatus,
                        NW_Uint16 aTransactionId,
                        NW_Int32 aChunkIndex,
                        void* aLoadContext,
                        NW_Url_Resp_t* aResponse )
{
	return NW_HED_Loader_PartialRespCallback(aLoadStatus,aTransactionId,aChunkIndex,aLoadContext,aResponse);
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_Loader_PushLoad (void* clientData, 
                        NW_Uint16* transactionId)
{
  NW_HED_Loader_LoadQEntry_t* loadQEntry;
  NW_ADT_DynamicVector_t* loadQueue = NULL;

  /* make sure that the LoadQueue is initialized */
  loadQueue = GetNW_HED_Loader_LoadQueue();

  if (loadQueue == NULL) {
    StoreNW_HED_Loader_LoadQueue((NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New (sizeof (NW_HED_Loader_LoadQEntry_t),
      8, 4));
  }

  loadQueue = GetNW_HED_Loader_LoadQueue();
  if (loadQueue == NULL) {
    return KBrsrOutOfMemory;
  }
  
  /* create a new entry to the load queue and initialize it */
  loadQEntry = (NW_HED_Loader_LoadQEntry_t*)
    NW_ADT_DynamicVector_InsertAt (loadQueue, NULL, NW_ADT_Vector_AtEnd);
  if (loadQEntry == NULL) {
    return KBrsrOutOfMemory;
  }
  
  /* copy the clientData and transactionId */
  (void) NW_Mem_memcpy (&loadQEntry->clientData, &clientData,
                        sizeof (loadQEntry->clientData));

  (void) NW_Mem_memcpy (&loadQEntry->transactionId, transactionId,
                        sizeof (loadQEntry->transactionId));

  loadQEntry->loadStatus = KBrsrSuccess;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_HED_Loader_StartRequestHelper (NW_HED_Loader_t* thisObj,
                                  const NW_HED_UrlRequest_t* urlRequest,
                                  void* clientData, 
                                  NW_Uint16* transactionId, void* aLoadContext)
{
  TBrowserStatusCode      status;
  const NW_Ucs2    *url;
  NW_Http_Header_t *header   = NULL;
  NW_Buffer_t      *data     = NULL;
  NW_Cache_Mode_t  cacheMode;
  NW_Uint8         method;
  NW_Uint8         loadType;
  NW_Uint8*        dataBuf = NULL;
  NW_Uint32        dataLen = 0;
  
  if (NW_HED_UrlRequest_GetHeader (urlRequest) != NULL) {
	header  = (NW_Http_Header_t*)UrlLoader_HeadersCopy (NW_HED_UrlRequest_GetHeader (urlRequest)); //R->ul
  }

  if (NW_HED_UrlRequest_GetBody (urlRequest) != NULL) {
    data = NW_Buffer_New (0);
    if (data == NULL)
      return KBrsrOutOfMemory;

    /* The buffer copy can fails if file to be loaded is too big */

    if( NW_Buffer_CopyBuffers (data, NW_HED_UrlRequest_GetBody (urlRequest)) != KBrsrSuccess)
    {
     return KBrsrOutOfMemory;
    }
    dataBuf = data->data;
    dataLen = data->length;
  }

  /* initiate the load with the Loader subsystem */
  url = NW_HED_UrlRequest_GetRawUrl (urlRequest);  
  method = NW_HED_UrlRequest_GetMethod (urlRequest);  
  loadType = NW_HED_UrlRequest_GetLoadType(urlRequest);
  cacheMode = (NW_Cache_Mode_t) NW_HED_UrlRequest_GetCacheMode (urlRequest);

  status = UrlLoader_Request(url, header, dataBuf, dataLen, method, transactionId,
    thisObj, NW_HED_Loader_PartialRespCallback, cacheMode, NW_DEFAULTHEADERS_ALL, loadType,
	NW_HED_Loader_IsTopLevelRequest(urlRequest), NW_FALSE, urlRequest->reason, aLoadContext );

  NW_Buffer_FreeNotData(data);
  if (status == KBrsrSuccess) {
    status = NW_HED_Loader_PushLoad(clientData, transactionId);
  }

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_Loader_Class_t NW_HED_Loader_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Object_Dynamic_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_HED_Loader_t),
    /* construct         */ _NW_HED_Loader_Construct,
    /* destruct          */ _NW_HED_Loader_Destruct
  },
  { /* NW_HED_Loader	 */
    /* unused            */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_Loader_Construct (NW_Object_Dynamic_t* dynamicObject,
                          va_list* argList)
{
  NW_HED_Loader_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_Loader_Class));
  NW_ASSERT (argList != NULL);

  NW_TRY (status) {
  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM (argList);

  /* for convenience */
  thisObj = NW_HED_LoaderOf (dynamicObject);

  /* initialize the member variables */
  thisObj->requestList = (NW_ADT_DynamicVector_t*)
    NW_ADT_ResizableVector_New (sizeof (NW_Uint16), 8, 4);

    NW_THROW_OOM_ON_NULL (thisObj->requestList, status);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_Loader_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_Loader_t* thisObj;

  /* for convenience */
  thisObj = NW_HED_LoaderOf (dynamicObject);

  /* delete our dynamic members */
  NW_Object_Delete (thisObj->requestList);

  /* clear the reqest list*/
  NW_Object_Delete(GetNW_HED_Loader_LoadQueue());
  StoreNW_HED_Loader_LoadQueue(NULL);
}

/* ------------------------------------------------------------------------- */
/* the caller retains ownership of url                                       */
TBrowserStatusCode
NW_HED_Loader_StartLoad (NW_HED_Loader_t* thisObj,
                         const NW_Text_t* url,
                         NW_Uint8 reason,
                         NW_HED_ILoadRecipient_t* loadRecipient,
                         void* clientData,
                         NW_Uint16* transactionId,
                         NW_Uint8 loadType,
                         NW_Cache_Mode_t cacheMode)
{
  NW_HED_UrlRequest_t* urlRequest = NULL;

  NW_TRY (status) {
    urlRequest =
      NW_HED_UrlRequest_New ((NW_Text_t*) url, NW_URL_METHOD_GET, NULL, NULL,
                             reason, NW_HED_UrlRequest_LoadNormal, loadType);
    NW_THROW_OOM_ON_NULL (urlRequest, status);
    NW_HED_UrlRequest_SetCacheMode(urlRequest, cacheMode);
  
    status = NW_HED_Loader_StartRequest (thisObj, urlRequest, loadRecipient,
                                         clientData, transactionId, NULL );
    if( status == KBrsrCancelled )
      {
      // status == KBrsrCancelled means virtual request
      // do not handle as error
      status = KBrsrSuccess;
      }
    NW_THROW_ON_ERROR (status);
  }

  NW_CATCH (status) {
    NW_Object_Delete (urlRequest);
  }

  NW_FINALLY {
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
/* upon success the ownership of urlRequest is passed to this method. 
   The ownership is returned back if load was not successful         */
TBrowserStatusCode
NW_HED_Loader_StartRequest (NW_HED_Loader_t* thisObj,
                            const NW_HED_UrlRequest_t* urlRequest,
                            NW_HED_ILoadRecipient_t* loadRecipient,
                            void* clientData,
                            NW_Uint16* transactionId, void* aLoadContext )
{
  NW_HED_Loader_Context_t* context = NULL;
  NW_Ucs2* dest = NULL;

  NW_TRY (status) 
  {
  void                    *entry;
  NW_Text_t* safeUrl = NULL;  
  NW_Ucs2* storage = NULL;
  NW_Uint8 freeNeeded;
  NW_Text_Length_t charCount;
  const NW_Ucs2* origUrl = NULL;
  NW_Ucs2* escapedUrl = NULL;
  NW_Url_Schema_t schema = NW_SCHEMA_INVALID;
    
  /* Search for Url Request URL - If found delete url request set
  tansaction id and return*/

  /* For Kimono this is only for embedded loads */
  if ((urlRequest->reason == NW_HED_UrlRequest_Reason_DocLoadChild) && 
      (urlRequest->loadType != NW_UrlRequest_Type_Plugin))
  {
    status = NW_HED_Loader_LocateUrl(urlRequest->url, transactionId, loadRecipient );
    if (status == KBrsrSuccess){
      NW_Object_Delete((NW_HED_UrlRequest_t*)urlRequest);
      NW_LOG1(NW_LOG_LEVEL4, "Loader:LocateUrl successful %d", transactionId);
      // actually the request has not been cancelled but there is no better
      // way to tell that it is a virtual request.
      return KBrsrCancelled;
    }
  }

  /* allocate and initialize the Context */
  context = (NW_HED_Loader_Context_t*)NW_Mem_Malloc (sizeof (*context));
  NW_THROW_OOM_ON_NULL (context, status);
  
  /* get the orig url as a ucs2 string */
  origUrl = NW_HED_UrlRequest_GetRawUrl (urlRequest);
  NW_THROW_OOM_ON_NULL(origUrl, status);

//??? RFD: Need to fix this section of code to:
// Determine up-front:
// 1. what encoding to use
// 2. whether or not to UTF-8 encode
// 3. whether or not to escape-encode, which is based on things like the shema
//      (WTAI does not encode) and WML (which allows parts of the URL, such as
//      form fields, I think, to optionally be escaped), and then there's
//      something about chars after the # char, which I need to find out more about.
// 4. determine endianess if encoding is 2-bytes and not being UTF-8 encoded
//    (The endianness is important for 2-byte chars whether or not they are
//    escape-encoded.)
// 5. whether or not escape using "wise" or "unwise"
// Determining these things will depend on the schema, the original charset,
// whether orginal charset was explicitely specified or not, endianess, etc.
// WTAI schema, for example, uses UCS2 without encoding, whithout escaping, little endian.
//
// Then convert using the info determined above:
//  If UTF8 encoding:
//      1. UTF8 encode
//      2. escape-encode (wise or unwise)
//  If NOT UTF8 encoding:
//      1. convert unicode to target charset (which parts of the URL does this
//          apply to? Don't want to convert 'http:' to foreign charset for example.)
//      2. if target charset is 2-bytes, then convert endianess
//      3. if escape-encoding then escape using "wise" or "unwise"


  /* ensure that the url is escaped correctly */
  escapedUrl = NW_Str_New(NW_Url_EscapeUnwiseLen(origUrl));
  NW_THROW_OOM_ON_NULL(escapedUrl, status);
  NW_Url_EscapeUnwise(origUrl, escapedUrl);

  NW_Url_GetSchemeNoValidation(escapedUrl, &schema);

  /* Resolve any 8 or 16-bit characters */
  storage = (NW_Ucs2*)
  NW_Text_GetUCS2Buffer (urlRequest->url, NW_Text_Flags_Aligned, &charCount,
                           &freeNeeded);

  if (schema != NW_SCHEMA_WTAI) {
    /* It's not clear why NW_Url_ConvertUnicode() is called. 
     * For phonebook you end up converting content a second time,
     * leading to an incorrect result, so call has been removed for tel and mailto.
     * Maybe we should remove call entirely?
     */
//??? RFD: The dest results of ConvertUnicode is not UCS2 and therefore the type of dest is wrong. It is 2-bytes per char but it's not UCS2. It's been converted.
    status = NW_Url_ConvertUnicode(storage , &dest );
    NW_THROW_OOM_ON_NULL (dest, status);
    safeUrl = NW_Text_New (HTTP_iso_10646_ucs_2, dest,
              NW_Str_Strlen(dest),
              0);
  } else {
    safeUrl = NW_Text_New (HTTP_iso_10646_ucs_2, storage,
             NW_Str_Strlen(storage),
             0);
  }

  NW_THROW_OOM_ON_NULL (safeUrl, status);
  if (freeNeeded) {
    NW_Mem_Free(storage);
  }

  // The UrlRequest needs to hold a copy of the url.
  status = NW_HED_UrlRequest_UpdateUrl ( (NW_HED_UrlRequest_t*)urlRequest, safeUrl);
  _NW_THROW_ON_ERROR(status);

  context->urlRequest = (NW_HED_UrlRequest_t*) urlRequest;
  context->loadRecipient = loadRecipient;
  context->clientData = clientData;

  /* execute the load */
  status =  NW_HED_Loader_StartRequestHelper (thisObj,
                      (NW_HED_UrlRequest_t*) urlRequest,
                        context, transactionId, aLoadContext );
  if (status == KBrsrSuccess && schema == NW_SCHEMA_WTAI) {  
      status = KBrsrCancelled;
  }
  _NW_THROW_ON_ERROR(status);
  
   NW_LOG1(NW_LOG_LEVEL5, "NW_HED_Loader_StartRequest %d", *transactionId );
  /* insert the transactionId into the requestList */
  entry = NW_ADT_DynamicVector_InsertAt (thisObj->requestList, transactionId,
                                         NW_ADT_Vector_AtEnd);
  if (entry == NULL) {      
    status =
      NW_HED_Loader_DeleteLoad (thisObj, *transactionId,NULL);
    _NW_THROW_ON_ERROR( status);      
  }

  // only send EEventNewContentStart when this is the top level request
  if ( NW_ADT_Vector_GetSize (thisObj->requestList) == 1 &&
       thisObj->loaderListener != NULL ) 
    {
    NW_HED_ILoaderListener_IsLoading (thisObj->loaderListener, thisObj,
                                      NW_TRUE);
    }
  
  /* Loading has started. Fine. Now it's time to tell application to start to poll
     response status */
  (void)NW_HED_ILoaderListener_LoadProgressOn(thisObj->loaderListener, *transactionId);

  NW_CATCH (status) {
    NW_Mem_Free(context);
  }

  NW_FINALLY {
    NW_Str_Delete (escapedUrl);
    if (dest != NULL) {
      NW_Mem_Free(dest);
    }
    NW_Mem_Free(safeUrl);
    return status;
  }
  
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
NW_HED_Loader_GetRequestListSize (NW_HED_Loader_t* thisObj)
{
  NW_ADT_Vector_Metric_t numRequests;
  
  /* paramater assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_Loader_Class));  
  
  numRequests = NW_ADT_Vector_GetSize( thisObj->requestList );
  
  return numRequests;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_Loader_GetLoaderEntry (NW_HED_Loader_t* thisObj,
                              NW_Uint16 transactionId,
                              NW_HED_Loader_LoadQEntry_t** entry)
{
  /* paramater assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_Loader_Class));

  NW_TRY (status) {
    NW_ADT_Vector_Metric_t index;

    index = NW_ADT_Vector_GetElementIndex (thisObj->requestList, &transactionId);
    if (index == NW_ADT_Vector_AtEnd) {
      NW_THROW_STATUS (status, KBrsrBadInputParam);
    }

    *entry = NW_HED_Loader_GetLoadQueueLoadEntry(transactionId);
    if (*entry == NULL) {
      NW_THROW_STATUS (status, KBrsrNotFound);
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_Loader_DeleteLoad (NW_HED_Loader_t* thisObj,
                          NW_Uint16 transactionId,
                          void* clientData)
{
  /* paramater assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_Loader_Class));

  NW_TRY (status) {
    NW_HED_Loader_LoadQEntry_t *entry;
    NW_HED_Loader_Context_t* context;
    NW_ADT_DynamicVector_t* hedLoadQueue = NULL;

    /* ensure that the transaction id is legal for this loader object */
    status = NW_HED_Loader_GetLoaderEntry (thisObj, transactionId, &entry);
    _NW_THROW_ON_ERROR( status );

    /* instruct the loader subsystem to cancel the load */
    if (entry->loadStatus != KBrsrCancelled) {
      entry->loadStatus = KBrsrCancelled;

      //UrlLoader_Cancel(transactionId);
      if (clientData != NULL) {
        *(void**) clientData = NULL;
      }
    }
    else {
      /* get the load context and extract the client data if necessary and free
         the load context */
      (void) NW_Mem_memcpy (&context, &entry->clientData, sizeof (context));
      if (clientData != NULL) {
        *(void**) clientData = context->clientData;
      }
      NW_Object_Delete (context->urlRequest); 
      NW_Mem_Free (context);
    }


    /* remove the entry from the load queue */
    hedLoadQueue = GetNW_HED_Loader_LoadQueue();
    NW_ASSERT(hedLoadQueue);
    /*Ignore the status as the entry could be removed in CancelRequest from HedLoadQueue*/
    status = NW_ADT_DynamicVector_RemoveElement (hedLoadQueue,
                                                 (void*) entry);
    

    /* remove the transaction id entry from the loader */
    status =
      NW_ADT_DynamicVector_RemoveElement (thisObj->requestList, &transactionId);
    NW_LOG1(NW_LOG_LEVEL5, "NW_HED_Loader_DeleteLoad %d", transactionId );
    
    /* if this was the last outstanding request - notify the ILoaderListener */
    if (NW_ADT_Vector_GetSize (thisObj->requestList) == 0 &&
        thisObj->loaderListener != NULL) {
      NW_HED_ILoaderListener_IsLoading (thisObj->loaderListener, thisObj,
				        NW_FALSE);
    }
    NW_THROW_ON_ERROR (status);
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}


TBrowserStatusCode
NW_HED_Loader_RemoveTransFromLoadList( NW_HED_Loader_t* thisObj, NW_Uint16 transactionId )
  {
  /* paramater assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_Loader_Class));

  NW_TRY(status) 
    {
    NW_HED_Loader_LoadQEntry_t *entry;
    NW_ADT_DynamicVector_t* hedLoadQueue = NULL;

    /* ensure that the transaction id is legal for this loader object */
    status = NW_HED_Loader_GetLoaderEntry (thisObj, transactionId, &entry);
    if (status == KBrsrSuccess) 
      {
      /* remove the entry from the load queue */
      hedLoadQueue = GetNW_HED_Loader_LoadQueue();
      NW_ASSERT(hedLoadQueue);
      /*Ignore the status as the entry could be removed in CancelRequest from HedLoadQueue*/
      status = NW_ADT_DynamicVector_RemoveElement (hedLoadQueue,
                                                   (void*) entry);    

      /* remove the transaction id entry from the loader */
      status =
        NW_ADT_DynamicVector_RemoveElement (thisObj->requestList, &transactionId);
        NW_LOG1(NW_LOG_LEVEL5, "NW_HED_Loader_RemoveTransFromLoadList %d", transactionId );
      
      /* if this was the last outstanding request - notify the ILoaderListener */
      if (NW_ADT_Vector_GetSize (thisObj->requestList) == 0 &&
        thisObj->loaderListener != NULL) 
        {
        NW_HED_ILoaderListener_IsLoading (thisObj->loaderListener, thisObj, NW_FALSE);
        }
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
NW_HED_Loader_CancelLoad (NW_HED_Loader_t* thisObj,
                          NW_Uint16 transactionId, TBrowserStatusCode cancelReason )
{
  /* paramater assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_Loader_Class));

  NW_TRY (status) {
    NW_HED_Loader_LoadQEntry_t* entry = NULL;
  
    status = NW_HED_Loader_GetLoaderEntry (thisObj, transactionId, &entry);
    _NW_THROW_ON_ERROR( status );

    if (entry->loadStatus != KBrsrCancelled) {
      entry->loadStatus = cancelReason;
      //UrlLoader_Cancel(transactionId);
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_Loader_CancelAllLoads (NW_HED_Loader_t* thisObj)
{
  NW_ADT_Vector_Metric_t numRequests;
  NW_Int32 index;
  NW_HED_Loader_LoadQEntry_t* loadQEntry = NULL;

  /* paramater assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_Loader_Class));

  /* ensure that the supplied transactionId is valid for this Loader object */
  numRequests = NW_HED_Loader_GetRequestListSize (thisObj);
  if (numRequests == 0) {
    return KBrsrSuccess;
  }
  for (index = numRequests - 1; index >= 0; index--) {
    NW_Uint16* entry;

    entry = (NW_Uint16*) 
      NW_ADT_Vector_ElementAt (thisObj->requestList, NW_UINT16_CAST(index));
    if(entry){
      loadQEntry = NW_HED_Loader_GetLoadQueueLoadEntry (*entry);
      if ((loadQEntry)&&(loadQEntry->loadStatus != KBrsrCancelled)) {

        loadQEntry->loadStatus = KBrsrCancelled;
        //UrlLoader_Cancel(*entry);

      }
    }
  }

  /* successfull completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_Loader_t*
NW_HED_Loader_New (void)
{
  return (NW_HED_Loader_t*) NW_Object_New (&NW_HED_Loader_Class);
}

/* Methods to store & retrieve loader queue from the context.
 * These should be moved out of the context & into a class
 * if possible.
 */
void StoreNW_HED_Loader_LoadQueue(NW_ADT_DynamicVector_t* q)
{
  NW_Ctx_Set(NW_CTX_HED_LOAD_QUEUE, 0, q);
}

NW_ADT_DynamicVector_t* GetNW_HED_Loader_LoadQueue()
{
  return (NW_ADT_DynamicVector_t*)NW_Ctx_Get(NW_CTX_HED_LOAD_QUEUE, 0);
}
