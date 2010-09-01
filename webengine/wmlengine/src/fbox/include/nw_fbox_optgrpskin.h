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


#ifndef NW_FBOX_OPTGRPSKIN_H
#define NW_FBOX_OPTGRPSKIN_H

#include "nw_fbox_skin.h"
#include "nw_fbox_optgrpbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_OptGrpSkin_Class_s NW_FBox_OptGrpSkin_Class_t;
typedef struct NW_FBox_OptGrpSkin_s NW_FBox_OptGrpSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

typedef 
TBrowserStatusCode
(*NW_FBox_OptGrpSkin_AddOption_t)(NW_FBox_OptGrpSkin_t* thisObj,
                                  NW_FBox_OptionBox_t* optionBox);

typedef 
TBrowserStatusCode
(*NW_FBox_OptGrpSkin_AddOptGrp_t)(NW_FBox_OptGrpSkin_t* thisObj,
                                  NW_FBox_OptGrpBox_t* optGrpBox);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_OptGrpSkin_ClassPart_s {
  NW_FBox_OptGrpSkin_AddOption_t addOption;
  NW_FBox_OptGrpSkin_AddOptGrp_t addOptGrp;
} NW_FBox_OptGrpSkin_ClassPart_t;
  
struct NW_FBox_OptGrpSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_OptGrpSkin_ClassPart_t NW_FBox_OptGrpSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_OptGrpSkin_s {
  NW_FBox_Skin_t super;

  /* member variables */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_OptGrpSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_OptGrpSkin))

#define NW_FBox_OptGrpSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_OptGrpSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_OptGrpSkin_Class_t NW_FBox_OptGrpSkin_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_OptGrpSkin_AddOption(_object, _optionBox) \
  (NW_Object_Invoke (_object, NW_FBox_OptGrpSkin, addOption) \
  ((_object), (_optionBox)))
    
#define NW_FBox_OptGrpSkin_AddOptGrp(_object, _optGrpBox) \
  (NW_Object_Invoke (_object, NW_FBox_OptGrpSkin, addOptGrp) \
  ((_object), (_optGrpBox)))
    
/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_OptGrpSkin_t*
NW_FBox_OptGrpSkin_New(NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
