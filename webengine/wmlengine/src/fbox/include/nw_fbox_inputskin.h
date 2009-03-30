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


#ifndef NW_FBOX_INPUTSKIN_H
#define NW_FBOX_INPUTSKIN_H

#include "nw_fbox_buttonboxskin.h"
#include "nw_fbox_inputbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InputSkin_Class_s NW_FBox_InputSkin_Class_t;
typedef struct NW_FBox_InputSkin_s NW_FBox_InputSkin_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

typedef
TBrowserStatusCode
(*NW_FBox_InputSkin_DrawText_t)(NW_FBox_InputSkin_t* thisObj,
                                CGDIDeviceContext* deviceContext);

typedef
TBrowserStatusCode
(*NW_FBox_InputSkin_DrawCursor_t)(NW_FBox_InputSkin_t* thisObj,
                                CGDIDeviceContext* deviceContext,
                                NW_GDI_Point2D_t *start);
typedef
TBrowserStatusCode
(*NW_FBox_InputSkin_MoveCursorLeft_t)(NW_FBox_InputSkin_t* thisObj);

typedef
TBrowserStatusCode
(*NW_FBox_InputSkin_MoveCursorRight_t)(NW_FBox_InputSkin_t* thisObj);

typedef
TBrowserStatusCode
(*NW_FBox_InputSkin_ClearLastChar_t)(NW_FBox_InputSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_InputSkin_SetDisplayText_t)(NW_FBox_InputSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_InputSkin_SetDefaultVal_t)(NW_FBox_InputSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_InputSkin_SetActive_t)(NW_FBox_InputSkin_t* thisObj);

typedef 
TBrowserStatusCode
( *NW_FBox_InputSkin_SetFocus_t )( NW_FBox_InputSkin_t* thisObj, NW_Bool focus );

typedef 
TBrowserStatusCode
(*NW_FBox_InputSkin_DetailsDialog_t)(NW_FBox_InputSkin_t* thisObj);

typedef 
TBrowserStatusCode
(*NW_FBox_InputSkin_Split_t)(NW_FBox_InputSkin_t* thisObj,
                             NW_GDI_Metric_t space,
                             NW_LMgr_Box_t* *splitBox,
                             NW_Uint8 flags);

typedef 
TBrowserStatusCode
(*NW_FBox_InputSkin_SetVKB_t)(NW_FBox_InputSkin_t* thisObj);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_InputSkin_ClassPart_s {
  NW_FBox_InputSkin_DrawText_t drawText;
  NW_FBox_InputSkin_MoveCursorLeft_t moveCursorLeft;
  NW_FBox_InputSkin_MoveCursorRight_t moveCursorRight;
  NW_FBox_InputSkin_ClearLastChar_t clearLastChar;
  NW_FBox_InputSkin_SetDisplayText_t setDisplayText;
  NW_FBox_InputSkin_SetActive_t setActive;
  NW_FBox_InputSkin_SetFocus_t setFocus;
  NW_FBox_InputSkin_DetailsDialog_t detailsDialog;
  NW_FBox_InputSkin_Split_t split;
  NW_FBox_InputSkin_SetVKB_t setVKB;
} NW_FBox_InputSkin_ClassPart_t;
  
struct NW_FBox_InputSkin_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Skin_ClassPart_t NW_FBox_Skin;
  NW_FBox_InputSkin_ClassPart_t NW_FBox_InputSkin;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_InputSkin_s {
  NW_FBox_ButtonBoxSkin_t super;

  /* member variables */
  NW_Text_t* displayText;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_InputSkin_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_InputSkin))

#define NW_FBox_InputSkinOf(object) \
  (NW_Object_Cast (object, NW_FBox_InputSkin))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_InputSkin_Class_t NW_FBox_InputSkin_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_InputSkin_DrawText(_object, _deviceContext) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, drawText) \
  ((_object), (_deviceContext)))

#define NW_FBox_InputSkin_DrawCursor(_object, _deviceContext, _start) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, drawCursor) \
  ((_object), (_deviceContext), (_start)))

#define NW_FBox_InputSkin_MoveCursorLeft(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, moveCursorLeft) \
  ((_object)))

#define NW_FBox_InputSkin_MoveCursorRight(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, moveCursorRight) \
  ((_object)))

#define NW_FBox_InputSkin_ClearLastChar(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, clearLastChar) \
  ((_object)))

#define NW_FBox_InputSkin_SetDisplayText(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, setDisplayText) \
  ((_object)))

#define NW_FBox_InputSkin_SetActive(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, setActive) \
  ((_object)))

#define NW_FBox_InputSkin_SetFocus( _object, _focus ) \
  ( NW_Object_Invoke( _object, NW_FBox_InputSkin, setFocus ) \
  ( ( _object ), ( _focus ) ) )

#define NW_FBox_InputSkin_DetailsDialog(_object) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, detailsDialog) \
  ((_object)))

#define NW_FBox_InputSkin_Split(_object, _space, _splitBox, _flags) \
  (NW_Object_Invoke (_object, NW_FBox_InputSkin, split) \
  ((_object), (_space), (_splitBox), (_flags)))

#define NW_FBox_InputSkin_SetVKB( _object) \
  ( NW_Object_Invoke( _object, NW_FBox_InputSkin, setVKB) \
  ( ( _object )) )

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_InputSkin_GetMCharExtent(NW_FBox_InputSkin_t* skin,
                                 NW_GDI_Dimension2D_t *charExtent);
        
/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_InputSkin_t*
NW_FBox_InputSkin_New(NW_FBox_FormBox_t* formBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INPUTSKIN_H */
