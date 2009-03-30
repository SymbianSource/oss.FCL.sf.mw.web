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


#include "nw_fbox_inputboxi.h"

#include "nw_fbox_inputskin.h"
#include "nw_fbox_inputinteractor.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
//#include "nw_fbox_passwordbox.h"
#include "nw_hed_documentroot.h"
#include "nw_fbox_formboxutils.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"
#include "nw_wml1x_wml1xformliaison.h"
#include "wml_elm_attr.h"
#include "nw_wml_core.h"
#include "nw_wml1x_wml1xcontenthandler.h"


/* ------------------------------------------------------------------------- *
   private data
 * ------------------------------------------------------------------------- */
  
/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_FBox_InputBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_FBox_InputBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_FBox_InputBox_InterfaceList[] = {
  &_NW_FBox_InputBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_FBox_InputBox_Class_t  NW_FBox_InputBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_FormBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_FBox_InputBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_InputBox_t),
    /* construct                  */ _NW_FBox_InputBox_Construct,
    /* destruct                   */ _NW_FBox_InputBox_Destruct
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_FBox_InputBox_Split,
    /* resize                     */ _NW_FBox_FormBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_FBox_FormBox_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_FBox_InputBox_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_InputBox_InitSkin,
    /* initInteractor             */ _NW_FBox_InputBox_InitInteractor,
    /* reset                      */ _NW_FBox_InputBox_Reset
  },
  { /* NW_FBox_InputBox           */
    /* getHeight                  */ _NW_FBox_InputBox_GetHeight,
    /* setHeight                  */ _NW_FBox_InputBox_SetHeight,
    /* setDisplayText             */ _NW_FBox_InputBox_SetDisplayText,
    /* modifyBuffer               */ _NW_FBox_InputBox_ModifyBuffer
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InputBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_InputBox_t* thisObj;
  TBrowserStatusCode status;
  NW_FBox_FormLiaison_t* formLiaison;
  void *controlId;
  NW_Text_t* buffer;
  NW_Bool    initialValue = NW_FALSE;

  /* for convenience */
  thisObj = NW_FBox_InputBoxOf (dynamicObject);
  buffer = NULL;
  /* invoke our superclass constructor */
  status = _NW_FBox_FormBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  thisObj->width = (NW_Uint16)va_arg (*argp, NW_Uint32);
  thisObj->appServices = va_arg (*argp, NW_HED_AppServices_t*);
  thisObj->title = NULL;
  thisObj->validator = NULL;
  thisObj->active = NW_FALSE;
  thisObj->focus = NW_FALSE;
  thisObj->maxChars = NW_FBox_InputBox_MaxCharacters;
  thisObj->IsMaxlength = NW_FALSE;
  thisObj->value = NULL;

  /* Get initial value from the form liaison */
  formLiaison =  NW_FBox_FormBox_GetFormLiaison(thisObj);
  controlId = NW_FBox_FormBox_GetFormCntrlID(thisObj);
  (void) NW_FBox_FormLiaison_GetStringValue(formLiaison, controlId, &buffer, &initialValue);
  thisObj->initialValueUsed = initialValue;
  
  if (buffer){
    NW_FBox_InputBox_SetDefaultVal(thisObj, buffer);
  }
  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_InputBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_InputBox_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_InputBoxOf (dynamicObject);

  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_InputBox_Class));

  /* Destroy our Value object */
  if(thisObj->value != NULL){
    NW_Object_Delete(thisObj->value);
  }
  if (thisObj->title != NULL) {
    NW_Str_Delete(thisObj->title);
  }
  if (thisObj->validator != NULL) {
    NW_Object_Delete(thisObj->validator);
  }
}

/* ------------------------------------------------------------------------- */
NW_Uint16
_NW_FBox_InputBox_GetHeight(NW_FBox_InputBox_t* box)
{
  NW_ASSERT(NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class));
  NW_REQUIRED_PARAM(box);
  return 1;
}

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

void
_NW_FBox_InputBox_SetHeight(NW_FBox_InputBox_t* box, NW_Uint16 height)
{
  NW_ASSERT(NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class));
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(height);  
//  box->height = height;
}

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_InputBox_SetDisplayText(NW_FBox_InputBox_t* thisObj,
                                NW_Bool bufferModificationOn)
{
  NW_FBox_InputSkin_t* skin;
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_InputBox_Class));
  NW_REQUIRED_PARAM(thisObj);
  NW_REQUIRED_PARAM(bufferModificationOn);
  skin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(thisObj));
  return NW_FBox_InputSkin_SetDisplayText(skin);
}


/* ------------------------------------------------------------------------- */
NW_FBox_Skin_t*
_NW_FBox_InputBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  NW_ASSERT(NW_Object_IsInstanceOf(formBox, &NW_FBox_InputBox_Class));
  return (NW_FBox_Skin_t *)NW_FBox_InputSkin_New(formBox);
}

/* ------------------------------------------------------------------------- */
NW_FBox_Interactor_t*
_NW_FBox_InputBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  NW_ASSERT(NW_Object_IsInstanceOf(formBox, &NW_FBox_InputBox_Class));
  return (NW_FBox_Interactor_t *)NW_FBox_InputInteractor_New(formBox);
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_InputBox_ModifyBuffer(NW_FBox_InputBox_t* inputBox, 
                              NW_Ucs2 c, 
                              NW_Uint32 repeated)
{
  NW_REQUIRED_PARAM(inputBox);
  NW_REQUIRED_PARAM(c);
  NW_REQUIRED_PARAM(repeated);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InputBox_Split(NW_LMgr_Box_t* box,
                         NW_GDI_Metric_t space,
                         NW_LMgr_Box_t* *splitBox,
                         NW_Uint8 flags)
{
  NW_FBox_InputSkin_t* inputSkin;
  NW_FBox_FormBox_t* formBox;

  if (flags & NW_LMgr_Box_SplitFlags_Nowrap) {
    *splitBox = NULL;
    return KBrsrSuccess;
  }
  *splitBox = NULL;
  formBox = NW_FBox_FormBoxOf(box);
  inputSkin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(formBox));
  return NW_FBox_InputSkin_Split(inputSkin, space, splitBox, flags);
}


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

NW_Text_Length_t
NW_FBox_InputBox_GetBufferLength(NW_FBox_InputBox_t* inputBox)
{
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));
  if (inputBox->value == NULL){
    return 0;
  }
  return NW_Text_GetCharCount(inputBox->value);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_InputBox_CheckFormLiaisonVal(NW_FBox_InputBox_t* inputBox)
{
  NW_Text_t* text;
  NW_FBox_FormLiaison_t* formLiaison;
  void* formControlId;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));

  /* get the UCS2 pointer to the value and construct a new Text
     object from this */
  text = NW_Text_Copy(inputBox->value, NW_TRUE);

  /* we pass ownership of the text object to the formLiaison */ 
  formLiaison = NW_FBox_FormBox_GetFormLiaison (inputBox);
  formControlId = NW_FBox_FormBox_GetFormCntrlID(inputBox);

  status = NW_FBox_FormLiaison_ValidateStringValue (formLiaison, formControlId, 
                                                    text);

  NW_Object_Delete( text );

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_InputBox_SetFormLiaisonVal(NW_FBox_InputBox_t* inputBox)
{
  NW_Text_t* text = NULL;
  NW_FBox_FormLiaison_t* formLiaison;
  void* formControlId;
  TBrowserStatusCode status;
  NW_Bool  dummy;

  // parameter assertion block
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));

  // get UCS2 pointer to value and construct new Text object from this
  if (inputBox->value != NULL){
    text = NW_Text_Copy(inputBox->value, NW_TRUE);
  }else{
    text = (NW_Text_t*) NW_Text_UCS2_New (NULL, 0, 0);
  }
  // pass ownership of text object to formLiaison
  formLiaison = NW_FBox_FormBox_GetFormLiaison (inputBox);
  formControlId = NW_FBox_FormBox_GetFormCntrlID(inputBox);

  status = NW_FBox_FormLiaison_SetStringValue (formLiaison, formControlId, 
                                               text);
  if (status == KBrsrWmlbrowserInputNonconformingToMask) {
    // Expected that may not match format mask.  Force to success so
    // will contine.
    status = KBrsrSuccess;
  } else if (status == KBrsrOutOfMemory) {
    return KBrsrOutOfMemory;
  } else if (status != KBrsrSuccess) {
    // Gracefully attempt to recover from other types of errors, at least
    // trying to keep the displayed text in synch with what the user entered.
    // Without this, expected possible errors such as KBrsrWmlbrowserBadContent
    // for example, will cause displayed text to be out of synch.
    return NW_FBox_InputBox_SetDisplayText (inputBox, NW_TRUE);
  }

  // get formLiaison's intrepretation of value and set that as default
  status =
    NW_FBox_FormLiaison_GetStringValue (formLiaison, formControlId, &text, &dummy);
  if (status != KBrsrSuccess){
    return status;
  }
  status = NW_FBox_InputBox_SetDefaultVal (inputBox, text);
  NW_ASSERT (status == KBrsrSuccess);
  return NW_FBox_InputBox_SetDisplayText (inputBox, NW_TRUE);
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
NW_FBox_InputBox_SetDefaultVal(NW_FBox_InputBox_t* thisObj, 
                                NW_Text_t* defaultVal)
{
  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_InputBox_Class));
  NW_ASSERT (defaultVal == NULL ||
             NW_Object_IsInstanceOf (defaultVal, &NW_Text_Abstract_Class));

  if (thisObj->value){
    NW_Object_Delete(thisObj->value);
    thisObj->value = NULL;
  }
  if (defaultVal == NULL){
    return KBrsrSuccess;
  }
  thisObj->value = NW_Text_Copy(defaultVal, NW_TRUE);
  if (thisObj->value == NULL){
    return KBrsrOutOfMemory;
  }
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
NW_FBox_InputBox_SetActive(NW_FBox_InputBox_t* inputBox, NW_Bool active)
{
  NW_AppServices_SoftKeyAPI_t* softKeyAPI;
  NW_FBox_InputSkin_t* inputSkin;
  TBrowserStatusCode status;

  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));
  
  inputBox->active = active;
  softKeyAPI = &(inputBox->appServices->softKeyApi);
  NW_ASSERT(softKeyAPI != NULL);

  if (active == NW_TRUE) {
    if (softKeyAPI->setLeft != NULL) {
      softKeyAPI->setLeft(NW_SOFTKEY_OK);
    }
    if (softKeyAPI->setRight != NULL) {
      softKeyAPI->setRight(NW_SOFTKEY_CLEAR);
    }
    inputSkin = NW_FBox_InputSkinOf(NW_FBox_FormBox_GetSkin(inputBox));
    status = NW_FBox_InputSkin_SetActive (inputSkin);
    if (status != KBrsrSuccess) {
      NW_FBox_InputBox_SetActive (inputBox, NW_FALSE);
      (void) inputBox->appServices->errorApi.notifyError (NULL, status, NULL, NULL);
    }
  }
  else {
    if (softKeyAPI->setLeft != NULL) {
      softKeyAPI->setLeft(NW_SOFTKEY_OPTIONS);
    }
    if (softKeyAPI->setRight != NULL) {
      softKeyAPI->setRight(NW_SOFTKEY_BACK);
    }
  }
}

/* --------------------------------------------------------------------------*/
void
NW_FBox_InputBox_SetFocus( NW_FBox_InputBox_t* inputBox, NW_Bool focus )
{
  NW_FBox_InputSkin_t* inputSkin;
  NW_ASSERT( NW_Object_IsInstanceOf( inputBox, &NW_FBox_InputBox_Class ) );
  inputSkin = NW_FBox_InputSkinOf( NW_FBox_FormBox_GetSkin( inputBox ) );
  NW_FBox_InputSkin_SetFocus( inputSkin, focus );
}

/* --------------------------------------------------------------------------*/
void
NW_FBox_InputBox_SetVKB( NW_FBox_InputBox_t* inputBox)
{
  NW_FBox_InputSkin_t* inputSkin;
  NW_ASSERT( NW_Object_IsInstanceOf( inputBox, &NW_FBox_InputBox_Class ) );
  inputSkin = NW_FBox_InputSkinOf( NW_FBox_FormBox_GetSkin( inputBox ) );
  NW_FBox_InputSkin_SetVKB( inputSkin);
}


/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_InputBox_Reset (NW_FBox_FormBox_t* box)
{
  TBrowserStatusCode status;
  NW_FBox_FormLiaison_t* formLiaison;
  void* formControlId;
  NW_Text_t *buffer;
  NW_FBox_InputBox_t* thisObj;
  NW_Bool dummy;

  /* Call superclass to NULL/reset values. */
  status = _NW_FBox_FormBox_Reset(box);
  if ( status != KBrsrSuccess ) {
    return status;
  }

  thisObj = NW_FBox_InputBoxOf (box);

  formControlId = NW_FBox_FormBox_GetFormCntrlID(thisObj);
  formLiaison = NW_FBox_FormBox_GetFormLiaison(NW_FBox_FormBoxOf(thisObj));

  /* SetInitialStringValue will retrieve the the default form value... */
  status = NW_FBox_FormLiaison_SetInitialStringValue(formLiaison, 
                                                     formControlId,
													 NW_FBox_InputBox_GetMaxChars(thisObj));
  if (status == KBrsrWmlbrowserInputNonconformingToMask) {
    /* Expected that may not match format mask.  Force to success so 
       will contine. */
    status = KBrsrSuccess;
  }

  status = NW_FBox_FormLiaison_GetStringValue(formLiaison, formControlId, &buffer, &dummy);
  NW_ASSERT(status == KBrsrSuccess);
  if (status != KBrsrSuccess){
    return status;
  }

  NW_FBox_InputBox_SetDefaultVal(thisObj, buffer);
  status = NW_FBox_InputBox_SetDisplayText(thisObj, NW_TRUE);
  if (status != KBrsrSuccess) {
    return status;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
NW_GDI_Metric_t
_NW_FBox_InputBox_GetBaseline (NW_LMgr_Box_t* box)
  {
  CGDIFont* font;
  TGDIFontInfo fontInfo;

  /* parameter assertion block */
  NW_ASSERT (box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class));

  /* Get the font info */
  font = NW_LMgr_Box_GetFont (box);
  if (font != NULL) 
    {
    NW_LMgr_FrameInfo_t padding;
    NW_LMgr_FrameInfo_t borderWidth;

    (void) font->GetFontInfo ( &fontInfo);

    /* Get the padding and the border width */
    NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameTop );
    NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameTop );

    return (NW_GDI_Metric_t)(fontInfo.baseline + borderWidth.top + padding.top); 
  }

  /* unable to get the FontInfo we return our height */
  return NW_LMgr_Box_GetFormatBounds(box).dimension.height;
}


/* --------------------------------------------------------------------------*/
TBrowserStatusCode
NW_FBox_InputBox_GetFormat(NW_FBox_InputBox_t* thisObj,
                           const NW_Text_t** format,
                           NW_FBox_Validator_Mode_t*  mode,
                           NW_FBox_Validator_EmptyOk_t* emptyOk)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_LMgr_Property_t prop;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_InputBox_Class));
  NW_ASSERT( format != NULL );
  NW_ASSERT( emptyOk != NULL );
  NW_ASSERT( mode != NULL );

  *format = NULL;
  *emptyOk = NW_FBox_Validator_EmptyOk_None;
  *mode = NW_FBox_Validator_Mode_None;

  /* Get and set -wap-input-format property. */
  status = NW_LMgr_Box_GetProperty( NW_LMgr_BoxOf(thisObj),
                                    NW_CSS_Prop_inputFormat,
                                    &prop );

  if ( ( status == KBrsrSuccess ) &&
       ( prop.type == NW_CSS_ValueType_Text ) ) {
    /* Found -wap-input-format property.  Set it on the box. */
    *format = NW_TextOf( prop.value.object );
  }

  /* Get and set -wap-input-required property. */
  status = NW_LMgr_Box_GetProperty( NW_LMgr_BoxOf(thisObj),
                                    NW_CSS_Prop_inputRequired,
                                    &prop );

  if ( ( status == KBrsrSuccess ) &&
       ( prop.type == NW_CSS_ValueType_Token ) ) {
    /* Found -wap-input-required property.  Set it on the box. */
    if ( prop.value.token == NW_CSS_PropValue_true ) {
      /* Input required is true.  Note this translates to emptyOk == FALSE. */
      *emptyOk = NW_FBox_Validator_EmptyOk_False;
    }
    else if ( prop.value.token == NW_CSS_PropValue_false ) {
      /* Input required is false.  Note this translates to emptyOk == TRUE. */
      *emptyOk = NW_FBox_Validator_EmptyOk_True;
    }
  }

  status = NW_LMgr_Box_GetProperty( NW_LMgr_BoxOf(thisObj), NW_CSS_Prop_istyle,
                                    &prop );
  if ( ( status == KBrsrSuccess ) &&
       ( prop.type == NW_CSS_ValueType_Integer ) ) {
    switch (prop.value.integer)
      {
      case 1:
        {
        *mode = NW_FBox_Validator_Mode_Hiragana;
        break;
        }
      case 2:
        {
        *mode = NW_FBox_Validator_Mode_Hankakukana;
        break;
        }
      case 3:
        {
        *mode = NW_FBox_Validator_Mode_Alphabet;
        break;
        }
      case 4:
        {
        *mode = NW_FBox_Validator_Mode_Numeric;
        break;
        }
      default:
        {
        break;
        }
      }
  }

  status = NW_LMgr_Box_GetProperty( NW_LMgr_BoxOf(thisObj), NW_CSS_Prop_mode,
                                    &prop );
  if ( ( status == KBrsrSuccess ) &&
       ( prop.type == NW_CSS_ValueType_Token ) ) {
    switch (prop.value.token)
      {
      case NW_CSS_PropValue_hiragana:
        {
        *mode = NW_FBox_Validator_Mode_Hiragana;
        break;
        }
      case NW_CSS_PropValue_katakana:
        {
        *mode = NW_FBox_Validator_Mode_Katakana;
        break;
        }
      case NW_CSS_PropValue_hankakukana:
        {
        *mode = NW_FBox_Validator_Mode_Hankakukana;
        break;
        }
      case NW_CSS_PropValue_alphabet:
        {
        *mode = NW_FBox_Validator_Mode_Alphabet;
        break;
        }
      case NW_CSS_PropValue_numeric:
        {
        *mode = NW_FBox_Validator_Mode_Numeric;
        break;
        }
      default:
        {
        break;
        }
      }
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Uint8
_NW_FBox_InputBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                               NW_Evt_Event_t* event)
{
  NW_FBox_InputBox_t* thisObj;
  NW_FBox_Interactor_t* interactor;
  NW_Uint8 eventAbsorbed;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  /* obtain the implementer */
  thisObj = (NW_FBox_InputBox_t*) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_InputBox_Class));

  thisObj = NW_FBox_InputBoxOf(thisObj);
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_InputBox_Class));

  interactor = NW_FBox_FormBox_GetInteractor(NW_FBox_FormBoxOf(thisObj));

  eventAbsorbed =
    NW_FBox_Interactor_ProcessEvent (interactor,
                                     NW_LMgr_ActiveBoxOf (thisObj), event);
  if (eventAbsorbed){
    return eventAbsorbed;
  }

  return _NW_LMgr_ActiveBox_IEventListener_ProcessEvent (eventListener, event);
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_FBox_InputBox_t*
NW_FBox_InputBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_Uint16 colulmns,
                      NW_HED_AppServices_t* appServices)
{
  return (NW_FBox_InputBox_t*)
    NW_Object_New (&NW_FBox_InputBox_Class, (NW_Uint32)numProperties, eventHandler, 
                  formCntrlId, formLiaison, (NW_Uint32)colulmns, appServices);
}



