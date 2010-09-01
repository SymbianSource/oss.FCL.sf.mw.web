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


#include "nw_image_abstractimagei.h"
#include "nw_adt_resizablevector.h"
#include "nw_adt_vector.h"
#include "nw_image_iimageobserver.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Image_AbstractImage_Class_t NW_Image_AbstractImage_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Dynamic_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_Image_AbstractImage_t),
    /* construct                 */ _NW_Image_AbstractImage_Construct,
    /* destruct                  */ _NW_Image_AbstractImage_Destruct
  },
  { /* NW_Image_Abstract         */
    /* getSize                   */ NULL,
    /* draw                      */ NULL,
    /* drawScaled                */ NULL,
    /* drawInRect                */ NULL,
    /* incrementImage            */ NULL,
    /* getLoopCount              */ _NW_Image_AbstractImage_GetLoopCount,
    /* setLoopCount              */ _NW_Image_AbstractImage_SetLoopCount,
    /* getDelay                  */ _NW_Image_AbstractImage_GetDelay,
    /* isAnimated                */ _NW_Image_AbstractImage_IsAnimated,
    /* setImageObserver          */ _NW_Image_AbstractImage_SetImageObserver,
    /* removeImageObserver       */ _NW_Image_AbstractImage_RemoveImageObserver,
    /* imageOpened               */ _NW_Image_AbstractImage_ImageOpened,
    /* imageSizeChanged          */ _NW_Image_AbstractImage_ImageSizeChanged,
    /* imageDecoded              */ _NW_Image_AbstractImage_ImageDecoded,
    /* imageOpenStarted          */ _NW_Image_AbstractImage_ImageOpenStarted,
    /* forceImageDecode          */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_AbstractImage_Construct(NW_Object_Dynamic_t* dynamicObject,
                          va_list *argp)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Image_AbstractImage_t* thisObj;

  NW_REQUIRED_PARAM( argp );
  /* parameter assertion block */
  NW_ASSERT( NW_Object_IsInstanceOf( dynamicObject, &NW_Image_AbstractImage_Class ) );
  thisObj = NW_Image_AbstractImageOf( dynamicObject );

  thisObj->imageObserverList = (NW_ADT_DynamicVector_t*)NW_ADT_ResizableVector_New( sizeof( NW_ADT_DynamicVector_t* ), 10, 5 );
  if( thisObj->imageObserverList == NULL )
    {
    status = KBrsrOutOfMemory;
    }
  thisObj->isOpening = NW_FALSE;

  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_Image_AbstractImage_Destruct( NW_Object_Dynamic_t* dynamicObject )
  {
  NW_Image_AbstractImage_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT( NW_Object_IsInstanceOf( dynamicObject, &NW_Image_AbstractImage_Class ) );

  /* for convenience */
  thisObj = NW_Image_AbstractImageOf( dynamicObject );

  if( thisObj->imageObserverList )
    {
    // and delete it
    NW_Object_Delete( thisObj->imageObserverList );
    }
  }


/* ------------------------------------------------------------------------- */
NW_Int16
_NW_Image_AbstractImage_GetLoopCount(NW_Image_AbstractImage_t* image)
{
  return image->loopCount;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_AbstractImage_SetLoopCount(NW_Image_AbstractImage_t* image, NW_Uint16 loopCount)
{
  image->loopCount = loopCount;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_Image_AbstractImage_GetDelay(NW_Image_AbstractImage_t* image)
{

  return image->delay;

}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_Image_AbstractImage_IsAnimated(NW_Image_AbstractImage_t* image)
{
  return image->isAnimated;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_AbstractImage_IncrementImage(NW_Image_AbstractImage_t* image)
{
  NW_REQUIRED_PARAM(image);
  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_AbstractImage_SetImageObserver( NW_Image_AbstractImage_t* abstractImage,
                                          NW_Image_IImageObserver_t* observer )
{
  NW_Image_AbstractImage_t* thisObj;

  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( observer != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_AbstractImageOf( abstractImage );

  if( NW_ADT_DynamicVector_InsertAt( thisObj->imageObserverList, &observer,
          NW_ADT_Vector_AtEnd ) == NULL )
    {
    return KBrsrOutOfMemory;
    }
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_AbstractImage_RemoveImageObserver( NW_Image_AbstractImage_t* abstractImage,
                                             NW_Image_IImageObserver_t* observer )
  {
  NW_Image_AbstractImage_t* thisObj;
  TBrowserStatusCode status = KBrsrFailure;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( observer != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_AbstractImageOf( abstractImage );

  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
  // lookup the observer list and remove if one matches
  for( index = 0; index < numItems; index++ )
  {
    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
      NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );
    NW_ASSERT( imageObserver );

    if( imageObserver == observer )
      {
      status = NW_ADT_DynamicVector_RemoveAt( thisObj->imageObserverList, index );
      break;
      }
    }
  return status;
  }

/* ------------------------------------------------------------------------- */
void
_NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImage_t* abstractImage, NW_Bool failed,
                                     NW_Int16 suppressDupReformats )
  {
  NW_Image_AbstractImage_t* thisObj;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t newNumItems;
  NW_ADT_Vector_Metric_t index;

  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_AbstractImageOf( abstractImage );

  thisObj->isOpening = NW_FALSE;

  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
  // inform all the observers about the image open event.
  // It usually involves image replacement ( from broken image to valid image)
  // It can also involve relayout event. see the actual implementation of it.
  for( index = 0; index < numItems; index++ )
  {
    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
      NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );
    NW_ASSERT( imageObserver );

    if( imageObserver )
      {
      // 
      // Given the comment above, suppressDupReformats is set to start, continue, or stop.
      // During the first interation of this loop it "starts" suppressing reformats 
      // until the last interation.  Image observers in the same format-box are only 
      // reformated once.  These reformats are triggered when the current image 
      // observer is in a different flow-box or it reaches the end of the loop.
      //
      // This helps speed up the special case when an image is duplicated many times
      // within the same document.
      //
      // See the comment about "iSuppressDupReformats" in NW_MVC_View_ReformatBox 
      // for more information.
      //
      if (numItems > 1)
        {
        if (index == 0)
          {
          suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Start;
          }
        else if (index == (numItems - 1))
          {
          suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Stop;
          }
        else
          {
          suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Continue;
          }
        }

      NW_Image_IImageObserver_ImageOpened( imageObserver, thisObj, failed, 
          suppressDupReformats );

      // refresh numItems as at CSS level, imageOpened() can end up deleteing some
      // items from this list.
      newNumItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
      if( numItems != newNumItems )
        { 
        // check if the item was deleted behind the index since if it was not -so the deleted 
        // item was ahead in the queue then we need to update the index.
        numItems = newNumItems;
        if( index < newNumItems )
          {
          NW_Image_IImageObserver_t* newImageObserver = *(NW_Image_IImageObserver_t**)
          NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );

          if( imageObserver != newImageObserver )
            {
            index--;
            }
          }
        }
      }
    }
  }

/* ------------------------------------------------------------------------- */
void
_NW_Image_AbstractImage_ImageSizeChanged( NW_Image_AbstractImage_t* abstractImage )
  {
  NW_Image_AbstractImage_t* thisObj;
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t numItems;

  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_AbstractImageOf( abstractImage );

  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
  // inform all the observers about the size chage event.
  // It involves relayout event on the entire tree
  for( index = 0; index < numItems; index++ )
  {
    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
      NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );
    NW_ASSERT( imageObserver );

    if( imageObserver )
      {
      NW_Image_IImageObserver_SizeChanged( imageObserver, thisObj );
      // refresh numItems as at CSS level, imageOpened() can end up deleteing some
      // items from this list.
      // numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
      }
    }
  }

/* ------------------------------------------------------------------------- */
void
_NW_Image_AbstractImage_ImageOpenStarted( NW_Image_AbstractImage_t* abstractImage )
  {
  NW_Image_AbstractImage_t* thisObj;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_AbstractImageOf( abstractImage );
  
  thisObj->isOpening = NW_TRUE;

  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
  // inform all the observers about the image decode event
  for( index = 0; index < numItems; index++ )
  {
    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
      NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );
    NW_ASSERT( imageObserver );

    if( imageObserver )
      {
      NW_Image_IImageObserver_ImageOpeningStarted( imageObserver, NW_Image_AbstractImageOf( thisObj ) );
      }
    }
  }

/* ------------------------------------------------------------------------- */
void
_NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImage_t* abstractImage, 
                                      NW_Bool failed )
  {
  NW_Image_AbstractImage_t* thisObj;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( abstractImage, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_AbstractImageOf( abstractImage );

  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
  // inform all the observers about the image decode event
  // it involves refresh event on the image box
  for( index = 0; index < numItems; index++ )
  {
    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
      NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );
    NW_ASSERT( imageObserver );

    if( imageObserver )
      {
      NW_Image_IImageObserver_ImageDecoded( imageObserver, thisObj, failed );
      }
    }
}

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Image_AbstractImage_DecrementLoopCount(NW_Image_AbstractImage_t* image)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Int16 loopCount;

  loopCount = NW_Image_AbstractImage_GetLoopCount(image);
  if (loopCount > 0) {
    loopCount--;
    status = NW_Image_AbstractImage_SetLoopCount(image, loopCount);
  }
  return status;
}

/* ------------------------------------------------------------------------- */
void
NW_Image_AbstractImage_ImageDestroyed( NW_Image_AbstractImage_t* image )
  {
  NW_Image_AbstractImage_t* thisObj;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  NW_ASSERT( image != NULL );

  thisObj = NW_Image_AbstractImageOf( image );

  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( thisObj->imageObserverList ) );
  // forward destroy event to the observers
  for( index = 0; index < numItems; index++ )
    {
    // 
    // Given the comment above, suppressDupReformats is set to start, continue, or stop.
    // During the first interation of this loop it "starts" suppressing reformats 
    // until the last interation.  Image observers in the same format-box are only 
    // reformated once.  These reformats are triggered when the current image 
    // observer is in a different flow-box or it reaches the end of the loop.
    //
    // This helps speed up the special case when an image is duplicated many times
    // within the same document.
    //
    // See the comment about "iSuppressDupReformats" in NW_MVC_View_ReformatBox 
    // for more information.
    //
    NW_Int16 suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Default;

    if (numItems > 1)
      {
      if (index == 0)
        {
        suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Continue;
        }
      else if (index == (numItems - 1))
        {
        suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Stop;
        }
      else
        {
        suppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Continue;
        }
      }

    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
        NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( thisObj->imageObserverList ), index );

    if( imageObserver )
      {
      if( thisObj->isOpening == NW_TRUE )
        {
        NW_Image_IImageObserver_ImageOpened( imageObserver, thisObj, NW_TRUE,
            suppressDupReformats );
        }
      NW_Image_IImageObserver_ImageDestroyed( imageObserver, NW_Image_AbstractImageOf( thisObj ) );
      }
    }
  }

// Check from all the observers if the image is visible
/* ------------------------------------------------------------------------- */
NW_Bool
NW_Image_AbstractImage_IsVisible(NW_Image_AbstractImage_t* aImage)
    {
    NW_ADT_Vector_Metric_t numItems;
    NW_ADT_Vector_Metric_t index;
    NW_Bool isVisible = NW_FALSE;

    numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( aImage->imageObserverList ) );
    for( index = 0; index < numItems; index++ )
        {
        NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
            NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( aImage->imageObserverList ), index );

        if( imageObserver )
            {
            isVisible = NW_Image_IImageObserver_IsVisible(imageObserver);
            }
        if (isVisible)
            {
            break;
            }
        }
    return isVisible;
    }
