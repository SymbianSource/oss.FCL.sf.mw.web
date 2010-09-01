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


#ifndef NW_FBOX_EPOC32OPTGRPSKIN_H
#define NW_FBOX_EPOC32OPTGRPSKIN_H

#include "nw_fbox_optgrpskin.h"
#include "nw_fbox_formbox.h"
#include "nw_hed_appservices.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32OptGrpSkin_Class_s NW_FBox_Epoc32OptGrpSkin_Class_t;
typedef struct NW_FBox_Epoc32OptGrpSkin_s NW_FBox_Epoc32OptGrpSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32OptGrpSkin_ClassPart_s {
  void** unused;
} NW_FBox_Epoc32OptGrpSkin_ClassPart_t;
  
struct NW_FBox_Epoc32OptGrpSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_OptGrpSkin_ClassPart_t NW_FBox_OptGrpSkin;
  NW_FBox_Epoc32OptGrpSkin_ClassPart_t NW_FBox_Epoc32OptGrpSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_Epoc32OptGrpSkin_s {
  NW_FBox_OptGrpSkin_t super;

  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Epoc32OptGrpSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Epoc32OptGrpSkin))

#define NW_FBox_Epoc32OptGrpSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_Epoc32OptGrpSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Epoc32OptGrpSkin_Class_t NW_FBox_Epoc32OptGrpSkin_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_EPOC32OPTGRPSKIN_H */
