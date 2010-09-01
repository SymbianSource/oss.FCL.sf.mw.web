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


#ifndef NW_OBJECT_SHAREDI_H
#define NW_OBJECT_SHAREDI_H

#include "nw_object_dynamici.h"
#include "nw_object_shared.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp);

NW_OBJECT_EXPORT
void
_NW_Object_Shared_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_Reserve (NW_Object_Shared_t* object);

NW_OBJECT_EXPORT
NW_Bool
_NW_Object_Shared_Release (NW_Object_Shared_t* object);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_LockObject (NW_Object_Shared_t* object);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_UnlockObject (NW_Object_Shared_t* object);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_GetObject (NW_Object_Shared_t* object,
                             NW_Object_Dynamic_t** sharedObject);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_AdoptObject (NW_Object_Shared_t* object,
                               NW_Object_Dynamic_t* sharedObject);

NW_OBJECT_EXPORT
TBrowserStatusCode
_NW_Object_Shared_OrphanObject (NW_Object_Shared_t* object,
                                NW_Object_Dynamic_t** sharedObject);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_SHAREDI_H */
