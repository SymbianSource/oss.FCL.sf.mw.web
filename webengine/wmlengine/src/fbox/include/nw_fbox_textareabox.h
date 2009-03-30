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


#ifndef NW_FBOX_TEXTAREABOX_H
#define NW_FBOX_TEXTAREABOX_H

#include "nw_fbox_inputbox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_TextAreaBox_Class_s NW_FBox_TextAreaBox_Class_t;
typedef struct NW_FBox_TextAreaBox_s NW_FBox_TextAreaBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_TextAreaBox_ClassPart_s {
  void** unused;
} NW_FBox_TextAreaBox_ClassPart_t;
  
struct NW_FBox_TextAreaBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
  NW_FBox_InputBox_ClassPart_t NW_FBox_InputBox;
  NW_FBox_TextAreaBox_ClassPart_t NW_FBox_TextAreaBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_TextAreaBox_s {
  NW_FBox_InputBox_t super;

  /* member variables */
  NW_Uint16 height;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_TextAreaBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_TextAreaBox))

#define NW_FBox_TextAreaBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_TextAreaBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_TextAreaBox_Class_t NW_FBox_TextAreaBox_Class;

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_TextAreaBox_t*
NW_FBox_TextAreaBox_New (NW_ADT_Vector_Metric_t numProperties,
                         NW_LMgr_EventHandler_t* eventHandler,
                         void* formCntrlId,
                         NW_FBox_FormLiaison_t* formLiaison,
                         NW_Uint16 columns,
                         NW_HED_AppServices_t* appServices,
                         NW_Uint16 rows);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_TEXTAREABOX_H */
