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


#ifndef NW_FBOX_BUTTONBOX_H
#define NW_FBOX_BUTTONBOX_H

#include "nw_fbox_formbox.h"
#include "nw_text_ucs2.h"
#include "nw_image_abstractimage.h"
#include "nw_hed_appservices.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */

typedef enum NW_FBox_ButtonBox_Type_e{
  NW_FBox_ButtonBox_Submit = 1,
  NW_FBox_ButtonBox_Reset,
  NW_FBox_ButtonBox_Button
}NW_FBox_ButtonBox_Type_t;

/* ------------------------------------------------------------------------ *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_ButtonBox_Class_s NW_FBox_ButtonBox_Class_t;
typedef struct NW_FBox_ButtonBox_s NW_FBox_ButtonBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_ButtonBox_ClassPart_s {
  void** unused;
} NW_FBox_ButtonBox_ClassPart_t;
  
struct NW_FBox_ButtonBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
  NW_FBox_ButtonBox_ClassPart_t NW_FBox_ButtonBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_ButtonBox_s {
  NW_FBox_FormBox_t super;

  /* member variables */
  NW_FBox_ButtonBox_Type_t buttonType;
  NW_Text_t* value;
  NW_HED_AppServices_t* appServices;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_ButtonBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_ButtonBox))

#define NW_FBox_ButtonBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_ButtonBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_ButtonBox_Class_t NW_FBox_ButtonBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_ButtonBox_GetValue(_buttonBox) \
  (NW_FBox_ButtonBoxOf(_buttonBox)->value)

#define NW_FBox_ButtonBox_GetButtonType(_buttonBox) \
  (NW_FBox_ButtonBoxOf(_buttonBox)->buttonType)

#define NW_FBox_ButtonBox_SetValue(_buttonBox, _value) \
  (NW_FBox_ButtonBoxOf(_buttonBox)->value = (_value))

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_ButtonBox_t*
NW_FBox_ButtonBox_New (NW_ADT_Vector_Metric_t numProperties,
                       NW_LMgr_EventHandler_t* eventHandler,
                       void* formCntrlId,
                       NW_FBox_FormLiaison_t* formLiaison,
                       NW_FBox_ButtonBox_Type_t buttonType,
                       NW_Text_t* value,
                       NW_Image_AbstractImage_t* image,
                       NW_HED_AppServices_t* appServices);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_BUTTONBOX_H */
