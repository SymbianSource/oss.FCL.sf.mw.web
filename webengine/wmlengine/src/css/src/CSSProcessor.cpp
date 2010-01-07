/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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
#include "nwx_assert.h"
#include "nwx_string.h"
#include "nwx_logger.h"
#include "BrsrStatusCodes.h"
#include "CSSDeclListHandle.h"
#include "CSSHandler.h"
#include "CSSRuleList.h"
#include "CSSImageList.h"
#include "LMgrBoxTreeListener.h"
#include "TKeyArrayPtr.h"
#include "urlloader_urlresponse.h"
#include "nw_css_processori.h"
#include "HEDDocumentListener.h"
#include "nw_hed_contenthandler.h"
#include "nw_hed_documentroot.h"
#include "nw_hed_iloadrecipienti.h"
#include "nw_hed_loader.h"
#include "nw_adt_resizablevector.h"
#include "nw_text_ucs2.h"
#include "nw_gdi_utils.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_simplepropertylist.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nwx_settings.h"
#include "nwx_http_defs.h"
#include "GDIDeviceContext.h"
#include <bodypart.h>
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "MVCShell.h"
//#include "BrCtl.h"

struct TLoadListEntry
{
  NW_Uint16 iTransactionId;
  NW_Uint8      iLoadType;
  NW_Buffer_t*  iPartialChunkBuffer; // owns
};
typedef CArrayPtrFlat<TLoadListEntry> CCSSLoadList;
/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------------- */
static NW_LMgr_Box_t*
NW_CSS_Processor_BoxLookupByNode( NW_CSS_Processor_t* thisObj, NW_DOM_ElementNode_t* imageNode )
  {
  //lint -esym(550, status) Symbol not accessed

  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( imageNode != NULL );

  NW_LMgr_Box_t* imageBox = NULL;

  NW_TRY( status )
    {
    NW_LMgr_Box_t* box = NULL;
    NW_LMgr_BoxVisitor_t boxVisitor;
    NW_HED_DocumentRoot_t* documentRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( thisObj->owner );
    NW_THROW_ON_NULL( documentRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( documentRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    // create a box visitor to travel through the tree
    if( NW_LMgr_BoxVisitor_Initialize( &boxVisitor, NW_LMgr_BoxOf( rootBox ) ) == KBrsrSuccess )
      {
      while( ( box = NW_LMgr_BoxVisitor_NextBox( &boxVisitor, 0 ) )  != NULL )
        {
        NW_LMgr_PropertyList_t* boxPropList;

        boxPropList = NW_LMgr_Box_PropListGet( box );
        // It must be simple property
        if( boxPropList && NW_Object_IsInstanceOf( boxPropList, &NW_LMgr_SimplePropertyList_Class ) == NW_TRUE )
          {
          // check if this is the node that we are looking for
          if( (NW_DOM_ElementNode_t*)NW_LMgr_SimplePropertyListOf( boxPropList )->domNode == imageNode )
            {
            // return
            imageBox = box;
            break;
            }
          }
        }
      }
    }
  NW_CATCH( status )
    {
    imageBox = NULL;
    }
  NW_FINALLY
    {
    return imageBox;
    }
  NW_END_TRY
  }

static TBrowserStatusCode
NW_CSS_Processor_HandleCSSFetchDisabled( NW_CSS_Processor_t* thisObj,
                                         NW_Text_t* url,
                                         NW_Bool isSavedDeck,
                                         NW_Bool isHistLoad,
										 NW_Bool isReLoad,
                                         TInt cssBufferIndex)
{
  NW_CSS_Processor_PendingLoad_t* pendingLoad = NULL;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (url != NULL);

  NW_TRY (status) {
    void* dynEntry;

    pendingLoad = (NW_CSS_Processor_PendingLoad_t*) NW_Mem_Malloc(sizeof *pendingLoad);
    NW_THROW_OOM_ON_NULL (pendingLoad, status);

    pendingLoad->isSavedDeck = isSavedDeck;
    pendingLoad->isHistLoad = isHistLoad;
	pendingLoad->isReLoad = isReLoad;
    pendingLoad->url = NW_Text_Copy(url, NW_TRUE);
    pendingLoad->cssBufferIndex = cssBufferIndex;

    if(thisObj->pendingLoads == NULL)
    {
      thisObj->pendingLoads = (NW_ADT_DynamicVector_t*)
        NW_ADT_ResizableVector_New (sizeof (NW_CSS_Processor_PendingLoad_t*), 5, 10);
      NW_THROW_OOM_ON_NULL (thisObj->pendingLoads, status);
    }
    dynEntry = NW_ADT_DynamicVector_InsertAt (thisObj->pendingLoads, &pendingLoad,
                                           NW_ADT_Vector_AtEnd);
    NW_THROW_OOM_ON_NULL (dynEntry, status);
  }

  NW_CATCH (status) {
    NW_ADT_Vector_Metric_t size;
    NW_ADT_Vector_Metric_t index;

    NW_Object_Delete(pendingLoad->url);
    NW_Mem_Free (pendingLoad);
    size =  NW_ADT_Vector_GetSize(thisObj->pendingLoads);
    for (index =0; index <size; index++)
    {
      NW_CSS_Processor_PendingLoad_t* load;

      load = *(NW_CSS_Processor_PendingLoad_t**)
        NW_ADT_Vector_ElementAt (thisObj->pendingLoads, index);
      NW_Object_Delete(load->url);
      NW_Mem_Free(load);
    }

    NW_Object_Delete(thisObj->pendingLoads);
  }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/* -------------------------------------------------------------------------*/
static
NW_Cache_Mode_t NW_CSS_Processor_GetCacheMode(NW_Bool isSavedDeck, NW_Bool isHistLoad, NW_Bool isReLoad)
{
    if (isSavedDeck)
        return NW_CACHE_ONLYCACHE;
    else if(isHistLoad)
        return NW_CACHE_HISTPREV;
	else if (isReLoad)
		return NW_CACHE_NOCACHE;
    return NW_CACHE_NORMAL;

}


/* -------------------------------------------------------------------------*/
static
NW_Uint32
NW_CSS_Processor_ComputeEncoding(NW_Byte* data,
                                 NW_Uint32 length,
                                 NW_Uint32 encoding)
{
  /*
    The goal here is to permit the content author to do something
    which is within current specifications and can be depended on to
    work.

    If the server and loader can identify the charset as something
    other than Latin-1 then that charset is accepted as accurate.

    If the charset is Latin-1 (ISO-8859-1) then this either means that
    the server and loader accurately communicated the charset as
    Latin-1 or they did not communicate and the charset defaulted to
    Latin-1.  Thus, Latin-1 cannot be relied on to be accurate.  So
    for Latin-1 we apply the following constraint to determine the
    charset:

    Content authors must place a byte order mark (BOM) at the
    beginning of the document only if the encoding is UCS-2 or UTF-8.

    If the first bytes are:

    0xFE 0xFF ## ## where the two ## bytes are not both zero then the
    encoding is UCS-2 (UCS-2 is roughly a subset of UTF-16 in our use)

    0xEF 0xBB 0xBF then the encoding is UTF-8

    otherwise the encoding is left as Latin-1.

    Differentiating ASCII from Latin-1 is never an issue because it is
    a subset of Latin-1 which is the default.

    You can find a discussion of this type of encoding detection at
    http://www.w3.org/TR/2000/REC-xml-20001006#sec-guessing
  */
  if ((encoding != HTTP_iso_8859_1) || (data == NULL))
  {
    return encoding;
  }
  if (length >= 3) {
    if ((data[0] == 0xef) && (data[1] == 0xbb) && (data[2] == 0xbf)) {
      return HTTP_utf_8;
    }
  }
  if (length >= 4) {
    if (((data[0] == 0xfe) && (data[1] == 0xff))
        && ((data[2] != 0x00) || (data[3] != 0x00))) {
      return HTTP_iso_10646_ucs_2;
    }
  }
  return encoding;
}


/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_CSS_Processor_ProcessExternalBuffer(NW_CSS_Processor_t* thisObj,
                                       NW_Byte* data,
                                       NW_Uint32 length,
                                       NW_Uint32 charset,
                                       TInt bufferIndex)
{
  CCSSBufferList* bufferList;
  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (data != NULL);

  NW_TRY(status)
  {
    /* for convenience */
    bufferList = ruleList->GetBufferList();

    if (charset == 0)
    {/* implies - value not found in header */
      charset = bufferList->GetEncoding(bufferIndex);
      if (charset ==0){/* default to Latin1 */
        charset = HTTP_iso_8859_1;
      }
    }
    charset = NW_CSS_Processor_ComputeEncoding(data, length, charset);
    ruleList->SetCurrentBufferIndex(bufferIndex);

    TCSSParser parser(data, length, charset);
    charset = parser.ParseCharset();

    bufferList->UpdateEntry(data, length, charset, bufferIndex);

    // Parse the complete style sheet -- ignore all problems except
    // out of memory.  This allows it to apply at least some of the
    // style.
    status =  parser.ParseStyleSheet((MCSSEventListener*)(ruleList));
    NW_THROW_ON(status, KBrsrOutOfMemory);

    /* finally apply stylesheet */
    status = NW_CSS_Processor_ApplyStyleSheet(thisObj);
  }
  NW_CATCH (status)
  {}
  NW_FINALLY
  {
    ruleList->SetCurrentBufferIndex(-1);
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------*/
static
TBrowserStatusCode
NW_CSS_Processor_HandleCSSLoadComplete( NW_CSS_Processor_t* thisObj, NW_Uint16 transactionId,
                                        NW_Url_Resp_t* response, void* clientData )
  {
  TLoadListEntry* findEntry = NULL;

  NW_ASSERT( thisObj != NULL );

  NW_TRY( status )
    {
    CCSSLoadList* loadList = (CCSSLoadList*)(thisObj->iLoadList);
    TInt index = 0;    

    NW_ASSERT( response != NULL );

    findEntry = (TLoadListEntry*)NW_Mem_Malloc( sizeof( TLoadListEntry ) );
    NW_THROW_OOM_ON_NULL( findEntry, status );

    findEntry->iTransactionId = transactionId;
    TNumKeyArrayPtr matchkey( _FOFF( TLoadListEntry, iTransactionId ), ECmpTUint16 );
    NW_CSS_Processor_LoadContext_t* loadContext = (NW_CSS_Processor_LoadContext_t*) clientData;

    // if found the element remove it
    if( loadList->FindIsq( findEntry, matchkey, index ) == 0 )
      {
      TLoadListEntry* tempEntry = loadList->At( index );

      // If the image has not loaded the PartialChunkBuffer will be
      // null. Rather than crash we should handle it.
      NW_Byte* data;
      NW_Uint32 length;
      if (tempEntry->iPartialChunkBuffer)
        {
        data = tempEntry->iPartialChunkBuffer->data;
        length = tempEntry->iPartialChunkBuffer->length;
        }
      else
        {
        data = NULL;
        length = 0;
        }

      // css markup itself
      if( tempEntry->iLoadType == NW_CSS_Processor_LoadCSS )
        {
        NW_Http_CharSet_t charset = response->charset;
        // find out if the server response contained any charset
        // This gets the highest priority
        if( NW_Settings_GetEncoding() == EAutomatic )
          {
          charset = response->charset;
          }
        if (data)
            {
            status = NW_CSS_Processor_ProcessExternalBuffer( thisObj, data, length,
                              charset, loadContext->bufferIndex );
            NW_ASSERT( status == KBrsrSuccess );
            // NW_CSS_Processor_ProcessExternalBuffer takes data ownership
            if( status == KBrsrSuccess )
                {
                tempEntry->iPartialChunkBuffer->data = NULL;
                tempEntry->iPartialChunkBuffer->length = 0;
                }
            }
        }
      // it is an image
      else if( tempEntry->iLoadType == NW_CSS_Processor_LoadImage )
        {
        NW_Text_UCS2_t* url;

        url =
          NW_Text_UCS2_New( response->uri, 0, NW_Text_Flags_TakeOwnership );
        if( url == NULL )
          {
          NW_Mem_Free( data );
          status = KBrsrOutOfMemory;
          }
        else
          {
          // make a deep copy of the data 
          NW_Byte* retData = (NW_Byte*) NW_Mem_Malloc( length );
          if (retData == NULL)
                {
                  NW_Mem_Free( data );
                  status = KBrsrOutOfMemory;
                }
          NW_Mem_memcpy( retData, data, length );

          status = NW_CSS_Processor_SetImage( thisObj, retData, length,
                                             response->contentTypeString, NW_TextOf( url ) );
          response->uri = NULL;
  	      if( status != KBrsrSuccess )
            {
            NW_Mem_Free( data );
            }
          NW_Object_Delete( url );
          }
        }
      else
        {
        NW_Mem_Free( data );
        }
      loadList->Delete( index );
      NW_Buffer_Free(tempEntry->iPartialChunkBuffer);
      NW_Mem_Free( tempEntry );
      loadList->Compress();
      }
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    NW_Mem_Free( findEntry );

    // free response
    UrlLoader_UrlResponseDelete( response );
    return status;
    }
  NW_END_TRY
  }

/* -------------------------------------------------------------------------*/
static
TBrowserStatusCode
NW_CSS_Processor_BufferPartialChunk( NW_CSS_Processor_t* thisObj, NW_Uint16 transactionId,
                                     NW_Int32 chunkIndex, NW_Url_Resp_t* response  )
  {
  TLoadListEntry* findEntry = NULL;
  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( response != NULL );

  NW_TRY( status )
    {
    CCSSLoadList* loadList = (CCSSLoadList*)( thisObj->iLoadList );

    if( response->body != NULL )
      {
      TInt index = 0;

      findEntry = (TLoadListEntry*)NW_Mem_Malloc( sizeof( TLoadListEntry ) );
      NW_THROW_OOM_ON_NULL( findEntry, status );

      findEntry->iTransactionId = transactionId;
      // look-up the element in the load list
      TNumKeyArrayPtr matchkey( _FOFF( TLoadListEntry, iTransactionId ), ECmpTUint16 );
      if( loadList->FindIsq( findEntry, matchkey, index ) == 0 )
        {
        TLoadListEntry* tempEntry = loadList->At( index );
        // first chunk
        if( chunkIndex == 0 )
          {
          // pass response body ownership
          tempEntry->iPartialChunkBuffer = response->body;
          response->body = NULL;
          loadList->At( index ) = tempEntry;
          }
        else if( chunkIndex != -1 )
          {
          // append body to the buffer
          status = NW_Buffer_AppendBuffers( tempEntry->iPartialChunkBuffer, response->body );
          _NW_THROW_ON_ERROR( status );
          }
        else
          {
          // last partial response has no body. should not reach this code
          NW_ASSERT( NW_TRUE );
          }
        }
      else
        {
        // must find an item in the list
        NW_ASSERT( NW_TRUE );
        }
      }
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    NW_Mem_Free( findEntry );

    // free response
    UrlLoader_UrlResponseDelete( response );
    return status;
    }
  NW_END_TRY
  }

/* -------------------------------------------------------------------------*/
TBrowserStatusCode
NW_CSS_Processor_ProcessLoadRequest(NW_CSS_Processor_t* thisObj,
                                    NW_Text_t* url,
                                    NW_Uint8 loadType,
                                    NW_Bool isSavedDeck,
                                    NW_Bool isHistLoad,
									NW_Bool isReLoad,
                                    TInt cssBufferIndex)
{
  NW_CSS_Processor_LoadContext_t* loadContext = NULL;
  NW_Uint16 transactionId = 0;
  NW_Bool deleteLoadOnFailure = NW_FALSE;
  NW_HED_Loader_t *loader = NULL;
  NW_Ucs2* urlUcs2 = NULL;
  CBodyPart* bodyPart = NULL;
  TBool isUrlInMultipart = EFalse;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (url != NULL);

  NW_TRY (status)
  {
    NW_HED_DocumentRoot_t* docRoot =
      (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj->owner );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox )
      && !NW_Settings_GetCSSFetchEnabled() && loadType == NW_CSS_Processor_LoadCSS)
    {
      return NW_CSS_Processor_HandleCSSFetchDisabled(thisObj, url, isSavedDeck, isHistLoad, isReLoad, cssBufferIndex);
    }

    NW_HED_DocumentRoot_t *documentRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj->owner);
    loader = NW_HED_DocumentRoot_GetLoader (documentRoot);
    NW_ASSERT (loader != NULL);

    /* allocate and initialize the LoadContext */
    loadContext = (NW_CSS_Processor_LoadContext_t*)NW_Mem_Malloc (sizeof *loadContext);
    NW_THROW_OOM_ON_NULL (loadContext, status);
    loadContext->loadType = loadType;

    if (loadType == NW_CSS_Processor_LoadCSS)
    {
      loadContext->bufferIndex = cssBufferIndex;
    }

    NW_HED_ILoadRecipient_t *loadRecipient = (NW_HED_ILoadRecipient_t*)
      NW_Object_QueryInterface (thisObj, &NW_HED_ILoadRecipient_Class);
    NW_ASSERT (loadRecipient != NULL);

    if( NW_Object_IsInstanceOf (documentRoot->childNode, &NW_XHTML_ContentHandler_Class) )
        {
        // compare url with body part
        NW_Uint8 freeNeeded;
        urlUcs2 = NW_Text_GetUCS2Buffer( url, NULL, NULL, &freeNeeded);
        NW_XHTML_ContentHandler_t* xhtmlCH = (NW_XHTML_ContentHandler_t*) documentRoot->childNode;
        isUrlInMultipart = _NW_HED_CompositeContentHandler_IsUrlInMultipart( 
                                                 NW_HED_CompositeContentHandlerOf(xhtmlCH),
                                                 urlUcs2, &bodyPart );
        if( freeNeeded )
            {
            NW_Mem_Free( urlUcs2 );
            }
        }

    if( isUrlInMultipart )
        {
        TDataType dataType( bodyPart->ContentType() );

        TUint8* charset = (TUint8*) bodyPart->Charset().Ptr();
        TInt lenCh = bodyPart->Charset().Length();
        //R 
        /*CShell* shell = REINTERPRET_CAST(CShell*, NW_Ctx_Get(NW_CTX_BROWSER_APP, 0));  
        NW_ASSERT(shell);
        TInt32 uidInt = shell->GetUidFromCharset( charset, lenCh );
        TUid uid = TUid::Uid( uidInt );

         CBrCtl* brCtl = shell->BrCtl();
         
        brCtl->LoadDataL( bodyPart->Url(), bodyPart->Body(), dataType, uid, 
                          NULL, loadContext, loadRecipient, &transactionId, 
                          (NW_Uint8) (loadType == NW_CSS_Processor_LoadCSS ? NW_UrlRequest_Type_Css: NW_UrlRequest_Type_Image),
                          0);
        */
        }
    else
        {
        /* submit the load request */
        status = NW_HED_Loader_StartLoad (loader, url,
                                          NW_HED_UrlRequest_Reason_DocLoadChild,
                                          loadRecipient, loadContext,
                                          &transactionId,
                                          (NW_Uint8) (loadType == NW_CSS_Processor_LoadCSS ? NW_UrlRequest_Type_Css: NW_UrlRequest_Type_Image),
                                          NW_CSS_Processor_GetCacheMode(isSavedDeck, isHistLoad, isReLoad));
        NW_THROW_ON_ERROR (status);
        }
    deleteLoadOnFailure = NW_TRUE;

    /* store the transaction id in the load list */
    CCSSLoadList* loadList = (CCSSLoadList*)(thisObj->iLoadList);

    TLoadListEntry* entry = (TLoadListEntry*)NW_Mem_Malloc( sizeof( TLoadListEntry ) );
    NW_THROW_OOM_ON_NULL (entry, status);

    entry->iTransactionId = transactionId;
    entry->iLoadType = loadType;
    entry->iPartialChunkBuffer = NULL;

    TNumKeyArrayPtr matchkey(_FOFF(TLoadListEntry, iTransactionId), ECmpTUint16);
    TRAPD(ret, loadList->InsertIsqL(entry, matchkey));
    if (ret == KErrNoMemory)
    {
      NW_THROW_STATUS (status, KBrsrOutOfMemory);
    }
  }

  NW_CATCH (status) {
    if (deleteLoadOnFailure == NW_TRUE)
    {
      (void) NW_HED_Loader_DeleteLoad (loader, transactionId, NULL);
    }

    NW_Mem_Free (loadContext);
  }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_ProcessPendingLoads(NW_CSS_Processor_t* thisObj)
{
  NW_ADT_Vector_Metric_t numPending;
  NW_ADT_Vector_Metric_t index;
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (thisObj != NULL);

  if(thisObj->pendingLoads == NULL)
      return status;

  numPending = NW_ADT_Vector_GetSize (thisObj->pendingLoads);

  for( index = 0; index < numPending; index++)
  {
    NW_CSS_Processor_PendingLoad_t* load;
    load = *(NW_CSS_Processor_PendingLoad_t**)NW_ADT_Vector_ElementAt(thisObj->pendingLoads, index);
    status = NW_CSS_Processor_ProcessLoadRequest(thisObj,
                                                 load->url,
                                                 NW_CSS_Processor_LoadCSS,
                                                 load->isSavedDeck,
                                                 load->isHistLoad,
												 load->isReLoad,
                                                 load->cssBufferIndex);
    NW_Object_Delete(load->url);
    NW_Mem_Free(load);
    if(status == KBrsrOutOfMemory)
    {
      NW_Object_Delete(thisObj->pendingLoads);
      thisObj->pendingLoads = NULL;
      return status;
    }
  }
  NW_Object_Delete(thisObj->pendingLoads);
  thisObj->pendingLoads = NULL;
  return status;
}

NW_Bool
_NW_CSS_Processor_IImageObserver_IsVisible(NW_Image_IImageObserver_t* aImageObserver)
    {
    NW_REQUIRED_PARAM(aImageObserver);
    return NW_FALSE;
    }

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_CSS_Processor_Class_t NW_CSS_Processor_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_Object_Dynamic_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface,
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NW_CSS_Processor_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_CSS_Processor_t),
    /* construct                  */ _NW_CSS_Processor_Construct,
    /* destruct                   */ _NW_CSS_Processor_Destruct
  },
  { /* NW_CSS_Processor          */
    /* unused                    */ NULL
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const NW_CSS_Processor_InterfaceList[] = {
  &NW_CSS_Processor_ILoadRecipient_Class,
  &NW_CSS_Processor_IImageObserver_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_HED_ILoadRecipient_Class_t NW_CSS_Processor_ILoadRecipient_Class = {
  { /* NW_Object_Base         */
    /* super                  */ &NW_HED_ILoadRecipient_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface,
  },
  { /* NW_Object_Interface  */
    /* offset               */ offsetof (NW_CSS_Processor_t, NW_HED_ILoadRecipient)
  },
  { /* NW_HED_LoadRecipient */
    /* processPartialLoad    */  _NW_CSS_Processor_ILoadRecipient_ProcessPartialLoad
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Image_IImageObserver_Class_t NW_CSS_Processor_IImageObserver_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_Image_IImageObserver_Class,
    /* queryInterface       */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface  */
    /* offset               */ offsetof( NW_CSS_Processor_t, NW_Image_IImageObserver )
  },
  { /* NW_Image_IImageObserver */
    /* imageOpened          */ _NW_CSS_Processor_IImageObserver_ImageOpened,
    /* sizeChanged          */ _NW_CSS_Processor_IImageObserver_SizeChanged,
    /* imageOpeningStarted  */ _NW_CSS_Processor_IImageObserver_ImageOpeningStarted,
    /* imageDecoded         */ _NW_CSS_Processor_IImageObserver_ImageDecoded,
    /* imageDestroyed       */ _NW_CSS_Processor_IImageObserver_ImageDestroyed,
    /* isVisible            */ _NW_CSS_Processor_IImageObserver_IsVisible
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_CSS_Processor_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp)
{
  NW_CSS_Processor_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_CSS_Processor_Class));
  NW_ASSERT (argp != NULL);

  NW_REQUIRED_PARAM(argp);

  /* for convenience */
  thisObj = NW_CSS_ProcessorOf (dynamicObject);

  /* initialize member variables */
  thisObj->imageList = NULL;

  TRAPD(ret1, thisObj->iLoadList = new (ELeave)CCSSLoadList(1));
  if (ret1 == KErrNoMemory)
  {
    return KBrsrOutOfMemory;
  }
  thisObj->isSavedDeck = NW_FALSE;
  thisObj->isHistLoad = NW_FALSE;
  thisObj->isReLoad = NW_FALSE;
  TRAPD(ret2, thisObj->ruleList = CCSSRuleList::NewL(thisObj));
  if (ret2 == KErrNoMemory)
  {
    return KBrsrOutOfMemory;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------*/
void
_NW_CSS_Processor_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  // for convenience
  NW_CSS_Processor_t* thisObj = NW_CSS_ProcessorOf (dynamicObject);

  NW_HED_DocumentRoot_t* documentRoot =
    (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj->owner);
  NW_HED_Loader_t* loader = NW_HED_DocumentRoot_GetLoader (documentRoot);
  NW_ASSERT (loader != NULL);

  CCSSLoadList* loadList = (CCSSLoadList*)(thisObj->iLoadList);
  TInt index = loadList->Count();
  while (index-- > 0)
  {
    NW_CSS_Processor_LoadContext_t* loadContext = NULL;

    TLoadListEntry* entry = loadList->At(index);
    if (NW_HED_Loader_DeleteLoad (loader, entry->iTransactionId, &loadContext)== KBrsrSuccess)
    {
      NW_Mem_Free (loadContext);
    }
     // release partial chunk buffer
    if( entry->iPartialChunkBuffer != NULL )
      {
      NW_Buffer_Free( entry->iPartialChunkBuffer );
      }
    NW_Mem_Free (entry);
  }

  if(thisObj->pendingLoads != NULL)
  {
    NW_ADT_Vector_Metric_t size;
    NW_ADT_Vector_Metric_t i;

    size =  NW_ADT_Vector_GetSize(thisObj->pendingLoads);
    for (i =0; i <size; i++)
    {
      NW_CSS_Processor_PendingLoad_t* load;

      load = *(NW_CSS_Processor_PendingLoad_t**)
        NW_ADT_Vector_ElementAt (thisObj->pendingLoads, i);
      NW_Object_Delete(load->url);
      NW_Mem_Free(load);
    }
    NW_Object_Delete(thisObj->pendingLoads);
  }

  if (loadList != NULL)
	delete ((CCSSLoadList*)loadList);
  if (thisObj->imageList != NULL)
	delete ((CCSSImageList*)thisObj->imageList);
  if (thisObj->cssHandler != NULL)
	delete ((CCSSHandler*)thisObj->cssHandler);
  if (thisObj->ruleList != NULL)
	delete ((CCSSRuleList*)thisObj->ruleList);
}

/* -------------------------------------------------------------------------*
   NW_CSS_Processor methods
 * -------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_CSS_Processor_ILoadRecipient_ProcessPartialLoad(NW_HED_ILoadRecipient_t* loadListener,
                                             TBrowserStatusCode loadStatus,
                                             NW_Uint16 transactionId,
                                             NW_Int32 chunkIndex,
                                             NW_Url_Resp_t* response,
                                             NW_HED_UrlRequest_t* urlRequest,
                                             void* clientData)
    {
    NW_CSS_Processor_t* thisObj;
    TBrowserStatusCode status = KBrsrSuccess;
    
    NW_ASSERT (loadListener != NULL);
    NW_ASSERT (urlRequest != NULL);
    
    // avoid 'unrefereced formal parameter' warnings
    NW_REQUIRED_PARAM (transactionId);
    
    // parameter assertion block
    NW_ASSERT (NW_Object_IsInstanceOf (loadListener,
        &NW_CSS_Processor_ILoadRecipient_Class));
    NW_ASSERT (NW_Object_IsInstanceOf (urlRequest, &NW_HED_UrlRequest_Class));
    
    // get the implementer
    thisObj = (NW_CSS_Processor_t*)NW_Object_Interface_GetImplementer (loadListener);
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_CSS_Processor_Class));
    
    if( chunkIndex >= 0 )
        {
        status = NW_CSS_Processor_BufferPartialChunk( thisObj, transactionId, chunkIndex, response );
        }
    // last chunk
    else
        {
        if( loadStatus == KBrsrSuccess )
            {
            status = NW_CSS_Processor_HandleCSSLoadComplete( thisObj, transactionId, response, clientData );
            }
        // free request on last chunk
        NW_Object_Delete( urlRequest );
        // free the clientData
        NW_Mem_Free (clientData);
        }
    return status;
    }

/* ------------------------------------------------------------------------- *
   NW_CSS_Processor_IImageObserver_Class implementation
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_CSS_Processor_IImageObserver_ImageOpened( NW_Image_IImageObserver_t* imageObserver,
                                              NW_Image_AbstractImage_t* abstractImage,
                                              NW_Bool failed,
                                              NW_Int16 suppressDupReformats )
  {
  NW_TRY( status )
    {
    NW_CSS_Processor_t* thisObj;

    NW_REQUIRED_PARAM( suppressDupReformats );

    // parameter assertion block
    NW_ASSERT( imageObserver != NULL );
    NW_ASSERT( abstractImage != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( abstractImage,
                                      &NW_Image_Virtual_Class ));
    NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                      &NW_CSS_Processor_IImageObserver_Class ));

    // obtain the implementer
    thisObj = (NW_CSS_Processor_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_CSS_Processor_Class ) );
    NW_ASSERT( thisObj->imageList );

    NW_HED_DocumentRoot_t* docRoot =
      (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj->owner );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );
    // replace corresponding images on boxproperty.
    // we need to look up the node first so that we can get the box based
    // on the node information

    if( failed == NW_FALSE )
      {
      CCSSImageList* imageList = (CCSSImageList*)(thisObj->imageList);
      NW_DOM_ElementNode_t* imageNode = NULL;
      CCSSImageListEntry* imageEntry = imageList->EntryByVirtualImage( abstractImage, &imageNode );

      if( imageEntry )
        {
        NW_LMgr_PropertyName_t imagePropertyName = (NW_LMgr_PropertyName_t)
          ( imageEntry->ListItem() ? NW_CSS_Prop_listStyleImage : NW_CSS_Prop_backgroundImage );

        // get the corresponding box
        NW_LMgr_Box_t* imageBox = NW_CSS_Processor_BoxLookupByNode(
          thisObj, imageNode );
        NW_THROW_ON_NULL( imageBox, status, KBrsrFailure );

        NW_LMgr_Property_t imageProperty;
        // this box has the image. let's replace the image on it.
        if( NW_LMgr_Box_GetProperty( imageBox, imagePropertyName, &imageProperty ) == KBrsrSuccess )
          {
			if(imageProperty.value.object != abstractImage)
			  {
			  // replace the image on the box. the old image is freed by NW_LMgr_Box_SetProperty
			  imageProperty.value.object = (NW_Object_t*)abstractImage;
			  NW_LMgr_Box_SetProperty( imageBox, imagePropertyName, &imageProperty );
			  }
            // and remove the ownership from the image list
            imageList->RemoveVirtualImageOwnership( *imageEntry, abstractImage );
          }

        // and finally call decode or relayout based on the image type
        if( imageEntry->ListItem() )
          {
          // list item
          // relayout is needed
          NW_Image_IImageObserver_SizeChanged( imageObserver, abstractImage );
          }
        else
          {
          // background image
          // decode it unless it is not in the view

          NW_GDI_Rectangle_t imageRect;

          if( NW_LMgr_Box_GetVisibleBounds( imageBox, &imageRect ) == NW_TRUE )
            {
            // we need the rootbox to get the device context which then gives us the
            // current view coordinates
              CGDIDeviceContext* deviceContext =
                NW_LMgr_RootBox_GetDeviceContext( rootBox );
            NW_THROW_ON_NULL( deviceContext, status, KBrsrFailure );

            NW_GDI_Rectangle_t viewRect = deviceContext->ClipRect();

            if( NW_GDI_Rectangle_Cross( &imageRect, &viewRect, NULL ) )
              {
              // force decode
              (void)NW_Image_AbstractImage_ForceImageDecode( abstractImage );
              }
            }
          }
        }
      }
    // removed from the queue
    NW_LMgr_RootBox_ImageOpened( rootBox );
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    return;
    }
  NW_END_TRY
  }


/* ------------------------------------------------------------------------- */
void
_NW_CSS_Processor_IImageObserver_SizeChanged( NW_Image_IImageObserver_t* imageObserver,
                                              NW_Image_AbstractImage_t* abstractImage )
  {
  NW_TRY( status )
    {
    NW_CSS_Processor_t* thisObj;

    NW_REQUIRED_PARAM( abstractImage );

    // parameter assertion block
    NW_ASSERT( imageObserver != NULL );
    NW_ASSERT( abstractImage != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( abstractImage,
                                      &NW_Image_Virtual_Class ));
    NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                      &NW_CSS_Processor_IImageObserver_Class ));

    // obtain the implementer
    thisObj = (NW_CSS_Processor_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT( thisObj->owner != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_CSS_Processor_Class ) );

    NW_HED_DocumentRoot_t* docRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( thisObj->owner );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    // get image box
    CCSSImageList* imageList = (CCSSImageList*)(thisObj->imageList);
    NW_DOM_ElementNode_t* imageNode = NULL;
    CCSSImageListEntry* imageEntry = imageList->EntryByVirtualImage( abstractImage, &imageNode );

    if( imageEntry )
      {
      // get the corresponding box
      NW_LMgr_Box_t* box = NW_CSS_Processor_BoxLookupByNode(
        thisObj, imageNode );

      NW_THROW_ON_NULL( box, status, KBrsrFailure );
      // table performance: to prevent additional relayouts on the entire page (max pass of the automatic
      // table algorithm), images update parent table size by setting a flag on the table. the actual resize
      // happens when a relayout event is generated.
      while( box->parent != NULL )
        {
        // find the parent table.
        if( NW_Object_IsClass( box->parent, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
          {
          NW_LMgr_StaticTableBox_TableSizeChanged( NW_LMgr_StaticTableBoxOf( box->parent ) );
          break;
          }
        box = NW_LMgr_BoxOf( box->parent );
        }
      }
    NW_LMgr_RootBox_SetNeedsRelayout( rootBox );
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    return;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
void
_NW_CSS_Processor_IImageObserver_ImageOpeningStarted( NW_Image_IImageObserver_t* imageObserver,
                                                      NW_Image_AbstractImage_t* abstractImage )
  {
  NW_TRY( status )
    {
  NW_CSS_Processor_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );
  /* parameter assertion block */
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage,
                                    &NW_Image_Virtual_Class ));
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &NW_CSS_Processor_IImageObserver_Class ));

  /* obtain the implementer */
  thisObj = (NW_CSS_Processor_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_CSS_Processor_Class ) );

    NW_HED_DocumentRoot_t* docRoot =
      (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj->owner );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    NW_LMgr_RootBox_ImageOpeningStarted( rootBox );
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    return;
    }
  NW_END_TRY
  }


/* ------------------------------------------------------------------------- */
void
_NW_CSS_Processor_IImageObserver_ImageDecoded( NW_Image_IImageObserver_t* imageObserver,
                                               NW_Image_AbstractImage_t* abstractImage,
                                               NW_Bool failed)
  {
  NW_REQUIRED_PARAM(failed);

  NW_TRY( status )
    {
    NW_CSS_Processor_t* thisObj;

    /* parameter assertion block */
    NW_ASSERT( imageObserver != NULL );
    NW_ASSERT( abstractImage != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( abstractImage,
                                      &NW_Image_Virtual_Class ));
    NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                      &NW_CSS_Processor_IImageObserver_Class ));

    /* obtain the implementer */
    thisObj = (NW_CSS_Processor_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_CSS_Processor_Class ) );
    NW_ASSERT( thisObj->owner != NULL );
    NW_ASSERT( thisObj->imageList != NULL );

    // refresh corresponding image box.

    // first get the image list entry
    CCSSImageList* imageList = (CCSSImageList*)(thisObj->imageList);
    NW_DOM_ElementNode_t* imageNode;
    CCSSImageListEntry* imageEntry = imageList->EntryByVirtualImage( abstractImage, &imageNode );

    if( imageEntry )
      {
      NW_LMgr_Box_t* imageBox = NW_CSS_Processor_BoxLookupByNode( thisObj, imageNode );
      NW_THROW_ON_NULL( imageBox, status, KBrsrFailure );

      NW_LMgr_Box_Refresh( imageBox );
      }
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    return;
    }
  NW_END_TRY
  }


/* ------------------------------------------------------------------------- */
void
_NW_CSS_Processor_IImageObserver_ImageDestroyed( NW_Image_IImageObserver_t* imageObserver,
                                                 NW_Image_AbstractImage_t* abstractImage )
  {
  NW_CSS_Processor_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage,
                                    &NW_Image_Virtual_Class ));
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &NW_CSS_Processor_IImageObserver_Class ));

  /* obtain the implementer */
  thisObj = (NW_CSS_Processor_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_CSS_Processor_Class ) );
  NW_ASSERT( thisObj->imageList != NULL );

  CCSSImageList* imageList = (CCSSImageList*)(thisObj->imageList);
  CCSSImageListEntry* imageEntry = imageList->EntryByVirtualImage( abstractImage, NULL );

  if( imageEntry )
    {
    imageList->VirtualImageDestroyed( *imageEntry, abstractImage );
    }
  }


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* Add an entry for the buffer to be received. This will be called when LINK
 * or import statement is encountered. This is so because, we issue a load request
 * and then wait for the response to come. But since in CSS the order of
 * statements matter, we want to save an entry
 */
TBrowserStatusCode
NW_CSS_Processor_Load(NW_CSS_Processor_t* thisObj,
                      NW_Text_t* url,
                      NW_Text_t* encoding,
                      NW_Bool isSavedDeck,
                      NW_Bool isHistLoad,
					  NW_Bool isReLoad)
{
  NW_Uint32 charset;
  CCSSBufferList* bufferList;
  NW_Ucs2* urlStr = NULL;
  NW_Text_Length_t urlLen;
  NW_Bool freeNeeded = NW_FALSE;
  TInt cssBufferIndex = 0;
  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;

  NW_ASSERT(thisObj);
  if (url == NULL){
    return KBrsrSuccess;
  }
  charset = 0;
  if (encoding != NULL){
    const NW_Ucs2* storage;
    NW_Text_Length_t length;

    storage = NW_Text_GetUCS2Buffer (encoding, 0, &length, NULL);
    /* find integer representation for charset string */
    TCSSReader reader((TText8*)storage, (length+1)*2, HTTP_iso_10646_ucs_2);

    charset = reader.GetCharsetVal();
    NW_Object_Delete(encoding);
  }

  bufferList = ruleList->GetBufferList();

  urlStr = NW_Text_GetUCS2Buffer (url, 0, &urlLen, &freeNeeded);

  TRAPD(ret, bufferList->AddL(NULL, 0, charset, NW_FALSE, urlStr));
  cssBufferIndex = bufferList->GetSize() - 1;
  if (freeNeeded)
  {
    NW_Str_Delete(urlStr);
  }
  if (ret == KErrNoMemory)
  {
    return KBrsrOutOfMemory;
  }

  return NW_CSS_Processor_ProcessLoadRequest(thisObj,
                                             NW_TextOf(url),
                                             NW_CSS_Processor_LoadCSS,
                                             isSavedDeck,
                                             isHistLoad,
											 isReLoad,
                                             cssBufferIndex);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_ApplyStyles(NW_CSS_Processor_t* thisObj,
                             NW_DOM_ElementNode_t* aElementNode,
                             NW_LMgr_Box_t* aBox,
                             const NW_Evt_Event_t* aEvent,
                             NW_HED_DomHelper_t* aDomHelper)
{
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (aElementNode != NULL);
  NW_ASSERT (aBox != NULL);
  NW_ASSERT (aDomHelper != NULL);

  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;

  TRAPD(ret, ruleList->ApplyStylesL(aElementNode, aBox, aEvent, aDomHelper));
  return (ret==KErrNoMemory ? KBrsrOutOfMemory : KBrsrSuccess);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_ResolveURL (NW_CSS_Processor_t* thisObj, NW_Text_t* url, NW_Text_t** retUrl,
                             TInt aBufferIndex)
    {
    if (aBufferIndex == -1)
        {
        // inline style does not have a buffer index. Resolve the url with the markup url
        *retUrl = NULL;
        return KBrsrSuccess;
        }
    // CSS2 spec at http://www.w3.org/TR/1998/REC-CSS2-19980512/syndata.html#value-def-uri
    // chapter 4.3.4 specifies that relative url of a background image has to be resolved 
    // against the absolute url of the css and not the html

    NW_Uint8 freeLoadUrl = NW_FALSE;
    CCSSBufferList* bufferList;
    CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;
    bufferList = ruleList->GetBufferList();

    TInt count = bufferList->Count();

    if (count <= aBufferIndex)
        {
        // Cannot find the entry in the load list
        NW_ASSERT(0);
        *retUrl = NULL;
        return KBrsrSuccess;
        }
        
    const NW_Ucs2* docUrl = bufferList->EntryUrl(aBufferIndex);
    if (docUrl == NULL)
        {
        // Cannot find the entry in the load list
        *retUrl = NULL;
        return KBrsrSuccess;
        }

    const NW_Ucs2* loadUrl = NULL;
    NW_Text_UCS2_t* absUrlText = NULL;
    NW_Ucs2* absUrl = NULL;
    NW_Bool isRelative;
    
    NW_TRY(status)
        {
        /* get the url as a ucs2 char string */
        loadUrl = NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Aligned, NULL, &freeLoadUrl);
        NW_THROW_OOM_ON_NULL((void*)loadUrl, status);
        
        if(!NW_Text_GetCharCount(url))
            {
            isRelative = NW_TRUE;
            }
        else
            {
            /* if the supplied URL is absolute we simply return that */
            status = NW_Url_IsRelative (loadUrl, &isRelative);
            NW_THROW_ON_ERROR (status);
            }
        if ( isRelative == NW_FALSE)
            {
            if (freeLoadUrl)
                {
                NW_Mem_Free ((NW_Ucs2*) loadUrl);
                }
            /*make a deep copy of the text */
            *retUrl = NW_Text_Copy(url, NW_TRUE);
            NW_THROW_OOM_ON_NULL(*retUrl, status);
            return KBrsrSuccess;
            }
        
        if(!NW_Str_Strlen(loadUrl))
            {
            absUrl = NW_Str_Newcpy(docUrl);
            NW_THROW_OOM_ON_NULL(absUrl, status);
            }
        else
            {
            /* construct the absolute URL */
            status = NW_Url_RelToAbs (docUrl, loadUrl, &absUrl);
            NW_THROW_ON_ERROR (status);
            }
        
        absUrlText = NW_Text_UCS2_New (absUrl, 0, NW_Text_Flags_TakeOwnership);
        NW_THROW_OOM_ON_NULL(absUrlText, status);
        absUrl = NULL;
        
        /* fall through */
        
        }
        
        NW_CATCH (status)
            { /* empty */
            }
        
        NW_FINALLY {
            
            if (freeLoadUrl == NW_TRUE)
                {
                NW_Mem_Free ((NW_Ucs2*) loadUrl);
                }
            
            NW_Mem_Free (absUrl);
            
            *retUrl = (NW_Text_t*) absUrlText;
            return status;
            }
        NW_END_TRY
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_GetImage(NW_CSS_Processor_t* thisObj,
                          NW_Text_t* url,
                          NW_LMgr_Box_t* box,
                          NW_LMgr_PropertyName_t propName,
                          NW_Image_AbstractImage_t** image,
                          TInt aBufferIndex)
  {
  NW_Text_t* retUrl = NULL;
  TBrowserStatusCode status = KBrsrSuccess;
  CCSSImageList* imageList = NULL;
  TBool found = EFalse;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (url != NULL);
  NW_ASSERT (image != NULL);

  *image = NULL;
  status = NW_CSS_Processor_ResolveURL (thisObj, url, &retUrl, aBufferIndex);
  if (status == KBrsrSuccess)
      {
      if (retUrl == NULL)
          {
        // inline style
        status = NW_HED_ContentHandler_ResolveURL (thisObj->owner, url, &retUrl);
          }
      else
          {
          NW_Object_Delete(url);
          }
      }

  if (status != KBrsrSuccess)
    {
    NW_Object_Delete(url);
    return status;
    }
  // give ownership to url
  url = retUrl;
  retUrl = NULL;

  if (thisObj->imageList == NULL)
    {
    //  create the imageList
    TRAPD(ret, thisObj->imageList = CCSSImageList::NewL( *thisObj ));
    if (ret == KErrNoMemory)
      {
      //RETURN OOM
      // TODO: Should OOM be returned here? - vishy 8/13/2002
      NW_Object_Delete(url);
      return KBrsrOutOfMemory;
      }
    }
  imageList = (CCSSImageList*)(thisObj->imageList);

  TRAPD( ret, *image = imageList->GetImageL( url, found ) );
  if( ret == KErrNone)
    {
    // first check which node has this box
    NW_ASSERT( box != NULL );
    // lookup the dom node which is associated with the image
    NW_DOM_ElementNode_t* imageNode;
    NW_LMgr_PropertyList_t* boxPropList;

    boxPropList = NW_LMgr_Box_PropListGet( box );
    NW_ASSERT( boxPropList != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( boxPropList, &NW_LMgr_SimplePropertyList_Class ) );

    imageNode = (NW_DOM_ElementNode_t*)NW_LMgr_SimplePropertyListOf( boxPropList )->domNode;
    NW_ASSERT( imageNode != NULL );

    if( !found )
      {
      // no load has been requested on this image
      // let's create an entry in the image list and
      // issue the load

      // list item can be declared either as NW_CSS_Prop_listStyleImage or
      // NW_CSS_Prop_listStyle. NW_CSS_Prop_listStyle is the shortcut version
      // of NW_CSS_Prop_listStyleImage
      TBool listItem( propName == NW_CSS_Prop_listStyleImage ||
        propName == NW_CSS_Prop_listStyle );

      // and create an image entry
      TRAPD(ret, imageList->CreateImageEntryL( url, imageNode, listItem ) );
      if( ret != KErrNone )
        {
        // unable to create an image entry
        // what to do?
        // return NULL; ???
        }
      // Load url
      if( NW_Settings_GetImagesEnabled() )
        {
        status = NW_CSS_Processor_ProcessLoadRequest(thisObj,
          NW_TextOf(url),
          NW_CSS_Processor_LoadImage,
          thisObj->isSavedDeck,
          thisObj->isHistLoad,
		  thisObj->isReLoad,
          0);
        }
      }
    else
      {
      // image is either being loaded or has already been loaded.
      // let's create a virtual image
	  TInt ret;
      TRAP( ret, *image = imageList->AddVirtualNodeToImageEntryL( url, imageNode ) );
      NW_Object_Delete (url);
      }
    }
  else
    {
    NW_Object_Delete (url);
    }
  // KErrNoMemory to be handled

  if (ret == KErrNoMemory)
    {
    return KBrsrOutOfMemory;
    }
  return status;
  }

/*-------------------------------------------------------------------------*/
TBrowserStatusCode
NW_CSS_Processor_SetImage(NW_CSS_Processor_t* thisObj,
                          NW_Byte* data,
                          NW_Uint32 length,
                          NW_Http_ContentTypeString_t contentTypeString,
                          NW_Text_t* url)
{
  CCSSImageList* imageList = NULL;
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (url != NULL);

  if (data == NULL)
    {
    //No image data
    return status;
  }


  if (thisObj->imageList == NULL)
  {
    TRAPD(ret, thisObj->imageList = CCSSImageList::NewL( *thisObj ));
    if (ret == KErrNoMemory)
    {
      status = KBrsrOutOfMemory;
    }
  }
  if (thisObj->imageList)
  {
    imageList = (CCSSImageList*)(thisObj->imageList);
    TRAPD(ret, imageList->SetImageL(url, data, length, contentTypeString ));
    if (ret == KErrNoMemory)
    {
      status = KBrsrOutOfMemory;
    }
  }
  return status;
}

/*-------------------------------------------------------------------------*/
TBrowserStatusCode
NW_CSS_Processor_ShowImages(NW_CSS_Processor_t* thisObj)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (thisObj != NULL);

  if (thisObj->imageList != NULL)
  {
    CCSSImageList* imageList = (CCSSImageList*)thisObj->imageList;
    status = imageList->ShowImages();
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_ApplyStyleSheet(NW_CSS_Processor_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;
  NW_HED_ContentHandler_t* contentHandler = (NW_HED_ContentHandler_t*)(thisObj->owner);

  NW_TRY(status)
  {
    CCSSHandler* cssHandler = NULL;

    /* we don't want to reapply styles if there is no stylesheet */
    if ((ruleList->GetSize() ==0) || (contentHandler->boxTree == NULL))
    {
      NW_THROW_SUCCESS(status);
    }

    if (thisObj->cssHandler == NULL)
    {
      TRAPD(ret, thisObj->cssHandler = CCSSHandler::NewL(thisObj));
      if (ret != KErrNone)
      {
        NW_THROW_STATUS(status, KBrsrOutOfMemory);
      }
    }
    // let the view know about the css apply so that any outstanding format can
    // be cancelled
    NW_HED_DocumentRoot_t* documentRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( thisObj->owner );
    NW_THROW_ON_NULL( documentRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( documentRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    rootBox->boxTreeListener->StartCSSApply ();

    cssHandler = (CCSSHandler*)thisObj->cssHandler;
    TRAPD(ret, cssHandler->ApplyStylesL());
    if (ret == KErrNoMemory)
    {
      NW_THROW_STATUS(status, KBrsrOutOfMemory);
    }
    else if (ret != KErrNone)
    {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
  }
  NW_CATCH( status)
  {
  }
  NW_FINALLY
  {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
NW_CSS_Processor_Suspend(NW_CSS_Processor_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  if (thisObj->cssHandler)
  {
    CCSSHandler* cssHandler = (CCSSHandler*)thisObj->cssHandler;
    cssHandler->Cancel();
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_ApplyStyleAttribute(NW_CSS_Processor_t* thisObj, NW_Text_t* styleVal, NW_LMgr_Box_t* box)
{
  NW_Ucs2* storage;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (styleVal != NULL);
  NW_ASSERT (box != NULL);

  NW_TRY (status)
  {
    storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (styleVal, NW_Text_Flags_Aligned, &length,
                                       &freeNeeded);
    NW_THROW_OOM_ON_NULL (storage, status);

    TCSSParser parser((NW_Byte *)(storage), (length+1)*2, HTTP_iso_10646_ucs_2);

    TRAPD(ret, TCSSDeclListHandle::ApplyStylesL(box, &parser, thisObj, 
    /* inline style does not have a buffer index */-1));
    if (freeNeeded)
    {
      NW_Mem_Free((NW_Ucs2*)storage);
    }
    if (ret == KErrNoMemory)
    {
      NW_THROW_STATUS(status, KBrsrOutOfMemory);
    }
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_Uint32
NW_CSS_Processor_GetNumCSSImages(NW_CSS_Processor_t* thisObj)
{
  TInt numImages = 0;

  NW_ASSERT (thisObj != NULL);

  if (thisObj->imageList)
  {
    CCSSImageList* imageList = (CCSSImageList*)thisObj->imageList;
    numImages = imageList->GetNumImages();
  }
  return (NW_Uint32)numImages;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
NW_CSS_Processor_GetNumStyleSheets(NW_CSS_Processor_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  TInt numStyleSheets = 0;
  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;
  CCSSBufferList* bufferList = ruleList->GetBufferList();
  if (bufferList)
  {
    numStyleSheets = bufferList->GetExternalSize();
  }
  return (NW_Uint32)numStyleSheets;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_CSS_Processor_WriteMultipartSegment(NW_CSS_Processor_t* thisObj,
                                       NW_Osu_File_t fh,
                                       NW_Uint32 numCSSImages,
                                       NW_Uint32 numStyleSheets)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (thisObj != NULL);

  if (numStyleSheets > 0)
  {
    CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;
    CCSSBufferList* bufferList = ruleList->GetBufferList();

    status = bufferList->WriteMultipartSegments(fh);
    if (status == KBrsrOutOfMemory)
      return status;
  }
  if (numCSSImages > 0)
  {
    CCSSImageList* imageList = (CCSSImageList*)thisObj->imageList;
    status = imageList->WriteMultipartSegments(fh);
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_CSS_Processor_ProcessBuffer(NW_CSS_Processor_t* thisObj,
                       NW_Byte* data, NW_Uint32 length, NW_Uint32 encoding, NW_Bool doNotDelete)
{
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (data != NULL);

  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;

  return ruleList->ProcessBuffer(data, length, encoding, doNotDelete);
}

/* ------------------------------------------------------------------------- */
void
NW_CSS_Processor_SetDictionary(NW_CSS_Processor_t* thisObj, NW_WBXML_Dictionary_t* dictionary)
{
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (dictionary != NULL);

  CCSSRuleList* ruleList = (CCSSRuleList*)thisObj->ruleList;
  ruleList->SetDictionary(dictionary);
}

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */
NW_CSS_Processor_t*
NW_CSS_Processor_New()
{
  return (NW_CSS_Processor_t *)
    (NW_Object_New (&NW_CSS_Processor_Class));
}

/* -------------------------------------------------------------------------*/
static
TBrowserStatusCode
NW_CSS_Processor_ReplaceBox (NW_LMgr_Box_t** box,
                             NW_LMgr_PropertyValue_t *displayVal)
{
  NW_ASSERT (box != NULL);
  NW_ASSERT (displayVal != NULL);

  NW_LMgr_Box_t* containerBox = *box;
  NW_LMgr_ContainerBox_t* parentBox = containerBox->parent;
  NW_LMgr_Box_t* replacement = NULL;

  NW_TRY(status)
  {
    NW_LMgr_Property_t prop;
    NW_ADT_Vector_Metric_t childCount = 0;
    NW_ADT_Vector_Metric_t index = 0;
    NW_ADT_DynamicVector_t* children = NULL;

    if (displayVal->token == NW_CSS_PropValue_display_inline)
    {
      replacement = (NW_LMgr_Box_t*)NW_LMgr_ContainerBox_New(0);
    }
    else
    {
      replacement = (NW_LMgr_Box_t*)NW_LMgr_BidiFlowBox_New(0);
    }
    NW_THROW_OOM_ON_NULL (replacement, status);

    /* Take over the property list from the container box */
    if (replacement->propList != NULL)
    {
      NW_Object_Delete(replacement->propList);
    }
    replacement->propList = containerBox->propList;
    containerBox->propList = NULL;

    /* replace the old box with the new - first detach children as we need to
       transfer them later */
    children = NW_LMgr_ContainerBoxOf(containerBox)->children;
    NW_LMgr_ContainerBoxOf(containerBox)->children = NULL;
    status = NW_LMgr_ContainerBox_ReplaceChild(parentBox, containerBox, replacement);
    _NW_THROW_ON_ERROR (status);
    NW_Object_Delete(containerBox);
    containerBox = replacement;
    replacement = NULL;

    if (displayVal->token == NW_CSS_PropValue_display_marquee)
    {
      prop.type = NW_CSS_ValueType_Token;
      prop.value.token = NW_CSS_PropValue_hidden;
      status = NW_LMgr_Box_SetProperty(containerBox, NW_CSS_Prop_overflow, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      prop.type = NW_CSS_ValueType_Token;
      prop.value.token = NW_CSS_PropValue_display_block;
      status = NW_LMgr_Box_SetProperty(containerBox, NW_CSS_Prop_display, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      replacement = (NW_LMgr_Box_t*)NW_LMgr_MarqueeBox_New(0);
      NW_THROW_OOM_ON_NULL (replacement, status);

      prop.type = NW_CSS_ValueType_Token;
      prop.value.token = NW_CSS_PropValue_display_block;
      status = NW_LMgr_Box_SetProperty(replacement, NW_CSS_Prop_display, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      prop.type = NW_CSS_ValueType_Token;
      prop.value.token = NW_CSS_PropValue_nowrap;
      status = NW_LMgr_Box_SetProperty(replacement, NW_CSS_Prop_whitespace, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      prop.type = NW_CSS_ValueType_Px;
      prop.value.token = 0;
      status = NW_LMgr_Box_SetProperty(replacement, NW_CSS_Prop_borderWidth, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      prop.type = NW_CSS_ValueType_Px;
      prop.value.token = 0;
      status = NW_LMgr_Box_SetProperty(replacement, NW_CSS_Prop_padding, &prop);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(containerBox), replacement);
      if (status != KBrsrSuccess)
      {
        NW_Object_Delete(replacement);
      }
      else
      {
        containerBox = replacement;
      }
    }
    /* Transfer the children of the old box to the new replacement box */
    NW_Object_Delete (NW_LMgr_ContainerBoxOf(containerBox)->children);
    NW_LMgr_ContainerBoxOf(containerBox)->children = children;
    childCount = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(containerBox));

    /* update the parent pointer for all the children */
    for (index = 0; index < childCount; index++)
    {
      NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(containerBox), index);
      if (child)
        child->parent = NW_LMgr_ContainerBoxOf(containerBox);
    }

  }
  NW_CATCH (status)
  {
    *box = NULL;
     if(containerBox)
     {
        NW_Object_Delete(containerBox);
     }
     if (replacement)
     {
       NW_Object_Delete (replacement);
     }
  }
  NW_FINALLY
  {
   *box = containerBox;
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------*/
static
TBrowserStatusCode
NW_CSS_Processor_HandleListItemImage (NW_LMgr_Box_t** box)
{
  NW_ASSERT (box != NULL);

  NW_LMgr_ContainerBox_t* parentBox = (*box)->parent;
  NW_LMgr_BidiFlowBox_t* flowBox = NULL;

  NW_TRY(status)
  {
    flowBox = NW_LMgr_BidiFlowBox_New(0);
    NW_THROW_OOM_ON_NULL (flowBox, status);

    // Add Image or ImgContainer to the flowBox - this will take care of removing it
    // from the parentbox
    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(flowBox), *box);
    _NW_THROW_ON_ERROR (status);

    NW_LMgr_BoxOf(flowBox)->propList = (*box)->propList;
    (*box)->propList = NULL;
    *box = NW_LMgr_BoxOf(flowBox);
    status = NW_LMgr_ContainerBox_AddChild(parentBox, *box);
    _NW_THROW_ON_ERROR (status);
  }
  NW_CATCH (status)
  {
    if (flowBox)
    {
      NW_Object_Delete(flowBox);
      *box = NULL;
    }
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------*/
// Check display Val and replace box is necessary
TBrowserStatusCode
NW_CSS_Processor_HandleDisplayVal (NW_LMgr_Box_t** box, NW_LMgr_PropertyValue_t *displayVal)
{
  NW_ASSERT (box != NULL);
  NW_ASSERT (displayVal != NULL);

  NW_TRY (status)
  {
    // If box is image box, check the display and see if we need to replace it
    if( NW_Object_IsInstanceOf( *box, &NW_LMgr_ImgContainerBox_Class ) )
    {
      if (displayVal->token == NW_CSS_PropValue_display_listItem)
      {
        status = NW_CSS_Processor_HandleListItemImage(box);
      }
    }
    // If box is container box, check the display and see if we need to replace it
    else if (NW_Object_IsClass(*box, &NW_LMgr_ContainerBox_Class))
    {
      // if the display is block or list-item, or marquee, create a BidiFlowBox to replace the container box
      if (displayVal->token != NW_CSS_PropValue_display_inline)
      {
        // we need to replace the container box
        status = NW_CSS_Processor_ReplaceBox (box, displayVal);
      }
    }
    // If box is bidiflow box, check the display and see if we need to replace it
    else if (NW_Object_IsClass(*box, &NW_LMgr_BidiFlowBox_Class))
    {
      // if the display inline, create a ContainerBox to replace the BiDiFlow box
      if (displayVal->token == NW_CSS_PropValue_display_inline)
      {
        // we need to replace the container box
        status = NW_CSS_Processor_ReplaceBox (box, displayVal);
      }
      else if (displayVal->token == NW_CSS_PropValue_display_marquee)
      {
        status = NW_CSS_Processor_ReplaceBox (box, displayVal);
      }
    }
    _NW_THROW_ON_ERROR (status);
  }
  NW_CATCH (status)
  {
    *box = NULL;
  }
  NW_FINALLY
  {
    return status;
  } NW_END_TRY
}


#ifdef _BROWSER_ENG_DEBUG
#pragma message ("Browser eng debug is defined")

void NW_CSS_PropertyList_Dump (const NW_LMgr_SimplePropertyList_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  if (thisObj->map != NULL)
  {
    TBrowserStatusCode status;
    NW_Ucs2 nullProp[] = {'N', 'o', '-', 'n', 'a', 'm', 'e', '\0'};

    /* Iterate through the map to find all of the properties */
    TInt count = NW_LMgr_SimplePropertyList_Count(thisObj);
    for (TInt i = 0; i < count; i++)
    {
        const TCSSPropertyTablePropEntry* propEntry;
        NW_LMgr_PropertyName_t key;
        NW_LMgr_PropertyList_Entry_t entry;
        NW_Text_t* propval;

        status = NW_LMgr_SimplePropertyList_GetNameAt(thisObj, i, &key);
        if (status != KBrsrSuccess)
        {
          NW_LOG1(NW_LOG_LEVEL3, "%s", "Error. Could not retrieve property list entry.");
          continue;
        }

        /* get the text property name */
        propEntry = TCSSPropertyTable::GetPropEntryByToken(key);

        status = NW_LMgr_PropertyList_Get (thisObj, key, &entry);
        /* Only print property if status == success */
        if (status == KBrsrSuccess)
        {
          const NW_Ucs2* nameProp = nullProp;
          if (propEntry)
          {
            nameProp = propEntry->strName;
          }

          /* Display the appropriate representation of the property */
          switch (entry.type)
          {
          case NW_CSS_ValueType_Integer:
          case NW_CSS_ValueType_Px:
          case NW_CSS_ValueType_Token:
            /* print the integer name and value */
            NW_LOG2(NW_LOG_LEVEL3, "%s:%d; ",
                    nameProp,
                    entry.value.integer);
            break;
          case NW_CSS_ValueType_Color:
            /* print the color name and value in hex */
            NW_LOG2(NW_LOG_LEVEL3, "%s:#%06x; ",
                    nameProp,
                    entry.value.integer);
            break;
          case NW_CSS_ValueType_Number:
          case NW_CSS_ValueType_Em:
          case NW_CSS_ValueType_Ex:
          case NW_CSS_ValueType_In:
          case NW_CSS_ValueType_Cm:
          case NW_CSS_ValueType_Mm:
          case NW_CSS_ValueType_Pt:
          case NW_CSS_ValueType_Pc:
          case NW_CSS_ValueType_Percentage:
            /* print the floating point property name and value */
            NW_LOG2(NW_LOG_LEVEL3, "%s:%f; ",
                    nameProp,
                    entry.value.decimal);
            break;
          case NW_CSS_ValueType_Text:
            propval = (NW_Text_t*)entry.value.object;
            /* print the text property name and value */
            NW_LOG2(NW_LOG_LEVEL3, "%s:%s; ",
                    nameProp,
                    propval->storage);
            break;
          case NW_CSS_ValueType_Object:
          case NW_CSS_ValueType_Image:
          case NW_CSS_ValueType_Font:
          default:
            /* print the property name, but not its explicit value: just report "OBJECT" instead */
            NW_LOG1(NW_LOG_LEVEL3, "%s:<OBJECT>; ",
                    nameProp);
          }
        }
        else
        {
            /* print the property name, reporting the error */
            NW_LOG1(NW_LOG_LEVEL3, "%s:<ERROR>; ",
                    (propEntry==NULL?nullProp:propEntry->strName));
        }
    }
  }
}


/* ------------------------------------------------------------------------- */
void
NW_CSS_Processor_PrintProperties (NW_LMgr_Box_t *box)
{
  NW_Ucs2 typeBuf[128];

  NW_ASSERT (box != NULL);

  NW_Str_StrcpyConst(typeBuf, (const char* const)NW_LMgr_Box_GetBoxType(box));
  NW_LOG2(NW_LOG_LEVEL3, "PROPERTIES FOR: %s [%X]", typeBuf, box);

  if (NW_Object_IsInstanceOf(box->propList, &NW_LMgr_SimplePropertyList_Class))
  {
    NW_LMgr_SimplePropertyList_t* spropList;

    spropList = NW_LMgr_SimplePropertyListOf (box->propList);

    if (spropList != NULL)
    {
      NW_CSS_PropertyList_Dump(spropList);
    }
  }
}
#endif /* _BROWSER_ENG_DEBUG*/
