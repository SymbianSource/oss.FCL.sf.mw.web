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


#ifndef NW_IMAGE_ABSTRACTIMAGEI_H
#define NW_IMAGE_ABSTRACTIMAGEI_H

#include "nw_object_dynamici.h"
#include "nw_image_abstractimage.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_AbstractImage_Construct( NW_Object_Dynamic_t* dynamicObject,
                                   va_list *argp);

NW_IMAGE_EXPORT
void
_NW_Image_AbstractImage_Destruct( NW_Object_Dynamic_t* dynamicObject );

extern
TBrowserStatusCode
_NW_Image_AbstractImage_IncrementImage(NW_Image_AbstractImage_t* image); 

extern
NW_Int16
_NW_Image_AbstractImage_GetLoopCount(NW_Image_AbstractImage_t* image); 

extern
TBrowserStatusCode
_NW_Image_AbstractImage_SetLoopCount(NW_Image_AbstractImage_t* image, NW_Uint16 loopCount);

extern
NW_Uint32
_NW_Image_AbstractImage_GetDelay(NW_Image_AbstractImage_t* image);

extern
NW_Bool
_NW_Image_AbstractImage_IsAnimated(NW_Image_AbstractImage_t* image); 

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_AbstractImage_SetImageObserver( NW_Image_AbstractImage_t* epocImage,
                                          NW_Image_IImageObserver_t* observer );

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_AbstractImage_RemoveImageObserver( NW_Image_AbstractImage_t* image,
                                             NW_Image_IImageObserver_t* observer );

NW_IMAGE_EXPORT
void
_NW_Image_AbstractImage_ImageOpened( NW_Image_AbstractImage_t* image, NW_Bool failed,
                                     NW_Int16 suppressDupReformats );


NW_IMAGE_EXPORT
void
_NW_Image_AbstractImage_ImageSizeChanged( NW_Image_AbstractImage_t* image );

NW_IMAGE_EXPORT
void
_NW_Image_AbstractImage_ImageDecoded( NW_Image_AbstractImage_t* image,
                                      NW_Bool failed);

NW_IMAGE_EXPORT
void
_NW_Image_AbstractImage_ImageOpenStarted( NW_Image_AbstractImage_t* image );


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_ABSTRACTIMAGEI_H */
