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


#include "nw_evt_openviewereventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_OpenViewerEvent_Class_t NW_Evt_OpenViewerEvent_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Evt_Event_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_Evt_OpenViewerEvent_t),
    /* construct             */ NULL,
    /* destruct              */ NULL
  },
  { /* NW_Evt_Event          */
    /* unused                */ 0
  },
  { /* NW_Evt_OpenViewerEvent*/
    /* unused                */ 0
  }
};

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Evt_OpenViewerEvent_t*
NW_Evt_OpenViewerEvent_New (void)
{
  return (NW_Evt_OpenViewerEvent_t*) NW_Object_New (&NW_Evt_OpenViewerEvent_Class);
}

TBrowserStatusCode
NW_Evt_OpenViewerEvent_Initialize (NW_Evt_OpenViewerEvent_t* openViewerEvent)
{
  return NW_Object_Initialize (&NW_Evt_OpenViewerEvent_Class, openViewerEvent);
}
