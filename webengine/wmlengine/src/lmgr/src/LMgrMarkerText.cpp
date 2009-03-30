/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class represents the marker text box
*
*/

#include "LMgrMarkerText.h"
#include "nw_lmgr_textboxi.h"
/* ------------------------------------------------------------------------- */
const
LMgrMarkerText_Class_t  LMgrMarkerText_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_TextBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (LMgrMarkerText_t),
    /* construct                 */ _NW_LMgr_TextBox_Construct,
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
    /* getText                   */ _NW_LMgr_TextBox_GetText,
    /* getStart                  */ _NW_LMgr_TextBox_GetStart
  },
  { /* NW_LMgr_TextBox           */
    /* unused                    */ NW_Object_Unused
  },
  { /* LMgrMarkerText           */
    /* unused                    */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
LMgrMarkerText_t*
LMgrMarkerText_New (NW_ADT_Vector_Metric_t numProperties,
                     NW_Text_t* aText)
{
  return (LMgrMarkerText_t*)
    NW_Object_New (&LMgrMarkerText_Class, numProperties, aText );
}
