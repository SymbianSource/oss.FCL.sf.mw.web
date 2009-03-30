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


#ifndef NW_IMAGE_EPOC32CANNEDIMAGE_H
#define NW_IMAGE_EPOC32CANNEDIMAGE_H

#include "nw_image_epoc32simpleimage.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_Epoc32Canned_Class_s NW_Image_Epoc32Canned_Class_t;
typedef struct NW_Image_Epoc32Canned_s NW_Image_Epoc32Canned_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_Epoc32Canned_ClassPart_s {
  void** unused;
} NW_Image_Epoc32Canned_ClassPart_t;

struct NW_Image_Epoc32Canned_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Image_AbstractImage_ClassPart_t NW_Image_AbstractImage;
  NW_Image_Epoc32Simple_ClassPart_t NW_Image_Epoc32Simple;
  NW_Image_Epoc32Canned_ClassPart_t NW_Image_Epoc32Canned;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Image_Epoc32Canned_s {
  NW_Image_Epoc32Simple_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_Epoc32Canned_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Image_Epoc32Canned))

#define NW_Image_Epoc32CannedOf(_object) \
  (NW_Object_Cast ((_object), NW_Image_Epoc32Canned))

#define NW_Image_Epoc32Canned_GetData(_object) \
  ((NW_Image_Epoc32CannedOf(_object))->data)

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT const NW_Image_Epoc32Canned_Class_t NW_Image_Epoc32Canned_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
NW_Image_Epoc32Canned_Destroy(NW_Image_AbstractImage_t* image);


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_Image_Epoc32Canned_t*
NW_Image_Epoc32Canned_New (NW_GDI_Dimension3D_t size, 
                           const void* data, 
                           const void* mask,
                           NW_Bool invertedMask,
                           NW_Bool isTransparent,
                           TImageType imageType,
                           const void* rawData,
                           NW_Int32 length);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_EPOC32SIMPLEIMAGE_H */
