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


#include "nw_image_virtualimagei.h"
#include "BrsrStatusCodes.h"
#include "nw_image_virtualimage.h"
#include "nw_object_interface.h"
#include "GDIDevicecontext.h"

NW_Bool
_NW_Image_VirtualImage_IImageObserver_IsVisible(NW_Image_IImageObserver_t* aImageObserver)
    {
  NW_Image_Virtual_t* thisObj;

  // parameter assertion block
  NW_ASSERT( aImageObserver != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( aImageObserver,
                                    &_NW_Image_VirtualImage_ImageObserver_Class ) );
  // obtain the implementer
  thisObj = (NW_Image_Virtual_t*)NW_Object_Interface_GetImplementer( aImageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Virtual_Class ) );

  // forward this event to our observer
  return NW_Image_AbstractImage_IsVisible( NW_Image_AbstractImageOf( thisObj ));
    }

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Image_Virtual_Class_t NW_Image_Virtual_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Image_AbstractImage_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ _NW_Image_VirtualImage_InterfaceList
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Image_Virtual_t),
    /* construct               */ _NW_Image_VirtualImage_Construct,
    /* destruct                */ _NW_Image_VirtualImage_Destruct
  },
  { /* NW_Image_Abstract       */
    /* getSize                 */ _NW_Image_VirtualImage_GetSize,
    /* draw                    */ _NW_Image_VirtualImage_Draw,
    /* drawScaled              */ _NW_Image_VirtualImage_DrawScaled,
    /* drawInRect              */ _NW_Image_VirtualImage_DrawInRect,
    /* incrementImage          */ _NW_Image_VirtualImage_IncrementImage,
    /* getLoopCount            */ _NW_Image_VirtualImage_GetLoopCount,
    /* setLoopCount            */ _NW_Image_VirtualImage_SetLoopCount,
    /* getDelay                */ _NW_Image_VirtualImage_GetDelay,
    /* isAnimated              */ _NW_Image_VirtualImage_IsAnimated,
    /* setImageObserver        */ _NW_Image_VirtualImage_SetImageObserver,
    /* removeImageObserver     */ _NW_Image_VirtualImage_RemoveImageObserver,
    /* imageOpened             */ _NW_Image_AbstractImage_ImageOpened,
    /* imageSizeChanged        */ _NW_Image_AbstractImage_ImageSizeChanged,
    /* imageDecoded            */ _NW_Image_AbstractImage_ImageDecoded,
    /* imageOpenStarted        */ _NW_Image_AbstractImage_ImageOpenStarted,
    /* forceImageDecode        */ _NW_Image_VirtualImage_ForceImageDecode
  },
  { /* NW_Image_Virtual        */
    /* unused                  */ 0
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const _NW_Image_VirtualImage_InterfaceList[] = {
  &_NW_Image_VirtualImage_ImageObserver_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_Image_IImageObserver_Class_t _NW_Image_VirtualImage_ImageObserver_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_Image_IImageObserver_Class,
    /* queryInterface       */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface  */
    /* offset               */ offsetof( NW_Image_Virtual_t, NW_Image_IImageObserver )
  },
  { /* NW_Image_IImageObserver */
    /* imageOpened          */ _NW_Image_VirtualImage_IImageObserver_ImageOpened,
    /* sizeChanged          */ _NW_Image_VirtualImage_IImageObserver_SizeChanged,
    /* imageOpeningStarted  */ _NW_Image_VirtualImage_IImageObserver_ImageOpeningStarted,
    /* imageDecoded         */ _NW_Image_VirtualImage_IImageObserver_ImageDecoded,
    /* imageDestroyed       */ _NW_Image_VirtualImage_IImageObserver_ImageDestroyed,
    /* isVisible            */ _NW_Image_VirtualImage_IImageObserver_IsVisible
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- *
   NW_Image_VirtualImage_ImageObserver implementation
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_Image_VirtualImage_IImageObserver_ImageOpened( NW_Image_IImageObserver_t* imageObserver,
                                                   NW_Image_AbstractImage_t* abstractImage,
                                                   NW_Bool failed,
                                                   NW_Int16 suppressDupReformats )
  {
  NW_Image_Virtual_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );

  // parameter assertion block
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_Image_VirtualImage_ImageObserver_Class ) );
  // obtain the implementer
  thisObj = (NW_Image_Virtual_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Virtual_Class ) );

  // forward this event to our observer
  NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImageOf( thisObj ), failed,
                                      suppressDupReformats );
  }


/* ------------------------------------------------------------------------- */
void
_NW_Image_VirtualImage_IImageObserver_SizeChanged( NW_Image_IImageObserver_t* imageObserver,
                                                   NW_Image_AbstractImage_t* abstractImage )
  {
  NW_Image_Virtual_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );

  // parameter assertion block
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_Image_VirtualImage_ImageObserver_Class ) );
  // obtain the implementer
  thisObj = (NW_Image_Virtual_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Virtual_Class ) );

  // forward this event to our observer
  NW_Image_AbstractImage_ImageSizeChanged( NW_Image_AbstractImageOf( thisObj ) );
  }


/* ------------------------------------------------------------------------- */
void
_NW_Image_VirtualImage_IImageObserver_ImageOpeningStarted( NW_Image_IImageObserver_t* imageObserver,
                                                           NW_Image_AbstractImage_t* abstractImage )
  {
  NW_Image_Virtual_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );

  // parameter assertion block
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_Image_VirtualImage_ImageObserver_Class ) );
  // obtain the implementer
  thisObj = (NW_Image_Virtual_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Virtual_Class ) );

  // forward this event to our observer
  NW_Image_AbstractImage_ImageOpenStarted( NW_Image_AbstractImageOf( thisObj ) );
  }


/* ------------------------------------------------------------------------- */
void
_NW_Image_VirtualImage_IImageObserver_ImageDecoded( NW_Image_IImageObserver_t* imageObserver,
                                                    NW_Image_AbstractImage_t* abstractImage,
                                                    NW_Bool failed)
  {
  NW_Image_Virtual_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );

  // parameter assertion block
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_Image_VirtualImage_ImageObserver_Class ) );
  // obtain the implementer
  thisObj = (NW_Image_Virtual_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Virtual_Class ) );

  // forward this event to our observer
  NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImageOf( thisObj ), failed );
  }

/* ------------------------------------------------------------------------- */
void
_NW_Image_VirtualImage_IImageObserver_ImageDestroyed( NW_Image_IImageObserver_t* imageObserver,
                                                      NW_Image_AbstractImage_t* abstractImage )
  {
  NW_Image_Virtual_t* thisObj;

  NW_REQUIRED_PARAM( abstractImage );

  // parameter assertion block
  NW_ASSERT( imageObserver != NULL );
  NW_ASSERT( abstractImage != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( imageObserver,
                                    &_NW_Image_VirtualImage_ImageObserver_Class ) );
  // obtain the implementer
  thisObj = (NW_Image_Virtual_t*)NW_Object_Interface_GetImplementer( imageObserver );
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_Image_Virtual_Class ) );

  // dont forward image destroy event to our observer as destroy event is sent by
  // this->destructor
  thisObj->image = NULL;
  }


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list *argp)
{
  NW_Image_Virtual_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertion block */
  NW_ASSERT( dynamicObject != NULL );
  NW_ASSERT( argp != NULL );
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_Image_Virtual_Class));

  /* for convenience */
  thisObj = NW_Image_VirtualOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_Image_AbstractImage_Construct( dynamicObject, argp );

  if( status != KBrsrSuccess )
    {
    return status;
    }

  /* initialize the member variables */
  thisObj->image = va_arg (*argp, NW_Image_AbstractImage_t*);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->image, &NW_Image_AbstractImage_Class));

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_Image_VirtualImage_Destruct( NW_Object_Dynamic_t* dynamicObject )
{
  NW_Image_Virtual_t* thisObj;
  NW_Image_AbstractImage_t* superObject;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  /* parameter assertions */
  NW_ASSERT( dynamicObject != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( dynamicObject, &NW_Image_Virtual_Class ) );

  /* for convenience */
  thisObj = NW_Image_VirtualOf( dynamicObject );

  superObject = NW_Image_AbstractImageOf( dynamicObject );

  // first notify the observers on the destroy event
  NW_Image_AbstractImage_ImageDestroyed( NW_Image_AbstractImageOf( thisObj ) );

  // stop observing
  numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( superObject->imageObserverList ) );
  // lookup the observer list and remove them all
  for( index = 0; index < numItems; index++ )
    {
    NW_Image_IImageObserver_t* imageObserver = *(NW_Image_IImageObserver_t**)
      NW_ADT_Vector_ElementAt( NW_ADT_VectorOf( superObject->imageObserverList ), index );
    NW_ASSERT( imageObserver );

    if( imageObserver )
      {
      _NW_Image_VirtualImage_RemoveImageObserver( NW_Image_AbstractImageOf( thisObj ), imageObserver );
      }
    // updatet numItems as it might be changing
    numItems = NW_ADT_Vector_GetSize( NW_ADT_VectorOf( superObject->imageObserverList ) );
    }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_GetSize (const NW_Image_AbstractImage_t* image,
                           NW_GDI_Dimension3D_t* size)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Virtual_Class));
  NW_ASSERT (size != NULL);

  /* invoke the alias Image method */
  return NW_Image_AbstractImage_GetSize (NW_Image_VirtualOf (image)->image, size);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_Draw (NW_Image_AbstractImage_t* image,
                       CGDIDeviceContext* deviceContext,
                       const NW_GDI_Point2D_t* location)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Virtual_Class));
  NW_ASSERT (location != NULL);

  /* invoke the alias Image method */
  return NW_Image_AbstractImage_Draw (NW_Image_VirtualOf (image)->image, deviceContext,
                        location);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_DrawScaled(NW_Image_AbstractImage_t* image,
                                  CGDIDeviceContext* deviceContext,
                                  const NW_GDI_Point2D_t* location,
                                  const NW_GDI_Dimension3D_t* size)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Virtual_Class));
  NW_ASSERT (location != NULL);
  NW_ASSERT (size != NULL);

  /* invoke the alias Image method */
  return NW_Image_AbstractImage_DrawScaled (NW_Image_VirtualOf (image)->image, deviceContext,
                        location, size);
}

/* -------------------------------------------------------------------------
 * Function:    _NW_Image_VirtualImage_DrawInRect
 * Description: Draw scaled image on a certain location within a rectangle.
 *
 * @param     image:          the virtual image to be drawn
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
_NW_Image_VirtualImage_DrawInRect(NW_Image_AbstractImage_t* image,
                                  CGDIDeviceContext* deviceContext,
                                  const NW_GDI_Point2D_t* location,
                                  const NW_GDI_Dimension3D_t* size,
                                  const NW_GDI_Rectangle_t* rect)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Virtual_Class));
  NW_ASSERT (location != NULL);

  /* invoke the alias Image method */
  return NW_Image_AbstractImage_DrawInRect (NW_Image_VirtualOf (image)->image, deviceContext,
                        location, size, rect);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_IncrementImage(NW_Image_AbstractImage_t* image)
{
  return NW_Image_AbstractImage_IncrementImage(NW_Image_VirtualOf (image)->image);
}

/* ------------------------------------------------------------------------- */
NW_Int16
_NW_Image_VirtualImage_GetLoopCount(NW_Image_AbstractImage_t* image)
{
  return NW_Image_AbstractImage_GetLoopCount(NW_Image_VirtualOf (image)->image);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_SetLoopCount(NW_Image_AbstractImage_t* image, NW_Uint16 loopCount)
{
  return NW_Image_AbstractImage_SetLoopCount(NW_Image_VirtualOf (image)->image, loopCount);
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_Image_VirtualImage_GetDelay(NW_Image_AbstractImage_t* image)
{

  return NW_Image_AbstractImage_GetDelay(NW_Image_VirtualOf (image)->image);

}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_Image_VirtualImage_IsAnimated(NW_Image_AbstractImage_t* image)
{
  return NW_Image_AbstractImage_IsAnimated(NW_Image_VirtualOf (image)->image);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_SetImageObserver( NW_Image_AbstractImage_t* image,
                                         NW_Image_IImageObserver_t* observer )
{
  NW_Image_Virtual_t* thisObj;
  TBrowserStatusCode status = KBrsrFailure;

  // parameter assertion block
  NW_ASSERT( image != NULL );
  NW_ASSERT( observer != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( image, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_VirtualOf( image );

  // invoke superclass function
  _NW_Image_AbstractImage_SetImageObserver( NW_Image_AbstractImageOf( image ), observer );

  // since virtual image just points to a "real" image, we
  // need to observ that "real" image and forward each and every event which
  // happens to that image back to our observer as it had happened to this
  // virtual image
  if( thisObj->image != NULL )
    {
    status = NW_Image_AbstractImage_SetImageObserver(
        NW_Image_AbstractImageOf( thisObj->image ),
        &thisObj->NW_Image_IImageObserver );
    }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_RemoveImageObserver( NW_Image_AbstractImage_t* image,
                                            NW_Image_IImageObserver_t* observer )
{
  NW_Image_Virtual_t* thisObj;
  TBrowserStatusCode status = KBrsrFailure;

  // parameter assertion block
  NW_ASSERT( image != NULL );
  NW_ASSERT( observer != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( image, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_VirtualOf( image );

  // virtual image points to one "real" image, we need to
  // remove observer from that image too
  if( thisObj->image != NULL  )
    {
    status = NW_Image_AbstractImage_RemoveImageObserver(
        NW_Image_AbstractImageOf( thisObj->image ),
        &thisObj->NW_Image_IImageObserver );
    }
  // invoke superclass function
  status = _NW_Image_AbstractImage_RemoveImageObserver( NW_Image_AbstractImageOf( image ), observer );
  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Image_VirtualImage_ForceImageDecode( NW_Image_AbstractImage_t* image )
{
  NW_Image_Virtual_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  // parameter assertion block
  NW_ASSERT( image != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( image, &NW_Image_AbstractImage_Class ) );

  thisObj = NW_Image_VirtualOf( image );

  // force the "real" image to decode
  if( thisObj->image != NULL )
    {
    status = NW_Image_AbstractImage_ForceImageDecode( thisObj->image );
    }

  return status;
}
/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Image_Virtual_t*
NW_Image_Virtual_New (NW_Image_AbstractImage_t* image)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_AbstractImage_Class));

  /* instantiate and return the VirtualImage */
  return (NW_Image_Virtual_t*) NW_Object_New (&NW_Image_Virtual_Class, image);
}
