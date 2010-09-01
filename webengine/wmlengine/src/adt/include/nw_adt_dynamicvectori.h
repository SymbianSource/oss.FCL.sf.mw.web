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


#ifndef NW_ADT_DYNAMICVECTORI_H
#define NW_ADT_DYNAMICVECTORI_H

#include "nw_adt_vectori.h"
#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_DynamicVector_ResizeCapacity(_dynamicVector, _capacityNeeded) \
  (NW_Object_Invoke(_dynamicVector, NW_ADT_DynamicVector, resizeCapacity) \
   ((_dynamicVector), (_capacityNeeded)))

#define NW_ADT_DynamicVector_MoveElements(_dynamicVector, _srcIndex, _dstIndex) \
  (NW_Object_Invoke(_dynamicVector, NW_ADT_DynamicVector, moveElements) \
   ((_dynamicVector), (_srcIndex), (_dstIndex)))

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_DynamicVector_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp);
NW_ADT_EXPORT
NW_ADT_Vector_Metric_t
_NW_ADT_DynamicVector_GetElementSize (const NW_ADT_Vector_t* vector);

NW_ADT_EXPORT
NW_ADT_Vector_Metric_t
_NW_ADT_DynamicVector_GetSize (const NW_ADT_Vector_t* vector);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_DynamicVector_MoveElements (NW_ADT_DynamicVector_t* dynamicVector,
                                    NW_ADT_Vector_Metric_t srcIndex,
                                    NW_ADT_Vector_Metric_t dstIndex);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_DynamicVector_Clear (NW_ADT_DynamicVector_t* dynamicVector);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_DYNAMICVECTORI_H */


