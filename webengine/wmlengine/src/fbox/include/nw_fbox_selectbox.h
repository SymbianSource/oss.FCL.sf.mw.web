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


#ifndef NW_FBOX_SELECTBOX_H
#define NW_FBOX_SELECTBOX_H

#include "nw_fbox_formbox.h"
#include "nw_text_ucs2.h"
#include "nw_hed_appservices.h"
#include "nw_fbox_optionbox.h"
#include "nw_fbox_optgrpbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_SelectBox_Class_s NW_FBox_SelectBox_Class_t;
typedef struct NW_FBox_SelectBox_s NW_FBox_SelectBox_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_SelectBox_ClassPart_s {
  void** unused;
} NW_FBox_SelectBox_ClassPart_t;
  
struct NW_FBox_SelectBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
  NW_FBox_SelectBox_ClassPart_t NW_FBox_SelectBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_SelectBox_s {
  NW_FBox_FormBox_t super;

  /* member variables */
  NW_Text_t* name;
  NW_Text_t* title;
  NW_ADT_DynamicVector_t* children;
  NW_HED_AppServices_t* appServices;
  NW_Bool active;
  NW_Bool multiple;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_SelectBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_SelectBox))

#define NW_FBox_SelectBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_SelectBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_SelectBox_Class_t NW_FBox_SelectBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_SelectBox_SetFormLiaisonVal(NW_FBox_SelectBox_t* selectBox);

void
NW_FBox_SelectBox_SetActive(NW_FBox_SelectBox_t* selectBox, NW_Bool active);

void
NW_FBox_SelectBox_AddOption(NW_FBox_SelectBox_t* selectBox, 
                            NW_FBox_OptionBox_t* optionBox,
                            NW_LMgr_Box_t** styleBox);

void
NW_FBox_SelectBox_AddOptGrp(NW_FBox_SelectBox_t* selectBox, 
                            NW_FBox_OptGrpBox_t* optGrpBox);

#define NW_FBox_SelectBox_IsMultiple(_selectBox) \
  (NW_FBox_SelectBoxOf(_selectBox)->multiple)

#define NW_FBox_SelectBox_GetName(_selectBox) \
  (NW_FBox_SelectBoxOf(_selectBox)->name)

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_SelectBox_t*
NW_FBox_SelectBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_HED_AppServices_t* appServices,
                      NW_Bool multiple,
                      NW_Text_t* name,
                      NW_Text_t* title);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
