/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_INLINEINPUTINTERACTORI_H
#define NW_FBOX_INLINEINPUTINTERACTORI_H

#include "nw_fbox_inputinteractori.h"
#include "nw_fbox_inlineinputinteractor.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_InlineInputInteractor_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp);

NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_InlineInputInteractor_ProcessEvent(
                        NW_FBox_Interactor_t* interactor,
                        NW_LMgr_ActiveBox_t* box,
                        NW_Evt_Event_t* event);



#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_INLINEINPUTINTERACTORI_H */
