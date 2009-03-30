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


#ifndef NW_ADT_VECTORI_H
#define NW_ADT_VECTORI_H

#include "nw_object_dynamici.h"
#include "nw_adt_vector.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected method accessors
 * ------------------------------------------------------------------------- */
#define NW_ADT_Vector_AddressAt(_object, _index) \
  (NW_Object_Invoke (_object, NW_ADT_Vector, addressAt) \
   (NW_ADT_VectorOf (_object), (_index)))

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_VECTORI_H */


