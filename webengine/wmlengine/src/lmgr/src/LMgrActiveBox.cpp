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


#include "nw_lmgr_activeboxi.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_LMgr_ActiveBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_LMgr_ActiveBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_LMgr_ActiveBox_InterfaceList[] = {
  &_NW_LMgr_ActiveBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_LMgr_ActiveBox_Class_t NW_LMgr_ActiveBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_LMgr_Box_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_LMgr_ActiveBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_LMgr_ActiveBox_t),
    /* construct                  */ _NW_LMgr_ActiveBox_Construct,
    /* destruct                   */ _NW_LMgr_ActiveBox_Destruct
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_LMgr_Box_Split,
    /* resize                     */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_LMgr_Box_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_LMgr_Box_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_ActiveBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_LMgr_ActiveBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_LMgr_ActiveBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_LMgr_Box_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the member variables */
  thisObj->eventHandler = va_arg (*argp, NW_LMgr_EventHandler_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_ActiveBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_ActiveBox_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_ActiveBoxOf (dynamicObject);

  /* destroy the EventHandler */
  NW_Object_Delete (thisObj->eventHandler);
}

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_LMgr_ActiveBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                NW_Evt_Event_t* event)
{
  NW_LMgr_ActiveBox_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  /* obtain the implementer */
  thisObj = (NW_LMgr_ActiveBox_t *) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_ActiveBox_Class));

  /* notify the event handler (if there is one) */
  if (thisObj->eventHandler != NULL) {
    return NW_LMgr_EventHandler_ProcessEvent (thisObj->eventHandler,
                                              NW_LMgr_BoxOf (thisObj), event);
  }
  return 0;
}
