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


#ifndef NW_IMAGE_EPOC32SIMPLEIMAGE_H
#define NW_IMAGE_EPOC32SIMPLEIMAGE_H

#include "nw_image_abstractimage.h"
#include "nw_hed_documentnode.h"
#include "BrsrStatusCodes.h"
#include "ImageUtils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_Epoc32Simple_Class_s NW_Image_Epoc32Simple_Class_t;
typedef struct NW_Image_Epoc32Simple_s NW_Image_Epoc32Simple_t;

typedef
TBrowserStatusCode
(*NW_Image_Epoc32Simple_PartialNextChunk_t) ( NW_Image_AbstractImage_t* image, NW_Buffer_t* nextChunk );

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_Epoc32Simple_ClassPart_s {
  NW_Image_Epoc32Simple_PartialNextChunk_t partialNextChunk;  
} NW_Image_Epoc32Simple_ClassPart_t;

struct NW_Image_Epoc32Simple_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Image_AbstractImage_ClassPart_t NW_Image_AbstractImage;
  NW_Image_Epoc32Simple_ClassPart_t NW_Image_Epoc32Simple;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Image_Epoc32Simple_s {
  NW_Image_AbstractImage_t super;

  /* member variables */
  void*                  decoder;       /* CEpoc32ImageDecoder */
  NW_GDI_Dimension3D_t   size;          /* size of image */
  const void*            bitmap;        /* image to display */
  const void*            mask;          /* bitmap mask */
  NW_Bool                invertedMask;  /* ETrue if mask is inverted, else EFalse */
  TImageType             imageType;     /* images that Symbian can't recognize */
  void*                  rawData;       /* undecoded image data */
  NW_Int32               rawDataLength; /* undecoded image data length */
  NW_Bool                needsDecode;   /* NW_TRUE if object needs decode before display */
  NW_Uint32              prevFlags;     /* Frame info flags from previous image decode */
  NW_GDI_Color_t         prevBackColor; /* background color from previous image frame */
  NW_GDI_Rectangle_t     prevRect;      /* topleft x,y and the size of previous image frame*/
  NW_ADT_DynamicVector_t* docNodeList;  /* Image content handler List */
  NW_Bool                imageDecoding; /* partial image decoding */
  NW_Int32              srcWidth;      /* Width found in the HTML <img> if any */
  NW_Int32              srcHeight;     /* Height found in the HTML <img>if any */
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_Epoc32Simple_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Image_Epoc32Simple))

#define NW_Image_Epoc32SimpleOf(_object) \
  (NW_Object_Cast ((_object), NW_Image_Epoc32Simple))

#define NW_Image_Epoc32Simple_GetData(_object) \
  ((NW_Image_Epoc32SimpleOf(_object))->data)

#define NW_Image_Epoc32Simple_GetRawData(_object) \
  ((NW_Image_Epoc32SimpleOf(_object))->rawData)

#define NW_Image_Epoc32Simple_GetRawDataLength(_object) \
  ((NW_Image_Epoc32SimpleOf(_object))->rawDataLength)

#define NW_Image_Epoc32Simple_PartialNextChunk( _image, _buffer ) \
  (NW_Object_Invoke ((_image), NW_Image_Epoc32Simple, partialNextChunk) \
    ( ( _image ), ( _buffer ) ) )

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT const NW_Image_Epoc32Simple_Class_t NW_Image_Epoc32Simple_Class;


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_Image_Epoc32Simple_t*
NW_Image_Epoc32Simple_New (NW_GDI_Dimension3D_t size, 
                           const void* data, 
                           const void* mask,
                           NW_Bool invertedMask,
                           NW_Bool isTransparent,
                           TImageType imageType,
                           void* rawData,
                           NW_Int32 length,
                           NW_HED_DocumentNode_t* documentNode);

TBrowserStatusCode
NW_Image_Epoc32Simple_SetSize (NW_Image_AbstractImage_t* image,
                               NW_GDI_Dimension3D_t* aSize);

TBrowserStatusCode
NW_Image_Epoc32Simple_SetAnimation(NW_Image_AbstractImage_t* image,
                                   NW_Bool isAnimated,
                                   NW_Uint32 delay );

void
NW_Image_Epoc32Simple_SetBitmap(NW_Image_Epoc32Simple_t* image,
                                void* bitmap,
                                void* mask,
                                NW_Int32 frameNum,
                                void* frameInfo,
                                NW_Bool frameIsReady );

void NW_Image_Epoc32Simple_DestroyRawData( NW_Image_Epoc32Simple_t* aImage );

TBrowserStatusCode
NW_Image_Epoc32Simple_AddDocNode(NW_Image_Epoc32Simple_t* epocImage, 
                                 NW_HED_DocumentNode_t* documentNode);

void
NW_Image_Epoc32Simple_OutOfView( NW_Image_Epoc32Simple_t* aEpocImage );

TBrowserStatusCode
NW_Image_Epoc32Simple_Initialize( NW_Image_Epoc32Simple_t* thisObj );

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_EPOC32SIMPLEIMAGE_H */
