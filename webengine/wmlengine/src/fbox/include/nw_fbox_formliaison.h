/*
* Copyright (c) 2000, 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBox_FormLiaison_h
#define NW_FBox_FormLiaison_h

#include <e32std.h>

#include "nw_object_dynamic.h"
#include "nw_evt_ecmaevent.h"
#include "nw_lmgr_box.h"
#include "NW_FBox_EXPORT.h"
#include "nw_hed_documentroot.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FormLiaison_Class_s NW_FBox_FormLiaison_Class_t;
typedef struct NW_FBox_FormLiaison_s NW_FBox_FormLiaison_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_GetWBXMLVersion_t) (NW_FBox_FormLiaison_t* formLiaison,
																					NW_Uint32* version);


typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_AddControl_t) (NW_FBox_FormLiaison_t* formLiaison,
                                     void* controlId);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_SetInitialStringValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                                void* controlId, 
																								NW_Text_Length_t maxChars);
typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_SetStringValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                         void* controlId,
                                         NW_Text_t* value);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_ValidateStringValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                              void* controlId,
                                              const NW_Text_t* value);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_GetStringValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                         void* controlId,
                                         NW_Text_t** valueOut,
                                         NW_Bool*    initialValueUsed);
 

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_GetStringName_t) (NW_FBox_FormLiaison_t* formLiaison,
                                         void* controlId,
                                         NW_Ucs2** nameOut);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_GetStringTitle_t) (NW_FBox_FormLiaison_t* formLiaison,
                                         void* controlId,
                                         NW_Ucs2** titleOut);
 
typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_SetBoolValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                       void* controlId,
                                       NW_Bool *value);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_ResetRadioValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                          void* controlId,
                                          NW_Bool *value);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_ToggleBoolValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                          void* controlId);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_GetBoolValue_t) (NW_FBox_FormLiaison_t* formLiaison,
                                       void* controlId,
                                       NW_Bool* valueOut);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_Reset_t) (NW_FBox_FormLiaison_t* formLiaison,
                                void* _controlId);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_Submit_t) (NW_FBox_FormLiaison_t* formLiaison,
                                 void* controlId);

typedef
TBrowserStatusCode
( *NW_FBox_FormLiaison_Focus_t ) ( NW_FBox_FormLiaison_t* formLiaison,
                                   void* controlId );

typedef
NW_Bool
(*NW_FBox_FormLiaison_IsOptionMultiple_t) (NW_FBox_FormLiaison_t* formLiaison,
                                           void* controlId);

typedef
NW_Bool
(*NW_FBox_FormLiaison_IsOptionSelected_t) (NW_FBox_FormLiaison_t* formLiaison,
                                           void* controlId);

typedef
NW_Bool
(*NW_FBox_FormLiaison_OptionHasOnPick_t) (NW_FBox_FormLiaison_t* formLiaison,
                                          void* controlId);

typedef
NW_Bool
(*NW_FBox_FormLiaison_IsLocalNavOnPick_t) (NW_FBox_FormLiaison_t* formLiaison,
                                          void* controlId);

typedef
NW_Bool
(*NW_FBox_FormLiaison_GetInitialValue_t) (NW_FBox_FormLiaison_t* formLiaison,
																					void* controlId);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_GetDocRoot_t) (NW_FBox_FormLiaison_t* formLiaison,
                                     NW_HED_DocumentRoot_t** contentHandler);

typedef
TBrowserStatusCode
(*NW_FBox_FormLiaison_DelegateEcmaEvnt_t) (NW_FBox_FormLiaison_t* formLiaison,
                                           void* controlId,
                                           NW_ECMA_Evt_Type_t ecmaEvent);


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FormLiaison_ClassPart_s {
  NW_FBox_FormLiaison_GetWBXMLVersion_t getWBXMLVersion;
  NW_FBox_FormLiaison_AddControl_t addControl;
  NW_FBox_FormLiaison_SetInitialStringValue_t setInitialStringValue;
  NW_FBox_FormLiaison_SetStringValue_t setStringValue;
  NW_FBox_FormLiaison_ValidateStringValue_t validateStringValue;
  NW_FBox_FormLiaison_GetStringValue_t getStringValue;
  NW_FBox_FormLiaison_GetStringName_t getStringName;
  NW_FBox_FormLiaison_GetStringTitle_t getStringTitle;
  NW_FBox_FormLiaison_SetBoolValue_t setBoolValue;
  NW_FBox_FormLiaison_ResetRadioValue_t resetRadioValue;
  NW_FBox_FormLiaison_ToggleBoolValue_t toggleBoolValue;
  NW_FBox_FormLiaison_GetBoolValue_t getBoolValue;
  NW_FBox_FormLiaison_Reset_t reset;
  NW_FBox_FormLiaison_Submit_t submit;
  NW_FBox_FormLiaison_Focus_t focus;
  NW_FBox_FormLiaison_IsOptionMultiple_t isOptionMultiple;
  NW_FBox_FormLiaison_IsOptionSelected_t isOptionSelected;
  NW_FBox_FormLiaison_OptionHasOnPick_t optionHasOnPick;
  NW_FBox_FormLiaison_IsLocalNavOnPick_t isLocalNavOnPick;
  NW_FBox_FormLiaison_GetInitialValue_t getInitialValue;
  NW_FBox_FormLiaison_GetDocRoot_t getDocRoot;
  NW_FBox_FormLiaison_DelegateEcmaEvnt_t delegateEcmaEvent;
} NW_FBox_FormLiaison_ClassPart_t;

struct NW_FBox_FormLiaison_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_FormLiaison_ClassPart_t NW_FBox_FormLiaison;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_FBox_FormLiaison_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  HBufC* iMostRecentFileSelectionPath;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_FormLiaison_GetClassPart(_formLiaison) \
  (NW_Object_GetClassPart(_formLiaison, NW_FBox_FormLiaison))

#define NW_FBox_FormLiaisonOf(object) \
  (NW_Object_Cast (object, NW_FBox_FormLiaison))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_FormLiaison_Class_t NW_FBox_FormLiaison_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_FormLiaison_GetWBXMLVersion(_formLiaison, _version) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getWBXMLVersion) \
     ((_formLiaison), (_version)))

#define NW_FBox_FormLiaison_SetBoxTree(_formLiaison, _boxTree) \
  (NW_FBox_FormLiaisonOf (_formLiaison)->formBoxTree = NW_LMgr_BoxOf (_boxTree), KBrsrSuccess)

#define NW_FBox_FormLiaison_GetBoxTree(_formLiaison) \
  (NW_OBJECT_CONSTCAST(NW_LMgr_Box_t*) NW_FBox_FormLiaisonOf (_formLiaison)->formBoxTree)

#define NW_FBox_FormLiaison_AddControl(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, addControl) \
     ((_formLiaison), (_controlId)))

#define NW_FBox_FormLiaison_SetInitialStringValue(_formLiaison, _controlId, _maxChars) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, setInitialStringValue) \
     ((_formLiaison), (_controlId), (_maxChars)))

#define NW_FBox_FormLiaison_SetStringValue(_formLiaison, _controlId, _value) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, setStringValue) \
     ((_formLiaison), (_controlId), (_value)))

#define NW_FBox_FormLiaison_ValidateStringValue(_formLiaison, _controlId, _value) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, validateStringValue) \
     ((_formLiaison), (_controlId), (_value)))

#define NW_FBox_FormLiaison_GetStringValue(_formLiaison, _controlId, _valueOut, _valueUsedOut) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getStringValue) \
     ((_formLiaison), (_controlId), (_valueOut), (_valueUsedOut)))

#define NW_FBox_FormLiaison_GetStringName(_formLiaison, _controlId, _nameOut) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getStringName) \
     ((_formLiaison), (_controlId), (_nameOut)))

#define NW_FBox_FormLiaison_GetStringTitle(_formLiaison, _controlId, _titleOut) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getStringTitle) \
     ((_formLiaison), (_controlId), (_titleOut)))

#define NW_FBox_FormLiaison_SetBoolValue(_formLiaison, _controlId, _value) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, setBoolValue) \
     ((_formLiaison), (_controlId), (_value)))

#define NW_FBox_FormLiaison_ResetRadioValue(_formLiaison, _controlId, _value) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, resetRadioValue) \
     ((_formLiaison), (_controlId), (_value)))

#define NW_FBox_FormLiaison_ToggleBoolValue(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, toggleBoolValue) \
     ((_formLiaison), (_controlId)))

#define NW_FBox_FormLiaison_GetBoolValue(_formLiaison, _controlId, _valueOut) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getBoolValue) \
     ((_formLiaison), (_controlId), (_valueOut)))

#define NW_FBox_FormLiaison_Reset(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, reset) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_Submit(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, submit) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_Focus( _formLiaison, _controlId ) \
  ( NW_Object_Invoke ( _formLiaison, NW_FBox_FormLiaison, focus ) \
     ( ( _formLiaison ), ( _controlId ) ) )

#define NW_FBox_FormLiaison_IsOptionMultiple(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, isOptionMultiple) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_IsOptionSelected(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, isOptionSelected) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_OptionHasOnPick(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, optionHasOnPick) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_IsLocalNavOnPick(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, isLocalNavOnPick) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_GetInitialValue(_formLiaison, _controlId) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getInitialValue) \
     ((_formLiaison), (_controlId) ))

#define NW_FBox_FormLiaison_GetDocRoot(_formLiaison, _docRoot) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, getDocRoot) \
     ((_formLiaison), (_docRoot) ))

#define NW_FBox_FormLiaison_DelegateEcmaEvent(_formLiaison, _controlId, _ecmaEvent) \
  (NW_Object_Invoke (_formLiaison, NW_FBox_FormLiaison, delegateEcmaEvent) \
    ((_formLiaison), (_controlId), (_ecmaEvent) ))


/* -------------------------------------------------------------------------- *
    final methods
 * ---------------------------------------------------------------------------*/

/**
* Gets the most-recent-file-selection-path.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aPath Ptr into which the most-recent-file-selection-path is returned.
* @return Browser status code, indicating status of operation:
*           KBrsrSuccess
*/
extern
TBrowserStatusCode
NW_FBox_FormLiaison_GetMostRecentFileSelectionPath(
    NW_FBox_FormLiaison_t* aThisObj, TPtrC& aPath );

/**
* Sets the most-recent-file-selection-path.
* @since 2.6
* @param aThisObj Ptr to the OOC class structure for this class.
* @param aPath Ptr to the most-recent-file-selection-path.
* @return Browser status code, indicating status of operation:
*           KBrsrSuccess
*           KBrsrOutOfMemory
*/
extern
TBrowserStatusCode
NW_FBox_FormLiaison_SetMostRecentFileSelectionPath(
    NW_FBox_FormLiaison_t* aThisObj, TPtrC& aPath );
   

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Tempest_FBox_FBoxFormLiaison_h */
