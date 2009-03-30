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


#include "BrsrStatusCodes.h"
#include "nw_xhtml_epoc32contenthandleri.h"
#include "nw_structure_structuremodule.h"
#include "nw_text_textmodule1.h"
#include "nw_text_textmodule2.h"
#include "nw_hypertext_hypertextmodule.h"
#include "nw_list_listmodule.h"
#include "nw_basicforms_basicformsmodule.h"
#include "nw_basictables_basictablesmodule.h"
#include "nw_image_imagemodule.h"
#include "nw_metainformation_metainformationmodule.h"
#include "nw_link_linkmodule.h"
#include "nw_base_basemodule.h"
#include "nw_style_stylemodule.h"
#include "nw_frame_framemodule.h"
#include "nw_dom_domvisitor.h"
#include "nw_object_objectmodule.h"
#include "nw_hed_compositecontenthandler.h"

#include "nw_lmgr_rootbox.h"
#include "nw_imagech_epoc32contenthandler.h"
//#include "nw_ecma_contenthandler.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_loadreq.h"
#include "nwx_multipart_generator.h"
#include "nwx_logger.h"
#include "nw_hed_childiterator.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_image_epoc32simpleimage.h"
#include "nw_image_epoc32cannedimage.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"
#include "Epoc32ImageDecoder.h"

#include "HEDDocumentListener.h"
#include "nw_system_optionlist.h"

#include "nwx_settings.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Markup_Module_t* const _NW_XHTML_ContentHandler_Modules[] = {
  (const NW_Markup_Module_t*) &NW_XHTML_StructureModule,
  (const NW_Markup_Module_t*) &NW_XHTML_TextModule1,
  (const NW_Markup_Module_t*) &NW_XHTML_TextModule2,
  (const NW_Markup_Module_t*) &NW_XHTML_HypertextModule,
  (const NW_Markup_Module_t*) &NW_XHTML_BasicFormsModule,
  (const NW_Markup_Module_t*) &NW_XHTML_BasicTablesModule,
  (const NW_Markup_Module_t*) &NW_XHTML_ImageModule,
  (const NW_Markup_Module_t*) &NW_XHTML_MetaInformationModule,
  (const NW_Markup_Module_t*) &NW_XHTML_LinkModule,
  (const NW_Markup_Module_t*) &NW_XHTML_BaseModule,
  (const NW_Markup_Module_t*) &NW_XHTML_StyleModule,
  (const NW_Markup_Module_t*) &NW_XHTML_FrameModule,
};

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_Epoc32ContentHandler_Class_t NW_XHTML_Epoc32ContentHandler_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_XHTML_ContentHandler_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                 */
    /* interfaceList                  */ NW_XHTML_Epoc32_InterfaceList
  },
  { /* NW_Object_Dynamic              */
    /* instanceSize                   */ sizeof (NW_XHTML_Epoc32ContentHandler_t),
    /* construct                      */ _NW_XHTML_ContentHandler_Construct,
    /* destruct                       */ NULL
  },
  { /* NW_HED_DocumentNode            */
    /* cancel                         */ _NW_XHTML_ContentHandler_Cancel,
    /* partialLoadCallback	          */ _NW_XHTML_Epoc32ContentHandler_PartialLoadCallback,
    /* initialize                     */ _NW_XHTML_ContentHandler_Initialize,
    /* nodeChanged                    */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                     */ _NW_XHTML_Epoc32ContentHandler_GetBoxTree,
    /* processEvent                   */ _NW_XHTML_ContentHandler_ProcessEvent,
    /* handleError                    */ _NW_HED_DocumentNode_HandleError,
    /* suspend                        */ _NW_XHTML_ContentHandler_Suspend,
    /* resume                         */ _NW_XHTML_CompositeContentHandler_Resume,
    /* allLoadsCompleted              */ _NW_XHTML_ContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted   */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus                      */ _NW_XHTML_ContentHandler_LoseFocus,
    /* gainFocus                      */ _NW_XHTML_ContentHandler_GainFocus,
    /* handleLoadComplete             */ _NW_XHTML_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler          */
    /* partialNextChunk               */ _NW_XHTML_ContentHandler_PartialNextChunk,
    /* getTitle                       */ _NW_XHTML_ContentHandler_GetTitle,
    /* getUrl                         */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl                     */ _NW_XHTML_ContentHandler_ResolveURL,
    /* createHistoryEntry             */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry        */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse                 */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree                  */ _NW_XHTML_ContentHandler_CreateBoxTree,
    /* handleRequest                  */ _NW_XHTML_ContentHandler_HandleRequest,
    /* featureQuery                   */ _NW_HED_ContentHandler_FeatureQuery,
    /* responseComplete               */ _NW_HED_ContentHandler_ResponseComplete
  },
  { /* NW_HED_CompositeContentHandler */
    /* documentDisplayed              */ _NW_XHTML_ContentHandler_DocumentDisplayed,
  },
  { /* NW_XHTML_ContentHandler        */
    /* numModules                     */ sizeof (_NW_XHTML_ContentHandler_Modules) / sizeof (NW_Markup_Module_t*),
    /* modules                        */ _NW_XHTML_ContentHandler_Modules
  },
  { /* NW_XHTML_Epoc32ContentHandler */
    /* unused                         */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const NW_XHTML_Epoc32_InterfaceList[] = {
  &NW_XHTML_Epoc32ContentHandler_ImageViewer_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_Markup_ImageViewer_Class_t NW_XHTML_Epoc32ContentHandler_ImageViewer_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Markup_ImageViewer_Class,
    /* querySecondary            */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary       */
    /* offset                    */ offsetof (NW_XHTML_Epoc32ContentHandler_t,
                                              NW_Markup_ImageViewer)
  },
  { /* NW_Object_Aggregate       */
    /* secondaryList             */ _NW_Markup_ImageViewer_SecondaryList,
    /* construct                 */ NULL,
    /* destruct                  */ NULL
  },
  { /* NW_Markup_ImageViewer     */
    /* load                      */ _NW_XHTML_Epoc32ContentHandler_ImageViewer_Load,
    /* isNoSave                  */ _NW_XHTML_Epoc32ContentHandler_ImageViewer_IsNoSave,
    /* getAltText                */ _NW_XHTML_Epoc32ContentHandler_ImageViewer_GetAltText
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_Epoc32ContentHandler_GetBoxTree( NW_HED_DocumentNode_t* documentNode,
                                            NW_LMgr_Box_t** boxTree )
{
  NW_XHTML_ContentHandler_t*  thisObj;
  NW_HED_ContentHandler_t*    contentHandler;
  NW_HED_DocumentRoot_t*      docRoot;
  TBrowserStatusCode          status = KBrsrSuccess;

  // parameter assertion block
  NW_ASSERT( NW_Object_IsInstanceOf( documentNode,
                                     &NW_XHTML_ContentHandler_Class ) );
  // for convenience
  thisObj = NW_XHTML_ContentHandlerOf( documentNode );
  contentHandler = NW_HED_ContentHandlerOf( documentNode );
  docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj );
  NW_ASSERT( docRoot != NULL );

  NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
  NW_ASSERT( rootBox != NULL );

  *boxTree = NULL;
 
  // call createbox tree if it either has not been created or
  // rebuild is needed
  if( !contentHandler->boxTree )
    {
    // invoke our superclass method
    status = NW_HED_ContentHandler_CreateBoxTree( thisObj, &contentHandler->boxTree );
    }
  // here comes the partial box tree creation, if there are more nodes to create boxes
  else if( thisObj->prevInitializedDomNode != thisObj->lastInitializedDomNode)
    {
    status = NW_HED_ContentHandler_CreateBoxTree( thisObj, &contentHandler->boxTree );
    }
  *boxTree = contentHandler->boxTree;
  // return the box tree
  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_Epoc32ContentHandler_PartialLoadCallback( NW_HED_DocumentNode_t* documentNode,
                                                    TBrowserStatusCode loadStatus,
                                                    NW_Int32 chunkIndex,
                                                    NW_HED_UrlRequest_t* urlRequest,
                                                    NW_Url_Resp_t* response,
                                                    NW_Uint16 transactionId,
                                                    void* context )
  {
  
  NW_TRY( status )
    {
    NW_HED_DocumentNode_t* childNode;
    NW_ASSERT( documentNode != NULL );
    NW_ASSERT( response != NULL );

    // Ignore HEAD responses.
    if (response && (response->method == NW_URL_METHOD_HEAD))
      {
      // Skip the rest of this method
      NW_THROW_SUCCESS(status);
      }

    if( chunkIndex == 0 )
      {
      NW_HED_ContentHandler_t* contentHandler;

      // create content handler
      contentHandler = NW_HED_CompositeContentHandler_CreateContentHandler(
        NW_HED_ContentHandlerOf( documentNode ),
		response->contentTypeString, urlRequest, response, context );
      NW_THROW_ON_NULL( contentHandler, status, KBrsrFailure );

      // forward first chunk to the content handler
      status = NW_HED_ContentHandler_PartialNextChunk( contentHandler, chunkIndex, 
          response, context );
      response = NULL;
      _NW_THROW_ON_ERROR( status );

      // special handling for images
      if( NW_Object_IsInstanceOf( contentHandler, &NW_ImageCH_Epoc32ContentHandler_Class ) )
        {
        // set context on image content handler
        // context is a pointer to the elementNode which is used to look up the associated box
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
        NW_ImageCH_Epoc32ContentHandler_ForceImageOpen(
          NW_ImageCH_Epoc32ContentHandlerOf( contentHandler ) );
#endif // INCREMENTAL_IMAGE_ON
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

      // further chunks
      if( chunkIndex != -1 )
        {
        // pass chunks to the content handler
        status = NW_HED_ContentHandler_PartialNextChunk(
          NW_HED_ContentHandlerOf( childNode ), chunkIndex, response, context );
        // response has been taken by the child content handler
        response = NULL;

        _NW_THROW_ON_ERROR( status );
        }
      // last chunk
      else
        {
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
        /*//R->ecma
        else if(NW_Object_IsInstanceOf (childNode, &NW_Ecma_ContentHandler_Class))
          {
          // set the element node on the content handler
          NW_EcmaContentHandlerSetElementNode((NW_Ecma_ContentHandler_t*)childNode, 
                                         (NW_DOM_ElementNode_t*)context);
          }
		*/
        // Inform the content handler that the response is complete.
        status = NW_HED_ContentHandler_ResponseComplete(
          NW_HED_ContentHandlerOf( childNode ), response, context);
        // last chunk. content handler is all set.
        // intialize it
        status = NW_HED_DocumentNode_Initialize( childNode, KBrsrSuccess );
          // notify the owner of the embedded load complete
          NW_HED_DocumentNode_HandleLoadComplete( documentNode, urlRequest, BRSR_STAT_CLASS_NONE, loadStatus,
            transactionId, context );
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

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/************************************************************************

  Function: NW_XHTML_Epoc32ContentHandler_SaveToFile

  Description:  Write multipart document to file containing xhtml, images,
                and CSS components which were externally loaded. Multipart
                document consists of:
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
NW_XHTML_Epoc32ContentHandler_SaveToFile(NW_XHTML_Epoc32ContentHandler_t *thisObj,
                                         NW_Ucs2* file)
{
  NW_HED_DomTree_t* domTree = NULL;
  NW_XHTML_ContentHandler_t *xhtmlCH;
  NW_Osu_File_t fh = NULL;
  NW_Buffer_t* buffer;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Uint32 numImages = 0;
  NW_Uint32 numScripts = 0;
  NW_Uint32 numCSSImages = 0;
  NW_Uint32 numStyleSheets = 0;
  NW_Uint32 numSegments;
  NW_Byte numSegmentsBuf[5];
  NW_Uint32 numSegmentsBufLen;
  NW_Byte* p;
  NW_Byte* segment;
  NW_Uint32 segmentLen;
  NW_Ucs2* url;
  char* urlAscii = NULL;

  TUint originalEncoding = NW_Settings_GetOriginalEncoding();

  xhtmlCH = NW_XHTML_ContentHandlerOf(thisObj);
  domTree = xhtmlCH->domTree;
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

  /* Number of external style sheets. */
  if (xhtmlCH->processor != NULL)
    numStyleSheets = NW_CSS_Processor_GetNumStyleSheets(xhtmlCH->processor);

  /* Number of images; these are in a list of image content handlers. */
  if (xhtmlCH->super.NW_HED_CompositeNode.children != NULL)
    {
    numImages = NW_HED_CompositeContentHandler_GetImageCount(NW_HED_CompositeContentHandlerOf(xhtmlCH));
    //numScripts= NW_HED_CompositeContentHandler_GetScriptCount(NW_HED_CompositeContentHandlerOf(xhtmlCH));
    }

  /* Other images are in the processor's CSS image list */
  if (xhtmlCH->processor != NULL)
    numCSSImages = NW_CSS_Processor_GetNumCSSImages(xhtmlCH->processor);

  /* Get count of multipart segments (one for xhtml page, plus number of
   * external style sheets, plus number of images). Write variable length
   * integer to a buffer, then write buffer to file
   */
  numSegments = 1 + numStyleSheets + numImages + numCSSImages + numScripts;
  p = NW_StoreUint(numSegmentsBuf, numSegments);
  numSegmentsBufLen = p - numSegmentsBuf;
  status = NW_Osu_WriteFile(fh, numSegmentsBuf, numSegmentsBufLen);
  if (status != KBrsrSuccess)
    goto cleanup;

  /* Get response URL. If it is NULL, use it anyway. */
  url = (NW_Ucs2*) NW_HED_UrlResponse_GetRawUrl (NW_HED_ContentHandler_GetUrlResponse (thisObj));
  if (url != NULL)
    urlAscii = NW_Str_CvtToAscii(url);

  /* Create multipart segment from xhtml document & write to file */
  buffer = NW_HED_DomTree_GetDomBuffer(domTree);

  status = NW_CreateMultipartSegment
            (
              NW_HED_DomTree_GetCharEncoding(domTree),
			  originalEncoding,
              (NW_Http_ContentTypeString_t)HTTP_application_xhtml_xml_saved_string,
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

  /* Write style sheets and CSS Images to file as multipart segments */
  if (xhtmlCH->processor != NULL)
  {
    status = NW_CSS_Processor_WriteMultipartSegment(xhtmlCH->processor, fh, numCSSImages, numStyleSheets);
      if (status != KBrsrSuccess)
        goto cleanup;
  }

  /* Write images to file as multipart segments */
  status = NW_HED_CompositeContentHandler_SaveImagesToFile(xhtmlCH, fh);
  if (status != KBrsrSuccess)
    goto cleanup;

  /* Write scripts to file as multipart segments */
  //status = NW_HED_CompositeContentHandler_SaveScriptsToFile(xhtmlCH, fh);

cleanup:
  NW_LOG1(NW_LOG_LEVEL3, "NW_XHTML_Epoc32ContentHandler_SaveToFile: Result %d", status);
  NW_Osu_CloseFile(fh);

  if (status != KBrsrSuccess)
    (void)NW_Osu_Delete(file);

  return status;
}


/* ------------------------------------------------------------------------- *
   NW_Markup_ImageViewer methods
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
NW_Bool
_NW_XHTML_Epoc32ContentHandler_ImageViewer_IsNoSave (NW_Markup_ImageViewer_t* imageViewer,
                                                     NW_LMgr_Box_t* imageBox)
{
  NW_Text_t* alt = NULL;
  NW_DOM_ElementNode_t* elementNode = NULL;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_XHTML_ContentHandler_t* contentHandler;
  NW_Ucs2* storage = NULL;
  NW_LMgr_PropertyValue_t value;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded;
  NW_Bool ret = NW_FALSE;


  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_XHTML_Epoc32ContentHandler_ImageViewer_Class));
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageBox,
                                     &NW_FBox_ImageButtonBox_Class) ||
             NW_Object_IsDerivedFrom(imageBox,
                                     &NW_LMgr_ImgContainerBox_Class));

  /* for convenience */
  contentHandler = (NW_XHTML_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (imageViewer);
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_XHTML_ContentHandler_Class));

  value.object = NULL;
  status = NW_LMgr_Box_GetPropertyValue(imageBox, NW_CSS_Prop_elementId, NW_CSS_ValueType_Object, &value);
  if (status != KBrsrSuccess)
  {
    return NW_FALSE;
  }
  elementNode = (NW_DOM_ElementNode_t*) value.object;

  /* find the 'alt' attribute and get its string value */
  status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                  NW_XHTML_AttributeToken_alt, &alt);
  if (status != KBrsrSuccess) {
    return NW_FALSE;
  }
  storage = NW_Text_GetUCS2Buffer (alt, 0, &length, &freeNeeded);
  if(storage == NULL)
  {
    NW_Object_Delete(alt);
    return NW_FALSE;
  }

  if (!NW_Str_StricmpConst (storage, WAE_ASC_NOSAVE_STR) || !NW_Str_StricmpConst (storage, WAE_ASC_NOKIA_STR))
  {
    ret = NW_TRUE;
  }

  NW_Object_Delete(alt);
  if (freeNeeded) {
    NW_Mem_Free ((NW_Ucs2*) storage) ;
  }

  return ret;

}

/* ------------------------------------------------------------------------- */
NW_Ucs2*
_NW_XHTML_Epoc32ContentHandler_ImageViewer_GetAltText (NW_Markup_ImageViewer_t* imageViewer,
                                                       NW_LMgr_Box_t* imageBox)
{
  NW_Text_t* alt = NULL;
  NW_DOM_ElementNode_t* elementNode = NULL;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_XHTML_ContentHandler_t* contentHandler;
  NW_Ucs2* storage = NULL;
  NW_LMgr_PropertyValue_t value;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded;


  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_XHTML_Epoc32ContentHandler_ImageViewer_Class));
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageBox,
                                     &NW_FBox_ImageButtonBox_Class) ||
             NW_Object_IsDerivedFrom(imageBox,
                                     &NW_LMgr_ImgContainerBox_Class));

  /* for convenience */
  contentHandler = (NW_XHTML_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (imageViewer);
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_XHTML_ContentHandler_Class));

  value.object = NULL;
  status = NW_LMgr_Box_GetPropertyValue(imageBox, NW_CSS_Prop_elementId, NW_CSS_ValueType_Object, &value);
  if (status != KBrsrSuccess)
  {
    return NULL;
  }
  elementNode = (NW_DOM_ElementNode_t*) value.object;

  /* find the 'alt' attribute and get its string value */
  status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                  NW_XHTML_AttributeToken_alt, &alt);
  if (status != KBrsrSuccess) {
    return NULL;
  }
  storage = NW_Text_GetUCS2Buffer (alt, NW_Text_Flags_NullTerminated |
                                   NW_Text_Flags_CompressWhitespace, &length, &freeNeeded);

  NW_Object_Delete(alt);
  if(storage == NULL)
  {
    return NULL;
  }

  if (!freeNeeded) {
    return NW_Str_Newcpy(storage);
  }
  return storage;
}

// ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_Epoc32ContentHandler_ImageViewer_Load (NW_Markup_ImageViewer_t* imageViewer)
    {
    NW_XHTML_ContentHandler_t* contentHandler;
    NW_Text_t* url = NULL;
    NW_DOM_ElementNode_t* elementNode = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    NW_LMgr_BoxVisitor_t boxVisitor;
    NW_LMgr_Box_t* box = NULL;
    
    // parameter assertion block 
    NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
        &NW_XHTML_Epoc32ContentHandler_ImageViewer_Class));
    
    // for convenience 
    contentHandler = (NW_XHTML_ContentHandler_t*)NW_Object_Aggregate_GetAggregator (imageViewer);
    NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_XHTML_ContentHandler_Class));
    box = NW_HED_ContentHandlerOf(contentHandler)->boxTree;
    if (box == NULL)
        {
        return KBrsrSuccess;
        }
    
    // create the Box Visitor object and use it to traverse the box tree 
    if ((status = NW_LMgr_BoxVisitor_Initialize (&boxVisitor, box)) != KBrsrSuccess)
        {
        return status;
        }
    
    // Iterate through all children 
    while ((box = NW_LMgr_BoxVisitor_NextBox(&boxVisitor,0)) != NULL)
        {
        if ((NW_Object_IsInstanceOf(box, &NW_LMgr_ContainerBox_Class) &&
            !NW_Object_IsInstanceOf(box->propList, &NW_LMgr_SlavePropertyList_Class))
            || NW_Object_IsClass(box, &NW_FBox_ImageButtonBox_Class)
            || NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class))
            {
            NW_LMgr_PropertyValue_t value;
            
            // checks if image in ImageButtonBox is loaded or not. If it is, we do not want to
            // issue the load      
            if (NW_Object_IsClass(box, &NW_FBox_ImageButtonBox_Class))
                {
                NW_FBox_ImageButtonBox_t* imageButtonBox = NW_FBox_ImageButtonBoxOf(box);
                if ((imageButtonBox->image) &&
                    (!NW_Object_IsInstanceOf(imageButtonBox->image, &NW_Image_Epoc32Canned_Class)))
                    {
                    continue;
                    }
                }
            else if( NW_Object_IsDerivedFrom( box, &NW_LMgr_ImgContainerBox_Class ) == NW_TRUE )
                {
                // checks if image in ImageContainerBox is loaded or not. If it is, we do not want to
                // issue the load      
                // TODO: have a load status flag on the box instead of checking the broken
                // status as a broken status could be a result of 404
                NW_LMgr_ImgContainerBox_t* imageBox = NW_LMgr_ImgContainerBoxOf( box );
                if( !imageBox->isBroken )
                    {
                    continue;
                    }
                }
            value.object = NULL;
            (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Object, &value);
            if (value.object == NULL)
                {
                continue;
                }
            
            elementNode = (NW_DOM_ElementNode_t*)value.object;
            
            // find the 'src' attribute and get its string value 
            status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                NW_XHTML_AttributeToken_src, &url);
            if (status == KBrsrSuccess)
                {
                NW_XHTML_ContentHandler_t* thisObj = NW_XHTML_ContentHandlerOf( contentHandler );
                NW_ASSERT(thisObj != NULL);

                const NW_XHTML_ElementHandler_t* elementHandler = NW_XHTML_ContentHandler_GetElementHandler(
                    thisObj, elementNode );
                NW_ASSERT(elementHandler != NULL);             

                // Before loading the image, verify that the element handler is image element handler
                if (NW_XHTML_ImageModule_IsImageElementHandler(elementHandler))
                    {
                    // load the image 
                    status = NW_XHTML_ContentHandler_StartLoad (contentHandler,
                        url, NW_HED_UrlRequest_Reason_DocLoadChild, elementNode,
                        NW_UrlRequest_Type_Image);
                    }
                // cleanup after ourselves 
                NW_Object_Delete (url);
                    
                }
            if (status == KBrsrOutOfMemory)
                {
                break;
                }
            }
        }
    
    if( contentHandler->processor )
        {
        NW_CSS_Processor_ShowImages( contentHandler->processor );
        }
    
    // Only care about out-of-memory errors.
    return (status == KBrsrOutOfMemory) ? KBrsrOutOfMemory : KBrsrSuccess;
    }
