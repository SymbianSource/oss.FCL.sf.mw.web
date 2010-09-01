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


#ifndef NW_ANIMATEDIMAGEBOX_H
#define NW_ANIMATEDIMAGEBOX_H

#include "nw_lmgr_imgcontainerboxi.h"
#include "nw_image_abstractimage.h"
#include "nw_lmgr_animatedimageaggregate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NW_LMgr_AnimatedImageBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_AnimatedImageBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AnimatedImageBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_ImgContainerBox_ClassPart_t NW_LMgr_ImgContainerBox;
  NW_LMgr_AnimatedImageBox_ClassPart_t NW_LMgr_AnimatedImageBox;
} NW_LMgr_AnimatedImageBox_Class_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AnimatedImageBox_s {
  NW_LMgr_ImgContainerBox_t super;

  /* implemented aggregate */
  NW_LMgr_AnimatedImageAggregate_t NW_LMgr_AnimatedImageAggregate;

  NW_Bool iIsSuspended;

} NW_LMgr_AnimatedImageBox_t;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_AnimatedImageBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_AnimatedImageBox))
  
#define NW_LMgr_AnimatedImageBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_AnimatedImageBox))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_AnimatedImageBox_Class_t NW_LMgr_AnimatedImageBox_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
void
NW_LMgr_AnimatedImageBox_Suspend (NW_LMgr_AnimatedImageBox_t* aAnimatedBox);

void
NW_LMgr_AnimatedImageBox_Resume (NW_LMgr_AnimatedImageBox_t* aAnimatedBox);

NW_LMGR_EXPORT
NW_LMgr_AnimatedImageBox_t*
NW_LMgr_AnimatedImageBox_New (NW_ADT_Vector_Metric_t numProperties, NW_Image_AbstractImage_t* virtualImage, 
                             NW_Text_t* altText, NW_Bool brokenImage );

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* NW_IMAGEBOX_H */
