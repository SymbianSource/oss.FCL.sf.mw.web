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


#ifndef NW_ADT_SEGMENTEDVECTOR_H
#define NW_ADT_SEGMENTEDVECTOR_H

#include "nw_adt_dynamicvector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_SegmentedVector_Class_s NW_ADT_SegmentedVector_Class_t;
typedef struct NW_ADT_SegmentedVector_s NW_ADT_SegmentedVector_t;

/* ------------------------------------------------------------------------- */
typedef
NW_Uint8*
(*NW_ADT_SegmentedVector_AllocateSegment_t) (NW_ADT_SegmentedVector_t* segmentedVector);

/* ------------------------------------------------------------------------- */
typedef struct NW_ADT_SegmentedVector_ClassPart_s {
  NW_ADT_Vector_Metric_t segmentListIncrement;
  NW_ADT_SegmentedVector_AllocateSegment_t allocateSegment;
} NW_ADT_SegmentedVector_ClassPart_t;

/* ------------------------------------------------------------------------- */

struct NW_ADT_SegmentedVector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Vector_ClassPart_t NW_ADT_Vector;
  NW_ADT_DynamicVector_ClassPart_t NW_ADT_DynamicVector;
  NW_ADT_SegmentedVector_ClassPart_t NW_ADT_SegmentedVector;
};

/* ------------------------------------------------------------------------- */
struct NW_ADT_SegmentedVector_s {
  NW_ADT_DynamicVector_t super;

  NW_ADT_Vector_Metric_t segmentSize;
  NW_Uint8** segmentList;
  NW_ADT_Vector_Metric_t segmentListSize;
  NW_ADT_Vector_Metric_t numSegments;
};

/* ------------------------------------------------------------------------- */
#define NW_ADT_SegmentedVector_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_ADT_SegmentedVector))

#define NW_ADT_SegmentedVectorOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_SegmentedVector))

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_SegmentedVector_Class_t NW_ADT_SegmentedVector_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_SegmentedVector_t*
NW_ADT_SegmentedVector_New (NW_ADT_Vector_Metric_t elementSize,
                            NW_ADT_Vector_Metric_t segmentSize);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_SEGMENTEDVECTOR_H */
