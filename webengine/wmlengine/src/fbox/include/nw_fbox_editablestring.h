/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_EDITABLESTRING_H
#define NW_FBOX_EDITABLESTRING_H

#include "nw_adt_resizablevector.h"
#include "nw_text_ucs2.h"
#include "NW_FBox_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- */
typedef struct NW_FBox_EditableString_Class_s NW_FBox_EditableString_Class_t;
typedef struct NW_FBox_EditableString_s NW_FBox_EditableString_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_FBox_EditableString_ClassPart_s {
  void** unused;
} NW_FBox_EditableString_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_FBox_EditableString_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Vector_ClassPart_t NW_ADT_Vector;
  NW_ADT_DynamicVector_ClassPart_t NW_ADT_DynamicVector;
  NW_ADT_ResizableVector_ClassPart_t NW_ADT_ResizableVector;
  NW_FBox_EditableString_ClassPart_t NW_FBox_EditableString;
};

/* ------------------------------------------------------------------------- */
struct NW_FBox_EditableString_s {
  NW_ADT_ResizableVector_t super;

  /* member variables */
};

/* ------------------------------------------------------------------------- */
#define NW_FBox_EditableString_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_EditableString))

#define NW_FBox_EditableStringOf(_object) \
  (NW_Object_Cast (_object, NW_FBox_EditableString))


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_EditableString_Class_t NW_FBox_EditableString_Class;

/* ------------------------------------------------------------------------- *
   final functions
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
NW_Ucs2*
NW_FBox_EditableString_GetString (NW_FBox_EditableString_t* editableString,
                                  NW_ADT_Vector_Metric_t index,
                                  NW_Text_Length_t* length);

NW_FBOX_EXPORT
NW_Text_UCS2_t*
NW_FBox_EditableString_GetText (NW_FBox_EditableString_t* editableString,
                                NW_ADT_Vector_Metric_t index);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_EditableString_SetInitialString (NW_FBox_EditableString_t* thisObj,
                                         const NW_Ucs2* string,
                                         NW_Text_Length_t length);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_EditableString_SetInitialText (NW_FBox_EditableString_t* editableString,
                                       const NW_Text_t* text);

NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_EditableString_InsertChar (NW_FBox_EditableString_t* editableString,
                                   NW_Ucs2 character,
                                   NW_ADT_Vector_Metric_t position);
NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_EditableString_ReplaceChar (NW_FBox_EditableString_t* editableString,
                                    NW_Ucs2 character,
                                    NW_ADT_Vector_Metric_t position);
NW_FBOX_EXPORT
TBrowserStatusCode
NW_FBox_EditableString_ClearChar (NW_FBox_EditableString_t* editableString,
                                  NW_ADT_Vector_Metric_t position);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
NW_FBox_EditableString_t*
NW_FBox_EditableString_New (NW_ADT_Vector_Metric_t elementSize,
                            NW_ADT_Vector_Metric_t initialCapacity,
                            NW_ADT_Vector_Metric_t capacityIncrement);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_EDITABLESTRING_H */
