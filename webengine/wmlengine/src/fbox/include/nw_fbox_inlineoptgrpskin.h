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


#ifndef NW_FBOX_INLINEOPTGRPSKIN_H
#define NW_FBOX_INLINEOPTGRPSKIN_H

#include "nw_fbox_optgrpskin.h"
#include "nw_fbox_formbox.h"
#include "nw_lmgr_bidiflowbox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineOptGrpSkin_Class_s NW_FBox_InlineOptGrpSkin_Class_t;
typedef struct NW_FBox_InlineOptGrpSkin_s NW_FBox_InlineOptGrpSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InlineOptGrpSkin_ClassPart_s {
  void** unused;
} NW_FBox_InlineOptGrpSkin_ClassPart_t;
  
struct NW_FBox_InlineOptGrpSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_OptGrpSkin_ClassPart_t NW_FBox_OptGrpSkin;
  NW_FBox_InlineOptGrpSkin_ClassPart_t NW_FBox_InlineOptGrpSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_InlineOptGrpSkin_s {
  NW_FBox_OptGrpSkin_t super;

  /* member variables */
  NW_LMgr_BidiFlowBox_t* bidiFlowBox;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InlineOptGrpSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_InlineOptGrpSkin))

#define NW_FBox_InlineOptGrpSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_InlineOptGrpSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_InlineOptGrpSkin_Class_t NW_FBox_InlineOptGrpSkin_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_InlineOptGrpSkin_GetBidiFlowBox(_object)\
  (NW_FBox_InlineOptGrpSkinOf(_object)->bidiFlowBox)

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_InlineOPTGRPSKIN_H */
