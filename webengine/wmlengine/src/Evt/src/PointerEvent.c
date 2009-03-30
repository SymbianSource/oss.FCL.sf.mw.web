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


#include "nw_evt_pointereventi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Evt_PointerEvent_Class_t NW_Evt_PointerEvent_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Evt_Event_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Evt_PointerEvent_t),
    /* construct               */ _NW_Evt_PointerEvent_Construct,
    /* destruct                */ NULL
  },
  { /* NW_Evt_Event           */
    /* unused                  */ NW_Object_Unused
  },
  { /* NW_Evt_PointerEvent */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Evt_PointerEvent_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  NW_Evt_PointerEvent_t* thisObj = NULL;

  /* for convenience */
  thisObj = NW_Evt_PointerEventOf (dynamicObject);

  if(thisObj == NULL) {
    return KBrsrFailure;
  }

  /* initialize our members */
  thisObj->type = va_arg (*argp, NW_Evt_PointerEvent_Type_t);
  thisObj->location.x = va_arg (*argp, NW_GDI_Metric_t);
  thisObj->location.y = va_arg (*argp, NW_GDI_Metric_t);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Evt_PointerEvent_t*
NW_Evt_PointerEvent_New (NW_Evt_PointerEvent_Type_t type, 
                             NW_GDI_Metric_t x, NW_GDI_Metric_t y)
{
  return (NW_Evt_PointerEvent_t*) NW_Object_New (&NW_Evt_PointerEvent_Class,
                                                     type, x, y);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Evt_PointerEvent_Initialize (NW_Evt_PointerEvent_t* event,
                                    NW_Evt_PointerEvent_Type_t type,
                                    NW_GDI_Metric_t x, NW_GDI_Metric_t y)
{
  return NW_Object_Initialize (&NW_Evt_PointerEvent_Class, event, type, x, y);
}

