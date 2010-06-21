/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include <e32std.h>

#include "nw_lmgr_slavepropertylist.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_wml1x_wml1xcontenthandler.h"
#include "nw_wml1x_wml1xdefaultstylesheet.h"
#include "GDIDeviceContext.h"
#include "nw_imagech_epoc32contenthandleri.h"
#include "nwx_http_defs.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_image_virtualimage.h"
#include "nw_system_optionlist.h"
#include "HEDDocumentListener.h"
#include "nw_lmgr_rootbox.h"
#include "nw_image_epoc32simpleimage.h"
#include "epoc32imagedecoder.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nwx_logger.h"
#include "nwx_settings.h"
#include <MemoryManager.h>

/*required for animations*/

/* ------------------------------------------------------------------------- */

/* find context box to be deleted */
/* ------------------------------------------------------------------------- */
static
NW_LMgr_Box_t*
NW_LMgr_ImageCH_Epoc32ContentHandler_FindContextBox( NW_HED_DocumentNode_t* documentNode,
                                                    void* context )
  {
  NW_LMgr_Box_t* box = NULL;
  NW_LMgr_Box_t* contextBox = NULL;
  NW_LMgr_BoxVisitor_t boxVisitor;

  box = NW_HED_ContentHandlerOf( documentNode )->boxTree;
  if(NW_LMgr_BoxVisitor_Initialize( &boxVisitor, box ) != KBrsrSuccess )
    {
    return NULL;
    }
  while( ( box = NW_LMgr_BoxVisitor_NextBox( &boxVisitor, 0 ) ) != NULL )
    {
    if( ( NW_Object_IsInstanceOf( box, &NW_LMgr_ContainerBox_Class ) &&
         !NW_Object_IsInstanceOf( box->propList, &NW_LMgr_SlavePropertyList_Class ) )
         || NW_Object_IsClass( box, &NW_FBox_ImageButtonBox_Class )
         || NW_Object_IsDerivedFrom( box, &NW_LMgr_ImgContainerBox_Class ) )
      {
      NW_LMgr_PropertyValueType_t type = NW_CSS_ValueType_Object;
      NW_LMgr_PropertyValue_t value;

      // "wml image" (wbmp) element Id is integer type while "html image" Id is object type
      if( NW_Object_IsInstanceOf( documentNode, &NW_Wml1x_ContentHandler_Class) )
        {
        type = NW_CSS_ValueType_Integer;
        }
      if( NW_LMgr_Box_GetPropertyValue( box, NW_CSS_Prop_elementId, type, &value ) == KBrsrSuccess )
        {
        if( type == NW_CSS_ValueType_Object && value.object == context ||
            type == NW_CSS_ValueType_Integer && value.integer == NW_INT32_CAST(context) )
          {
          // We found a box that has element Id same as context
          contextBox = box;
          break;
          }
        }
      }
    }
  return contextBox;
  }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_ImageCH_Epoc32ContentHandler_HandleValidImageBox( NW_ImageCH_Epoc32ContentHandler_t* thisObj )
  {
  NW_ASSERT( thisObj );

  NW_TRY( status )
    {
    NW_HED_DocumentNode_t* documentNode = NW_HED_DocumentNodeOf( thisObj )->parent;
    NW_LMgr_Box_t* cannedImageBox;

    // The box tree hasn't been created yet; there is no need to replace the
    // missing image icon.
    if( NW_HED_ContentHandlerOf( documentNode )->boxTree == NULL )
      {
      NW_THROW_STATUS( status, KBrsrFailure );
      }

    // find the context box (placeholder with the canned image)
    cannedImageBox = NW_LMgr_ImageCH_Epoc32ContentHandler_FindContextBox( documentNode, thisObj->context );

    // The "cannedImageBox" is a placeholder for the real image. If it doesn't
    // exist, then the image was loaded before we had time to create it so 
    // there is nothing to replace
    if( cannedImageBox != NULL )
      {
      // When the cannedImageBox is imagebuttonbox we know we are dealing
      // input type=image tag
      if( NW_Object_IsClass( cannedImageBox, &NW_FBox_ImageButtonBox_Class ) )
        {
        NW_FBox_ImageButtonBox_t* imageButtonBox = NW_FBox_ImageButtonBoxOf( cannedImageBox );
        imageButtonBox->image = thisObj->image;
        // set boxtree
        NW_HED_ContentHandler_t* contentHandler = NW_HED_ContentHandlerOf( thisObj );
        contentHandler->boxTree = NW_LMgr_BoxOf( imageButtonBox );
        }
      else
        {
        // set boxtree
        NW_HED_ContentHandler_t* contentHandler = NW_HED_ContentHandlerOf( thisObj );
        contentHandler->boxTree = NW_LMgr_BoxOf( cannedImageBox );  
        }
      if( NW_Object_IsInstanceOf( documentNode, &NW_Wml1x_ContentHandler_Class) )
        {
        // here comes some WML specific calls
        NW_LMgr_Property_t    prop;
        NW_LMgr_PropertyValue_t id;
        id.integer = NW_INT32_CAST( thisObj->context );

        AddImageProperties( NW_Wml1x_ContentHandlerOf( documentNode ), cannedImageBox, id );
        NW_Wml1x_ImageWithinAnchorDefaultStyle( &prop, cannedImageBox );
        }
      }
    }
  NW_CATCH(status)
    {
    }
  NW_FINALLY
    {
    return status;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ImageCH_Epoc32ContentHandler_Class_t NW_ImageCH_Epoc32ContentHandler_Class = {
  { /* NW_Object_Core                */
    /* super                         */ &NW_HED_ContentHandler_Class,
    /* queryInterface                */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                */
    /* interfaceList                 */ _NW_ImageCH_Epoc32ContentHandler_InterfaceList
  },
  { /* NW_Object_Dynamic             */
    /* instanceSize                  */ sizeof (NW_ImageCH_Epoc32ContentHandler_t),
    /* construct                     */ _NW_ImageCH_Epoc32ContentHandler_Construct,
    /* destruct                      */ _NW_ImageCH_Epoc32ContentHandler_Destruct
  },
  { /* NW_HED_DocumentNode           */
    /* cancel                        */ _NW_HED_DocumentNode_Cancel,
    /* partialLoadCallback	     */ NULL,
    /* initialize                    */ _NW_ImageCH_Epoc32ContentHandler_Initialize,
    /* nodeChanged                   */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                    */ _NW_ImageCH_Epoc32ContentHandler_GetBoxTree,
    /* processEvent                  */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                   */ _NW_HED_DocumentNode_HandleError,
    /* suspend                       */ _NW_HED_ContentHandler_Suspend,
    /* resume                        */ _NW_HED_DocumentNode_Resume,
    /* allLoadsCompleted             */ _NW_HED_ContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted  */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus                     */ _NW_HED_ContentHandler_LoseFocus,
    /* gainFocus                     */ _NW_HED_ContentHandler_GainFocus,
    /* handleLoadComplete           */ _NW_HED_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler         */
    /* partialNextChunk              */ _NW_ImageCH_Epoc32ContentHandler_PartialNextChunk,
    /* getTitle                      */ _NW_ImageCH_Epoc32ContentHandler_GetTitle,
    /* getUrl                        */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl                    */ _NW_HED_ContentHandler_ResolveURL,
    /* createHistoryEntry            */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry       */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse                */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree                 */ _NW_ImageCH_Epoc32ContentHandler_CreateBoxTree,
    /* handleRequest                 */ _NW_HED_ContentHandler_HandleRequest,
    /* featureQuery                  */ _NW_HED_ContentHandler_FeatureQuery,
    /* responseComplete              */ _NW_HED_ContentHandler_ResponseComplete
  },
  { /* NW_ImageCH_Epoc32ContentHandler */
   /* unused                         */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const _NW_ImageCH_Epoc32ContentHandler_InterfaceList[] = {
  &_NW_ImageCH_Epoc32_ImageObserver_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_Image_IImageObserver_Class_t _NW_ImageCH_Epoc32_ImageObserver_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_Image_IImageObserver_Class,
    /* queryInterface       */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface  */
    /* offset               */ offsetof( NW_ImageCH_Epoc32ContentHandler_t, NW_Image_IImageObserver )
  },
  { /* NW_Image_IImageObserver */
    /* imageOpened          */ _NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpened,
    /* sizeChanged          */ _NW_ImageCH_Epoc32ContentHandler_IImageObserver_SizeChanged,
    /* imageOpeningStarted  */ _NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpeningStarted,
    /* imageDecoded         */ _NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDecoded,
    /* imageDestroyed       */ _NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDestroyed,
    /* isVisible            */ _NW_ImageCH_Epoc32ContentHandler_IImageObserver_IsVisible
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_Construct( NW_Object_Dynamic_t* dynamicObject,
                                            va_list* argList )
{
  NW_ImageCH_Epoc32ContentHandler_t* thisObj;
  NW_HED_UrlRequest_t* urlRequest;
  TBrowserStatusCode status = KBrsrSuccess;

  /* for convenience */
  thisObj = NW_ImageCH_Epoc32ContentHandlerOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_HED_ContentHandler_Construct (dynamicObject, argList);
  if (status != KBrsrSuccess) 
    {
    return status;
    }

  NW_HED_ContentHandler_SetIsEmbeddable (thisObj, NW_TRUE);

  /* get the loadMode */
  urlRequest = va_arg (*argList, NW_HED_UrlRequest_t*);
  NW_ASSERT (urlRequest);
  NW_ASSERT (NW_Object_IsInstanceOf (urlRequest, &NW_HED_UrlRequest_Class));

  thisObj->noStore = (NW_Bool)va_arg (*argList, NW_Uint32);

  thisObj->loadMode = urlRequest->loadMode;

  /* copy the request url */
  const NW_Ucs2* rawUrl = NW_HED_UrlRequest_GetRawUrl(urlRequest);
  if (rawUrl != NULL)
    {
    thisObj->requestUrl = NW_Str_Newcpy(rawUrl);
    if (thisObj->requestUrl == NULL)
       {
       status = KBrsrOutOfMemory;
       }
    }

  thisObj->altText = NULL;

  /*
  ** TODO this is the error passed in NW_HED_Image_ContentHandler_NewBrokenImage
  ** in the future this may be useful information.  It isn't right now...

  thisObj->brokenImageError = va_arg (*argList, TBrowserStatusCode);
  */

  return status;
}


/* ------------------------------------------------------------------------- */
void
_NW_ImageCH_Epoc32ContentHandler_Destruct( NW_Object_Dynamic_t* dynamicObject )
  {

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_ImageCH_Epoc32ContentHandler_Class));

  /* for convenience */
  NW_ImageCH_Epoc32ContentHandler_t* thisObj = NW_ImageCH_Epoc32ContentHandlerOf( dynamicObject );

  // removed observer
  if( thisObj->image )
    {
    (void) NW_Image_AbstractImage_RemoveImageObserver( thisObj->image,
      &thisObj->NW_Image_IImageObserver );
    }

   /* deallocate the buffer */
  NW_Object_Delete( thisObj->image );
  thisObj->image = NULL;

  /* free the title */
  NW_Object_Delete( thisObj->title );

   /* deallocate the request url */
  NW_Mem_Free( thisObj->requestUrl );

  /* deallocate the alt text */
  NW_Mem_Free( thisObj->altText );
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_Initialize( NW_HED_DocumentNode_t* documentNode,
  TBrowserStatusCode aInitStatus )
  {
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ImageCH_Epoc32ContentHandler_t* thisObj;

  NW_REQUIRED_PARAM( aInitStatus );

  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_ImageCH_Epoc32ContentHandler_Class));

  // for convenience 
  thisObj = NW_ImageCH_Epoc32ContentHandlerOf (documentNode);

  // let the image decoder know that no more data comes in.
  // no need to inform virtual images as they share the same
  // decoder
  if( thisObj->image && NW_Object_IsClass( thisObj->image, &NW_Image_Epoc32Simple_Class ) == NW_TRUE )
    {
    NW_Image_Epoc32Simple_t* simpleImage = NW_Image_Epoc32SimpleOf( thisObj->image );

    status = NW_Image_Epoc32Simple_Initialize( simpleImage );
    }
  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_GetBoxTree( NW_HED_DocumentNode_t* documentNode,
                                             NW_LMgr_Box_t** boxTree )
  {
  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_ImageCH_Epoc32ContentHandler_Class));

  NW_HED_ContentHandler_t* contentHandler = NW_HED_ContentHandlerOf( documentNode );
  if( contentHandler->boxTree )
    {
	  *boxTree = contentHandler->boxTree;
	  return KBrsrSuccess;
    }
  return _NW_HED_ContentHandler_GetBoxTree( documentNode, boxTree );
  }


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_PartialNextChunk(NW_HED_ContentHandler_t* contentHandler,
                                                  NW_Int32 chunkIndex,
                                                  NW_Url_Resp_t* response,
                                                  void* context)
    {
    NW_TRY(status) 
        {
        NW_ASSERT(contentHandler != NULL);
        NW_ASSERT(response != NULL);
        
        NW_Image_Epoc32Simple_t* simpleImage;
        NW_ImageCH_Epoc32ContentHandler_t* thisObj;
        
        // for convenience 
        thisObj = NW_ImageCH_Epoc32ContentHandlerOf(contentHandler);
        // make sure we have a valid response
        NW_THROW_ON_NULL(response, status, KBrsrUnexpectedError);
        
        // first chunk
        if (chunkIndex == 0)
            {
            // invoke superclass
            status = NW_HED_ContentHandler_Class.NW_HED_ContentHandler.partialNextChunk( 
                                           contentHandler, chunkIndex, response, context);
            // response has been released by the base class call.
            response = NULL;
            _NW_THROW_ON_ERROR(status);
            
            // Do we have an image content-type that Symbian won't recognize,
            // but the browser supports.
            TImageType imageType = GetRecognizedImageType((const char*)
                                        contentHandler->response->contentTypeString);
                
            NW_GDI_Dimension3D_t size = {0, 0, 0};
            // create bitmap
            simpleImage = NW_Image_Epoc32Simple_New(size, NULL, NULL, NW_FALSE, NW_FALSE,
                                                    imageType,
                                                    contentHandler->response->body->data, 
                                                    contentHandler->response->body->length,
                                                    NW_HED_DocumentNodeOf(thisObj));
            NW_THROW_OOM_ON_NULL(simpleImage, status);
            
            // The image now has ownership of the raw data.
            contentHandler->response->body->length = 0;
            contentHandler->response->body->allocatedLength = 0;
            delete contentHandler->response->body;   
            contentHandler->response->body = NULL;
            
            thisObj->image = NW_Image_AbstractImageOf(simpleImage);
            // set observer
            status = NW_Image_AbstractImage_SetImageObserver(thisObj->image,
                                                             &thisObj->NW_Image_IImageObserver);
            NW_THROW_ON(KBrsrOutOfMemory, status);
            // ignore observer errors -except OOM
            status = KBrsrSuccess;
            }
        // do not take last chunk as it has no data
        else if (chunkIndex != -1)
            {
            // make sure that the image has been created when the first 
            // chunk came in
            NW_THROW_ON_NULL(thisObj->image, status, KBrsrUnexpectedError);
            
            NW_ASSERT(NW_Object_IsInstanceOf(thisObj->image,
                                             &NW_Image_Epoc32Simple_Class));    
            simpleImage = NW_Image_Epoc32SimpleOf(thisObj->image);
             //Checking is simpleimage valid for decoding
                
            if( simpleImage->imageType == ERecognizedImage ||
                simpleImage->imageType == EWbmpImage || 
                simpleImage->imageType == EOtaImage )
                {
                // pass chunk to the decoded
                status = NW_Image_Epoc32Simple_PartialNextChunk(NW_Image_AbstractImageOf(simpleImage),
                                                            response->body);
                }
				
            // NW_Image_Epoc32Simple_PartialNextChunk takes body ownership
            response->body = NULL;
            
            _NW_THROW_ON_ERROR(status);      
            }
        else
            {
            // this function musn't be called with the last partial response that has no body.
            // Initialize must be called instead
            NW_ASSERT(NW_TRUE);
            }
        }
    NW_CATCH(status) 
        {
        }
    NW_FINALLY 
        {
        // free response
        UrlLoader_UrlResponseDelete(response);
        return status;
        } 
    NW_END_TRY
    }


/* ------------------------------------------------------------------------- */
const NW_Text_t*
_NW_ImageCH_Epoc32ContentHandler_GetTitle( NW_HED_ContentHandler_t* contentHandler )
{
  static const NW_Ucs2 storage[] = { 'I', 'm', 'a', 'g', 'e', '\0' };

  NW_ImageCH_Epoc32ContentHandler_t* thisObj;

  /* for convenience */
  thisObj = NW_ImageCH_Epoc32ContentHandlerOf( contentHandler );

  /* we create the title just-in-time */
  if( thisObj->title == NULL )
    {
    thisObj->title = (NW_Text_t*) NW_Text_UCS2_New ((NW_Ucs2*) storage, 0, 0);
    }
  return thisObj->title;
  }


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_CreateBoxTree( NW_HED_ContentHandler_t* contentHandler,
                                                NW_LMgr_Box_t** boxTree )
{
  NW_ImageCH_Epoc32ContentHandler_t* thisObj;
  NW_Image_AbstractImage_t* image = NULL;
  NW_LMgr_Box_t* box = NULL;
  NW_Bool brokenImage = NW_FALSE;

  /* parameter assertion block */
  NW_ASSERT( NW_Object_IsInstanceOf (contentHandler,
                                     &NW_HED_ContentHandler_Class ) );

  /* for convenience */
  thisObj = NW_ImageCH_Epoc32ContentHandlerOf( contentHandler );

  /* create the image alias from our image data*/
  if( thisObj->image != NULL )
  {
  /* This case arises when this Image Content Handler contains the Virtual
     Image */
    if( NW_Object_IsInstanceOf( thisObj->image, &NW_Image_Virtual_Class ) )
      {
      NW_Image_Virtual_t* virtualImage = NW_Image_VirtualOf( thisObj->image );
      image = (NW_Image_AbstractImage_t*) NW_Image_Virtual_New( virtualImage->image );
      }
    else
      {
      image = (NW_Image_AbstractImage_t*)NW_Image_Virtual_New( thisObj->image );
      }
    }
  /* for some reason the above failed so create a broken image instead */
  if( image == NULL )
    {
    NW_HED_DocumentRoot_t         *documentRoot;
    NW_LMgr_RootBox_t             *rootBox;
    NW_Image_CannedImages_t* cannedImages;

    /* get the RootBox from through IDocumentListener interface */
    documentRoot =
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (contentHandler);
    NW_ASSERT(documentRoot != NULL);

    /* extract the image from our canned images */
    rootBox = NW_HED_DocumentRoot_GetRootBox(documentRoot);
    NW_ASSERT(rootBox);

    cannedImages = rootBox->cannedImages;
    NW_ASSERT(cannedImages!=NULL);

    image = (NW_Image_AbstractImage_t*)
                       NW_Image_CannedImages_GetImage( cannedImages,
                                                      NW_Image_Broken );
    brokenImage = NW_TRUE;
    }
  /* create the image box */
  if( image != NULL )
    {
    box = (NW_LMgr_Box_t*) NW_LMgr_AnimatedImageBox_New(0, image, NULL, brokenImage );
    }

  if( box == NULL )
    {
    NW_Object_Delete( image );
    }

  /* return the box (which may be NULL )*/
  *boxTree = box;
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
NW_HED_UrlRequest_LoadMode_t
NW_ImageCH_Epoc32ContentHandler_GetLoadMode( NW_ImageCH_Epoc32ContentHandler_t* thisObj )
  {
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );

  return thisObj->loadMode;
  }


/* ------------------------------------------------------------------------- *
   NW_ImageCH_Epoc32_ImageObserver implementation
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpened( NW_Image_IImageObserver_t* imageObserver,
                                                             NW_Image_AbstractImage_t* abstractImage,
                                                             NW_Bool failed,
                                                             NW_Int16 suppressDupReformats )
  {
  NW_ImageCH_Epoc32ContentHandler_t* thisObj = NULL;
  NW_HED_DocumentRoot_t* docRoot = NULL;
  NW_LMgr_RootBox_t* rootBox = NULL;
  NW_Bool forceReformat = NW_FALSE;
  NW_Bool relayoutNeeded = NW_FALSE;
  NW_Bool smallScreenOn;

  //lint -e{550} Symbol not accessed
  NW_TRY( status )
    {
    // parameter assertion block
    NW_ASSERT( imageObserver != NULL );
    NW_ASSERT( abstractImage != NULL );
    NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                      &_NW_ImageCH_Epoc32_ImageObserver_Class ));

    NW_LOG0(NW_LOG_LEVEL5, "_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpened START");
    // obtain the implementer
    thisObj = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );
    // get rootbox 
    docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    smallScreenOn = NW_LMgr_RootBox_GetSmallScreenOn( rootBox );

    if( failed == NW_FALSE )
      {
      relayoutNeeded = NW_TRUE;

      if( thisObj->context )
        {
        status =  NW_LMgr_ImageCH_Epoc32ContentHandler_HandleValidImageBox( thisObj );
        // When status is KBrsrFailure, box tree has not been completely created;
        // calling MVCView::ReformatBox in the later code will have problems
        // and program will revisit here because decoder is SetActive again;
        // so no need to execute code following.
        // Only set context to NULL when the status is KBrsrSuccess
        if (status == KBrsrSuccess)
          {
          thisObj->context = NULL;
          }
        else
          {
          NW_THROW( KBrsrSuccess );
          }
        }

      /*
      ** See the comment about "iSuppressDupReformats" in NW_MVC_View_ReformatBox.
      **
      ** This helps speed up the special case when an image is duplicated many times
      ** within the same document.
      */
      if( smallScreenOn )
        {
        // Start suppressing reformats.
        if (suppressDupReformats != NW_Image_AbstractImage_SuppressDupReformats_Stop)
          {
          rootBox->boxTreeListener->EnableSuppressDupReformats (suppressDupReformats);
          }

        // Stop suppressing reformats.
        else 
          {
          // This is the last observer.  If any of the image observers needed a 
          // reformat then force the reformat (below) even if this box doesn't require one
          forceReformat = rootBox->boxTreeListener->IsForcedReformatNeeded ();

          rootBox->boxTreeListener->EnableSuppressDupReformats (suppressDupReformats);
          }
        }

      // get the image box
      NW_LMgr_Box_t* imageBox = NW_HED_ContentHandlerOf( thisObj )->boxTree;

      if( imageBox )
        {
        // first get the visible part of the imagebox and then we can check if the visible 
        // part is in the view
        NW_GDI_Rectangle_t imageRect;

        if( NW_LMgr_Box_GetVisibleBounds( imageBox, &imageRect ) == NW_TRUE )
          {
          // we need the rootbox to get the device context which then gives us the
          // current view coordinates
          CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext( rootBox );
          NW_THROW_ON_NULL( deviceContext, status, KBrsrFailure );

          NW_GDI_Rectangle_t viewRect = deviceContext->ClipRect();

          // check if the image box is in the view
          if( NW_GDI_Rectangle_Cross( &imageRect, &viewRect, NULL ) )
            {
            // force decode
            status = NW_Image_AbstractImage_ForceImageDecode( abstractImage );
            NW_THROW_ON_ERROR( status );
            }
          }

        NW_LMgr_Property_t hProp;
        NW_LMgr_Property_t wProp;
        // check if the replaced image needs relayout.
        // call relayout, unless both height and width attributes are set.
        if( ( NW_LMgr_Box_GetProperty( imageBox, NW_CSS_Prop_height, &hProp ) == KBrsrSuccess ) &&
            ( NW_LMgr_Box_GetProperty( imageBox, NW_CSS_Prop_width, &wProp ) == KBrsrSuccess ) )
          {
                    NW_GDI_Rectangle_t bounds;
                    // inner rectangle's width, height corresponds to css properties
                    (void) NW_LMgr_Box_GetInnerRectangle(imageBox, &bounds);

          if( (hProp.value.integer != bounds.dimension.height) || 
              (wProp.value.integer != bounds.dimension.width) )
            {
            relayoutNeeded = NW_TRUE;
            }
          else
            {
            relayoutNeeded = NW_FALSE;
            }
          }
          // when image is ready to be displayed we need to
          // 1. create a virtual image in order that we could replace the canned -broken- image 
          // in the image container box
          // 2. replace the canned image
          NW_Image_AbstractImage_t* image = NULL;

          if( NW_Object_IsInstanceOf( abstractImage, &NW_Image_Virtual_Class ) )
            {
            NW_Image_Virtual_t* virtualImage = NW_Image_VirtualOf( abstractImage );
            image = (NW_Image_AbstractImage_t*)NW_Image_Virtual_New( virtualImage->image );
            }
          else
            {
            image = (NW_Image_AbstractImage_t*)NW_Image_Virtual_New( abstractImage );
            }
          NW_THROW_OOM_ON_NULL( image, status );

          // replace image
          if( NW_Object_IsInstanceOf( imageBox, &NW_LMgr_ImgContainerBox_Class ) == NW_TRUE )
            {
            NW_LMgr_ImgContainerBox_ReplaceBrokenImage( imageBox, image );
            }
          else if( NW_Object_IsInstanceOf( imageBox, &NW_FBox_ImageButtonBox_Class ) == NW_TRUE )
            {
            NW_LMgr_ImageButtonBox_ReplaceBrokenImage( imageBox, image );
            }
          else
            {
            NW_THROW_UNEXPECTED( status );
            }

        // handle relayout needs
        if( ( relayoutNeeded == NW_TRUE ) || ( forceReformat == NW_TRUE ) )
          {
          if( smallScreenOn )
            {
            // in vertical layout mode we just want to format the flow 
            // containing the image
            (void) rootBox->boxTreeListener->ReformatBox(imageBox);

            if ((suppressDupReformats == NW_Image_AbstractImage_SuppressDupReformats_Start) ||
                (suppressDupReformats == NW_Image_AbstractImage_SuppressDupReformats_Continue))
              {
              rootBox->boxTreeListener->ForcedReformatNeeded ();
              }
            }
          else
            {
            NW_Image_IImageObserver_SizeChanged( imageObserver, abstractImage );
            }
          }
        }
      }
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    // open ends. remove it from the queue
    NW_LMgr_RootBox_ImageOpened( rootBox );

    // Now that the image is opened and its box has the updated size info
    // recalculate the RootBox's extents, this also causes the scrollbar values
    // to be updated.
    if( ( relayoutNeeded == NW_TRUE ) || ( forceReformat == NW_TRUE ) )
      {
      NW_LMgr_RootBox_ExtendsCalculationNeeded ( rootBox );
      }

    NW_LOG0(NW_LOG_LEVEL5, "_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpened ENDS");
    return;
    }
  NW_END_TRY
  }


/* ------------------------------------------------------------------------- */
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_SizeChanged( NW_Image_IImageObserver_t* imageObserver,
                                                             NW_Image_AbstractImage_t* abstractImage )
  {
  NW_REQUIRED_PARAM( abstractImage );

  NW_TRY( status )
    {
    NW_ImageCH_Epoc32ContentHandler_t* thisObj;

    // parameter assertion block
    NW_ASSERT( imageObserver != NULL );
    NW_ASSERT( abstractImage != NULL );
    NW_ASSERT(NW_Object_IsInstanceOf( imageObserver,
                                      &_NW_ImageCH_Epoc32_ImageObserver_Class ));

    // obtain the implementer
    thisObj = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );

    // relayout on the entire tree
    // first get rootBox and then we can initialize the
    // relayout event on that.
    NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    // update image container size
    NW_LMgr_Box_t* box;

    status = NW_HED_DocumentNode_GetBoxTree( NW_HED_DocumentNodeOf( thisObj ), &box );
    NW_THROW_ON_ERROR( status );
  
    // table performance: to prevent additional relayouts on the entire page (max pass of the automatic 
    // table algorithm), images update parent table size by setting a flag on the table. the actual resize 
    // happens when a relayout event is generated.
    while( box->parent != NULL )
      {
      // find the parent table.
      if( NW_Object_IsInstanceOf( box->parent, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
        {
        NW_LMgr_StaticTableBox_TableSizeChanged( NW_LMgr_StaticTableBoxOf( box->parent ) );
        break;
        }
      box = NW_LMgr_BoxOf( box->parent );
      }
    // and send layout event
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
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpeningStarted( NW_Image_IImageObserver_t* imageObserver,
                                                                     NW_Image_AbstractImage_t* abstractImage )
  {
  NW_ImageCH_Epoc32ContentHandler_t* thisObj;
  NW_HED_DocumentRoot_t*             documentRoot;
  NW_LMgr_RootBox_t*                 rootBox;

  NW_REQUIRED_PARAM( abstractImage );

  /* parameter assertion block */
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT(NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_ImageCH_Epoc32_ImageObserver_Class ));

    {
    /* obtain the implementer */
    thisObj = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );

    // get the RootBox through IDocumentListener interface 
    documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( thisObj );
    NW_ASSERT( documentRoot != NULL );

    rootBox = NW_HED_DocumentRoot_GetRootBox( documentRoot );
    NW_ASSERT( rootBox != NULL );

    NW_LMgr_RootBox_ImageOpeningStarted( rootBox );
    }
  }


/* ------------------------------------------------------------------------- */
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDecoded( NW_Image_IImageObserver_t* imageObserver,
                                                              NW_Image_AbstractImage_t* abstractImage,
                                                              NW_Bool failed)
  {
  NW_REQUIRED_PARAM( abstractImage );
  
  //lint -e{550} Symbol not accessed
  NW_TRY( status )
    {
    NW_ImageCH_Epoc32ContentHandler_t* thisObj;

    // parameter assertion block 
    NW_ASSERT( imageObserver != NULL );
    NW_ASSERT( abstractImage != NULL );
    NW_ASSERT(NW_Object_IsInstanceOf( imageObserver,
                                      &_NW_ImageCH_Epoc32_ImageObserver_Class ));

    NW_LOG0(NW_LOG_LEVEL5, "_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDecoded START");
    // obtain the implementer 
    thisObj = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( imageObserver );
    NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );
   
    NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode( thisObj );
    NW_THROW_ON_NULL( docRoot, status, KBrsrFailure );

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox( docRoot );
    NW_THROW_ON_NULL( rootBox, status, KBrsrFailure );

    NW_LMgr_Box_t* box = NW_HED_ContentHandlerOf( thisObj )->boxTree;
    NW_THROW_ON_NULL( box, status, KBrsrFailure );
   
    // if the decode failed, we want to replace the unencoded image with the broken image
    if (failed)
      {    
      NW_Image_AbstractImage_t* cannedImage = NULL;
      NW_Image_AbstractImage_t* image = NULL;

      NW_ASSERT( rootBox->cannedImages != NULL );

      cannedImage = (NW_Image_AbstractImage_t*)
          NW_Image_CannedImages_GetImage( rootBox->cannedImages, NW_Image_Missing );
      NW_THROW_OOM_ON_NULL( cannedImage, status );
     
      image = (NW_Image_AbstractImage_t*)NW_Image_Virtual_New( cannedImage );
      NW_THROW_OOM_ON_NULL( image, status );

      // replace image
      if( NW_Object_IsInstanceOf( box, &NW_LMgr_ImgContainerBox_Class ) == NW_TRUE )
        {
        NW_LMgr_ImgContainerBox_ReplaceBrokenImage( box, image );
        NW_LMgr_ImgContainerBox_t* imageBox = NW_LMgr_ImgContainerBoxOf(box);
        imageBox->isBroken = NW_TRUE;
        }
      else if( NW_Object_IsInstanceOf( box, &NW_FBox_ImageButtonBox_Class ) == NW_TRUE )
        {
        NW_LMgr_ImageButtonBox_ReplaceBrokenImage( box, image);
        }
      else
        {
        NW_THROW_UNEXPECTED( status );
        }
      }
      if( NW_Object_IsInstanceOf( box, &NW_LMgr_AnimatedImageBox_Class ) == NW_TRUE ||
        NW_Object_IsInstanceOf( box, &NW_FBox_ImageButtonBox_Class ) == NW_TRUE )
        {
  	    NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t*)
            NW_Object_QuerySecondary (box, &NW_LMgr_AnimatedImageAggregate_Class);
  	    status = NW_LMgr_AnimatedImageAggregate_StartTimer (AnimatedImageAggregate); 
        }
      else
        {
        NW_THROW_UNEXPECTED( status );
        }
    // refresh the image box
    NW_LMgr_Box_Refresh( box );
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    NW_LOG0(NW_LOG_LEVEL5, "_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDecoded END");
    return;
    }
  NW_END_TRY
  }


/* ------------------------------------------------------------------------- */
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDestroyed( NW_Image_IImageObserver_t* imageObserver,
                                                                NW_Image_AbstractImage_t* abstractImage )
  {
  NW_ImageCH_Epoc32ContentHandler_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );

  /* parameter assertion block */
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT(NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_ImageCH_Epoc32_ImageObserver_Class ));

  /* obtain the implementer */
  thisObj = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );

  thisObj->image = NULL;
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_IsVisible( NW_Image_IImageObserver_t* aImageObserver)
  {
  NW_ImageCH_Epoc32ContentHandler_t* thisObj;

  NW_ASSERT( aImageObserver != NULL );
  NW_ASSERT(NW_Object_IsInstanceOf( aImageObserver,
                                    &_NW_ImageCH_Epoc32_ImageObserver_Class ));

  thisObj = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( aImageObserver );
  NW_ASSERT (NW_Object_IsInstanceOf( thisObj, &NW_ImageCH_Epoc32ContentHandler_Class ) );

  NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)
      NW_HED_DocumentNode_GetRootNode(NW_HED_DocumentNodeOf(thisObj));
  NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox(docRoot);

  CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  const NW_GDI_Point2D_t* origin = deviceContext->Origin();
  const NW_GDI_Dimension2D_t* displaySize = &(deviceContext->DisplayBounds()->dimension);

  NW_HED_ContentHandler_t* contentHandler = NW_HED_ContentHandlerOf(thisObj);
  NW_GDI_Rectangle_t bounds = NW_LMgr_Box_GetDisplayBounds( contentHandler->boxTree );
  
  NW_Bool isVisible = NW_TRUE;

  // checking out of view only along the y direction
  if ( ((origin->y + displaySize->height) < bounds.point.y) ||
     (origin->y > (bounds.point.y + bounds.dimension.height)) )
      {
      isVisible = NW_FALSE;
      }
  return isVisible;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/************************************************************************

  Function: NW_ImageCH_Epoc32ContentHandler_GetVirtualHandler

  Description:  This method creates a new ImageContentHandler which is basically
               a clone of the origins (imageCH) ImageContentHandler. So it makes
               a virtual copy of the actual Image and sets it on the new ImageCH

  Parameters: parent - The parent ContentHandler (XHTML/WML) of the new ImageCH,
              urlRequest
              imageCH - the Image Content Handler to be cloned

  Return Value: The Content Handler containing the virtual image

**************************************************************************/

/* -------------------------------------------------------------------------*/
TBrowserStatusCode
NW_ImageCH_Epoc32ContentHandler_GetVirtualHandler( NW_HED_DocumentNode_t* parent,
                                                   NW_HED_UrlRequest_t* urlRequest,
                                                   NW_ImageCH_Epoc32ContentHandler_t* imageCH,
                                                   NW_ImageCH_Epoc32ContentHandler_t** newVirtualImageCH )
  {
  NW_ImageCH_Epoc32ContentHandler_t* virtualImageCH = NULL;

  NW_TRY( status ) 
    {
    NW_ASSERT( NW_HED_ContentHandlerOf( imageCH )->response != NULL );

    virtualImageCH = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_New( 
      &NW_ImageCH_Epoc32ContentHandler_Class, parent, urlRequest, NW_FALSE, imageCH );
    // OOM check
    NW_THROW_OOM_ON_NULL( virtualImageCH, status );

    // if the bitmap exists then take it as a virtual image
    if( imageCH->image )
      {
      virtualImageCH->image = (NW_Image_AbstractImage_t*)NW_Image_Virtual_New( imageCH->image );
      // OOM check
      NW_THROW_OOM_ON_NULL( virtualImageCH->image, status );
    
      // add it to the top level content handler
      status = NW_Image_Epoc32Simple_AddDocNode(NW_Image_Epoc32SimpleOf(imageCH->image),
                                                NW_HED_DocumentNodeOf(virtualImageCH));
      _NW_THROW_ON_ERROR( status );
      
      // observ the virtual image
      status = NW_Image_AbstractImage_SetImageObserver( NW_Image_AbstractImageOf( virtualImageCH->image ),
        &NW_ImageCH_Epoc32ContentHandlerOf( virtualImageCH )->NW_Image_IImageObserver );
      NW_THROW_ON( KBrsrOutOfMemory, status );
      // ignore observer errors -except OOM
      status = KBrsrSuccess;
      }
    }
  NW_CATCH( status )
    {
    // destroy the newly created virtual image handler
    NW_Object_Delete( virtualImageCH );
    virtualImageCH = NULL;
    }
  NW_FINALLY
    {
    *newVirtualImageCH = virtualImageCH;
    return status;
    }
  NW_END_TRY
  }

/* -------------------------------------------------------------------------*/
void
NW_ImageCH_Epoc32ContentHandler_ForceImageOpen( NW_ImageCH_Epoc32ContentHandler_t* imageCH )
  {
  /* parameter assertion block */
  NW_ASSERT( imageCH != NULL );
  NW_ASSERT( imageCH->image != NULL );
  NW_ASSERT(NW_Object_IsInstanceOf( imageCH->image,
                                    &NW_Image_Epoc32Simple_Class ));

  NW_Image_Epoc32Simple_t* simpleImage = NW_Image_Epoc32SimpleOf( imageCH->image );
  CEpoc32ImageDecoder* decoder = STATIC_CAST( CEpoc32ImageDecoder*, simpleImage->decoder );
  
  TInt error = KErrNone;  
  TRAP(error, decoder->Open())
  }

/* -------------------------------------------------------------------------*/
NW_HED_ContentHandler_t*
NW_ImageCH_Epoc32ContentHandler_NewBrokenImage (NW_HED_DocumentNode_t* parent,
                                        NW_HED_UrlRequest_t* urlRequest,
                                        TBrowserStatusCode error)
{
  return (NW_HED_ContentHandler_t*) NW_Object_New (&NW_ImageCH_Epoc32ContentHandler_Class,
      parent, urlRequest, error);
}
