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


#ifndef NW_FBOX_EPOC32INLINEFORMIMAGES_H
#define NW_FBOX_EPOC32INLINEFORMIMAGES_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include "nw_fbox_epoc32formimages.h"

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineFormImages_Class_s NW_FBox_InlineFormImages_Class_t;
typedef struct NW_FBox_InlineFormImages_s NW_FBox_InlineFormImages_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineFormImages_ClassPart_s {
  void** unused;
} NW_FBox_InlineFormImages_ClassPart_t;

struct NW_FBox_InlineFormImages_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_FBox_FormImages_ClassPart_t NW_FBox_FormImages;
  NW_FBox_InlineFormImages_ClassPart_t NW_FBox_InlineFormImages;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_FBox_InlineFormImages_s {
  NW_FBox_FormImages_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InlineFormImages_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_FBox_FormImages))

#define NW_FBox_InlineFormImagesOf(_object) \
  (NW_Object_Cast ((_object), NW_FBox_FormImages))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_FBox_InlineFormImages_Class_t NW_FBox_InlineFormImages_Class;
extern const NW_FBox_InlineFormImages_t NW_FBox_InlineFormImages;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_FBOX_EPOC32FORMIMAGES_H */
