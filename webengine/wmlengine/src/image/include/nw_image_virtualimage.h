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


#ifndef NW_IMAGE_VIRTUALIMAGE_H
#define NW_IMAGE_VIRTUALIMAGE_H

#include "nw_image_abstractimage.h"
#include "nw_image_iimageobserver.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_Virtual_Class_s NW_Image_Virtual_Class_t;
typedef struct NW_Image_Virtual_s NW_Image_Virtual_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Image_Virtual_ClassPart_s {
  void** unused;
} NW_Image_Virtual_ClassPart_t;

struct NW_Image_Virtual_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Image_AbstractImage_ClassPart_t NW_Image_AbstractImage;
  NW_Image_Virtual_ClassPart_t NW_Image_Virtual;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Image_Virtual_s {
  NW_Image_AbstractImage_t super;

  /* member variables */
  NW_Image_AbstractImage_t* image;
  // whenever the virtual image is set to be observed, it
  // must set to observ its simple image
  NW_Image_IImageObserver_t NW_Image_IImageObserver;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Image_Virtual_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Image_Virtual))

#define NW_Image_VirtualOf(_object) \
  (NW_Object_Cast ((_object), NW_Image_Virtual))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT const NW_Image_Virtual_Class_t NW_Image_Virtual_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT
NW_Image_Virtual_t*
NW_Image_Virtual_New (NW_Image_AbstractImage_t* image);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_VIRTUALIMAGE_H */
