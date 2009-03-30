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


#ifndef NW_MSG_MESSAGEI_H
#define NW_MSG_MESSAGEI_H

#include "nw_object_dynamici.h"
#include "nw_msg_message.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_MSG_EXPORT
TBrowserStatusCode
_NW_Msg_Message_Construct (NW_Object_Dynamic_t* dynamicObject,
                           va_list* argList);

NW_MSG_EXPORT
void
_NW_Msg_Message_Destruct (NW_Object_Dynamic_t* dynamicObject);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MSG_MESSAGEI_H */
