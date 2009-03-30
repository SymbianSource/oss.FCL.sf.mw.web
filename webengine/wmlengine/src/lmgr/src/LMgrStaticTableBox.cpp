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


#include "nw_lmgr_statictableboxi.h" 
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_rulebox.h"
#include "BrsrStatusCodes.h"

#define NW_LMGR_STATICTABLEBOX_MAX_CONSTRAINT (NW_GDI_Metric_t)20000
// min constraint should always be the minimum width of a box
//#define NW_LMGR_STATICTABLEBOX_MIN_CONSTRAINT (NW_GDI_Metric_t)NW_LMGR_BOX_MIN_WIDTH
/* ------------------------------------------------------------------------- *
 * Private Methods 
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* Collapse a cell and set position/size to 0 */
static
TBrowserStatusCode 
NW_LMgr_StaticTableBox_CollapseCell( NW_LMgr_StaticTableBox_t* thisObj, 
                                     NW_LMgr_StaticTableCellBox_t* cellBox )
  {
  NW_LMgr_BoxVisitor_t* visitor; 

  NW_REQUIRED_PARAM( thisObj );

  NW_TRY( status ) 
    {
    NW_LMgr_Box_t*      child;
    NW_GDI_Rectangle_t  zeroRect;

    NW_ASSERT( thisObj != NULL );
    NW_ASSERT( cellBox != NULL );
    
    // set zero rect for collapse
    zeroRect.point.x = 0;
    zeroRect.point.y = 0;
    zeroRect.dimension.width = 0;
    zeroRect.dimension.height = 0;

    // travell through and collapse all children.
    visitor = NW_LMgr_ContainerBox_GetBoxVisitor( cellBox );

    NW_THROW_OOM_ON_NULL( visitor, status );

    child = NW_LMgr_BoxVisitor_NextBox( visitor, 0 );

    while( ( child = NW_LMgr_BoxVisitor_NextBox( visitor, 0 ) ) != NULL ) 
      {
      if( NW_Object_IsDerivedFrom( child, &NW_LMgr_ContainerBox_Class) == NW_TRUE )
        {
        status = NW_LMgr_ContainerBox_Collapse( NW_LMgr_ContainerBoxOf( child ) );
        }
      else if( NW_Object_IsDerivedFrom( child, &NW_LMgr_TextBox_Class) == NW_TRUE )
        {
        status = NW_LMgr_TextBox_Collapse( NW_LMgr_TextBoxOf( child ) );
        }
      // check if collapse is fine
      NW_THROW_ON_ERROR( status );
      // and then finalize the collapse by setting position/size to 0
      NW_LMgr_Box_SetFormatBounds( child, zeroRect );
      }
    // successful completion 
    status = KBrsrSuccess;
    // set cell to zero
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( cellBox ), zeroRect );
    } 
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    NW_Object_Delete( visitor );
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
/* Collapse the table and set colums constraint to 0 */
static
TBrowserStatusCode 
NW_LMgr_StaticTableBox_CollapseTable( NW_LMgr_StaticTableBox_t* thisObj )
  {
  NW_TRY( status ) 
    {
    NW_ADT_Vector_Metric_t          colIndex;
    NW_ADT_Vector_Metric_t          rowIndex;
    NW_LMgr_StaticTableContext_t*   tableContext;
    NW_LMgr_Box_t*                  cellBox;    

    NW_ASSERT( thisObj != NULL );

    tableContext = thisObj->ctx;
    NW_THROW_OOM_ON_NULL( tableContext, status );

    // step through all the cells and collapse them
    for( colIndex = 0; colIndex < tableContext->numCols; colIndex++ )
      {
      for( rowIndex = 0; rowIndex < tableContext->numRows; rowIndex++ )
        {        
        status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy( tableContext, colIndex, rowIndex, &cellBox );
        NW_THROW_ON_ERROR( status );
        // cellbox can be NULL even if status == KBrsrSuccess
        if( cellBox != NULL )
          {
          // collpase cell
          status = NW_LMgr_StaticTableBox_CollapseCell( thisObj, NW_LMgr_StaticTableCellBoxOf( cellBox ) );
          NW_THROW_ON_ERROR( status );
          }
        }
      // table collapse requires to set column constraint to 0
      NW_LMgr_StaticTableContext_SetColConstraint( tableContext, colIndex, 0 );
      }
    // set table height info to 0
    thisObj->ctx->y = 0;
    thisObj->ctx->contentHeight = 0;
    // all the cells are collapsed
    status = KBrsrSuccess;
    }
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
/* Set the current width of a container by getting its children size         */
/* please note that this function needs revise when arabic-hebrew comes to   */
/* the picture.                                                              */
static
TBrowserStatusCode
NW_LMgr_StaticTableBox_CalculateContainerWidth( NW_LMgr_ContainerBox_t* containerBox )
  {
  NW_TRY( status ) 
    {
    NW_GDI_Metric_t        boxWidth = NW_LMGR_BOX_MIN_WIDTH;
    NW_GDI_Metric_t        lineWidth = 0;
    NW_GDI_Metric_t        nextBoxX = 0;
    NW_LMgr_Box_t*         childBox;
    NW_GDI_Rectangle_t     boxBounds;
    NW_ADT_Vector_Metric_t childIndex;
    NW_ADT_Vector_Metric_t childCount;
    NW_LMgr_FrameInfo_t    frameInfo;
    NW_LMgr_FrameInfo_t    tempframeInfo;    

    NW_ASSERT( containerBox != NULL );
    
    childCount = NW_LMgr_ContainerBox_GetChildCount( containerBox );

    // travell through all boxes in this container and set the longest line as the box's width
    for( childIndex = 0; childIndex < childCount; childIndex++ ) 
      { 
      childBox = NW_LMgr_ContainerBox_GetChild( NW_LMgr_ContainerBoxOf( containerBox ), childIndex );
    
      NW_THROW_OOM_ON_NULL( childBox, status );
       
      // if this container includes an other container, then call that one first to find out the width.
      // However if the container is a table box (nested tables) then no need to call its children as 
      // at this point all nested tables are all set.
      if( NW_Object_IsDerivedFrom( childBox, &NW_LMgr_ContainerBox_Class ) == NW_TRUE &&
          NW_Object_IsClass( childBox, &NW_LMgr_StaticTableBox_Class ) == NW_FALSE )
        {
        status = NW_LMgr_StaticTableBox_CalculateContainerWidth( NW_LMgr_ContainerBoxOf( childBox ) );

        NW_THROW_ON_ERROR( status );
        }
      boxBounds = NW_LMgr_Box_GetFormatBounds( childBox );
      // fix rulebox and boxes far too small
      if( NW_Object_IsDerivedFrom( childBox, &NW_LMgr_RuleBox_Class ) == NW_TRUE || 
         boxBounds.dimension.width < NW_LMGR_BOX_MIN_WIDTH )
        {
        boxBounds.dimension.width = NW_LMGR_BOX_MIN_WIDTH;
        NW_LMgr_Box_SetFormatBounds( childBox, boxBounds );
        }
      // check if this box is inline with those previous boxes
      if( boxBounds.dimension.width > 0 )
        {
        // nextBoxX indicates where the next box is supposed to be if it was in same line
        // note that this is not right-to-left proof!
        if( boxBounds.point.x < nextBoxX )
          {
          // this box is in the next line
          lineWidth = 0;
          }
        // calculate where the next box should be if it was in line
        nextBoxX = boxBounds.point.x + boxBounds.dimension.width;
        // add up the line width
        lineWidth+= boxBounds.dimension.width;
        // get the widest line
        if( lineWidth > boxWidth )
          {
          boxWidth = lineWidth;
          }
        }
      }
    NW_GDI_Rectangle_t containerBoxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( containerBox ) );
    // set width 
    containerBoxBounds.dimension.width = boxWidth;
    // adjust width with borders and margins
    NW_LMgr_Box_GetBorderWidth( NW_LMgr_BoxOf( containerBox ), &frameInfo, ELMgrFrameLeft|ELMgrFrameRight );
    containerBoxBounds.dimension.width+=( frameInfo.left + frameInfo.right );

    NW_LMgr_Box_GetMargins( NW_LMgr_BoxOf( containerBox ), &frameInfo, &tempframeInfo, 
      ELMgrFrameLeft|ELMgrFrameRight );
    containerBoxBounds.dimension.width+=( frameInfo.left + frameInfo.right );

    NW_LMgr_Box_GetPadding( NW_LMgr_BoxOf( containerBox ), &frameInfo, ELMgrFrameLeft|ELMgrFrameRight );
    containerBoxBounds.dimension.width+=( frameInfo.left + frameInfo.right );
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( containerBox ), containerBoxBounds );

    status = KBrsrSuccess;
    }
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }


/* ------------------------------------------------------------------------- */
/* Set the current width of a container by getting its children size         */
/* please note that this function needs revise when arabic-hebrew comes to   */
/* the picture.                                                              */
static
TBrowserStatusCode
NW_LMgr_StaticTableBox_CalculateContainerMinWidth( NW_LMgr_ContainerBox_t* containerBox, 
                                                   NW_GDI_Metric_t* containerWidth )
  {
  NW_TRY( status ) 
    {
    NW_GDI_Metric_t        minWidth = 0;
    NW_ADT_Vector_Metric_t childIndex;
    NW_ADT_Vector_Metric_t childCount;
    NW_LMgr_Box_t*         childBox;
    NW_GDI_Dimension2D_t   boxMinSize;
    NW_LMgr_FrameInfo_t    frameInfo;
    NW_LMgr_FrameInfo_t    tempframeInfo;    

    NW_ASSERT( containerBox != NULL );
    
    childCount = NW_LMgr_ContainerBox_GetChildCount( containerBox );
    *containerWidth = NW_LMGR_BOX_MIN_WIDTH;

    // travell through all boxes in this container and set the longest line as the box's width
    for( childIndex = 0; childIndex < childCount; childIndex++ ) 
      { 
      childBox = NW_LMgr_ContainerBox_GetChild( NW_LMgr_ContainerBoxOf( containerBox ), childIndex );    
      NW_THROW_OOM_ON_NULL( childBox, status );
       
      // if this container includes an other container, then call that one first to find out the width.
      // However if the container is a table box (nested tables) then no need to call its children as 
      // at this point all nested tables are all set.
      if( NW_Object_IsDerivedFrom( childBox, &NW_LMgr_ContainerBox_Class ) == NW_TRUE )
        {
        if( NW_Object_IsClass( childBox, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
          {
          NW_LMgr_StaticTableBox_t* tableBox = NW_LMgr_StaticTableBoxOf( childBox );

          if( tableBox->ctx == NULL )
            {
            // context should always be initialized.
            status = KBrsrUnexpectedError;
            }
          else
            {
            minWidth = tableBox->ctx->tableMinWidth;          
            }
          }
        else
          {
          status = NW_LMgr_StaticTableBox_CalculateContainerMinWidth( NW_LMgr_ContainerBoxOf( childBox ), &minWidth );
          }
        NW_THROW_ON_ERROR( status );
        }
      else if( 
               NW_Object_IsDerivedFrom( childBox, &NW_LMgr_RuleBox_Class ) == NW_TRUE )
        {
        minWidth = NW_LMGR_BOX_MIN_WIDTH;
        }
      else
        {
		
        (void) NW_LMgr_Box_GetMinimumContentSize( childBox, &boxMinSize );

		if(boxMinSize.width == NW_LMGR_BOX_MIN_WIDTH && (NW_Object_IsInstanceOf (childBox, &NW_LMgr_AbstractTextBox_Class))){
			NW_LMgr_AbstractTextBox_GetMinimumSizeByWord(NW_LMgr_AbstractTextBoxOf(childBox), &boxMinSize );
		}
		
		minWidth = boxMinSize.width;
        }
      if( minWidth > *containerWidth )
        {
        *containerWidth = minWidth;
        }
      }
    // adjust minimum width with borders and margins
    NW_LMgr_Box_GetBorderWidth( NW_LMgr_BoxOf( containerBox ), &frameInfo, ELMgrFrameLeft|ELMgrFrameRight );
    *containerWidth+=( frameInfo.left + frameInfo.right );

    NW_LMgr_Box_GetMargins( NW_LMgr_BoxOf( containerBox ), &frameInfo, &tempframeInfo, 
      ELMgrFrameLeft|ELMgrFrameRight );
    *containerWidth+=( frameInfo.left + frameInfo.right );

    NW_LMgr_Box_GetPadding( NW_LMgr_BoxOf( containerBox ), &frameInfo, ELMgrFrameLeft|ELMgrFrameRight );
    *containerWidth+=( frameInfo.left + frameInfo.right );

    status = KBrsrSuccess;
    }
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }






/* ---------------------------------------------------------------------------- */
/* Set the current width of a cell by getting its children size, plus it checks 
 * additional propeties such as cell padding and borders                        
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableBox_CalculateCellWidth( NW_LMgr_StaticTableBox_t* thisObj, NW_LMgr_ContainerBox_t* cellBox, 
                                           NW_GDI_Metric_t* cellWidth, NW_Bool minSize )
  {
  NW_TRY( status ) 
    {
    NW_LMgr_FrameInfo_t            padding;
    NW_LMgr_FrameInfo_t            borderWidth; 
    NW_LMgr_Property_t             prop;
    NW_LMgr_StaticTableContext_t*  tableContext;
  
    NW_ASSERT( cellBox != NULL );
    NW_ASSERT( thisObj != NULL );

    tableContext = thisObj->ctx;
    NW_THROW_OOM_ON_NULL( tableContext, status );

    // get the cell's width
    if( minSize == NW_TRUE )
      {
      status = NW_LMgr_StaticTableBox_CalculateContainerMinWidth( cellBox, cellWidth );
      }
    else
      { 
      status = NW_LMgr_StaticTableBox_CalculateContainerWidth( cellBox );
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( cellBox ) );
      *cellWidth = boxBounds.dimension.width; 
      }
    // check return value
    NW_THROW_ON_ERROR( status );
    // add padding
    NW_LMgr_StaticTableCellBox_GetPadding( NW_LMgr_StaticTableCellBoxOf( cellBox ), &padding );
    // and borders
    status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr( tableContext, NW_LMgr_BoxOf( cellBox ), &borderWidth, NULL, NULL ); 
    NW_THROW_ON_ERROR(status);
    // what about cellSpacing?
    *cellWidth = *cellWidth + (NW_GDI_Metric_t)
    ( borderWidth.left + borderWidth.right + padding.left + padding.right );
        
        if( NW_LMgr_Box_GetProperty( NW_LMgr_BoxOf( cellBox ), NW_CSS_Prop_width, &prop ) == KBrsrSuccess )
      {
      NW_LMgr_Property_t  origProp;
      // do not take percentage type into account (as we do not know about the 100% size ) 
      // unless the table width is set
      // get the original property as NW_LMgr_Box_GetProperty calls resolve on the property
      status = NW_LMgr_Box_GetRawProperty( NW_LMgr_BoxOf( cellBox ), NW_CSS_Prop_width, &origProp );
      if( status != KBrsrSuccess )
        {
        // if getting raw property fails then we should use the 
        // result of the getPropery instead. 
        origProp.type = prop.type;
        }

      // adjust pixel cell width
      if( origProp.type != NW_CSS_ValueType_Percentage )
        {
        if( (NW_GDI_Metric_t)prop.value.integer > *cellWidth )
          {
          *cellWidth = (NW_GDI_Metric_t)prop.value.integer;
          }
        }
      // adjust percentage cell width
      else 
        {
        NW_LMgr_Property_t  tableProp;

        // get the table width to resolve the cell's percentage value
        if( NW_LMgr_Box_GetRawProperty( NW_LMgr_BoxOf( thisObj ), NW_CSS_Prop_width, &tableProp ) == KBrsrSuccess )
          {
          // works only if we've got the pixel value
          if( tableProp.type == NW_CSS_ValueType_Px )
            {            
            // calculate new cellWidth
            NW_GDI_Metric_t newCellWidth = (NW_GDI_Metric_t) ( 
              (NW_Float32)tableProp.value.integer / 100 * (NW_Float32)origProp.value.decimal );

            if( newCellWidth > *cellWidth )
              {
              *cellWidth = newCellWidth;
              }
            }
          }
        }
      }

    // for some formatting reason, container width cannot be less than NW_LMGR_BOX_MIN_WIDTH
    if( *cellWidth < NW_LMGR_BOX_MIN_WIDTH )
      {
      *cellWidth = NW_LMGR_BOX_MIN_WIDTH;
      }
    }
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }


/* ---------------------------------------------------------------------------- */
/* Set the current size of a table by getting all cells' size                  */
static
TBrowserStatusCode
NW_LMgr_StaticTableBox_CalculateTableMinMaxWidth( NW_LMgr_StaticTableBox_t* thisObj )
  {
  NW_TRY( status )
    {
    NW_LMgr_StaticTableContext_t*   tableContext;
    NW_ADT_Vector_Metric_t          rowIndex;
    NW_ADT_Vector_Metric_t          colIndex;
    NW_LMgr_Box_t*                  rowBox;
    NW_LMgr_Box_t*                  cellBox;
    NW_GDI_Metric_t                 tableMaxWidth = 0;
    NW_GDI_Metric_t                 tableMinWidth = 0;
    NW_GDI_Metric_t                 maxMaxColWidth;
    NW_GDI_Metric_t                 maxMinColWidth;
    NW_Bool                         sameCell;
    NW_LMgr_Property_t              prop;
    NW_LMgr_PropertyValue_t         visibilityVal;
    NW_LMgr_FrameInfo_t             borderWidth;

    NW_ASSERT( thisObj != NULL );

    tableContext = thisObj->ctx;

    NW_THROW_OOM_ON_NULL( tableContext, status );
    
    for( colIndex = 0; colIndex < tableContext->numCols; colIndex++ ) 
      {
      maxMaxColWidth = 0;
      maxMinColWidth = 0;
      // get rows
      for( rowIndex = 0; rowIndex < tableContext->numRows; rowIndex++ ) 
        { 
        rowBox = NW_LMgr_ContainerBox_GetChild( NW_LMgr_ContainerBoxOf( thisObj ), rowIndex );
        NW_THROW_OOM_ON_NULL( rowBox, status );

        status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy( tableContext, colIndex, rowIndex, &cellBox );
        NW_THROW_ON_ERROR( status );

        // if the box is not cellbox skip it
        if(!NW_Object_IsInstanceOf (cellBox, &NW_LMgr_StaticTableCellBox_Class))
          continue;

        // check if this row is visible
        visibilityVal.token = NW_CSS_PropValue_visible;

        (void)NW_LMgr_Box_GetPropertyValue ( rowBox, NW_CSS_Prop_visibility, NW_CSS_ValueType_Token, &visibilityVal );

        if( visibilityVal.token == NW_CSS_PropValue_visible && cellBox != NULL )
          {
          // check colspan too. as we really dont wanna add colspan cells as many times as many colspans we do have here
          sameCell = NW_FALSE;
          prop.value.integer = 1;
          NW_LMgr_Box_GetPropertyFromList( cellBox, NW_CSS_Prop_colSpan, &prop );
          if( (NW_ADT_Vector_Metric_t)prop.value.integer > 1 )
            {
            NW_ADT_Vector_Metric_t    currentCol;
            NW_ADT_Vector_Metric_t    currentRow;

            status = NW_LMgr_StaticTableContext_PxPyToLxLy( tableContext, colIndex, rowIndex, &currentCol, &currentRow );
            NW_THROW_ON_ERROR( status );
            // cell position returns with the real position of the cell and that could be different to the row/col index when
            // colspan is present.
            // for examp: 
            // <tr><td colspan 2>row 1</td></tr>
            // <tr><td>row2 col1</td><td>row2 col2</td></tr>
            // for both columns in the first row, tableContext->cellBoxes[] returns with the very same cell box
            sameCell = (NW_Bool)( ( currentCol != colIndex || currentRow != rowIndex ) ? NW_TRUE : NW_FALSE );
            }
          if( sameCell != NW_TRUE )
            {
            NW_GDI_Metric_t cellMinWidth;
            NW_GDI_Metric_t cellMaxWidth;

            status = NW_LMgr_StaticTableBox_CalculateCellWidth( thisObj, NW_LMgr_ContainerBoxOf( cellBox ), &cellMinWidth, NW_TRUE );
            _NW_THROW_ON_ERROR( status );

            status = NW_LMgr_StaticTableBox_CalculateCellWidth( thisObj, NW_LMgr_ContainerBoxOf( cellBox ), &cellMaxWidth, NW_FALSE );            
            NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( cellBox ) );
            boxBounds.dimension.width = cellMaxWidth;
            NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( cellBox ), boxBounds );
            _NW_THROW_ON_ERROR( status );
          
            // set min/max cell width
            NW_LMgr_StaticTableCellBox_SetMaxWidth( NW_LMgr_StaticTableCellBoxOf( cellBox ), cellMaxWidth );
            NW_LMgr_StaticTableCellBox_SetMinWidth( NW_LMgr_StaticTableCellBoxOf( cellBox ), cellMinWidth);

            // find the widest cell in this column in order to set table width
            if( cellMaxWidth > maxMaxColWidth )
              {
              maxMaxColWidth = cellMaxWidth;
              }
            if( cellMinWidth > maxMinColWidth )
              {
              maxMinColWidth = cellMinWidth;
              }
            }
          }
        }
      tableMaxWidth+=maxMaxColWidth;
      tableMinWidth+=maxMinColWidth;
      }   
    // and finally add border 
    NW_LMgr_Box_GetBorderWidth( NW_LMgr_BoxOf( thisObj ), &borderWidth, ELMgrFrameLeft|ELMgrFrameRight );

    // adjust table width
    tableMaxWidth+=(borderWidth.left + borderWidth.right );
    tableMinWidth+=(borderWidth.left + borderWidth.right );

    // If the 'table' or 'inline-table' element's 'width' property has a specified value and it is bigger
    // than the compute value, then take width instead of the compute value
    if( NW_LMgr_BoxOf( thisObj )->propList != NULL &&
        NW_LMgr_PropertyList_Get( ( ( NW_LMgr_Box_t * ) thisObj )->propList, NW_CSS_Prop_width, &prop ) == KBrsrSuccess )
      {
      // do not take percentage type into account
      if( prop.type != NW_CSS_ValueType_Percentage )
        {
        if( (NW_GDI_Metric_t)prop.value.integer > tableMaxWidth )
          {
          tableMaxWidth = (NW_GDI_Metric_t)prop.value.integer;
          }
        if( (NW_GDI_Metric_t)prop.value.integer > tableMinWidth )
          {
          tableMinWidth = (NW_GDI_Metric_t)prop.value.integer;
          }
        }
      }
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( tableContext->tableBox ) );

    boxBounds.dimension.width = tableMaxWidth;
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( tableContext->tableBox ), boxBounds );

    tableContext->tableMaxWidth = tableMaxWidth;
    tableContext->tableMinWidth = tableMinWidth;
    } 
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------ */
/* set columns contraint */
static
void
NW_LMgr_StaticTableBox_SetColumnConstraint( NW_LMgr_StaticTableBox_t* thisObj, NW_GDI_Metric_t constraint )
  {
  //lint -esym(550, status) Warning -- Symbol 'status' (line 554) not accessed
  NW_TRY( status )
    {
    NW_ADT_Vector_Metric_t        colIndex;
    NW_LMgr_StaticTableContext_t* tableContext;

    NW_ASSERT( thisObj != NULL );

    tableContext = thisObj->ctx;

    NW_THROW_OOM_ON_NULL( tableContext, status );

    for( colIndex = 0; colIndex < tableContext->numCols; colIndex++ ) 
      {
      tableContext->colConstraints[ colIndex ] = constraint; 
      }
    status = KBrsrSuccess;
    }
  NW_CATCH( status ) 
    {
    }
  NW_FINALLY 
    {
    } 
  NW_END_TRY;
  }

/* ------------------------------------------------------------------------ */
/* constrain table */
static
void
NW_LMgr_StaticTableBox_ConstrainTable( NW_LMgr_StaticTableBox_t* thisObj )
  {
  NW_ASSERT( thisObj != NULL );

  if( thisObj->automaticWidthPass == NW_LMgr_StaticTableBox_AutomaticWidth_MaximumPass )
    {
    NW_LMgr_StaticTableBox_SetColumnConstraint( thisObj, NW_LMGR_STATICTABLEBOX_MAX_CONSTRAINT );
    }
  // minimum pass is eliminated by some performance improvement
  else
    {
    // final/undefined(fixed) pass does not require the constraints to be set as they are being calculated in this pass
    } 
  }

/* ------------------------------------------------------------------------- *
 * Public methods to set automatic table calculation passes.
 * 1. maximum pass: calculate the maximum width
 * 2. final pass  : calculate the final column width based on min and max
 *    undefined pass: error situation when automatic calculation failed.
 */
void
NW_LMgr_StaticTableBox_SetAutomaticTableWidthPass( NW_LMgr_StaticTableBox_t* thisObj, 
                                                   NW_LMgr_StaticTableBox_AutomaticWidth_PassTypes_t passType )
  {
  NW_ASSERT( thisObj != NULL );

  thisObj->automaticWidthPass = passType;
  }

/* ------------------------------------------------------------------------- *
NW_LMgr_StaticTableBox_TableSizeChanged( NW_LMgr_StaticTableBox_t* thisObj )
*/
void
NW_LMgr_StaticTableBox_TableSizeChanged( NW_LMgr_StaticTableBox_t* thisObj )
  {
  NW_ASSERT( thisObj != NULL );

  thisObj->needsRelayout = NW_TRUE;
  }

/* ------------------------------------------------------------------------- *
 * Whenever and image comes in which involves relayout flow, we need to initiate 
 * the relayout flow right from the topmost table.
 * This function takes the "dirty" table and figures out the topmost table where
 * the layout flow has to be initiated. 
 * Most of the time it does not involve the entire tree to be laid out so
 * it saves time for us.
 */
void
NW_LMgr_StaticTableBox_UpdatesTableMinMaxWidth( NW_LMgr_StaticTableBox_t* thisObj )
  {
  NW_LMgr_Box_t*  box;
  NW_LMgr_Box_t*  topMostTable;

  NW_ASSERT( thisObj != NULL );

  // get topmost parent table
  topMostTable = NW_LMgr_BoxOf( thisObj );
  box = topMostTable;
  while( box->parent != NULL )
    {
    if( NW_Object_IsClass( box->parent, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
      {
      topMostTable = NW_LMgr_BoxOf( box->parent );
      }
    box = NW_LMgr_BoxOf( box->parent );
    }

  // initiate relayout event.
  // format context is NULL 
  (void)NW_LMgr_Box_Resize( topMostTable , NULL);   
  }

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_ResizeTable
 * Parameters:    thisObj - the table box
 * Description:   This method calls sets the width and height for the table.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
static
TBrowserStatusCode
NW_LMgr_StaticTableBox_ResizeTable (NW_LMgr_StaticTableBox_t* thisObj)
{
  NW_LMgr_StaticTableContext_t *tableContext = thisObj->ctx;
  NW_LMgr_Box_t* tableBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_ContainerBox_t *tableContainer = NW_LMgr_ContainerBoxOf(tableBox);
  NW_LMgr_Box_t* rowBox;
  NW_ADT_Vector_Metric_t rowIndex;
  NW_GDI_Metric_t propHeight;
  NW_LMgr_Property_t prop;

  NW_REQUIRED_PARAM(thisObj);

  prop.value.token = NW_CSS_PropValue_auto;
  prop.type = NW_CSS_ValueType_Token;
  NW_LMgr_Box_GetProperty(tableBox, NW_CSS_Prop_height, &prop);

  /* In case the height property was explicitly set and is greater than the
   * calculated content height, we will stretch the rows to fit the given
   * height */
  NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds( tableBox );
  if (prop.type == NW_CSS_ValueType_Px) 
    {
    propHeight = (NW_GDI_Metric_t)prop.value.integer;
    if (propHeight > tableContext->contentHeight) 
      {
      for (rowIndex = 0; rowIndex < tableContext->numRows; rowIndex++) 
        {
        rowBox = NW_LMgr_ContainerBox_GetChild(tableContainer, rowIndex);
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( rowBox );

        if( tableContext->contentHeight ) 
          {
          boxBounds.dimension.height = (NW_GDI_Metric_t)
            (boxBounds.dimension.height * propHeight / tableContext->contentHeight);
          }
        else 
          { // to prevent divided by zero
          // rows height calulates desired height/ number of rows (divided equaly) as the actual 
          // content height is unknown.
          boxBounds.dimension.height = (NW_GDI_Metric_t)( propHeight / tableContext->numRows );
          } 
        //
        NW_LMgr_Box_SetFormatBounds( rowBox, boxBounds );
        }
      tableBoxBounds.dimension.height = propHeight;
      }
    else 
      {
      /* Otherwise simply use the minimum height */
      tableBoxBounds.dimension.height = (NW_GDI_Metric_t)tableContext->contentHeight;
      }
    }

  else 
    {
    /* Otherwise simply use the minimum height */
    tableBoxBounds.dimension.height = (NW_GDI_Metric_t)tableContext->contentHeight;
  }
  NW_LMgr_Box_SetFormatBounds( tableBox, tableBoxBounds );

  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_PlaceTable
 * Parameters:    thisObj - table object
 * Description:   This method places the table on the canvas.  The y position
 *                has already been determined by the enclosing flow;  here
 *                we deal with the horizontal alignment of the table within
 *                that block.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
static
TBrowserStatusCode
NW_LMgr_StaticTableBox_PlaceTable (NW_LMgr_StaticTableBox_t* thisObj)
{
  NW_LMgr_StaticTableContext_t *tableContext = thisObj->ctx;
  NW_LMgr_Box_t *tableBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_PropertyValue_t alignVal, floatVal, dirVal;
  NW_LMgr_PropertyValueToken_t tableAlign;
  NW_GDI_Metric_t flowWidth;
  NW_GDI_Metric_t tableWidth;
  NW_GDI_Metric_t flowX;
  NW_LMgr_Box_t* parentBox = NULL;
  NW_LMgr_RootBox_t* rootBox;
  NW_Int32 rightLimit;

  NW_TRY(status) {
    /* The table-align property should be handled separately */
    alignVal.token = NW_CSS_PropValue_left;
    status = NW_LMgr_Box_GetPropertyValue(tableBox, NW_CSS_Prop_tableAlign, 
                                          NW_CSS_ValueType_Token, &alignVal);
    NW_THROW_ON(status, KBrsrOutOfMemory);

	//If not able to find the table-align property from the table box, get its text align property which
	//will be gotten from parent or grandparents (such as <p> tag) if itself does not have.
    if (status == KBrsrNotFound) 
	{
		status = NW_LMgr_Box_GetPropertyValue(tableBox, NW_CSS_Prop_textAlign, 
                                          NW_CSS_ValueType_Token, &alignVal);
	}
    NW_THROW_ON(status, KBrsrOutOfMemory);

    /* If there is no align attribute, look for dir attribute */
    if (status == KBrsrNotFound) {
      dirVal.token = NW_CSS_PropValue_ltr;
      NW_LMgr_Box_GetPropertyValue(tableBox, NW_CSS_Prop_textDirection, 
                                  NW_CSS_ValueType_Token, &dirVal);
      if (dirVal.token == NW_CSS_PropValue_rtl) {
        alignVal.token = NW_CSS_PropValue_right;
      }
    }

    tableAlign = alignVal.token;

    /* Get the anonymous table container */
    parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(tableBox); 

    /* The float property should be applied to the container */
    floatVal.token = NW_CSS_PropValue_none;
    status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_float, 
                                          NW_CSS_ValueType_Token, &floatVal);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    /* We only align the table if it is not floating */
    NW_GDI_Rectangle_t parentBoxBounds = NW_LMgr_Box_GetFormatBounds( parentBox );
    NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds( tableBox );
    if (floatVal.token == NW_CSS_PropValue_none) 
      {
      flowX = parentBoxBounds.point.x;
      flowWidth = parentBoxBounds.dimension.width;
      tableWidth = tableBoxBounds.dimension.width;

      switch (tableAlign) 
        {
        case NW_CSS_PropValue_center:
          tableContext->leftMargin = (NW_GDI_Metric_t)(flowX + (flowWidth - tableWidth)/2);
          break;
        case NW_CSS_PropValue_right:
          tableContext->leftMargin = (NW_GDI_Metric_t)(flowX + flowWidth - tableWidth);
          break;
        default:
          // tableBoxBounds.point.x has taken left margin into account
          tableContext->leftMargin = tableBoxBounds.point.x;
          break;
        }
      tableBoxBounds.point.x = tableContext->leftMargin;
      NW_LMgr_Box_SetFormatBounds( tableBox, tableBoxBounds );
      }
    else 
      {
      tableContext->leftMargin = tableBoxBounds.point.x;
      }
    // update
    tableBoxBounds = NW_LMgr_Box_GetFormatBounds( tableBox );

    if (tableContext->leftMargin < 0) 
      {
      tableBoxBounds.point.x = tableContext->leftMargin = parentBoxBounds.point.x;
      NW_LMgr_Box_SetFormatBounds( tableBox, tableBoxBounds );
      }
    
    rootBox = NW_LMgr_Box_GetRootBox( tableBox );

    // update
    tableBoxBounds = NW_LMgr_Box_GetFormatBounds( tableBox );

    if( rootBox )
      {
      rightLimit = NW_LMgr_RootBox_GetRightLimit( rootBox );
      if (tableContext->leftMargin + tableBoxBounds.dimension.width > rightLimit ) 
        {
          NW_Int32 margin = tableContext->leftMargin - tableBoxBounds.point.x;
          tableContext->leftMargin = rightLimit - tableBoxBounds.dimension.width;
          tableBoxBounds.point.x = tableContext->leftMargin - margin ;
          NW_LMgr_Box_SetFormatBounds( tableBox, tableBoxBounds );
        }
      }
    /* Finally place the table on the y axis */
    // update
    tableBoxBounds = NW_LMgr_Box_GetFormatBounds( tableBox );
    tableBoxBounds.point.y = tableContext->y;
    NW_LMgr_Box_SetFormatBounds( tableBox, tableBoxBounds );
    }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY;
}

/* ------------------------------------------------------------------------- */

const
NW_LMgr_StaticTableBox_Class_t  NW_LMgr_StaticTableBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_FormatBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_StaticTableBox_t),
    /* construct                 */ _NW_LMgr_StaticTableBox_Construct,
    /* destruct                  */ _NW_LMgr_StaticTableBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_StaticTableBox_Resize,
                                    _NW_LMgr_StaticTableBox_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_StaticTableBox_Constrain,
    /* draw                      */ _NW_LMgr_StaticTableBox_Draw,
    /* render                    */ _NW_LMgr_ContainerBox_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
   /* NW_LMgr_StaticTableBox */
  {
    /* unused                    */ NW_Object_Unused
  }
};

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_Construct
 * Parameters:    
 * Description:   This constructor.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  TBrowserStatusCode status;
  NW_LMgr_StaticTableBox_t *thisObj = NW_LMgr_StaticTableBoxOf(dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_LMgr_ContainerBox_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  thisObj->ctx = NULL;
  thisObj->automaticWidthPass = NW_LMgr_StaticTableBox_AutomaticWidth_UndefinedPass;
  thisObj->needsRelayout = NW_FALSE;      

  /* successful completion */
  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_Destruct
 * Parameters:    
 * Description:   This destructor.
 * Returns:       KBrsrSuccess
 */
void
_NW_LMgr_StaticTableBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_StaticTableBox_t *thisObj = NW_LMgr_StaticTableBoxOf(dynamicObject);

  NW_Object_Delete(thisObj->ctx);
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_Constrain
 * Parameters:    box - the table box
 * Description:   This method does the brunt of the work of table formatting.
 *                It initializes the table context, constrains the rows and
 *                cells, formats them and finally places them in the proper
 *                positions.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context)
{
  NW_LMgr_StaticTableBox_t* thisObj;
  NW_LMgr_StaticTableContext_t *tableContext = NULL;
 
  /* for convenience */
  thisObj = NW_LMgr_StaticTableBoxOf(box);

  NW_TRY (status) {

    if (thisObj->ctx == NULL)
    {
      /* Allocate the memory for the TableContext structure */
      thisObj->ctx = NW_LMgr_StaticTableContext_New();
      NW_THROW_OOM_ON_NULL(thisObj->ctx, status);
    }

    /* For convenience */
    tableContext = thisObj->ctx;

    /* Initialize the context */
    status = NW_LMgr_StaticTableContext_Initialize(tableContext, box);
    NW_THROW_ON_ERROR(status);

    /* Constrain the table */
    // this does not really do anything in case of final/undefined passes
    NW_LMgr_StaticTableBox_ConstrainTable(thisObj);

    // check if we are ready to calculate the final contraints
    if( thisObj->automaticWidthPass == NW_LMgr_StaticTableBox_AutomaticWidth_FinalPass ) 
      {
      status = NW_LMgr_StaticTableContext_FindConstraints( tableContext, NW_TRUE );

      // Reset automaticWidthPass back to it original value.  This is necessary in order to
      // to support switching screen modes on pages that contain grid mode tables as the
      // constraints need to be recalulated after a screen mode switch.  
      //
      // This is purposly done before the result is checked. 
      thisObj->automaticWidthPass = NW_LMgr_StaticTableBox_AutomaticWidth_UndefinedPass;

      _NW_THROW_ON_ERROR(status);
      } 
    else if ( thisObj->automaticWidthPass == NW_LMgr_StaticTableBox_AutomaticWidth_UndefinedPass ) 
      {
      /* Find the column constraints */
      status = NW_LMgr_StaticTableContext_FindConstraints( tableContext, NW_FALSE );
      _NW_THROW_ON_ERROR(status);
      }

    /* First resize the rows to determine the table height.  We start at the last row 
     * because that makes it easier to take into account cells with the rowSpan property set.
     */

    context->newFormatContext = context;
    context->formatBox = NW_LMgr_BoxOf(thisObj);
    context->formatChildren = NW_TRUE;
    context->referenceCount ++;

  } NW_CATCH (status) {
  } NW_FINALLY {
   // tableContext- cleaned up in postresize
    return status;
  } NW_END_TRY
}


TBrowserStatusCode
_NW_LMgr_StaticTableBox_PostResize(NW_LMgr_Box_t* box)
{
  NW_LMgr_StaticTableBox_t* thisObj;
  NW_LMgr_StaticTableContext_t *tableContext = NULL;
  NW_ADT_Vector_Metric_t rowIndex, rowCount;
  NW_LMgr_PropertyValue_t visibilityVal;
  NW_LMgr_Box_t *rowBox;
  NW_LMgr_ContainerBox_t *tableContainer;
 
  /* for convenience */
  thisObj = NW_LMgr_StaticTableBoxOf(box);
  tableContainer = NW_LMgr_ContainerBoxOf(box);

  NW_TRY (status) {

    tableContext = thisObj->ctx;

    NW_ASSERT(tableContext != NULL);

    for (rowIndex = 0; rowIndex < tableContext->numRows; rowIndex++) {

      rowBox = NW_LMgr_ContainerBox_GetChild(tableContainer, 
                                             (NW_ADT_Vector_Metric_t)
                                               (tableContext->numRows - 1 - rowIndex));
      NW_GDI_Rectangle_t rowBoxBounds = NW_LMgr_Box_GetFormatBounds( rowBox );

      /* Add the row height to the box height */
      tableContext->contentHeight = (NW_GDI_Metric_t)
        (tableContext->contentHeight + rowBoxBounds.dimension.height);
    }

    /* Resize the table box */
    status = NW_LMgr_StaticTableBox_ResizeTable(thisObj);
    _NW_THROW_ON_ERROR(status);

    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
    /* Set the cursor to the top of the STB */
    tableContext->y = boxBounds.point.y;
  
    /* Place the table */
    status = NW_LMgr_StaticTableBox_PlaceTable(thisObj);
    _NW_THROW_ON_ERROR(status);

    /* How many rows in the table */
    rowCount = NW_LMgr_ContainerBox_GetChildCount(tableContainer);

    /* Now place the rows and their contained cells */
    for (rowIndex = 0; rowIndex < rowCount; rowIndex++) {

      /* Get the row */
      rowBox = NW_LMgr_ContainerBox_GetChild(tableContainer, rowIndex);
      NW_GDI_Rectangle_t rowBoxBounds = NW_LMgr_Box_GetFormatBounds( rowBox );
  
      // JADE: hack; to-do: why row box is not staticrowbox? 
      if (!NW_Object_IsInstanceOf (rowBox, &NW_LMgr_StaticTableRowBox_Class))
	  {
	  continue;
	  }
      
      /* Check if visibility val for the row is collapse */
      visibilityVal.token = NW_CSS_PropValue_visible;
      (void) NW_LMgr_Box_GetPropertyValue (rowBox, NW_CSS_Prop_visibility,
                                           NW_CSS_ValueType_Token, &visibilityVal);
      if (visibilityVal.token == NW_CSS_PropValue_collapse) {
        continue;
      }

      /* Place the row */
      status =
        NW_LMgr_StaticTableRowBox_PlaceRow (NW_LMgr_StaticTableRowBoxOf(rowBox),
                                            tableContext->leftMargin,
                                            tableContext->y);
      _NW_THROW_ON_ERROR(status);

      /* Update the tableContext */
      tableContext->y = (NW_GDI_Metric_t)
        (tableContext->y + rowBoxBounds.dimension.height);
    }
    // once we are ready with resizing, 
    // 1. set relayoutNeeds flag to false 
    // 2. check if table width needs to be calculated
    thisObj->needsRelayout = NW_FALSE;
    
    if( thisObj->automaticWidthPass == NW_LMgr_StaticTableBox_AutomaticWidth_MaximumPass )
        // minimum pass is eliminated by some performance improvement
      {
      status = NW_LMgr_StaticTableBox_CalculateTableMinMaxWidth( thisObj );
      _NW_THROW_ON_ERROR(status);
      // collapse the table
      status = NW_LMgr_StaticTableBox_CollapseTable( thisObj );
      _NW_THROW_ON_ERROR(status);
      }

  } NW_CATCH (status) {
  } NW_FINALLY {
    if (tableContext) {
      (void)NW_LMgr_StaticTableContext_Cleanup(tableContext);
    }
    return status;
  } NW_END_TRY
}


/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_Constrain
 * Parameters:    box - the table box
 *                constrain - the constraint
 * Description:   This method constrains the width of the table.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Constrain(NW_LMgr_Box_t* box,
                                  NW_GDI_Metric_t constraint)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_GDI_Dimension2D_t minSize;
  NW_GDI_Dimension2D_t setSize;
  NW_LMgr_Box_t *thisTable = box;
  NW_LMgr_FrameInfo_t borderWidth;

  NW_ASSERT(box != NULL);

  /* Get minimum size for the box. */
  (void) NW_LMgr_Box_GetMinimumSize(thisTable, &minSize);
  NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameLeft|ELMgrFrameRight );
  minSize.width -= (borderWidth.left + borderWidth.right);

  /* See if any size properties are set, Currently we ignore return
   * status and use the values of setSize members to see if size
   * properties were set 
   */
  (void) NW_LMgr_Box_GetSizeProperties(thisTable, &setSize);
  
  /* If a constraint was supplied, apply it */
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  if(constraint > 0)
    {
    /* If no horizontal size property, set according to constraint */
    if(setSize.width <= 0)
      {
      boxBounds.dimension.width = constraint;
      }
    /* Else, set to minimum width.  Any size properties are already included
     * in this value. 
     */
    else 
      {
      boxBounds.dimension.width = minSize.width;
      if(constraint < minSize.width)
        {
        /* constraint too small */
        status = KBrsrLmgrConstraintTooSmall;
        }
      } 
    }

  /* If no constraint was provided, set to minimum width.  We cannot do
   * a layout without a constraint. */
  else 
    {
    boxBounds.dimension.width = minSize.width;
    }
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return status;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableBox_Draw
 * Parameters:    box - the table box
 *                deviceContext - the device context
 *                hasFocus - does the box have focus?
 * Description:   This method overrides the base class draw.  We leave it
 *                blank because we don't want the table to draw its own
 *                borders.  Table borders will be drawn by the cells that
 *                own them.
 * Returns:       KBrsrSuccess, KBrsrOutOfMemory
 */
TBrowserStatusCode
_NW_LMgr_StaticTableBox_Draw (NW_LMgr_Box_t* box,
                              CGDIDeviceContext* deviceContext,
                              NW_Uint8 hasFocus)
{
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(deviceContext);
  NW_REQUIRED_PARAM(hasFocus);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_StaticTableBox_t*
NW_LMgr_StaticTableBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_StaticTableBox_t*)
    NW_Object_New (&NW_LMgr_StaticTableBox_Class, numProperties);
}
