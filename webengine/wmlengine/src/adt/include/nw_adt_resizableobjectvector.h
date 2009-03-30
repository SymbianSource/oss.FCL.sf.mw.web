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


#ifndef NW_ADT_RESIZABLEOBJECTVECTOR_H
#define NW_ADT_RESIZABLEOBJECTVECTOR_H

#include "nw_adt_resizablevector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_ResizableObjectVector_Class_s
               NW_ADT_ResizableObjectVector_Class_t;
typedef struct NW_ADT_ResizableObjectVector_s
               NW_ADT_ResizableObjectVector_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_ResizableObjectVector_ClassPart_s {
  void** unused;
} NW_ADT_ResizableObjectVector_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_ADT_ResizableObjectVector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Vector_ClassPart_t NW_ADT_Vector;
  NW_ADT_DynamicVector_ClassPart_t NW_ADT_DynamicVector;
  NW_ADT_ResizableVector_ClassPart_t NW_ADT_ResizableVector;
  NW_ADT_ResizableObjectVector_ClassPart_t NW_ADT_ResizableObjectVector;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_ResizableObjectVector_s {
  NW_ADT_ResizableVector_t super;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_ResizableObjectVector_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_ResizableObjectVector))

#define NW_ADT_ResizableObjectVectorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_ResizableObjectVector))

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const
NW_ADT_ResizableObjectVector_Class_t NW_ADT_ResizableObjectVector_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_ResizableObjectVector_t*
NW_ADT_ResizableObjectVector_New (NW_ADT_Vector_Metric_t initialCapacity,
                                  NW_ADT_Vector_Metric_t capacityIncrement);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_RESIZABLEOBJECTVECTOR_H */
