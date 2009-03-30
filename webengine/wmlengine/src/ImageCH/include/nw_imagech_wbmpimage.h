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


#ifndef NW_IMAGECH_WBMPIMAGE_H
#define NW_IMAGECH_WBMPIMAGE_H

#include "nw_image_abstractimage.h"
#include <nwx_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ImageCH_WBMP_Class_s NW_ImageCH_WBMP_Class_t;
typedef struct NW_ImageCH_WBMP_s NW_ImageCH_WBMP_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ImageCH_WBMP_ClassPart_s {
  void** unused;
} NW_ImageCH_WBMP_ClassPart_t;

struct NW_ImageCH_WBMP_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Image_AbstractImage_ClassPart_t NW_Image_AbstractImage;
  NW_ImageCH_WBMP_ClassPart_t NW_ImageCH_WBMP;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ImageCH_WBMP_s {
  NW_Image_AbstractImage_t super;

  /* member variables */
  void* data;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ImageCH_WBMP_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_ImageCH_WBMP))

#define NW_ImageCH_WBMPOf(_object) \
  (NW_Object_Cast ((_object), NW_ImageCH_WBMP))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_ImageCH_WBMP_Class_t NW_ImageCH_WBMP_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_ImageCH_WBMP_t*
NW_ImageCH_WBMP_New (NW_Buffer_t* buffer);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGECH_WBMPIMAGE_H */
