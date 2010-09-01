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


#ifndef NW_IMAGE_VIRTUALIMAGEI_H
#define NW_IMAGE_VIRTUALIMAGEI_H

#include "nw_image_abstractimagei.h"
#include "nw_image_virtualimage.h"
#include "nw_image_iimageobserveri.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   protected static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_Object_Class_t* const _NW_Image_VirtualImage_InterfaceList[];
NW_LMGR_EXPORT const NW_Image_IImageObserver_Class_t _NW_Image_VirtualImage_ImageObserver_Class;

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_VirtualImage_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list *argp);

NW_IMAGE_EXPORT
void
_NW_Image_VirtualImage_Destruct( NW_Object_Dynamic_t* dynamicObject );

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_VirtualImage_GetSize (const NW_Image_AbstractImage_t* image,
                           NW_GDI_Dimension3D_t* size);

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_VirtualImage_Draw ( NW_Image_AbstractImage_t* image,
                        CGDIDeviceContext* deviceContext,
                        const NW_GDI_Point2D_t* location);

TBrowserStatusCode
_NW_Image_VirtualImage_DrawScaled(NW_Image_AbstractImage_t* image,
                                  CGDIDeviceContext* deviceContext,
                                  const NW_GDI_Point2D_t* location,
                                  const NW_GDI_Dimension3D_t* size);
TBrowserStatusCode
_NW_Image_VirtualImage_DrawInRect(NW_Image_AbstractImage_t* image,
                                  CGDIDeviceContext* deviceContext,
                                  const NW_GDI_Point2D_t* location,
                                  const NW_GDI_Dimension3D_t* size,
                                  const NW_GDI_Rectangle_t* rect);

extern
TBrowserStatusCode
_NW_Image_VirtualImage_IncrementImage(NW_Image_AbstractImage_t* image);

extern
NW_Int16
_NW_Image_VirtualImage_GetLoopCount(NW_Image_AbstractImage_t* image);

extern
TBrowserStatusCode
_NW_Image_VirtualImage_SetLoopCount(NW_Image_AbstractImage_t* image, NW_Uint16 loopCount);

extern
NW_Uint32
_NW_Image_VirtualImage_GetDelay(NW_Image_AbstractImage_t* image);

extern
NW_Bool
_NW_Image_VirtualImage_IsAnimated(NW_Image_AbstractImage_t* image);

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_VirtualImage_SetImageObserver( NW_Image_AbstractImage_t* image,
                                         NW_Image_IImageObserver_t* observer );

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_VirtualImage_RemoveImageObserver( NW_Image_AbstractImage_t* image,
                                         NW_Image_IImageObserver_t* observer );

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_VirtualImage_ForceImageDecode( NW_Image_AbstractImage_t* image );

NW_IMAGE_EXPORT
void
_NW_Image_VirtualImage_IImageObserver_ImageOpened( NW_Image_IImageObserver_t* imageObserver,
                                                   NW_Image_AbstractImage_t* abstractImage,
                                                   NW_Bool failed,
                                                   NW_Int16 suppressDupReformats );

NW_IMAGE_EXPORT
void
_NW_Image_VirtualImage_IImageObserver_SizeChanged( NW_Image_IImageObserver_t* imageObserver,
                                                   NW_Image_AbstractImage_t* abstractImage );

NW_IMAGE_EXPORT
void
_NW_Image_VirtualImage_IImageObserver_ImageOpeningStarted( NW_Image_IImageObserver_t* imageObserver,
                                                           NW_Image_AbstractImage_t* abstractImage );

NW_IMAGE_EXPORT
void
_NW_Image_VirtualImage_IImageObserver_ImageDecoded( NW_Image_IImageObserver_t* imageObserver,
                                                    NW_Image_AbstractImage_t* abstractImage,
                                                    NW_Bool failed);

NW_IMAGE_EXPORT
void
_NW_Image_VirtualImage_IImageObserver_ImageDestroyed( NW_Image_IImageObserver_t* imageObserver,
                                                    NW_Image_AbstractImage_t* abstractImage );

NW_Bool
_NW_Image_VirtualImage_IImageObserver_IsVisible(NW_Image_IImageObserver_t* aImageObserver);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_VIRTUALIMAGEI_H */
