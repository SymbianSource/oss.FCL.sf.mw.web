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


#include "nw_system_timeri.h"
#include "nwx_time.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_System_Timer_Class_t NW_System_Timer_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Object_Dynamic_Class,
    /* querySecondary        */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base        */
    /* secondaryList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_System_Timer_t),
    /* construct             */ _NW_System_Timer_Construct,
    /* destruct              */ NULL
  },
  { /* NW_System_Timer       */
    /* stop                 */ NULL,
    /* resume               */ NULL,
    /* isRunning            */ _NW_System_Timer_IsRunning,
    /* read                 */ _NW_System_Timer_Read
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_System_Timer_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argList)
{
  NW_System_Timer_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_System_Timer_Class));
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_System_TimerOf (dynamicObject);

  /* initialize the member variables */
  thisObj->callback = va_arg (*argList, NW_System_Timer_Callback_t);
  thisObj->clientData = va_arg (*argList, void*);
  thisObj->period = va_arg (*argList, NW_Uint32);
  thisObj->repeating = (NW_Bool)va_arg (*argList, NW_Uint32);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   NW_System_Timer methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_System_Timer_IsRunning (NW_System_Timer_t* timer,
                            NW_Bool* isRunning)
{
  NW_System_Timer_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (timer, &NW_System_Timer_Class));

  /* for convenience */
  thisObj = NW_System_TimerOf (timer);

  *isRunning = thisObj->isRunning;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_System_Timer_Read (NW_System_Timer_t* timer,
                       NW_Uint32* remainingTime)
{
  NW_System_Timer_t* thisObj;
  NW_Uint32 elapsedTime;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (timer, &NW_System_Timer_Class));

  /* for convenience */
  thisObj = NW_System_TimerOf (timer);

  if (thisObj->isRunning) {
    elapsedTime = NW_Time_GetTimeMSec() - thisObj->startTime;
    if(elapsedTime > thisObj->period) {
      *remainingTime = 0;
    } else  {
      *remainingTime = thisObj->remainingTime - elapsedTime;
    }
    return KBrsrSuccess;
  }

  *remainingTime = thisObj->remainingTime;

  return KBrsrSuccess;
}

