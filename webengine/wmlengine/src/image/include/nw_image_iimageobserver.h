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


#ifndef NW_IMAGE_IIMAGEOBSERVER_H
#define NW_IMAGE_IIMAGEOBSERVER_H



#include "NW_LMgr_EXPORT.h"
#include "nw_object_interface.h"
#include "nw_object_dynamic.h"
#include "nw_image_imageobserver.h"
#include "nw_image_abstractimage.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
void
(*NW_Image_IImageObserver_ImageOpened_t) ( NW_Image_IImageObserver_t* imageObserver,
                                           NW_Image_AbstractImage_t* abstractImage,
                                           NW_Bool failed,
                                           NW_Int16 suppressDupReformats );

typedef
void
(*NW_Image_IImageObserver_SizeChanged_t) ( NW_Image_IImageObserver_t* imageObserver,
                                           NW_Image_AbstractImage_t* abstractImage );

typedef
void
(*NW_Image_IImageObserver_ImageOpeningStarted_t) ( NW_Image_IImageObserver_t* imageObserver,
                                                   NW_Image_AbstractImage_t* abstractImage );

typedef
void
(*NW_Image_IImageObserver_ImageDecoded_t) ( NW_Image_IImageObserver_t* imageObserver,
                                            NW_Image_AbstractImage_t* abstractImage,
                                            NW_Bool failed);
typedef
void
(*NW_Image_IImageObserver_ImageDestroyed_t) ( NW_Image_IImageObserver_t* imageObserver,
                                              NW_Image_AbstractImage_t* abstractImage );

typedef
NW_Bool
(*NW_Image_IImageObserver_IsVisible_t) ( NW_Image_IImageObserver_t* aImageObserver);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_IImageObserver_ClassPart_s {
   NW_Image_IImageObserver_ImageOpened_t imageOpened;
   NW_Image_IImageObserver_SizeChanged_t sizeChanged;
   NW_Image_IImageObserver_ImageOpeningStarted_t imageOpeningStarted;
   NW_Image_IImageObserver_ImageDecoded_t imageDecoded;
   NW_Image_IImageObserver_ImageDestroyed_t imageDestroyed;
   NW_Image_IImageObserver_IsVisible_t isVisible;
} NW_Image_IImageObserver_ClassPart_t;
  
struct NW_Image_IImageObserver_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_Image_IImageObserver_ClassPart_t NW_Image_IImageObserver;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Image_IImageObserver_s {
  NW_Object_Interface_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_IImageObserver_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Image_IImageObserver))

#define NW_Image_IImageObserverOf(object) \
  (NW_Object_Cast (object, NW_Image_IImageObserver))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_Image_IImageObserver_Class_t NW_Image_IImageObserver_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Image_IImageObserver_ImageOpened( _imageObserver, _abstractImage, _failed, _suppressDupReformats ) \
  (NW_Object_Invoke ( _imageObserver, NW_Image_IImageObserver, imageOpened ) \
    ( (_imageObserver), (_abstractImage), (_failed), (_suppressDupReformats) ))

#define NW_Image_IImageObserver_SizeChanged( _imageObserver, _abstractImage ) \
  (NW_Object_Invoke ( _imageObserver, NW_Image_IImageObserver, sizeChanged ) \
    ( (_imageObserver), (_abstractImage) ))

#define NW_Image_IImageObserver_ImageOpeningStarted( _imageObserver, _abstractImage ) \
  (NW_Object_Invoke ( _imageObserver, NW_Image_IImageObserver, imageOpeningStarted ) \
    ( (_imageObserver), (_abstractImage) ))

#define NW_Image_IImageObserver_ImageDecoded( _imageObserver, _abstractImage, _failed ) \
  (NW_Object_Invoke ( _imageObserver, NW_Image_IImageObserver, imageDecoded ) \
    ( (_imageObserver), (_abstractImage), (_failed) ))

#define NW_Image_IImageObserver_ImageDestroyed( _imageObserver, _abstractImage ) \
  (NW_Object_Invoke ( _imageObserver, NW_Image_IImageObserver, imageDestroyed ) \
    ( (_imageObserver), (_abstractImage) ))

#define NW_Image_IImageObserver_IsVisible( _imageObserver ) \
  (NW_Object_Invoke ( _imageObserver, NW_Image_IImageObserver, isVisible ) \
    ( (_imageObserver) ))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_IIMAGEOBSERVER_H */
