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


#include "nw_fbox_formboxi.h"
#include "nw_fbox_inputskin.h"
#include "nw_fbox_selectskin.h"
#include "nw_fbox_radiobox.h"
#include "nw_fbox_clearaction.h"
#include "BrsrStatusCodes.h"
/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_FormBox_Class_t  NW_FBox_FormBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_LMgr_ActiveBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_FormBox_t),
    /* construct                  */ _NW_FBox_FormBox_Construct,
    /* destruct                   */ _NW_FBox_FormBox_Destruct
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
    /* getBaseline                */ _NW_FBox_FormBox_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ NULL,
    /* initInteractor             */ NULL,
    /* reset                      */ _NW_FBox_FormBox_Reset
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_FormBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp)
{
  NW_FBox_FormBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_FormBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_LMgr_ActiveBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the member variables */
  thisObj->formCntrlId = va_arg (*argp, void*);
  thisObj->formLiaison = va_arg (*argp, NW_FBox_FormLiaison_t*);
  thisObj->skin = NW_FBox_FormBox_InitSkin(thisObj);
  if (thisObj->skin == NULL)
  {
    NW_LMgr_ActiveBoxOf(thisObj)->eventHandler = NULL;
    return KBrsrOutOfMemory;
  }
  if (NW_Object_IsInstanceOf(thisObj->skin, &NW_FBox_SelectSkin_Class))
  {
    NW_FBox_SelectSkinOf(thisObj->skin)->hasFormBox = NW_TRUE;
  }
  thisObj->interactor = NW_FBox_FormBox_InitInteractor(thisObj);
  /* successful completion */
  return KBrsrSuccess;

}

/* ------------------------------------------------------------------------- */
void
_NW_FBox_FormBox_Destruct(NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_FormBox_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_FormBoxOf (dynamicObject);

  /* Destroy the skin */
  if(thisObj->skin != NULL){
    if (NW_Object_IsInstanceOf(thisObj->skin, &NW_FBox_SelectSkin_Class))
    {
      NW_FBox_SelectSkinOf(thisObj->skin)->hasFormBox = NW_FALSE;
      NW_FBox_SelectSkin_Release(NW_FBox_SelectSkinOf(thisObj->skin));
    }
    else
    {
      NW_Object_Delete(thisObj->skin);
    }
  }

  /* Destroy the skin */
  if(thisObj->interactor != NULL){
    NW_Object_Delete(thisObj->interactor);
  }
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_FormBox_Draw (NW_LMgr_Box_t* box,
                       CGDIDeviceContext* deviceContext,
                       NW_Uint8 hasFocus)
{
  NW_FBox_FormBox_t* formBox;
  
  formBox = NW_FBox_FormBoxOf(box);
  
  return NW_FBox_Skin_Draw(formBox->skin, box, deviceContext, hasFocus);
}

/* --------------------------------------------------------------------------*/
NW_GDI_Metric_t
_NW_FBox_FormBox_GetBaseline (NW_LMgr_Box_t* box)
{
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );

  return NW_LMgr_Box_GetFormatBounds(box).dimension.height - padding.top - padding.bottom;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_FormBox_Resize (NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context)
{
  NW_LMgr_FrameInfo_t borderWidth;
  NW_LMgr_FrameInfo_t padding;
  NW_GDI_Dimension2D_t size;
  NW_REQUIRED_PARAM(context);
  NW_TRY(status) {
    /* Get the box info */
    NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );
    NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );

    status = NW_LMgr_Box_GetMinimumContentSize(box, &size);
    NW_THROW_ON_ERROR(status);
    //
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

    boxBounds.dimension.height = (NW_GDI_Metric_t)(size.height + 
                                                borderWidth.bottom + 
                                                borderWidth.top + 
                                                padding.bottom + 
                                                padding.top);
    boxBounds.dimension.width = (NW_GDI_Metric_t)(size.width +
                                               borderWidth.left + 
                                               borderWidth.right +
                                               padding.left + 
                                               padding.right);
    NW_LMgr_Box_SetFormatBounds( box, boxBounds );
    if (NW_Object_IsClass(box, &NW_FBox_InputBox_Class))
    {
      NW_FBox_Skin_t* skin;
      NW_FBox_InputSkin_t* inputSkin;
      skin = NW_FBox_FormBox_GetSkin(box);
      inputSkin= NW_FBox_InputSkinOf(skin);
      (void) NW_FBox_InputSkin_SetDisplayText(inputSkin);
    }

  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_FormBox_GetMinimumContentSize (NW_LMgr_Box_t* box, 
                                        NW_GDI_Dimension2D_t* size)
{
  NW_FBox_Skin_t* skin;
  NW_GDI_Dimension3D_t formSize;

  NW_TRY(status) {
    /* parameter assertion block */
    NW_ASSERT (box != NULL);
    NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class));
    NW_ASSERT (size != NULL);

    /* Get text entent of masterString to set minimum height */
    skin = NW_FBox_FormBox_GetSkin(box);
    status = NW_FBox_Skin_GetSize (skin, box, &formSize);
    NW_THROW_ON_ERROR(status);

    size->height = formSize.height;
    size->width = formSize.width;
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY;
}

/* --------------------------------------------------------------------------*/
NW_Bool
_NW_FBox_FormBox_HasFixedContentSize(NW_LMgr_Box_t *box)
{
  NW_REQUIRED_PARAM(box);
  return NW_TRUE;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_FBox_FormBox_Reset (NW_FBox_FormBox_t* thisObj)
{
  TBrowserStatusCode status;
  NW_FBox_FormLiaison_t* formLiaison;
  void* formControlId;

  formControlId = NW_FBox_FormBox_GetFormCntrlID(thisObj);
  formLiaison = NW_FBox_FormBox_GetFormLiaison(NW_FBox_FormBoxOf(thisObj));

  /* set the value to NULL */ 
  status = NW_FBox_FormLiaison_SetStringValue(formLiaison, formControlId, NULL);
  if (status != KBrsrSuccess) {
		NW_Bool value = NW_FALSE;
    // radio box check has to be before checkbox;
		if ( NW_Object_IsInstanceOf(thisObj, &NW_FBox_RadioBox_Class) )
      {
      // because original reset doesn't work for the case where by default no radio checked; 
      // ResetRadioValue method is dedicating to reset radio value only 
			value = NW_FBox_FormLiaison_GetInitialValue(formLiaison, formControlId);
      status = NW_FBox_FormLiaison_ResetRadioValue(formLiaison, formControlId, &value);
      }
		else if ( NW_Object_IsInstanceOf(thisObj, &NW_FBox_CheckBox_Class) )
      {
			value = NW_FBox_FormLiaison_GetInitialValue(formLiaison, formControlId);
      status = NW_FBox_FormLiaison_SetBoolValue( formLiaison, formControlId, &value );
      }
  }

  return status;
}
