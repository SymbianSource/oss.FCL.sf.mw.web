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


#ifndef NW_FBOX_PASSWORDBOXI_H
#define NW_FBOX_PASSWORDBOXI_H

#include "nw_fbox_inputboxi.h"
#include "nw_fbox_passwordbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

TBrowserStatusCode
_NW_FBox_PasswordBox_SetObscureDisplayText (NW_FBox_InputBox_t* thisObj,
                                NW_Bool bufferModificationOn);

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT
void
_NW_FBox_PasswordBox_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_PasswordBox_SetDisplayText(NW_FBox_InputBox_t* thisObj,
                                NW_Bool bufferModificationOn);

NW_FBOX_EXPORT
TBrowserStatusCode
_NW_FBox_PasswordBox_ModifyBuffer(NW_FBox_InputBox_t* thisObj, 
                                  NW_Ucs2 c, 
                                  NW_Uint32 repeated);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_PASSWORDBOXI_H */
