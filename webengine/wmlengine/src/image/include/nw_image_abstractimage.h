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


#ifndef NW_IMAGE_ABSTRACTIMAGE_H
#define NW_IMAGE_ABSTRACTIMAGE_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "nw_gdi_types.h"
#include "BrsrTypes.h"
#include "nw_image_imageObserver.h"

#include "NW_Image_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   constants
 * ------------------------------------------------------------------------- */
#define NW_Image_AbstractImage_SuppressDupReformats_Start 0
#define NW_Image_AbstractImage_SuppressDupReformats_Stop 1
#define NW_Image_AbstractImage_SuppressDupReformats_Continue 2
#define NW_Image_AbstractImage_SuppressDupReformats_Default 3

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_AbstractImage_Class_s NW_Image_AbstractImage_Class_t;
typedef struct NW_Image_AbstractImage_s NW_Image_AbstractImage_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_GetSize_t) (const NW_Image_AbstractImage_t* image,
                                     NW_GDI_Dimension3D_t* size);

typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_Draw_t) ( NW_Image_AbstractImage_t* image,
                                  CGDIDeviceContext* deviceContext,
                                 const NW_GDI_Point2D_t* location);
typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_DrawScaled_t) (NW_Image_AbstractImage_t* image,
                                        CGDIDeviceContext* deviceContext,
                                        const NW_GDI_Point2D_t* location,
                                        const NW_GDI_Dimension3D_t* size);

typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_DrawInRect_t) (NW_Image_AbstractImage_t* image,
                                        CGDIDeviceContext* deviceContext,
                                        const NW_GDI_Point2D_t* location,
                                        const NW_GDI_Dimension3D_t* size,
                                        const NW_GDI_Rectangle_t* rect);

typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_IncrementImage_t) (NW_Image_AbstractImage_t* image);

typedef
NW_Int16
(*NW_Image_AbstractImage_GetLoopCount_t) (NW_Image_AbstractImage_t* image); 

typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_SetLoopCount_t) (NW_Image_AbstractImage_t* image, NW_Uint16 loopCount);

typedef
NW_Uint32
(*NW_Image_AbstractImage_GetDelay_t) (NW_Image_AbstractImage_t* image);

typedef
NW_Bool
(*NW_Image_AbstractImage_IsAnimated_t) (NW_Image_AbstractImage_t* image); 

typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_SetImageObserver_t) ( NW_Image_AbstractImage_t* image,
                                               NW_Image_IImageObserver_t* observer);
typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_RemoveImageObserver_t) ( NW_Image_AbstractImage_t* image,
                                               NW_Image_IImageObserver_t* observer );

typedef
void
(*NW_Image_AbstractImage_ImageOpened_t) ( NW_Image_AbstractImage_t* image, NW_Bool failed,
                                          NW_Int16  suppressDupReformats);

typedef
void
(*NW_Image_AbstractImage_ImageSizeChanged_t) ( NW_Image_AbstractImage_t* image );

typedef
void
(*NW_Image_AbstractImage_ImageDecoded_t) ( NW_Image_AbstractImage_t* image,
                                          NW_Bool failed);

typedef
void
(*NW_Image_AbstractImage_ImageOpenStarted_t) ( NW_Image_AbstractImage_t* image );

typedef
TBrowserStatusCode
(*NW_Image_AbstractImage_ForceImageDecode_t) ( NW_Image_AbstractImage_t* image );



/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_AbstractImage_ClassPart_s {
  NW_Image_AbstractImage_GetSize_t getSize;
  NW_Image_AbstractImage_Draw_t draw;
  NW_Image_AbstractImage_DrawScaled_t drawScaled;
  NW_Image_AbstractImage_DrawInRect_t drawInRect;
  NW_Image_AbstractImage_IncrementImage_t incrementImage;
  NW_Image_AbstractImage_GetLoopCount_t getLoopCount;
  NW_Image_AbstractImage_SetLoopCount_t setLoopCount;
  NW_Image_AbstractImage_GetDelay_t getDelay;
  NW_Image_AbstractImage_IsAnimated_t isAnimated;
  NW_Image_AbstractImage_SetImageObserver_t setImageObserver;
  NW_Image_AbstractImage_RemoveImageObserver_t removeImageObserver;
  NW_Image_AbstractImage_ImageOpened_t imageOpened;
  NW_Image_AbstractImage_ImageSizeChanged_t imageSizeChanged;
  NW_Image_AbstractImage_ImageDecoded_t imageDecoded;
  NW_Image_AbstractImage_ImageOpenStarted_t imageOpenStarted;
  NW_Image_AbstractImage_ForceImageDecode_t forceImageDecode;
} NW_Image_AbstractImage_ClassPart_t;

struct NW_Image_AbstractImage_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Image_AbstractImage_ClassPart_t NW_Image_AbstractImage;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Image_AbstractImage_s {
  NW_Object_Dynamic_t super;

  /*member variables*/
  /*for animated images*/
  NW_Bool isAnimated; 
  NW_Uint32 delay;
  NW_Int16 loopCount;
  NW_ADT_DynamicVector_t* imageObserverList; /* image observer list */
  NW_Bool isOpening; 
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_AbstractImage_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Image_AbstractImage))

#define NW_Image_AbstractImageOf(_object) \
  (NW_Object_Cast ((_object), NW_Image_AbstractImage))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT const NW_Image_AbstractImage_Class_t NW_Image_AbstractImage_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Image_AbstractImage_GetSize(_image, _size) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, getSize) \
    ((_image), (_size)))

#define NW_Image_AbstractImage_Draw(_image, _deviceContext, _location) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, draw) \
    ((_image), (_deviceContext), (_location)))

#define NW_Image_AbstractImage_DrawScaled(_image, _deviceContext, _location, _size) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, drawScaled) \
    ((_image), (_deviceContext), (_location), (_size)))

#define NW_Image_AbstractImage_DrawInRect(_image, _deviceContext, _location, _size, _rect) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, drawInRect) \
    ((_image), (_deviceContext), (_location), (_size), (_rect)))

#define NW_Image_AbstractImage_IncrementImage(_image) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, incrementImage) \
    ((_image)))

#define NW_Image_AbstractImage_GetLoopCount(_image) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, getLoopCount) \
    ((_image)))

#define NW_Image_AbstractImage_SetLoopCount(_image, _loopCount) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, setLoopCount) \
    ((_image), (_loopCount)))

#define NW_Image_AbstractImage_GetDelay(_image) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, getDelay) \
    ((_image)))

#define NW_Image_AbstractImage_IsAnimated(_image) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, isAnimated) \
    ((_image)))

#define NW_Image_AbstractImage_SetImageObserver( _image, _observer ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, setImageObserver) \
    ( ( _image ), (_observer ) ) )

#define NW_Image_AbstractImage_RemoveImageObserver( _image, _observer ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, removeImageObserver) \
    ( ( _image ), (_observer ) ) )

#define NW_Image_AbstractImage_ImageOpened( _image, _failed, _suppressDupReformats ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, imageOpened) \
    ( ( _image ), ( _failed ), (_suppressDupReformats) ) )

#define NW_Image_AbstractImage_ImageSizeChanged( _image ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, imageSizeChanged) \
    ( ( _image ) ) )

#define NW_Image_AbstractImage_ImageDecoded( _image, _failed ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, imageDecoded) \
    ( ( _image ), ( _failed ) ) )

#define NW_Image_AbstractImage_ImageOpenStarted( _image ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, imageOpenStarted) \
    ( ( _image ) ) )

#define NW_Image_AbstractImage_ForceImageDecode( _image ) \
  (NW_Object_Invoke ((_image), NW_Image_AbstractImage, forceImageDecode) \
    ( ( _image ) ) )

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT
TBrowserStatusCode
NW_Image_AbstractImage_DecrementLoopCount(NW_Image_AbstractImage_t* image);

NW_IMAGE_EXPORT
void
NW_Image_AbstractImage_ImageDestroyed( NW_Image_AbstractImage_t* image );

NW_Bool
NW_Image_AbstractImage_IsVisible(NW_Image_AbstractImage_t* aImage);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_ABSTRACTIMAGE_H */
