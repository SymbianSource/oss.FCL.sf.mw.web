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


#include "nw_fbox_radioboxi.h"

#include "nw_lmgr_rootbox.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_RadioBox_Class_t  NW_FBox_RadioBox_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_FBox_CheckBox_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_FBox_RadioBox_t),
    /* construct             */ _NW_FBox_CheckBox_Construct,
    /* destruct              */ NULL
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
    /* getBaseline           */ _NW_FBox_FormBox_GetBaseline,
    /* shift                 */ _NW_LMgr_Box_Shift,
    /* clone                 */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox     */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox       */
    /* initSkin              */ _NW_FBox_CheckBox_InitSkin,
    /* initInteractor        */ _NW_FBox_CheckBox_InitInteractor,
    /* reset                 */ _NW_FBox_FormBox_Reset
  },
  { /* NW_FBox_CheckBox      */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_RadioBox      */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_FBox_RadioBox_t*
NW_FBox_RadioBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_LMgr_EventHandler_t* eventHandler,
                      void* formCntrlId,
                      NW_FBox_FormLiaison_t* formLiaison)
{
  return (NW_FBox_RadioBox_t*)
    NW_Object_New (&NW_FBox_RadioBox_Class, numProperties, eventHandler, 
                           formCntrlId, formLiaison);
}
