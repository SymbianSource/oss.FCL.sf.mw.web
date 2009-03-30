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


#ifndef NW_ADT_MAPI_H
#define NW_ADT_MAPI_H

#include "nw_object_dynamici.h"
#include "nw_adt_map.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_ADT_Map_Lookup(_map, _key) \
  (NW_Object_Invoke (_map, NW_ADT_Map, lookup) \
   ((_map), (_key)))

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
TBrowserStatusCode
_NW_ADT_Map_Construct (NW_Object_Dynamic_t* dynamicObject,
                       va_list* argp);

NW_ADT_EXPORT
void
_NW_ADT_Map_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_ADT_EXPORT
NW_ADT_Vector_Metric_t
_NW_ADT_Map_Lookup (const NW_ADT_Map_t* thisObj,
                    const void* key);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_MAPI_H */
