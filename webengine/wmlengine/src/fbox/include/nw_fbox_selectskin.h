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


#ifndef NW_FBOX_SELECTSKIN_H
#define NW_FBOX_SELECTSKIN_H

#include "nw_object_shared.h"
#include "nw_fbox_buttonboxskin.h"
#include "nw_fbox_selectbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_SelectSkin_Class_s NW_FBox_SelectSkin_Class_t;
typedef struct NW_FBox_SelectSkin_s NW_FBox_SelectSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

typedef 
TBrowserStatusCode
(*NW_FBox_SelectSkin_SetActive_t)(NW_FBox_SelectSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_SelectSkin_AddOption_t)(NW_FBox_SelectSkin_t* thisObj,
                                  NW_FBox_OptionBox_t* optionBox,
                                  NW_LMgr_Box_t** styleBox);

typedef 
TBrowserStatusCode
(*NW_FBox_SelectSkin_AddOptGrp_t)(NW_FBox_SelectSkin_t* thisObj,
                                  NW_FBox_OptGrpBox_t* optGrpBox);

typedef 
TBrowserStatusCode
(*NW_FBox_SelectSkin_DetailsDialog_t)(NW_FBox_SelectSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_SelectSkin_RefreshText_t)(NW_FBox_SelectSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_SelectSkin_Split_t)(NW_FBox_SelectSkin_t* thisObj,
                             NW_GDI_Metric_t space,
                             NW_LMgr_Box_t* *splitBox,
                             NW_Uint8 flags);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_SelectSkin_ClassPart_s {
  NW_FBox_SelectSkin_SetActive_t setActive;
  NW_FBox_SelectSkin_AddOption_t addOption;
  NW_FBox_SelectSkin_AddOptGrp_t addOptGrp;
  NW_FBox_SelectSkin_DetailsDialog_t detailsDialog;
  NW_FBox_SelectSkin_RefreshText_t refreshText;
  NW_FBox_SelectSkin_Split_t split;
} NW_FBox_SelectSkin_ClassPart_t;
  
struct NW_FBox_SelectSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_SelectSkin_ClassPart_t NW_FBox_SelectSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_SelectSkin_s {
  NW_FBox_ButtonBoxSkin_t super;

  /* private don't touch */
  NW_Object_Shared_t* sharingObject;

  /* member variables */
  NW_Bool hasFormBox;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_SelectSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_SelectSkin))

#define NW_FBox_SelectSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_SelectSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_SelectSkin_Class_t NW_FBox_SelectSkin_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_SelectSkin_SetActive(_object) \
  (NW_Object_Invoke (_object, NW_FBox_SelectSkin, setActive) \
  ((_object)))

#define NW_FBox_SelectSkin_AddOption(_object, _optionBox, _box) \
  (NW_Object_Invoke (_object, NW_FBox_SelectSkin, addOption) \
  ((_object), (_optionBox), (_box)))

#define NW_FBox_SelectSkin_AddOptGrp(_object, _optGrpBox) \
  (NW_Object_Invoke (_object, NW_FBox_SelectSkin, addOptGrp) \
  ((_object), (_optGrpBox)))

#define NW_FBox_SelectSkin_DetailsDialog(_object) \
  (NW_Object_Invoke (_object, NW_FBox_SelectSkin, detailsDialog) \
  ((_object)))

#define NW_FBox_SelectSkin_RefreshText(_object) \
  (NW_Object_Invoke (_object, NW_FBox_SelectSkin, refreshText) \
  ((_object)))

#define NW_FBox_SelectSkin_Split(_object, _space, _splitBox, _flags) \
  (NW_Object_Invoke (_object, NW_FBox_SelectSkin, split) \
  ((_object), (_space), (_splitBox), (_flags)))

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_SelectSkin_GetMCharExtent(NW_FBox_SelectSkin_t* skin,
                                 NW_GDI_Dimension2D_t *charExtent);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_SelectSkin_Reserve(NW_FBox_SelectSkin_t* thisObj);


NW_FBOX_EXPORT
NW_Bool
NW_FBox_SelectSkin_Release(NW_FBox_SelectSkin_t* thisObj);
        
/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_SelectSkin_t*
NW_FBox_SelectSkin_New(NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INPUTSKIN_H */
