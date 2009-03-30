/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_lmgr_formatcontext.h"
#include "nw_lmgr_flowboxi.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_cssproperties.h"
#include "GDIDeviceContext.h"
#include "nw_fbox_inputbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "BrsrStatusCodes.h"
#include "MemoryManager.h"

/* The static instance of the Flow_Box class object 
 * Flow box is a pure virtual class with several methods
 * for which there are no default implementations.
 */

/* ------------------------------------------------------------------------- */

const
NW_LMgr_FlowBox_Class_t  NW_LMgr_FlowBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_FormatBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_FlowBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_FlowBox_Resize,
                                    _NW_LMgr_FlowBox_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
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
  }
};

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_FlowBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context)
{
  NW_TRY(status) {
    /* invoke superclass resize */
    status = NW_LMgr_FormatBox_Class.NW_LMgr_Box.resize(box, context);
    _NW_THROW_ON_ERROR(status);

  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

TBrowserStatusCode
_NW_LMgr_FlowBox_PostResize(NW_LMgr_Box_t* box)
  {
    NW_GDI_Dimension2D_t size;
    NW_LMgr_FrameInfo_t padding, borderWidth;
    NW_LMgr_PropertyValue_t bodyFlag;

    NW_ASSERT(box);

    NW_TRY(status) 
    {
      /* Check for the height property */
      NW_LMgr_Box_GetSizeProperties(box, &size);

      status = NW_LMgr_FormatBox_Class.NW_LMgr_Box.postResize(box);
      _NW_THROW_ON_ERROR(status);

      /* we should always respect size.height if it's >= 0 */
      if (size.height >= 0) 
      {
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
        if ( size.height >= boxBounds.dimension.height ||
             NW_Object_IsInstanceOf(box, &NW_LMgr_StaticTableBox_Class) ||
             NW_Object_IsInstanceOf(box, &NW_LMgr_StaticTableCellBox_Class) ||
             NW_Object_IsInstanceOf(box, &NW_LMgr_StaticTableRowBox_Class) )
        {
          NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameTop|ELMgrFrameBottom );
          NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameTop|ELMgrFrameBottom );
          boxBounds.dimension.height = (NW_GDI_Metric_t)
            (size.height + borderWidth.top + borderWidth.bottom
             + padding.top + padding.bottom);
          NW_LMgr_Box_SetFormatBounds( box, boxBounds );
        }
      }
      bodyFlag.token = NW_CSS_PropValue_none;
      (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_flags,
                                     NW_CSS_ValueType_Token, &bodyFlag);
      if (bodyFlag.token == NW_CSS_PropValue_flags_bodyElement) {

        const NW_GDI_Rectangle_t* rectangle;
        CGDIDeviceContext* deviceContext;
        NW_LMgr_RootBox_t* rootBox;

        rootBox = NW_LMgr_Box_GetRootBox(box);
        deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
        rectangle = deviceContext->DisplayBounds();
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
        if( boxBounds.dimension.height < rectangle->dimension.height)
        {
          boxBounds.dimension.height = (NW_GDI_Metric_t)
            (rectangle->dimension.height - boxBounds.point.y);
          NW_LMgr_Box_SetFormatBounds( box, boxBounds );
        }
      }
    }
    NW_CATCH(status) {
    }
    NW_FINALLY {
      return status;
    }
    NW_END_TRY
  }

/* ----------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_LMgr_FlowBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                  NW_LMgr_FormatContext_t* context)
{
  NW_LMgr_RootBox_t *rootBox;
  NW_LMgr_Box_t *tableBox;
  NW_GDI_Metric_t minWidth;
  NW_LMgr_FrameInfo_t paddingInfo;
  NW_LMgr_FrameInfo_t borderWidthInfo;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( format ) );
  NW_LMgr_Property_t dirProp;

  NW_LMgr_Box_GetPadding(NW_LMgr_BoxOf(format), &paddingInfo, ELMgrFrameAll );
  NW_LMgr_Box_GetBorderWidth(NW_LMgr_BoxOf(format), &borderWidthInfo, ELMgrFrameAll );

  context->startMargin = 
    (NW_GDI_Metric_t)(boxBounds.point.x + paddingInfo.left
                      + borderWidthInfo.left);
  context->endMargin = (NW_GDI_Metric_t)(boxBounds.point.x
                                         + boxBounds.dimension.width
                                         - paddingInfo.right
                                         - borderWidthInfo.right);

  /* Set initial vertical position */
  context->y = (NW_GDI_Metric_t) (boxBounds.point.y + paddingInfo.top
                                  + borderWidthInfo.top);

  minWidth = NW_LMGR_BOX_MIN_WIDTH;

  if (context->endMargin < context->startMargin + minWidth) 
    {
    context->endMargin = (NW_GDI_Metric_t)(context->startMargin + minWidth);
    boxBounds.dimension.width =  (NW_GDI_Metric_t)(minWidth + paddingInfo.left + 
      borderWidthInfo.left + paddingInfo.right + borderWidthInfo.right);
    
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( format ), boxBounds );
    // 
    status = KBrsrLmgrFormatOverflow;   
  }
  boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( format ) );
  /* Now reset initial height. This will change as children are laid out */  
  boxBounds.dimension.height =  (NW_GDI_Metric_t)( paddingInfo.top + paddingInfo.bottom +
    borderWidthInfo.top + borderWidthInfo.bottom);
  NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( format ), boxBounds );
    
  if (!NW_Object_IsClass(format, &NW_LMgr_RootBox_Class)) {
    rootBox = NW_LMgr_Box_GetRootBox (format);
    NW_ASSERT (rootBox != NULL);
  }
  else {
    rootBox = NW_LMgr_RootBoxOf(format);
  }

  tableBox = NW_LMgr_BoxOf(format);
  while (tableBox != NULL) {
    if (NW_Object_IsClass(tableBox, &NW_LMgr_StaticTableBox_Class)) {
      break;
    }
    tableBox = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent (tableBox);
  }
  if (tableBox == NULL) {
    context->placedFloats = rootBox->placedFloats;
  }
  else {
    context->placedFloats = NULL;
  }
  context->pendingFloats = rootBox->pendingFloats;

  /* Now set the flow-specific context variables */
  context->lineStart = NW_LMgr_FlowBox_GetLineStart(NW_LMgr_FlowBoxOf(format),
                                                    context, context->y);
  context->lineEnd = NW_LMgr_FlowBox_GetLineEnd(NW_LMgr_FlowBoxOf(format),
                                                context, context->y);

  /* Get the flow direction */
  dirProp.type = NW_CSS_ValueType_Token;
  dirProp.value.token = NW_CSS_PropValue_ltr;
  NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(format), NW_CSS_Prop_textDirection, &dirProp);
  if (dirProp.value.token == NW_CSS_PropValue_ltr) {
    context->direction = NW_GDI_FlowDirection_LeftRight;
  }
  else {
    context->direction = NW_GDI_FlowDirection_RightLeft;
  }

  /* Set the initial x position */
  if (dirProp.value.token == NW_CSS_PropValue_ltr) {
    context->x = (NW_GDI_Metric_t)(context->lineStart + 
                      NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBoxOf(format),
                                                     context));
  }
  else {
    context->x = (NW_GDI_Metric_t)(context->lineEnd - 
                      NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBoxOf(format),
                                                     context));
  }
  return status;
}

/* --------------------------------------------------------------------------*/
NW_GDI_Metric_t
_NW_LMgr_FlowBox_GetBaseline(NW_LMgr_Box_t* box){

  NW_LMgr_PropertyValue_t baselineVal;
  NW_GDI_Metric_t baseline = 0;
  NW_LMgr_Box_t *childFlow;
  TBrowserStatusCode status;

  childFlow = box;
  baselineVal.integer = 0;
  while (NW_Object_IsInstanceOf(childFlow, &NW_LMgr_FlowBox_Class)) {
    status = NW_LMgr_Box_GetPropertyValue(childFlow, NW_CSS_Prop_flowBaseline,
                                          NW_CSS_ValueType_Px, &baselineVal);
    if (status != KBrsrNotFound) 
      {
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( childFlow );
      baseline = (NW_GDI_Metric_t)(boxBounds.point.y
                                   - boxBounds.point.y
                                   + baselineVal.integer);
      break;
    }
    childFlow =
      NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(childFlow), 0);
    if (childFlow == NULL) {
      break;
    }
  }
  return baseline;
}

/* --------------------------------------------------------------------------*/
/* Private method to calculate the left margin of the flow at position y. 
 */

NW_GDI_Metric_t
NW_LMgr_FlowBox_GetLineStart(NW_LMgr_FlowBox_t* flow, 
                             NW_LMgr_FormatContext_t* context,
                             NW_GDI_Metric_t where) {

  NW_ADT_Vector_Metric_t floatCount, index;
  NW_GDI_Metric_t leftMargin;
  NW_GDI_Metric_t floatRight, floatTop, floatBottom;
  NW_LMgr_Box_t *floatBox;
  NW_LMgr_PropertyValue_t floatValue;
  NW_ADT_DynamicVector_t *placedFloats = context->placedFloats;
  NW_LMgr_FrameInfo_t margin;

  NW_REQUIRED_PARAM(flow);

  leftMargin = context->startMargin;

  if (placedFloats == NULL) {
    return leftMargin;
  }

  floatCount = NW_ADT_Vector_GetSize(placedFloats);

  for (index = 0; index < floatCount; index++) {
    
    floatBox =
      *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
    NW_ASSERT(floatBox != NULL);

    floatValue.integer = NW_CSS_PropValue_none;
    NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                                 NW_CSS_ValueType_Token, &floatValue);

    if (floatValue.token == NW_CSS_PropValue_left) {
  
      NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameRight|ELMgrFrameTop|ELMgrFrameBottom );
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );

      floatTop = (NW_GDI_Metric_t)(boxBounds.point.y - margin.top);
      floatBottom = (NW_GDI_Metric_t)(boxBounds.point.y
                                      + boxBounds.dimension.height
                                      + margin.bottom);
      floatRight = (NW_GDI_Metric_t)(boxBounds.point.x
                                     + boxBounds.dimension.width
                                     + margin.right);

      if ((where >= floatTop)
          && (where < floatBottom)
          && (leftMargin < floatRight)) {
        leftMargin = floatRight;
      }
    }
  }
  return leftMargin;
}


/* --------------------------------------------------------------------------*/
/* Private method to calculate the left margin of the flow at position y. 
 */

NW_GDI_Metric_t
NW_LMgr_FlowBox_GetLineEnd(NW_LMgr_FlowBox_t* flow, 
                           NW_LMgr_FormatContext_t* context,
                           NW_GDI_Metric_t where) {

  NW_ADT_Vector_Metric_t floatCount, index;
  NW_GDI_Metric_t rightMargin;
  NW_GDI_Metric_t floatLeft, floatTop, floatBottom;
  NW_LMgr_Box_t *floatBox;
  NW_LMgr_PropertyValue_t floatValue;
  NW_ADT_DynamicVector_t *placedFloats = context->placedFloats;
  NW_LMgr_FrameInfo_t margin;

  NW_REQUIRED_PARAM(flow);

  rightMargin = context->endMargin;

  if (placedFloats == NULL) {
    return rightMargin;
  }

  floatCount = NW_ADT_Vector_GetSize(placedFloats);

  for (index = 0; index < floatCount; index++) {
    
    floatBox =
      *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
    NW_ASSERT(floatBox != NULL);

    floatValue.integer = NW_CSS_PropValue_none;
    NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                                 NW_CSS_ValueType_Token, &floatValue);

    if (floatValue.token == NW_CSS_PropValue_right) {

      NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameLeft|ELMgrFrameTop|ELMgrFrameBottom );
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );
  
      floatTop = (NW_GDI_Metric_t)(boxBounds.point.y - margin.top);
      floatBottom = (NW_GDI_Metric_t)(boxBounds.point.y
                                      + boxBounds.dimension.height
                                      + margin.bottom);
      floatLeft = (NW_GDI_Metric_t)(boxBounds.point.x
                                    - margin.left - 1);

      if ((where >= floatTop)
          && (where < floatBottom)
          && (rightMargin > floatLeft)) {
        rightMargin = floatLeft;
      }
    }
  }
  return rightMargin;
}

/* Private method to calculate any first-line indentation */
NW_GDI_Metric_t
NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBox_t* flow, 
                               NW_LMgr_FormatContext_t* context) 
{
  NW_GDI_Metric_t indent = 0;
  NW_LMgr_PropertyValue_t tIndent;
  NW_LMgr_PropertyValue_t tAlign;

  if (context->lineNumber == 0) {
    /* Get the text alignment */
    tAlign.token = NW_CSS_PropValue_left;
    NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow), NW_CSS_Prop_textAlign,
                                 NW_CSS_ValueType_Token, &tAlign);

    if (tAlign.token == NW_CSS_PropValue_left) {
      /* Get the text indent */
      tIndent.integer = 0;
      NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow), NW_CSS_Prop_textIndent,
                                   NW_CSS_ValueType_Px, &tIndent);

      indent = (NW_GDI_Metric_t)tIndent.integer;
    }
  }
  return indent;
}
