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


#ifndef NW_FBOX_EPOC32INPUTSKIN_H
#define NW_FBOX_EPOC32INPUTSKIN_H

#include "nw_fbox_inputskin.h"
#include "nw_fbox_formbox.h"
#include "nw_system_timer.h"

#include "nw_fbox_CEpoc32InputBox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
#define NW_FBOX_INPUTBOX_LINE_SPACING_IN_TWIPS        0x32
#define NW_FBOX_INPUTBOX_LINE_SPACING_IN_PIXELS       0x01

 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32InputSkin_Class_s NW_FBox_Epoc32InputSkin_Class_t;
typedef struct NW_FBox_Epoc32InputSkin_s NW_FBox_Epoc32InputSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Epoc32InputSkin_ClassPart_s {
  void** unused;
} NW_FBox_Epoc32InputSkin_ClassPart_t;
  
struct NW_FBox_Epoc32InputSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_InputSkin_ClassPart_t NW_FBox_InputSkin;
  NW_FBox_Epoc32InputSkin_ClassPart_t NW_FBox_Epoc32InputSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_Epoc32InputSkin_s {
  NW_FBox_InputSkin_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* textList;
  CEpoc32InputBox* cppEpoc32InputBox;
  CActiveSchedulerWait activeSchedulerWait;
  NW_Uint32  posCursorValue; /* current cursor position for this input element */
  NW_Uint32  prevCursorValue; /*previous cursor position - in case we cancel   */
  NW_Bool    reallyEdited; /* legal to use cursor position zero if already edited */
  NW_System_Timer_t* vkbTimer; /* delay launching of VKB */
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Epoc32InputSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Epoc32InputSkin))

#define NW_FBox_Epoc32InputSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_Epoc32InputSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Epoc32InputSkin_Class_t NW_FBox_Epoc32InputSkin_Class;


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */
void
NW_FBox_Epoc32InputSkin_Scroll( NW_FBox_Epoc32InputSkin_t* thisObj, 
                                TInt deltaY );

TBrowserStatusCode
NW_FBox_Epoc32InputSkin_GetViewRect(NW_FBox_Epoc32InputSkin_t* thisObj, 
                                    TRect& viewRect);

TBrowserStatusCode
NW_FBox_Epoc32InputSkin_TryExitEditMode(NW_FBox_Epoc32InputSkin_t* thisObj, 
                                     NW_Bool saveText);

TBrowserStatusCode
NW_FBox_Epoc32InputSkin_ReformatBox(NW_FBox_Epoc32InputSkin_t* inputSkin);

TBrowserStatusCode
NW_FBox_Epoc32InputSkin_Redraw(NW_FBox_Epoc32InputSkin_t* inputSkin);

TBrowserStatusCode
NW_FBox_Epoc32InputSkin_RedrawScreen(NW_FBox_Epoc32InputSkin_t* thisObj);

NW_Bool
NW_FBox_Epoc32InputSkin_TextChanged(NW_FBox_Epoc32InputSkin_t* inputSkin,
                                    const NW_Ucs2* string);

TBrowserStatusCode
NW_FBox_Epoc32InputSkin_SetValue(NW_FBox_Epoc32InputSkin_t* inputSkin, 
                                 const NW_Ucs2* string,
                                 NW_Text_Length_t length);


void
NW_FBox_Epoc32InputSkin_ClearTextList(NW_FBox_Epoc32InputSkin_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_Epoc32INPUTSKIN_H */
