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


#include "nw_object_aggregatei.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Object_Aggregate_Class_t NW_Object_Aggregate_Class = {
  { /* NW_Object_Core      */
    /* super               */ &NW_Object_Secondary_Class,
    /* querySecondary      */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary */
    /* offset              */ 0
  },
  { /* NW_Object_Aggregate */
    /* secondaryList       */ NULL,
    /* construct           */ NULL,
    /* destruct            */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   public/protected methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Aggregate_Initialize (NW_Object_Core_t* core,
                                 va_list* argList)
{
  NW_TRY (status) {
    /* call our superclass method */
    status = _NW_Object_Core_Initialize (core, argList);
    if (status != KBrsrSuccess) {
      NW_THROW (status);
    }

    /* call the class constructor on the newly allocated core */
    if (NW_Object_Aggregate_GetClassPart (core).construct != NULL) {
      status = NW_Object_Aggregate_Construct (core, argList);
      NW_THROW_ON_ERROR (status);
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_Object_Aggregate_Terminate (NW_Object_Core_t* core)
{
  /* call the class destructor on the object */
  if (NW_Object_Aggregate_GetClassPart (core).destruct != NULL) {
    NW_Object_Aggregate_Destruct (core);
  }

  /* call our superclass method */
  _NW_Object_Core_Terminate (core);
}

