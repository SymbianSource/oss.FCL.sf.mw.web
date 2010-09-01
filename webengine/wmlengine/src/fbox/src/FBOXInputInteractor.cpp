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


#include "nw_fbox_inputinteractori.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_fbox_inputskin.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_unicodekeyevent.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
const
NW_FBox_InputInteractor_Class_t  NW_FBox_InputInteractor_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_Interactor_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_InputInteractor_t),
    /* construct                   */ _NW_FBox_Interactor_Construct,
    /* destruct                */ NULL
  },
  { /* NW_FBox_Interactor                */
    /* processEvent            */ _NW_FBox_InputInteractor_ProcessEvent
  },
  { /* NW_FBox_InputInteractor */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_FBox_InputInteractor_ProcessEvent(
                        NW_FBox_Interactor_t* interactor,
                        NW_LMgr_ActiveBox_t* box,
                        NW_Evt_Event_t* event)
{
  NW_Evt_UnicodeKeyEvent_t *unicodeKeyEvent;
  NW_FBox_InputBox_t* inputBox;
  NW_Uint8 eventAbsorbed;

  NW_REQUIRED_PARAM(interactor);
  eventAbsorbed = NW_LMgr_EventNotAbsorbed;

  inputBox = NW_FBox_InputBoxOf(box);

  if (NW_Object_Core_GetClass (event) == &NW_Evt_FocusEvent_Class) {
    /* we are not doing anything for the gained focus event.
     * Only for lost focus event we commit the value of 
     * inputBox to the formLiaison
     */
    
    eventAbsorbed = NW_LMgr_EventAbsorbed;
    if(!NW_Evt_FocusEvent_GetHasFocus(event)) {
      TBrowserStatusCode status = KBrsrSuccess;
      /* We set the form liaison value in case the input element is active */
      if(NW_FBox_InputBox_IsActive(inputBox))
      {
        status = NW_FBox_InputBox_CheckFormLiaisonVal(inputBox);
      }
      if (status == KBrsrWmlbrowserInputNonconformingToMask) {
        eventAbsorbed = NW_LMgr_EventNotAbsorbed;

        /* Display dialog that user input did not match the format mask. */
        (void) inputBox->appServices->errorApi.notifyError(NULL, 
                  KBrsrWmlbrowserInputNonconformingToMask,
                  NULL, NULL);
      }
      else if (status == KBrsrSuccess) {
        /* Valid input.  Set the new value in the form liaison. */
        (void) NW_FBox_InputBox_SetFormLiaisonVal(inputBox);
      }
      NW_FBox_InputBox_SetActive(inputBox, NW_FALSE);
    }
  }
  else if (NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class)
  {
    if (!NW_FBox_InputBox_IsActive(inputBox)){
      NW_FBox_InputBox_SetActive(inputBox, NW_TRUE);
    }else{
      NW_FBox_InputBox_SetActive(inputBox, NW_FALSE);
      NW_FBox_InputBox_SetFormLiaisonVal(inputBox);
    }
    eventAbsorbed = NW_LMgr_EventAbsorbed;
  }
  else if (NW_Object_Core_GetClass (event) == &NW_Evt_AccessKeyEvent_Class)
  {
    if (!NW_FBox_InputBox_IsActive(inputBox)){
      NW_FBox_InputBox_SetActive(inputBox, NW_TRUE);
      eventAbsorbed = NW_LMgr_EventAbsorbed;
    }
  }
  else if (NW_Object_Core_GetClass (event) == &NW_Evt_UnicodeKeyEvent_Class)
  {
    unicodeKeyEvent = NW_Evt_UnicodeKeyEventOf(event);
    if (unicodeKeyEvent->keyVal == '#' && !NW_Evt_KeyEvent_IsLongPress(NW_Evt_KeyEventOf(event)))
    {
      TBrowserStatusCode status = KBrsrSuccess;
      if(status) {} // fix rvct compiler warning
      NW_FBox_InputSkin_t* inputSkin;
      inputSkin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(inputBox));
      if ((NW_Object_Core_GetClass(inputBox) != &NW_FBox_PasswordBox_Class) && 
          !NW_Evt_KeyEvent_IsLongPress(NW_Evt_KeyEventOf(event)))
      {
        status = NW_FBox_InputSkin_DetailsDialog(inputSkin);               
        NW_ASSERT(status == KBrsrSuccess);
      }
      eventAbsorbed = NW_LMgr_EventAbsorbed;
      status = KBrsrSuccess;
    }
  }

  if (eventAbsorbed == NW_LMgr_EventAbsorbed){
    NW_LMgr_Box_Refresh (NW_LMgr_BoxOf (box));
    return NW_LMgr_EventAbsorbed;
  }

  return NW_LMgr_EventNotAbsorbed;
}
