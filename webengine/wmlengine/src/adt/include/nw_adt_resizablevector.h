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


#ifndef NW_ADT_RESIZABLEVECTOR_H
#define NW_ADT_RESIZABLEVECTOR_H

#include "nw_adt_dynamicvector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_ResizableVector_Class_s NW_ADT_ResizableVector_Class_t;
typedef struct NW_ADT_ResizableVector_s NW_ADT_ResizableVector_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_ResizableVector_ClassPart_s {
  void** unused;
} NW_ADT_ResizableVector_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_ResizableVector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Vector_ClassPart_t NW_ADT_Vector;
  NW_ADT_DynamicVector_ClassPart_t NW_ADT_DynamicVector;
  NW_ADT_ResizableVector_ClassPart_t NW_ADT_ResizableVector;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_ResizableVector_s {
  NW_ADT_DynamicVector_t super;

  /* member variables */
  NW_ADT_Vector_Metric_t increment;
  NW_Uint8* array;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_ResizableVector_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_ResizableVector))

#define NW_ADT_ResizableVectorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_ResizableVector))

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_ResizableVector_Class_t NW_ADT_ResizableVector_Class;

/* ------------------------------------------------------------------------- *
   publice methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_ResizableVector_GetIncrement(_object) \
  (NW_OBJECT_CONSTCAST(NW_ADT_Vector_Metric_t) NW_ADT_ResizableVectorOf (_object)->increment) 
/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_ResizableVector_t*
NW_ADT_ResizableVector_New (NW_ADT_Vector_Metric_t elementSize,
                            NW_ADT_Vector_Metric_t initialCapacity,
                            NW_ADT_Vector_Metric_t capacityIncrement);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_RESIZABLEVECTOR_H */
