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


#ifndef NW_OBJECT_DYNAMICI_H
#define NW_OBJECT_DYNAMICI_H

#include "nw_object_basei.h"
#include "nw_object_dynamic.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Dynamic_Initialize (NW_Object_Core_t* core,
                               va_list* argList);

NW_OBJECT_EXPORT
void
_NW_Object_Dynamic_Terminate (NW_Object_Core_t* object);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Object_Dynamic_Construct(_dynamic, _argList) \
  (NW_Object_Invoke ((_dynamic), NW_Object_Dynamic, construct) ( \
     NW_Object_DynamicOf (_dynamic), \
     (_argList)))

#define NW_Object_Dynamic_Destruct(_dynamic) \
  (NW_Object_Invoke ((_dynamic), NW_Object_Dynamic, destruct) ( \
     NW_Object_DynamicOf (_dynamic)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_DYNAMICI_H */
