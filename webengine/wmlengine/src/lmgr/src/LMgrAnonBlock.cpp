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
* Description:  The class represents the anonymous block box
*                See CSS2 Spec from WWW.W3C.ORG (page 98)
*
*/


#include "nw_lmgr_bidiflowboxi.h"
#include "LMgrAnonBlock.h"

const
LMgrAnonBlock_Class_t  LMgrAnonBlock_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_BidiFlowBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (LMgrAnonBlock_t),
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
  }
};

TBrowserStatusCode
LMgrAnonBlock_Collapse (LMgrAnonBlock_t* aAnonBlock)
{
    TBrowserStatusCode status = KBrsrSuccess;
    // we need to transfer the children of the anonymous block 
    // to the parent
    
    // get the parent box
    NW_LMgr_ContainerBox_t* parent = (NW_LMgr_BoxOf(aAnonBlock))->parent;
    NW_ASSERT(parent);
    NW_ADT_Vector_t* children = (NW_ADT_Vector_t*)NW_LMgr_ContainerBox_GetChildren(NW_LMgr_ContainerBoxOf(aAnonBlock));

    if (parent && children)
        {
        // find position of anonymous block
        NW_ADT_Vector_Metric_t index = NW_LMgr_ContainerBox_GetChildIndex(parent, NW_LMgr_BoxOf(aAnonBlock));
        status = NW_LMgr_ContainerBox_RemoveChild(parent, NW_LMgr_BoxOf(aAnonBlock));

        // transfer children at the position from where we removed the anonymous
        // box
        NW_ADT_Vector_Metric_t numChildren = NW_ADT_Vector_GetSize(children);
        NW_ADT_Vector_Metric_t childCount=numChildren;
        // start from the end of the children list and insert in the same position
        while (childCount>0)
            {
            childCount--;
            NW_LMgr_Box_t* child = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt (children, childCount);
            NW_ASSERT(child);
            if (child)
                {
                // insert child will remove the box from previous parent and 
                // attach it to new parent and also sets the parent on the box
                status = NW_LMgr_ContainerBox_InsertChildAt(parent, child, index);
                if (status == KBrsrOutOfMemory)
                    {
                    break;
                    }
                }
            }
        } 
        NW_Object_Delete(aAnonBlock);       
    return status;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
LMgrAnonBlock_t*
LMgrAnonBlock_New (NW_ADT_Vector_Metric_t aNumProperties)
{
  return (LMgrAnonBlock_t*)
    NW_Object_New (&LMgrAnonBlock_Class, aNumProperties);
}
