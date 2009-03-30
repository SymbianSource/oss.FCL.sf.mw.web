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


#include "nw_evt_activateeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_ActivateEvent_Class_t NW_Evt_ActivateEvent_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Evt_Event_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_Evt_ActivateEvent_t),
    /* construct             */ NULL,
    /* destruct              */ NULL
  },
  { /* NW_Evt_Event         */
    /* unused                */ 0
  },
  { /* NW_Evt_ActivateEvent */
    /* unused                */ 0
  }
};

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Evt_ActivateEvent_t*
NW_Evt_ActivateEvent_New (void)
{
  return (NW_Evt_ActivateEvent_t*) NW_Object_New (&NW_Evt_ActivateEvent_Class);
}

TBrowserStatusCode
NW_Evt_ActivateEvent_Initialize (NW_Evt_ActivateEvent_t* activateEvent)
{
  return NW_Object_Initialize (&NW_Evt_ActivateEvent_Class, activateEvent);
}
