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


#include "nw_fbox_textareaboxi.h"
#include "nw_lmgr_rootbox.h"
#include <nwx_string.h>
#include "nw_fbox_textareaskin.h"
#include "nw_fbox_inputinteractor.h"
#include "BrsrStatusCodes.h"

/*
**----------------------------------------------------------------
**  Macros
**----------------------------------------------------------------
*/
#define TEXTAREABOX_DEFAULT_HEIGHT 2
 
/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_TextAreaBox_Class_t  NW_FBox_TextAreaBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_InputBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_TextAreaBox_t),
    /* construct                  */ _NW_FBox_TextAreaBox_Construct,
    /* destruct                   */ NULL
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
    /* clone                      */ _NW_LMgr_Box_Clone,
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_TextAreaBox_InitSkin,
    /* initInteractor             */ _NW_FBox_InputBox_InitInteractor,
    /* reset                      */ _NW_FBox_InputBox_Reset
  },
  { /* NW_FBox_InputBox           */
    /* getHeight                  */ _NW_FBox_TextAreaBox_GetHeight,
    /* setHeight                  */ _NW_FBox_TextAreaBox_SetHeight,
    /* setDisplayText             */ _NW_FBox_InputBox_SetDisplayText,
    /* modifyBuffer               */ _NW_FBox_InputBox_ModifyBuffer
  },
  { /* NW_FBox_TextAreaBox        */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_TextAreaBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_TextAreaBox_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_TextAreaBoxOf (dynamicObject);
  /* invoke our superclass constructor */
  status = _NW_FBox_InputBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  thisObj->height = (NW_Uint16)va_arg (*argp, NW_Uint32);
  if(thisObj->height == 0)
  {
    /* 
     * If the rows attribute is not specified or its value is zero then
     * specification requires, set height to the default value, which is 1.
     */
    thisObj->height = TEXTAREABOX_DEFAULT_HEIGHT;
  }
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint16
_NW_FBox_TextAreaBox_GetHeight(NW_FBox_InputBox_t* box)
{
  NW_FBox_TextAreaBox_t* textAreaBox;

  textAreaBox = NW_FBox_TextAreaBoxOf(box);
  return textAreaBox->height;
}

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void
_NW_FBox_TextAreaBox_SetHeight(NW_FBox_InputBox_t* box, NW_Uint16 height)
{
  NW_FBox_TextAreaBox_t* textAreaBox;

  textAreaBox = NW_FBox_TextAreaBoxOf(box);
  textAreaBox->height = height;
}

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_FBox_Skin_t*
_NW_FBox_TextAreaBox_InitSkin(NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_Skin_t *)NW_FBox_InputSkin_New(formBox);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_TextAreaBox_t*
NW_FBox_TextAreaBox_New (NW_ADT_Vector_Metric_t numProperties,
                         NW_LMgr_EventHandler_t* eventHandler,
                         void* formCntrlId,
                         NW_FBox_FormLiaison_t* formLiaison,
                         NW_Uint16 columns,
                         NW_HED_AppServices_t* appServices,
                         NW_Uint16 rows)
{
  return (NW_FBox_TextAreaBox_t*)
    NW_Object_New (&NW_FBox_TextAreaBox_Class, (NW_Uint32)numProperties, eventHandler, 
                  formCntrlId, formLiaison, (NW_Uint32)columns, appServices, (NW_Uint32)rows);
}
