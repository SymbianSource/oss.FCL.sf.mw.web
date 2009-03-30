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


#include "nw_fbox_hiddenboxi.h"
#include "nw_fbox_inputskin.h"
#include "nw_fbox_inputinteractor.h"
#include "nw_fbox_inlineinputskin.h"
#include "nw_lmgr_rootbox.h"


/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_HiddenBox_Class_t  NW_FBox_HiddenBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_InputBox_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_HiddenBox_t),
    /* construct                  */ _NW_FBox_InputBox_Construct,
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
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox     */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_FormBox            */
    /* initSkin                   */ _NW_FBox_InputBox_InitSkin,
    /* initInteractor             */ _NW_FBox_InputBox_InitInteractor,
    /* reset                      */ _NW_FBox_InputBox_Reset
  },
   {/* NW_FBox_InputBox           */
    /* getHeight                  */ _NW_FBox_InputBox_GetHeight,
    /* setHeight                  */ _NW_FBox_InputBox_SetHeight,
    /* setDisplayText             */ _NW_FBox_InputBox_SetDisplayText,
    /* modifyBuffer               */ _NW_FBox_InputBox_ModifyBuffer
  },
  { /* NW_FBox_HiddenBox      */
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
NW_FBox_HiddenBox_t*
NW_FBox_HiddenBox_New (NW_ADT_Vector_Metric_t numProperties,
                         NW_LMgr_EventHandler_t* eventHandler,
                         void* formCntrlId,
                         NW_FBox_FormLiaison_t* formLiaison,
                         NW_HED_AppServices_t* appServices)
{
  return (NW_FBox_HiddenBox_t*)
    NW_Object_New (&NW_FBox_HiddenBox_Class, numProperties, eventHandler, 
                  formCntrlId, formLiaison, appServices);
}
