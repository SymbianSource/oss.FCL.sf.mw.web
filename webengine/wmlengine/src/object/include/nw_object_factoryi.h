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


#ifndef _NW_Object_FactoryI_h_
#define _NW_Object_FactoryI_h_


#include "nw_object_corei.h"
#include "nw_object_factory.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected prototypes
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Factory_InitializeObject (NW_Object_Factory_t* factory,
                                     const NW_Object_Class_t* objClass,
                                     NW_Object_t* object,
                                     va_list* argList);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Factory_TerminateObject (NW_Object_Factory_t* factory,
                                    NW_Object_t* object);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Factory_NewObject (NW_Object_Factory_t* factory,
                              const NW_Object_Class_t* objClass,
                              va_list* argList,
                              NW_Object_t** object);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Factory_DeleteObject (NW_Object_Factory_t* factory,
                                 NW_Object_t* object);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_FactoryI_h_ */
