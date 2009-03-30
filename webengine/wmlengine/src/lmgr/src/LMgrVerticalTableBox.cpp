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


#include "nw_lmgr_verticaltableboxi.h"

#include "nw_lmgr_verticaltablerowbox.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nwx_settings.h"
#include "BrsrStatusCodes.h"

#define NW_Default_VerticalLayoutPadding 2
/* ----------------------------------------------------------------------------*
 * static data
 * ----------------------------------------------------------------------------*/

const
NW_LMgr_VerticalTableBox_Class_t  NW_LMgr_VerticalTableBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_BidiFlowBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_VerticalTableBox_t),
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
  { /* NW_LMgr_LRFlowBox         */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_VerticalTableBox         */
    /* unused                    */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_VerticalTableBox_t*
NW_LMgr_VerticalTableBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_VerticalTableBox_t*)
    NW_Object_New (&NW_LMgr_VerticalTableBox_Class, numProperties);
}

/* This method substitutes the boxes for <table>, <tr>, <th>, and <td> when 
   switching between normal and vertical mode */
/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t* 
NW_LMgr_VerticalTableBox_SwitchBox (NW_LMgr_Box_t* box)
{
  const NW_Object_Class_t*  objClass = NULL;
  NW_LMgr_Box_t*            newBox = NULL;
  NW_LMgr_ContainerBox_t*   parent = NULL;
  TBrowserStatusCode        status = KBrsrSuccess;
  NW_ADT_Vector_Metric_t    childCount = 0;
  NW_ADT_Vector_Metric_t    index = 0;
  NW_LMgr_Property_t        prop;

  // Don't replace boxes in a grid mode table.
  if (NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_gridModeApplied, &prop) == KBrsrSuccess)
    {
    return NULL;
    }

  objClass = NW_Object_GetClass(box);
  if (objClass == &NW_LMgr_VerticalTableBox_Class)
  {
    newBox = (NW_LMgr_Box_t*)NW_LMgr_StaticTableBox_New(0);
  }
  else if (objClass == &NW_LMgr_VerticalTableRowBox_Class)
  {
    newBox = (NW_LMgr_Box_t*)NW_LMgr_StaticTableRowBox_New(0);
    //NW_LMgr_Box_RemoveProperty(box, NW_CSS_Prop_display);
  }
  else if (objClass == &NW_LMgr_VerticalTableCellBox_Class)
  {
    newBox = (NW_LMgr_Box_t*)NW_LMgr_StaticTableCellBox_New(0);
  }
  else if (objClass == &NW_LMgr_StaticTableBox_Class)
  {
    newBox = (NW_LMgr_Box_t*)NW_LMgr_VerticalTableBox_New(0);
  }
  else if (objClass == &NW_LMgr_StaticTableRowBox_Class)
  {
    newBox = (NW_LMgr_Box_t*)NW_LMgr_VerticalTableRowBox_New(0);
  }
  else if (objClass == &NW_LMgr_StaticTableCellBox_Class)
  {
    newBox = (NW_LMgr_Box_t*)NW_LMgr_VerticalTableCellBox_New(0);
  }

  if (newBox)
  {
    /* transfer the property list */
    (void) NW_LMgr_Box_SetPropList(newBox, box->propList);
    box->propList = NULL;
    parent = NW_LMgr_Box_GetParent(box);

    // Make sure we don't leak the children initialized in constructor.
    if (NW_LMgr_ContainerBoxOf(newBox)->children != NULL)
    {
        NW_Object_Delete (NW_LMgr_ContainerBoxOf(newBox)->children);
    }
    /* transfer the children */
    NW_LMgr_ContainerBoxOf(newBox)->children = NW_LMgr_ContainerBoxOf(box)->children;
    NW_LMgr_ContainerBoxOf(box)->children = NULL;
    childCount = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(newBox));

    /* update the parent pointer for all the children */
    for (index = 0; index < childCount; index++)
    {
      NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(newBox), index);
      if (child)
        child->parent = NW_LMgr_ContainerBoxOf(newBox);
    }
    /* replace old box with new box and delete the old box */
    status = NW_LMgr_ContainerBox_ReplaceChild(parent, box, newBox);
    if (status == KBrsrSuccess)
    {
      NW_Object_Delete(box);      
    }
    else
    {
      NW_Object_Delete(newBox);
      newBox = NULL;
    }
  }

  return newBox;
}

/* According to the Vertical Layout algorithm we need to ignore, width, height,
   border, margin, padding, and white-space properties for <table>, <tr>, <th>, 
   and <td> elements
   */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_VerticalTableBox_GetPropertyFromList(NW_LMgr_Box_t* box,
                                           NW_LMgr_PropertyName_t name,
                                           NW_LMgr_Property_t* property,
                                           NW_Bool* specialProperty)
{
  TBrowserStatusCode status = KBrsrNotFound;

  /* denotes that we do not need to look into the property List */
  *specialProperty = NW_TRUE;
  switch(name)
  {
  case NW_CSS_Prop_width:
  case NW_CSS_Prop_height:
    property->type = NW_CSS_ValueType_Token;
    property->value.integer = NW_CSS_PropValue_auto;
    break;

  case NW_CSS_Prop_leftBorderStyle:
  case NW_CSS_Prop_rightBorderStyle:
  case NW_CSS_Prop_topBorderStyle:
  case NW_CSS_Prop_bottomBorderStyle:
  case NW_CSS_Prop_borderStyle:
    property->type = NW_CSS_ValueType_Token;
    property->value.token = NW_CSS_PropValue_hidden;
    break;

  case NW_CSS_Prop_rightPadding:
  case NW_CSS_Prop_leftPadding:
  case NW_CSS_Prop_padding:
    status = KBrsrSuccess;
    break;

  case NW_CSS_Prop_topPadding:
  case NW_CSS_Prop_bottomPadding:
      if (NW_Object_IsClass(box, &NW_LMgr_VerticalTableRowBox_Class))
          {
          property->type = NW_CSS_ValueType_Px;
          property->value.integer = NW_Default_VerticalLayoutPadding;
          status = KBrsrSuccess;
          }
      break;

  case NW_CSS_Prop_whitespace:
    property->type = NW_CSS_ValueType_Token;
    property->value.token = NW_CSS_PropValue_normal;
    break;

  case NW_CSS_Prop_display:
    if (NW_Object_IsClass(box, &NW_LMgr_VerticalTableRowBox_Class))
    {
      property->type = NW_CSS_ValueType_Token;
      property->value.token = NW_CSS_PropValue_display_block;
      status = KBrsrSuccess; /* we want to return that we found the property */
    }
    else
      /* we need to look into the property List */
      *specialProperty = NW_FALSE;
    break;

  default:
    /* we need to look into the property List */
    *specialProperty = NW_FALSE;
  }
    
  return status;
}

