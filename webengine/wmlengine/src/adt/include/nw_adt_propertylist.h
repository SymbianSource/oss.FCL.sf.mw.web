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


#ifndef NW_ADT_PROPERTYLIST_H
#define NW_ADT_PROPERTYLIST_H

#include "nw_adt_valuelist.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_PropertyList_Class_s NW_ADT_PropertyList_Class_t;
typedef struct NW_ADT_PropertyList_s NW_ADT_PropertyList_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef NW_Uint8 NW_ADT_PropertyName_t;
typedef NW_ADT_ValueType_t NW_ADT_PropertyType_t;
typedef NW_ADT_Token_t NW_ADT_PropertyValueToken_t;
typedef NW_ADT_ValueList_Entry_t NW_ADT_Property_t;
typedef NW_ADT_Value_t NW_ADT_PropertyValue_t;

/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
enum {
  NW_ADT_PropertyType_Token = NW_ADT_ValueType_Token,
  NW_ADT_PropertyType_Integer = NW_ADT_ValueType_Integer,
  NW_ADT_PropertyType_Decimal = NW_ADT_ValueType_Decimal,
  NW_ADT_PropertyType_Pointer = NW_ADT_ValueType_Pointer,
  NW_ADT_PropertyType_Object = NW_ADT_ValueType_Object,
  NW_ADT_PropertyType_Text = NW_ADT_ValueType_Text,
  NW_ADT_PropertyType_Percent = NW_ADT_ValueType_Percent,
  NW_ADT_PropertyType_Em = NW_ADT_ValueType_Em,
  NW_ADT_PropertyType_Ex = NW_ADT_ValueType_Ex
};

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_PropertyList_ClassPart_s {
  void** unused;
} NW_ADT_PropertyList_ClassPart_t;

struct NW_ADT_PropertyList_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_ValueList_ClassPart_t NW_ADT_ValueList;
  NW_ADT_PropertyList_ClassPart_t NW_ADT_PropertyList;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ADT_PropertyList_s {
  NW_ADT_ValueList_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ADT_PropertyList_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_PropertyList))

#define NW_ADT_PropertyListOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_PropertyList))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_PropertyList_Class_t NW_ADT_PropertyList_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT 
TBrowserStatusCode
NW_ADT_PropertyList_Get (const NW_ADT_PropertyList_t* thisObj,
                         NW_ADT_PropertyName_t name,
                         NW_ADT_Property_t* property);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_PropertyList_Set (const NW_ADT_PropertyList_t* thisObj,
                         NW_ADT_PropertyName_t name,
                         const NW_ADT_Property_t* property);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_PropertyList_Remove (const NW_ADT_PropertyList_t* thisObj,
                            NW_ADT_PropertyName_t name);

NW_ADT_EXPORT
TBrowserStatusCode
NW_ADT_PropertyList_Copy (const NW_ADT_PropertyList_t* thisObj,
                          NW_ADT_PropertyList_t* dest);


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_PropertyList_t*
NW_ADT_PropertyList_New (NW_ADT_Vector_Metric_t capacity,
                         NW_ADT_Vector_Metric_t increment);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_PROPERTYLIST_H */
