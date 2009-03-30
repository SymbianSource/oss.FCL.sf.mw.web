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


#include "nw_fbox_keyboardeventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_KeyBoardEvent_Class_t NW_FBox_KeyBoardEvent_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Evt_Event_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_FBox_KeyBoardEvent_t),
    /* construct         */ _NW_FBox_KeyBoardEvent_Construct,
    /* destruct          */ NULL
  },
  { /* NW_Evt_Event     */
    /* unused            */ NW_Object_Unused
  },
  { /* NW_FBox_KeyBoardEvent  */
    /* unused            */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_KeyBoardEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  NW_FBox_KeyBoardEvent_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_KeyBoardEventOf (dynamicObject);

  /* initialize our members */
  thisObj->c = (NW_Ucs2)va_arg (*argp, NW_Uint32);
  thisObj->keyType = va_arg (*argp, NW_FBox_KeyBoardEvent_KeyType_t);
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_KeyBoardEvent_Initialize (NW_FBox_KeyBoardEvent_t* event,
                             NW_Ucs2 c, NW_FBox_KeyBoardEvent_KeyType_t keyType)
{
  return NW_Object_Initialize (&NW_FBox_KeyBoardEvent_Class, event, (NW_Uint32)c, keyType);
}
