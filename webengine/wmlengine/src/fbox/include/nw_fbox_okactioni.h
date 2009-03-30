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


#ifndef NW_FBOX_OKACTIONI_H
#define NW_FBOX_OKACTIONI_H

#include "nw_lmgr_actioni.h"
#include "nw_fbox_okaction.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_OkAction_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp);

NW_FBOX_EXPORT
void
_NW_FBox_OkAction_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
void
_NW_FBox_OkAction_Invoke(NW_LMgr_Action_t* Action);

NW_FBOX_EXPORT
const NW_Text_t*
_NW_FBox_OkAction_GetLabel(NW_LMgr_Action_t* Action);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_FBOX_OKACTIONI_H */
