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


#ifndef NW_HED_EVENTHANDLERI_H
#define NW_HED_EVENTHANDLERI_H

#include "nw_lmgr_eventhandleri.h"
#include "nw_hed_hedeventhandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_EventHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp);

NW_HED_EXPORT
NW_Uint8
_NW_HED_EventHandler_ProcessEvent (NW_LMgr_EventHandler_t* eventHandler,
                                   NW_LMgr_Box_t* box,
                                   NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_EVENTHANDLERI_H */