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


#include "nw_fbox_buttonboxi.h"
#include "nw_evt_ecmaevent.h"
#include "nwx_string.h"
#include "nw_evt_activateevent.h"
#include "nw_fbox_buttonboxskin.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_lmgr_rootbox.h"
#include "GDIDeviceContext.h"

#include "KimonoLocalizationStrings.h"
#include "BrsrStatusCodes.h"
#include <MemoryManager.h>

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
const
NW_LMgr_IEventListener_Class_t _NW_FBox_ButtonBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_FBox_ButtonBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
static
const NW_Object_Class_t* const _NW_FBox_ButtonBox_InterfaceList[] = {
  &_NW_FBox_ButtonBox_IEventListener_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_FBox_ButtonBox_Class_t  NW_FBox_ButtonBox_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_FBox_FormBox_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ _NW_FBox_ButtonBox_InterfaceList
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_FBox_ButtonBox_t),
    /* construct             */ _NW_FBox_ButtonBox_Construct,
    /* destruct              */ _NW_FBox_ButtonBox_Destruct
  },
  { /* NW_LMgr_Box           */
    /* split                 */ _NW_FBox_ButtonBox_Split,
    /* resize                */ _NW_FBox_FormBox_Resize,
  /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize   */ _NW_FBox_FormBox_HasFixedContentSize,
    /* constrain             */ _NW_LMgr_Box_Constrain,
    /* draw                  */ _NW_FBox_FormBox_Draw,
    /* render                */ _NW_LMgr_Box_Render,
    /* getBaseline           */ _NW_LMgr_ButtonBox_GetBaseline,
    /* shift                 */ _NW_LMgr_Box_Shift,
    /* clone                 */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox     */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox       */
    /* initSkin              */ _NW_FBox_ButtonBox_InitSkin,
    /* initInteractor        */ _NW_FBox_ButtonBox_InitInteractor,
    /* reset                 */ _NW_FBox_FormBox_Reset
  },
  { /* NW_FBox_ButtonBox     */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_ButtonBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_TRY(status) 
  {
    NW_FBox_ButtonBox_t* thisObj;
    NW_Text_UCS2_t* label;
    static const NW_Ucs2 submit[] = {'S','u','b','m','i','t','\0'};
    static const NW_Ucs2 reset[] = {'R','e','s','e','t','\0'};
    static const NW_Ucs2 blank[] = {' ','\0'};

    /* for convenience */
    thisObj = NW_FBox_ButtonBoxOf (dynamicObject);

    /* invoke our superclass constructor */
    status = _NW_FBox_FormBox_Construct (dynamicObject, argp);
    _NW_THROW_ON_ERROR(status);

    /* initialize the member variables */
    thisObj->buttonType = (NW_FBox_ButtonBox_Type_t)va_arg (*argp, NW_Uint32);
    thisObj->value = va_arg (*argp, NW_Text_t*);
    /*lint --e{550} Symbol not accessed */
    NW_Image_AbstractImage_t*image = va_arg (*argp, NW_Image_AbstractImage_t*);
    thisObj->appServices = va_arg (*argp, NW_HED_AppServices_t*); 

    if (thisObj->value == NULL)
    {
      label = NULL;
      if (thisObj->buttonType == NW_FBox_ButtonBox_Submit){
        NW_Ucs2* temp = thisObj->appServices->getLocalizedStringAPI.getLocalizedString( EKmLocSubmitButton );
        if (temp)
        {
          label = NW_Text_UCS2_New((NW_Ucs2*) temp, NW_Str_Strlen(temp), NW_TRUE);
        }
        else
        {
          label = NW_Text_UCS2_New((NW_Ucs2*) submit, NW_Str_Strlen(submit), 0);
        }
        NW_THROW_OOM_ON_NULL(label, status);
      }
      else if (thisObj->buttonType == NW_FBox_ButtonBox_Reset){
        NW_Ucs2* temp = thisObj->appServices->getLocalizedStringAPI.getLocalizedString( EKmLocResetButton );
        if (temp)
        {
          label = NW_Text_UCS2_New((NW_Ucs2*) temp, NW_Str_Strlen(temp), NW_TRUE);
        }
        else
        {
          label = NW_Text_UCS2_New((NW_Ucs2*) reset, NW_Str_Strlen(reset), 0);
        }
        NW_THROW_OOM_ON_NULL(label, status);
      }
      else if (thisObj->buttonType == NW_FBox_ButtonBox_Button){
        label =
          NW_Text_UCS2_New((NW_Ucs2*) blank, NW_Str_Strlen(blank), 0);
        NW_THROW_OOM_ON_NULL(label, status);
      }else {
        /* do nothing for now */
      }  
      thisObj->value = NW_TextOf(label);
    }

    NW_ASSERT(NW_Object_IsInstanceOf(thisObj->value, &NW_Text_Abstract_Class));

  } NW_CATCH (status)
  {
  }
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}


/* --------------------------------------------------------------------------*/
void
_NW_FBox_ButtonBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_ButtonBox_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_ButtonBoxOf (dynamicObject);

  /* Destroy our text object */
  if(thisObj->value != NULL){
    NW_Object_Delete(thisObj->value);
  }
}

/* --------------------------------------------------------------------------*/
NW_FBox_Skin_t*
_NW_FBox_ButtonBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_Skin_t *)NW_FBox_ButtonBoxSkin_New(formBox);
}

/* --------------------------------------------------------------------------*/
NW_FBox_Interactor_t*
_NW_FBox_ButtonBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  (void)formBox;
  return NULL;
}

/* --------------------------------------------------------------------------*/

NW_GDI_Metric_t
_NW_LMgr_ButtonBox_GetBaseline (NW_LMgr_Box_t* box)
  {
  CGDIFont* font;
  TGDIFontInfo fontInfo;

  /* parameter assertion block */
  NW_ASSERT (box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_ButtonBox_Class));

  /* Get the font info */
  font = NW_LMgr_Box_GetFont (box);
  if (font != NULL) 
    {
    NW_LMgr_FrameInfo_t padding;
    NW_LMgr_FrameInfo_t borderWidth;

    (void)font->GetFontInfo ( &fontInfo);

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
_NW_FBox_ButtonBox_Split(NW_LMgr_Box_t* box, 
                         NW_GDI_Metric_t space, 
                         NW_LMgr_Box_t* *splitBox,
                         NW_Uint8 flags)
{
  NW_ASSERT(box != NULL);

  *splitBox = NULL;
  /* If box does not fit in the space and the box is not
   on the new line the input box needs to be pushed on the new line */
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  if ((boxBounds.dimension.width > space) && !(flags & NW_LMgr_Box_SplitFlags_AtNewLine))
  {
    return KBrsrLmgrNoSplit;
  }

  if(boxBounds.dimension.width <= space) 
  {
    return KBrsrSuccess;
  }

  /* we are more than space */
  boxBounds.dimension.width = (NW_GDI_Metric_t)(space-3);
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   implemented interface methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_Uint8
_NW_FBox_ButtonBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                NW_Evt_Event_t* event)
{
  NW_FBox_ButtonBox_t* thisObj;
  NW_FBox_ButtonBox_Type_t type;
  NW_FBox_FormLiaison_t* formLiason;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  /* obtain the implementer */
  thisObj = (NW_FBox_ButtonBox_t*) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_ButtonBox_Class));

  type = NW_FBox_ButtonBox_GetButtonType(thisObj);
  formLiason = NW_FBox_FormBox_GetFormLiaison(thisObj);

  if ((NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class) ||
      (NW_Object_Core_GetClass (event) == &NW_Evt_AccessKeyEvent_Class))
    {
    // notify of the click event, 
    // if the ECMAScript is supported and  the element has OnClick attribute 
    // then the submit and reset function calls are handled after executing OnClick script.
    TBrowserStatusCode status = NW_FBox_FormLiaison_DelegateEcmaEvent (formLiason, 
                                             NW_FBox_FormBox_GetFormCntrlID(thisObj), 
                                             NW_Ecma_Evt_OnClick);
    if(status == KBrsrSuccess)
      {
      return NW_LMgr_EventAbsorbed;
      }
    // if the ecmascript is not supported or the element does not have the onclick attr
    // go ahead with submit or reset function
    if(status == KBrsrNotFound)
      {
      if (type == NW_FBox_ButtonBox_Submit)
        {
        /* all submit on form liason */
        status = NW_FBox_FormLiaison_Submit(formLiason, NW_FBox_FormBox_GetFormCntrlID (thisObj));
        if (status != KBrsrSuccess)
          {
          thisObj->appServices->errorApi.notifyError(NULL, status, NULL, NULL);
          }

        return NW_LMgr_EventAbsorbed;
        }
      else if (type == NW_FBox_ButtonBox_Reset)
        {
        /* all submit on form liason */
        (void) NW_FBox_FormLiaison_Reset(formLiason, NW_FBox_FormBox_GetFormCntrlID (thisObj));
        return NW_LMgr_EventAbsorbed;
        }
      }
    }
  else if ( NW_Object_Core_GetClass( event ) == &NW_Evt_FocusEvent_Class ) 
    {
    if ( NW_LMgr_Box_HasFocus( NW_LMgr_BoxOf( thisObj ) ) )
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
NW_FBox_ButtonBox_t*
NW_FBox_ButtonBox_New (NW_ADT_Vector_Metric_t numProperties,
                       NW_LMgr_EventHandler_t* eventHandler,
                       void* formCntrlId,
                       NW_FBox_FormLiaison_t* formLiaison,
                       NW_FBox_ButtonBox_Type_t buttonType,
                       NW_Text_t* value,
                       NW_Image_AbstractImage_t* image,
                       NW_HED_AppServices_t* appServices)
{
  return (NW_FBox_ButtonBox_t*)
    NW_Object_New (&NW_FBox_ButtonBox_Class, numProperties, eventHandler, 
                       formCntrlId, formLiaison, (NW_Uint32)buttonType, value, image, appServices);
}
