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


#ifndef NW_IMAGEBOX_H
#define NW_IMAGEBOX_H


#include "nw_image_abstractimage.h"
#include "nw_lmgr_mediabox.h"
#include "NW_LMgr_IEventListener.h"
#include "nw_lmgr_eventhandler.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Image boxes hold image content. TODO: These still need to be subclassed */

typedef struct NW_LMgr_ImageBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_ImageBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ImageBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_ImageBox_ClassPart_t NW_LMgr_ImageBox;
} NW_LMgr_ImageBox_Class_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ImageBox_s {
  NW_LMgr_MediaBox_t super;
  
  NW_Image_AbstractImage_t* image;
} NW_LMgr_ImageBox_t;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_ImageBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ImageBox))
  
#define NW_LMgr_ImageBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_ImageBox))

/* ------------------------------------------------------------------------- */

#define NW_LMgr_ImageBox_SetImage(_object, _image) \
  ((_object)->image = _image)

NW_LMGR_EXPORT const NW_LMgr_ImageBox_Class_t NW_LMgr_ImageBox_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_ImageBox_t*
NW_LMgr_ImageBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_Image_AbstractImage_t* image);
void
NW_LMgr_ImageBox_HandleVerticalLayout(NW_LMgr_Box_t *box, NW_GDI_Dimension2D_t *size, NW_LMgr_RootBox_t* rootBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_IMAGEBOX_H */
