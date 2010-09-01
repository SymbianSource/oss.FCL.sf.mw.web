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


#ifndef NW_IMGCONTAINERBOX_H
#define NW_IMGCONTAINERBOX_H

#include "nw_lmgr_mediabox.h"
#include "nw_image_abstractimage.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
/* Container boxes are boxes with children */
/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ImgContainerBox_Class_s NW_LMgr_ImgContainerBox_Class_t;
typedef struct NW_LMgr_ImgContainerBox_s NW_LMgr_ImgContainerBox_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ImgContainerBox_ClassPart_s {
  void** unused;
} NW_LMgr_ImgContainerBox_ClassPart_t;

struct NW_LMgr_ImgContainerBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_ImgContainerBox_ClassPart_t NW_LMgr_ImgContainerBox;
};

/* ------------------------------------------------------------------------- */
struct NW_LMgr_ImgContainerBox_s {
  NW_LMgr_MediaBox_t super;

  NW_Bool isBroken;
  NW_Text_t* altText;
  NW_Image_AbstractImage_t* virtualImage;
  NW_Bool iBoxPlaced;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_ImgContainerBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ImgContainerBox))

#define NW_LMgr_ImgContainerBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ImgContainerBox))

/* Final Methods ---------------------------------------------------------- */

/*-------------------------------------------------------------------------*/
NW_LMGR_EXPORT const NW_LMgr_ImgContainerBox_Class_t NW_LMgr_ImgContainerBox_Class;

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */
NW_IMAGE_EXPORT
void
NW_LMgr_ImgContainerBox_ReplaceBrokenImage( NW_LMgr_Box_t* box, NW_Image_AbstractImage_t* image );

NW_IMAGE_EXPORT
NW_Bool
NW_LMgr_ImgContainerBox_ImageIsBroken( NW_LMgr_Box_t* box );

NW_IMAGE_EXPORT
void
NW_LMgr_ImgContainerBox_ImageIsPlaced( NW_LMgr_Box_t* box );

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_ImgContainerBox_t*
NW_LMgr_ImgContainerBox_New (NW_ADT_Vector_Metric_t numProperties, NW_Image_AbstractImage_t* virtualImage, 
                             NW_Text_t* altText, NW_Bool brokenImage );



/* ------------------------------------------------------------------------- *
  global utility functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ImgBox_DrawContent( NW_LMgr_Box_t* box,
                            CGDIDeviceContext* deviceContext,
                            NW_Image_AbstractImage_t* image,
                            NW_Bool isBroken,
                            NW_Text_t* altText,
                            NW_Uint8 hasFocus);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ImgContainerBox_DisplayAltText(NW_LMgr_Box_t* box,
                                       NW_Text_t* altText,
                                       CGDIDeviceContext* deviceContext,
                                       NW_GDI_Metric_t space,
                                       NW_GDI_Point2D_t *textPos,
                                       CGDIFont *boxFont);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_CONTAINERBOX_H */
