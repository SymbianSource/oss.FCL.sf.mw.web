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


#include "nw_evt_keyeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_KeyEvent_Class_t NW_Evt_KeyEvent_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Evt_Event_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_Evt_KeyEvent_t),
    /* construct         */ _NW_Evt_KeyEvent_Construct,
    /* destruct          */ NULL
  },
  { /* NW_Evt_Event     */
    /* unused            */ 0
  },
  { /* NW_Evt_KeyEvent  */
    /* unused            */ 0
  }
};

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_Evt_KeyEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  NW_Evt_KeyEvent_t* keyEvent;

  NW_ASSERT(dynamicObject != NULL);

  /* for convenience */
  keyEvent = NW_Evt_KeyEventOf (dynamicObject);
  NW_REQUIRED_PARAM(argp);
   
  /* initialize the member variables */
  keyEvent->longPress = NW_FALSE;
  return KBrsrSuccess;
}
