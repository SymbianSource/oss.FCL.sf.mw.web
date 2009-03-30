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
#include "nw_image_epoc32simpleimagei.h"
#include "Epoc32ImageDecoder.h"
#include "nw_lmgr_rootbox.h"
#include "nw_hed_documentroot.h"
#include "HEDDocumentListener.h"
#include "nw_hed_contenthandler.h"
#include "nw_image_iimageobserver.h"
#include "nw_adt_resizablevector.h"
#include "GDIDeviceContext.h"

#include "nw_gdi_types.h"

#include <fbs.h>
#include <bitdev.h>
#include "BrsrStatusCodes.h"
#include "nwx_logger.h"
#include "MemoryManager.h"


// Forward declaration which can't be put inside header file without breaking
// compilation of OOC files.

TBrowserStatusCode 
scaleBitmap( CFbsBitmap* bitmap, CFbsBitmap** scaledBitmap, TRect* rect );


void
_NW_Image_Epoc32SimpleImage_ImageOpened( NW_Image_AbstractImage_t* aAbstractImage, NW_Bool aFailed,
                                     NW_Int16 aSuppressDupReformats )
    {
    // add the image to out of view list
    NW_Image_Epoc32Simple_t* thisObj = NW_Image_Epoc32SimpleOf (aAbstractImage);

    if (thisObj->docNodeList)
        {
        NW_HED_DocumentNode_t* docNode = 
            *(NW_HED_DocumentNode_t**)NW_ADT_Vector_ElementAt(thisObj->docNodeList, 0);
        NW_ASSERT(docNode);
        NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode(docNode);
        NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox(docRoot);
        NW_LMgr_RootBox_AddToOutOfViewList(rootBox, aAbstractImage);
        }
    _NW_Image_AbstractImage_ImageOpened(aAbstractImage, aFailed, aSuppressDupReformats );
    }

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Image_Epoc32Simple_Class_t NW_Image_Epoc32Simple_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Image_AbstractImage_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Image_Epoc32Simple_t),
    /* construct               */ _NW_Image_Epoc32Simple_Construct,
    /* destruct                */ _NW_Image_Epoc32Simple_Destruct
  },
  { /* NW_Image_Abstract       */
    /* getSize                 */ _NW_Image_Epoc32Simple_GetSize,
    /* draw                    */ _NW_Image_Epoc32Simple_Draw,
    /* drawScaled              */ _NW_Image_Epoc32Simple_DrawScaled,
    /* drawInRect              */ _NW_Image_Epoc32Simple_DrawInRect,
    /* incrementImage          */ _NW_Image_Epoc32Simple_IncrementImage,
    /* getLoopCount            */ _NW_Image_AbstractImage_GetLoopCount,
    /* setLoopCount            */ _NW_Image_AbstractImage_SetLoopCount,
    /* getDelay                */ _NW_Image_AbstractImage_GetDelay,
    /* isAnimated              */ _NW_Image_AbstractImage_IsAnimated,
    /* setImageObserver        */ _NW_Image_AbstractImage_SetImageObserver,
    /* removeImageObserver     */ _NW_Image_AbstractImage_RemoveImageObserver,
    /* imageOpened             */ _NW_Image_Epoc32SimpleImage_ImageOpened,
    /* imageSizeChanged        */ _NW_Image_AbstractImage_ImageSizeChanged,
    /* imageDecoded            */ _NW_Image_AbstractImage_ImageDecoded,
    /* imageOpenStarted        */ _NW_Image_AbstractImage_ImageOpenStarted,
    /* forceImageDecode        */ _NW_Image_Epoc32Simple_ForceImageDecode
  },
  { /* NW_Image_Simple         */
    /* partialNextChunk        */ _NW_Image_Epoc32Simple_PartialNextChunk
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

TBrowserStatusCode 
_NW_Image_Epoc32Simple_Construct (NW_Object_Dynamic_t* dynamicObject, va_list *argp)
    {
    NW_TRY(status) 
        {
        NW_Image_Epoc32Simple_t* thisObj;
        NW_HED_DocumentNode_t* documentNode;
        
        // parameter assertion block 
        NW_ASSERT(NW_Object_IsInstanceOf(dynamicObject, &NW_Image_Epoc32Simple_Class));
        NW_ASSERT(argp != NULL);
        
        // for convenience 
        thisObj = NW_Image_Epoc32SimpleOf (dynamicObject);
        
        // initialize the member variables 
        // Won't know if image is animated until after it is opened by decoder 
        NW_Image_AbstractImageOf(thisObj)->isAnimated = NW_FALSE;
        thisObj->prevFlags = 0;
        thisObj->needsDecode = NW_FALSE;
        thisObj->imageDecoding = NW_FALSE;
        
        NW_GDI_Dimension3D_t size = va_arg (*argp, NW_GDI_Dimension3D_t); 
        thisObj->size = size;
        
        const void* bitmap = va_arg (*argp, const void*); 
        thisObj->bitmap = bitmap;
        
        const void* mask = va_arg (*argp, const void*); 
        thisObj->mask = mask;

        thisObj->invertedMask = (NW_Bool)va_arg (*argp, NW_Uint32);
        
        // "isTransparent" doesn't appear to be used by any Epoc code 
        NW_Bool isTransparent = (NW_Bool)va_arg (*argp, NW_Uint32);
        (void)isTransparent;
        
        // Do we have a image type that isn't recognized by Symbian, but we handle
        thisObj->imageType = (TImageType)va_arg (*argp, NW_Uint32);
        
        // in order to make sure that we take rawData ownership, 
        // the code should not leave before this point.
        thisObj->rawData = va_arg (*argp, void*); 
        thisObj->rawDataLength = va_arg (*argp, NW_Int32); 
        
        documentNode = va_arg (*argp, NW_HED_DocumentNode_t*);
        if (documentNode)
            {
            thisObj->docNodeList = (NW_ADT_DynamicVector_t*)
                NW_ADT_ResizableVector_New(sizeof(void*), 10, 5 );
            // OOM check
            NW_THROW_OOM_ON_NULL(thisObj->docNodeList, status);
            
            NW_THROW_OOM_ON_NULL(NW_ADT_DynamicVector_InsertAt(thisObj->docNodeList, 
                &documentNode, NW_ADT_Vector_AtEnd), status);
            }
        
        // If the image is not provided, it needs to be converted from the raw data.
        // Construct an image decoder to do this. 
        if (!thisObj->bitmap)
            {
            TRAPD(err, thisObj->decoder = CEpoc32ImageDecoder::NewL(thisObj));
            if (err == KErrNoMemory)
                {
                NW_THROW_STATUS(status, KBrsrOutOfMemory);
                }
            else if (err != KErrNone)
                {
                NW_THROW_STATUS(status, KBrsrFailure);
                }
            }

           thisObj->srcHeight = 0;
           thisObj->srcWidth  = 0;

        // and finally invoke our superclass constructor 
        status = _NW_Image_AbstractImage_Construct(dynamicObject, argp);
        
        _NW_THROW_ON_ERROR(status);      
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

/* ------------------------------------------------------------------------- */
void
_NW_Image_Epoc32Simple_Destruct (NW_Object_Dynamic_t* dynamicObject)
  {
  NW_Image_Epoc32Simple_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_Image_Epoc32Simple_Class));

  /* for convenience */
  thisObj = NW_Image_Epoc32SimpleOf (dynamicObject);

  // first notify the observers on the destroy event
  NW_Image_AbstractImage_ImageDestroyed( NW_Image_AbstractImageOf( thisObj ) );

  /* Delete decoder, rawData & images */
  CEpoc32ImageDecoder* decoder = STATIC_CAST(CEpoc32ImageDecoder*, thisObj->decoder);
  delete decoder;

  NW_Image_Epoc32Simple_DestroyRawData( thisObj );

  if( thisObj->docNodeList )
    {
    NW_Object_Delete(thisObj->docNodeList);
    }

  // image ownership is transfered when decoding completes.
  if( thisObj->imageDecoding == NW_FALSE )
    {
    CFbsBitmap* oldBitmap = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)thisObj->bitmap);
    delete oldBitmap;
    CFbsBitmap* oldMask = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)thisObj->mask);
    delete oldMask;
    }
  }



/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_Epoc32Simple_PartialNextChunk( NW_Image_AbstractImage_t* image,
                                         NW_Buffer_t* nextChunk )
  {
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Image_Epoc32Simple_t* thisObj;

  NW_ASSERT( NW_Object_IsInstanceOf( image, &NW_Image_Epoc32Simple_Class ) );

  thisObj = NW_Image_Epoc32SimpleOf( image );
  NW_ASSERT( thisObj->decoder!= NULL );

  // buffer chunks
  void* tempBuffer;
  tempBuffer = NW_Mem_Malloc( thisObj->rawDataLength + nextChunk->length );
  if( tempBuffer != NULL )
    {
    // copy old buffer
    NW_Mem_memcpy( tempBuffer, thisObj->rawData, thisObj->rawDataLength );
    // free old buffer
    NW_Mem_Free( thisObj->rawData );
    // copy new chunk  
    NW_Mem_memcpy( ((NW_Byte*)tempBuffer) + thisObj->rawDataLength, 
      nextChunk->data, nextChunk->length );
    // set new buffer`
    thisObj->rawData = tempBuffer;
    // update length info
    thisObj->rawDataLength+= nextChunk->length;

#ifdef INCREMENTAL_IMAGE_ON
    CEpoc32ImageDecoder* decoder = STATIC_CAST( CEpoc32ImageDecoder*, thisObj->decoder );

    TRAPD( error, decoder->PartialNextChunkL( nextChunk ) );

    if( error == KErrNoMemory )
      {
      status = KBrsrOutOfMemory;
      }
    else if( error != KErrNone )
      {
      status = KBrsrFailure;
      }
#endif // INCREMENTAL_IMAGE_ON

    }
  else
    {
    status = KBrsrOutOfMemory;
    }
  // release chunk
  NW_Buffer_Free( nextChunk );

  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_Epoc32Simple_GetSize (const NW_Image_AbstractImage_t* image,
                                NW_GDI_Dimension3D_t* size)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Epoc32Simple_Class));
  NW_ASSERT (size != NULL);

  /* set the size and return */
  *size = NW_Image_Epoc32SimpleOf (image)->size;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_Epoc32Simple_Draw (NW_Image_AbstractImage_t* image,
                             CGDIDeviceContext* deviceContext,
                             const NW_GDI_Point2D_t* location)
{
  return NW_Image_AbstractImage_DrawInRect (image, deviceContext, location, NULL, NULL);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_Epoc32Simple_DrawScaled (NW_Image_AbstractImage_t* image,
                                   CGDIDeviceContext* deviceContext,
                                   const NW_GDI_Point2D_t* location,
                                   const NW_GDI_Dimension3D_t* size)
{
  return NW_Image_AbstractImage_DrawInRect (image, deviceContext, location, size, NULL);
}

/* ------------------------------------------------------------------------- 
 * Function:    _NW_Image_Epoc32Simple_DrawInRect
 * Description: Draw scaled image on a certain location within a rectangle.
 *
 * @param     image:          the simple image to be drawn
 * @param     deviceContext:  the device context object.
 * @param     location:       the top-left point to start drawing
 * @param     size:           the size to scale the image to.
 * @param     rect:           the rectangle to draw the image within
 *
 * note: Both the parameters size and rect could be NULL.
 *       If the size is NULL, the image is not scaled before being drawn.
 *       If the rect is NULL, draw the entire image.
*/
TBrowserStatusCode
_NW_Image_Epoc32Simple_DrawInRect (NW_Image_AbstractImage_t* image,
                                   CGDIDeviceContext* deviceContext,
                                   const NW_GDI_Point2D_t* location,
                                   const NW_GDI_Dimension3D_t* size,
                                   const NW_GDI_Rectangle_t* rect)
{
  NW_Image_Epoc32Simple_t* thisObj;
  NW_GDI_ImageBlock_t imageBlock;
  NW_GDI_Rectangle_t cropRect;
  NW_Bool imageScaled = NW_FALSE;
  NW_Bool imageOverlapped = NW_FALSE;
  NW_GDI_Rectangle_t overlap;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Epoc32Simple_Class));
  NW_ASSERT (location != NULL);

  /* for convenience */
  thisObj = NW_Image_Epoc32SimpleOf (image);
  CEpoc32ImageDecoder* decoder = STATIC_CAST(CEpoc32ImageDecoder*, thisObj->decoder);

  /* If there is a decoder, then the image needs to be decoded before displaying
   * if not done already, or if "needsDecode" (animated image has moved on to
   * next frame).
   */
  if (decoder != NULL && (thisObj->bitmap == NULL || thisObj->needsDecode))
    {
    decoder->Decode();
    }

  /* If there is already a bitmap, draw it. When the results of the decode operation
   * come back this function will be called again; then there should be a bitmap.
   */
  if( thisObj->bitmap )
    {
    imageBlock.colPalette.data = NULL;
    imageBlock.conversionStatus = NW_GDI_Image_ConversionRequired;
    imageBlock.size = thisObj->size;
    imageBlock.data = thisObj->bitmap;
    imageBlock.mask = thisObj->mask;
    imageBlock.invertedMask = thisObj->invertedMask;

    /* check if scaling required */
    if( size )
      {
      // if the image is scaled for some reason, then we have to make sure
      // that we are having the right size of it.
      CFbsBitmap* realBitmap = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)thisObj->bitmap);
      TSize bitmapRealSize = realBitmap->SizeInPixels();

      if( thisObj->size.width != size->width || thisObj->size.height != size->height || 
          bitmapRealSize.iWidth != size->width || bitmapRealSize.iHeight != size->height )
        {
        NW_GDI_ImageBlock_t scaledImageBlock;
    
        // scale current data 
        status = deviceContext->ScaleImage ( size, &imageBlock, &scaledImageBlock);

        if (status == KBrsrSuccess)
          {
          imageScaled = NW_TRUE;
          imageBlock.data = scaledImageBlock.data;
          imageBlock.mask = scaledImageBlock.mask;
          imageBlock.size = *size;
          }
        } 
      }
    
    /* if the parameter rect is NULL, we want to draw the entire image,
    else we only want to draw the overlap of image and the showing rectangle */
    if (rect == NULL)
    {
      cropRect.point.x = 0;
      cropRect.point.y = 0;
      cropRect.dimension.width = imageBlock.size.width;
      cropRect.dimension.height = imageBlock.size.height;
    }
    else
    {
      /* now based on the overlap of imageblock and showing rectangle,
      calculate the crop rectangle which is used to crop the image, the 
      difference between the overlap and crop image is: crop image's 
      coordinates are relative to image not absolute.
      example       (as x, y, width, height)
      rect            (10, 10, 50, 50)
      image           (30, 30, 40, 40)
      overlap area    (30, 30, 30, 30)
      crop rectangle  ( 0,  0, 30, 30)
      */
      imageOverlapped = TRUE;
      overlap.point.x = (location->x > rect->point.x) ? location->x : rect->point.x;
      overlap.point.y = (location->y > rect->point.y) ? location->y : rect->point.y;
      
      if (location->x + imageBlock.size.width > rect->point.x + rect->dimension.width) 
        overlap.dimension.width = rect->point.x + rect->dimension.width - overlap.point.x;
      else
        overlap.dimension.width = location->x + imageBlock.size.width - overlap.point.x;
      
      if (location->y + imageBlock.size.height > rect->point.y + rect->dimension.height) 
        overlap.dimension.height = rect->point.y + rect->dimension.height - overlap.point.y;
      else
        overlap.dimension.height = location->y + imageBlock.size.height - overlap.point.y;
      
      cropRect.point.x = overlap.point.x - location->x;
      cropRect.point.y = overlap.point.y - location->y;
      cropRect.dimension = overlap.dimension;

      /* if the rectangle and the image location does not overlap, return*/
      if (overlap.dimension.height <= 0 || overlap.dimension.width <= 0)
        return KBrsrSuccess;

        /* if the rectangle and the image location does not overlap, return
        if (rect->point.x > location->x + imageBlock.size.width ||
        rect->point.y > location->y + imageBlock.size.height ||
        location->x > rect->point.x + rect->dimension.width ||
        location->y > rect->point.y + rect->dimension.height)
        return KBrsrSuccess;
        
          if (rect->point.x < location->x)
          {
          cropRect.point.x = 0;
          cropRect.dimension.width = rect->point.x + rect->dimension.width - location->x ;
          }
          else
          {
          cropRect.point.x = rect->point.x - location->x;
          cropRect.dimension.width = location->x + imageBlock.size.width - rect->point.x;
          }
          if (rect->point.y < location->y)
          {
          cropRect.point.y = 0;
          cropRect.dimension.height = rect->point.y + rect->dimension.height - location->y;
          }
          else
          {
          cropRect.point.y = rect->point.y - location->y;
          cropRect.dimension.width = location->y + imageBlock.size.height - rect->->point.y;
          }
          }
      */
    /* do the draw */

    }

	if (imageOverlapped)
		deviceContext->DrawImage (&overlap.point, 
                              &cropRect,
                              &imageBlock);
	else
		deviceContext->DrawImage (location, 
                              &cropRect,
                              &imageBlock);
    if (imageScaled)
    {
        CFbsBitmap* bitmap = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)imageBlock.data);
        delete bitmap;
        CFbsBitmap* mask = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)imageBlock.mask);
        delete mask;
    }
  }
  return status;
}

/* ------------------------------------------------------------------------- */
/* _NW_Image_Epoc32Simple_IncrementImage
 *
 * Called on an animated image to cause it to move on to the next image.
 * Tells the decoder to go to the next image, and sets "needsDecode" to
 * NW_TRUE so that the decode will be done on the next attempt to draw.
 */
TBrowserStatusCode
_NW_Image_Epoc32Simple_IncrementImage(NW_Image_AbstractImage_t* image)
{
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Epoc32Simple_Class));

  NW_Bool   startingFrameSequenceOver=NW_FALSE;

  /* for convenience */
  NW_Image_Epoc32Simple_t* thisObj = NW_Image_Epoc32SimpleOf (image);

  CEpoc32ImageDecoder* decoder = STATIC_CAST(CEpoc32ImageDecoder*, thisObj->decoder);

  if (decoder)
    {
  if (decoder->IncrementImage(startingFrameSequenceOver))
  {
    /* update loop count if starting frame sequence again */
    if(startingFrameSequenceOver == NW_TRUE)
    {
      if(image->loopCount != -1)
      {
        image->loopCount--;
      }
    }
      }
    thisObj->needsDecode = NW_TRUE;
    return KBrsrSuccess;
  }
  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Image_Epoc32Simple_AddDocNode(NW_Image_Epoc32Simple_t* thisObj, 
                                 NW_HED_DocumentNode_t* documentNode)
{
  if (NW_ADT_DynamicVector_InsertAt(thisObj->docNodeList, &documentNode, NW_ADT_Vector_AtEnd) == NULL)
    return KBrsrOutOfMemory;
  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Image_Epoc32Simple_Initialize( NW_Image_Epoc32Simple_t* thisObj )
  {
  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Epoc32Simple_Class ) );
  // let the decoder know that we've got all the raw data
  if( thisObj->decoder )
    {
    CEpoc32ImageDecoder* decoder = STATIC_CAST( CEpoc32ImageDecoder*, thisObj->decoder );
    decoder->ImageDataComplete();
    status = KBrsrSuccess;
    }

  return status;
  }

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Image_Epoc32Simple_t*
NW_Image_Epoc32Simple_New (NW_GDI_Dimension3D_t size, 
                           const void* data, 
                           const void* mask,
                           NW_Bool invertedMask,
                           NW_Bool isTransparent,
                           TImageType imageType,
                           void* rawData,
                           NW_Int32 length,
                           NW_HED_DocumentNode_t* documentNode)
{
  //lint -e{437} Passing struct to ellipsis
  return (NW_Image_Epoc32Simple_t*) NW_Object_New (&NW_Image_Epoc32Simple_Class, 
                                                   size,
                                                   data,
                                                   mask,
                                                   (NW_Uint32)invertedMask,
                                                   (NW_Uint32)isTransparent,
                                                   (NW_Uint32)imageType,
                                                   rawData,
                                                   length,
                                                   documentNode);
}


/* ------------------------------------------------------------------------- */
/* NW_Image_Epoc32Simple_SetSize
 *
 * Called when the image has been opened by the image decoder. Sets the
 * (unscaled) image size.
 */
TBrowserStatusCode
NW_Image_Epoc32Simple_SetSize (NW_Image_AbstractImage_t* image,
                               NW_GDI_Dimension3D_t* aSize)
{
  NW_Image_Epoc32Simple_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Epoc32Simple_Class));
  NW_ASSERT (aSize != NULL);

  /* for convenience */
  thisObj = NW_Image_Epoc32SimpleOf (image);

  // we have to inform observers about the size change.
  NW_GDI_Dimension3D_s oldSize = thisObj->size;
  // set the size
  thisObj->size = *aSize;
  // first check if the previous size was valid.
  if( oldSize.width != 0 && thisObj->size.height != 0 )
  {
    // check if the size has changed at all 
    if( oldSize.width != aSize->width || 
        oldSize.height != aSize->height )
    {
      NW_Image_AbstractImage_ImageSizeChanged( NW_Image_AbstractImageOf( thisObj ) );
    }
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_Epoc32Simple_ForceImageDecode( NW_Image_AbstractImage_t* epocImage )
  {
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Image_Epoc32Simple_t* thisObj;

  NW_ASSERT( epocImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( epocImage, &NW_Image_Epoc32Simple_Class ) );

  thisObj = NW_Image_Epoc32SimpleOf( epocImage );

  CEpoc32ImageDecoder* decoder = STATIC_CAST( CEpoc32ImageDecoder*, thisObj->decoder );
  status = decoder->Decode();
  
  return status;
}


/* ------------------------------------------------------------------------- */
/* NW_Image_Epoc32Simple_SetAnimation
 *
 * Calling this function implies that the image is animated. It sets the delay
 * time, and notes that the image is animated.
 */
TBrowserStatusCode
NW_Image_Epoc32Simple_SetAnimation(NW_Image_AbstractImage_t* image,
                                   NW_Bool isAnimated,
                                   NW_Uint32 delay )
{
  image->isAnimated = isAnimated;
  image->delay = delay;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
/* NW_Image_Epoc32Simple_SetBitmapL
 *
 * Called by the image decoder when it has finished processing. Sets the bitmap
 * data, then forces a redraw.
 *
 * Parameters:
 *  image:     this object
 *  bitmap:    new bitmap to display
 *  mask:      new bitmap mask
 *  frameNum:  Current frame number (if animated)
 *  frameInfo: Details about current frame
 *  frameIsReady: the frame is fully decoded
 */
static
void
NW_Image_Epoc32Simple_SetBitmapL(NW_Image_Epoc32Simple_t* image,
                                CFbsBitmap* bitmap,
                                CFbsBitmap* mask,
                                NW_Int32 frameNum,
                                TFrameInfo* frameInfo,
                                NW_Bool frameIsReady )
  {
  if( frameIsReady == NW_TRUE )
    {
    // if the image has just been decoded, then image->bitmap is same as bitmap
    // init image->bitmap to NULL.
    if( image->imageDecoding == NW_TRUE )
      {
      image->bitmap = NULL;
      image->mask = NULL;
      image->imageDecoding = NW_FALSE;
      }
    CFbsBitmap* existingBitmap = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)image->bitmap);
    CFbsBitmap* existingMask = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)image->mask);

    // If there is transparency but no disposal property defined, then assume
    // the disposal property ELeaveInPlace.
    if( (frameInfo->iFlags & TFrameInfo::ETransparencyPossible) &&
      !(image->prevFlags & (TFrameInfo::ERestoreToBackground | TFrameInfo::ERestoreToPrevious)) )
      {
      image->prevFlags |= TFrameInfo::ELeaveInPlace;
      }

    // If there is an existing bitmap, then it must be animated. If the previous
    // frame had the flag "ELeaveInPlace", then the new bitmap must be rendered
    // on top of the old one for things to work properly. This does not apply to
    // the first frame when the animation goes back to the start (frameNum == 0).
    TPoint aStartPoint(0,0);
	if (existingBitmap != NULL && (image->prevFlags & TFrameInfo::ELeaveInPlace) && 
	   (frameNum > 0 || frameInfo->iFrameCoordsInPixels.iTl != aStartPoint)){
      // Push mask & bitmap on the cleanup stack to make sure they get deleted.
      CleanupStack::PushL(mask);
      CleanupStack::PushL(bitmap);

      // Render new bitmap on top of old bitmap.
      CFbsBitGc* bitmapContext=NULL;
      CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(existingBitmap);
      CleanupStack::PushL(bitmapDevice);
      User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
      CleanupStack::PushL(bitmapContext);
      if(frameNum == 0)
		 bitmapContext->Clear();
      if (mask == NULL)
        bitmapContext->BitBlt(frameInfo->iFrameCoordsInPixels.iTl, bitmap, frameInfo->iFrameCoordsInPixels.Size());
      else
        bitmapContext->BitBltMasked(frameInfo->iFrameCoordsInPixels.iTl, bitmap, frameInfo->iFrameCoordsInPixels.Size(), mask, EFalse);

      // Delete device, context & bitmap
      CleanupStack::PopAndDestroy(3);
		
      // If both masks exist, draw the new on top of the old.
      if (existingMask != NULL && mask != NULL)
        {
        CFbsBitGc* maskContext=NULL;
        CFbsBitmapDevice* maskDevice = CFbsBitmapDevice::NewL(existingMask);
        CleanupStack::PushL(maskDevice);
        User::LeaveIfError(maskDevice->CreateContext(maskContext));
        CleanupStack::PushL(maskContext);
        maskContext->BitBltMasked(frameInfo->iFrameCoordsInPixels.iTl, mask, frameInfo->iFrameCoordsInPixels.Size(), mask, EFalse);
        CleanupStack::PopAndDestroy(2);
        delete mask;
        }
      else if (mask != NULL)
        {
        // If the first image had no mask, then after that we would have to fill in the whole frame
        // anyway, so there is no point keeping a mask.
        delete mask;
        }
      // Pop mask from cleanup stack
      CleanupStack::Pop();
      }
    else
    {
        // The new bitmap and mask have no dependancy on the old; just replace the old
        // with the new.
        if(image->bitmap){
          TInt bitmapHandle = bitmap->Handle();
          CFbsBitmap* pBitmap = (CFbsBitmap*)image->bitmap;
          pBitmap->Duplicate(bitmapHandle);
        }
        else{
            image->bitmap = bitmap;
        } 
        
        if(image->mask){
           TInt bitmapHandle = mask->Handle();
           CFbsBitmap* pBitmask = (CFbsBitmap*)image->mask;
           pBitmask->Duplicate(bitmapHandle);
        }
        else{ 
            image->mask = mask;
        }
    }
    image->needsDecode = NW_FALSE;
    image->prevFlags = frameInfo->iFlags;
    }
  else
    {
    image->imageDecoding = NW_TRUE;
    image->bitmap = bitmap;
    image->mask = mask;
    }
  }

/* ------------------------------------------------------------------------- */
/* NW_Image_Epoc32Simple_DestroyRawData
 *
*/
void NW_Image_Epoc32Simple_DestroyRawData( NW_Image_Epoc32Simple_t* aImage )
  {
  NW_ASSERT( aImage != NULL );

  if( aImage->rawData )
    {
    delete aImage->rawData ;
    aImage->rawData = NULL;
    }
  }

/* ------------------------------------------------------------------------- */
/* NW_Image_Epoc32Simple_SetBitmap
 *
 * Non-leaving version of function "NW_Image_Epoc32Simple_SetBitmap". Called by
 * the image decoder when it has finished processing. Sets the bitmap data,
 * then forces a redraw. Parameters are "void*" so that OOC files can include
 * header file.
 *
 * Parameters:
 *  image:     this object
 *  bitmap:    new bitmap to display
 *  mask:      new bitmap mask
 *  frameNum:  Current frame number (if animated)
 *  frameInfo: Details about current frame
 */
void
NW_Image_Epoc32Simple_SetBitmap(NW_Image_Epoc32Simple_t* image,
                                void* bitmap,
                                void* mask,
                                NW_Int32 frameNum,
                                void* frameInfo,
                                NW_Bool frameIsReady )
{
  TInt err;
  TRAP(err, NW_Image_Epoc32Simple_SetBitmapL(image, 
                                              (CFbsBitmap*)bitmap,
                                              (CFbsBitmap*)mask,
                                              frameNum,
                                              (TFrameInfo*)frameInfo, 
                                              frameIsReady));
}

// first we check if image is visible, we go through the observer list for that.
// Observer knows which image is visible. If image is not visible, then we
// delete the image which is out of view. In case that the decoding is still going on 
// we cancel the decoding for out of view image
void
NW_Image_Epoc32Simple_OutOfView( NW_Image_Epoc32Simple_t* aEpocImage )
    {
      NW_Bool isVisible = NW_Image_AbstractImage_IsVisible(NW_Image_AbstractImageOf(aEpocImage));
      if(( aEpocImage->imageDecoding == NW_FALSE ) && (aEpocImage->bitmap) && (!isVisible))
        {
        // delet the image bitmap
        CFbsBitmap* oldBitmap = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)aEpocImage->bitmap);
        delete oldBitmap;
        aEpocImage->bitmap = NULL;
        CFbsBitmap* oldMask = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)aEpocImage->mask);
        delete oldMask;
        aEpocImage->mask = NULL;
        }
      else if ( aEpocImage->imageDecoding )
        {
        // cancel decoding
        CEpoc32ImageDecoder* decoder = STATIC_CAST(CEpoc32ImageDecoder*, aEpocImage->decoder);
        decoder->Cancel();
        }
    }
