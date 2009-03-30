/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Box class associated with the <area> tag
*
*/


#ifndef NW_AREABOXI_H
#define NW_AREABOXI_H

//  INCLUDES
#include "nw_lmgr_activeboxi.h"
#include "nw_lmgr_areabox.h"
#include "NW_LMgr_IEventListenerI.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AreaBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_AreaBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_LMGR_EXPORT
NW_Uint8
_NW_LMgr_AreaBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                              NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif // NW_AREABOXI_H
