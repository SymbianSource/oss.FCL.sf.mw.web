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


#ifndef NW_FBOX_EPOC32INPUTINTERACTORI_H
#define NW_FBOX_EPOC32INPUTINTERACTORI_H

#include "nw_fbox_inputinteractori.h"
#include "nw_fbox_epoc32inputinteractor.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
NW_Uint8
_NW_FBox_Epoc32InputInteractor_ProcessEvent(
                        NW_FBox_Interactor_t* interactor,
                        NW_LMgr_ActiveBox_t* box,
                        NW_Evt_Event_t* event);



#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_EPOC32INPUTINTERACTORI_H */
