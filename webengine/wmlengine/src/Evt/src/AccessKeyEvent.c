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


#include "nw_evt_accesskeyeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_AccessKeyEvent_Class_t NW_Evt_AccessKeyEvent_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Evt_KeyEvent_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Evt_AccessKeyEvent_t),
    /* construct               */ NULL,
    /* destruct                */ NULL
  },
  { /* NW_Evt_Event           */
    /* unused                  */ 0
  },
  { /* NW_Evt_KeyEvent        */
    /* unused                  */ 0
  },
  { /* NW_Evt_AccessKeyEvent  */
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
NW_Evt_AccessKeyEvent_Initialize (NW_Evt_AccessKeyEvent_t* event)
{
  return NW_Object_Initialize (&NW_Evt_AccessKeyEvent_Class, event);
}
