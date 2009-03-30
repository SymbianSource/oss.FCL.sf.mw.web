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


#include "nw_fbox_selectboxi.h"

#include "nw_evt_ecmaevent.h"

#include "nw_fbox_selectskin.h"
#include "nw_fbox_epoc32selectskin.h"
#include "nw_fbox_inputinteractor.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
#include "nw_fbox_passwordbox.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_unicodekeyevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_adt_resizablevector.h"
#include "nw_fbox_formboxutils.h"
#include "nw_hed_documentroot.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_FBox_SelectBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_FBox_SelectBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_FBox_SelectBox_InterfaceList[] = {
  &_NW_FBox_SelectBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_FBox_SelectBox_Class_t  NW_FBox_SelectBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_FormBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_FBox_SelectBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_SelectBox_t),
    /* construct                  */ _NW_FBox_SelectBox_Construct,
    /* destruct                   */ _NW_FBox_SelectBox_Destruct
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_FBox_SelectBox_Split,
    /* resize                     */ _NW_FBox_FormBox_Resize,
    /* postResize                 */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_FBox_FormBox_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_FBox_SelectBox_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_SelectBox_InitSkin,
    /* initInteractor             */ _NW_FBox_SelectBox_InitInteractor,
    /* reset                      */ _NW_FBox_SelectBox_Reset
  },
  { /* NW_FBox_SelectBox          */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_SelectBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_SelectBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_SelectBoxOf (dynamicObject);
  /* invoke our superclass constructor */
  status = _NW_FBox_FormBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  thisObj->appServices = va_arg (*argp, NW_HED_AppServices_t*);
  thisObj->multiple = (NW_Bool)va_arg(*argp,  NW_Uint32);
  thisObj->name = va_arg (*argp, NW_Text_t*);
  thisObj->title = va_arg(*argp, NW_Text_t*);
  thisObj->active = NW_FALSE;

  thisObj->children = (NW_ADT_DynamicVector_t*)
             NW_ADT_ResizableVector_New(sizeof(NW_FBox_FormBox_t*), 10, 5 );
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_SelectBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_SelectBox_t* thisObj;
  NW_ADT_Vector_Metric_t size;
  NW_ADT_Vector_Metric_t index;

  /* for convenience */
  thisObj = NW_FBox_SelectBoxOf (dynamicObject);

  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_SelectBox_Class));

  if (thisObj->children != NULL) {
    size =  NW_ADT_Vector_GetSize(thisObj->children);
    for (index =0; index <size; index++)
    {
      NW_FBox_OptionBox_t* optionBox;

      optionBox = *(NW_FBox_OptionBox_t**)
        NW_ADT_Vector_ElementAt (thisObj->children, index);
      NW_Object_Delete(optionBox);
    }

    NW_Object_Delete(thisObj->children);
  }

  NW_Object_Delete(thisObj->name);
  NW_Object_Delete(thisObj->title);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_SelectBox_SetDisplayText(NW_FBox_SelectBox_t* thisObj,
                                NW_Bool bufferModificationOn)
{

  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_SelectBox_Class));
  NW_REQUIRED_PARAM(thisObj);
  NW_REQUIRED_PARAM(bufferModificationOn);
  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */
NW_FBox_Skin_t*
_NW_FBox_SelectBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  NW_FBox_Skin_t  *skin;

  NW_ASSERT(NW_Object_IsInstanceOf(formBox, &NW_FBox_SelectBox_Class));
  skin = (NW_FBox_Skin_t *)NW_FBox_SelectSkin_New(formBox);
  if ((skin != NULL) && NW_Object_IsInstanceOf(skin, &NW_FBox_SelectSkin_Class))
  {
    NW_FBox_SelectSkin_Reserve(NW_FBox_SelectSkinOf(skin));
  }
  return skin;
}

/* ------------------------------------------------------------------------- */
NW_FBox_Interactor_t*
_NW_FBox_SelectBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  NW_ASSERT(NW_Object_IsInstanceOf(formBox, &NW_FBox_SelectBox_Class));
  return (NW_FBox_Interactor_t *)NW_FBox_InputInteractor_New(formBox);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_SelectBox_Split(NW_LMgr_Box_t* box,
                         NW_GDI_Metric_t space,
                         NW_LMgr_Box_t* *splitBox,
                         NW_Uint8 flags)
{
  NW_FBox_SelectSkin_t* selectSkin;
  NW_FBox_FormBox_t* formBox;

  if (flags & NW_LMgr_Box_SplitFlags_Nowrap) {
    *splitBox = NULL;
    return KBrsrSuccess;
  }
  *splitBox = NULL;
  formBox = NW_FBox_FormBoxOf(box);
  selectSkin = NW_FBox_SelectSkinOf(NW_FBox_FormBox_GetSkin(formBox));
  return NW_FBox_SelectSkin_Split(selectSkin, space, splitBox, flags);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

void
NW_FBox_SelectBox_AddOption(NW_FBox_SelectBox_t* selectBox, 
                            NW_FBox_OptionBox_t* optionBox,
                            NW_LMgr_Box_t** styleBox)
{
  NW_FBox_SelectSkin_t* selectSkin;

  (void) NW_ADT_DynamicVector_InsertAt(selectBox->children,
		                                   &optionBox, NW_ADT_Vector_AtEnd);
  selectSkin = (NW_FBox_SelectSkin_t*)
               NW_FBox_FormBox_GetSkin(selectBox);
  (void) NW_FBox_SelectSkin_AddOption(selectSkin, optionBox,styleBox);
}

/* ------------------------------------------------------------------------- */
void
NW_FBox_SelectBox_AddOptGrp(NW_FBox_SelectBox_t* selectBox, 
                            NW_FBox_OptGrpBox_t* optGrpBox)
{
  NW_FBox_SelectSkin_t* selectSkin;

  (void) NW_ADT_DynamicVector_InsertAt(selectBox->children,
		                                   &optGrpBox, NW_ADT_Vector_AtEnd);
  selectSkin = (NW_FBox_SelectSkin_t*)
               NW_FBox_FormBox_GetSkin(selectBox);
  (void) NW_FBox_SelectSkin_AddOptGrp(selectSkin, optGrpBox);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_SelectBox_SetFormLiaisonVal(NW_FBox_SelectBox_t* inputBox)
{
  NW_FBox_FormLiaison_t* formLiaison;
  NW_Ucs2* storage;
  NW_Text_Length_t length;
  NW_Bool freeNeeded;
  NW_Text_t* text;
  void* formControlId;
  TBrowserStatusCode status;
  NW_Bool   dummy;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_SelectBox_Class));

  /* build a UCS2 text object with the inputBox name */
  storage = 
    NW_Text_GetUCS2Buffer (inputBox->name,
                      NW_Text_Flags_Aligned | NW_Text_Flags_NullTerminated, 
                           &length, &freeNeeded);
  if (storage == NULL){
    return KBrsrOutOfMemory;
  }
  text = (NW_Text_t*)
    NW_Text_UCS2_New (storage, length,
                      (NW_Uint8) (freeNeeded ? NW_Text_Flags_TakeOwnership : 0));
  if (text == NULL) {
    return KBrsrOutOfMemory;
  }

  /* formLiaison is responsible for freeing the text text object */ 
  formControlId = NW_FBox_FormBox_GetFormCntrlID(inputBox);
  formLiaison = NW_FBox_FormBox_GetFormLiaison (inputBox);
  status = NW_FBox_FormLiaison_SetStringValue (formLiaison, formControlId, text);
  if (status != KBrsrSuccess){
    return status;
  }

  status =
    NW_FBox_FormLiaison_GetStringValue (formLiaison, formControlId, &text, &dummy);
  if (status != KBrsrSuccess){
    return status;
  }
  inputBox->name = text;

  return status;
}

/* --------------------------------------------------------------------------*/
void
NW_FBox_SelectBox_SetActive(NW_FBox_SelectBox_t* inputBox, NW_Bool active)
{
  NW_FBox_SelectSkin_t* skin;
  if (active){
    skin = NW_FBox_SelectSkinOf(NW_FBox_FormBox_GetSkin(inputBox));
    TBrowserStatusCode status = NW_FBox_SelectSkin_SetActive(skin);
    NW_ASSERT(status == KBrsrSuccess);
  }
}

/* --------------------------------------------------------------------------*/
static 
void
NW_FBox_SelectBox_ResetOption(NW_FBox_OptionBox_t* optionBox, NW_Bool *value)
{
  if(*value == NW_TRUE)
  {
    (void) NW_FBox_FormLiaison_SetBoolValue(NW_FBox_FormBox_GetFormLiaison(optionBox),
                                     NW_FBox_FormBox_GetFormCntrlID(optionBox), value);
  }
  else if(NW_FBox_OptionBox_IsSelected(optionBox))
  {
    *value = NW_TRUE;
    (void) NW_FBox_FormLiaison_SetBoolValue(NW_FBox_FormBox_GetFormLiaison(optionBox),
                                     NW_FBox_FormBox_GetFormCntrlID(optionBox), value);
  }
  else
  {
    (void) NW_FBox_FormLiaison_SetBoolValue(NW_FBox_FormBox_GetFormLiaison(optionBox),
                                     NW_FBox_FormBox_GetFormCntrlID(optionBox), NULL);
  }

  return;
}

/* --------------------------------------------------------------------------*/
static 
void
NW_FBox_SelectBox_ResetOptgrp(NW_FBox_OptGrpBox_t* optGrpBox, NW_Bool* isFirstOption)
{
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(optGrpBox->children));

  for(index = 0; index < numItems; index++)
  {
    NW_FBox_FormBox_t* formBox;
    formBox = *(NW_FBox_FormBox_t**)
      NW_ADT_Vector_ElementAt(optGrpBox->children, index);

    if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
    {   
      NW_Bool value = NW_FALSE;
      if((*isFirstOption == NW_TRUE) && 
		 !NW_FBox_OptionBox_IsMultiple(NW_FBox_OptionBoxOf(formBox)))
      {
        value = NW_TRUE;
        *isFirstOption = NW_FALSE;
      }
      NW_FBox_SelectBox_ResetOption(NW_FBox_OptionBoxOf(formBox), &value);
    }
    else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class))
    {
      /* recurse here */
      NW_FBox_SelectBox_ResetOptgrp(NW_FBox_OptGrpBoxOf(formBox), isFirstOption);
    }
    else{
      NW_ASSERT(0);
    }
  }

  return;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_SelectBox_Reset (NW_FBox_FormBox_t* box)
{
  TBrowserStatusCode status;
  NW_FBox_SelectSkin_t* selectSkin;
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;
  NW_FBox_SelectBox_t* selectBox;
  NW_Bool isFirstOption = NW_TRUE;

  selectBox = NW_FBox_SelectBoxOf (box);
  selectSkin = NW_FBox_SelectSkinOf(NW_FBox_FormBox_GetSkin(box));
  numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(selectBox->children));

  for(index = 0; index < numItems; index++)
  {
    NW_FBox_FormBox_t* formBox;
    formBox = *(NW_FBox_FormBox_t**)
      NW_ADT_Vector_ElementAt(selectBox->children, index);

    if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
    {   
      NW_Bool value = NW_FALSE;
      if((isFirstOption == NW_TRUE) && 
		 !NW_FBox_OptionBox_IsMultiple(NW_FBox_OptionBoxOf(formBox)))
      {
        value = NW_TRUE;
        isFirstOption = NW_FALSE;
      }
      NW_FBox_SelectBox_ResetOption(NW_FBox_OptionBoxOf(formBox), &value);
    }
    else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class))
    {
      NW_FBox_SelectBox_ResetOptgrp(NW_FBox_OptGrpBoxOf(formBox), &isFirstOption);
    }
    else{
      NW_ASSERT(0);
    }
  }

  status = NW_FBox_SelectSkin_RefreshText(selectSkin);

  return status;
}

// ------------------------------------------------------------------------- 
NW_GDI_Metric_t
_NW_FBox_SelectBox_GetBaseline (NW_LMgr_Box_t* box)
  {
    NW_FBox_FormBox_t* formBox;
    NW_FBox_Epoc32SelectSkin_t* epoc32SelectSkin;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( box, &NW_FBox_FormBox_Class ) );

    // Get the 'skin' class through the form.
    formBox = NW_FBox_FormBoxOf( box );
    NW_ASSERT( formBox );
    epoc32SelectSkin = NW_FBox_Epoc32SelectSkinOf( NW_FBox_FormBox_GetSkin( formBox ) );
    NW_ASSERT( epoc32SelectSkin );

    // Delegate the operation to the skin class.
    return NW_FBox_Epoc32SelectSkin_GetBaseline(epoc32SelectSkin, box );
  }

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Uint8
_NW_FBox_SelectBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                NW_Evt_Event_t* event)
  { 
  NW_FBox_SelectBox_t* thisObj;
  NW_Evt_UnicodeKeyEvent_t *unicodeKeyEvent;
  NW_Uint8 eventAbsorbed;
  
  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
    &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));
  
  // obtain the implementer 
  thisObj = (NW_FBox_SelectBox_t*) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_SelectBox_Class));
  
  eventAbsorbed = NW_LMgr_EventNotAbsorbed;
  if( NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class || 
      NW_Object_Core_GetClass( event ) == &NW_Evt_AccessKeyEvent_Class ) 
    {
    NW_FBox_FormLiaison_t* formLiaison= NW_FBox_FormBox_GetFormLiaison (thisObj);
    
    if( NW_FBox_FormLiaison_DelegateEcmaEvent (formLiaison, 
      NW_FBox_FormBox_GetFormCntrlID(thisObj), 
      NW_Ecma_Evt_OnFocus) == KBrsrNotFound)
        {
        (void)NW_FBox_FormLiaison_DelegateEcmaEvent (formLiaison, 
                                           NW_FBox_FormBox_GetFormCntrlID(thisObj), 
                                           NW_Ecma_Evt_OnClick);
        }
    
    if (!thisObj->active) 
      {
      NW_FBox_SelectBox_SetActive(thisObj, NW_TRUE);
      } 
    else 
      {
      NW_FBox_SelectBox_SetActive(thisObj, NW_FALSE);
      NW_FBox_SelectBox_SetFormLiaisonVal(thisObj);
      }
    eventAbsorbed = NW_LMgr_EventAbsorbed;
    } 
  else if( NW_Object_Core_GetClass( event ) == &NW_Evt_FocusEvent_Class )
    {
    eventAbsorbed = NW_LMgr_EventAbsorbed;
    } 
  else if (NW_Object_Core_GetClass (event) == &NW_Evt_UnicodeKeyEvent_Class) 
    {
    unicodeKeyEvent = NW_Evt_UnicodeKeyEventOf(event);
    if ((unicodeKeyEvent->keyVal == '#') && !NW_Evt_KeyEvent_IsLongPress(NW_Evt_KeyEventOf(event))) 
      {
      NW_FBox_SelectSkin_t* selectSkin;
      selectSkin = NW_FBox_SelectSkinOf(NW_FBox_FormBox_GetSkin(thisObj));
      TBrowserStatusCode status = NW_FBox_SelectSkin_DetailsDialog(selectSkin);               
      NW_ASSERT(status == KBrsrSuccess);
      eventAbsorbed = NW_LMgr_EventAbsorbed;
      }
    }
  if (eventAbsorbed == NW_LMgr_EventAbsorbed) 
    {
    return NW_LMgr_EventAbsorbed;
    }
  
  return _NW_LMgr_ActiveBox_IEventListener_ProcessEvent (eventListener, event);
  }

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_SelectBox_t*
NW_FBox_SelectBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_HED_AppServices_t* appServices,
                      NW_Bool multiple,
                      NW_Text_t* name,
                      NW_Text_t* title)
{
  return (NW_FBox_SelectBox_t*)
    NW_Object_New (&NW_FBox_SelectBox_Class, numProperties, eventHandler, 
                  formCntrlId, formLiaison, appServices, (NW_Uint32 )multiple, name, title);
}



