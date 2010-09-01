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


#ifndef NW_FBOX_INPUTBOX_H
#define NW_FBOX_INPUTBOX_H

#include "nw_fbox_formbox.h"
#include "nw_text_ucs2.h"
#include "nw_fbox_keymapper.h"
#include "nw_fbox_editablestring.h"
#include "nw_fbox_clearaction.h"
#include "nw_fbox_okaction.h"
#include "nw_hed_appservices.h"
#include "NW_FBox_Validator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   constants
 * ------------------------------------------------------------------------- */
#define NW_FBox_InputBox_MaxCharacters (NW_Text_Length_t)0x00ffffff

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InputBox_Class_s NW_FBox_InputBox_Class_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

typedef
NW_Uint16
(*NW_FBox_InputBox_GetHeight_t) (NW_FBox_InputBox_t* thisObj);

typedef
void
(*NW_FBox_InputBox_SetHeight_t) (NW_FBox_InputBox_t* thisObj, NW_Uint16 height);

typedef 
TBrowserStatusCode
(*NW_FBox_InputBox_SetDisplayText_t)(NW_FBox_InputBox_t* thisObj,
                                      NW_Bool bufferModificationOn);

typedef
TBrowserStatusCode
(*NW_FBox_InputBox_ModifyBuffer_t)(NW_FBox_InputBox_t* inputBox, 
                                   NW_Ucs2 c, 
                                   NW_Uint32 repeated);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InputBox_ClassPart_s {
  NW_FBox_InputBox_GetHeight_t getHeight;
  NW_FBox_InputBox_SetHeight_t setHeight;
  NW_FBox_InputBox_SetDisplayText_t setDisplayText;
  NW_FBox_InputBox_ModifyBuffer_t modifyBuffer;
} NW_FBox_InputBox_ClassPart_t;
  
struct NW_FBox_InputBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
  NW_FBox_InputBox_ClassPart_t NW_FBox_InputBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_InputBox_s {
  NW_FBox_FormBox_t super;

  /* member variables */
  NW_Text_Length_t cursorPosition;
  NW_Text_Length_t maxChars;
  NW_Text_Length_t width;
  NW_Bool active;
  NW_Bool focus;
  NW_Bool readonly;
  NW_Bool disabled;
  NW_Bool IsMaxlength;
  NW_FBox_KeyMapper_Mode_t mode; 
  NW_Ucs2* title;
  NW_FBox_Validator_t* validator;
  NW_FBox_EditableString_t* editableString;
  NW_FBox_OkAction_t* okAction;
  NW_FBox_ClearAction_t* clearAction;
  NW_Text_t* value;
  NW_HED_AppServices_t* appServices;
  NW_Bool initialValueUsed;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InputBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_InputBox))

#define NW_FBox_InputBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_InputBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_InputBox_Class_t NW_FBox_InputBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_InputBox_GetMaxChars(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->maxChars)

#define NW_FBox_InputBox_GetMode(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->mode)

#define NW_FBox_InputBox_SetMode(_inputBox, _mode) \
  (NW_FBox_InputBoxOf(_inputBox)->mode = _mode)

#define NW_FBox_InputBox_GetValidator(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->validator)

#define NW_FBox_InputBox_SetValidator(_inputBox, _validator) \
  (NW_FBox_InputBoxOf(_inputBox)->validator = (_validator))

#define NW_FBox_InputBox_GetTitle(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->title)

#define NW_FBox_InputBox_SetTitle(_inputBox, _title) \
  (NW_FBox_InputBoxOf(_inputBox)->title = _title)

#define NW_FBox_InputBox_IsActive(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->active)

#define NW_FBox_InputBox_IsFocus( _inputBox ) \
  ( NW_FBox_InputBoxOf( _inputBox )->focus )

#define NW_FBox_InputBox_SetMaxChars(_inputBox, _maxChars) \
  (NW_FBox_InputBoxOf(_inputBox)->maxChars = _maxChars)

#define NW_FBox_InputBox_SetIsMaxlength(_inputBox, _val) \
  (NW_FBox_InputBoxOf(_inputBox)->IsMaxlength = _val)

#define NW_FBox_InputBox_GetIsMaxlength(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->IsMaxlength)

#define NW_FBox_InputBox_GetWidth(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->width)

#define NW_FBox_InputBox_SetWidth(_inputBox, _width) \
  (NW_FBox_InputBoxOf(_inputBox)->width = _width)

#define NW_FBox_InputBox_GetCursorPosition(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->cursorPosition)

#define NW_FBox_InputBox_GetEditableString(_inputBox) \
  (NW_FBox_InputBoxOf(_inputBox)->editableString)

#define NW_FBox_InputBox_SetCursorPosition(_inputBox, _cursorPosition) \
  (NW_FBox_InputBoxOf(_inputBox)->cursorPosition = _cursorPosition)

#define NW_FBox_InputBox_GetHeight(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputBox, getHeight) \
  ((_object)))

#define NW_FBox_InputBox_SetHeight(_object, _height) \
  (NW_Object_Invoke (_object, NW_FBox_InputBox, setHeight) \
  ((_object), (_height)))

#define NW_FBox_InputBox_SetDisplayText(_object, _bufferModificationOn) \
  (NW_Object_Invoke (_object, NW_FBox_InputBox, setDisplayText) \
  ((_object), (_bufferModificationOn)))

#define NW_FBox_InputBox_ModifyBuffer(_object, _c, _repeated) \
  (NW_Object_Invoke (_object, NW_FBox_InputBox, modifyBuffer) \
  ((_object), (_c), (_repeated)))

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_InputBox_AppendBuffer(NW_FBox_InputBox_t* inputBox, 
                              NW_Ucs2 c, 
                              NW_Uint32 repeated);

NW_FBOX_EXPORT
NW_Text_Length_t
NW_FBox_InputBox_GetBufferLength(NW_FBox_InputBox_t* inputBox);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_InputBox_CheckFormLiaisonVal(NW_FBox_InputBox_t* inputBox);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_InputBox_SetFormLiaisonVal(NW_FBox_InputBox_t* inputBox);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_InputBox_SetDefaultVal(NW_FBox_InputBox_t* inputBox, 
                               NW_Text_t* defaultVal);

NW_FBOX_EXPORT
void
NW_FBox_InputBox_SetActive(NW_FBox_InputBox_t* inputBox, NW_Bool active);

NW_FBOX_EXPORT
void
NW_FBox_InputBox_SetFocus( NW_FBox_InputBox_t* inputBox, NW_Bool active );

NW_FBOX_EXPORT
void
NW_FBox_InputBox_SetVKB( NW_FBox_InputBox_t* inputBox);


NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_InputBox_GetFormat(NW_FBox_InputBox_t* thisObj,
                           const NW_Text_t** format, 
                           NW_FBox_Validator_Mode_t*  mode,
                           NW_FBox_Validator_EmptyOk_t* emptyOk);

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_InputBox_t*
NW_FBox_InputBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_Uint16 columns,
                      NW_HED_AppServices_t* appServices);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
