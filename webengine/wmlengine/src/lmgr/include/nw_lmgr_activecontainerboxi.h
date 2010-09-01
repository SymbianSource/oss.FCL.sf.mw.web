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


#ifndef NW_LMGR_ACTIVECONTAINERBOXI_H
#define NW_LMGR_ACTIVECONTAINERBOXI_H



#include "nw_lmgr_containerboxi.h"
#include "nw_lmgr_activecontainerbox.h"
#include "NW_LMgr_IEventListenerI.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ActiveContainerBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_ActiveContainerBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
_NW_LMgr_ActiveContainerBox_Clone (NW_LMgr_Box_t* thisObj);

NW_LMGR_EXPORT
NW_Uint8
_NW_LMgr_ActiveContainerBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                         NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ACTIVECONTAINERBOXI_H */
