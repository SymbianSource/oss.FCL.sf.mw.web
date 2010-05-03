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


#ifndef _NW_Markup_ImageViewer_h_
#define _NW_Markup_ImageViewer_h_

#include "NW_Markup_EXPORT.h"
#include "nw_object_aggregate.h"
#include "nw_hed_iimageviewer.h"
#include "nw_lmgr_box.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_ImageViewer_Class_s NW_Markup_ImageViewer_Class_t;
typedef struct NW_Markup_ImageViewer_s NW_Markup_ImageViewer_t;

/* ------------------------------------------------------------------------- *
   virtual method type definition(s)
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Markup_ImageViewer_LoadImage_t) (NW_Markup_ImageViewer_t* imageViewer);

typedef
NW_Bool
(*NW_Markup_ImageViewer_IsNoSave_t) (NW_Markup_ImageViewer_t* imageViewer,
                                     NW_LMgr_Box_t* imageBox);

typedef
NW_Ucs2*
(*NW_Markup_ImageViewer_GetAltText_t) (NW_Markup_ImageViewer_t* imageViewer,
                                       NW_LMgr_Box_t* imageBox);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_ImageViewer_ClassPart_s {
  NW_Markup_ImageViewer_LoadImage_t loadImage;
  NW_Markup_ImageViewer_IsNoSave_t isNoSave;
  NW_Markup_ImageViewer_GetAltText_t getAltText;
} NW_Markup_ImageViewer_ClassPart_t;

struct NW_Markup_ImageViewer_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_Object_Aggregate_ClassPart_t NW_Object_Aggregate;
  NW_Markup_ImageViewer_ClassPart_t NW_Markup_ImageViewer;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Markup_ImageViewer_s {
  NW_Object_Aggregate_t super;

  /* interface implementation(s) */
  NW_HED_IImageViewer_t NW_HED_IImageViewer;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_ImageViewer_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Markup_ImageViewer))

#define NW_Markup_ImageViewerOf(_object) \
  (NW_Object_Cast (_object, NW_Markup_ImageViewer))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_ImageViewer_Class_t NW_Markup_ImageViewer_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Markup_ImageViewer_h_ */
