/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_textboxi.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_splittextbox.h"

#include "nwx_settings.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- */
const
NW_LMgr_TextBox_Class_t  NW_LMgr_TextBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_AbstractTextBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_TextBox_t),
    /* construct                 */ _NW_LMgr_TextBox_Construct,
    /* destruct                  */ _NW_LMgr_TextBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_AbstractTextBox_Split,
    /* resize                    */ _NW_LMgr_AbstractTextBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_AbstractTextBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_AbstractTextBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_AbstractTextBox_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_AbstractTextBox   */
    /* getText                   */ _NW_LMgr_TextBox_GetText,
    /* getStart                  */ _NW_LMgr_TextBox_GetStart
  },
  { /* NW_LMgr_TextBox           */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_TextBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp){
  NW_LMgr_TextBox_t* textBox;
  TBrowserStatusCode status;

  /* for convenience */
  textBox = NW_LMgr_TextBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_LMgr_Box_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize our text member */
  textBox->text = va_arg (*argp, NW_Text_t*);
  NW_LMgr_AbstractTextBoxOf(textBox)->length = 0;  /* This will get initialized
                                                      upon the first GetDisplayText */

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
/* The text box destructor cleans up any cached GDI Font handle as well as
 * the NW_Text object supplied to the constructor
 */

void
_NW_LMgr_TextBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  /* for convenience */
  NW_LMgr_TextBox_t* textBox = NW_LMgr_TextBoxOf (dynamicObject);

  /* Delete the member text */
  if(textBox->text != NULL){
    NW_Object_Delete(textBox->text);
  }
}

/* ------------------------------------------------------------------------- */
NW_Text_Length_t
_NW_LMgr_TextBox_GetStart (NW_LMgr_AbstractTextBox_t *abstractText) 
{
  NW_REQUIRED_PARAM(abstractText);
  return 0;
}

/* ------------------------------------------------------------------------- */
NW_Text_t*
_NW_LMgr_TextBox_GetText (NW_LMgr_AbstractTextBox_t *abstractText) 
{
  NW_LMgr_TextBox_t* textBox = NW_LMgr_TextBoxOf(abstractText);

  return textBox->text;
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_TextBox_Collapse (NW_LMgr_TextBox_t* thisObj)
{
  NW_LMgr_ContainerBox_t* parent;
  NW_ADT_Vector_Metric_t index;
  NW_LMgr_Box_t* siblingBox;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_TextBox_Class));

  /* implement collapse */
  parent = NW_LMgr_Box_GetParent (thisObj);
  if (parent == NULL) {
    return KBrsrFailure;
  }
  
  /* this is the position of textBox */
  index = NW_LMgr_ContainerBox_GetChildIndex (parent, NW_LMgr_BoxOf(thisObj));

  /* Get following split text siblings */
  index++;
  siblingBox = NW_LMgr_ContainerBox_GetChild(parent, index);

  while (siblingBox != NULL &&
		 NW_Object_IsClass(siblingBox, &NW_LMgr_SplitTextBox_Class))
  {
   /* NW_LMgr_ContainerBox_RemoveChild(parent, siblingBox);*/
    NW_Object_Delete(siblingBox);
    siblingBox = NW_LMgr_ContainerBox_GetChild(parent, index);
  }
  NW_LMgr_AbstractTextBoxOf(thisObj)->length = 0;  
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMgr_TextBox_t*
NW_LMgr_TextBox_New (NW_ADT_Vector_Metric_t numProperties,
                     NW_Text_t* text)
{
  NW_LMgr_TextBox_t* textBox;

  NW_REQUIRED_PARAM(numProperties);

  textBox = (NW_LMgr_TextBox_t*)NW_Object_New(&NW_LMgr_TextBox_Class, 
                                              numProperties, 
                                              text);

  return textBox;

}
