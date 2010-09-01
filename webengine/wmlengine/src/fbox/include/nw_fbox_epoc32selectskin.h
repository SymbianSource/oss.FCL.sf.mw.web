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


#ifndef NW_FBOX_EPOC32SELECTSKIN_H
#define NW_FBOX_EPOC32SELECTSKIN_H

#include "nw_fbox_buttonboxskin.h"
#include "nw_fbox_formbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
#define NW_FBOX_SELECTBOX_LINE_SPACING_IN_PIXELS       0x02

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32SelectSkin_Class_s NW_FBox_Epoc32SelectSkin_Class_t;
typedef struct NW_FBox_Epoc32SelectSkin_s NW_FBox_Epoc32SelectSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_GDI_Metric_t
( *NW_FBox_Epoc32SelectSkin_GetBaseline_t ) (
    NW_FBox_Epoc32SelectSkin_t* aThisObj,
    NW_LMgr_Box_t* aLmgrBox );

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32SelectSkin_ClassPart_s {
  NW_FBox_Epoc32SelectSkin_GetBaseline_t getBaseline;
} NW_FBox_Epoc32SelectSkin_ClassPart_t;
  
struct NW_FBox_Epoc32SelectSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_SelectSkin_ClassPart_t NW_FBox_SelectSkin;
  NW_FBox_Epoc32SelectSkin_ClassPart_t NW_FBox_Epoc32SelectSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_Epoc32SelectSkin_s {
  NW_FBox_SelectSkin_t super;

  /* member variables */
  NW_Text_UCS2_t* value;

  NW_Text_UCS2_t* displayText;
  NW_ADT_DynamicVector_t* optionsList;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Epoc32SelectSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Epoc32SelectSkin))

#define NW_FBox_Epoc32SelectSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_Epoc32SelectSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Epoc32SelectSkin_Class_t NW_FBox_Epoc32SelectSkin_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

// Documented in nw_fbox_epoc32selectskini.h.
#define NW_FBox_Epoc32SelectSkin_GetBaseline(_object, _box) \
    (NW_Object_Invoke (_object, NW_FBox_Epoc32SelectSkin, getBaseline) \
    ((_object), (_box)))

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_GetValue(NW_FBox_Epoc32SelectSkin_t *skin, 
                               NW_Ucs2** newStr);

NW_FBOX_EXPORT
TBrowserStatusCode 
NW_FBox_Epoc32SelectSkin_UpdateSelection(NW_FBox_Epoc32SelectSkin_t* skin,
                                             NW_Ucs2* selectedText,
                                             NW_Bool* selectUpdated);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_EPOC32INPUTSKIN_H */
