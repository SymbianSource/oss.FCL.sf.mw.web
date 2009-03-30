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


#include "nw_xhtml_savetophonebkeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_SaveToPhoneBkEvent_Class_t NW_XHTML_SaveToPhoneBkEvent_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Evt_Event_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_XHTML_SaveToPhoneBkEvent_t),
    /* construct               */ NULL,
    /* destruct                */ NULL
  },
  { /* NW_Evt_Event           */
    /* unused                  */ 0
  },
  { /* NW_XHTML_SaveToPhoneBkEvent  */
    /* unused                  */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_SaveToPhoneBkEvent_Initialize (NW_XHTML_SaveToPhoneBkEvent_t* event)
{
  return NW_Object_Initialize (&NW_XHTML_SaveToPhoneBkEvent_Class, event);
}
