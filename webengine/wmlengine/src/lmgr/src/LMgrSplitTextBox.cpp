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


#include "nw_lmgr_splittextboxi.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_cssproperties.h"


#include "nwx_settings.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
/* The static instance of the Text_Box class object */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_SplitTextBox_Class_t  NW_LMgr_SplitTextBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_AbstractTextBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_SplitTextBox_t),
    /* construct                 */ _NW_LMgr_SplitTextBox_Construct,
    /* destruct                  */ NULL
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
    /* getText                   */ _NW_LMgr_SplitTextBox_GetText,
    /* getStart                  */ _NW_LMgr_SplitTextBox_GetStart
  },
  { /* NW_LMgr_SplitTextBox      */
    /* unused                    */ 0
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_SplitTextBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp){
  NW_LMgr_SplitTextBox_t* textBox;

  /* for convenience */
  textBox = NW_LMgr_SplitTextBoxOf (dynamicObject);

  /* initialize our text member */
  textBox->prevBox = va_arg (*argp, NW_LMgr_AbstractTextBox_t*);
  NW_LMgr_AbstractTextBoxOf(textBox)->length = va_arg (*argp, NW_Text_Length_t);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Text_t*
_NW_LMgr_SplitTextBox_GetText (NW_LMgr_AbstractTextBox_t *abstractText)
{
  NW_LMgr_SplitTextBox_t* thisObj = NW_LMgr_SplitTextBoxOf(abstractText);
  NW_LMgr_AbstractTextBox_t *prevBox;
  NW_LMgr_TextBox_t *principalBox;

  /* Ideally, this function should call itself recursively to find
   * the original text.  The following loop takes a shortcut to
   * reduce stack usage.
   */
  prevBox = thisObj->prevBox;
  while (NW_Object_IsInstanceOf(prevBox, &NW_LMgr_SplitTextBox_Class)) {
    prevBox = NW_LMgr_SplitTextBoxOf(prevBox)->prevBox;
  }
  
  //Make sure the previous entry is still valid
  if (!NW_Object_IsInstanceOf(prevBox, &NW_LMgr_TextBox_Class)) {
     return NULL;
  }

  principalBox = NW_LMgr_TextBoxOf(prevBox);
  return principalBox->text;
}

/* ------------------------------------------------------------------------- */
NW_Text_Length_t
_NW_LMgr_SplitTextBox_GetStart (NW_LMgr_AbstractTextBox_t *abstractText)
{
  NW_LMgr_SplitTextBox_t* thisObj = NW_LMgr_SplitTextBoxOf(abstractText);
  NW_LMgr_AbstractTextBox_t* prevBox = NULL;
  NW_Text_Length_t start;

  /* Ideally, this function should call itself recursively to determine
   * the starting position.  The following loop takes a shortcut to
   * reduce stack usage.
   */
  prevBox = thisObj->prevBox;
  start = 0;
  while (NW_Object_IsInstanceOf(prevBox, &NW_LMgr_SplitTextBox_Class)) {
    start = (NW_Text_Length_t)(start + NW_LMgr_AbstractTextBox_GetLength(prevBox));
    prevBox = NW_LMgr_SplitTextBoxOf(prevBox)->prevBox;
  }
  start = (NW_Text_Length_t)(start + NW_LMgr_AbstractTextBox_GetLength(prevBox));

  return start;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_SplitTextBox_t*
NW_LMgr_SplitTextBox_New (NW_LMgr_AbstractTextBox_t* textBox,
                          NW_Text_Length_t length)
{
  return (NW_LMgr_SplitTextBox_t*)NW_Object_New(&NW_LMgr_SplitTextBox_Class,
                                                textBox,
                                                length);
}
