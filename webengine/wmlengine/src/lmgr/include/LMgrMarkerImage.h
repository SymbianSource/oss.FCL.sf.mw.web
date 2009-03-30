/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class represents the marker box
*
*/

#ifndef _LMGRMARKERIMAGE_H
#define _LMGRMARKERIMAGE_H

#include "nw_lmgr_animatedimagebox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



typedef struct LMgrMarkerImage_ClassPart_s {
  void** unused;
} LMgrMarkerImage_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct LMgrMarkerImage_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_ImgContainerBox_ClassPart_t NW_LMgr_ImgContainerBox;
  NW_LMgr_AnimatedImageBox_ClassPart_t NW_LMgr_AnimatedImageBox;
  LMgrMarkerImage_ClassPart_t LMgrMarkerImage;
} LMgrMarkerImage_Class_t;

/* ------------------------------------------------------------------------- */

typedef struct LMgrMarkerImage_s 
{
  NW_LMgr_AnimatedImageBox_t super;
} LMgrMarkerImage_t;

/* ------------------------------------------------------------------------- */
#define LMgrMarkerImage_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, LMgrMarkerImage))


#define LMgrMarkerImageOf(_object) \
((LMgrMarkerImage_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const LMgrMarkerImage_Class_t LMgrMarkerImage_Class;

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
LMgrMarkerImage_t*
LMgrMarkerImage_New (NW_ADT_Vector_Metric_t numProperties, NW_Image_AbstractImage_t* aVirtualImage, 
                             NW_Text_t* aAltText, NW_Bool aBrokenImage );

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _LMGRMARKERIMAGE_H */
