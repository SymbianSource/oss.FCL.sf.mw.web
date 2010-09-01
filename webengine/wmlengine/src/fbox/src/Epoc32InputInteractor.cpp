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


#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "nw_fbox_epoc32inputinteractori.h"

#include "nw_fbox_inputbox.h"

#include "nw_evt_activateevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_navevent.h"
#include "nw_evt_unicodekeyevent.h"
#include "nw_evt_controlkeyevent.h"
#include "nw_lmgr_rootbox.h"

#include "nw_fbox_keymapper.h"
#include "nw_fbox_textareabox.h"
#include "nw_fbox_epoc32inputskin.h"
#include "nwx_time.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32InputInteractor_Class_t  NW_FBox_Epoc32InputInteractor_Class = {
  { /* NW_Object_Core                */
    /* super                         */ &NW_FBox_InputInteractor_Class,
    /* queryInterface                */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                */
    /* interfaceList                 */ NULL
  },
  { /* NW_Object_Dynamic             */
    /* instanceSize                  */ sizeof (NW_FBox_Epoc32InputInteractor_t),
    /* construct                     */ _NW_FBox_Interactor_Construct,
    /* destruct                      */ NULL
  },
  { /* NW_FBox_Interactor            */
    /* processEvent                  */ _NW_FBox_Epoc32InputInteractor_ProcessEvent
  },
  { /* NW_FBox_InputInteractor       */
    /* unused                        */ NW_Object_Unused
  },
  { /* NW_FBox_Epoc32InputInteractor */
    /* unused                        */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_FBox_Epoc32InputInteractor_ProcessEvent (NW_FBox_Interactor_t* interactor,
                        NW_LMgr_ActiveBox_t* box,
                        NW_Evt_Event_t* event)
{
  NW_FBox_InputBox_t* inputBox;
  NW_FBox_InputSkin_t* inputSkin;
  NW_Uint8 eventAbsorbed;

  NW_REQUIRED_PARAM(interactor);

  inputBox = NW_FBox_InputBoxOf(box);
  inputSkin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(inputBox));
  eventAbsorbed = NW_LMgr_EventNotAbsorbed;

  /* Deactivate inputBox */
  if (NW_Object_IsInstanceOf(event, &NW_Evt_ControlKeyEvent_Class) &&
      NW_FBox_InputBox_IsActive(inputBox))
    {
    NW_Evt_ControlKeyType_t keyVal;

    keyVal = NW_Evt_ControlKeyEvent_GetKeyVal(event);

    if (keyVal == NW_Evt_ControlKeyType_SoftA)
      {
      if (NW_FBox_Epoc32InputSkin_TryExitEditMode(NW_FBox_Epoc32InputSkinOf(inputSkin), NW_TRUE) != KBrsrSuccess)
        {
        return NW_LMgr_EventNotAbsorbed;
        }
      }
    else if (keyVal == NW_Evt_ControlKeyType_SoftB)
      {
      NW_FBox_Epoc32InputSkin_TryExitEditMode(NW_FBox_Epoc32InputSkinOf(inputSkin), NW_FALSE);
      }
    else
      {
      return NW_LMgr_EventNotAbsorbed;
      }
    eventAbsorbed = NW_LMgr_EventAbsorbed;
    }

  /* Activate inputBox */
  else if ((NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class) ||
           (NW_Object_Core_GetClass (event) == &NW_Evt_AccessKeyEvent_Class))
    {
    if (!NW_FBox_InputBox_IsActive(inputBox))
      {
      NW_FBox_InputBox_SetActive(inputBox, NW_TRUE);
      }
    else
      {
      NW_FBox_InputBox_SetVKB(inputBox);
      }
    eventAbsorbed = NW_LMgr_EventAbsorbed;
    }

  /* The activation made by UnicodeKeyEvent, in this case we have to repeat 
     the event for Edwin */
  else if (NW_Object_IsInstanceOf(event, &NW_Evt_UnicodeKeyEvent_Class))
    {
    TKeyEvent keyEvent;

    if (!NW_FBox_InputBox_IsActive(inputBox))
      {
      NW_FBox_InputBox_SetActive(inputBox, NW_TRUE);
      }
    NW_FBox_InputSkin_t* skin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(inputBox));
    NW_FBox_Epoc32InputSkin_t* epoc32InputSkin = NW_FBox_Epoc32InputSkinOf(skin);
    /* Make sure that we are active and the input is activated completly*/
    if (NW_FBox_InputBox_IsActive(inputBox) && epoc32InputSkin->cppEpoc32InputBox)
      {
      keyEvent.iCode      = ((NW_Evt_UnicodeKeyEvent_t*)event)->keyVal;
      keyEvent.iScanCode  = ((NW_Evt_UnicodeKeyEvent_t*)event)->keyVal;
      keyEvent.iModifiers = 0x00008001;
      keyEvent.iRepeats   = 0x00000000;
      CCoeEnv::Static()->SimulateKeyEventL( keyEvent, EEventKey );
      eventAbsorbed = NW_LMgr_EventAbsorbed;
      }
    }
  else if ( NW_Object_Core_GetClass( event ) == &NW_Evt_FocusEvent_Class ) 
    {
    if ( !NW_FBox_InputBox_IsFocus( inputBox ) )
      {
      NW_FBox_InputBox_SetFocus( inputBox, NW_TRUE );
      }
    else
      {
      NW_FBox_InputBox_SetFocus( inputBox, NW_FALSE );
      }
    eventAbsorbed = NW_LMgr_EventAbsorbed;
    }
  if (eventAbsorbed == NW_LMgr_EventAbsorbed)
    {
    return NW_LMgr_EventAbsorbed;
    }
  return NW_LMgr_EventNotAbsorbed;
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_InputInteractor_t*
NW_FBox_InputInteractor_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_InputInteractor_t*)
    NW_Object_New (&NW_FBox_Epoc32InputInteractor_Class, formBox);
}
