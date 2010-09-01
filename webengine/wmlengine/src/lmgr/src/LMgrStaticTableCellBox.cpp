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


#include "nw_lmgr_statictablecellboxi.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_formatcontext.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_lmgr_rootbox.h"
#include "GDIDeviceContext.h"
#include "nwx_math.h"
#include "nw_lmgr_eventhandler.h"
#include "nw_lmgr_activecontainerbox.h"
#include "BrsrStatusCodes.h"
#include "BoxRender.h"
#include "nwx_settings.h"

/* ------------------------------------------------------------------------- */

static
void
NW_LMgr_StaticTableCellBox_GetEdge(NW_LMgr_Box_t* box, 
                                   NW_LMgr_StaticTableBorderSide_t side,
                                   NW_LMgr_StaticTableBorderEdge_t* edge) 
{
  NW_LMgr_FrameInfo_t widthInfo; 
  NW_LMgr_FrameInfo_t styleInfo; 
  NW_LMgr_FrameInfo_t colorInfo; 
  NW_LMgr_Property_t  prop;
  TBrowserStatusCode  status;

  NW_LMgr_Box_GetBorderWidth(box, &widthInfo, ELMgrFrameAll );
  NW_LMgr_Box_GetBorderStyle(box, &styleInfo, ELMgrFrameAll );
  NW_LMgr_Box_GetBorderColor(box, &colorInfo);

  switch (side) {
  case left:
    edge->width = widthInfo.left;
    edge->style = styleInfo.left;
    edge->color = colorInfo.left;
    break;
  case right:
    edge->width = widthInfo.right;
    edge->style = styleInfo.right;
    edge->color = colorInfo.right;
    break;
  case top:
    edge->width = widthInfo.top;
    edge->style = styleInfo.top;
    edge->color = colorInfo.top;
    break;
  case bottom:
    edge->width = widthInfo.bottom;
    edge->style = styleInfo.bottom;
    edge->color = colorInfo.bottom;
    break;
  default:
    NW_ASSERT(0);
    break;
  }

  // While in SSM force borders on grid mode tables to 1.
  if (edge->width > 1) {
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_gridModeApplied, &prop);
    if (status == KBrsrSuccess) {
      NW_LMgr_RootBox_t*  rootBox = NW_LMgr_Box_GetRootBox(box);
      NW_ASSERT (rootBox != NULL);

      if (NW_LMgr_RootBox_GetSmallScreenOn(rootBox)) {
        edge->width = 1;
      }
    }
  }

  return;
}

static
void
NW_LMgr_StaticTableCellBox_UpdateEdge(NW_LMgr_StaticTableBorderEdge_t* edge,
                                      NW_LMgr_StaticTableBorderEdge_t* newEdge) 
    {
    /* If the edge is hidden, no need to update it; simply return */
    if (edge->style == NW_CSS_PropValue_hidden) 
        {
        return;
        }

    /* If the new edge is hidden, store that and return */
    if (newEdge->style == NW_CSS_PropValue_hidden) 
        {
        edge->style = NW_CSS_PropValue_hidden;
        edge->width = 0;
        return;
        }

    /* Everyone can overwrite "style==none" */
    if (newEdge->style == NW_CSS_PropValue_none) 
        {
        return;
        }

    /* According to CSS, the border with the largest width wins.
     If all borders have the same width, then the border with
     the strongest style wins. Here we assume that
     NW_CSS_PropValue_double > ...solid > ...dashed > ...dotted > ...none > */
    if (newEdge->width > edge->width) 
        {
        *edge = *newEdge;
        }
    // comparison of width supercedes comparison of styel
    else if ( newEdge->width == edge->width &&
              newEdge->style > edge->style ) 
        {
        *edge = *newEdge;
        }

    return;
    }


void
NW_LMgr_StaticTableCellBox_GetPadding(NW_LMgr_StaticTableCellBox_t *thisObj,
                                      NW_LMgr_FrameInfo_t *paddingInfo)
{
  NW_LMgr_PropertyValue_t value;
  NW_LMgr_Box_t *thisCell = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_Box_t *thisRow = NW_LMgr_BoxOf(thisCell->parent);
  NW_LMgr_StaticTableBox_t *thisSTB = NW_LMgr_StaticTableBoxOf(thisRow->parent);
  NW_LMgr_Box_t *thisTable = NW_LMgr_BoxOf(thisSTB);
  NW_LMgr_Property_t cellPadding;
  NW_GDI_Metric_t px;
  TBrowserStatusCode status;
  NW_GDI_Rectangle_t bounds;

  value.integer = 0;
  NW_LMgr_Box_GetPropertyValue(thisTable, NW_CSS_Prop_flags, NW_CSS_ValueType_Integer, &value);

  if (value.integer & NW_CSS_PropValue_flags_wmlTable) {
    if (paddingInfo != NULL) {
      paddingInfo->left = 1;
      paddingInfo->right = 1;
      paddingInfo->top = 1;
      paddingInfo->bottom = 1;
    }
  }
  else {

    cellPadding.value.integer = 0;
    cellPadding.type = NW_CSS_ValueType_Px;
    status = NW_LMgr_Box_GetRawProperty(thisTable, NW_CSS_Prop_cellPadding, &cellPadding);
    if (status == KBrsrSuccess) {
      if (cellPadding.type == NW_CSS_ValueType_Percentage) {
        bounds = NW_LMgr_Box_GetFormatBounds(thisCell);
        px = (NW_GDI_Metric_t)((cellPadding.value.decimal * bounds.dimension.width) / 100);
      }
      else {
        px = (NW_GDI_Metric_t)cellPadding.value.integer;
      }
      paddingInfo->left = paddingInfo->right = paddingInfo->top = paddingInfo->bottom = px;
    }
    else {
      NW_LMgr_Box_GetPadding(thisCell, paddingInfo, ELMgrFrameAll );
    }
  }
}

// ---------------------------------------------------------------------
// NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox() updates the side of an edge with data from a box.
//   The box is presumed to be more specific than the sources applied to the edge thus far.
//   If the box does not specify color data, then the color data in the edge is kept. 
static
void
NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(NW_LMgr_Box_t* box,  // Source for more specific edge data.
    NW_LMgr_StaticTableBorderSide_t side,  // The side to update.
    NW_LMgr_StaticTableBorderEdge_t* edge) // Cumulative edge data.
    {
    TBrowserStatusCode status;
    NW_LMgr_StaticTableBorderEdge_t tempEdge;
    NW_LMgr_Property_t prop;
    NW_LMgr_PropertyName_t cssPropName;


    NW_LMgr_StaticTableCellBox_GetEdge(box, side, &tempEdge);

    switch(side) {
        case top:
            cssPropName = NW_CSS_Prop_topBorderColor;
            break;
        case left:
            cssPropName = NW_CSS_Prop_leftBorderColor;
            break;
        case right:
            cssPropName = NW_CSS_Prop_rightBorderColor;
            break;
        case bottom:
            cssPropName = NW_CSS_Prop_bottomBorderColor;
            break;
        default:
            return;
        }

    
    // if the box doesn't specify color, keep value from less specific element.
    if (!edge->color)
        {
        status = NW_LMgr_Box_GetPropertyFromList(box, cssPropName, &prop);
        if (status != KBrsrNotFound) 
            {
            edge->color = prop.value.integer;
            }
        else
            {
            status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_borderColor, &prop);
            
            if (status != KBrsrNotFound) 
                {
                edge->color = prop.value.integer;
                }
            else
                {
                edge->color = tempEdge.color;
                }
            }
        }


    NW_LMgr_StaticTableCellBox_UpdateEdge(edge, &tempEdge);
    }



/* Public function to get the cell borders */
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_CalculateBorders(NW_LMgr_StaticTableCellBox_t *thisObj,
                                            NW_Uint16 currentCol, 
                                            NW_Uint16 currentRow, 
                                            NW_Uint16 colSpan, 
                                            NW_Uint16 rowSpan,
                                            NW_LMgr_StaticTableBorderEdge_t *leftEdge,
                                            NW_LMgr_StaticTableBorderEdge_t *rightEdge,
                                            NW_LMgr_StaticTableBorderEdge_t *topEdge,
                                            NW_LMgr_StaticTableBorderEdge_t *bottomEdge)
    {
    NW_LMgr_Box_t *thisCell = NW_LMgr_BoxOf(thisObj);
    NW_LMgr_Box_t *thisRow = NW_LMgr_BoxOf(thisCell->parent);
    NW_LMgr_StaticTableBox_t *thisSTB = NW_LMgr_StaticTableBoxOf(thisRow->parent);
    NW_LMgr_Box_t *thisTable = NW_LMgr_BoxOf(thisSTB);
    NW_LMgr_Box_t *tempBox = NULL;
    NW_LMgr_StaticTableContext_t *context = thisSTB->ctx;

    NW_LMgr_StaticTableBorderEdge_t tempEdge;

    NW_ADT_Vector_Metric_t numCols = NW_LMgr_StaticTableContext_GetNumCols(context);
    NW_ADT_Vector_Metric_t numRows = NW_LMgr_StaticTableContext_GetNumRows(context);

    NW_LMgr_PropertyValue_t value;

    NW_TRY(status) 
        {
    
        /* Optimization for WML tables */
        value.integer = 0;
        NW_LMgr_Box_GetPropertyValue(thisTable, NW_CSS_Prop_flags, NW_CSS_ValueType_Integer, &value);
    
        if (value.integer & NW_CSS_PropValue_flags_wmlTable) 
            {
            if (leftEdge) 
                {
                leftEdge->width = 1;
                leftEdge->style = NW_CSS_PropValue_solid;
                leftEdge->color = 0x000000;
                }
        
            if (rightEdge) 
                {
                rightEdge->width = 1;
                rightEdge->style = NW_CSS_PropValue_solid;
                rightEdge->color = 0x000000;
                }
        
            if (topEdge) 
                {
                topEdge->width = 1;
                topEdge->style = NW_CSS_PropValue_solid;
                topEdge->color = 0x000000;
                }
        
            if (bottomEdge) 
                {
                bottomEdge->width = 1;
                bottomEdge->style = NW_CSS_PropValue_solid;
                bottomEdge->color = 0x000000;
                }
        
            NW_THROW_SUCCESS(status);
            }
    
        /* Get left border */
        if (leftEdge) 
            {
            NW_Mem_memset(leftEdge, 0, sizeof(NW_LMgr_StaticTableBorderEdge_t));
        
            // Our cell wall has the highest priority.
            NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisCell, left, leftEdge);
            
            if (currentCol > 0) 
                {
                /* Update with the right edge of the neighbor on the left */
                status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy
                    (context, (NW_Uint16)(currentCol - 1), currentRow, &tempBox);
                NW_ASSERT(status != KBrsrNotFound);
                NW_THROW_ON_ERROR(status);

                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(tempBox, right, leftEdge);
                }
            else
                {
                // Now look at our row
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisRow, left, leftEdge);

                /* Check the table border */
                NW_LMgr_StaticTableCellBox_GetEdge(thisTable, left, &tempEdge);
                NW_LMgr_StaticTableCellBox_UpdateEdge(leftEdge, &tempEdge);
                }
            }
    
        /* Get right border */
        if (rightEdge) 
            {
            NW_Mem_memset(rightEdge, 0, sizeof(NW_LMgr_StaticTableBorderEdge_t));
        
            // Our own cell walls have the highest priority.
            NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisCell, right, rightEdge);

            if (currentCol + colSpan < numCols) 
                {
                /* Update with the left edge of the neighbor on the right */
                status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy
                    (context, (NW_Uint16)(currentCol + colSpan), currentRow, &tempBox);
                NW_ASSERT(status != KBrsrNotFound);
                NW_THROW_ON_ERROR(status);
            
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(tempBox, left, rightEdge);
                }
            else
                {
                // Now look at our row
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisRow, right, rightEdge);

                //If we are the last cell in the row, look at the table border 
                NW_LMgr_StaticTableCellBox_GetEdge(thisTable, right, &tempEdge);
                NW_LMgr_StaticTableCellBox_UpdateEdge(rightEdge, &tempEdge);
                }
            }
    
        /* Get top border */
        if (topEdge) 
            {
            NW_Mem_memset(topEdge, 0, sizeof(NW_LMgr_StaticTableBorderEdge_t));
            
            /* Initialize to our own cell walls */
            NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisCell, top, topEdge);

            /* Now look at our row */
            NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisRow, top, topEdge);
            
            if (currentRow > 0)
                {       
                // Look at row immediately before us
                tempBox = NW_LMgr_StaticTableBox_GetRow(thisSTB, (NW_Uint16)(currentRow - 1));
                NW_THROW_OOM_ON_NULL (tempBox, status);                    
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(tempBox, bottom, topEdge);

                /* Look at the cell immediately above us */
                status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy
                    (context, currentCol, (NW_Uint16)(currentRow - 1), &tempBox);
                NW_ASSERT(status != KBrsrNotFound);
                NW_THROW_ON_ERROR(status);
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(tempBox, bottom, topEdge);
                }
            else
                {
                /* Default to  the table border */
                NW_LMgr_StaticTableCellBox_GetEdge(thisTable, top, &tempEdge);
                NW_LMgr_StaticTableCellBox_UpdateEdge(topEdge, &tempEdge);
                }
            }
    
        /* Get bottom border */
        if (bottomEdge) 
            {
            NW_Mem_memset(bottomEdge, 0, sizeof(NW_LMgr_StaticTableBorderEdge_t));
        
            /* Default to our own cell walls */
            NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisCell, bottom, bottomEdge);

            /* Next look at our row */
            NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(thisRow, bottom, bottomEdge);

            if (currentRow + rowSpan < numRows) 
                {
                /* Look at row immediately after us */
                tempBox = NW_LMgr_StaticTableBox_GetRow(thisSTB, (NW_Uint16)(currentRow + rowSpan));
                NW_THROW_OOM_ON_NULL (tempBox, status);
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(tempBox, top, bottomEdge);            
            
                /* And the cell immediately below us */
                status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy
                    (context, currentCol, (NW_Uint16)(currentRow + rowSpan), &tempBox);
                NW_ASSERT(status != KBrsrNotFound);
                NW_THROW_ON_ERROR(status);
                NW_LMgr_StaticTableCellBox_UpdateEdgeFromBox(tempBox, top, bottomEdge);
                }
            else
                {
                NW_LMgr_StaticTableCellBox_GetEdge(thisTable, bottom, &tempEdge);
                NW_LMgr_StaticTableCellBox_UpdateEdge(bottomEdge, &tempEdge);
                }
            }
        }
    NW_CATCH(status) 
        {
        }
    NW_FINALLY 
        {
        return status;
        }
    NW_END_TRY
    }


/* ------------------------------------------------------------------------- */
/* Public function to get the cell background */
void
NW_LMgr_StaticTableCellBox_GetBackground(NW_LMgr_StaticTableCellBox_t *thisObj,
                                         NW_LMgr_Property_t *backgroundProp)
{ 
  NW_LMgr_Box_t *thisCell = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_Box_t *thisRow = NW_LMgr_BoxOf(thisCell->parent);
  NW_LMgr_Box_t *thisTable = NW_LMgr_BoxOf(thisRow->parent);
  NW_LMgr_Property_t prop;
  TBrowserStatusCode status;

  /* Find background color */
  if (backgroundProp != NULL) {

    /* First check the table background */
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_transparent;
    status = NW_LMgr_Box_GetProperty(thisTable, NW_CSS_Prop_backgroundColor, &prop);
    *backgroundProp = prop;

    /* Next the row background */
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_transparent;
    status = NW_LMgr_Box_GetProperty(thisRow, NW_CSS_Prop_backgroundColor, &prop);
    if (status == KBrsrSuccess && prop.type == NW_CSS_ValueType_Color) {
      *backgroundProp = prop;
    }

    /* Finally the cellbox */
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_transparent;
    status = NW_LMgr_Box_GetProperty(thisCell, NW_CSS_Prop_backgroundColor, &prop);
    if (status == KBrsrSuccess && prop.type == NW_CSS_ValueType_Color) {
      *backgroundProp = prop;
    }
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_GetMinimumCellSize(NW_LMgr_StaticTableCellBox_t *thisObj,
                                              NW_GDI_Dimension2D_t *minSize)
{
  NW_LMgr_FrameInfo_t borderWidth, paddingInfo;
  NW_LMgr_Box_t *cellBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_Box_t *rowBox = NW_LMgr_BoxOf(cellBox->parent);
  NW_LMgr_StaticTableBox_t *tableBox = NW_LMgr_StaticTableBoxOf(rowBox->parent);
  NW_LMgr_StaticTableContext_t *context = tableBox->ctx;
  NW_GDI_Dimension2D_t minContentSize;

  NW_TRY (status) {

    /* Get the border information */
    status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr
                (context, cellBox, &borderWidth, NULL, NULL); 
    NW_THROW_ON_ERROR(status);

    /* Get padding info */
  NW_LMgr_StaticTableCellBox_GetPadding(thisObj, &paddingInfo);

  /* Get the minimum content size */
  status = NW_LMgr_Box_GetMinimumContentSize(cellBox, &minContentSize);
    _NW_THROW_ON_ERROR (status);

  /* Calculate the minimum size for the box */
    minSize->width = (NW_GDI_Metric_t)
      (minContentSize.width +  borderWidth.left + borderWidth.right +
       paddingInfo.left + paddingInfo.right);
    minSize->height = (NW_GDI_Metric_t)
      (minContentSize.height + borderWidth.top + borderWidth.bottom +
       paddingInfo.top + paddingInfo.bottom);

  } NW_CATCH (status) {
  } NW_FINALLY {
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_ShiftContent(NW_LMgr_StaticTableCellBox_t *thisObj,
                                        NW_GDI_Point2D_t *delta)
{
  NW_LMgr_BoxVisitor_t* visitor = NULL;

  NW_TRY (status) {
  NW_LMgr_Box_t *child;

  visitor = NW_LMgr_ContainerBox_GetBoxVisitor((NW_LMgr_ContainerBox_t*)thisObj);
    NW_THROW_OOM_ON_NULL (visitor, status);
 
  /* Invoke base-class shift on this box and all children */
  (void)NW_LMgr_BoxVisitor_NextBox(visitor, NULL);
  while((child = NW_LMgr_BoxVisitor_NextBox(visitor, NULL)) != NULL){
    status = NW_LMgr_Box_Class.NW_LMgr_Box.shift(child, delta);
      _NW_THROW_ON_ERROR(status);
  }

  } NW_CATCH (status) {
  } NW_FINALLY {
  NW_Object_Delete(visitor);
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_Stretch(NW_LMgr_StaticTableCellBox_t *thisObj,
                                   NW_GDI_Rectangle_t *newRect) 
{
  NW_GDI_Metric_t rowBaseline;
  NW_GDI_Metric_t rowHeight;
  NW_GDI_Metric_t cellHeight, cellBaseline;
  NW_GDI_Metric_t yOffset;
  NW_GDI_Point2D_t delta;
  NW_LMgr_Box_t *cellBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_Box_t *rowBox = NW_LMgr_BoxOf(cellBox->parent);
  NW_LMgr_PropertyValueToken_t align;
  NW_LMgr_Property_t prop;

  NW_TRY (status) {

  /* Get the cell info */
  cellBaseline = NW_LMgr_Box_GetBaseline (cellBox);
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( cellBox );

  cellHeight = boxBounds.dimension.height; 

  /* Get the row info */
  rowBaseline = NW_LMgr_Box_GetBaseline (rowBox);
  rowHeight = newRect->dimension.height;
  
  /* Get the alignment property.  If the cell does
   * not have it, try the row box (vAlign is not inherited!) */
  prop.value.token = NW_CSS_PropValue_baseline;
  status = NW_LMgr_PropertyList_Get (cellBox->propList, NW_CSS_Prop_verticalAlign, &prop);
  if ((status == KBrsrNotFound) || (prop.type & NW_CSS_ValueType_DefaultStyle)){
    status = NW_LMgr_Box_GetProperty (rowBox, NW_CSS_Prop_verticalAlign, &prop);
  }
  align = prop.value.token;

  /* We need to calculate the final position for the cell content */

  /* First we account for borders and padding */
  yOffset = 0;

  /* Now we deal with vertical alignment */
  switch (align) {
  case NW_CSS_PropValue_baseline: 
    yOffset = (NW_GDI_Metric_t)(rowBaseline - cellBaseline);
    break;
  case NW_CSS_PropValue_top: 
    yOffset = (NW_GDI_Metric_t)0;
    break;
  case NW_CSS_PropValue_bottom: 
    yOffset = (NW_GDI_Metric_t)(rowHeight - cellHeight);
    break;
  case NW_CSS_PropValue_middle: 
    yOffset = (NW_GDI_Metric_t)(rowHeight / 2 - cellHeight / 2);
    break;
  }

  /* Now we shift the content to its final position */
  delta.x = (NW_GDI_Metric_t)(newRect->point.x - boxBounds.point.x);
  delta.y = (NW_GDI_Metric_t)(newRect->point.y + yOffset - boxBounds.point.y);

  status = NW_LMgr_StaticTableCellBox_ShiftContent(thisObj, &delta);
    _NW_THROW_ON_ERROR (status);

  /* Finally we set the box size to the final size */
  NW_LMgr_Box_SetFormatBounds(cellBox, *newRect);

  } NW_CATCH (status) {
  } NW_FINALLY {
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
const
NW_LMgr_StaticTableCellBox_Class_t  NW_LMgr_StaticTableCellBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_BidiFlowBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_StaticTableCellBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_StaticTableCellBox_Resize,
                                    _NW_LMgr_StaticTableCellBox_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_FormatBox_Constrain,
    /* draw                      */ _NW_LMgr_StaticTableCellBox_Draw,
    /* render                    */ _NW_LMgr_StaticTableCellBox_Render,
    /* getBaseline               */ _NW_LMgr_FlowBox_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox         */
    /* applyFormatProps          */ _NW_LMgr_StaticTableCellBox_ApplyFormatProps
  },
  { /* NW_LMgr_FlowBox           */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_BidiFlowBox       */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_StaticTableCellBox*/
    /* unused                    */ NW_Object_Unused
  }
};



TBrowserStatusCode
NW_LMgr_StaticTableCell_SetConstraint (NW_LMgr_StaticTableRowBox_t* thisObj, NW_LMgr_Box_t *cellBox)
{
  NW_LMgr_Box_t *rowBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_StaticTableBox_t *table = NW_LMgr_StaticTableBoxOf(rowBox->parent);
  NW_LMgr_StaticTableContext_t *context;
  NW_ADT_Vector_Metric_t cellIndex, rowIndex;
  NW_ADT_Vector_Metric_t currentCol, currentRow;
  NW_ADT_Vector_Metric_t i;
  NW_ADT_Vector_Metric_t colSpan;
  NW_GDI_Metric_t constraint;

  NW_TRY(status) {

     /* Get the table context */
    context = NW_LMgr_StaticTableBox_GetContext(table);

    /* Which row is this? */
    rowIndex = NW_LMgr_ContainerBox_GetChildIndex(rowBox->parent, rowBox);
    cellIndex = NW_LMgr_ContainerBox_GetChildIndex(NW_LMgr_ContainerBoxOf(rowBox), cellBox);

    /* Constrain the cells */

      /* Get the position of the cell in the table */
      status = NW_LMgr_StaticTableContext_GetCellPxPyFromLxLy
                                             (context, cellIndex, rowIndex,
                                              &currentCol, &currentRow,
                                              &colSpan, NULL);
      if (status == KBrsrNotFound) {     
        /* This is a legal condition. */
        /* It happens if cells are not included in the table */
        status = KBrsrSuccess;       
        // TODO handle the error - Jay
        //continue;
      }
      NW_THROW_ON_ERROR(status);

      /* Calculate the constraint for the cell */
      constraint = 0;
      for (i = currentCol; i < currentCol + colSpan; i++) {
        constraint = (NW_GDI_Metric_t)
          (constraint + NW_LMgr_StaticTableContext_GetColConstraint(context, i));
      }
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( cellBox );
      /* Constrain the cell and let it resize itself */
      boxBounds.dimension.width = constraint;
      NW_LMgr_Box_SetFormatBounds( cellBox, boxBounds );
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context)
{
  TBrowserStatusCode   status ;
  NW_LMgr_StaticTableCell_SetConstraint((NW_LMgr_StaticTableRowBox_t*)box->parent, box );
  /* invoke superclass resize */
  status = NW_LMgr_FormatBox_Class.NW_LMgr_Box.resize(box, context);
  return status;
}


TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_PostResize(NW_LMgr_Box_t* box)
{
  NW_GDI_Dimension2D_t size;
  NW_LMgr_FrameInfo_t padding, borderWidth;
  NW_GDI_Rectangle_t newRect;
  NW_GDI_Metric_t newHeight;
  NW_LMgr_StaticTableCellBox_t *thisObj = NW_LMgr_StaticTableCellBoxOf(box);
  NW_LMgr_StaticTableBox_t* thisTable = 
              NW_LMgr_StaticTableBoxOf(NW_LMgr_BoxOf(box->parent)->parent);
  NW_LMgr_StaticTableContext_t* tableContext = thisTable->ctx;

  NW_TRY(status) {

    /* Get the box width and height properties */
    NW_LMgr_Box_GetSizeProperties(box, &size);

    /* If the height is explicitely set, we need to stretch/crop the cell */
    if (size.height >= 0) {

      /* Get the border information */
      status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr
                  (tableContext, box, &borderWidth, NULL, NULL); 
      NW_THROW_ON_ERROR(status);

      /* Get padding info */
      NW_LMgr_StaticTableCellBox_GetPadding(thisObj, &padding);

      newHeight = (NW_GDI_Metric_t)
          (size.height + borderWidth.top + borderWidth.bottom
           + padding.top + padding.bottom);

      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

      if (boxBounds.dimension.height < newHeight) 
        {
        newRect = boxBounds;
        newRect.dimension.height = newHeight;

        status = NW_LMgr_StaticTableCellBox_Stretch(thisObj, &newRect);
        _NW_THROW_ON_ERROR(status);
        }
      else 
        {
        boxBounds.dimension.height = (NW_GDI_Metric_t)
          (size.height + borderWidth.top + borderWidth.bottom
           + padding.top + padding.bottom);
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

/* -------------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Render (NW_LMgr_Box_t* box,
                                    CGDIDeviceContext * deviceContext,
                                    NW_GDI_Rectangle_t* clipRect,
                                    NW_LMgr_Box_t *currentBox,
                                    NW_Uint8 flags, 
                                    NW_Bool* hasFocus, 
                                    NW_Bool* skipChildren,
                                    NW_LMgr_RootBox_t* rootBox )
{
  NW_GDI_Rectangle_t oldClip;
  NW_GDI_Rectangle_t newClip;
  NW_GDI_Rectangle_t realClip;
  NW_LMgr_PropertyValue_t value;
  NW_LMgr_PropertyValue_t floatVal;
  NW_LMgr_PropertyValue_t visibilityVal;
  NW_GDI_Rectangle_t viewBounds;  
  NW_GDI_Rectangle_t visibleArea;
  NW_Bool clipRectChanged = NW_FALSE;
  NW_Bool containerVisible = NW_FALSE;
  void** ptr = NULL;
  NW_Bool cachePresent = NW_FALSE;

  NW_LMgr_StaticTableCellBox_t *thisObj = NW_LMgr_StaticTableCellBoxOf(box);
  
  NW_LMgr_Box_t *rowBox = NW_LMgr_BoxOf(box->parent);
  NW_LMgr_StaticTableBox_t *thisTable  = NW_LMgr_StaticTableBoxOf(rowBox->parent);

  NW_LMgr_StaticTableContext_t* thisContext = thisTable->ctx;

  // Save the old clip rect 
  oldClip = deviceContext->ClipRect();

  NW_TRY (status) 
    {
    // Should we draw floats? 
    if (!(flags & NW_LMgr_Box_Flags_DrawFloats)) 
      {
      floatVal.token = NW_CSS_PropValue_none;
      NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatVal);
      if (floatVal.token != NW_CSS_PropValue_none) 
        {
        NW_THROW_SUCCESS (status);
        }
      }
    // check if the this box has focus
    // if hasFocus is already set to true by any parent
    // then it should not be overuled.
    if( !(*hasFocus) && currentBox == box )
      {
      *hasFocus = NW_TRUE;
      }

    // Get the view bounds 
    viewBounds = *(deviceContext->DisplayBounds());
    viewBounds.point = *(deviceContext->Origin());

    // Determine the part of the box within the clip.  
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
    if (!NW_GDI_Rectangle_Cross(clipRect, &(boxBounds), &visibleArea)) 
      {
      NW_THROW_SUCCESS( status );
      }
    // Is the box visible on-screen?  If not, stil try to draw
    // the children 
    if (!NW_GDI_Rectangle_Cross(&viewBounds, &visibleArea, NULL)) 
      {
      NW_THROW_SUCCESS( status );
      }
    // If the clip is not visible on-screen, we 
    // draw neither the Container nor its children;
    // in case we are calculating the extents, we will still
    // have to peek into all children 
    if (!NW_GDI_Rectangle_Cross(clipRect, &viewBounds, &realClip)) 
      {
      NW_THROW_SUCCESS( status );
      }

    // Save the old clip rect 
    oldClip = deviceContext->ClipRect();
    // Set the new clip rect 
    deviceContext->SetClipRect(  &realClip );
    clipRectChanged = NW_TRUE;

    // If the visibility val is set to hidden, don't draw the 
    // Container -- but still try to draw the children 
    visibilityVal.token = NW_CSS_PropValue_visible;
    (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
    if (visibilityVal.token != NW_CSS_PropValue_visible) 
      {
      goto drawChildren;
      }   

    // make the box draw itself 
    status = NW_LMgr_Box_Draw (box, deviceContext, NW_FALSE);
    containerVisible = NW_TRUE;
    _NW_THROW_ON_ERROR (status);

    // Add the box and the clip to the cache 
    cachePresent = NW_LMgr_Box_Update_RenderCache (rootBox, box, &realClip, &ptr);
    if (cachePresent)
        {
		NW_THROW_OOM_ON_NULL(ptr, status);
		}

drawChildren:
 
    // Modify the clip rectangle, if necessary 
    thisObj->cellClip = *clipRect;

    value.token = NW_CSS_PropValue_visible;
    NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_overflow, NW_CSS_ValueType_Token, &value);
    if (value.token == NW_CSS_PropValue_hidden) 
      {
      NW_LMgr_FrameInfo_t padding, borderWidth;

      // Get the border information 
      status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr
                  (thisContext, box, &borderWidth, NULL, NULL);
      if (status == KBrsrNotFound)
        {
          NW_THROW(status);
        }
      NW_THROW_ON_ERROR(status);

      // Get padding info 
     NW_LMgr_StaticTableCellBox_GetPadding(thisObj, &padding);
     NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );

      thisObj->cellClip.point.x = (NW_GDI_Metric_t)(boxBounds.point.x 
                                   + borderWidth.left + padding.left);
      thisObj->cellClip.point.y = (NW_GDI_Metric_t)(boxBounds.point.y 
                                   + borderWidth.top + padding.top);
      thisObj->cellClip.dimension.width = (NW_GDI_Metric_t)(boxBounds.dimension.width 
                                   - borderWidth.left - padding.left
                                   - borderWidth.right - padding.right);
      thisObj->cellClip.dimension.height = (NW_GDI_Metric_t)(boxBounds.dimension.height
                                   - borderWidth.top - padding.top
                                   - borderWidth.bottom - padding.bottom);

      if (!NW_GDI_Rectangle_Cross(&(thisObj->cellClip), clipRect, &newClip)) 
        {
        // children are out of view
        containerVisible = NW_FALSE;
        NW_THROW_SUCCESS (status);
        }
      else
      {
        // modify cliprect for the children.
        *clipRect = newClip;
      }
      thisObj->cellClip = newClip;
      }
    } 
  NW_CATCH (status) 
    {
    } 
  NW_FINALLY 
    {
    // skip children unless the container is visible
    *skipChildren = (NW_Bool)( containerVisible == NW_TRUE ? NW_FALSE : NW_TRUE );
    if( clipRectChanged == NW_TRUE )
      {
      // Reset the clip rect 
      deviceContext->SetClipRect( &oldClip);
      }

    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Draw(NW_LMgr_Box_t *box,
                                 CGDIDeviceContext *deviceContext,
                                 NW_Bool hasFocus)
{

  NW_LMgr_Box_t tempBox;
  NW_LMgr_FrameInfo_t borderWidth, borderStyle, borderColor;
  NW_LMgr_Box_t *cellBox = box;
  NW_GDI_Rectangle_t clipRect;
  NW_GDI_Metric_t initialLineWidth;

  NW_LMgr_StaticTableBox_t* thisTable = 
              NW_LMgr_StaticTableBoxOf(NW_LMgr_BoxOf(box->parent)->parent);
  NW_LMgr_StaticTableContext_t* thisContext = thisTable->ctx;

  NW_TRY (status) {

  NW_REQUIRED_PARAM(hasFocus);
  /* Get a helper box for drawing */
  status = NW_LMgr_Box_Initialize (&tempBox, 1);
  _NW_THROW_ON_ERROR (status);

  /* Get the border information */
    status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr
                (thisContext, box, &borderWidth, &borderStyle, &borderColor); 
    NW_THROW_ON_ERROR(status);

  /* Set coordinates */
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( cellBox );
  NW_LMgr_Box_SetFormatBounds( &tempBox, boxBounds );
  NW_LMgr_Box_UpdateDisplayBounds( &tempBox, boxBounds );

  clipRect = deviceContext->ClipRect();
  if (!NW_GDI_Rectangle_Cross(&(boxBounds), &clipRect, NULL)) {
     NW_THROW_SUCCESS(status);
  }
  NW_LMgr_Box_Predraw (cellBox, deviceContext, hasFocus);

  initialLineWidth = deviceContext->LineWidth();

  deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);

  /* draw the border */
  /* TODO add support for other border types */
    status = NW_LMgr_Box_DrawBorder( NW_LMgr_Box_GetDisplayBounds(&tempBox), deviceContext, &borderWidth, &borderStyle, &borderColor);
    _NW_THROW_ON_ERROR(status);

  /* set back the original values */
  deviceContext->SetLineWidth( initialLineWidth);

  } NW_CATCH (status) {
  } NW_FINALLY {
    NW_Object_Terminate (&tempBox);
  return status;
  } NW_END_TRY
}

TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                             NW_LMgr_FormatContext_t* context)
{
  NW_LMgr_Box_t *box = NW_LMgr_BoxOf(format);
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_FrameInfo_t borderWidth;
  NW_LMgr_RootBox_t *rootBox;
  NW_LMgr_PropertyValue_t dirValue;
  NW_GDI_Metric_t bottomMargin = 0;
  NW_LMgr_StaticTableCellBox_t *thisObj = NW_LMgr_StaticTableCellBoxOf(format);
  NW_LMgr_StaticTableBox_t* thisTable = 
              NW_LMgr_StaticTableBoxOf(NW_LMgr_BoxOf(((NW_LMgr_Box_t *) format)->parent)->parent);
  NW_LMgr_StaticTableContext_t* thisContext = thisTable->ctx;

  NW_TRY(status) {

  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );    
  context->startMargin = 0;
  context->endMargin = boxBounds.dimension.width;
  context->y = 0;

  /* Table cells do not have margins.  As a result, we only include the
   * padding and border areas when calculating the start margin. */

  NW_LMgr_StaticTableCellBox_GetPadding(thisObj, &padding);

  context->startMargin = (NW_GDI_Metric_t)(context->startMargin + padding.left);
  context->endMargin = (NW_GDI_Metric_t)(context->endMargin - padding.right);
  context->y = (NW_GDI_Metric_t)(context->y + padding.top);
  bottomMargin = (NW_GDI_Metric_t)padding.bottom;

  /* Get the border information */
  status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr
              (thisContext, box, &borderWidth, NULL, NULL); 
  NW_THROW_ON_ERROR(status);
 
  context->startMargin = (NW_GDI_Metric_t) (context->startMargin + borderWidth.left); 
  context->endMargin = (NW_GDI_Metric_t) (context->endMargin - borderWidth.right); 
  context->y = (NW_GDI_Metric_t) (context->y + borderWidth.top);
  bottomMargin = (NW_GDI_Metric_t) (bottomMargin + borderWidth.bottom);

  /* Now reset initial height. This will change as children are laid out */  
  boxBounds.dimension.height = 
    (NW_GDI_Metric_t) (context->y + bottomMargin);
  NW_LMgr_Box_SetFormatBounds( box, boxBounds ); 

  /* Set the global context */
  if (!NW_Object_IsClass(format, &NW_LMgr_RootBox_Class)) {
    rootBox = NW_LMgr_Box_GetRootBox (box);
    NW_ASSERT (rootBox != NULL);
  }
  else {
    rootBox = NW_LMgr_RootBoxOf(format);
  }
  context->placedFloats = rootBox->placedFloats;
  context->pendingFloats = rootBox->pendingFloats;

  /* Now set the flow-specific context variables */
  context->lineStart = context->startMargin;
  context->lineEnd = context->endMargin;

  /* Get the flow direction */
  dirValue.token = NW_CSS_PropValue_ltr;
  (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(format), 
                                     NW_CSS_Prop_textDirection,
                                     NW_CSS_ValueType_Token, 
                                     &dirValue);
  if (dirValue.token == NW_CSS_PropValue_ltr) {
    context->direction = NW_GDI_FlowDirection_LeftRight;
  }
  else {
    context->direction = NW_GDI_FlowDirection_RightLeft;
  }

  /* Set the initial x position */
  if (dirValue.token == NW_CSS_PropValue_ltr) {
    context->x = (NW_GDI_Metric_t)(context->lineStart + 
                      NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBoxOf(format),
                                                     context));
  }
  else {
    context->x = (NW_GDI_Metric_t)(context->lineEnd - 
                      NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBoxOf(format),
                                                     context));
  }

  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_StaticTableCellBox_Float(NW_LMgr_FlowBox_t* flow, 
                                  NW_LMgr_Box_t* box,
                                  NW_LMgr_FormatContext_t* context){

  NW_ADT_DynamicVector_t *pendingFloats = context->pendingFloats;
  NW_ADT_DynamicVector_t *placedFloats = context->pendingFloats;

  NW_REQUIRED_PARAM(flow);

  /* If the float context is not set, remove the box */
  if (pendingFloats == NULL || placedFloats == NULL) {
    return KBrsrFailure;
  }

  if (NW_ADT_DynamicVector_InsertAt(pendingFloats, &box, NW_ADT_Vector_AtEnd) == NULL) {
    return KBrsrOutOfMemory;
  }
  // we do not handle floating in cell.
  return KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_StaticTableCellBox_t*
NW_LMgr_StaticTableCellBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_StaticTableCellBox_t*)
    NW_Object_New (&NW_LMgr_StaticTableCellBox_Class, numProperties);
}
