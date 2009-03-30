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


#include "nw_fbox_optionboxi.h"

#include "nw_evt_activateevent.h"
#include "nw_fbox_optionboxskin.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_FBox_OptionBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_FBox_OptionBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_FBox_OptionBox_InterfaceList[] = {
  &_NW_FBox_OptionBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_FBox_OptionBox_Class_t  NW_FBox_OptionBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_CheckBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_FBox_OptionBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_OptionBox_t),
    /* construct                  */ _NW_FBox_OptionBox_Construct,
    /* destruct                   */ _NW_FBox_OptionBox_Destruct
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_LMgr_Box_Split,
    /* resize                     */ _NW_FBox_FormBox_Resize,
  /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_FBox_FormBox_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_FBox_FormBox_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_LMgr_Box_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_OptionBox_InitSkin,
    /* initInteractor             */ _NW_FBox_OptionBox_InitInteractor,
    /* reset                      */ _NW_FBox_FormBox_Reset
  },
  { /* NW_FBox_CheckBox           */
    /* unused                     */ 0
  },
  { /* NW_FBox_OptionBox          */
    /* unused                     */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_OptionBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_OptionBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_OptionBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_CheckBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  thisObj->text = va_arg (*argp, NW_Text_t*);
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_FBox_OptionBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_OptionBox_t* thisObj;
  
  /* for convenience */
  thisObj = NW_FBox_OptionBoxOf (dynamicObject);

  /* Destroy our text object */
  NW_Object_Delete(thisObj->text);
  thisObj->text = NULL;
}

/* --------------------------------------------------------------------------*/
NW_FBox_Skin_t*
_NW_FBox_OptionBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_Skin_t *)NW_FBox_OptionBoxSkin_New(formBox);
}

/* --------------------------------------------------------------------------*/
NW_FBox_Interactor_t*
_NW_FBox_OptionBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  (void)formBox;
  return NULL;
}


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

NW_Bool
NW_FBox_OptionBox_IsMultiple(NW_FBox_OptionBox_t* optionBox)
{
  NW_FBox_FormLiaison_t* formLiason = NW_FBox_FormBox_GetFormLiaison(optionBox);

  return NW_FBox_FormLiaison_IsOptionMultiple(formLiason,
                                      NW_FBox_FormBox_GetFormCntrlID(optionBox));

}

NW_Bool
NW_FBox_OptionBox_IsSelected(NW_FBox_OptionBox_t* optionBox)
{
  NW_FBox_FormLiaison_t* formLiason = NW_FBox_FormBox_GetFormLiaison(optionBox);

  return NW_FBox_FormLiaison_IsOptionSelected(formLiason,
                                      NW_FBox_FormBox_GetFormCntrlID(optionBox));

}

NW_Bool
NW_FBox_OptionBox_HasOnPick(NW_FBox_OptionBox_t* optionBox)
{
  NW_FBox_FormLiaison_t* formLiason = NW_FBox_FormBox_GetFormLiaison(optionBox);

  return NW_FBox_FormLiaison_OptionHasOnPick(formLiason,
                                      NW_FBox_FormBox_GetFormCntrlID(optionBox));
}

NW_Bool
NW_FBox_OptionBox_IsLocalNavOnPick(NW_FBox_OptionBox_t* optionBox)
{
  NW_FBox_FormLiaison_t* formLiason = NW_FBox_FormBox_GetFormLiaison(optionBox);

  return NW_FBox_FormLiaison_IsLocalNavOnPick(formLiason,
                                      NW_FBox_FormBox_GetFormCntrlID(optionBox));
}
/* --------------------------------------------------------------------------*/
void
NW_FBox_OptionBox_ChangeState(NW_FBox_OptionBox_t* optionBox)
{
  NW_FBox_FormLiaison_t* formLiason = NW_FBox_FormBox_GetFormLiaison(optionBox);

  (void) NW_FBox_FormLiaison_ToggleBoolValue(formLiason,
      NW_FBox_FormBox_GetFormCntrlID(optionBox));
}

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Uint8
_NW_FBox_OptionBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                NW_Evt_Event_t* event)
{
  NW_FBox_OptionBox_t* thisObj;
  NW_FBox_FormLiaison_t* formLiaison;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  /* obtain the implementer */
  thisObj = (NW_FBox_OptionBox_t*) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_OptionBox_Class));

  formLiaison = NW_FBox_FormBox_GetFormLiaison(thisObj);

  if (NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class) {
    (void) NW_FBox_FormLiaison_ToggleBoolValue (formLiaison,
                                         NW_FBox_FormBox_GetFormCntrlID (thisObj));
    return NW_LMgr_EventAbsorbed;
  }

  return _NW_FBox_CheckBox_IEventListener_ProcessEvent (eventListener, event);
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_OptionBox_t*
NW_FBox_OptionBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_Text_t* text)
{
  return (NW_FBox_OptionBox_t*)
    NW_Object_New (&NW_FBox_OptionBox_Class, numProperties, eventHandler,
                           formCntrlId, formLiaison, text);
}
