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


/* CEpoc32ImageDecoder class
 *
 * Every non-canned image has an image decoder associated with it. This is opened
 * when the image is created in order to get the size. This is an asynchronous
 * operation; "OpenComplete" is called when the opening is complete.
 *
 * The first time an image is displayed, "Decode" is called, which starts the
 * asynchrounous decoding process. When this completes, the image object is
 * updated with the decoded image, which then causes the new image to be
 * displayed.
 *
 * A decode is also done every time the frame number of an animated image is
 * changed.
 */
#include <bitdev.h>
#include <e32math.h>
#include "Epoc32ImageDecoder.h"
#include "nw_hed_contenthandler.h"
#include "nwx_logger.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_lmgr_rootbox.h"
#include "GDIDeviceContext.h"
#include "MvcShell.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_statuscodeconvert.h"
#include "nw_dom_attribute.h"
#include "nw_dom_element.h"
#include "Oma2Agent.h"
#include "nwx_ctx.h"



/* The Media server seems to return a value larger than this if the animation
 * should end; it is 0xffff x 10; I'm not sure if this is significant. I didn't
 * find any documentation for this. */
#define MAXIMUM_DELAY 655350

_LIT8(KWbmpMimeType, "image/vnd.wap.wbmp");
_LIT8(KOtaMimeType, "image/vnd.nokia.ota-bitmap");

/* ------------------------------------------------------------------------- */

/* find context box for finding the DOM Tree */
/* ------------------------------------------------------------------------- */
static
NW_LMgr_Box_t*
NW_LMgr_ImageCH_Epoc32ContentHandler_FindContextBox_Decoder( NW_HED_DocumentNode_t* documentNode,
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

  /*
  * Get <img> attribute information from the box tree
  *
*/

TInt CEpoc32ImageDecoder::ImgAttrInfo()
    {
    NW_Image_AbstractImage_t* abstractImage = NULL;
    NW_Image_IImageObserver_t* imageObserver = NULL;
    NW_ImageCH_Epoc32ContentHandler_t* imgaeCHContentHandler = NULL;
    NW_HED_DocumentNode_t* documentNode = NULL;
    NW_LMgr_Box_t* cannedImageBox = NULL;
    NW_ADT_Vector_Metric_t numItems;
    TBrowserStatusCode status;
    TInt error = KErrNone;
    
    abstractImage = NW_Image_AbstractImageOf( iImage );
    
    numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( abstractImage->imageObserverList ) );
    if(numItems > 0)
        {
        imageObserver = *(NW_Image_IImageObserver_t**)
            NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( abstractImage->imageObserverList ), 0 );//Take first one as this is prossed
        
        NW_ASSERT( imageObserver );
        
        imgaeCHContentHandler = (NW_ImageCH_Epoc32ContentHandler_t*)NW_Object_Interface_GetImplementer( imageObserver );
        
        if(NW_Object_IsInstanceOf( imgaeCHContentHandler, &NW_ImageCH_Epoc32ContentHandler_Class ) ) 
            {
            documentNode = NW_HED_DocumentNodeOf( imgaeCHContentHandler )->parent;
            
            cannedImageBox= 
                NW_LMgr_ImageCH_Epoc32ContentHandler_FindContextBox_Decoder( documentNode, imgaeCHContentHandler->context );
            }
        }/*end if(numItems > 0)*/
    
    if(cannedImageBox)
        {
        
        NW_LMgr_PropertyValue_t value;
        value.object = NULL;
        
        (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf( cannedImageBox ), NW_CSS_Prop_elementId, NW_CSS_ValueType_Object, &value);
        
        if (value.object)
            {
            
            NW_Uint16 nodeToken;
            NW_Uint16 attrToken = 0;
            NW_DOM_AttributeListIterator_t listIter;
            NW_DOM_AttributeHandle_t attrHandle;
            NW_String_t attrValueStr;
            NW_Uint32 attrEncoding;
            NW_Ucs2 *ucs2AttrVal = NULL;
            NW_DOM_ElementNode_t* elementNode = (NW_DOM_ElementNode_t*)value.object;
            nodeToken = NW_DOM_Node_getNodeToken(elementNode);
            if(nodeToken == NW_XHTML_ElementToken_img)
                {
                if (NW_DOM_ElementNode_hasAttributes(elementNode) )
                    {
                    /* Go through all the attributes and check for height and width */
                    
                    status = StatusCodeConvert(NW_DOM_ElementNode_getAttributeListIterator(elementNode, &listIter) );
                    
                    
                    if(status == KBrsrSuccess)
                        {
                        while (NW_DOM_AttributeListIterator_getNextAttribute(&listIter, &attrHandle)
                            == NW_STAT_WBXML_ITERATE_MORE) 
                            {
                            attrToken = NW_DOM_AttributeHandle_getToken(&attrHandle);
                            if( (attrToken == NW_XHTML_AttributeToken_width) ||
                                (attrToken == NW_XHTML_AttributeToken_height) )
                                {
                                attrEncoding = NW_DOM_AttributeHandle_getEncoding(&attrHandle);
                                status = StatusCodeConvert(NW_DOM_AttributeHandle_getValue(&attrHandle, &attrValueStr) );
                                
                                if(status != KBrsrSuccess)
                                    {
                                    error = KErrGeneral;
                                    goto imgAttrInfoFinal;
                                    }
                                status = StatusCodeConvert( NW_String_stringToUCS2Char(&attrValueStr, attrEncoding, &ucs2AttrVal) );
                                
                                if(status != KBrsrSuccess)
                                    {
                                    error = KErrGeneral;
                                    goto imgAttrInfoFinal;
                                    }
                                
                                if(attrToken == NW_XHTML_AttributeToken_width)
                                    {
                                    iImage->srcWidth = NW_Str_Atoi(ucs2AttrVal);
                                    }
                                else if(attrToken == NW_XHTML_AttributeToken_height)
                                    {
                                    iImage->srcHeight =  NW_Str_Atoi(ucs2AttrVal);
                                    }
                                
                                if(NW_String_getStorage(&attrValueStr) != NULL)
                                    {
                                    NW_String_deleteStorage(&attrValueStr);
                                    }
                                    
                                if(ucs2AttrVal != NULL)
                                    {
                                    NW_Mem_Free(ucs2AttrVal);
                                    }
                                }                
                            }/*end while*/
                        
                        }/*end if(status == KBrsrSuccess)*/
                    else 
                        {
                        error = KErrGeneral;
                        goto imgAttrInfoFinal;
                        }
                    
                    } /*end if (NW_DOM_ElementNode_hasAttributes(elementNode) )*/
                }/*end if(nodeToken == NW_XHTML_ElementToken_img)*/
            
            
            }/*end if(value.object) */
        }/*end if(cannedImageBox)) */
imgAttrInfoFinal:
        
    if( cannedImageBox )
        {
        // scale images if they are wider than the screen width in small screen mode
        // do not scale if image size is specified by the markup
        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( cannedImageBox );
        NW_ASSERT( rootBox != NULL );
        // image scaling only applies to small screen layout
        if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
            {
            CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext( rootBox );
            NW_ASSERT( deviceContext != NULL );
            const NW_GDI_Rectangle_t* rectangle = deviceContext->DisplayBounds();
            // check if the image is scalable. if the width/height proporition is bigger
            // than 10, then we do not scale no matter how big the image is        
            TInt imagProp = Max( iFrameInfo.iOverallSizeInPixels.iHeight / iFrameInfo.iOverallSizeInPixels.iWidth, 
                iFrameInfo.iOverallSizeInPixels.iWidth / iFrameInfo.iOverallSizeInPixels.iHeight );
            //
            if( imagProp < 10 &&
                iFrameInfo.iOverallSizeInPixels.iWidth > rectangle->dimension.width )
                {
                // scale the image down close to the screen width
                // 
                // As per Symbian code, the height and width can't be reduced more
                // than 1/8 th of original size. It is found that Symbian currently
                // handling size 1/8, 1/4, 1/2 or original frame size correctly. Also,
                // width and height need to reduce in ratio (e.g. if width is reduced by half then
                // height need to reduce by half). So,used the following alogorithm.
                
                TInt  halfHeight = (iFrameInfo.iOverallSizeInPixels.iHeight + 1)/2;
                TInt  halfWidth =  (iFrameInfo.iOverallSizeInPixels.iWidth + 1)/2;
                TInt  oneFourHeight = (iFrameInfo.iOverallSizeInPixels.iHeight + 3)/4;
                TInt  oneFourWidth =  (iFrameInfo.iOverallSizeInPixels.iWidth + 3)/4;
                TInt  oneEightHeight = (iFrameInfo.iOverallSizeInPixels.iHeight + 7)/8;
                TInt  oneEightWidth =  (iFrameInfo.iOverallSizeInPixels.iWidth + 7)/8;
                
                if( rectangle->dimension.width > halfWidth)
                    {
                    iImage->srcHeight = iFrameInfo.iOverallSizeInPixels.iHeight;
                    iImage->srcWidth = iFrameInfo.iOverallSizeInPixels.iWidth;
                    }
                else
                    {
                    if( rectangle->dimension.width > oneFourWidth)
                        {
                        iImage->srcHeight = halfHeight;
                        iImage->srcWidth = halfWidth;
                        }
                    else if( rectangle->dimension.width > oneEightWidth)
                        {
                        iImage->srcHeight = oneFourHeight;
                        iImage->srcWidth = oneFourWidth;
                        }
                    else
                        {
                        iImage->srcHeight = oneEightHeight;
                        iImage->srcWidth = oneEightWidth;
                        }
                    }
                }
            }      
        }
    return error;
    
    }/*end IMG_ATTR_INFO()*/

/*
 * NewL
 *
 * Creates a new CEpoc32ImageDecoder object
 *
 * Parameters:
 *    aImage: associated OOC image object
 *
 * Returns:
 *    New image decoder
 */
CEpoc32ImageDecoder* CEpoc32ImageDecoder::NewL(NW_Image_Epoc32Simple_t* aImage)
{

  CEpoc32ImageDecoder* self = new (ELeave) CEpoc32ImageDecoder(aImage);
  CleanupStack::PushL(self);
  self->ConstructL();
 
  CleanupStack::Pop(); //self

  return self;
}

/*
 * ConstructL
 *
 * Creates image converter, wait object, and opens connection
 * to image converter.
 */
void CEpoc32ImageDecoder::ConstructL()
  {
  CActiveScheduler::Add( this );
  // open is called by whoever intialized this dedoder. This changes was introduced by
  // the "image open started - open completed" queueing functionality.
  // OpenL();
  // CreateComplete();
  }

/*
 * CEpoc32ImageDecoder::CEpoc32ImageDecoder
 *
 * Constructor
 */
CEpoc32ImageDecoder::CEpoc32ImageDecoder(NW_Image_Epoc32Simple_t* aImage) : CActive ( CActive::EPriorityLow + 1 ),
    iState(ID_IDLE),
    iImageConverter(NULL),
    iImage(aImage),
    iImageDataPtr((const TUint8 *)NW_Image_Epoc32Simple_GetRawData(aImage), NW_Image_Epoc32Simple_GetRawDataLength(aImage)),
    iIsAnimated(NW_FALSE),
    iCurrentFrame(0),
    iBitmap(NULL),
    iMask(NULL),
    iImageHasNotBeenOpened(NW_TRUE),
    iMoreData( EFalse ),
    iFirstFrameComplete( EFalse ),
    iRawDataComplete( EFalse ),
	iDRMOutBuf(NULL),
	iFinished(EFalse)
  {
  }



void CEpoc32ImageDecoder::DeleteImageConverterBitmapMask()
  {
  if (iImageConverter != NULL)
    {
    /* Closing image converter. Cancels any outstanding operation */
    iImageConverter->Cancel();
    delete iImageConverter;
    iImageConverter = NULL;
    }
  delete iBitmap;
  delete iMask;
  iBitmap = NULL;
  iMask = NULL;
  // call shell to do some compress on heap ( prevent memory fregmentation )
  CShell* shellInstance = REINTERPRET_CAST(CShell*, (NW_Ctx_Get(NW_CTX_BROWSER_APP, 0)));
  NW_ASSERT( shellInstance != NULL );
  shellInstance->StartIdle();
  }


/*
 * CEpoc32ImageDecoder::CEpoc32ImageDecoder
 *
 * Destructor. Closes image converter, stops wait object, frees
 * data.
 */
CEpoc32ImageDecoder::~CEpoc32ImageDecoder()
  {
  Cancel();
  DeleteImageConverterBitmapMask();
  delete[] iDRMOutBuf;
  iDRMOutBuf = NULL;
  }



/*
 * Function for handling the DRM images.
 * This function checks if image is DRM 
 * then handle this.
 *
 *
 */

TInt CEpoc32ImageDecoder::CheckForDRMImage()
{
 TInt error = KErrNone;
 using namespace ContentAccess;
 RArray<ContentAccess::TAgent> agents;
 TInt i;
 HBufC8 *DRMInBuf;
 TPtr8 ptrInBuf(NULL,0,0);
 TInt inBufLen = 0;
 TRequestStatus status;
 TInt contentMgrErr = KErrNone;						 
 TAgent agentDRM;
 TInt drmDeCryptBufLen = 0;

	 if(iDRMOutBuf != NULL)
	 {
	  delete iDRMOutBuf;
	  iDRMOutBuf = NULL;
	 }

	drmDeCryptBufLen = iImageDataPtr.Size()+256;
	iDRMOutBuf = new (ELeave) TUint8[drmDeCryptBufLen];
	
  
     //One more byte than the original buffer for DRM code
	 inBufLen = iImageDataPtr.Size() + 1;
	 DRMInBuf = HBufC8::NewL(inBufLen + 1);

	 ptrInBuf.Set(DRMInBuf->Des() );
     ptrInBuf.Append(EView);
	 ptrInBuf.Append( (const TUint8 *) iImageDataPtr.Ptr(), iImageDataPtr.Size());
	 TPtr8 ptrOutBuf(iDRMOutBuf,0, drmDeCryptBufLen );
	 
	 ContentAccess::CManager* manager = 
					   (ContentAccess::CManager*) NW_Ctx_Get(NW_CTX_CONT_ACCESS_MGR, 0);
      
	 manager->ListAgentsL( agents );

	 for (i = 0; i < agents.Count(); i++)
		{
		if (agents[i].Name().Compare(KOmaDrm2AgentName) == 0)
			{
			 agentDRM = agents[i];
			 break;
			}
		}
		

	  manager->AgentSpecificCommand( agentDRM, EBufferContainsOma1Dcf, 
									   iImageDataPtr ,ptrOutBuf, status);
      
	  User::WaitForRequest(status);

	  if(status == KErrNone)
		{
		//Now decrypt the buffer.
		 contentMgrErr = manager->AgentSpecificCommand( agentDRM, EDecryptOma1DcfBuffer, 
													 ptrInBuf,ptrOutBuf);
         if(contentMgrErr == KErrNone)
			{
			 iImageDataPtr.Set((const TUint8*) ptrOutBuf.Ptr(),ptrOutBuf.Size());
			 delete iImageConverter;
			 iImageConverter = NULL;
  			 TRAP(error, iImageConverter = CBufferedImageDecoder::NewL(iRfs))
             if(error == KErrNone)
			 {
			  TRAP(error, iImageConverter->OpenL(iImageDataPtr));
			 }
			} 
		  else
		  {
			error = contentMgrErr;
		  }

	   } 
	 agents.Close();
	 delete DRMInBuf;
     DRMInBuf = NULL;

 return error;
}


/*
 * Open
 *
 * Returns:
 *
 */
void CEpoc32ImageDecoder::Open()
    {
    TInt error = KErrNone;
    
    NW_LOG0(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::Open started");
    if (iImageHasNotBeenOpened && iImage)
        {
        // inform the observer
        NW_Image_AbstractImage_ImageOpenStarted(NW_Image_AbstractImageOf(iImage));
        }

    // create image converter
    if (!iImageConverter)
        {
        // Symbian API requires a handle to RFs, but it is never used.
        // iRFs is not connected to the server
        iImageDataPtr.Set((const TUint8 *)NW_Image_Epoc32Simple_GetRawData(iImage),
                          NW_Image_Epoc32Simple_GetRawDataLength(iImage));
        
        if (iImage->imageType == EWbmpImage)
            {
            // We have a WBMP image
            TRAP(error, iImageConverter = CBufferedImageDecoder::NewL(iRfs))
                if (error == KErrNone)
                    {
                    TRAP(error, iImageConverter->OpenL(iImageDataPtr, KWbmpMimeType()));
					if(error != KErrNone)
					 {
                      error = CheckForDRMImage();
					 } /*end if(error != KErrNone)*/
                    }
            }/*end if (iImage->imageType == EWbmpImage)*/
        else if (iImage->imageType == EOtaImage)
            {
            // We have a OTA image
            TRAP(error, iImageConverter = CBufferedImageDecoder::NewL(iRfs))
                if (error == KErrNone)
                    {
                    TRAP(error, iImageConverter->OpenL(iImageDataPtr, KOtaMimeType()));
					 if(error != KErrNone)
					 {
                      error = CheckForDRMImage();
					 } /*end if(error != KErrNone)*/
                    }
			} /*end else if (iImage->imageType == EOtaImage)*/
        else
            {
            TRAP(error, iImageConverter = CBufferedImageDecoder::NewL(iRfs))
                if (error == KErrNone)
                    {

					 TRAP(error, iImageConverter->OpenL(iImageDataPtr));

						if(error != KErrNone)
						{
                         error = CheckForDRMImage();
						} /*end if(error != KErrNone)*/
					} /*end if (error == KErrNone)*/
			} /*end else*/

        // check if open completed
        if (error == KErrNone && !iImageConverter->ValidDecoder())
            {
            error = KErrUnderflow;
            }
        CreateComplete(error);
        }
    else if (!iImageConverter->ValidDecoder())
        {
        CreateComplete(KErrUnderflow);
        }
    else
        {
        CreateComplete(KErrNone);
        }
    }

/*
 * Decode
 *
 * Non-leaving version of DoDecodeL
 *
 * Returns:
 *    KErrNone on success, error code otherwise.
 */
TInt CEpoc32ImageDecoder::Decode()
  {
  NW_LOG0(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::Decode started");
  TInt error = KErrNone;
  if( !iImageConverter )
    {
    // no converter
    Open();
    }
  else if( !iImageConverter->ValidDecoder() )
    {
    // no decoder
    // check if the decoder is being created
    // if not then call open again.
    if( !IsActive() )
      {
      CreateComplete( KErrUnderflow );
      }
    }
  else if (iState == ID_IDLE)
    {
    TRAP(error, DoDecodeL());
    }
  if (error != KErrNone)
    {
    DeleteImageConverterBitmapMask();
    }
  return error;
}

/*
 * imageSizeCheck
 *
 * Check the size of the image and take actions accordingly 
 */


 NW_Bool CEpoc32ImageDecoder::ImageSizeCheck(TSize aFrameInfoOverallSize, TSize* currSize)
 {
  NW_Bool reSizing = NW_FALSE;
  
  /* As per Symbian code, the height and width can't be reduced more
    than 1/8 th of original size. It is found that Symbian currently
    handling size 1/8, 1/4, 1/2 or original frame size correctly. Also,
    width and height need to reduce in ratio (e.g. if width is reduced by half then
    height need to reduce by half). So,used the following alogorithm.
  */
  
  TInt  halfHeight = (aFrameInfoOverallSize.iHeight + 1)/2;
  TInt  halfWidth =  (aFrameInfoOverallSize.iWidth + 1)/2;
  TInt  oneFourHeight = (aFrameInfoOverallSize.iHeight + 3)/4;
  TInt  oneFourWidth =  (aFrameInfoOverallSize.iWidth + 3)/4;
  TInt  oneEightHeight = (aFrameInfoOverallSize.iHeight + 7)/8;
  TInt  oneEightWidth =  (aFrameInfoOverallSize.iWidth + 7)/8;


  if( (iImage->srcWidth > 0) && (iImage->srcHeight > 0) )
  {
        reSizing = NW_TRUE;
        
        if( (iImage->srcHeight > halfHeight) || (iImage->srcWidth > halfWidth))
            {
            currSize->iHeight = aFrameInfoOverallSize.iHeight;
            currSize->iWidth = aFrameInfoOverallSize.iWidth;
            }
        else if( (iImage->srcHeight > oneFourHeight) || (iImage->srcWidth > oneFourWidth) )
            {
            currSize->iHeight = halfHeight;
            currSize->iWidth = halfWidth;
            }
        else if( (iImage->srcHeight > oneEightHeight) || (iImage->srcWidth > oneEightWidth) )
            {
            currSize->iHeight = oneFourHeight;
            currSize->iWidth = oneFourWidth;
            }
        else
            {
            /*Can't be less than 1/8th of size */
            currSize->iHeight = oneEightHeight;
            currSize->iWidth = oneEightWidth;
            }
        
  
  } /*end if( (iImage->srcWidth > 0) && (iImage->srcHeight > 0) )*/
//finalImageSize: 
  return reSizing;
  
 }
/*
 * DoDecodeL
 *
 * Starts decoding image. When completed, the function "ConvertComplete"
 * will be called.
 *
 * Returns:
 *    KErrNone on success, error code otherwise.
 */
void CEpoc32ImageDecoder::DoDecodeL()
{
 TInt error;
 NW_Bool actualSizing;
 TSize currSize;
 

  // check animation only if we've got all the data
  if( iRawDataComplete )
    {
    CheckAnimation();
    }

  actualSizing = ImageSizeCheck(iFrameInfo.iOverallSizeInPixels, &currSize);

  iBitmap = new (ELeave) CFbsBitmap();
 
  if(actualSizing == NW_TRUE)
  { 
   error = iBitmap->Create( currSize, GetDisplayMode(iFrameInfo) );
  }
  else
  {
  error = iBitmap->Create( iFrameInfo.iOverallSizeInPixels, GetDisplayMode(iFrameInfo) );
  }

  if(error != KErrNone)
  {
   User::LeaveIfError(error);
  }

  if ( iFrameInfo.iFlags & TFrameInfo::ETransparencyPossible )
  {
    iMask = new (ELeave) CFbsBitmap();

    TSize imageSize = actualSizing ? (currSize) : (iFrameInfo.iOverallSizeInPixels);
    
    // A mask cannot have a color. See comment in ImageConversionPriv.cpp
    if (iFrameInfo.iFlags & TFrameInfo::EAlphaChannel)
        {
        User::LeaveIfError(iMask->Create( imageSize, EGray256));
        }
    else
        {
        User::LeaveIfError(iMask->Create( imageSize, EGray2));
        }

    iImageConverter->Convert(&iStatus, *iBitmap, *iMask, iCurrentFrame);
    SetActive();
    iState = ID_DECODING;
  }
  else
  {
    iMask = NULL;
    iImageConverter->Convert(&iStatus, *iBitmap, iCurrentFrame);
    SetActive();
    iState = ID_DECODING;
  }
}


/*
 * GetLoopCount
 *
 * Checks for presence of a netscape 2.0 extension header which
 * contains a loop count. If a loop count value is found, then
 * use that value to determine the loop count, otherwise default to
 * looping 1x.
 *
 * This code maps loop counts as they are expressed in image files (both
 * with and without the netscape 2.0 extension header present) to a
 * loop count that can be used internally.
 *
 * Specifically for animated images,
 *
 *    if the netscape 2.0 extension header is present,
 *      an extension header value of 0 maps to -1 (infinite loop)
 *      an extension header value of N maps to N+1
 *
 *    if the extension header is not present,
 *      set loop count to 1 (same behavior as IE or opera)
 *
 * For non-animated images, loop count is set to 0.
 *
 */
TInt16 CEpoc32ImageDecoder::GetLoopCount()
{
  if(!iIsAnimated)
  {
    return 0;       /* no animation = no looping */
  }

  /* have a loop, figure out count */

  /* first see if have a netscape 2.0 extension header */
  TInt16  loopCountFromFile=0;
  const TUint8 extString[] = { 'N', 'E', 'T', 'S', 'C', 'A', 'P','E','2','.','0','\3','\1' };
  const TInt sizeofextString = sizeof(extString);

  TPtrC8 rawDataPtr((TUint8*)iImage->rawData, iImage->rawDataLength);
  TInt offset = rawDataPtr.Find(extString, sizeofextString);
  if(offset != KErrNotFound)
  {
    /* found a header, get the loop count -
       (the loop count is in the 2 bytes following the header string listed above,
        stored low byte then high byte)
    */
    loopCountFromFile = (TInt16)((rawDataPtr[offset+sizeofextString+1] * 256) +
                                  rawDataPtr[offset+sizeofextString]);
    if(loopCountFromFile != 0)
    {
      return ++loopCountFromFile;       /* +1 to make it 1 based rather than 0 based */
    }
    else
    {
      /* 0 indicates infinite - map to internal loop count infinite value */
      return -1;
    }
  }
  else
  {
    /* no header found, assume 1x thru loop */
    return 1;
  }
}


/*
 * CheckAnimation
 *
 * Checks to see if an image is animated, then notifies the owner
 * (an NW_Image_Epoc32Simple object).
 */
void CEpoc32ImageDecoder::CheckAnimation()
{
  NW_LOG0(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::CheckAnimation started");

  // check if the header needs further processing
  if( !iImageConverter->IsImageHeaderProcessingComplete() )
    {
    iImageConverter->ContinueProcessingHeaderL();
    }

  /* Get the current frame information from the image converter */
  iFrameInfo = iImageConverter->FrameInfo( iCurrentFrame );

  /* FrameCount() doesn't return number of frames, it returns the
   * number of frames it has discovered so far - we won't know if
   * it's animated until after the first frame is converted.
   */
  TUint frames = iImageConverter->FrameCount();
  NW_LOG2(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::CheckAnimation: iIsAnimated = %d frameCount = %d", ((TInt)((iIsAnimated)?1:0)), frames);
  if( ( iIsAnimated || (frames > 1)) && !iFinished)
    {
    TInt64 delay = iFrameInfo.iDelay.Int64() / TInt64(1000);
    NW_Int32 delay32 = I64INT(delay);

    iIsAnimated = (delay32 < MAXIMUM_DELAY);

    NW_Image_Epoc32Simple_SetAnimation(NW_Image_AbstractImageOf(iImage), iIsAnimated,
                                       delay32 );
    }
  NW_LOG0(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::CheckAnimation ending");
}

/*
 * IncrementImage
 *
 * Called on an animated image to display the next image.
 *
 * Returns:
 *    Sets startingFrameSequenceOver to T if the frame sequence
 *    is started over otherwise it is set to F.
 *
 *    KErrNone on success, error code otherwise.
 */
NW_Bool CEpoc32ImageDecoder::IncrementImage(NW_Bool& startingFrameSequenceOver)
{
  if(iImageConverter != NULL) 
  {

  startingFrameSequenceOver = NW_FALSE;

  /* Don't do anything if we are in the process of decoding an image. */
  if (iState != ID_IDLE || iImageConverter == NULL)
    {
    return NW_FALSE;
    }

  /* Increment the image. */
  iCurrentFrame++;

  /* If reached the end, go back to the beginning.  Note, the method
   * "FrameCount" returns the number of frames discovered so far.
   */
  if (iCurrentFrame >= (TUint)iImageConverter->FrameCount())
    {
    iCurrentFrame = 0;
    startingFrameSequenceOver = NW_TRUE;
    }

  return NW_TRUE;
}
  else
  {
   return NW_FALSE;
  }
}


/*
 * PartialNextChunkL
 * new chunk of raw data
 */
void CEpoc32ImageDecoder::PartialNextChunkL( NW_Buffer_t* aNextChunk )
  {
  // append the incoming raw data to the image converter's buffer
  if( iImageConverter )
    {
    TPtrC8 nextChunk;
    nextChunk.Set( aNextChunk->data, aNextChunk->length );

    iImageConverter->AppendDataL( nextChunk );

    // convert the first frame and wait for all the raw data
    // if the image is animated
    if( !iFirstFrameComplete )
      {
      // if the converter is busy then just set the flag indicating
      // that we received more raw data
      if( iState == ID_IDLE )
        {
        // check if the decoder has already been created
        if( iImageConverter->ValidDecoder() )
          {
          TRequestStatus* status = &iStatus;
          iState = ID_DECODING;
          // more image data
          iImageConverter->ContinueConvert( status );
          SetActive();
          }
        else
          {
          // no decoder means not enough info came in. let's continue open.
          iState = ID_INITIALIZING;
          TRAPD( error, iImageConverter->ContinueOpenL() );
          if( error != KErrNone )
            {
            // if ContinueOpenL leaves we need to inform the observer that the open failed
            NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImageOf( iImage ), NW_TRUE,
                NW_Image_AbstractImage_SuppressDupReformats_Default );                    
            User::Leave( error );
            }
          CreateComplete( iImageConverter->ValidDecoder() ? KErrNone : KErrUnderflow );
          }
        }
      else
        {
        // converter is busy
        iMoreData = ETrue;
        }
      }
    }
  }


/*
 * ImageDataComplete
 * end of raw data
 */
void CEpoc32ImageDecoder::ImageDataComplete()
  {
  // this function checks if an image needs further processing
  // ( for examp if an animated image comes in multiple chunks )
  iRawDataComplete = ETrue;

#ifdef INCREMENTAL_IMAGE_ON
  // check if the imageconverter is available and get the converting status
  if( iImageConverter )
    {
    // return if the converter is busy
    if( iState == ID_IDLE && iFirstFrameComplete )
      {
      // no more data. let's check if the image is animated
      CheckAnimation();

      // If it's not animated, there is no further use for the converter
      if( !iIsAnimated )
        {
        delete iImageConverter;
        iImageConverter = NULL;
        }
      else
        {
        // set loop count 
        NW_Image_AbstractImage_SetLoopCount( NW_Image_AbstractImageOf( iImage ), 
          GetLoopCount() );
        // fake image decode forces the image box to refresh.
        // otherwise the animation just does not start up
        // inform the observers
        NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImageOf( iImage ), NW_FALSE );
        }
      }
    }
#else // INCREMENTAL_IMAGE_ON
  Open();
#endif // INCREMENTAL_IMAGE_ON
  }

/*
 * ConvertComplete
 *
 * Image decode has completed. Transfers ownership of the bitmaps
 * back to the OOC image object, which causes the image to be drawn.
 *
 * Parameters:
 *    aError: Error code from conversion
 */
void CEpoc32ImageDecoder::ConvertComplete()
  {
  iState = ID_IDLE;
  NW_LOG0(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::ConvertComplete started");

  switch (iStatus.Int())
    {
    case KErrNone:
      {
      if( iBitmap )
        {
        // check animation only if we've got all the data
        if( iRawDataComplete )
          {
          NW_LOG1(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::ConvertComplete: Calling CheckAnimation. iFirstFrameComplete = %d", ((TInt)((iFirstFrameComplete)?1:0)));

          CheckAnimation();
          // set loop if the data are already complete and
          // this is the first frame as it means that
          // the last chunk came in while we were busy either
          // with opening or with decoding the first frame
          if( !iFirstFrameComplete )
            {
            // set loop count 
            NW_Image_AbstractImage_SetLoopCount( NW_Image_AbstractImageOf( iImage ), 
              GetLoopCount() );
            }
          }
        /* Transfer ownership of bitmaps to OOC object */
        NW_Image_Epoc32Simple_SetBitmap(iImage, (void*)iBitmap, (void*)iMask, iCurrentFrame, (void*)&iFrameInfo, NW_TRUE );
        // inform the observers
        NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImageOf( iImage ), NW_FALSE );
        // Transfer ownership of Bitmap and mask to OOC
        iBitmap = NULL;
        iMask = NULL;
        iFirstFrameComplete = ETrue;
        //
        if( iIsAnimated == NW_FALSE )
          {
          // Set the decode state after decoding complete
          iState = ID_DECODE_COMPLETE;
          DeleteImageConverterBitmapMask();
          }
        }
      break;
      }
    case KErrUnderflow:
      {

      // When converting the operation can complete with KErrUnderflow, if there is
      // insufficient information in the descriptor. In this situation, ContinueConvert()
      // should be called repeatedly until the descriptor has accumulated enough information
      // for ContinueConvert() to complete with KErrNone

      if( iBitmap )
        {
        // partial image decoding
        NW_Image_Epoc32Simple_SetBitmap(iImage, (void*)iBitmap, (void*)iMask, iCurrentFrame, (void*)&iFrameInfo, NW_FALSE );
        // DO NOT transfer ownership of Bitmap and mask to OOC
        NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImageOf( iImage ), NW_FALSE );
        }
      // check if more data is available
      if( iMoreData )
        {
        iState = ID_DECODING;
        TRequestStatus* status = &iStatus;
        iImageConverter->ContinueConvert( status );
        SetActive();
        iMoreData = EFalse;
        }
      break;
      }
    default:
      {
      DeleteImageConverterBitmapMask();
      break;
      }
    }
  }

/*
 * OpenComplete
 *
 * Image open has completed. Tell the OOC image object of the image dimensions,
 * and trigger the synchronous OpenL to continue.
 *
 * Parameters:
 *    aError: Error code from conversion
 */
void CEpoc32ImageDecoder::OpenComplete()
  {
  NW_GDI_Dimension3D_s size;
  iState = ID_IDLE;
  TSize currSize;


  NW_LOG0(NW_LOG_LEVEL5, "CEpoc32ImageDecoder::OpenComplete started");
  switch( iStatus.Int() )
    {
  case KErrNone:
      {
       iFrameInfo = iImageConverter->FrameInfo( iCurrentFrame );
       if(iImage->bitmap == NULL ){
         TInt error;
         NW_Bool actualSizing;
         TFrameInfo aFrameInfo = iImageConverter->FrameInfo(0);
         actualSizing = ImageSizeCheck(aFrameInfo.iOverallSizeInPixels, &currSize);
           
         CFbsBitmap* pDestBitmap = new (ELeave) CFbsBitmap();
         if(actualSizing == NW_TRUE) { 
            error = pDestBitmap->Create( currSize, GetDisplayMode(aFrameInfo) );
         }
         else{
             error = pDestBitmap->Create( aFrameInfo.iOverallSizeInPixels, GetDisplayMode(aFrameInfo) );
          }
         iImage->bitmap = pDestBitmap;
         if(iImage->mask == NULL && aFrameInfo.iFlags & TFrameInfo::ETransparencyPossible ){
            CFbsBitmap* pDestMask = new (ELeave) CFbsBitmap();
            TSize imageSize = actualSizing ? (currSize) : (aFrameInfo.iOverallSizeInPixels);
            // A mask cannot have a color. See comment in ImageConversionPriv.cpp
            if (aFrameInfo.iFlags & TFrameInfo::EAlphaChannel){
                User::LeaveIfError(pDestMask->Create( imageSize, EGray256));
            }
            else{
                User::LeaveIfError(pDestMask->Create( imageSize, EGray2));
            }
            iImage->mask = pDestMask;
           }
          }

      if( iImageHasNotBeenOpened )
        {
        iImageHasNotBeenOpened = NW_FALSE;
        size.depth = 1;

        ImgAttrInfo();
        
        NW_Bool actualSizing = ImageSizeCheck(iFrameInfo.iOverallSizeInPixels, &currSize);
        
        
        // default value of srcHeight and srcWidth; they will change is image is scaled
        iImage->srcHeight = iFrameInfo.iOverallSizeInPixels.iHeight;
        iImage->srcWidth = iFrameInfo.iOverallSizeInPixels.iWidth;
        // set the original size
        size.width = iFrameInfo.iOverallSizeInPixels.iWidth;
        size.height = iFrameInfo.iOverallSizeInPixels.iHeight;
        //
        NW_Image_Epoc32Simple_SetSize(NW_Image_AbstractImageOf(iImage), &size);
        NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImageOf( iImage ), NW_FALSE,
            NW_Image_AbstractImage_SuppressDupReformats_Default );
        // image open can force decoding.
        //lint -e{774} Boolean within 'if' always evaluates to True
        // iImageHasNotBeenOpened set to TRUE from NW_LMgr_ImageCH_Epoc32ContentHandler_HandleValidImageBox
        // under special condition; do NOT delete iImageConverter in that case
        if( !iImageHasNotBeenOpened && iState != ID_DECODING )
          {
          DeleteImageConverterBitmapMask();
          }
        }
      else
        {
        TRAPD(error, DoDecodeL());
        if (error != KErrNone)
          {
          DeleteImageConverterBitmapMask();
          NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImageOf( iImage ), NW_TRUE );
          }
        }
      break;
      }
    case KErrUnderflow:
      {

      // When opening the operation can complete with KErrUnderflow, if there is
      // insufficient information in the descriptor. In this situation, ContinueOpen()
      // should be called repeatedly until the descriptor has accumulated enough information
      // for ContinueOpen() to complete with KErrNone

      // inform the observers
      if( iMoreData )
        {
        iState = ID_INITIALIZING;
        TRAPD( error, iImageConverter->ContinueOpenL() );
        if( error != KErrNone )
          {
          // if ContinueOpenL leaves we need to inform the observer that the open failed
          NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImageOf( iImage ), NW_TRUE,
              NW_Image_AbstractImage_SuppressDupReformats_Default );                    
          User::Leave( error );
          }
        CreateComplete( iImageConverter->ValidDecoder() ? KErrNone : KErrUnderflow );
        iMoreData = EFalse;
        }
	  else //If there is no more data under the KErrUnderflow, it is an error such as corrupted data 
	  {
          NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImageOf( iImage ), NW_TRUE,
              NW_Image_AbstractImage_SuppressDupReformats_Default );                    
	  }

      break;
      }
    default:
      {
      // we failed to open the image. let the observer know about it.
      NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImageOf( iImage ), NW_TRUE,
          NW_Image_AbstractImage_SuppressDupReformats_Default );
      break;
      }
    }
  }

/*
 * CreateComplete
 *
 *
 */
void CEpoc32ImageDecoder::CreateComplete( TInt aError )
{
  TRequestStatus* status = &iStatus;
  iState = ID_INITIALIZING;
  User::RequestComplete( status, aError );
  SetActive();
}

/*
 * GetDisplayMode
 *
 * Calculates display mode copying the logic from the image viewer
 */
TDisplayMode CEpoc32ImageDecoder::GetDisplayMode( TFrameInfo& aFrameInfo )
{
  if( aFrameInfo.iFlags & TFrameInfo::ECanDither )
    {
	  if( aFrameInfo.iFrameDisplayMode < EColor64K )
	    {
  	  return aFrameInfo.iFrameDisplayMode;
	    }
	  else
	    {      // This should come from settings
	    return EColor4K;
	    }
    }
  else
    {
	  return aFrameInfo.iFrameDisplayMode;
    }
  }

void CEpoc32ImageDecoder::RunL()
  {
  switch (iState)
    {
    case ID_INITIALIZING:
      {
      OpenComplete();
      break;
      }
    case ID_DECODING:
      {
      ConvertComplete();
      break;
      }
    default:
      {
      NW_ASSERT(0);
      }
    }
  }


/*
 * ResetImage
 *
 * Called on an animated image to display the last frame.
*/
void CEpoc32ImageDecoder::ResetImage()
{
  iIsAnimated = NW_FALSE;
  iCurrentFrame = (TUint)iImageConverter->FrameCount() - 1;	
  iFinished = ETrue;
}

TBool CEpoc32ImageDecoder::Finished()
{
	return iFinished;
}

