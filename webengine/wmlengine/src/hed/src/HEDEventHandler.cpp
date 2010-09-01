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


#include "nw_hed_hedeventhandleri.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_EventHandler_Class_t NW_HED_EventHandler_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_EventHandler_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_HED_EventHandler_t),
    /* construct		 */ _NW_HED_EventHandler_Construct,
    /* destruct			 */ NULL
  },
  { /* NW_LMgr_EventHandler      */
    /* processEvent              */ _NW_HED_EventHandler_ProcessEvent
  },
  { /* NW_HED_EventHandler       */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_EventHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp)
{
  NW_HED_EventHandler_t* thisObj;

  /* for convenience */
  thisObj = NW_HED_EventHandlerOf (dynamicObject);

  /* initialize our member variables */
  thisObj->documentNode = va_arg (*argp, NW_HED_DocumentNode_t*);
  thisObj->context = va_arg (*argp, void*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_HED_EventHandler_ProcessEvent (NW_LMgr_EventHandler_t* eventHandler,
                                   NW_LMgr_Box_t* box,
                                   NW_Evt_Event_t* event)
{
  NW_HED_EventHandler_t* thisObj;

  /* for convenience */
  thisObj = NW_HED_EventHandlerOf (eventHandler);

  /* simply call the stored DocumentNode's 'ProcessEvent' method */
  return NW_HED_DocumentNode_ProcessEvent (thisObj->documentNode, box,
                                           event, thisObj->context);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_EventHandler_t*
NW_HED_EventHandler_New (NW_HED_DocumentNode_t* documentNode,
                         void* context)
{
  return (NW_HED_EventHandler_t*)
    NW_Object_New (&NW_HED_EventHandler_Class, documentNode, context);
}
