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


#include "nw_evt_controlkeyeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_ControlKeyEvent_Class_t NW_Evt_ControlKeyEvent_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Evt_KeyEvent_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Evt_ControlKeyEvent_t),
    /* construct               */ _NW_Evt_ControlKeyEvent_Construct,
    /* destruct                */ NULL
  },
  { /* NW_Evt_Event           */
    /* unused                  */ 0
  },
  { /* NW_Evt_KeyEvent        */
    /* unused                  */ 0
  },
  { /* NW_Evt_ControlKeyEvent */
    /* unused                  */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Evt_ControlKeyEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp)
{
  NW_TRY (status) {
    NW_Evt_ControlKeyEvent_t* thisObj;

    /* for convenience */
    thisObj = NW_Evt_ControlKeyEventOf (dynamicObject);

    /* invoke our superclass constructor */
    status = _NW_Evt_KeyEvent_Construct (dynamicObject, argp);
    _NW_THROW_ON_ERROR(status);

    /* initialize our members */
    thisObj->keyVal = va_arg (*argp, NW_Evt_ControlKeyType_t);

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
NW_Evt_ControlKeyEvent_t* 
NW_Evt_ControlKeyEvent_New (NW_Evt_ControlKeyType_t keyType)
{
  return (NW_Evt_ControlKeyEvent_t*) NW_Object_New (&NW_Evt_ControlKeyEvent_Class,
                                                     keyType);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Evt_ControlKeyEvent_Initialize (NW_Evt_ControlKeyEvent_t* event,
                                    NW_Evt_ControlKeyType_t keyType)
{
  return NW_Object_Initialize (&NW_Evt_ControlKeyEvent_Class, event, keyType);
}
