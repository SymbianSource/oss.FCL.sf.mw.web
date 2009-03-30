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


#include "nw_lmgr_statictablerowboxi.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_cssproperties.h"
#include "BrsrStatusCodes.h"
#include "nw_lmgr_textbox.h"



/* ------------------------------------------------------------------------- */


const
NW_LMgr_StaticTableRowBox_Class_t  NW_LMgr_StaticTableRowBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_FormatBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_StaticTableRowBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_StaticTableRowBox_ResizeRow,
    /* postResize                */ _NW_LMgr_StaticTableRowBox_PostResizeRow,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize, 
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_StaticTableRowBox_Draw,
    /* render                    */ _NW_LMgr_StaticTableRowBox_Render,
    /* getBaseline               */ _NW_LMgr_StaticTableRowBox_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox         */
    /* applyFormatProps          */ _NW_LMgr_StaticTableRowBox_ApplyFormatProps
  },
    /* NW_LMgr_StaticTableRowBox */
  {
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
/* Virtual method implementations                                            */
/* ------------------------------------------------------------------------- */

NW_GDI_Metric_t
_NW_LMgr_StaticTableRowBox_GetBaseline(NW_LMgr_Box_t* rowBox)
{
  NW_LMgr_ContainerBox_t *rowContainer = NW_LMgr_ContainerBoxOf(rowBox);
  NW_ADT_Vector_Metric_t cellIndex, cellCount;
  NW_LMgr_PropertyValueToken_t align;
  NW_LMgr_Box_t *cellBox;
  NW_GDI_Metric_t tempBase = 0, rowBaseline = 0;
  NW_LMgr_Property_t prop;

  cellCount = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

  /* Look for a baseline-aligned box */
  for (cellIndex = 0; cellIndex < cellCount; cellIndex++) { 

    cellBox = NW_LMgr_ContainerBox_GetChild(rowContainer, cellIndex);

    /* Get the alignment property */
    prop.value.token = NW_CSS_PropValue_baseline;
    (void)NW_LMgr_Box_GetProperty (cellBox, NW_CSS_Prop_verticalAlign, &prop);
    align = prop.value.token;

    if (align == NW_CSS_PropValue_baseline) {
      tempBase = NW_LMgr_Box_GetBaseline(cellBox);
      if (tempBase > rowBaseline) {
        rowBaseline = tempBase;
      }
    }
  }

  /* Otherwise, the baseline is not defined */
  return rowBaseline;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableRowBox_Draw
 * Parameters:    box - the table box
 *                deviceContext - the device context
 *                hasFocus - does the box have focus?
 * Description:   This method overrides the base class draw.  We leave it
 *                blank because we don't want the table row to draw its own
 *                borders.  Table borders will be drawn by the cells that
 *                own them.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
TBrowserStatusCode
_NW_LMgr_StaticTableRowBox_Draw (NW_LMgr_Box_t* box,
                                 CGDIDeviceContext* deviceContext,
                                 NW_Uint8 hasFocus)
{
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(deviceContext);
  NW_REQUIRED_PARAM(hasFocus);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_StaticTableRowBox_Render (NW_LMgr_Box_t* box,
                                    CGDIDeviceContext *deviceContext,
                                    NW_GDI_Rectangle_t* clipRect,
                                    NW_LMgr_Box_t *currentBox,
                                    NW_Uint8 flags, 
                                    NW_Bool* hasFocus, 
                                    NW_Bool* skipChildren,
                                    NW_LMgr_RootBox_t* rootBox )
{
  NW_LMgr_PropertyValue_t visibilityVal;

  // Check if visibility val for the row is collapse 
  visibilityVal.token = NW_CSS_PropValue_visible;
  (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                       NW_CSS_ValueType_Token, &visibilityVal);
  if (visibilityVal.token == NW_CSS_PropValue_collapse) 
    {
    return KBrsrSuccess;
  }

  // Now call the base class render to render the children 
  // invoke our superclass constructor 
  return _NW_LMgr_ContainerBox_Render (box,
                                       deviceContext,
                                       clipRect,
                                       currentBox,
                                       flags, 
                                       hasFocus, 
                                       skipChildren,
                                       rootBox );
}

TBrowserStatusCode
_NW_LMgr_StaticTableRowBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                             NW_LMgr_FormatContext_t* context)
{
	NW_REQUIRED_PARAM(format);
	NW_REQUIRED_PARAM(context);

	return NW_TRUE;
}

/* ------------------------------------------------------------------------- *
 * Public final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_StaticTableRowBox_ConstrainRow (NW_LMgr_StaticTableRowBox_t* rowBox)
{
  NW_LMgr_Box_t *thisRow = NW_LMgr_BoxOf(rowBox);
  NW_LMgr_Box_t *thisTable = NW_LMgr_BoxOf(thisRow->parent);

  NW_GDI_Rectangle_t rowBoxBounds = NW_LMgr_Box_GetFormatBounds( thisRow ); 
  NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds( thisTable ); 

  rowBoxBounds.dimension.width = tableBoxBounds.dimension.width;
  NW_LMgr_Box_SetFormatBounds( thisRow, rowBoxBounds ); 

  return KBrsrSuccess;
}


TBrowserStatusCode
_NW_LMgr_StaticTableRowBox_ResizeRow (NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context)
{
  NW_LMgr_StaticTableRowBox_t* rowBox = (NW_LMgr_StaticTableRowBox_t*) box;
  NW_LMgr_StaticTableBox_t* thisObj = (NW_LMgr_StaticTableBox_t*)NW_LMgr_Box_GetParent(rowBox);
  NW_LMgr_PropertyValue_t visibilityVal;
  TBrowserStatusCode status = KBrsrSuccess;
 
  visibilityVal.token = NW_CSS_PropValue_visible;
  (void) NW_LMgr_Box_GetPropertyValue (box, 
                                       NW_CSS_Prop_visibility,
                                       NW_CSS_ValueType_Token, 
                                       &visibilityVal);
  if (visibilityVal.token != NW_CSS_PropValue_collapse) {
  // call contraint row only on fixed algorithm
  if( thisObj->automaticWidthPass == NW_LMgr_StaticTableBox_AutomaticWidth_UndefinedPass ) 
    {
    (void)NW_LMgr_StaticTableRowBox_ConstrainRow( NW_LMgr_StaticTableRowBoxOf( rowBox ) );
    }

    context->formatBox = box;
    context->formatChildren = NW_TRUE;
    context->newFormatContext = context;
    context->referenceCount ++;
    }
  return status;
}

/* ------------------------------------------------------------------------- */
/* ResizeRow resizes the individual cells in the row and calculates
 * its final height and baseline.  At the end we increment the cumulative
 * height of the table content.
 * ResizeRow assumes that the row originates at (0,0).  We will shift it
 * to its final vertical position in PlaceRow.
 */
TBrowserStatusCode
_NW_LMgr_StaticTableRowBox_PostResizeRow (NW_LMgr_Box_t* box)
{
  TBrowserStatusCode status ;
  NW_LMgr_StaticTableRowBox_t* rowBox = (NW_LMgr_StaticTableRowBox_t*) box;
  NW_LMgr_Box_t *thisRow = NW_LMgr_BoxOf(rowBox);
  NW_LMgr_ContainerBox_t *rowContainer = NW_LMgr_ContainerBoxOf(rowBox);
  NW_LMgr_Box_t *tableBox = NW_LMgr_BoxOf(thisRow->parent);
  NW_LMgr_ContainerBox_t *tableContainer = NW_LMgr_ContainerBoxOf(tableBox);

  NW_ADT_Vector_Metric_t cellCount, cellIndex;
  NW_ADT_Vector_Metric_t currentRow;
  NW_ADT_Vector_Metric_t rowSpan;
  NW_ADT_Vector_Metric_t i;
  NW_GDI_Metric_t rowHeight, rowBaseline, rowDescent;
  NW_GDI_Metric_t cellHeight, cellBaseline;
  NW_LMgr_Property_t prop;
  NW_LMgr_Box_t* cellBox;
  NW_GDI_Dimension2D_t rowSize;
  NW_LMgr_PropertyValueToken_t align;
  NW_Uint8 pass;
  NW_LMgr_Box_t *tempRowBox;


    /* Visit each cell in the row */
    cellCount =
      NW_LMgr_ContainerBox_GetChildCount(rowContainer);

    /* Initialize the row info */
    currentRow = NW_LMgr_ContainerBox_GetChildIndex(tableContainer, thisRow);
    rowHeight = 0;
    rowBaseline = 0;
  
    /* We pass through the row twice.  In the first pass we calculate the
     * baseline for the row, if any.  Next we calculate the height
     * for the row given the baseline */
    for (pass = 0; pass < 2; pass++) {
      for (cellIndex = 0; cellIndex < cellCount; cellIndex++) { 

        /* Get the cell */
        cellBox = NW_LMgr_ContainerBox_GetChild(rowContainer, cellIndex);

        /* Get the cell info */
        cellBaseline = NW_LMgr_Box_GetBaseline (cellBox);
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( cellBox ); 

        cellHeight = boxBounds.dimension.height; 

        /* Get the alignment property */
        prop.value.token = NW_CSS_PropValue_baseline;
        status = NW_LMgr_Box_GetProperty (cellBox, NW_CSS_Prop_verticalAlign, &prop);
        if (status == KBrsrNotFound) {
          (void)NW_LMgr_Box_GetProperty (thisRow, NW_CSS_Prop_verticalAlign, &prop);
        }
        align = prop.value.token;

        /* If the rowSpan property is >1, we must take into account the other
         * rows that the cell spans.  The minimum height that this cell will
         * take up on our row will be its height minus the total height of
         * any other rows that it spans */
        prop.value.integer = 1;
        (void)NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_rowSpan,
                                               &prop);
        rowSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
        for (i = (NW_ADT_Vector_Metric_t)(currentRow + 1); 
             i < currentRow + rowSpan; 
             i++) {
          tempRowBox = NW_LMgr_ContainerBox_GetChild(tableContainer, i);
          if (tempRowBox)
          {
          NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( tempRowBox ); 
          cellHeight = (NW_GDI_Metric_t)
            (cellHeight - boxBounds.dimension.height);
          }
        }

        /* Calculate the baseline and the height */
        switch (pass) {
        case 0:
          if (align == NW_CSS_PropValue_baseline) {
            if (cellHeight - cellBaseline > rowHeight - rowBaseline) {
              rowDescent = (NW_GDI_Metric_t)(cellHeight - cellBaseline);
            }
            else {
              rowDescent = (NW_GDI_Metric_t)(rowHeight - rowBaseline);
            }
            rowBaseline = (NW_GDI_Metric_t)
              ((rowBaseline > cellBaseline) ? rowBaseline : cellBaseline);
            rowHeight = (NW_GDI_Metric_t)(rowBaseline + rowDescent);
          }
          break;
        case 1:
          if (rowHeight < cellHeight) {
            switch (align) {
            case NW_CSS_PropValue_bottom:
              rowBaseline =
                (NW_GDI_Metric_t)(rowBaseline + cellHeight - rowHeight);
              break;
            case NW_CSS_PropValue_middle:
              rowBaseline =
                (NW_GDI_Metric_t)(rowBaseline + (cellHeight - rowHeight)/2);
              break;
            }
            rowHeight = cellHeight;
          }
          break;
        }
      }
    }
    /* If the row height we obtained is less than the specified row height,
       we use the specified value */
    NW_LMgr_Box_GetSizeProperties(thisRow, &rowSize);
    if (rowSize.height > rowHeight) {
      rowHeight = rowSize.height;
    }
    NW_GDI_Rectangle_t rowBoxBounds = NW_LMgr_Box_GetFormatBounds( thisRow ); 
    rowBoxBounds.dimension.height = rowHeight;
    NW_LMgr_Box_SetFormatBounds( thisRow, rowBoxBounds ); 
  return KBrsrSuccess;
  }



/* ------------------------------------------------------------------------- */
/* The alignment algorithm is defined in [CSS2 17.5.3].
 * 1.  First we align any cells with align=baseline.  This establishes the
 *     baseline for the row.
 * 2.  Next we align cells with align=top.  This establishes the top position
 *     for the row and a provisional height.
 * 3.  Finally we align the bottom- and middle-aligned cells.  This may
 *     increase the total height of the row.
 * 4.  If a cell spans several rows, it is positioned at the time we
 *     reach the first row in which it is contained.  We start with the
 *     last row in order to facilitate this behavior. [This not in CSS2]
 */
TBrowserStatusCode
NW_LMgr_StaticTableRowBox_PlaceRow (NW_LMgr_StaticTableRowBox_t* rowBox,
                                    NW_GDI_Metric_t atX, NW_GDI_Metric_t atY)
{
  NW_LMgr_Box_t *thisRow ;
  NW_LMgr_ContainerBox_t *rowContainer ;
  NW_LMgr_Box_t *tableBox ;
  NW_LMgr_ContainerBox_t *tableContainer ;
  NW_LMgr_StaticTableContext_t *thisContext ;

  NW_GDI_Rectangle_t newRect;
  NW_ADT_Vector_Metric_t cellCount, cellIndex;
  NW_ADT_Vector_Metric_t currentRow, currentCol, prevCol;
  NW_ADT_Vector_Metric_t i;
  NW_GDI_Metric_t cellHeight;
  NW_GDI_Metric_t currentX;
  NW_ADT_Vector_Metric_t rowSpan, colSpan;
  NW_LMgr_Box_t* cellBox;
  NW_LMgr_Box_t* currentRowBox;


  thisRow = NW_LMgr_BoxOf(rowBox);
    
  rowContainer = NW_LMgr_ContainerBoxOf(rowBox);


  tableBox = NW_LMgr_BoxOf(thisRow->parent);
  tableContainer = NW_LMgr_ContainerBoxOf(tableBox);
  thisContext = NW_LMgr_StaticTableBoxOf(tableBox)->ctx;
  NW_TRY(status) {
    /* Visit each cell in the row */
    cellCount =
      NW_LMgr_ContainerBox_GetChildCount(rowContainer);

    /* Initialize the row info */
    currentRow = NW_LMgr_ContainerBox_GetChildIndex(tableContainer, thisRow);
    currentX = atX;
    currentCol = 0;
  
    /* Place the cells */
    for (cellIndex = 0; cellIndex < cellCount; cellIndex++) { 

      /* Get the cell */
      cellBox = NW_LMgr_ContainerBox_GetChild(rowContainer, cellIndex);
    // JADE hack; to-do: why cellbox is not staticcellbox?
    if (!NW_Object_IsInstanceOf (cellBox, &NW_LMgr_StaticTableCellBox_Class))
	    {
	    continue;
	    }
      
      /* Determine its position */
      prevCol = currentCol;
      status = NW_LMgr_StaticTableContext_LxLyToPxPy
                       (thisContext, cellIndex, currentRow,
                        &currentCol, &currentRow,
                        &colSpan, &rowSpan);
      if (status == KBrsrNotFound) {
        status = KBrsrSuccess;
          break;
        }
       NW_THROW_ON_ERROR(status);

      for (i = prevCol; i < currentCol; i++) {
        currentX = (NW_GDI_Metric_t)
          (currentX + NW_LMgr_StaticTableContext_GetColConstraint(thisContext, i));
      }

      /* If the rowSpan property is >1, we calculate the total space that
       * the cell will occupy by adding the heights of all its rows. */
      cellHeight = 0;
      for (i = currentRow; i < currentRow + rowSpan; i++) {
        currentRowBox = NW_LMgr_ContainerBox_GetChild(tableContainer, i);
        NW_GDI_Rectangle_t rowBoxBounds = NW_LMgr_Box_GetFormatBounds( currentRowBox); 

        cellHeight = (NW_GDI_Metric_t)
          (cellHeight
           + rowBoxBounds.dimension.height);
      }

      /* Get the new rectangle for the cell */
      NW_GDI_Metric_t currentColConstraint = 
          NW_LMgr_StaticTableContext_GetColConstraint(thisContext, currentCol);
      NW_GDI_Rectangle_t cellBoxBounds = NW_LMgr_Box_GetFormatBounds( cellBox ); 
      newRect.point.x = currentX;
      newRect.point.y = atY;
      newRect.dimension.width = cellBoxBounds.dimension.width;
      if (newRect.dimension.width < currentColConstraint)
          {
          newRect.dimension.width = currentColConstraint;
          }
      newRect.dimension.height = cellHeight;

      /* Stretch both moves the cell to its final position in the flow and
       * stretches it to its final size.  Stretch also takes care of
       * any vertical alignment adjustments that need to be made. */
      status =
        NW_LMgr_StaticTableCellBox_Stretch((NW_LMgr_StaticTableCellBox_t*)cellBox, 
                                           &newRect);
      _NW_THROW_ON_ERROR(status);

      /* Prepare the tableContext for the next cell */
      currentX = (NW_GDI_Metric_t)(currentX + newRect.dimension.width);
      currentCol = (NW_ADT_Vector_Metric_t)(currentCol + colSpan);
    }

    /* Finally, place the row */
    NW_GDI_Rectangle_t rowBoxBounds = NW_LMgr_Box_GetFormatBounds( thisRow ); 
    rowBoxBounds.point.x = atX;
    rowBoxBounds.point.y = atY;
    NW_LMgr_Box_SetFormatBounds( thisRow, rowBoxBounds ); 
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_LMgr_StaticTableRowBox_t*
NW_LMgr_StaticTableRowBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_StaticTableRowBox_t*)
    NW_Object_New (&NW_LMgr_StaticTableRowBox_Class, numProperties);
}
