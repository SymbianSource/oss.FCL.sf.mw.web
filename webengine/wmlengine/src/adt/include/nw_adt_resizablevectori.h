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


#ifndef NW_ADT_RESIZABLEVECTORI_H
#define NW_ADT_RESIZABLEVECTORI_H

#include "nw_adt_dynamicvectori.h"
#include "nw_adt_resizablevector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_ResizableVector_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp);

NW_ADT_EXPORT
void
_NW_ADT_ResizableVector_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_ResizableVector_ResizeCapacity (NW_ADT_DynamicVector_t* dynamicVector,
                                        NW_ADT_Vector_Metric_t capacityNeeded);

NW_ADT_EXPORT
NW_Uint8*
_NW_ADT_ResizableVector_AddressAt (const NW_ADT_Vector_t* vector,
                                   NW_ADT_Vector_Metric_t index);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_ResizableVector_MoveElements (NW_ADT_DynamicVector_t* dynamicVector,
                                      NW_ADT_Vector_Metric_t srcIndex,
                                      NW_ADT_Vector_Metric_t dstIndex);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_ResizableVector_GetArray(_resizableVector) \
  ((NW_Uint8* const) NW_ADT_ResizableVectorOf (_resizableVector)->array)

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_RESIZABLEVECTORI_H */
