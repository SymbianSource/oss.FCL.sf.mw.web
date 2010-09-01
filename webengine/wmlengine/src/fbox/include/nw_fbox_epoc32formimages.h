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


#ifndef NW_FBOX_EPOC32FORMIMAGES_H
#define NW_FBOX_EPOC32FORMIMAGES_H

#include "nw_object_base.h"
#include "nw_fbox_formbox.h"
#include "nw_image_abstractimage.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FormImages_Class_s NW_FBox_FormImages_Class_t;
typedef struct NW_FBox_FormImages_s NW_FBox_FormImages_t;

typedef struct NW_FBox_FormImages_Entry_s NW_FBox_FormImages_Entry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FormImages_ClassPart_s {
  NW_Uint8 numCheckBoxEntries;
  const NW_FBox_FormImages_Entry_t* checkBoxEntries;
  NW_Uint8 numRadioBoxEntries;
  const NW_FBox_FormImages_Entry_t* radioBoxEntries;
} NW_FBox_FormImages_ClassPart_t;

struct NW_FBox_FormImages_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_FBox_FormImages_ClassPart_t NW_FBox_FormImages;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_FBox_FormImages_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_FormImages_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_FBox_FormImages))

#define NW_FBox_FormImagesOf(_object) \
  (NW_Object_Cast ((_object), NW_FBox_FormImages))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_FormImages_Class_t NW_FBox_FormImages_Class;
NW_FBOX_EXPORT const NW_FBox_FormImages_t NW_FBox_FormImages;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
const NW_Image_AbstractImage_t*
NW_FBox_FormImages_GetImage (NW_FBox_FormImages_t* thisObj,
                             NW_Bool hasFocus,
                             NW_Bool state,
							               NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_EPOC32FORMIMAGES_H */
