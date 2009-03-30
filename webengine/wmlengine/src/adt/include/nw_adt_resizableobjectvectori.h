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


#ifndef NW_ADT_RESIZABLEOBJECTVECTORI_H
#define NW_ADT_RESIZABLEOBJECTVECTORI_H

#include "nw_adt_resizablevectori.h"
#include "nw_adt_resizableobjectvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_ResizableObjectVector_Construct (NW_Object_Dynamic_t* dynamicObject,
                                         va_list* argp);

NW_ADT_EXPORT
void
_NW_ADT_ResizableObjectVector_Destruct (NW_Object_Dynamic_t* dynamicObject);


NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_ResizableObjectVector_ResizeCapacity (
  NW_ADT_DynamicVector_t* dynamicVector,
  NW_ADT_Vector_Metric_t capacityNeeded);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_RESIZABLEVECTORI_H */
