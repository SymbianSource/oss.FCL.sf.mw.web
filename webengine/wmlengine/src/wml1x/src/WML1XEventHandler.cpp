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


#include <nw_wml_core.h>
#include "nw_hed_documentrooti.h"
#include "nw_wml1x_wml1xeventhandleri.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_timerevent.h"
#include "nw_fbox_passwordbox.h"
#include "nw_evt_focusevent.h"   
#include "nw_evt_accesskeyevent.h" 
#include "nw_lmgr_activecontainerbox.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Wml1x_EventHandler_Class_t NW_Wml1x_EventHandler_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_EventHandler_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_Wml1x_EventHandler_t),
    /* construct		 */ _NW_Wml1x_EventHandler_Construct,
    /* destruct			 */ NULL
  },
  { /* NW_LMgr_EventHandler      */
    /* processEvent              */ _NW_Wml1x_EventHandler_ProcessEvent
  },
  { /* NW_Wml1x_EventHandler       */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Wml1x_EventHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp)
{
  NW_Wml1x_EventHandler_t* thisObj;

  /* for convenience */
  thisObj = NW_Wml1x_EventHandlerOf (dynamicObject);

  /* initialize our member variables */
  thisObj->ch = va_arg (*argp, NW_Wml1x_ContentHandler_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_Wml1x_EventHandler_ProcessEvent (NW_LMgr_EventHandler_t* eventHandler,
                                     NW_LMgr_Box_t* box,
                                     NW_Evt_Event_t* event)
{
  NW_Wml1x_EventHandler_t*  thisObj;
  NW_LMgr_PropertyValue_t    value;
  TBrowserStatusCode               status = KBrsrSuccess;
  NW_Uint8                  returnVal = !NW_LMgr_EventAbsorbed;

  /* for convenience */
  thisObj = NW_Wml1x_EventHandlerOf (eventHandler);

  if ((NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class) ||
      (NW_Object_Core_GetClass(event) == &NW_Evt_AccessKeyEvent_Class)) {
    /* use RB WML 1.x event handler */
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Integer, &value);

    if (status == KBrsrSuccess) {
      status = NW_Wml_GUIEvent(&(thisObj->ch->wmlInterpreter), 
                              NW_UINT16_CAST(value.integer), 
                              NULL); /* NULL will contain input parm */
    }

    returnVal = NW_LMgr_EventAbsorbed;

  }
  else if(NW_Object_Core_GetClass(event) == &NW_Evt_FocusEvent_Class) {
    if(NW_Object_Core_GetClass(box) == &NW_LMgr_ActiveContainerBox_Class) {
      returnVal = NW_LMgr_EventAbsorbed;
    }
  }

  if (status != KBrsrSuccess) {
    NW_HED_DocumentNode_HandleError (thisObj->ch, NULL, 
        BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t) status);
  }
  return returnVal;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Wml1x_EventHandler_t*
NW_Wml1x_EventHandler_New (NW_Wml1x_ContentHandler_t* ch)
{
  return (NW_Wml1x_EventHandler_t*)
    NW_Object_New (&NW_Wml1x_EventHandler_Class, ch);
}

