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

#include "nw_wml1x_epoc32contenthandleri.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"  
#include "nw_hed_compositecontenthandler.h"
#include "nwx_logger.h"
#include "nwx_osu_file.h"
#include "nw_loadreq.h"
#include "nwx_multipart_generator.h"
#include "Epoc32ImageDecoder.h"

#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_wml1x_wml1xapi.h"
#include "wml_elm_attr.h"
#include "wml_task.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"
#include "nwx_settings.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Wml1x_Epoc32ContentHandler_Class_t NW_Wml1x_Epoc32ContentHandler_Class = {
  { /* NW_Object_Core                       */
    /* super                                */ &NW_Wml1x_ContentHandler_Class,
    /* queryInterface                       */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                       */
    /* interfaceList                        */ NW_Wml1x_Epoc32_InterfaceList
  },
  { /* NW_Object_Dynamic                    */
    /* instanceSize                         */ sizeof (NW_Wml1x_Epoc32ContentHandler_t),
    /* construct                            */ _NW_Wml1x_ContentHandler_Construct,
    /* destruct                             */ NULL
  },
  { /* NW_HED_DocumentNode                  */
    /* cancel                               */ _NW_Wml1x_ContentHandler_Cancel,
    /* partialLoadCallback	                */ _NW_Wml1x_Epoc32ContentHandler_PartialLoadCallback,
    /* initialize                           */ _NW_Wml1x_ContentHandler_Initialize,
    /* nodeChanged                          */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                           */ _NW_Wml1x_ContentHandler_GetBoxTree,
    /* processEvent                         */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                          */ _NW_HED_DocumentNode_HandleError,
    /* suspend                              */ _NW_Wml1x_ContentHandler_Suspend,
    /* resume                               */ _NW_Wml1x_ContentHandler_Resume,
    /* allLoadsCompleted                    */ _NW_Wml1x_ContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted         */ _NW_Wml1x_ContentHandler_IntraPageNavigationCompleted,
    /* loseFocus                            */ _NW_Wml1x_ContentHandler_LoseFocus,
    /* gainFocus                            */ _NW_Wml1x_ContentHandler_GainFocus,
    /* handleLoadComplete                   */ _NW_Wml1x_Epoc32ContentHandler_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler                */
    /* partialNextChunk                     */ _NW_HED_ContentHandler_PartialNextChunk,
    /* getTitle                             */ _NW_Wml1x_ContentHandler_GetTitle,
    /* getUrl                               */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl                           */ _NW_HED_ContentHandler_ResolveURL,
    /* createHistoryEntry                   */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry              */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse                       */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree                        */ NULL,
    /* handleRequest                        */ _NW_Wml1x_ContentHandler_HandleRequest,
    /* featureQuery                         */ _NW_Wml1x_ContentHandler_FeatureQuery,
    /* responseComplete                     */ _NW_HED_ContentHandler_ResponseComplete
  },
  { /* NW_HED_CompositeContentHandler       */
    /* documentDisplayed                    */ _NW_HED_CompositeContentHandler_DocumentDisplayed
  },
  { /* NW_Wml1x_ContentHandler              */
    /* unused                               */ NW_Object_Unused
  },
  { /* NW_Wml1x_Epoc32ContentHandler        */
    /* unused                               */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const NW_Wml1x_Epoc32_InterfaceList[] = {
  &NW_Wml1x_Epoc32ContentHandler_ImageViewer_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_Markup_ImageViewer_Class_t NW_Wml1x_Epoc32ContentHandler_ImageViewer_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Markup_ImageViewer_Class,
    /* querySecondary            */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary       */
    /* offset                    */ offsetof (NW_Wml1x_Epoc32ContentHandler_t,
                                              NW_Markup_ImageViewer)
  },
  { /* NW_Object_Aggregate       */
    /* secondaryList             */ _NW_Markup_ImageViewer_SecondaryList,
    /* construct                 */ NULL,
    /* destruct                  */ NULL
  },
  { /* NW_Markup_ImageViewer     */
    /* load                      */ _NW_Wml1x_Epoc32ContentHandler_ImageViewer_Load,
    /* isNoSave                  */ _NW_Wml1x_Epoc32ContentHandler_ImageViewer_IsNoSave,
    /* getAltText                */ _NW_Wml1x_Epoc32ContentHandler_ImageViewer_GetAltText
  }
};


/*****************************************************************

  Name:         _NW_Wml1x_Epoc32ContentHandler_PartialLoadCallback()

  Description:  

  Parameters:   

  Algorithm:    

  Return Value: 

*****************************************************************/
TBrowserStatusCode
_NW_Wml1x_Epoc32ContentHandler_PartialLoadCallback( NW_HED_DocumentNode_t* documentNode,
                                                    TBrowserStatusCode loadStatus,
                                                    NW_Int32 chunkIndex,
                                                    NW_HED_UrlRequest_t* urlRequest,
                                                    NW_Url_Resp_t* response,
                                                    NW_Uint16 transactionId,
                                                    void* context )
  {  
  NW_REQUIRED_PARAM( transactionId );
  NW_REQUIRED_PARAM( loadStatus );

  NW_TRY( status ) 
    {
    NW_HED_DocumentNode_t* childNode;
    NW_Wml1x_ContentHandler_t  *thisObj;
    
    NW_ASSERT( documentNode != NULL );
    // for convenience
    thisObj = NW_Wml1x_ContentHandlerOf( documentNode );

    // first chunk
    if( chunkIndex == 0 )
      {
      NW_HED_ContentHandler_t* contentHandler;
      NW_ASSERT( response != NULL );

      // create content handler 
      contentHandler = NW_HED_CompositeContentHandler_CreateContentHandler( 
        NW_HED_ContentHandlerOf( documentNode ), 
        response->contentTypeString, urlRequest, response, context );
	  NW_THROW_ON_NULL( contentHandler, status, KBrsrFailure );

      // the only content we do support is image and wml script
      // forward first chunk to the content handler
      status = NW_HED_ContentHandler_PartialNextChunk( contentHandler, 
            chunkIndex, response, context );
      // child content handler takes the ownership of the response
      response = NULL;

      _NW_THROW_ON_ERROR( status );
      
      if( NW_Object_IsInstanceOf( contentHandler, &NW_ImageCH_Epoc32ContentHandler_Class) == NW_TRUE )
        {
        // set context on image content handler
        NW_ImageCH_Epoc32ContentHandlerOf( contentHandler )->context = context;
        // handle virtual images
        status = NW_HED_CompositeContentHandler_HandleImageLoad(
           NW_HED_CompositeContentHandlerOf( documentNode ),
           chunkIndex,
	         contentHandler,
           urlRequest );
        // catch only OOM
        NW_THROW_ON( KBrsrOutOfMemory, status );

#ifdef INCREMENTAL_IMAGE_ON
        // open the image
        NW_ImageCH_Epoc32ContentHandler_ForceImageOpen( NW_ImageCH_Epoc32ContentHandlerOf( contentHandler ) );
#endif // INCREMENTAL_IMAGE_ON
        }
      else if( NW_Object_IsInstanceOf( contentHandler, &NW_WmlScript_ContentHandler_Class ) == NW_TRUE )
        {
        //
        NW_THROW_SUCCESS( status );
        }
      else
        {
        //
        NW_THROW_SUCCESS( status );
        }
      } 
    // further chunks
    else 
      {
      // find the corresponding content handler. It must have been created when the
      // first chunk came in.
      NW_HED_CompositeNode_t* compositeNode = (NW_HED_CompositeNode_t*)
        (NW_Object_QueryAggregate( documentNode, &NW_HED_CompositeNode_Class));
      NW_ASSERT( compositeNode != NULL );

      childNode = NW_HED_CompositeNode_LookupChild( compositeNode, context );
      NW_THROW_ON_NULL( childNode, status, KBrsrFailure );

      if( chunkIndex != -1 )
        {
        // pass chunks to the content handler
        status = NW_HED_ContentHandler_PartialNextChunk( 
          NW_HED_ContentHandlerOf( childNode ), chunkIndex, response, context );        
        // child content handler takes the ownership of the response
        response = NULL;

        _NW_THROW_ON_ERROR( status );      
        }
      // last chunk
      else
        {
        // last chunk. content handler is all set. 
        // intialize it
        if( NW_Object_IsInstanceOf( childNode, &NW_ImageCH_Epoc32ContentHandler_Class ) )
          {
          // this takes care of virtual image content handlers
          status = NW_HED_CompositeContentHandler_HandleImageLoad(
             NW_HED_CompositeContentHandlerOf( documentNode ),
             chunkIndex,
	           NW_HED_ContentHandlerOf( childNode ),
             urlRequest );
          // catch only OOM
          NW_THROW_ON( KBrsrOutOfMemory, status );
          }
        thisObj->wmlInterpreter.outstandingLoadCount--;
        status = NW_HED_DocumentNode_Initialize( childNode, KBrsrSuccess );
        }
      }
    _NW_THROW_ON_ERROR( status );
    }
  NW_CATCH( status ) 
    {
    }
  NW_FINALLY 
    {
    // content handler has the response ownership, so
    // it must be released no matter what
    UrlLoader_UrlResponseDelete( response );

    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
// The function is called when a load issued by the document node is failed
void
_NW_Wml1x_Epoc32ContentHandler_HandleLoadComplete( NW_HED_DocumentNode_t* documentNode,
                                        const NW_HED_UrlRequest_t* urlRequest,
                                        NW_Int16 errorClass,
                                        TBrowserStatusCode error,
                                        NW_Uint16 transactionId,
                                        void* context )
  {
  NW_REQUIRED_PARAM( errorClass );
  NW_REQUIRED_PARAM( error );
  NW_REQUIRED_PARAM( transactionId );

  NW_ASSERT( documentNode != NULL );
  NW_ASSERT( urlRequest != NULL );

  // if the wml image was loaded as LOCALSRC and it failed then we
  // need to check if the image referred by the SRC is available.
  if( urlRequest->loadType == NW_UrlRequest_Type_Image && 
      urlRequest->loadMode == NW_HED_UrlRequest_LoadLocal ) 
    {    
    (void)LoadSrcImage( NW_Wml1x_ContentHandlerOf( documentNode ), 
      NW_UINT16_CAST( NW_INT32_CAST( context ) ) );
    }
  }

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/************************************************************************
 
  Function: NW_Wml1x_Epoc32ContentHandler_SaveToFile

  Description:  Write multipart document to file containing encoded wml.
                Multipart document consists of:
                  Number of segments
                  Segments - each with
                      header length (including content type length)
                      data length
                      content type (including character encoding)
                      headers
                      data
 
  Parameters:   thisObj  - in: content handler
                file     - in: file name
    
  Return Value: KBrsrSuccess
                KBrsrFailure
                KBrsrOutOfMemory
                KBrsrSavedPageFailed
                  
**************************************************************************/
TBrowserStatusCode
NW_Wml1x_Epoc32ContentHandler_SaveToFile(NW_Wml1x_Epoc32ContentHandler_t *thisObj,
                                         NW_Ucs2* file)
{
  NW_Wml1x_ContentHandler_t *wml1xch;
  NW_HED_DomTree_t* domTree = NULL;
  NW_Osu_File_t fh = NULL;
  NW_Buffer_t* buffer;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Uint32 numImages = 0;
  NW_Uint32 numSegments;
  NW_Byte numSegmentsBuf[5];
  NW_Uint32 numSegmentsBufLen;
  NW_Byte* p;
  NW_Byte* segment;
  NW_Uint32 segmentLen;
  NW_Ucs2* url;
  char* urlAscii = NULL;

  TUint originalEncoding = NW_Settings_GetOriginalEncoding();

  wml1xch = NW_Wml1x_ContentHandlerOf(thisObj);
  domTree = NW_Wml1x_ContentHandler_GetDomTree (wml1xch);
  NW_ASSERT(domTree);

  /* Delete any existing file */
  (void)NW_Osu_Delete(file);

  /* Create new file */
  fh = NW_Osu_CreateNewFile(file);
  if (fh == NW_INVALID_FILE_HANDLE)
  {
    status = KBrsrSavedPageFailed;
    goto cleanup;
  }

  /* Number of images; they are in a list of image content handlers. */
  if (wml1xch->super.NW_HED_CompositeNode.children != NULL)
    numImages = NW_HED_CompositeContentHandler_GetImageCount(NW_HED_CompositeContentHandlerOf(wml1xch));

  /* Get count of multipart segments (one for wmlc page, plus number of
   * images). Write variable length integer to a buffer, then write
   * buffer to file
   */
  numSegments = 1 + numImages;
  p = NW_StoreUint(numSegmentsBuf, numSegments);
  numSegmentsBufLen = p - numSegmentsBuf;
  status = NW_Osu_WriteFile(fh, numSegmentsBuf, numSegmentsBufLen);
  if (status != KBrsrSuccess)
    goto cleanup;

  /* Get response URL. If it is NULL, use it anyway. */
  url = (NW_Ucs2*) NW_HED_UrlResponse_GetRawUrl (NW_HED_ContentHandler_GetUrlResponse (wml1xch));
  if (url != NULL)
    urlAscii = NW_Str_CvtToAscii(url);

  /* Create multipart segment from wml document & write to file */
  buffer = NW_HED_DomTree_GetDomBuffer(domTree);
  status = NW_CreateMultipartSegment
            (
              NW_HED_DomTree_GetCharEncoding(domTree),
			  originalEncoding,
              (NW_Http_ContentTypeString_t)HTTP_application_wmlc_saved_string,
              urlAscii,
              buffer->data,
              buffer->length,
              &segment,
              &segmentLen
            );

  NW_Mem_Free(urlAscii);
  if (status != KBrsrSuccess)
    goto cleanup;

  status = NW_Osu_WriteFile(fh, segment, segmentLen);
  NW_Mem_Free(segment);
  if (status != KBrsrSuccess)
    goto cleanup;

  /* Write images to file */
  status = NW_HED_CompositeContentHandler_SaveImagesToFile(wml1xch, fh);

cleanup:
  NW_LOG1(NW_LOG_LEVEL3, "NW_Wml1x_Epoc32ContentHandler_SaveToFile: Result %d", status);
  NW_Osu_CloseFile(fh);

  if (status != KBrsrSuccess)
    (void)NW_Osu_Delete(file);

  return status;
}

/* ------------------------------------------------------------------------- *
   NW_Markup_ImageViewer methods
 * ------------------------------------------------------------------------- */

NW_Bool
_NW_Wml1x_Epoc32ContentHandler_ImageViewer_IsNoSave (NW_Markup_ImageViewer_t* imageViewer,
                                                     NW_LMgr_Box_t* imageBox)
{
  NW_Uint16 elementId;
  NW_LMgr_PropertyValue_t value; 
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Wml1x_ContentHandler_t* contentHandler;
  NW_Ucs2                   *alt = NULL;
  NW_Bool ret = NW_FALSE;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_Wml1x_Epoc32ContentHandler_ImageViewer_Class));
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsDerivedFrom (imageBox,
                                     &NW_LMgr_ImgContainerBox_Class) ||
             NW_Object_IsInstanceOf (imageBox,
                                     &NW_FBox_ImageButtonBox_Class));

  /* for convenience */
  contentHandler = (NW_Wml1x_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (imageViewer);
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_Wml1x_ContentHandler_Class));

  /* get into elementId the element id we set in the box's properties */
  status = NW_LMgr_Box_GetPropertyValue(imageBox, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);
  if (status != KBrsrSuccess) 
  {
    return NW_FALSE;
  }

  elementId = NW_UINT16_CAST(value.integer);
 
  /* get the ALT_ATTR value if any */
  status = NW_Wml_GetAttribute(&(contentHandler->wmlInterpreter), 
                                         elementId, 
                                         ALT_ATTR,
                                         &alt);
  if ((status != KBrsrSuccess) || (alt == NULL)) {
    return NW_FALSE;
  }

  if (!NW_Str_StricmpConst (alt, WAE_ASC_NOSAVE_STR) || !NW_Str_StricmpConst (alt, WAE_ASC_NOKIA_STR))
  {
    ret = NW_TRUE;
  }

  NW_Str_Delete(alt);
  alt = NULL;

  return ret;
}

/* ------------------------------------------------------------------------- */
NW_Ucs2*
_NW_Wml1x_Epoc32ContentHandler_ImageViewer_GetAltText (NW_Markup_ImageViewer_t* imageViewer,
                                                       NW_LMgr_Box_t* imageBox)
{
  NW_Uint16 elementId;
  NW_LMgr_PropertyValue_t value; 
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Wml1x_ContentHandler_t* contentHandler;
  NW_Ucs2                   *alt = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_Wml1x_Epoc32ContentHandler_ImageViewer_Class));
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsDerivedFrom (imageBox,
                                     &NW_LMgr_ImgContainerBox_Class) ||
             NW_Object_IsInstanceOf (imageBox,
                                     &NW_FBox_ImageButtonBox_Class));

  /* for convenience */
  contentHandler = (NW_Wml1x_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (imageViewer);
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_Wml1x_ContentHandler_Class));

  /* get into elementId the element id we set in the box's properties */
  status = NW_LMgr_Box_GetPropertyValue(imageBox, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);
  if (status != KBrsrSuccess) 
  {
    return NULL;
  }

  elementId = NW_UINT16_CAST(value.integer);
 
  /* get the ALT_ATTR value if any */
  status = NW_Wml_GetAttribute(&(contentHandler->wmlInterpreter), 
                                         elementId, 
                                         ALT_ATTR,
                                         &alt);
  if (status != KBrsrSuccess) {
    return NULL;
  }

  return alt;
}
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Wml1x_Epoc32ContentHandler_ImageViewer_Load (NW_Markup_ImageViewer_t* imageViewer)
{
  NW_Wml1x_ContentHandler_t* contentHandler;
  NW_LMgr_PropertyValue_t value; 
  NW_Uint16 elementId;
  NW_Wml_ElType_e           elType = UNKNOWN_ELEMENT;
  NW_Wml_Element_t          *el;
  NW_LMgr_Box_t              *box = NULL;
  NW_LMgr_Box_t              *rootBox = NULL;
  NW_LMgr_BoxVisitor_t       boxVisitor;
  TBrowserStatusCode                status = KBrsrSuccess;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_Wml1x_Epoc32ContentHandler_ImageViewer_Class));

  /* for convenience */
  contentHandler = (NW_Wml1x_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (imageViewer);
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_Wml1x_ContentHandler_Class));

  rootBox = NW_HED_ContentHandlerOf (contentHandler)->boxTree;   
  NW_ASSERT (rootBox != NULL);

  /* initialize the boxVisitor */
  status = NW_LMgr_BoxVisitor_Initialize (&boxVisitor, NW_LMgr_BoxOf (rootBox));
  if (status != KBrsrSuccess) {
    return status;
  }
  
  /* traverse the boxTree */
  while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL) 
  {
    if ((NW_Object_IsInstanceOf(box, &NW_LMgr_ContainerBox_Class) && 
        !NW_Object_IsInstanceOf(box->propList, &NW_LMgr_SlavePropertyList_Class))
         || NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class))        
    {   /* get into elementId the element id we set in the box's properties */
        status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);

    if (status == KBrsrSuccess) 
    {
      elementId = NW_UINT16_CAST(value.integer);
      status = NW_Wml_GetElementType(&(contentHandler->wmlInterpreter),
                                     elementId,
                                     &elType,
                                     &el);
      if (status != KBrsrSuccess)
        return status;

      if(elType == IMAGE_ELEMENT)
        status = NW_UI_LoadImage(contentHandler, elementId, NW_TRUE);
    }
   }
  }
  return KBrsrSuccess;
}
