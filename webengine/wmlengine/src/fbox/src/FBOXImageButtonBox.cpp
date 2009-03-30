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


#include "nw_fbox_imagebuttonboxi.h"

#include "nwx_string.h"
#include "nw_evt_activateevent.h"
#include "nw_fbox_imagebuttonboxskin.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_lmgr_rootbox.h"
#include "GDIDeviceContext.h"
#include "nw_image_virtualimage.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_ImageButtonBox_Class_t  NW_FBox_ImageButtonBox_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_FBox_FormBox_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* secondaryList         */ _NW_FBox_ImageButtonBox_SecondaryList
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_FBox_ImageButtonBox_t),
    /* construct             */ _NW_FBox_ImageButtonBox_Construct,
    /* destruct              */ _NW_FBox_ImageButtonBox_Destruct,
  },
  { /* NW_LMgr_Box           */
    /* split                 */ _NW_FBox_ImageButtonBox_Split,
    /* resize                */ _NW_FBox_FormBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize   */ _NW_FBox_FormBox_HasFixedContentSize,
    /* constrain             */ _NW_LMgr_Box_Constrain,
    /* draw                  */ _NW_FBox_ImageButtonBox_Draw,
    /* render                */ _NW_LMgr_Box_Render,
    /* getBaseline           */ _NW_FBox_ImageButtonBox_GetBaseline,
    /* shift                 */ _NW_LMgr_Box_Shift,
    /* clone                 */ _NW_LMgr_Box_Clone
    
  },
  { /* NW_LMgr_ActiveBox     */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox       */
    /* initSkin              */ _NW_FBox_ImageButtonBox_InitSkin,
    /* initInteractor        */ _NW_FBox_ImageButtonBox_InitInteractor,
    /* reset                 */ _NW_FBox_FormBox_Reset
  },
  { /* NW_FBox_ImageButtonBox*/
    /* unused                */ NW_Object_Unused     
  }
};

/* ------------------------------------------------------------------------- */
const NW_Object_Class_t* const _NW_FBox_ImageButtonBox_SecondaryList[] = {
  &_NW_FBox_ImageButtonBox_IEventListener_Class,
  &_NW_LMgr_AnimatedImageButtonBoxAggregate_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const
NW_LMgr_IEventListener_Class_t _NW_FBox_ImageButtonBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ActiveBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_FBox_ImageButtonBox_IEventListener_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
const NW_LMgr_AnimatedImageAggregate_Class_t _NW_LMgr_AnimatedImageButtonBoxAggregate_Class = {
  { /* NW_Object_Core                */
    /* super                         */ &NW_LMgr_AnimatedImageAggregate_Class,
    /* querySecondary                */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary           */
    /* offset                        */ offsetof (NW_FBox_ImageButtonBox_t, NW_LMgr_AnimatedImageAggregate)
  },
  { /* NW_Object_Aggregate           */
    /* secondaryList                 */ NULL,
    /* construct                     */ NULL,
    /* destruct                      */ NULL
  },
  { /* NW_LMgr_AnimatedImageAggregate*/
    /* animate                       */ _NW_LMgr_AnimatedImageAggregate_Animate,
    /* getImage                      */ _NW_FBox_ImageButtonBox_AnimatedImageAggregate_GetImage
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_ImageButtonBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_FBox_ImageButtonBox_t* thisObj;
  TBrowserStatusCode status;
  NW_Text_UCS2_t* label;
  static const NW_Ucs2 noText[] = {'\0'};

  /* for convenience */
  thisObj = NW_FBox_ImageButtonBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_FormBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the member variables */

  thisObj->value = va_arg (*argp, NW_Text_t*);
  thisObj->image = va_arg (*argp, NW_Image_AbstractImage_t*);
  

  if (thisObj->value == NULL) {  
    label = NULL;
    label = NW_Text_UCS2_New((NW_Ucs2*) noText, NW_Str_Strlen(noText), 0);  
    thisObj->value = NW_TextOf(label);
  }
  else
  {
    NW_Ucs2* storage = NULL;
    NW_Bool freeNeeded = NW_FALSE;
    NW_Uint16 flags = 0;
    NW_Text_Length_t len = 0;

    /* compress whitespaces */
    flags = NW_Text_Flags_Aligned | NW_Text_Flags_Copy | NW_Text_Flags_NullTerminated;
    flags |= NW_Text_Flags_CompressWhitespace;
    storage = NW_Text_GetUCS2Buffer (thisObj->value, flags, &len, &freeNeeded);
    if (storage == NULL){
      return KBrsrOutOfMemory;
    }
    status = NW_Text_SetStorage(thisObj->value, storage, len, freeNeeded);
  }
  
  return status;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_ImageButtonBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_ImageButtonBox_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_ImageButtonBoxOf (dynamicObject);
  
  /* Destroy our text object */
  if(thisObj->value != NULL)
    {
    NW_Object_Delete(thisObj->value);
  }
  // destroy virtual image -do not destroy canned image though
  if( NW_Object_IsInstanceOf( thisObj->image, &NW_Image_Virtual_Class ) == NW_TRUE )
    {
    NW_Object_Delete( thisObj->image );
    }
}

/* ------------------------------------------------------------------------- */
/* NW_LMgr_Box           */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_ImageButtonBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus)
{
  NW_FBox_ImageButtonBox_t* thisObj;
  NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate;
  TBrowserStatusCode status;

  thisObj = NW_FBox_ImageButtonBoxOf (box);

  /* invoke our superclass draw method first */
  status =  _NW_FBox_FormBox_Draw (box, deviceContext, hasFocus);
  
  /* support for animations. Note the method checks for animations also, i.e. 
     ImageButtonBox need not be concerned wether the image is animated or not - vishy */

  /* Get the aggregate object */
  AnimatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t*)
        NW_Object_QuerySecondary (thisObj, &NW_LMgr_AnimatedImageAggregate_Class);
  status = NW_LMgr_AnimatedImageAggregate_Animate (AnimatedImageAggregate); 

  return status;

}

/* ------------------------------------------------------------------------- */
/* NW_FBox_FormBox       */
/* ------------------------------------------------------------------------- */

NW_FBox_Skin_t*
_NW_FBox_ImageButtonBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_Skin_t *)NW_FBox_ImageButtonBoxSkin_New(formBox);
}

/* --------------------------------------------------------------------------*/
NW_FBox_Interactor_t*
_NW_FBox_ImageButtonBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  (void)formBox;
  return NULL;
}

/* --------------------------------------------------------------------------*/

NW_GDI_Metric_t
_NW_FBox_ImageButtonBox_GetBaseline (NW_LMgr_Box_t* box)
  {
  CGDIFont* font;
  TGDIFontInfo fontInfo;
  NW_FBox_ImageButtonBox_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_ImageButtonBox_Class));

  /* for convenience */
  thisObj = NW_FBox_ImageButtonBoxOf (box);

  if ( thisObj->image )
    {
    return NW_LMgr_Box_GetFormatBounds(box).dimension.height;
    }

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


/* ------------------------------------------------------------------------- *
   implemented interface/aggregate methods
 * ------------------------------------------------------------------------- */

NW_Image_AbstractImage_t*
_NW_FBox_ImageButtonBox_AnimatedImageAggregate_GetImage (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate)
{
  NW_FBox_ImageButtonBox_t* thisObj;

  /* obtain the implementer */
  thisObj =  (NW_FBox_ImageButtonBox_t*) NW_Object_Interface_GetImplementer (AnimatedImageAggregate);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_ImageButtonBox_Class));

  return thisObj->image;
}


/* --------------------------------------------------------------------------*/
NW_Uint8
_NW_FBox_ImageButtonBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                     NW_Evt_Event_t* event)
  {
  NW_FBox_ImageButtonBox_t* thisObj;
  NW_FBox_FormLiaison_t* formLiason;
  TBrowserStatusCode status = KBrsrSuccess;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
    &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));
  
  /* obtain the implementer */
  thisObj = (NW_FBox_ImageButtonBox_t*) NW_Object_Interface_GetImplementer (eventListener);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_ImageButtonBox_Class));
  
  formLiason = NW_FBox_FormBox_GetFormLiaison( thisObj );

  if ((NW_Object_Core_GetClass (event) == &NW_Evt_ActivateEvent_Class) ||
    (NW_Object_Core_GetClass (event) == &NW_Evt_AccessKeyEvent_Class))
    {    
    // set the button active in order that we can send .x .y data
    // at submit time
    void* formControlId = NW_FBox_FormBox_GetFormCntrlID( thisObj );
    
    // "on" is the keyword
    NW_Ucs2 onStr[] = { 'o', 'n', 0 };
    NW_Text_t* onText = NW_Text_New( HTTP_iso_10646_ucs_2, onStr, 
      NW_Str_Strlen( onStr ), NW_Text_Flags_Copy );
    // cannot handle OOM properly
    if( onText )
      {
      // SetStringValue takes onText's ownership, unless it fails.
      if( NW_FBox_FormLiaison_SetStringValue( formLiason, formControlId, onText )
          != KBrsrSuccess )
        {
        NW_Object_Delete( onText );
        }
      }
    // notify of the click event, 
    // if the ECMAScript is supported and  the element has OnClick attribute 
    // then the submit and reset function calls are handled after executing OnClick script.
    status = NW_FBox_FormLiaison_DelegateEcmaEvent( formLiason, formControlId, NW_Ecma_Evt_OnClick );
    
    if( status == KBrsrSuccess )
      {
      return NW_LMgr_EventAbsorbed;
      }
    
    /* all submit on form liason */
    (void)NW_FBox_FormLiaison_Submit( formLiason, NW_FBox_FormBox_GetFormCntrlID( thisObj ) );
    return NW_LMgr_EventAbsorbed;   
    }
  else if(NW_Object_Core_GetClass(event) == &NW_Evt_FocusEvent_Class) {
    return NW_LMgr_EventAbsorbed;
    }
  
  return _NW_LMgr_ActiveBox_IEventListener_ProcessEvent (eventListener, event);
  }

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_ImageButtonBox_Split(NW_LMgr_Box_t* box, 
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

  /* if width was specified we do not want to change it /
  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
  if (status == KBrsrSuccess){
    return status;
  }*/
  boxBounds.dimension.width = space;
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return KBrsrSuccess;
}


/* --------------------------------------------------------------------------*/
void
NW_LMgr_ImageButtonBox_ReplaceBrokenImage( NW_LMgr_Box_t* box, NW_Image_AbstractImage_t* image )
  {
  NW_FBox_ImageButtonBox_t* thisObj;

  NW_ASSERT( box != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf ( box, &NW_FBox_ImageButtonBox_Class ) );

  thisObj = NW_FBox_ImageButtonBoxOf( box );

  // dont free canned image
  // but replace it
  thisObj->image = image;  
  }

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
NW_FBox_ImageButtonBox_t*
NW_FBox_ImageButtonBox_New (NW_ADT_Vector_Metric_t numProperties,
                            NW_LMgr_EventHandler_t* eventHandler,
                            void* formCntrlId,
                            NW_FBox_FormLiaison_t* formLiaison,
                            NW_Text_t* value,
                            NW_Image_AbstractImage_t* image)
{
  return (NW_FBox_ImageButtonBox_t*)
    NW_Object_New (&NW_FBox_ImageButtonBox_Class, numProperties, eventHandler, 
                       formCntrlId, formLiaison,value, image);
}
