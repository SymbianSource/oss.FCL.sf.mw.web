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


#ifndef NW_ADT_FIXEDVECTORI_H
#define NW_ADT_FIXEDVECTORI_H

#include "nw_adt_dynamicvectori.h"
#include "nw_adt_fixedvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_FixedVector_Construct (NW_Object_t* object,
                               va_list* argp);

NW_ADT_EXPORT
NW_Uint8*
_NW_ADT_FixedVector_AddressAt (const NW_ADT_Vector_t* vector,
                               NW_ADT_Vector_Metric_t index);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_FixedVector_ResizeCapacity (NW_ADT_DynamicVector_t* dynamicVector,
                                    NW_ADT_Vector_Metric_t capacityNeeded);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_FixedVector_MoveElements (NW_ADT_DynamicVector_t* dynamicVector,
                                  NW_ADT_Vector_Metric_t srcIndex,
                                  NW_ADT_Vector_Metric_t dstIndex);

NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_FixedVector_Clear (NW_ADT_DynamicVector_t* dynamicVector);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_FIXEDVECTORI_H */
