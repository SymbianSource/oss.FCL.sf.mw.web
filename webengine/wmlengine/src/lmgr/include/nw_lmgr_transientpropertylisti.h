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


#ifndef NW_LMGR_TRANSIENTPROPERTYLISTI_H
#define NW_LMGR_TRANSIENTPROPERTYLISTI_H

#include "nw_lmgr_simplepropertylisti.h"
#include "nw_lmgr_transientpropertylist.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_TransientPropertyList_Construct (NW_Object_Dynamic_t* dynamicObject,
                                          va_list* argp);

NW_LMGR_EXPORT 
TBrowserStatusCode
_NW_LMgr_TransientPropertyList_Get (const NW_LMgr_PropertyList_t* propertyList,
                                    NW_LMgr_PropertyName_t key,
                                    NW_LMgr_PropertyList_Entry_t* entry);

NW_LMGR_EXPORT
TBrowserStatusCode 
_NW_LMgr_TransientPropertyList_Remove (NW_LMgr_PropertyList_t* propertyList,
                                       NW_LMgr_PropertyName_t key);

NW_LMGR_EXPORT
NW_LMgr_PropertyList_t*
_NW_LMgr_TransientPropertyList_Clone (const NW_LMgr_PropertyList_t* propertyList);

NW_LMGR_EXPORT
TBrowserStatusCode 
_NW_LMgr_TransientPropertyList_Clear (NW_LMgr_PropertyList_t* propertyList) ;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_TRANSIENTPROPERTYLISTI_H */
