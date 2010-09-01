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
* Description:  The class represents the anonymous table box 
*                that wraps the caption and the table
                 See CSS2 Spec from WWW.W3C.ORG (page 251)
*
*/


#include "nw_lmgr_bidiflowboxi.h"
#include "LMgrAnonTableBox.h"

const
LMgrAnonTableBox_Class_t  LMgrAnonTableBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &LMgrAnonBlock_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (LMgrAnonTableBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_FlowBox_Resize,
    /* postResize                */ _NW_LMgr_FlowBox_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_FormatBox_Constrain,
    /* draw                      */ _NW_LMgr_Box_Draw,
    /* render                    */ _NW_LMgr_ContainerBox_Render,
    /* getBaseline               */ _NW_LMgr_FlowBox_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox         */
    /* applyFormatProps          */ _NW_LMgr_FlowBox_ApplyFormatProps
  },
  { /* NW_LMgr_FlowBox           */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_BidiFlowBox       */
    /* unused                    */ NW_Object_Unused
  },
  { /* LMgrAnonBlock             */
    /* unused                    */ NW_Object_Unused
  },
  { /* LMgrAnonTableBox          */
    /* unused                    */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
LMgrAnonTableBox_t*
LMgrAnonTableBox_New (NW_ADT_Vector_Metric_t aNumProperties)
{
  return (LMgrAnonTableBox_t*)
    NW_Object_New (&LMgrAnonTableBox_Class, aNumProperties);
}
