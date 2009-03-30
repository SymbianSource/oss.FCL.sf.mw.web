/*
* Copyright (c) 2000-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_IMAGE_CANNEDIMAGES_H
#define NW_IMAGE_CANNEDIMAGES_H

#include "nw_object_dynamic.h"
#include "nw_image_abstractimage.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */

#define NW_Image_Invalid        0
#define NW_Image_Square         1
#define NW_Image_Disc           2
#define NW_Image_Circle         3
#define NW_Image_Missing        4
#define NW_Image_Broken         5
#define NW_Image_SelectArrow    6
#define NW_Image_Object         7   
#define NW_Image_SelectFile     8
#define NW_Image_ImageMap       9

#define NW_Image_NumberOfCannedImages 9

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_CannedImages_Class_s NW_Image_CannedImages_Class_t;
typedef struct NW_Image_CannedImages_s NW_Image_CannedImages_t;

typedef struct NW_Image_CannedImages_Entry_s NW_Image_CannedImages_Entry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_CannedImages_ClassPart_s {
  NW_Uint8 numEntries;
  const NW_Image_CannedImages_Entry_t* entries;
} NW_Image_CannedImages_ClassPart_t;

struct NW_Image_CannedImages_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Image_CannedImages_ClassPart_t NW_Image_CannedImages;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Image_CannedImages_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_Image_AbstractImage_t* dictionary[NW_Image_NumberOfCannedImages];
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_CannedImages_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Image_CannedImages))

#define NW_Image_CannedImagesOf(_object) \
  (NW_Object_Cast ((_object), NW_Image_CannedImages))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT const NW_Image_CannedImages_Class_t NW_Image_CannedImages_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT
const NW_Image_AbstractImage_t*
NW_Image_CannedImages_GetImage (NW_Image_CannedImages_t* cannedImages,
                                NW_Int8 imageId);
/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_Image_CannedImages_t*
NW_Image_CannedImages_New();

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_CANNEDIMAGES_H */
