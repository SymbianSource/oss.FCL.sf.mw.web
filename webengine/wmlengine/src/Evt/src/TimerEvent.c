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


#include "nw_evt_timereventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_TimerEvent_Class_t NW_Evt_TimerEvent_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Evt_Event_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_Evt_TimerEvent_t),
    /* construct         */ _NW_Evt_TimerEvent_Construct,
    /* destruct          */ NULL
  },
  { /* NW_Evt_Event     */
    /* unused            */ NW_Object_Unused
  },
  { /* NW_Evt_TimerEvent  */
    /* unused            */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Evt_TimerEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  NW_Evt_TimerEvent_t* thisObj;

  /* for convenience */
  thisObj = NW_Evt_TimerEventOf (dynamicObject);

  /* initialize our members */
  thisObj->TimerType = va_arg (*argp, NW_Evt_TimerType_t);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Evt_TimerEvent_Initialize (NW_Evt_TimerEvent_t* event,
                             NW_Evt_TimerType_t TimerType)
{
  return NW_Object_Initialize (&NW_Evt_TimerEvent_Class, event, TimerType);
}
