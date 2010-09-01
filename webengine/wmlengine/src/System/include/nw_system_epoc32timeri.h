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



#ifndef _NW_System_Epoc32TimerI_h_
#define _NW_System_Epoc32TimerI_h_

#include "nw_system_timeri.h"
#include "nw_system_epoc32timer.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected prototypes
 * ------------------------------------------------------------------------- */
NW_SYSTEM_EXPORT
TBrowserStatusCode
_NW_System_Epoc32Timer_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argList);

NW_SYSTEM_EXPORT
void
_NW_System_Epoc32Timer_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_SYSTEM_EXPORT
TBrowserStatusCode
_NW_System_Epoc32Timer_Stop (NW_System_Timer_t* timer);

NW_SYSTEM_EXPORT
TBrowserStatusCode
_NW_System_Epoc32Timer_Resume (NW_System_Timer_t* timer);

NW_SYSTEM_EXPORT
TBrowserStatusCode
_NW_System_Epoc32Timer_IsRunning (NW_System_Timer_t* timer,
								  NW_Bool* isRunning);

NW_SYSTEM_EXPORT
TBrowserStatusCode
_NW_System_Epoc32Timer_Read (NW_System_Timer_t* timer,
							 NW_Uint32* remainingTime);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_System_Epoc32TimerI_h_ */
