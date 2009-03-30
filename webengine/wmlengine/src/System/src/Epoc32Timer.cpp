/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h> // avoid NULL redefinition warning
#include "nw_system_epoc32timeri.h"
#include "nwx_time.h"

#include "MVCShell.h"
#include "CEpoc32Timer.h"

#include "nwx_ctx.h"
#include "nwx_defs_symbian.h"
#include "BrsrStatusCodes.h"
#include "MemoryManager.h"


/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_System_Epoc32Timer_Class_t NW_System_Epoc32Timer_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_System_Timer_Class,
    /* querySecondary       */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base       */
    /* secondaryList        */ NULL
  },
  { /* NW_Object_Dynamic    */
    /* instanceSize         */ sizeof (NW_System_Epoc32Timer_t),
    /* construct            */ _NW_System_Epoc32Timer_Construct,
    /* destruct             */ _NW_System_Epoc32Timer_Destruct
  },
  { /* NW_System_Timer      */
    /* stop                 */ _NW_System_Epoc32Timer_Stop,
    /* resume               */ _NW_System_Epoc32Timer_Resume,
    /* isRunning            */ _NW_System_Epoc32Timer_IsRunning,
    /* read                 */ _NW_System_Epoc32Timer_Read
  },
  { /* NW_System_Epoc32Timer */
    /* unused               */ NW_Object_Unused
  }
};



/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_System_Epoc32Timer_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argList)
{
  TBrowserStatusCode status;
  NW_System_Epoc32Timer_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_System_Epoc32Timer_Class));
  NW_ASSERT (argList != NULL);

  /* invoke our superclass constructor */
  status = _NW_System_Timer_Construct (dynamicObject, argList);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* for convenience */
  thisObj = NW_System_Epoc32TimerOf (dynamicObject);


  // RB: Period is in milliseconds
  TUint32 timerPeriod(1000 * thisObj->super.period);
  
  TCallBack cb((TThreadFunction)thisObj->super.callback, thisObj->super.clientData);

  /* initialize the member variables */
  thisObj->epocTimer = CEpoc32Timer::New(timerPeriod, thisObj->super.repeating, cb);
  if(thisObj->epocTimer == NULL) {
    return KBrsrOutOfMemory;
  }
  
  thisObj->super.startTime = NW_Time_GetTimeMSec();
  thisObj->super.remainingTime = thisObj->super.period;
  thisObj->super.isRunning = NW_TRUE;
  
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_System_Epoc32Timer_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_System_Epoc32Timer_t* thisObj;
  CEpoc32Timer* epocTimer = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_System_Epoc32Timer_Class));

  /* for convenience */
  thisObj = NW_System_Epoc32TimerOf (dynamicObject);
  epocTimer = (CEpoc32Timer*)thisObj->epocTimer;

  epocTimer->Cancel();
  delete epocTimer;
}

// ------------------------------------------------------------------------- 
// ------------------------------------------------------------------------- 

TBrowserStatusCode
_NW_System_Epoc32Timer_IsRunning (NW_System_Timer_t* timer,
                            NW_Bool* isRunning)
{
  NW_System_Epoc32Timer_t* thisObj;
  CEpoc32Timer* epocTimer = NULL;

  /* for convenience */
  thisObj = NW_System_Epoc32TimerOf (timer);
  epocTimer = (CEpoc32Timer*)thisObj->epocTimer;

  if (epocTimer)
	{
    *isRunning = (NW_Bool)epocTimer->IsActive();
    return KBrsrSuccess;
	}

  return KBrsrFailure;
}

TBrowserStatusCode
_NW_System_Epoc32Timer_Read (NW_System_Timer_t* timer,
                       NW_Uint32* remainingTime)
{
  NW_System_Epoc32Timer_t* thisObj;
  CEpoc32Timer* epocTimer = NULL;

  /* for convenience */
  thisObj = NW_System_Epoc32TimerOf (timer);
  epocTimer = (CEpoc32Timer*)thisObj->epocTimer;

  if (epocTimer)
	{
    *remainingTime = epocTimer->MicrosecondsLeftUntilNextTick()/1000;
    return KBrsrSuccess;
	}
	
  return KBrsrFailure;
}


/* ------------------------------------------------------------------------- *
   NW_System_Timer methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_System_Epoc32Timer_Stop (NW_System_Timer_t* timer)
{
  NW_System_Epoc32Timer_t* thisObj;
  CEpoc32Timer* epocTimer = NULL;

  /* for convenience */
  thisObj = NW_System_Epoc32TimerOf (timer);
  epocTimer = (CEpoc32Timer*)thisObj->epocTimer;

  if (epocTimer)
  {
    epocTimer->Stop();
    return KBrsrSuccess;
  }

  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_System_Epoc32Timer_Resume (NW_System_Timer_t* timer)
{
  NW_System_Epoc32Timer_t* thisObj;
  CEpoc32Timer* epocTimer = NULL;

  /* for convenience */
  thisObj = NW_System_Epoc32TimerOf (timer);
  epocTimer = (CEpoc32Timer*)thisObj->epocTimer;

  if (epocTimer)
  {
    epocTimer->Resume();
    return KBrsrSuccess;
  }

  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
NW_System_Timer_t*
_NW_System_Timer_New (NW_System_Timer_Callback_t callback,
                      void* clientData,
                      NW_Uint32 period,
                      NW_Bool repeating)
{
  return (NW_System_Timer_t*)
    NW_Object_New (&NW_System_Epoc32Timer_Class, callback, clientData, period,
                   (NW_Uint32)repeating);
}
