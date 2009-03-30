/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_fbox_checkboxi.h"

#include "nw_evt_activateevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_focusevent.h"
#include "nw_fbox_checkboxskin.h"
#include "nw_fbox_radiobox.h"
#include "nw_lmgr_rootbox.h"
#include "LMgrBoxTreeListener.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_FBox_CheckBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_FBox_CheckBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_FBox_CheckBox_InterfaceList[] = {
  &_NW_FBox_CheckBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_FBox_CheckBox_Class_t  NW_FBox_CheckBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_FormBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_FBox_CheckBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_CheckBox_t),
    /* construct                  */ _NW_FBox_CheckBox_Construct,
    /* destruct                   */ NULL
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_LMgr_Box_Split,
    /* resize                     */ _NW_FBox_FormBox_Resize,
    /* postResize                */  _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_FBox_FormBox_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_FBox_FormBox_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_FBox_FormBox_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_CheckBox_InitSkin,
    /* initInteractor             */ _NW_FBox_CheckBox_InitInteractor,
    /* reset                      */ _NW_FBox_FormBox_Reset
  },
  { /* NW_FBox_CheckBox           */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_CheckBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  TBrowserStatusCode status;
  NW_FBox_CheckBox_t* thisObj;
  /* invoke our superclass constructor */
  status = _NW_FBox_FormBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  /* for convenience */
  thisObj = NW_FBox_CheckBoxOf (dynamicObject);
  thisObj->focus = NW_FALSE;
  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
NW_FBox_Skin_t*
_NW_FBox_CheckBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_Skin_t *)NW_FBox_CheckBoxSkin_New(formBox);
}

/* --------------------------------------------------------------------------*/
NW_FBox_Interactor_t*
_NW_FBox_CheckBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  (void)formBox;
  return NULL;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Bool
NW_FBox_CheckBox_GetState (NW_FBox_CheckBox_t* thisObj)
{
  NW_FBox_FormLiaison_t* formLiason;
  NW_Bool state;

  /* no use checking for errors; cast to void */
  formLiason = NW_FBox_FormBox_GetFormLiaison(thisObj);
  state = NW_FALSE;
  (void) NW_FBox_FormLiaison_GetBoolValue (formLiason,
                                           NW_FBox_FormBox_GetFormCntrlID(thisObj),
                                           &state);

  return state;
}

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Uint8
_NW_FBox_CheckBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                               NW_Evt_Event_t* event)
{
  NW_FBox_CheckBox_t* thisObj;
  NW_FBox_FormLiaison_t* formLiason;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  /* obtain the implementer */
  thisObj = (NW_FBox_CheckBox_t*) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_CheckBox_Class));

  formLiason = NW_FBox_FormBox_GetFormLiaison(thisObj);

  if ((NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class) ||
      (NW_Object_Core_GetClass (event) == &NW_Evt_AccessKeyEvent_Class)) 
    {
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(thisObj));

    /* state change is taken care of by the form liaison */
    (void) NW_FBox_FormLiaison_ToggleBoolValue (formLiason,
                                         NW_FBox_FormBox_GetFormCntrlID (thisObj));

    (void)NW_LMgr_RootBox_GetBoxTreeListener(rootBox)->RedrawDisplay (NW_TRUE);
    /* we absorbed the event! */
    return NW_LMgr_EventAbsorbed;
    } 
  else if ( NW_Object_Core_GetClass( event ) == &NW_Evt_FocusEvent_Class ) 
    {
     if(NW_Evt_FocusEvent_GetHasFocus(event))
      {
      /* state change is taken care of by the form liaison */
      ( void ) NW_FBox_FormLiaison_Focus( 
                                    formLiason,
                                    NW_FBox_FormBox_GetFormCntrlID( thisObj ) );
      }
    return NW_LMgr_EventAbsorbed;
    }
  return _NW_LMgr_ActiveBox_IEventListener_ProcessEvent( eventListener, event );
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_FBox_CheckBox_t*
NW_FBox_CheckBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison)
{
  return (NW_FBox_CheckBox_t*)
    NW_Object_New (&NW_FBox_CheckBox_Class, numProperties, eventHandler,
                   formCntrlId, formLiaison);
}
