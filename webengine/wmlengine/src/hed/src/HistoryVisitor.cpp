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


#include "nw_hed_historyvisitori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
const
NW_HED_HistoryVisitor_Class_t NW_HED_HistoryVisitor_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_HistoryVisitor_t),
    /* construct               */ _NW_HED_HistoryVisitor_Construct,
    /* destruct                */ _NW_HED_HistoryVisitor_Destruct
  },
  { /* NW_HED_HistoryVisitor     */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_HistoryVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_HED_HistoryVisitor_t* thisObj;
  NW_HED_HistoryVisitor_Location_t location;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_HistoryVisitor_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_HED_HistoryVisitorOf (dynamicObject);

  /* get the stack */
  thisObj->stack = (NW_HED_HistoryStack_t*) va_arg (*argp, const NW_HED_HistoryStack_t*);
  if ((thisObj->stack == NULL) || (NW_Object_IsInstanceOf (thisObj->stack, 
      &NW_HED_HistoryStack_Class) == NW_FALSE)) {
    return KBrsrFailure;
  }

  /* set the starting position */
  location = (NW_HED_HistoryVisitor_Location_t) va_arg (*argp, TInt);

  if (location == NW_HED_HistoryVisitor_Loacation_AtBeginning) {
    thisObj->current = 0;
  }

  else if (location == NW_HED_HistoryVisitor_Loacation_AtEnd) {
    thisObj->current = (NW_HED_HistoryStack_Metric_t) (NW_ADT_Vector_GetSize (thisObj->stack->vector) - 1);
  }

  else if (location == NW_HED_HistoryVisitor_Loacation_Current) {
    thisObj->current = thisObj->stack->current;
  }

  else {
    return KBrsrFailure;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_HistoryVisitor_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_REQUIRED_PARAM (dynamicObject);

  /* Nothing needs to be done */
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_HistoryEntry_t* 
 NW_HED_HistoryVisitor_Prev (NW_HED_HistoryVisitor_t* thisObj)
{
  return NW_HED_HistoryStack_GetEntry( thisObj->stack, NW_HED_HistoryStack_Direction_Previous );
}

/* ------------------------------------------------------------------------- */
const NW_HED_HistoryEntry_t* 
NW_HED_HistoryVisitor_Next (NW_HED_HistoryVisitor_t* thisObj)
{
  return NW_HED_HistoryStack_GetEntry( thisObj->stack, NW_HED_HistoryStack_Direction_Next );
}

/* ------------------------------------------------------------------------- */
const NW_HED_HistoryEntry_t* 
NW_HED_HistoryVisitor_Current (NW_HED_HistoryVisitor_t* thisObj)
{
  return NW_HED_HistoryStack_GetEntry( thisObj->stack, NW_HED_HistoryStack_Direction_Current );
}
/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_HistoryVisitor_Initialize (NW_HED_HistoryVisitor_t* thisObj,
                                  NW_HED_HistoryStack_t* stack,
                                  NW_HED_HistoryVisitor_Location_t location)
{
  return NW_Object_Initialize (&NW_HED_HistoryVisitor_Class, thisObj,
      stack, location);
}


/* ------------------------------------------------------------------------- */
NW_HED_HistoryVisitor_t*
NW_HED_HistoryVisitor_New (NW_HED_HistoryStack_t* stack,
                           NW_HED_HistoryVisitor_Location_t location)
{
  return (NW_HED_HistoryVisitor_t*) NW_Object_New (&NW_HED_HistoryVisitor_Class,
      stack, location);
}

