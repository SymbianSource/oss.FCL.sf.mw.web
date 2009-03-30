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


#ifndef NW_ADT_TEXTMAP_H
#define NW_ADT_TEXTMAP_H

#include "nw_adt_map.h"
#include "nw_adt_dynamicvector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_TextMap_Class_s NW_ADT_TextMap_Class_t;
typedef struct NW_ADT_TextMap_s NW_ADT_TextMap_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_TextMap_ClassPart_s {
  void** unused;
} NW_ADT_TextMap_ClassPart_t;

struct NW_ADT_TextMap_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Map_ClassPart_t NW_ADT_Map;
  NW_ADT_TextMap_ClassPart_t NW_ADT_TextMap;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ADT_TextMap_s {
  NW_ADT_Map_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ADT_TextMap_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_TextMap))

#define NW_ADT_TextMapOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_TextMap))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_TextMap_Class_t NW_ADT_TextMap_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_TextMap_t*
NW_ADT_TextMap_New (NW_Uint32 valueSize,
                    NW_ADT_DynamicVector_t* dynamicVector);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_TEXTMAP_H */
