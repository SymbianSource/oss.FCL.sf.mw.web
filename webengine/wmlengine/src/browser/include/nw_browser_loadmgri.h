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


#ifndef NW_BROWSER_LOADMGRI_H
#define NW_BROWSER_LOADMGRI_H

#include "nw_object_dynamici.h"
#include "nw_mvc_iloadlisteneri.h"
#include "nw_object_interface.h"
#include "nw_browser_loadmgr.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
extern const NW_Object_Class_t* const _NW_Browser_LoadMgr_SecondaryList[];
extern const NW_MVC_ILoadListener_Class_t NW_Browser_LoadMgr_ILoadListener_Class;

/* ------------------------------------------------------------------------- *
   protected symbols
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_Browser_LoadMgr_Construct (NW_Object_Dynamic_t* dynamicObject,
			       va_list* argList);

extern
void
_NW_Browser_LoadMgr_ILoadListener_LoadComplete (NW_MVC_ILoadListener_t* loadListener,
						NW_Bool success);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_BROWSER_LOADMGRI_H */
