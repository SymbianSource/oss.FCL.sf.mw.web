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


#ifndef NW_ADT_MAPITERATORI_H
#define NW_ADT_MAPITERATORI_H

#include "nw_adt_vectoriteratori.h"
#include "nw_adt_mapiterator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_MapIterator_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argList);

NW_Uint32
_NW_ADT_MapIterator_GetElementSize (NW_ADT_Iterator_t* iterator);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_MAPITERATORI_H */
