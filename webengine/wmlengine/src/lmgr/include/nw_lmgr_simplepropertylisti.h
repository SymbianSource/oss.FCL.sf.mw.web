/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_LMGR_SIMPLEPROPERTYLISTI_H
#define NW_LMGR_SIMPLEPROPERTYLISTI_H

#include "nw_lmgr_propertylisti.h"
#include "nw_lmgr_simplepropertylist.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_SimplePropertyList_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_SimplePropertyList_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT 
TBrowserStatusCode
_NW_LMgr_SimplePropertyList_Get (const NW_LMgr_PropertyList_t* propertyList,
                                 NW_LMgr_PropertyName_t key,
                                 NW_LMgr_PropertyList_Entry_t* entry);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_SimplePropertyList_Set (NW_LMgr_PropertyList_t* propertyList,
                                 NW_LMgr_PropertyName_t key,
                                 const NW_LMgr_PropertyList_Entry_t* entry);

NW_LMGR_EXPORT
TBrowserStatusCode 
_NW_LMgr_SimplePropertyList_Remove (NW_LMgr_PropertyList_t* propertyList,
                                    NW_LMgr_PropertyName_t key);

NW_LMGR_EXPORT
NW_LMgr_PropertyList_t*
_NW_LMgr_SimplePropertyList_Clone (const NW_LMgr_PropertyList_t* propertyList);

NW_LMGR_EXPORT
TBrowserStatusCode 
_NW_LMgr_SimplePropertyList_Clear (NW_LMgr_PropertyList_t* propertyList);

NW_LMGR_EXPORT
TBrowserStatusCode 
_NW_LMgr_SimplePropertyList_Copy (const NW_LMgr_SimplePropertyList_t* source,
                                  NW_LMgr_PropertyList_t* destination);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_SimplePropertyList_Copy(_source, _destination) \
  (_NW_LMgr_SimplePropertyList_Copy ( \
     NW_LMgr_SimplePropertyListOf (_source), NW_LMgr_PropertyListOf (_destination)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_SIMPLEPROPERTYLISTI_H */
