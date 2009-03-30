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


#ifndef NW_LMGR_PROPERTYLIST_H
#define NW_LMGR_PROPERTYLIST_H

#include "nw_object_dynamic.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_adt_types.h"
#include "NW_LMgr_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_PropertyList_Class_s NW_LMgr_PropertyList_Class_t;
typedef struct NW_LMgr_PropertyList_s NW_LMgr_PropertyList_t;

/* ------------------------------------------------------------------------- *
   value types
 * ------------------------------------------------------------------------- */
typedef NW_Uint8 NW_LMgr_PropertyValueType_t;
#define NW_CSS_ValueType_Integer     (NW_Uint8)1        /* integer */
#define NW_CSS_ValueType_Number      (NW_Uint8)2        /* decimal */
#define NW_CSS_ValueType_Px          (NW_Uint8)3        /* integer */
#define NW_CSS_ValueType_Em          (NW_Uint8)4        /* decimal */
#define NW_CSS_ValueType_Ex          (NW_Uint8)5        /* decimal */
#define NW_CSS_ValueType_In          (NW_Uint8)6        /* decimal */
#define NW_CSS_ValueType_Cm          (NW_Uint8)7        /* decimal */
#define NW_CSS_ValueType_Mm          (NW_Uint8)8        /* decimal */
#define NW_CSS_ValueType_Pt          (NW_Uint8)9        /* decimal */
#define NW_CSS_ValueType_Pc          (NW_Uint8)10       /* decimal */
#define NW_CSS_ValueType_Percentage  (NW_Uint8)11       /* decimal */
#define NW_CSS_ValueType_Token       (NW_Uint8)12       /* integer */
#define NW_CSS_ValueType_Color       (NW_Uint8)13       /* integer */
/* note that no NW_CSS_ValueType other than object derived types may have a
   value greater than NW_CSS_ValueType_Object */
#define NW_CSS_ValueType_Object      (NW_Uint8)14       /* object  */
#define NW_CSS_ValueType_Text        (NW_Uint8)15       /* object  */
#define NW_CSS_ValueType_Image       (NW_Uint8)16       /* object  */
#define NW_CSS_ValueType_Font        (NW_Uint8)17       /* object  */
#define NW_CSS_ValueType_Copy        (NW_Uint8) 0x80
//#define NW_CSS_ValueType_Mask        (NW_Uint8) 0x7f
#define NW_CSS_ValueType_DefaultStyle  (NW_Uint8) 0x40
#define NW_CSS_ValueType_Mask          (NW_Uint8) 0x3f

/* ------------------------------------------------------------------------- *
   pseudo classes
 * ------------------------------------------------------------------------- */
#define NW_CSS_Pseudo_None           (NW_LMgr_PropertyName_t)0x0100
#define NW_CSS_Pseudo_Link           (NW_LMgr_PropertyName_t)0x0200
#define NW_CSS_Pseudo_Visited        (NW_LMgr_PropertyName_t)0x0400
#define NW_CSS_Pseudo_Hover          (NW_LMgr_PropertyName_t)0x0800
#define NW_CSS_Pseudo_Active         (NW_LMgr_PropertyName_t)0x1000
#define NW_CSS_Pseudo_Focus          (NW_LMgr_PropertyName_t)0x2000
#define NW_CSS_Pseudo_FirstLine      (NW_LMgr_PropertyName_t)0x4000

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef NW_Uint32 NW_LMgr_PropertyValueToken_t;
typedef NW_Uint16 NW_LMgr_PropertyName_t;

typedef union NW_LMgr_PropertyValue_s {
  NW_Int32 integer;
  NW_LMgr_PropertyValueToken_t token;
  NW_Float32 decimal;
  NW_Object_t* object;
} NW_LMgr_PropertyValue_t;

typedef struct NW_LMgr_PropertyList_Entry_s {
  NW_LMgr_PropertyValue_t value;
  NW_Uint8 type;
} NW_LMgr_PropertyList_Entry_t;

typedef NW_LMgr_PropertyList_Entry_t NW_LMgr_Property_t;

/* ------------------------------------------------------------------------- *
   virtual method definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_LMgr_PropertyList_Get_t) (const NW_LMgr_PropertyList_t* propertyList,
                               NW_LMgr_PropertyName_t key,
                               NW_LMgr_PropertyList_Entry_t* entry);

typedef
TBrowserStatusCode
(*NW_LMgr_PropertyList_Set_t) (NW_LMgr_PropertyList_t* propertyList,
                               NW_LMgr_PropertyName_t key,
                               const NW_LMgr_PropertyList_Entry_t* entry);

typedef
TBrowserStatusCode 
(*NW_LMgr_PropertyList_Remove_t) (NW_LMgr_PropertyList_t* propertyList,
                                  NW_LMgr_PropertyName_t key);

typedef
NW_LMgr_PropertyList_t*
(*NW_LMgr_PropertyList_Clone_t) (const NW_LMgr_PropertyList_t* propertyList);

typedef
TBrowserStatusCode 
(*NW_LMgr_PropertyList_Clear_t) (NW_LMgr_PropertyList_t* propertyList) ;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_PropertyList_ClassPart_s {
  NW_LMgr_PropertyList_Get_t get;
  NW_LMgr_PropertyList_Set_t set;
  NW_LMgr_PropertyList_Remove_t remove;
  NW_LMgr_PropertyList_Clone_t clone;
  NW_LMgr_PropertyList_Clear_t clear;
} NW_LMgr_PropertyList_ClassPart_t;

struct NW_LMgr_PropertyList_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_PropertyList_ClassPart_t NW_LMgr_PropertyList;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_PropertyList_s {
  NW_Object_Dynamic_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_PropertyList_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_PropertyList))

#define NW_LMgr_PropertyListOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_PropertyList))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_PropertyList_Class_t NW_LMgr_PropertyList_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_PropertyList_GetSize(_object) \
  (NW_ADT_Vector_GetSize (NW_LMgr_PropertyListOf (_object)->list))

#define NW_LMgr_PropertyList_Get(_object, _key, _entry) \
  (NW_Object_Invoke (_object, NW_LMgr_PropertyList, get) \
     (NW_LMgr_PropertyListOf (_object), (_key), (_entry)))

#define NW_LMgr_PropertyList_Set(_object, _key, _entry) \
  (NW_Object_Invoke (_object, NW_LMgr_PropertyList, set) \
     (NW_LMgr_PropertyListOf (_object), (_key), (_entry)))

#define NW_LMgr_PropertyList_Remove(_object, _key) \
  (NW_Object_Invoke (_object, NW_LMgr_PropertyList, remove) \
     (NW_LMgr_PropertyListOf (_object), (_key)))

#define NW_LMgr_PropertyList_Clone(_propertyList) \
  (NW_Object_Invoke (_propertyList, NW_LMgr_PropertyList, clone) \
     (NW_LMgr_PropertyListOf (_propertyList)))

#define NW_LMgr_PropertyList_Clear(_object) \
  (NW_Object_Invoke (_object, NW_LMgr_PropertyList, clear) \
     (NW_LMgr_PropertyListOf (_object)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_PROPERTYLIST_H */
