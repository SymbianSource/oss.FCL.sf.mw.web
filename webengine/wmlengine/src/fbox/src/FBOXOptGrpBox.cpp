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


#include "nw_fbox_optgrpboxi.h"

#include "nw_evt_activateevent.h"
#include "nw_fbox_optgrpskin.h"
#include "nw_adt_resizablevector.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_OptGrpBox_Class_t  NW_FBox_OptGrpBox_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_FBox_FormBox_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_FBox_OptGrpBox_t),
    /* construct             */ _NW_FBox_OptGrpBox_Construct,
    /* destruct              */ _NW_FBox_OptGrpBox_Destruct
  },
  { /* NW_LMgr_Box           */
    /* split                 */ _NW_LMgr_Box_Split,
    /* resize                */ _NW_FBox_FormBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize */ _NW_FBox_FormBox_GetMinimumContentSize,
    /* hasFixedContentSize   */ _NW_FBox_FormBox_HasFixedContentSize,
    /* constrain             */ _NW_LMgr_Box_Constrain,
    /* draw                  */ _NW_FBox_FormBox_Draw,
    /* render                */ _NW_LMgr_Box_Render,
    /* getBaseline           */ _NW_LMgr_Box_GetBaseline,
    /* shift                 */ _NW_LMgr_Box_Shift,
    /* clone                 */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox     */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox       */
    /* initSkin              */ _NW_FBox_OptGrpBox_InitSkin,
    /* initInteractor        */ _NW_FBox_OptGrpBox_InitInteractor,
    /* reset                 */ _NW_FBox_FormBox_Reset
  },
  { /* NW_FBox_OptGrpBox     */
    /* unused                */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_OptGrpBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_OptGrpBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_OptGrpBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_FormBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  thisObj->text = va_arg (*argp, NW_Text_t*);
  thisObj->children = NULL;
  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_OptGrpBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_OptGrpBox_t* thisObj;
  NW_ADT_Vector_Metric_t size;
  NW_ADT_Vector_Metric_t index;

  /* for convenience */
  thisObj = NW_FBox_OptGrpBoxOf (dynamicObject);

  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_OptGrpBox_Class));

  if (thisObj->children)
  {
    size =  NW_ADT_Vector_GetSize(thisObj->children);
    for (index =0; index <size; index++)
    {
      NW_FBox_FormBox_t* box;

      box = *(NW_FBox_FormBox_t**)
        NW_ADT_Vector_ElementAt (thisObj->children, index);
      NW_Object_Delete(box);
    }

    NW_Object_Delete(thisObj->children);
  }
  NW_Object_Delete(thisObj->text);
}

/* --------------------------------------------------------------------------*/
NW_FBox_Skin_t*
_NW_FBox_OptGrpBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_Skin_t *)NW_FBox_OptGrpSkin_New(formBox);
}

/* --------------------------------------------------------------------------*/
NW_FBox_Interactor_t*
_NW_FBox_OptGrpBox_InitInteractor(NW_FBox_FormBox_t* formBox)
{
  (void)formBox;
  return NULL;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
NW_FBox_OptGrpBox_AddOption(NW_FBox_OptGrpBox_t* optGrpBox, 
                            NW_FBox_OptionBox_t* optionBox)
{
  NW_FBox_OptGrpSkin_t* skin;

  if (optGrpBox->children == NULL){
    optGrpBox->children = (NW_ADT_DynamicVector_t*)
               NW_ADT_ResizableVector_New(sizeof(NW_FBox_FormBox_t*), 1, 2);
  }
  NW_ASSERT(optGrpBox->children);
  NW_FBox_OptionBox_t* optionBox2 = *(NW_FBox_OptionBox_t**)
      NW_ADT_DynamicVector_InsertAt(optGrpBox->children, 
                                   &optionBox, NW_ADT_Vector_AtEnd);
  NW_ASSERT(optionBox2 == optionBox);
  skin = (NW_FBox_OptGrpSkin_t*)
         NW_FBox_FormBox_GetSkin(optGrpBox);
  (void) NW_FBox_OptGrpSkin_AddOption(skin, optionBox);
}


/* ------------------------------------------------------------------------- */
void
NW_FBox_OptGrpBox_AddOptGrp(NW_FBox_OptGrpBox_t* optGrpBox, 
                            NW_FBox_OptGrpBox_t* childOptGrpBox)
{
  NW_FBox_OptGrpSkin_t* skin;

  if (optGrpBox->children == NULL){
    optGrpBox->children = (NW_ADT_DynamicVector_t*)
               NW_ADT_ResizableVector_New(sizeof(NW_FBox_FormBox_t*), 10, 5);
  }
  NW_ASSERT(optGrpBox->children);
  (void) NW_ADT_DynamicVector_InsertAt(optGrpBox->children,
		                                   &childOptGrpBox, 
																			 NW_ADT_Vector_AtEnd);
  skin = (NW_FBox_OptGrpSkin_t*)
               NW_FBox_FormBox_GetSkin(optGrpBox);
  (void) NW_FBox_OptGrpSkin_AddOptGrp(skin, childOptGrpBox);
}

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
NW_FBox_OptGrpBox_GetNumOptions(NW_FBox_OptGrpBox_t* optGrpBox)
{
  if (optGrpBox->children == NULL)
    return 0;
  return NW_ADT_Vector_GetSize(optGrpBox->children);
}

/* ------------------------------------------------------------------------- */
NW_FBox_OptionBox_t*
NW_FBox_OptGrpBox_GetOption(NW_FBox_OptGrpBox_t* optGrpBox,
                            NW_ADT_Vector_Metric_t index)
{
  return *(NW_FBox_OptionBox_t**)
    NW_ADT_Vector_ElementAt (optGrpBox->children, index);
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_FBox_OptGrpBox_t*
NW_FBox_OptGrpBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison,
                      NW_Text_t* text)
{
  return (NW_FBox_OptGrpBox_t*)
    NW_Object_New (&NW_FBox_OptGrpBox_Class, numProperties, eventHandler,
                           formCntrlId, formLiaison, text);
}
