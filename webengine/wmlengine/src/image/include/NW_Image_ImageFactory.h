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


#ifndef NW_Image_ImageFactory_h
#define NW_Image_ImageFactory_h

#include "nw_object_core.h"
#include "nw_image_abstractimage.h"
#include "nwx_buffer.h"
#include "nwx_http_header.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_ImageFactory_Class_s NW_Image_ImageFactory_Class_t;
typedef struct NW_Image_ImageFactory_s NW_Image_ImageFactory_t;

typedef struct NW_Image_ImageFactory_MimeTableEntry_s NW_Image_ImageFactory_MimeTableEntry_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Image_AbstractImage_t*
(*NW_Image_ImageFactory_CreateImage_t)(const NW_Image_ImageFactory_t* imageFactory,
                                    NW_Http_ContentTypeString_t contentTypeString,
                                    NW_Buffer_t* imageData);


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_ImageFactory_ClassPart_s {
  const NW_Image_ImageFactory_MimeTableEntry_t* mimeTable;
  NW_Uint16 numEntries;
  NW_Image_ImageFactory_CreateImage_t createImage;
} NW_Image_ImageFactory_ClassPart_t;

struct NW_Image_ImageFactory_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Image_ImageFactory_ClassPart_t NW_Image_ImageFactory;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Image_ImageFactory_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_ImageFactory_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Image_ImageFactory))

#define NW_Image_ImageFactoryOf(_object) \
  (NW_Object_Cast ((_object), NW_Image_ImageFactory))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT const NW_Image_ImageFactory_Class_t NW_Image_ImageFactory_Class;
NW_IMAGE_EXPORT const NW_Image_ImageFactory_t NW_Image_ImageFactory;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_Image_ImageFactory_CreateImage(_imageFactory, _contentTypeString, _imageData) \
  (NW_Object_Invoke ((_imageFactory), NW_Image_ImageFactory, createImage) \
    ((_imageFactory), (_contentTypeString), (_imageData)))


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_Image_ImageFactory_h */
