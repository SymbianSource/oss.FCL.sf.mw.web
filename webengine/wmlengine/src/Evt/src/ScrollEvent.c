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


#include "nw_evt_scrolleventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_ScrollEvent_Class_t NW_Evt_ScrollEvent_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_Evt_Event_Class,
    /* queryInterface         */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base         */
    /* interfaceList          */ NULL
  },
  { /* NW_Object_Dynamic      */
    /* instanceSize           */ sizeof (NW_Evt_ScrollEvent_t),
    /* construct              */ _NW_Evt_ScrollEvent_Construct,
    /* destruct               */ NULL
  },
  { /* NW_Evt_Event          */
    /* unused                 */ NW_Object_Unused
  },
  { /* NW_Evt_ScrollEvent */
    /* unused                 */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Evt_ScrollEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  NW_Evt_ScrollEvent_t* thisObj;

  /* for convenience */
  thisObj = NW_Evt_ScrollEventOf (dynamicObject);

  if(thisObj == NULL) {
    return KBrsrFailure;
  }

  /* initialize our members */
  thisObj->direction = va_arg (*argp, NW_Evt_ScrollEvent_Direction_t);
  thisObj->amount = (NW_Uint16)va_arg(*argp,  NW_Uint32);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Evt_ScrollEvent_t*
NW_Evt_ScrollEvent_New (NW_Evt_ScrollEvent_Direction_t direction, NW_Uint16 amount)
{
  return (NW_Evt_ScrollEvent_t*) NW_Object_New (&NW_Evt_ScrollEvent_Class,
                                                     direction, (NW_Uint32) amount);
}

TBrowserStatusCode
NW_Evt_ScrollEvent_Initialize (NW_Evt_ScrollEvent_t* event,
                                   NW_Evt_ScrollEvent_Direction_t direction, NW_Uint16 amount)
{
  return NW_Object_Initialize (&NW_Evt_ScrollEvent_Class, event, direction, (NW_Uint32)amount);
}
