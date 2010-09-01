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


#ifndef NW_OBJECT_COREI_H
#define NW_OBJECT_COREI_H

#include "nw_object_core.h"
#include <stdarg.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Core_Initialize (NW_Object_Core_t* core,
                            va_list* argList);

NW_OBJECT_EXPORT
void
_NW_Object_Core_Terminate (NW_Object_Core_t* core);

NW_OBJECT_EXPORT
NW_Object_t*
_NW_Object_Core_QuerySecondary (const NW_Object_Core_t* core,
                                const NW_Object_Class_t* objClass);
#define _NW_Object_Core_QueryInterface _NW_Object_Core_QuerySecondary

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Core_CallInitialize (const NW_Object_Class_t* objClass,
                                NW_Object_Core_t* core,
                                va_list* argList);

NW_OBJECT_EXPORT
void
_NW_Object_Core_CallTerminate (NW_Object_Core_t* core);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Object_Core_CallInitialize(_objClass, _core, _argList) \
  (_NW_Object_Core_CallInitialize ( \
     (_objClass), \
     (NW_Object_Core_t*) (_core), \
     (_argList)))

#define NW_Object_Core_CallTerminate(_core) \
  (_NW_Object_Core_CallTerminate ( \
     NW_Object_CoreOf (_core)))

#define NW_Object_Core_QuerySecondary(_core, _class) \
  (NW_Object_Invoke ((_core), NW_Object_Core, querySecondary) (\
    NW_Object_CoreOf (_core), (_class)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_COREI_H */
