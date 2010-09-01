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


#ifndef NW_FBOX_OPTGRPBOX_H
#define NW_FBOX_OPTGRPBOX_H

#include "nw_fbox_formbox.h"
#include "nw_fbox_optionbox.h"
#include "nw_adt_dynamicvector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_OptGrpBox_Class_s NW_FBox_OptGrpBox_Class_t;
typedef struct NW_FBox_OptGrpBox_s NW_FBox_OptGrpBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_OptGrpBox_ClassPart_s {
  void** unused;
} NW_FBox_OptGrpBox_ClassPart_t;
  
struct NW_FBox_OptGrpBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
  NW_FBox_OptGrpBox_ClassPart_t NW_FBox_OptGrpBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_OptGrpBox_s {
  NW_FBox_FormBox_t super;

  /* member variables */
  NW_Text_t* text;
  NW_ADT_DynamicVector_t* children;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_OptGrpBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_OptGrpBox))

#define NW_FBox_OptGrpBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_OptGrpBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_OptGrpBox_Class_t NW_FBox_OptGrpBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_OptGrpBox_GetText(_optGrpBox) \
  (NW_FBox_OptGrpBoxOf(_optGrpBox)->text)

NW_FBOX_EXPORT
void
NW_FBox_OptGrpBox_AddOption(NW_FBox_OptGrpBox_t* optGrpBox, 
                            NW_FBox_OptionBox_t* optionBox);

NW_FBOX_EXPORT
void
NW_FBox_OptGrpBox_AddOptGrp(NW_FBox_OptGrpBox_t* optGrpBox, 
                            NW_FBox_OptGrpBox_t* childOptGrpBox);

NW_FBOX_EXPORT
NW_ADT_Vector_Metric_t
NW_FBox_OptGrpBox_GetNumOptions(NW_FBox_OptGrpBox_t* optGrpBox);

NW_FBOX_EXPORT
NW_FBox_OptionBox_t*
NW_FBox_OptGrpBox_GetOption(NW_FBox_OptGrpBox_t* optGrpBox,
                            NW_ADT_Vector_Metric_t index);


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_OptGrpBox_t*
NW_FBox_OptGrpBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_Text_t* text);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_OPTGRPBOX_H */
