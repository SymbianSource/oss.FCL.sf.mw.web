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


#ifndef NW_IMAGE_EPOC32SIMPLEIMAGEI_H
#define NW_IMAGE_EPOC32SIMPLEIMAGEI_H

#include "nw_image_abstractimagei.h"
#include "nw_image_epoc32simpleimage.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
_NW_Image_Epoc32Simple_Construct (NW_Object_Dynamic_t* dynamicObject,
                          va_list *argp);

extern
void
_NW_Image_Epoc32Simple_Destruct (NW_Object_Dynamic_t* dynamicObject);


NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_PartialNextChunk( NW_Image_AbstractImage_t* image,
                                        NW_Buffer_t* nextChunk );

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_GetSize (const NW_Image_AbstractImage_t* image,
                          NW_GDI_Dimension3D_t* size);

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_Draw ( NW_Image_AbstractImage_t* image,
                       CGDIDeviceContext* deviceContext,
                       const NW_GDI_Point2D_t* location);

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_DrawScaled ( NW_Image_AbstractImage_t* image,
                       CGDIDeviceContext* deviceContext,
                       const NW_GDI_Point2D_t* location,
                       const NW_GDI_Dimension3D_t* size);

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_DrawInRect ( NW_Image_AbstractImage_t* image,
                       CGDIDeviceContext* deviceContext,
                       const NW_GDI_Point2D_t* location,
                       const NW_GDI_Dimension3D_t* size,
                       const NW_GDI_Rectangle_t* rect);


NW_IMAGE_EXPORT
void
_NW_Image_Epoc32SimpleImage_ImageOpened( NW_Image_AbstractImage_t* aAbstractImage, NW_Bool aFailed,
                                     NW_Int16 aSuppressDupReformats );

NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_IncrementImage(NW_Image_AbstractImage_t* image);


NW_IMAGE_EXPORT
TBrowserStatusCode
_NW_Image_Epoc32Simple_ForceImageDecode( NW_Image_AbstractImage_t* image );


/* ------------------------------------------------------------------------- *
   protected methods 
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Image_Epoc32Simple_Scale(NW_Image_AbstractImage_t* image,
                            CGDIDeviceContext* deviceContext,
                            const NW_GDI_Dimension3D_t* size);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_EPOC32SIMPLEIMAGEI_H */
