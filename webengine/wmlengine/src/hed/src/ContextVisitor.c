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


#include "nw_hed_contextvisitori.h"

#include "nw_hed_context.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_ContextVisitor_Class_t NW_HED_ContextVisitor_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Object_Dynamic_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_HED_ContextVisitor_t),
    /* construct             */ _NW_HED_ContextVisitor_Construct,
    /* destruct              */ NULL
  },
  { /* NW_HED_ContextVisitor */
    /* unused                */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ContextVisitor_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_HED_ContextVisitor_t* thisObj;

  /* for convenience */
  thisObj = NW_HED_ContextVisitorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->context = va_arg (*argp, NW_HED_Context_t*);
  NW_ASSERT (thisObj->context != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->context, &NW_HED_Context_Class));

  thisObj->current = 0;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_Context_Variable_t*
NW_HED_ContextVisitor_NextVariable (NW_HED_ContextVisitor_t* thisObj)
{
  const NW_HED_Context_Variable_t* var;

  NW_ASSERT (thisObj);

  var = NW_HED_Context_GetEntry (thisObj->context, thisObj->current);
  
  if (var) {
    thisObj->current++;
  }

  return var;
}

/* ------------------------------------------------------------------------- */
void
NW_HED_ContextVisitor_Reset (NW_HED_ContextVisitor_t* thisObj)
{
  NW_ASSERT (thisObj);

  thisObj->current = 0;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_ContextVisitor_Initialize (NW_HED_ContextVisitor_t* thisObj,
                                  const NW_HED_Context_t* context)
{
  return NW_Object_Initialize (&NW_HED_ContextVisitor_Class, thisObj, context);
}

/* ------------------------------------------------------------------------- */
NW_HED_ContextVisitor_t*
NW_HED_ContextVisitor_New (NW_HED_Context_t* context)
{
  return (NW_HED_ContextVisitor_t*)
    NW_Object_New (&NW_HED_ContextVisitor_Class, context);
}
