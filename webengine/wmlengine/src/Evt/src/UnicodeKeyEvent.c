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


#include "nw_evt_unicodekeyeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_UnicodeKeyEvent_Class_t NW_Evt_UnicodeKeyEvent_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Evt_KeyEvent_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_Evt_UnicodeKeyEvent_t),
    /* construct         */ _NW_Evt_UnicodeKeyEvent_Construct,
    /* destruct          */ NULL
  },
  { /* NW_Evt_Event     */
    /* unused            */ NW_Object_Unused
  },
  { /* NW_Evt_KeyEvent     */
    /* unused            */ NW_Object_Unused
  },
  { /* NW_Evt_UnicodeKeyEvent  */
    /* unused            */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Evt_UnicodeKeyEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  NW_TRY (status) {
    NW_Evt_UnicodeKeyEvent_t* thisObj;

    /* for convenience */
    thisObj = NW_Evt_UnicodeKeyEventOf (dynamicObject);

    /* invoke our superclass constructor */
    status = _NW_Evt_KeyEvent_Construct (dynamicObject, argp);
    _NW_THROW_ON_ERROR(status);

    /* initialize our members */
    thisObj->keyVal = (NW_Ucs2) va_arg (*argp, NW_Uint32);

    /* successful completion */
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Evt_UnicodeKeyEvent_t* 
NW_Evt_UnicodeKeyEvent_New (NW_Ucs2 keyVal)
{
  return (NW_Evt_UnicodeKeyEvent_t*) NW_Object_New (&NW_Evt_UnicodeKeyEvent_Class,
                                                     (NW_Uint32)keyVal);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Evt_UnicodeKeyEvent_Initialize (NW_Evt_UnicodeKeyEvent_t* event,
                                    NW_Ucs2 keyVal)
{
  return NW_Object_Initialize (&NW_Evt_UnicodeKeyEvent_Class, event, (NW_Uint32)keyVal);
}
