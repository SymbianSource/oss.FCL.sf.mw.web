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
#include <brctldefs.h>

#include "nw_hed_historystacki.h"

#include "nw_adt_fixedvector.h"
#include "nwx_logger.h"
#include "BrsrStatusCodes.h"
#include "nwx_http_defs.h"

#include "StringUtils.h"
#include "nwx_string.h"

#include "BrCtl.h"
#include "WmlControl.h"
#include "HistoryController.h"
#include "HistoryStack.h"
#include "HistoryEntry.h"
#include "WmlInterface.h"

class CWmlControl;
/* ------------------------------------------------------------------------- *
   constants
 * ------------------------------------------------------------------------- */
#define NW_HED_HistoryStack_StackSize ((NW_HED_HistoryStack_Metric_t) 20)

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_DeleteEntry (NW_HED_HistoryStack_t* thisObj,
                                 NW_HED_HistoryStack_Metric_t index)
{
  NW_HED_HistoryEntry_t* entry = NULL;
  if(index >= 0)
    {
    entry = NW_HED_HistoryStack_GetEntryByIndex( thisObj, index );
    thisObj->iWmlCtrl->HistoryController().deleteEntry( index );
    }
  return entry;
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryStack_Metric_t
NW_HED_HistoryStack_GetIndex (NW_HED_HistoryStack_t* thisObj,
                              NW_HED_HistoryStack_Direction_t direction)
{
  NW_HED_HistoryStack_Metric_t ind;
  ind = thisObj->iWmlCtrl->HistoryController().index( KimonoToOssDirection( direction ) );
  return ind;
}

/* ------------------------------------------------------------------------- */
const
NW_HED_HistoryStack_Class_t NW_HED_HistoryStack_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_HistoryStack_t),
    /* construct               */ _NW_HED_HistoryStack_Construct,
    /* destruct                */ _NW_HED_HistoryStack_Destruct
  },
  { /* NW_HED_HistoryStack     */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_HistoryStack_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_HED_HistoryStack_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_HistoryStack_Class));
  NW_ASSERT (argp != NULL);

  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM (argp);

  /* for convenience */
  thisObj = NW_HED_HistoryStackOf (dynamicObject);

  // create the history-entry vector
  thisObj->vector = (NW_ADT_DynamicVector_t*)
    NW_ADT_FixedVector_New (sizeof (NW_HED_HistoryEntry_t*),
                            NW_HED_HistoryStack_StackSize);
  thisObj->current = -1;

  if (!thisObj->vector) {
    return KBrsrOutOfMemory;
  }
  
  thisObj->iReason = NW_HED_UrlRequest_Reason_DocLoad;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_HistoryStack_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_HistoryStack_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_HistoryStack_Class));

  /* for convenience */
  thisObj = NW_HED_HistoryStackOf (dynamicObject);
  
  /* deallocate our history-entry vector */
  NW_Object_Delete (thisObj->vector);
  //delete iStackUtility;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryStack_SetCurrent(NW_HED_HistoryStack_t* thisObj,
                         NW_HED_HistoryStack_Direction_t direction)
{
  thisObj->iWmlCtrl->HistoryController().setCurrentL( KimonoToOssDirection( direction ) );
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryStack_SetCurrentEntry(NW_HED_HistoryStack_t* thisObj,
                                    NW_HED_HistoryEntry_t* entry)
{
  int index = NW_HED_HistoryStack_GetIndexByEntry (thisObj, entry);
  if(index > 0)
    {
    thisObj->iWmlCtrl->HistoryController().setCurrentIndex(index);
    return KBrsrSuccess;
    }
  else
    {
    return KBrsrFailure;
    }
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_GetEntry (NW_HED_HistoryStack_t* thisObj,
                         NW_HED_HistoryStack_Direction_t direction)
{
  NW_HED_HistoryEntry_t* kEntry = NULL;
  THistoryStackDirection entryDir = KimonoToOssDirection(direction);
  int ind = thisObj->iWmlCtrl->HistoryController().index(EHistoryStackDirectionCurrent);
  if( ind < 0 )
    {
    return kEntry;
    }
  else
    {
    HistoryEntry* entry = NULL;
    if(entryDir == EHistoryStackDirectionPrevious)
        {
        entry = thisObj->iWmlCtrl->HistoryController().entryByIndex(ind -1);
        }
    else if(entryDir == EHistoryStackDirectionNext)
        {
        entry = thisObj->iWmlCtrl->HistoryController().entryByIndex(ind + 1);   
        }
    else
        {
        entry = thisObj->iWmlCtrl->HistoryController().entry(EHistoryStackDirectionCurrent);
        }
    if(entry)
        {
        HBufC16* reqDes = NULL;
        
        TUint reqLength = entry->requestUrl().Length();
        reqDes = HBufC16::New(reqLength + 1);
        if (reqDes) 
            {
            TPtr urlPtr(reqDes->Des());
            urlPtr.Copy(entry->requestUrl());
            urlPtr.ZeroTerminate();
      
            NW_Text_t* textReqUrl = NW_Text_New (HTTP_iso_10646_ucs_2, (void*)urlPtr.Ptr(), urlPtr.Length(), NW_TRUE);
            
            if (textReqUrl) 
                {
                NW_HED_UrlRequest_t* urlReq = NW_HED_UrlRequest_New (textReqUrl, NW_URL_METHOD_GET, NULL, NULL, thisObj->iReason, NW_HED_UrlRequest_LoadNormal, 1);
        
                const NW_Ucs2* uri = (const NW_Ucs2*)textReqUrl->storage;
                NW_Uint8        *aBody = NULL;
                NW_Uint32       aBodyLength = 0;
                void            *aHeaders = NULL;
                NW_Uint8        *aContentTypeString = NULL; 
                NW_Uint8        *aContentLocationString = NULL;
                NW_Bool         aNoStore = EFalse;
                NW_Uint8        *aBoundaryString = NULL; 
                NW_Ucs2         *aLastModified = NULL;
                NW_Uint16       aCharset = 0;
                NW_Uint8        *aCharsetString = 0;
                NW_Uint32       aHttpStatus = 200 ; 
                NW_Uint16       aTransId = 0;
                NW_Uint32       aContentLength = 0;

                NW_Url_Resp_t* respUrl = UrlLoader_UrlResponseNew(uri,
                                                                   aBody,
                                                                   aBodyLength, 
                                                                   aHeaders,
                                                                   aContentTypeString,
                                                                   aContentLocationString,
                                                                   aNoStore,
                                                                   aBoundaryString, 
                                                                   aLastModified,
                                                                   aCharset,
                                                                   aCharsetString,
                                                                   aHttpStatus, 
                                                                   NW_URL_METHOD_GET,
                                                                   aTransId,
                                                                   aContentLength);

                NW_HED_UrlResponse_t* urlResp = NW_HED_UrlResponse_New (respUrl);
                kEntry = NW_HED_HistoryEntry_New (urlReq, urlResp);
                }
            }
        }  
        return kEntry;
    }
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_RemoveEntry (NW_HED_HistoryStack_t* thisObj,
                                 NW_HED_HistoryStack_Direction_t direction)
{
  NW_HED_HistoryStack_Metric_t index;
  // parameter assertions 
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_HED_HistoryStack_Class));

  index = NW_HED_HistoryStack_GetIndex (thisObj, direction);

  // remove the entry
  return NW_HED_HistoryStack_DeleteEntry (thisObj, index);
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryStack_InsertEntry (NW_HED_HistoryStack_t* thisObj,
                                 NW_HED_HistoryEntry_t* historyEntry,
                                 NW_HED_HistoryStack_Location_t /*location*/)
{
    TPtrC formContentType(KNullDesC);
    WebCore::FormData* formData = NULL;
    TInt reqLength = historyEntry->urlRequest->url->characterCount;
    TInt respLength = historyEntry->urlResponse->url->characterCount;

    if (NW_Asc_strlen((char *)historyEntry->urlRequest->url->storage) == 1 ) { // 16 bit 
        HBufC* urlreq = NULL;
        HBufC* urlresp = NULL;
        HBufC8* urlResponse8 = NULL;
        HBufC8* urlRequest8 = NULL;

        urlreq = HBufC::New(reqLength + 1);
        if (urlreq) {
            TPtr urlReq16(urlreq->Des());
            urlReq16.Copy((TUint16*) historyEntry->urlRequest->url->storage, reqLength);
            urlReq16.ZeroTerminate();

            if (!thisObj->iWmlCtrl->HistoryController().containsItemForURL((TUint16*)urlReq16.Ptr())) {  
                urlresp = HBufC::New(respLength + 1);
                if (urlresp) {
                    TPtr urlResp16(urlresp->Des());
                    urlResp16.Copy((TUint16*)historyEntry->urlResponse->url->storage, respLength);
                    urlResp16.ZeroTerminate();

                    urlResponse8 = HBufC8::New(urlResp16.Length() + 1);
                    if (urlResponse8) {
                        TPtr8 url8RespPtr(urlResponse8->Des());
                        url8RespPtr.Copy(urlResp16);
                        url8RespPtr.ZeroTerminate();

                        urlRequest8 = HBufC8::New(urlReq16.Length() + 1);
                        if (urlRequest8) {
                            TPtr8 url8ReqPtr(urlRequest8->Des());
                            url8ReqPtr.Copy(urlReq16);
                            url8ReqPtr.ZeroTerminate();

                            thisObj->iWmlCtrl->HistoryController().insert(url8ReqPtr, url8RespPtr, formContentType, formData);  
                            thisObj->iWmlCtrl->HistoryController().setCurrentEntryTitle(urlReq16);
                        }
                    }
                }
            }
        }
        delete urlreq;
        delete urlresp;
        delete urlResponse8;
        delete urlRequest8;
    }
    else {  // 8 bit
        HBufC* urlRequest16 = HBufC::NewL(reqLength + 1);
        HBufC8* urlresp = NULL;
        HBufC8* urlreq = NULL;
        if (urlRequest16) {
            TPtr urlRequest16Ptr(urlRequest16->Des());
            urlRequest16Ptr.Copy((unsigned short *)historyEntry->urlRequest->url->storage, reqLength);
            urlRequest16Ptr.ZeroTerminate();

            if (!thisObj->iWmlCtrl->HistoryController().containsItemForURL( urlRequest16->Des().Ptr() ) ) {
                urlresp = HBufC8::NewL(respLength + 1);
                if (urlresp) {
                    TPtr8 ptrResp(urlresp->Des());
                    ptrResp.Copy((TUint8*) historyEntry->urlResponse->url->storage, respLength);
                    ptrResp.ZeroTerminate();

                    urlreq = HBufC8::NewL(reqLength + 1);
                    if (urlreq) {
                        TPtr8 ptrReq(urlreq->Des());
                        ptrReq.Copy((TUint8*) historyEntry->urlRequest->url->storage, reqLength);
                        ptrReq.ZeroTerminate();

                        thisObj->iWmlCtrl->HistoryController().insert(ptrReq, ptrResp, formContentType, formData); 
                        thisObj->iWmlCtrl->HistoryController().setCurrentEntryTitle(urlRequest16Ptr);
                    }
                }
            }
        }
        delete urlresp;
        delete urlreq;
        delete urlRequest16;
    }
    return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryStack_PushEntry (NW_HED_HistoryStack_t* thisObj,
                          NW_HED_HistoryEntry_t* historyEntry)
  {
  return NW_HED_HistoryStack_InsertEntry (thisObj, historyEntry, 
      NW_HED_HistoryStack_Loacation_AtEnd);
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryStack_DeleteAll (NW_HED_HistoryStack_t* thisObj)
{  
    thisObj->iWmlCtrl->HistoryController().clearHistoryList();  
    return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_HistoryStack_IsUrlinHistory (NW_HED_HistoryStack_t* thisObj, 
                                    NW_HED_UrlRequest_t* urlRequest)
{
  TPtrC16 url( (TUint16*) urlRequest->url->storage );
  return thisObj->iWmlCtrl->HistoryController().containsItemForURL(url);  
}

/* ------------------------------------------------------------------------- */
NW_Uint8
NW_HED_HistoryStack_Length(NW_HED_HistoryStack_t* thisObj)
{
  return thisObj->iWmlCtrl->HistoryController().historyLength();
}

/* ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_GetEntryByIndex (NW_HED_HistoryStack_t* thisObj,NW_Uint8 historyIndex)
{
  NW_HED_HistoryEntry_t* kEntry = NULL;
  HistoryEntry* entry = thisObj->iWmlCtrl->HistoryController().entryByIndex(historyIndex);
  if(entry)
  {
    NW_Text_t* textReqUrl = NW_Text_New (HTTP_iso_10646_ucs_2, 
                                   (void*) thisObj->iWmlCtrl->HistoryController().requestUrl().Ptr(), 
                                   (NW_Text_Length_t)thisObj->iWmlCtrl->HistoryController().requestUrl().Length(), 
                                    NW_FALSE);

    NW_Text_t* textResUrl = NW_Text_New (HTTP_iso_10646_ucs_2, 
                                   (void*) thisObj->iWmlCtrl->HistoryController().responseUrl().Ptr(), 
                                   (NW_Text_Length_t)thisObj->iWmlCtrl->HistoryController().responseUrl().Length() + 1, 
                                    NW_FALSE);

   
    //urlReq
    NW_HED_UrlRequest_t* urlReq = NW_HED_UrlRequest_New (textReqUrl, NW_URL_METHOD_GET, NULL, NULL, 1, NW_HED_UrlRequest_LoadNormal, 1);
    
    NW_Ucs2   *aUri = (NW_Ucs2*)textResUrl->storage;
    NW_Uint8        *aBody = NULL;
    NW_Uint32       aBodyLength = 0;
    void            *aHeaders = NULL;
    NW_Uint8        *aContentTypeString = NULL; 
    NW_Uint8        *aContentLocationString = NULL;
    NW_Bool         aNoStore = EFalse;
    NW_Uint8        *aBoundaryString = NULL; 
    NW_Ucs2         *aLastModified = NULL;
    NW_Uint16       aCharset = 0;
    NW_Uint8        *aCharsetString = 0;
    NW_Uint32       aHttpStatus = 200 ; 
    NW_Uint16       aTransId = 0;
    NW_Uint32       aContentLength = 0;
    //urlResp
    NW_Url_Resp_t* respUrl = UrlLoader_UrlResponseNew((const NW_Ucs2*)aUri,
                                                   aBody,
                                                   aBodyLength, 
                                                   aHeaders,
                                                   aContentTypeString,
                                                   aContentLocationString,
                                                   aNoStore,
                                                   aBoundaryString, 
                                                   aLastModified,
                                                   aCharset,
                                                   aCharsetString,
                                                   aHttpStatus, 
                                                   NW_URL_METHOD_GET,
                                                   aTransId,
                                                   aContentLength);
  
    NW_HED_UrlResponse_t* urlResp = NW_HED_UrlResponse_New (respUrl);
    //kimono entry
    kEntry = NW_HED_HistoryEntry_New (urlReq, urlResp);
    }  
    return kEntry;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_HistoryStack_ValidateEntry (const NW_HED_HistoryEntry_t* historyEntry)
    {
    // Entry is valid if the method is not NW_URL_METHOD_DATA
    // Data loads should not be saved in History stack, because we 
    // do not keep the data
    NW_Uint8 method;
    NW_ASSERT (historyEntry != NULL);
    method = NW_HED_UrlRequest_GetMethod(historyEntry->urlRequest);
    return (NW_Bool)(method != NW_URL_METHOD_DATA);
    }

/* ------------------------------------------------------------------------- */
NW_Int8
NW_HED_HistoryStack_GetIndexByEntry (NW_HED_HistoryStack_t* thisObj,
                                     NW_HED_HistoryEntry_t* historyEntry)
    {
    // Return the index of a given entry, or -1 if the entry could not be found
    HistoryEntry* entry = NULL; 
    TInt size = thisObj->iWmlCtrl->HistoryController().historyLength();

    for (TInt i = 0; i < size; i++)
        {
        entry = thisObj->iWmlCtrl->HistoryController().entryByIndex(i);
        if ( entry->requestUrl().Ptr() == historyEntry->urlRequest->url->storage )
            {
            return i;
            }
        }
    
    return -1;

    }

/* ------------------------------------------------------------------------- */
void SetOssHistoryControl( NW_HED_HistoryStack_t* thisObj, CWmlControl* aWmlControl )
    {
    thisObj->iWmlCtrl = aWmlControl;
    }


void SetUrlRequestReason(NW_HED_HistoryStack_t* thisObj, TInt aReason)
    {
    thisObj->iReason = aReason;
    }


void SetPossibleWmlOEB(NW_HED_HistoryStack_t* thisObj, NW_Bool aFlag)
    {
    thisObj->iWmlCtrl->HistoryController().setPossibleWmlOEB(aFlag);
    }

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_HistoryStack_t*
NW_HED_HistoryStack_New (void)
{
  return (NW_HED_HistoryStack_t*) NW_Object_New (&NW_HED_HistoryStack_Class);
}

/* ------------------------------------------------------------------------- */
THistoryStackDirection KimonoToOssDirection( 
    NW_HED_HistoryStack_Direction_t aDirection )
    {
    THistoryStackDirection direction = EHistoryStackDirectionCurrent;

    switch( aDirection )
        {
        case NW_HED_HistoryStack_Direction_Next:
            {
            direction = EHistoryStackDirectionNext;
            break;
            }
        case NW_HED_HistoryStack_Direction_Previous:
            {
            direction = EHistoryStackDirectionPrevious;
            break;
            }
        case NW_HED_HistoryStack_Direction_Current:
        default:
            {
            direction = EHistoryStackDirectionCurrent;
            break;
            }
        }
    return direction;
    }



