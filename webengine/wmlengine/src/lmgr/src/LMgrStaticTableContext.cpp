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


#include "nw_lmgr_statictablecontexti.h"
#include "nw_adt_vectoriterator.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_abstracttextbox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_fbox_formbox.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
   private constants
 * ------------------------------------------------------------------------- */
#define GRID_MODE_COL_CONSTRAINT_ROW_COUNT 5
#define GRID_MODE_IMAGE_TO_CELL_RATIO 0.65
#define GRID_MODE_IMAGE_ONLY_WIDTH_TO_NORMAL_WIDTH_RATIO 0.70

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* This function must be called only once and only from NW_LMgr_StaticTableContext_GetTableStats */
static
TBrowserStatusCode
NW_LMgr_SetCellsFlowDirection(NW_LMgr_ContainerBox_t *tableContainer, NW_LMgr_Box_t *rowBox)
{
  NW_Int32 j;
  NW_ADT_Vector_Metric_t cellCount;
  NW_LMgr_Box_t *cellBox = NULL;
  NW_LMgr_PropertyValue_t dirValue;
  TBrowserStatusCode status = KBrsrSuccess;
    /* Get the flow direction */
    dirValue.token = NW_CSS_PropValue_ltr;
    (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(tableContainer), 
                                       NW_CSS_Prop_textDirection,
                                       NW_CSS_ValueType_Token, 
                                       &dirValue);
    if (dirValue.token == NW_CSS_PropValue_rtl) {
      cellCount = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(rowBox));

      /* Go through all the cells in the row */
      for (j = cellCount - 1; j > 0; j--) {

        cellBox = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(rowBox), (NW_ADT_Vector_Metric_t)(cellCount - 1));

        status = NW_LMgr_ContainerBox_RemoveChild (NW_LMgr_ContainerBoxOf(rowBox), cellBox);
        if (status == KBrsrSuccess)
        {
          status = NW_LMgr_ContainerBox_InsertChildAt (NW_LMgr_ContainerBoxOf(rowBox), cellBox, (NW_ADT_Vector_Metric_t)(cellCount - j - 1));
          if (status != KBrsrSuccess)
          {
            NW_Object_Delete(cellBox);
          }
        }
      }
    }
  return status;
}


/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_FindSpan
 * Parameters:    thisObj - the object
 *                px - 
 *                py -
 *                spanx -
 *                spany - 
 *                spanColSpan -
 *                spanRowSpan -
 * Description:   
 * Returns:
 */

static
NW_Bool
NW_LMgr_StaticTableContext_FindSpan(NW_LMgr_StaticTableContext_t *thisObj, 
                                    NW_Uint16 px, NW_Uint16 py,
                                    NW_Uint16 *spanx, NW_Uint16 *spany,
                                    NW_Uint16 *spanColSpan, NW_Uint16 *spanRowSpan)
{
  NW_Uint32 *mapEntry;
  NW_Uint16 map_px;
  NW_Uint16 map_py;
  NW_Uint16 map_colSpan;
  NW_Uint16 map_rowSpan;
  NW_ADT_Vector_Metric_t vectorSize;
  NW_ADT_Vector_Metric_t index;

  /* Are the locations within range? */
  if (px >= thisObj->numCols || py >= thisObj->numRows) {
    return NW_FALSE;
  }

  if (thisObj->spans) {
    vectorSize = NW_ADT_Vector_GetSize(thisObj->spans);

    for (index = 0; index < vectorSize; index++) {

      mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->spans, index);

      map_py = NW_LMGR_STATICTABLESPANENTRY_GETPY(*mapEntry);

      /* If we have passed our vert position, we're done */
      if (map_py > py) {
        break;
      }

      map_px = NW_LMGR_STATICTABLESPANENTRY_GETPX(*mapEntry);
      map_rowSpan = NW_LMGR_STATICTABLESPANENTRY_GETROWSPAN(*mapEntry);
      map_colSpan = NW_LMGR_STATICTABLESPANENTRY_GETCOLSPAN(*mapEntry);

      /* Check whether (px,py) falls within this cell */
      if ((py >= map_py) && (py < map_py + map_rowSpan)) {
        if ((px >= map_px) && (px < map_px + map_colSpan)) {
          if (spanx) {
            *spanx = map_px;
          }
          if (spany) {
            *spany = map_py;
          }
          if (spanColSpan) {
            *spanColSpan = map_colSpan;
          }
          if (spanRowSpan) {
            *spanRowSpan = map_rowSpan;
          }
          return NW_TRUE;
        }
      }
    }
  }
  return NW_FALSE;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_AddSpanToMap
 * Parameters:    thisObj - the object
 *                px - 
 *                py -
 *                colSpan -
 *                rowSpan -
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_AddSpanToMap(NW_LMgr_StaticTableContext_t *thisObj, 
                                        NW_Uint16 px, NW_Uint16 py,
                                        NW_Uint16 colSpan,
                                        NW_Uint16 rowSpan)
{
  void** ptr;
  NW_Uint32 newEntry = 0;

  NW_TRY(status) {

    if (thisObj->spans == NULL) {
      thisObj->spans = 
        NW_ADT_ResizableVector_New (sizeof (NW_Uint32), 10, 5 );
      NW_THROW_OOM_ON_NULL(thisObj->spans, status);
    }

    NW_LMGR_STATICTABLESPANENTRY_INITIALIZE(newEntry, px, py, colSpan, rowSpan);

    ptr = NW_ADT_DynamicVector_InsertAt(thisObj->spans,
                                        &newEntry,
                                        NW_ADT_Vector_AtEnd);
    NW_THROW_OOM_ON_NULL(ptr, status);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_AddHEdgeToMap
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_AddHEdgeToMap(NW_LMgr_StaticTableContext_t *thisObj, 
                                         NW_Uint16 py, NW_Uint16 px1, NW_Uint16 px2,
                                         NW_Uint16 info)
{
  NW_Uint32 *mapEntry;
  NW_Uint32 newEntry = 0;
  NW_Uint16 map_py, map_px1, map_px2, map_info;
  void** ptr;
  NW_ADT_Vector_Metric_t vectorSize;

  NW_TRY(status) {

    if (thisObj->horizontalEdges == NULL) {
      thisObj->horizontalEdges = 
        NW_ADT_ResizableVector_New (sizeof (NW_Uint32), 10, 5 );
      NW_THROW_OOM_ON_NULL(thisObj->horizontalEdges, status);
    }

    vectorSize = NW_ADT_Vector_GetSize(thisObj->horizontalEdges);

    if (vectorSize > 0) {
      mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->horizontalEdges, (NW_ADT_Vector_Metric_t)(vectorSize - 1));
      NW_THROW_ON_NULL(mapEntry, status, KBrsrUnexpectedError);

      map_px1 = NW_LMGR_STATICTABLEHEDGEENTRY_GETPX1(*mapEntry);
      map_px2 = NW_LMGR_STATICTABLEHEDGEENTRY_GETPX2(*mapEntry);
      map_py = NW_LMGR_STATICTABLEHEDGEENTRY_GETPY(*mapEntry);
      map_info = NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(*mapEntry);

      if ((py == map_py) && (px1 >= map_px1) && (px1 <= map_px2) && (map_info == info)) {
        NW_LMGR_STATICTABLEHEDGEENTRY_SETPX2(*mapEntry, px2);
        NW_THROW_SUCCESS(status);
      }
    }

    /* Otherwise append the entry to the vector */
    NW_LMGR_STATICTABLEHEDGEENTRY_INITIALIZE(newEntry, py, px1, px2, info);
    ptr = NW_ADT_DynamicVector_InsertAt(thisObj->horizontalEdges,
                                        &newEntry,
                                        NW_ADT_Vector_AtEnd);
    NW_THROW_OOM_ON_NULL(ptr, status);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_AddVEdgeToMap
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_AddVEdgeToMap(NW_LMgr_StaticTableContext_t *thisObj, 
                                         NW_Uint16 px, NW_Uint16 py1, NW_Uint16 py2,
                                         NW_Uint16 info)
{
  NW_Uint32 *mapEntry;
  NW_Uint32 newEntry = 0;
  NW_Uint16 map_px;
  NW_Uint16 map_py2;
  NW_Uint16 map_info;
  void** ptr;
  NW_ADT_Vector_Metric_t vectorIndex;

  NW_TRY(status) {

    if (thisObj->verticalEdges == NULL) {
      thisObj->verticalEdges = 
        NW_ADT_ResizableVector_New (sizeof (NW_Uint32), 10, 5 );
      NW_THROW_OOM_ON_NULL(thisObj->verticalEdges, status);
    }

    vectorIndex = NW_ADT_Vector_GetSize(thisObj->verticalEdges);

    while (vectorIndex > 0) {

      vectorIndex--;

      mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->verticalEdges, vectorIndex);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      map_px = NW_LMGR_STATICTABLEVEDGEENTRY_GETPX(*mapEntry);
      map_py2 = NW_LMGR_STATICTABLEVEDGEENTRY_GETPY2(*mapEntry);
      map_info = NW_LMGR_STATICTABLEVEDGEENTRY_GETINFO(*mapEntry);

      if (map_py2 < py1) {
        break;
      }
      if ((map_py2 == py1) && (map_px == px) && (map_info == info)) {
        NW_LMGR_STATICTABLEVEDGEENTRY_SETPY2(*mapEntry, py2);
        NW_THROW_SUCCESS(status);
      }
    }

    /* Otherwise append the entry to the vector */
    NW_LMGR_STATICTABLEHEDGEENTRY_INITIALIZE(newEntry, px, py1, py2, info);
    ptr = NW_ADT_DynamicVector_InsertAt(thisObj->verticalEdges,
                                        &newEntry,
                                        NW_ADT_Vector_AtEnd);
    NW_THROW_OOM_ON_NULL(ptr, status);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_AddBorderInfo
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_AddBorderInfo(NW_LMgr_StaticTableContext_t* thisObj,
                                         NW_LMgr_StaticTableBorderEdge_t* edge,
                                         NW_Uint16 count)
{
  NW_LMgr_StaticTableBorderInfo_t *newBorderInfo;
  void** ptr;
  
  NW_TRY(status) {

    newBorderInfo = (NW_LMgr_StaticTableBorderInfo_t *) 
            NW_Mem_Malloc(sizeof(NW_LMgr_StaticTableBorderInfo_t));
    NW_THROW_OOM_ON_NULL(newBorderInfo, status);

    newBorderInfo->edge = *edge;
    newBorderInfo->count = count;

    ptr = NW_ADT_DynamicVector_InsertAt(thisObj->borderInfos,
                                           &newBorderInfo, NW_ADT_Vector_AtEnd);
    NW_THROW_OOM_ON_NULL(ptr, status);

    newBorderInfo = NULL;
  }
  NW_CATCH(status) {
    NW_Mem_Free(newBorderInfo);
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_GetTableStats
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetTableStats(NW_LMgr_StaticTableContext_t* thisObj)
{
  NW_ADT_Vector_Metric_t i, j, k;
  NW_ADT_Vector_Metric_t colSpan, rowSpan;
  NW_ADT_Vector_Metric_t rowIndex = 0;
  NW_ADT_Vector_Metric_t colIndex = 0;
  NW_ADT_Vector_Metric_t rowCount, cellCount;
  NW_LMgr_ContainerBox_t *tableContainer = NULL;
  NW_LMgr_Box_t *rowBox = NULL;
  NW_LMgr_ContainerBox_t *rowContainer = NULL;
  NW_LMgr_Property_t prop;
  NW_LMgr_Box_t *cellBox = NULL;
  NW_Int32*	growRowArray = NULL;

  NW_TRY (status) 
  {
    tableContainer = NW_LMgr_ContainerBoxOf(thisObj->tableBox);
    rowCount = NW_LMgr_ContainerBox_GetChildCount(tableContainer);

    // Create the array to hold which rows need to consider growing columns
    growRowArray = (NW_Int32*) NW_Mem_Malloc(sizeof(NW_Int32) * rowCount);
    NW_THROW_OOM_ON_NULL(growRowArray, status);

    NW_Mem_memset(growRowArray, 0x0, sizeof(NW_Int32) * rowCount);

    for (i = 0; i < rowCount; i++) 
    {
      rowBox = NW_LMgr_ContainerBox_GetChild (tableContainer, i);
      
      /* This function must be called only once and only from here */
      status = NW_LMgr_SetCellsFlowDirection(tableContainer, rowBox);
      NW_THROW_ON_ERROR(status);
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( rowBox ); 

      if (NW_Object_IsInstanceOf (rowBox, &NW_LMgr_AbstractTextBox_Class) &&
		  boxBounds.dimension.width == 0 &&
		  boxBounds.dimension.height == 0 )
	  {
		status = NW_LMgr_ContainerBox_RemoveChild (tableContainer, rowBox);
        NW_THROW_ON_ERROR(status);
    	rowCount = NW_LMgr_ContainerBox_GetChildCount(tableContainer);

		NW_Object_Delete(rowBox);
		continue;
	  }
	  
      rowContainer = NW_LMgr_ContainerBoxOf(rowBox);
      cellCount = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

      /* The number of columns in the table is determined by the longest row.
       * This is a slight deviation from CSS2 which allows us to 
       * render certain malformed pages.
       */      
      colIndex = 0;

      /* Go through all the cells in the row */
      for (j = 0; j < cellCount; j++) {

        cellBox = NW_LMgr_ContainerBox_GetChild (rowContainer, j);

        /* Get the colSpan and make sure it is legal */
        prop.value.integer = 1;
        (void)NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_colSpan, &prop);
        colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
        if (colSpan == 0) {
          colSpan = (NW_ADT_Vector_Metric_t)(thisObj->numCols - colIndex);  
        }

        /* Do the same for the rowSpan */
        prop.value.integer = 1;
        (void)NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_rowSpan, &prop);
        rowSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
        
        // if we have a rowspan that will span multiple rows
		if (rowSpan > 1) 
		{
			// check to see if we have enough rows to contain it, otherwise adjust the rowspan
			if ( i + rowSpan > rowCount)
			{
				rowSpan = rowCount - i;
			}
			
      		if (rowSpan <= rowCount )
      		{
      			// start at the row after ours
      			for (k = i + 1; k <= rowSpan; k++)
      			{
      				growRowArray[k] = growRowArray[k] + 1;
      			}
      		}
        }
        else if (rowSpan == 0) 
        {
        	rowSpan = (NW_ADT_Vector_Metric_t)(thisObj->numRows - rowIndex);
        }
        
        /* Update the cumulative colIndex */
        colIndex = (NW_ADT_Vector_Metric_t)(colIndex + colSpan);
      }

      /* Update the col count */
      if (rowIndex == 0) 
      {
        thisObj->numCols = colIndex;
      }
      else if (colIndex > thisObj->numCols) 
      {
        thisObj->numCols = (NW_ADT_Vector_Metric_t)(colIndex);
      }
      // do we need to add an extra column?
      else if (growRowArray[i] != 0)
      {
      	NW_Int16 diff = (colIndex + growRowArray[i]) - thisObj->numCols;
      	
      	if (diff > 0)
      	{
      		thisObj->numCols = thisObj->numCols + diff;
      	}
      }

      rowIndex++;
      
    }

    /* Here we set the rows field of TableContext.  The cols field has
     * already been set.
     */
    thisObj->numRows = rowIndex;

    /* If the table is empty, return failure */
    if ((thisObj->numRows == 0) || (thisObj->numCols == 0)) {
      NW_THROW_STATUS (status, KBrsrFailure);
    }
  } 
  NW_CATCH (status) 
  {
  } 
  NW_FINALLY 
  {
  	NW_Mem_Free(growRowArray);
    return status;
  } 
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_AddLocationInfoToMap
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_AddLocationInfoToMap(NW_LMgr_StaticTableContext_t* thisObj)
{
  NW_ADT_Vector_Metric_t i, j;
  NW_ADT_Vector_Metric_t cellCount;
  NW_Uint16 k;
  NW_Uint16 colSpan, rowSpan;
  NW_Uint16 rowIndex = 0;
  NW_Uint16 colIndex = 0;
  NW_LMgr_ContainerBox_t *tableContainer = NULL;
  NW_LMgr_ContainerBox_t *rowContainer = NULL;
  NW_LMgr_Box_t *rowBox = NULL;
  NW_LMgr_Box_t *cellBox = NULL;
  NW_LMgr_Property_t prop;
  NW_LMgr_PropertyValue_t dirValue;

  NW_TRY (status) {

    tableContainer = NW_LMgr_ContainerBoxOf(thisObj->tableBox);

    for (i = 0; i < thisObj->numRows; i++) {

      rowBox = NW_LMgr_ContainerBox_GetChild (tableContainer, i);
      rowContainer = NW_LMgr_ContainerBoxOf(rowBox);

      /* The following code is ran twice: 
       * 1) find if we need to add more cells in the end 
       * 2) make a map of the spans
       */
      
      /* How many cells in the row? */
      cellCount = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

      /* Start at column 0 */
      colIndex = 0;
      rowIndex = i;

      /* Go through all the cells in the row */
      for (j = 0; j < cellCount; j++) {

        /* Get the cell */
        cellBox = NW_LMgr_ContainerBox_GetChild (rowContainer, j);

        /* Skip any rowspans */
        while (colIndex < thisObj->numCols) {

          if (!NW_LMgr_StaticTableContext_FindSpan (thisObj, colIndex, rowIndex, NULL, NULL, &colSpan, NULL)) {
            break;
          }

          colIndex = NW_UINT16_CAST(colIndex + colSpan);
        }

        /* If the cell does not fit in the table, ignore it 
           and move on to the next row */
        if (colIndex >= thisObj->numCols) {
          break;
        }

        /* Get the cell span properties */
        prop.value.integer = 1;
        NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_colSpan, &prop);
        colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;

        /* If the specified spans make the cell too large for the table,
         * reduce the spans.  We modify the box property lists in order to
         * avoid having to calculate them again when we place the cells. */
        if (colIndex + colSpan > thisObj->numCols) {
          prop.type = NW_ADT_ValueType_Integer;
          prop.value.integer = (NW_Int32)(thisObj->numCols - colIndex);

          status = NW_LMgr_Box_SetProperty (cellBox, NW_CSS_Prop_colSpan, &prop);
          NW_THROW_ON_ERROR(status);

          colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
        }

        colIndex = (NW_ADT_Vector_Metric_t)(colIndex + colSpan);
      } // end of for (j = 0; j < cellCount; j++) {

      /* If this was the last cell in the row, and we still do not 
       * have enough columns, we pad the row with a dummy cell. */
      while (colIndex < thisObj->numCols) {
        /* find the span number that we should skip */
        for (k = colIndex; k < thisObj->numCols; ) {
          if (!NW_LMgr_StaticTableContext_FindSpan (thisObj, k, rowIndex, NULL, NULL, &colSpan, NULL)) {
            break;
          }
          k = NW_UINT16_CAST(k + colSpan);
        }
        colIndex = k;
        if (colIndex >= thisObj->numCols) {
          break;
        }
        /* Get a dummy cell */
        cellBox = (NW_LMgr_Box_t*)NW_LMgr_StaticTableCellBox_New(0);

        /* Set display block property */
        prop.type = NW_CSS_ValueType_Token;
        prop.value.token = NW_CSS_PropValue_display_block;
        status = NW_LMgr_Box_SetProperty (cellBox, NW_CSS_Prop_display, &prop);
        _NW_THROW_ON_ERROR(status);

        /* Set its span property */
        prop.type = NW_ADT_ValueType_Integer;
        prop.value.integer = (NW_Int32)1;

        status = NW_LMgr_Box_SetProperty (cellBox, NW_CSS_Prop_colSpan, &prop);
        _NW_THROW_ON_ERROR(status);

        /* Find the flow direction of the row */        
        dirValue.token = NW_CSS_PropValue_ltr;
        NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(rowContainer), 
                                     NW_CSS_Prop_textDirection,
                                     NW_CSS_ValueType_Token, 
                                     &dirValue);
        /* Insert the new box into the table */
        if (dirValue.token == NW_CSS_PropValue_rtl) {
          status = NW_LMgr_ContainerBox_InsertChildAt (rowContainer, cellBox, 0);
        }
        else {
          status = NW_LMgr_ContainerBox_AddChild(rowContainer, cellBox);
        }
        _NW_THROW_ON_ERROR(status);

        colIndex = NW_UINT16_CAST(colIndex + 1);
      } // end of while (colIndex < thisObj->numCols) {

	  /* Go through all the cells in the row for the 2nd time */
      cellCount = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

      /* Start at column 0 */
      colIndex = 0;
      rowIndex = i;

      /* Go through all the cells in the row */
      for (j = 0; j < cellCount; j++) {

        /* Get the cell */
        cellBox = NW_LMgr_ContainerBox_GetChild (rowContainer, j);

        /* Skip any rowspans */
        while (colIndex < thisObj->numCols) {

          if (!NW_LMgr_StaticTableContext_FindSpan (thisObj, colIndex, rowIndex, NULL, NULL, &colSpan, NULL)) {
            break;
          }

          colIndex = NW_UINT16_CAST(colIndex + colSpan);
        }

        /* If the cell does not fit in the table, ignore it 
           and move on to the next row */
        if (colIndex >= thisObj->numCols) {
          break;
        }

        /* Get the cell span properties */
        prop.value.integer = 1;
        NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_colSpan, &prop);
        colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
        if (colSpan == 0) {
          colSpan = (NW_ADT_Vector_Metric_t)(thisObj->numCols - colIndex);  
        }

        prop.value.integer = 1;
        NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_rowSpan, &prop);
        rowSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
        if (rowSpan == 0) {
          rowSpan = (NW_ADT_Vector_Metric_t)(thisObj->numRows - rowIndex);  
        }

        /* If the specified spans make the cell too large for the table,
         * reduce the spans.  We modify the box property lists in order to
         * avoid having to calculate them again when we place the cells. */
        if (colIndex + colSpan > thisObj->numCols) {
          prop.type = NW_ADT_ValueType_Integer;
          prop.value.integer = (NW_Int32)(thisObj->numCols - colIndex);

          status = NW_LMgr_Box_SetProperty (cellBox, NW_CSS_Prop_colSpan, &prop);
          NW_THROW_ON_ERROR(status);

          colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
          if (colSpan == 0) {
            colSpan = (NW_ADT_Vector_Metric_t)(thisObj->numCols - colIndex);  
		  }
        }
        if (rowIndex + rowSpan > thisObj->numRows) {
          prop.value.integer = (NW_Int32)(thisObj->numRows - rowIndex);

          status = NW_LMgr_Box_SetProperty (cellBox, NW_CSS_Prop_rowSpan, &prop);
          NW_THROW_ON_ERROR(status);

          rowSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;
          if (rowSpan == 0) {
            rowSpan = (NW_ADT_Vector_Metric_t)(thisObj->numRows - rowIndex);  
		  }
        }

        if (colSpan > 1 || rowSpan > 1) {
          status = NW_LMgr_StaticTableContext_AddSpanToMap
                      (thisObj, colIndex, rowIndex, colSpan, rowSpan);
          NW_THROW_ON_ERROR(status);
        }

        colIndex = (NW_ADT_Vector_Metric_t)(colIndex + colSpan);
      } // end of for (j = 0; j < cellCount; j++) {
    } // for (i = 0; i < thisObj->numRows; i++) {
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_GenerateBorderInfo
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_GenerateBorderInfo (NW_LMgr_StaticTableContext_t* thisObj)
{
  NW_LMgr_ContainerBox_t *tableContainer;
  NW_LMgr_ContainerBox_t *rowContainer;
  NW_LMgr_Box_t *rowBox;
  NW_ADT_Vector_Metric_t k, l;
  NW_ADT_Vector_Metric_t cellCount;
  NW_LMgr_Box_t *cellBox;
  NW_ADT_Vector_Metric_t numStyles;
  NW_LMgr_StaticTableBorderInfo_t *borderInfo;
  NW_LMgr_StaticTableBorderEdge_t edges[4];
  NW_Uint16 matches[4];
  NW_Bool lastRow = NW_FALSE;
  NW_Bool lastCol = NW_FALSE;
  NW_Bool lastRowTmp = NW_FALSE;
  NW_Uint16 px, py;
  NW_Uint16 colSpan;
  NW_Uint16 rowSpan = 0;
  NW_Uint16 numCols = thisObj->numCols;
  NW_Uint16 numRows = thisObj->numRows;
  NW_Uint16 preCount;
  NW_ADT_Vector_Metric_t rowIndex, cellIndex;

  NW_TRY (status) {

    if (thisObj->borderInfos == NULL) {
      thisObj->borderInfos = 
        NW_ADT_ResizableVector_New (sizeof (NW_LMgr_StaticTableBorderInfo_t), 10, 5 );
      NW_THROW_OOM_ON_NULL(thisObj->borderInfos, status);
    }

    tableContainer = NW_LMgr_ContainerBoxOf(thisObj->tableBox);

    for (rowIndex = 0; rowIndex < thisObj->numRows; ) {

      rowBox = NW_LMgr_ContainerBox_GetChild (tableContainer, rowIndex);
      rowContainer = NW_LMgr_ContainerBoxOf(rowBox);

      /* How many cells in the row? */
      cellCount = NW_LMgr_ContainerBox_GetChildCount(rowContainer);

      /* Go through all the cells in the row */
      for (cellIndex = 0; cellIndex < cellCount; ) {

        /* Get the cell */
        cellBox = NW_LMgr_ContainerBox_GetChild (rowContainer, cellIndex);

        /* Get the (px,py) of the cell */
        status = NW_LMgr_StaticTableContext_LxLyToPxPy(thisObj, cellIndex, rowIndex, 
                                                       &px, &py, 
                                                       &colSpan, &rowSpan);
        if (status == KBrsrNotFound) {
          status = KBrsrSuccess;
          break;
        }
        _NW_THROW_ON_ERROR(status);

        /* in case a cell not on the last row but its rowspan reach bottom of the table,
           we want to draw both top and bottom border of it */
        if (rowIndex + rowSpan == numRows && rowIndex < numRows-1) {
          lastRowTmp = NW_TRUE;
		}

        matches[left] = NW_UINT16_CAST(!lastCol ? 0 : 0xffff);
        matches[top] = NW_UINT16_CAST(!lastRow ? 0 : 0xffff);
        matches[right] = NW_UINT16_CAST(lastCol ? 0 : 0xffff);
        matches[bottom] = NW_UINT16_CAST((lastRow || lastRowTmp) ? 0 : 0xffff);
        NW_Mem_memset(edges, 0xffffffff, sizeof(NW_LMgr_StaticTableBorderEdge_t) * 4);

        /* Calculate the borders for the cell */
        status = NW_LMgr_StaticTableCellBox_CalculateBorders(NW_LMgr_StaticTableCellBoxOf(cellBox), 
                                                             px, py,
                                                             colSpan, rowSpan,
                                                             (lastCol ? NULL : &edges[left]), 
                                                             (lastCol ? &edges[right] : NULL), 
                                                             (lastRow ? NULL : &edges[top]),
                                                             ((lastRow || lastRowTmp) ? &edges[bottom] : NULL)); 
        NW_THROW_ON_ERROR(status);

        /* Do these border styles already exist? */
        numStyles = NW_ADT_Vector_GetSize (thisObj->borderInfos);
        for (k = 0; k < numStyles; k++) {
          borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
            NW_ADT_Vector_ElementAt (thisObj->borderInfos, k);

          if (!lastCol) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(borderInfo->edge, edges[left])) {
              borderInfo->count ++;
              matches[left] ++;
            }
          }
          if (!lastRow) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(borderInfo->edge, edges[top])) {
              borderInfo->count ++;
              matches[top] ++;
            }
          }
          if (lastCol) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(borderInfo->edge, edges[right])) {
              borderInfo->count ++;
              matches[right] ++;
            }
          }
          if (lastRow || lastRowTmp) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(borderInfo->edge, edges[bottom])) {
              borderInfo->count ++;
              matches[bottom] ++;
            }
          }
        }

        /* In case the style does not exist already, we must add a new
           BorderInfo entry to the list */
        for (k = 0; k < 4; k++) {
          if (matches[k] == 0) {
            preCount = 0;
            for (l = 0; l < k - 1; l++) {
              if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(edges[k], edges[l])) {
                preCount++;
              }
            }
            for (l = NW_UINT16_CAST(k + 1); l < 4; l++) {
              if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(edges[k], edges[l])) {
                break;
              }
            }
            if (l == 4) {
              status = NW_LMgr_StaticTableContext_AddBorderInfo(thisObj, &edges[k], (NW_Uint16)(preCount + 1));
              NW_THROW_ON_ERROR(status);
            }
          }
        }

        /* update the map */
        numStyles = NW_ADT_Vector_GetSize (thisObj->borderInfos);
        for (k = 0; k < numStyles; k++) {
          /* Get the border info */
          borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
            NW_ADT_Vector_ElementAt (thisObj->borderInfos, k);

          /* Update the border entry */
          if (!lastCol) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(edges[left], borderInfo->edge)) {
              status = NW_LMgr_StaticTableContext_AddVEdgeToMap(
                             thisObj, px, py, (NW_Uint16)(py + rowSpan), k);
              NW_THROW_ON_ERROR(status);
            }
          }
          if (!lastRow) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(edges[top], borderInfo->edge)) {
              status = NW_LMgr_StaticTableContext_AddHEdgeToMap(
                             thisObj, py, px, (NW_Uint16)(px + colSpan), k);
              NW_THROW_ON_ERROR(status);
            }
          }
          if (lastCol) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(edges[right], borderInfo->edge)) {
              status = NW_LMgr_StaticTableContext_AddVEdgeToMap(
                             thisObj, (NW_Uint16)(px + colSpan), py, (NW_Uint16)(py + rowSpan), k);
              NW_THROW_ON_ERROR(status);
            }
          }
          if (lastRow || lastRowTmp) {
            if (NW_LMGR_STATICTABLECONTEXT_CMPEDGE(edges[bottom], borderInfo->edge)) {
              status = NW_LMgr_StaticTableContext_AddHEdgeToMap(
                             thisObj, (NW_Uint16)(py + rowSpan), px, (NW_Uint16)(px + colSpan), k);
              NW_THROW_ON_ERROR(status);
            }
          }
        }
        if (lastCol) {
          lastCol = NW_FALSE;
          break;
        }
        if (px + colSpan == numCols) {
          lastCol = NW_TRUE;
        }
        else {
          cellIndex ++;
        }

        lastRowTmp = NW_FALSE;
      } // end of Go through all the cells in the row

      if (lastRow) {
        lastRow = NW_FALSE;
        break;
      }
      if (rowIndex == numRows - 1) {
        lastRow = NW_TRUE;
      }
      else {
        rowIndex ++;
      }
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_CompressBorderInfo
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_CompressBorderInfo (NW_LMgr_StaticTableContext_t* thisObj)
{
  NW_ADT_Vector_Metric_t k;
  NW_ADT_Vector_Metric_t numStyles;
  NW_LMgr_StaticTableBorderInfo_t *borderInfo;
  NW_Uint16 maxCount;
  NW_Uint16 map_info;
  NW_Uint32 *mapEntry;
  NW_ADT_Vector_Metric_t vectorSize;
  NW_ADT_Vector_Metric_t index;
  NW_ADT_ResizableVector_t *vector;
  NW_ADT_Vector_Metric_t size;

  NW_TRY (status) {

    /* Determine the primary border style */
    numStyles = NW_ADT_Vector_GetSize (thisObj->borderInfos);
    maxCount = 0;
    for (k = 0; k < numStyles; k++) {
      borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
        NW_ADT_Vector_ElementAt (thisObj->borderInfos, k);
      if (maxCount < borderInfo->count) {
        maxCount = borderInfo->count;
        thisObj->defaultInfo = k;
      }
    }

    if (thisObj->horizontalEdges) {

      /* Remove all edges from the vector that have the primary border style. */
      vectorSize = NW_ADT_Vector_GetSize(thisObj->horizontalEdges);
      if (vectorSize > 0) {
        for (index = 0; index < vectorSize; ) {
          mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->horizontalEdges, index);
          map_info = NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(*mapEntry);

          if (map_info == thisObj->defaultInfo) {
            status = NW_ADT_DynamicVector_RemoveAt(thisObj->horizontalEdges, index);
            NW_THROW_ON_ERROR(status);

            vectorSize --;
          }
          else {
            index ++;
          }
        }
      }

      /* Now reallocate the vectors so we don't keep more memory than we need */
      if (NW_ADT_Vector_GetSize(thisObj->horizontalEdges)
           < NW_ADT_DynamicVector_GetCapacity(thisObj->horizontalEdges)) {
        vector = thisObj->horizontalEdges;
        size = NW_ADT_Vector_GetSize(vector);
        if (size > 0) {
          thisObj->horizontalEdges = 
            NW_ADT_ResizableVector_New (sizeof (NW_Uint32), size, 5);
          NW_THROW_OOM_ON_NULL(thisObj->horizontalEdges, status);

          status =
            NW_ADT_DynamicVector_InsertVectorAt(thisObj->horizontalEdges, vector, NW_ADT_Vector_AtFront);
          NW_THROW_ON_ERROR(status);
        }
        else {
          thisObj->horizontalEdges = NULL;
        }
        NW_Object_Delete(vector);
      }
    }

    if (thisObj->verticalEdges) {

      /* Remove all edges from the vector that have the primary border style. */
      vectorSize = NW_ADT_Vector_GetSize(thisObj->verticalEdges);
      if (vectorSize > 0) {
        for (index = 0; index < vectorSize; ) {
          mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->verticalEdges, index);
          map_info = NW_LMGR_STATICTABLEVEDGEENTRY_GETINFO(*mapEntry);

          if (map_info == thisObj->defaultInfo) {
            status = NW_ADT_DynamicVector_RemoveAt(thisObj->verticalEdges, index);
            NW_THROW_ON_ERROR(status);

            vectorSize --;
          }
          else {
            index ++;
          }
        }
      }

      /* Now reallocate the vectors so we don't keep more memory than we need */
      if (NW_ADT_Vector_GetSize(thisObj->verticalEdges)
           < NW_ADT_DynamicVector_GetCapacity(thisObj->verticalEdges)) {
        vector = thisObj->verticalEdges;
        size = NW_ADT_Vector_GetSize(vector);
        if (size > 0) {
          thisObj->verticalEdges = 
            NW_ADT_ResizableVector_New (sizeof (NW_Uint32), size, 5);
          NW_THROW_OOM_ON_NULL(thisObj->verticalEdges, status);

          status =
            NW_ADT_DynamicVector_InsertVectorAt(thisObj->verticalEdges, vector, NW_ADT_Vector_AtFront);
          NW_THROW_ON_ERROR(status);
        }
        else {
          thisObj->verticalEdges = NULL;
        }
        NW_Object_Delete(vector);
      }
    }

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}


/* -------------------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_FindAutomaticConstraints
 * Parameters:    thisObj - the object
 * Description:   
 * Automatic table layout                                                               
 * http://www.w3.org/TR/REC-CSS2/tables.html#auto-table-layout                          
 *                                                                                        
 * Column widths are determined as follows:                                             
 * Calculate the minimum content width (MCW) of each cell: the formatted content         
 * may span any number of lines but may not overflow the cell box.                      
 * If the specified 'width' (W) of the cell is greater than MCW, W is the minimum       
 * cell width. A value of 'auto' means that MCW is the minimum cell width.              
 * Also, calculate the "maximum" cell width of each cell: formatting then content       
 * without breaking lines other than where explicit line breaks occur.                   
 *                                                                                      
 * For each column, determine a maximum and minimum column width from the cells         
 * that span only that column. The minimum is that required by the cell with the        
 * largest minimum cell width (or the column 'width', whichever is larger). The         
 * maximum is that required by the cell with the largest maximum cell width             
 * (or the column 'width', whichever is larger).                                        
 * For each cell that spans more than one column, increase the minimum widths of        
 * the columns it spans so that together, they are at least as wide as the cell.        
 * Do the same for the maximum widths. If possible, widen all spanned columns           
 * by approximately the same amount.                                                    
 * This gives a maximum and minimum width for each column. Column widths influence      
 * the final table width as follows:                                                    
 *                                                                                      
 * If the 'table' or 'inline-table' element's 'width' property has a specified value (W)
 * other than 'auto', the property's computed value is the greater of W and the minimum 
 * width required by all the columns plus cell spacing or borders (MIN).                
 * If W is greater than MIN, the extra width should be distributed over the columns.    
 * If the 'table' or 'inline-table' element has 'width: auto', the computed table width  
 * is the greater of the table's containing block width and MIN. However, if the maximum
 * width required by the columns plus cell spacing or borders (MAX) is less than that of
 * the containing block, use MAX.                                                       
 */
static
TBrowserStatusCode
NW_LMgr_StaticTableContext_FindAutomaticConstraints( NW_LMgr_StaticTableContext_t* thisObj )
  {
  NW_ADT_DynamicVector_t* minColumnWidthArray = NULL;
  NW_ADT_DynamicVector_t* maxColumnWidthArray = NULL;
  NW_TRY( status ) 
    {
    NW_ADT_Vector_Metric_t        rowIndex;
    NW_ADT_Vector_Metric_t        colIndex;
    NW_ADT_Vector_Metric_t        currentCol;
    NW_ADT_Vector_Metric_t        currentRow;
    NW_ADT_DynamicVector_t*       finalColumnWidthArray = NULL;
    NW_GDI_Metric_t               defaultCellWidthValue = NW_LMGR_BOX_MIN_WIDTH; 
    NW_GDI_Metric_t               nonResizeableValue = -1;
    NW_GDI_Metric_t               minCellWidth;
    NW_GDI_Metric_t               maxCellWidth;
    NW_GDI_Metric_t               tableWidth = 0;
    NW_GDI_Metric_t               minColumnsWidth = 0;
    NW_GDI_Metric_t               maxColumnsWidth = 0;
    NW_GDI_Metric_t               expandableWidth = 0;
    NW_GDI_Metric_t               maxExpandableWidth = 0;
    NW_GDI_Metric_t               columnWidth;
    NW_GDI_Metric_t               spaceAvailable;
    NW_GDI_Metric_t               spaceToDivide;
    NW_LMgr_Property_t            prop;
    NW_Bool                       colSpanFound = NW_FALSE;
    NW_LMgr_FrameInfo_t           borderWidth;
    NW_LMgr_Box_t*                cellBox;
    NW_LMgr_Box_t*                tableBox;

    NW_REQUIRED_PARAM( thisObj );
    
    NW_ASSERT( thisObj != NULL );

    tableBox = NW_LMgr_BoxOf( thisObj->tableBox );
    NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds( tableBox ); 
    NW_THROW_OOM_ON_NULL( tableBox, status );
    // min array
    minColumnWidthArray = (NW_ADT_DynamicVector_t*)NW_ADT_ResizableVector_New( 
      sizeof ( NW_GDI_Metric_t ), thisObj->numCols, 5 );
    NW_THROW_OOM_ON_NULL( minColumnWidthArray, status );
    // max array
    maxColumnWidthArray = (NW_ADT_DynamicVector_t*)NW_ADT_ResizableVector_New( 
      sizeof ( NW_GDI_Metric_t ), thisObj->numCols, 5 );
    NW_THROW_OOM_ON_NULL( maxColumnWidthArray, status );
    // step through all the cells and find out the max and the min width for each and every column.
    // no colspan is taken into accout in the first pass so that we can distribute colspan width proportinally
    // note: w3's recommendation is to distribute colspans equally but it gives badly rendered pages.
    for( colIndex = 0; colIndex < thisObj->numCols; colIndex++ )
      {
      // initialise min and max 
      NW_ADT_DynamicVector_InsertAt( minColumnWidthArray, &defaultCellWidthValue, colIndex );
      NW_ADT_DynamicVector_InsertAt( maxColumnWidthArray, &defaultCellWidthValue, colIndex ); 
      // take rows first so then at the end, we've got the colIndex-th max and min width
      for( rowIndex = 0; rowIndex < thisObj->numRows; rowIndex++ )
        {                                               
        status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy( thisObj, colIndex, rowIndex, &cellBox );
        NW_THROW_ON_ERROR( status );
        // cellbox can be NULL even if status == KBrsrSuccess
        if( cellBox != NULL )
          {
          // min width
          minCellWidth = NW_LMgr_StaticTableCellBox_GetMinWidth( NW_LMgr_StaticTableCellBoxOf( cellBox ) );
          // max width
          maxCellWidth = NW_LMgr_StaticTableCellBox_GetMaxWidth( NW_LMgr_StaticTableCellBoxOf( cellBox ) );
          // colspan checking 
          prop.value.integer = 1;
          NW_LMgr_Box_GetPropertyFromList( cellBox, NW_CSS_Prop_colSpan, &prop );

          if( (NW_ADT_Vector_Metric_t)prop.value.integer <= 1 )
            {
            // no colspan
            // check if this is bigger than the minimum value
            if( minCellWidth > *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndex ) )          
              {
              NW_ADT_DynamicVector_ReplaceAt( minColumnWidthArray, &minCellWidth, colIndex );
              }
            // and check if this is bigger than the maximum value
            if( maxCellWidth > *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndex ) )
              {
              NW_ADT_DynamicVector_ReplaceAt( maxColumnWidthArray, &maxCellWidth, colIndex );
              }
            }
          else
            {
            colSpanFound = NW_TRUE;
            }
          }
        }
      // calculate total max/min width
      minColumnsWidth+= *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndex );
      maxColumnsWidth+= *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndex );
      }
    // second pass reqired only if colspan is present
    if( colSpanFound == NW_TRUE )
      {
      NW_ADT_Vector_Metric_t colSpan;

      // colspan may change them
      minColumnsWidth = 0;
      maxColumnsWidth = 0;
      // step through all the cells and find out the max and the min width for each and every column.
      for( colIndex = 0; colIndex < thisObj->numCols; colIndex++ )
        {
        // take rows first so then at the end, we've got the colIndex-th max and min width
        for( rowIndex = 0; rowIndex < thisObj->numRows; rowIndex++ )
          {                                               
          status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy( thisObj, colIndex, rowIndex, &cellBox );
          NW_THROW_ON_ERROR( status );
          // cellbox can be NULL even if status == KBrsrSuccess
          if( cellBox != NULL )
            {
            // min width
            minCellWidth = NW_LMgr_StaticTableCellBox_GetMinWidth( NW_LMgr_StaticTableCellBoxOf( cellBox ) );
            // max width
            maxCellWidth = NW_LMgr_StaticTableCellBox_GetMaxWidth( NW_LMgr_StaticTableCellBoxOf( cellBox ) );
            // colspan handling 
            prop.value.integer = 1;
            NW_LMgr_Box_GetPropertyFromList( cellBox, NW_CSS_Prop_colSpan, &prop );
            colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;

            if( colSpan > 1 )
              {
              status = NW_LMgr_StaticTableContext_PxPyToLxLy( thisObj, colIndex, rowIndex, &currentCol, &currentRow );
              NW_THROW_ON_ERROR( status );
              // note: you get the very same cellbox back within the coloumn span. so let's compare currentCol to colIndex to see if 
              // we get an already calculated col span to avoid calculating it again and again.
              if( currentCol == colIndex && currentRow == rowIndex )
                {
                NW_GDI_Metric_t        minSpanColumnsWidth = 0;
                NW_GDI_Metric_t        maxSpanColumnsWidth = 0;
                NW_GDI_Metric_t        colSpanMinWidth;
                NW_GDI_Metric_t        colSpanMaxWidth;
                NW_ADT_Vector_Metric_t colSpanIndex;
                NW_ADT_Vector_Metric_t colIndexWithSpan;
                // first add up those columns width that this cell spans
                for( colSpanIndex = 0; colSpanIndex < colSpan; colSpanIndex++ )
                  {
                  // currentCol + colSpanIndex musnt't be more than tableContext->numCols
                  colIndexWithSpan = (NW_ADT_Vector_Metric_t)(  currentCol + colSpanIndex );
                  NW_ASSERT(  colIndexWithSpan < thisObj->numCols );

                  minSpanColumnsWidth+= *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndexWithSpan );
                
                  maxSpanColumnsWidth+= *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndexWithSpan );
                  }
                // and then distribute width proportionally
                for( colSpanIndex = 0; colSpanIndex < colSpan; colSpanIndex++ )
                  {
                  //lint --e{795} Conceivable division by 0
                  // TODO: evaluate this lint issue.

                  // currentCol + colSpanIndex musnt't be more than tableContext->numCols
                  colIndexWithSpan = (NW_ADT_Vector_Metric_t)(  currentCol + colSpanIndex );
                  NW_ASSERT(  colIndexWithSpan < thisObj->numCols );           
                  // distribute proportinaly
                  colSpanMinWidth = (NW_GDI_Metric_t)( (NW_Float32)minCellWidth/ (NW_Float32)minSpanColumnsWidth * 
                    (NW_Float32)( *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndexWithSpan ) ) + 1 ); // +1 is for rounding

                  colSpanMaxWidth = (NW_GDI_Metric_t)( (NW_Float32)maxCellWidth/ (NW_Float32)maxSpanColumnsWidth * 
                    (NW_Float32)( *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndexWithSpan ) ) + 1 ); // +1 is for rounding
                  // check if this is bigger than the minimum value
                  if( colSpanMinWidth > *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndexWithSpan ) )          
                    {
                    NW_ADT_DynamicVector_ReplaceAt( minColumnWidthArray, &colSpanMinWidth, colIndexWithSpan );
                    }
                  // and check if this is bigger than the maximum value
                  if( colSpanMaxWidth > *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndexWithSpan ) )
                    {
                    NW_ADT_DynamicVector_ReplaceAt( maxColumnWidthArray, &colSpanMaxWidth, colIndexWithSpan );
                    }
                  }
                }
              }
            }
          }
        // calculate total max/min width
        minColumnsWidth+= *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndex );
        maxColumnsWidth+= *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndex );
        }
      }    
    // at this point we've got all the min and max values. let's check if the table needs to be shrunken
    spaceAvailable = tableBoxBounds.dimension.width;
    // remove borders 
    NW_LMgr_Box_GetBorderWidth( tableBox, &borderWidth, ELMgrFrameLeft|ELMgrFrameRight );

    spaceAvailable-=(borderWidth.left + borderWidth.right);

    // 1. if the max width of columns is less than the table width and the width of the table is specified, 
    //    then we need to distribute the extra space among those columns. otherwise, we take the max column width
    // 2. if minimum width is bigger than the table width, then we cannot do much. go with the minimum width
    // 3. if the table width is somewhere between the min and the max columns width, then take a proportional value
    //    according to the w3c recommendation
    
    if( maxColumnsWidth < spaceAvailable )
      {
      status = NW_LMgr_Box_GetRawProperty( tableBox, NW_CSS_Prop_width, &prop );
      if( status == KBrsrSuccess )
        {
        // when we make the table bigger, then dont care about the non-resizeable cells
        spaceToDivide = spaceAvailable - maxColumnsWidth;
        // calculate columns length proportionaly
        for( colIndex = 0; colIndex < thisObj->numCols; colIndex++ )
          {
          //lint --e{795} Conceivable division by 0
          // TODO: evaluate this lint issue.

          // calculate the value according to what min array has unless this is a non-resizeable item
          columnWidth = *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndex );
          // add the extra space
          columnWidth+= (NW_GDI_Metric_t)( (NW_Float32)spaceToDivide / (NW_Float32)maxColumnsWidth * (NW_Float32)columnWidth );            
          // and replace the value in the MAX array
          NW_ADT_DynamicVector_ReplaceAt( maxColumnWidthArray, &columnWidth, colIndex );
          }
        // max array has both the modified column values as well as the fixed (non-resizeable) ones.
        }
        finalColumnWidthArray = maxColumnWidthArray;     
      }
    else if( minColumnsWidth > spaceAvailable )
      {
      finalColumnWidthArray = minColumnWidthArray;
      }
    else
      {
      // first take out those columns that are nonresizeable.
      for( colIndex = 0; colIndex < thisObj->numCols; colIndex++ )
        {
        NW_GDI_Metric_t minValue;
        NW_GDI_Metric_t maxValue;

        minValue = *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndex );
        maxValue = *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndex );
        // the column is considered to be nonresizeable if max equals to min.
        if( minValue == maxValue )
          {
          // nonresizeable elemets must be removed from bot available space and the total min
          spaceAvailable-= minValue;
          minColumnsWidth-= minValue;
          // minColumn array is used to indicate that the column is nonresizeable. max column still holds the column width value
          // set minimum value to nonResizeableValue
          NW_ADT_DynamicVector_ReplaceAt( minColumnWidthArray, &nonResizeableValue, colIndex );
          }
        else
          {
          // add up all expanable values
          maxExpandableWidth+= ( maxValue - minValue ); 
          }
        }
      // calculate columns lentgh proportionaly
      spaceToDivide = spaceAvailable - minColumnsWidth;
      for( colIndex = 0; colIndex < thisObj->numCols; colIndex++ )
        {
        // calculate the value according to what min array has unless this is a non-resizeable item
        columnWidth = *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndex );
        expandableWidth = *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( maxColumnWidthArray, colIndex ) -
          *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( minColumnWidthArray, colIndex );
        if( columnWidth != nonResizeableValue )
          {
          //lint --e{795, 414} Conceivable division by 0
          // TODO: evaluate this lint issue.

          // add the extra space
          columnWidth+= (NW_GDI_Metric_t)( (NW_Float32)spaceToDivide / (NW_Float32)maxExpandableWidth * (NW_Float32)expandableWidth );            
          // and replace the value in the MAX array
          NW_ADT_DynamicVector_ReplaceAt( maxColumnWidthArray, &columnWidth, colIndex );
          }
        }
      // max array has both the modified column values as well as the fixed (non-resizeable) ones.
      finalColumnWidthArray = maxColumnWidthArray;
      }
    // final includes the columns contstrains    
    NW_THROW_OOM_ON_NULL( finalColumnWidthArray, status );
    for( colIndex = 0; colIndex < thisObj->numCols; colIndex++ )
      {
      NW_LMgr_StaticTableContext_SetColConstraint( thisObj, colIndex, 
        *(NW_GDI_Metric_t*)NW_ADT_Vector_ElementAt( finalColumnWidthArray, colIndex ) );
      tableWidth+= NW_LMgr_StaticTableContext_GetColConstraint( thisObj, colIndex );
      }
    // add borders 
    NW_LMgr_Box_GetBorderWidth( tableBox, &borderWidth, ELMgrFrameLeft|ELMgrFrameRight );

    tableWidth+=(borderWidth.left + borderWidth.right);
    // and update the table width
    tableBoxBounds.dimension.width = tableWidth;
    NW_LMgr_Box_SetFormatBounds( tableBox, tableBoxBounds ); 
    }
  NW_CATCH( status ) 
    {
    }
  NW_FINALLY 
    {
    NW_Object_Delete( minColumnWidthArray );
    NW_Object_Delete( maxColumnWidthArray );
    return status;
    } 
  NW_END_TRY;
  }

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellBoxConstraint(NW_LMgr_StaticTableContext_t* thisObj,
    NW_LMgr_Box_t* box, NW_GDI_Metric_t tableWidth, NW_GDI_Metric_t* width, NW_Bool* isAuto,
    NW_Bool* isImage)
  {
  NW_LMgr_StaticTableCellBox_t*  cellBox;
  NW_GDI_Dimension2D_t           boxSize;
  NW_LMgr_BoxVisitor_t           visitor;
  NW_LMgr_Box_t*                 child;
  NW_Int32                       textWidth = 0;
  NW_Int32                       longestTextRun = 0;
  NW_LMgr_FrameInfo_t            borderWidth;
  NW_LMgr_FrameInfo_t            paddingInfo;
  NW_GDI_Metric_t                extraSpace = 0;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(box != NULL);
  NW_ASSERT(width != NULL);
  NW_ASSERT(isAuto != NULL);
  NW_ASSERT(isImage != NULL);

  NW_TRY(status) 
    {
    cellBox = NW_LMgr_StaticTableCellBoxOf(box);
    *isImage = NW_FALSE;

    // A box with a fixed width doesn't have an auto-width.
    *isAuto = !NW_LMgr_Box_HasFixedWidth(box);

    // Get the extra space needed for the border and padding.
    status = NW_LMgr_StaticTableContext_GetCellBordersFromPtr(thisObj, box, 
        &borderWidth, NULL, NULL); 
    NW_THROW_ON_ERROR(status);

    NW_LMgr_StaticTableCellBox_GetPadding(cellBox, &paddingInfo);

    extraSpace = borderWidth.left + borderWidth.right + paddingInfo.left + paddingInfo.right;

    // Get the cell's natural minimum size.
    status = NW_LMgr_StaticTableCellBox_GetMinimumCellSize(cellBox, &boxSize);
    NW_THROW_ON_ERROR(status);

    // Use this width unless it is overridden below.
    *width = boxSize.width;

    // If this is a fixed width cell, its all done.
    if (*isAuto == NW_FALSE)
      {
      NW_THROW_SUCCESS(status);
      }

    *width = extraSpace;

    // Iterate through the cell-box's child to estimate the amount of 
    // space needed.
    status = NW_LMgr_BoxVisitor_Initialize(&visitor, box);
    NW_THROW_ON_ERROR(status);

    while ((child = NW_LMgr_BoxVisitor_NextBox(&visitor, NULL)) != NULL) 
      {
      // If an image is found use the image's width property.
      if (NW_Object_IsInstanceOf(child, &NW_LMgr_ImgContainerBox_Class))
        {
        NW_LMgr_PropertyValue_t  value;

        status = NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_width, 
            NW_CSS_ValueType_Px, &value);

        if (status == KBrsrSuccess)
          {
          *width = value.integer + extraSpace;
          *isAuto = NW_FALSE;
          *isImage = NW_TRUE;          
          thisObj->gridModeImageCount++;

          NW_THROW_SUCCESS(status);
          }
        }
      
      // If an form-box is found use the form's skin size.
      else if (NW_Object_IsInstanceOf(child, &NW_FBox_FormBox_Class))
        {
        NW_FBox_Skin_t*       skin = NULL;
        NW_GDI_Dimension3D_t  skinSize;

        skin = NW_FBox_FormBox_GetSkin(child);
        if (skin)
          {
          NW_FBox_Skin_GetSize(skin, child, &skinSize);

          *width += skinSize.width;
          *isAuto = NW_FALSE;
          }
        }

      // If a text-box is found update the running total, as a cell-box
      // may contain many such boxes.
      else if (NW_Object_IsInstanceOf(child, &NW_LMgr_TextBox_Class))
        {
        NW_GDI_Dimension2D_t size;

        status = NW_LMgr_AbstractTextBox_GetDimensions(
            NW_LMgr_AbstractTextBoxOf(child), &size);

        if (status == KBrsrSuccess)
          {
          textWidth += size.width;
          }
        }

      // When calculating the width of a text block in a cell it needs
      // to be aware of line breaks that are found in the markup 
      // (i.e. <br/>).  When such a line break is found the longest
      // text run is tracked.
      else if (NW_Object_IsInstanceOf(child, &NW_LMgr_BreakBox_Class))
        {
        if (textWidth > longestTextRun)
          {
            longestTextRun = textWidth;
          }

        textWidth = 0;
        }

      else
        {
        NW_GDI_Dimension2D_t size;

        // Get the minimum content size.
        status = NW_LMgr_Box_GetMinimumContentSize(child, &size);
        _NW_THROW_ON_ERROR (status);

        *width += size.width;
        *isAuto = NW_FALSE;
        }
      }

      // See the comment in the BreakBox section above.  textWidth is 
      // only set to longestTextRun when longestTextRun is larger than
      // textWidth.  This is done because textWidth tracks the last text run, 
      // which may be larger than longestTextRun.
      if (longestTextRun > textWidth)
        {
        textWidth = longestTextRun;
        }

      // If the text is wider than the table clip it, so that a longer
      // text block doesn't cramp other columns.
      if (textWidth > tableWidth)
        {
        textWidth = tableWidth;
        }

      // If text was found, override the value set above.
      if (textWidth > 0)
        {
        *width += textWidth;
        *isAuto = NW_FALSE;
        }
    }

  NW_CATCH(status) 
    {
    }

  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }

static
TBrowserStatusCode
NW_LMgr_StaticTableContext_FindRowConstraints(NW_LMgr_StaticTableContext_t* thisObj,
    NW_LMgr_ContainerBox_t* rowBox, NW_Int32 rowIndex, NW_Int32* rowConstraints,
    NW_Bool* hasOnlyImages)
  {
  NW_Int32            cellCount;
  NW_Int32            colIndex;
  NW_Int32            autoColCount = 0;
  NW_Int32            autoColWidth = 0;
  NW_Int32            fixedColWidth = 0;
  NW_Int32            minAutoWidth = 0;
  NW_Int32            colSpan;
  NW_LMgr_Property_t  prop;
  NW_Int32            width;
  NW_Int32            tableWidth;
  NW_LMgr_Box_t*      cellBox = NULL;
  NW_Bool             isAuto;
  NW_Bool             isImage;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(rowBox != NULL);
  NW_ASSERT(rowConstraints != NULL);
  NW_ASSERT(hasOnlyImages != NULL);

  NW_TRY(status) 
    {
    *hasOnlyImages = NW_TRUE;

    // Get the table width.
    NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds(thisObj->tableBox); 
    tableWidth = tableBoxBounds.dimension.width;

    // Init colConstraints
    NW_Mem_memset(rowConstraints, 0x0, sizeof(NW_Int32) * thisObj->numCols);

    // Determine the constraint for each column.
    cellCount = NW_LMgr_ContainerBox_GetChildCount(rowBox);
    if (cellCount < 1) 
      {
      NW_THROW_UNEXPECTED(status);
      }

    colIndex = 0;
    for (NW_Int32 cellIndex = 0; cellIndex < cellCount; cellIndex++) 
      {
      // Get the cell box.
      cellBox = NW_LMgr_ContainerBox_GetChild(rowBox, (NW_ADT_Vector_Metric_t) cellIndex);
      if (! NW_Object_IsInstanceOf(cellBox, &NW_LMgr_StaticTableCellBox_Class)) 
        {
        NW_THROW_UNEXPECTED(status);
        }

      // Get the ColSpan property on this cell box.
      prop.value.integer = 1;
      NW_LMgr_Box_GetPropertyFromList(cellBox, NW_CSS_Prop_colSpan, &prop);
      colSpan = (NW_ADT_Vector_Metric_t) prop.value.integer;      
      if (colSpan < 1)
        {
        colSpan = 1;
        }

      // Calculate the minimum cell size.
      status = NW_LMgr_StaticTableContext_GetCellBoxConstraint(thisObj, cellBox, tableWidth, 
          &width, &isAuto, &isImage);
      NW_THROW_ON_ERROR(status);

      // If the row contains anything other than an image the set hasOnlyImages to false.
      if (isImage == NW_FALSE)
        {
        *hasOnlyImages = NW_FALSE;
        }

      // Push this box into the next column if it is part of a rowspan, unless its
      // the first cell in the span.
      while (colIndex < thisObj->numCols) 
        {
        NW_Uint16  spanStart;
        NW_Uint16  colSpanSize;

        if (NW_LMgr_StaticTableContext_FindSpan(thisObj, (NW_Uint16) colIndex, 
            (NW_Uint16) rowIndex, NULL, &spanStart, &colSpanSize, NULL) && 
            (spanStart != rowIndex)) 
          {
          // Push it over one or more columns, more if it is also part of a colSpan.
          // rowConstraints[colIndex] is set to -1 so that this cell doesn't affect
          // the constraints calculation.
          for (NW_Int32 i = 0; i < colSpanSize; i++)
            {
            rowConstraints[colIndex] = -1;
            colIndex++;
            }
          }
        else
          {
          break;
          }
        }
      
      // The cell box won't fit on the row so ignore it and all remaining
      // cells in this row.
      if (colIndex == thisObj->numCols)
        {
        break;
        }

      // If the cell is an auto width cell update the auto width values.
      if (isAuto) 
        {
        autoColCount += colSpan;
        minAutoWidth += width;
        colIndex+= colSpan;
        }

      // Otherwise update the fixed width values.
      else 
        {
        fixedColWidth += width;

        for (NW_Int32 i = 0; i < colSpan; i++) 
          {
          rowConstraints[colIndex] = width / colSpan;
          colIndex++;
          }
        }
      }

    // Calculate the width for the auto columns.
    if (autoColCount > 0) 
      {
      autoColWidth = (NW_GDI_Metric_t)(minAutoWidth / autoColCount);

      // Set the width for the auto columns.
      for (colIndex = 0; colIndex < thisObj->numCols; colIndex++) 
        {
        if (rowConstraints[colIndex] == 0) 
          {
          rowConstraints[colIndex] = autoColWidth;
          }
        }
      }
    }

  NW_CATCH(status) 
    {
    }

  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_FindGridModeConstraints
 * Parameters:    thisObj - the object
 * Description:   Sets the column constraints such that they are appropriate
 *                for a grid mode table.
 * Returns:
 */
static
TBrowserStatusCode
NW_LMgr_StaticTableContext_FindGridModeConstraints(NW_LMgr_StaticTableContext_t* thisObj)
  {
  NW_LMgr_ContainerBox_t*  row = NULL;
  NW_Int32                 colIndex;
  NW_Int32                 tableWidth;
  NW_Int32                 naturalWidth = 0;
  NW_Int32                 imageOnlyRowNaturalWidth = 0;
  NW_Int32*                rowConstraints = NULL;
  NW_Int32*                iamgeRowConstraints = NULL;
  NW_Bool                  hasOnlyImages;
  NW_Bool                  hasImageOnlyRow = NW_FALSE;
  NW_Bool                  isSlicedImage = NW_FALSE;

  NW_ASSERT(thisObj != NULL);

  NW_TRY(status) 
    {
    thisObj->gridModeImageCount = 0;

    // Get the table width.
    NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds(thisObj->tableBox); 
    tableWidth = tableBoxBounds.dimension.width;

    // Create the arrays to hold the row constraints.
    rowConstraints = (NW_Int32*) NW_Mem_Malloc(sizeof(NW_Int32) * thisObj->numCols);
    NW_THROW_OOM_ON_NULL(rowConstraints, status);

    iamgeRowConstraints = (NW_Int32*) NW_Mem_Malloc(sizeof(NW_Int32) * thisObj->numCols);
    NW_THROW_OOM_ON_NULL(iamgeRowConstraints, status);

    // Init the necessary contraint arrays.
    NW_Mem_memset(thisObj->colConstraints, 0x0, sizeof(NW_Int32) * thisObj->numCols);
    NW_Mem_memset(iamgeRowConstraints, 0x0, sizeof(NW_Int32) * thisObj->numCols);

    for (NW_Int16 i = 0; i < GRID_MODE_COL_CONSTRAINT_ROW_COUNT; i++)
      {
      // Get the row to be used to calculate the column constraints.
      row = (NW_LMgr_ContainerBox_t*) NW_LMgr_ContainerBox_GetChild(
          (NW_LMgr_ContainerBox_t*) thisObj->tableBox, i);

      if (row != NULL)
        { 
        status = NW_LMgr_StaticTableContext_FindRowConstraints(thisObj, row, i, 
            rowConstraints, &hasOnlyImages);
        NW_THROW_ON_ERROR(status);

        // Track the largest constraint of each column.
        for (colIndex = 0; colIndex < thisObj->numCols; colIndex++)
          {
          if (rowConstraints[colIndex] > thisObj->colConstraints[colIndex])
            {
            thisObj->colConstraints[colIndex] = rowConstraints[colIndex];
            }
          }

        // If this row only contains images also track the largest image constraints 
        // as well.
        if (hasOnlyImages == NW_TRUE)
          {
          hasImageOnlyRow = NW_TRUE;

          for (colIndex = 0; colIndex < thisObj->numCols; colIndex++)
            {
            if (rowConstraints[colIndex] > iamgeRowConstraints[colIndex])
              {
              iamgeRowConstraints[colIndex] = rowConstraints[colIndex];
              }
            }
          }
        }

      // No more rows.
      else
        {
        break;
        }
      }

    // Determine the natural width of the table.
    naturalWidth = 0;
    for (colIndex = 0; colIndex < thisObj->numCols; colIndex++)
      {
      naturalWidth += thisObj->colConstraints[colIndex];
      }

    // Determine the natural width of image-only rows.
    if (hasImageOnlyRow == NW_TRUE)
      {
      imageOnlyRowNaturalWidth = 0;
      for (colIndex = 0; colIndex < thisObj->numCols; colIndex++)
        {
        imageOnlyRowNaturalWidth += iamgeRowConstraints[colIndex];
        }
      }

    // Sliced images are larger images that content authors split into multiple
    // pieces and use a table to assemble the bigger image.  Grid mode tables are
    // used to layout these images.  In most cases these tables only contain images,
    // but there are cases where an author may add a few cells that contain text.
    // To ensure that these text cells don't skew the image scaling it also tracks
    // the constraints of rows that just contain images -- colConstraints is then 
    // overridden by iamgeRowConstraints.
    NW_Int32 rowCount = thisObj->numRows;

    // Only consider the rows it evalulated.
    if (rowCount > GRID_MODE_COL_CONSTRAINT_ROW_COUNT)
      {
      rowCount = GRID_MODE_COL_CONSTRAINT_ROW_COUNT;
      }

    if ((hasImageOnlyRow == NW_TRUE) && 
        (imageOnlyRowNaturalWidth >= (naturalWidth * GRID_MODE_IMAGE_ONLY_WIDTH_TO_NORMAL_WIDTH_RATIO)) && 
        (thisObj->gridModeImageCount > (GRID_MODE_IMAGE_TO_CELL_RATIO * thisObj->numCols * rowCount)))
      {
      for (colIndex = 0; colIndex < thisObj->numCols; colIndex++)
        {
        thisObj->colConstraints[colIndex] = iamgeRowConstraints[colIndex];
        }

      naturalWidth = imageOnlyRowNaturalWidth;
      isSlicedImage = NW_TRUE;
      }

    if (naturalWidth == 0)
      {
      NW_THROW_UNEXPECTED(status);
      }

    // Normalize the column contrains given the natural widths and 
    // the width given to the table -- so it fit horizontally on the screen.
    // If table is laying out a sliced image don't normalize it if its 
    // width is less than the available table width.
    if ((isSlicedImage == NW_FALSE) || (naturalWidth > tableWidth))
      {
      for (colIndex = 0; colIndex < thisObj->numCols; colIndex++)
        {
        NW_Float32 temp;

        temp = (NW_Float32) thisObj->colConstraints[colIndex];
        temp = (temp / naturalWidth) * tableWidth;
  
        thisObj->colConstraints[colIndex] = (NW_Int32) temp;
        }
      }
    }

  NW_CATCH(status) 
    {
    }

  NW_FINALLY 
    {
    NW_Mem_Free(rowConstraints);
    NW_Mem_Free(iamgeRowConstraints);

    return status;
    } NW_END_TRY
  }

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_FindFixedConstraints
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */
static
TBrowserStatusCode
NW_LMgr_StaticTableContext_FindFixedConstraints( NW_LMgr_StaticTableContext_t* thisObj )
{
  NW_LMgr_ContainerBox_t *tableContainer;
  NW_ADT_Vector_Metric_t colSpan;
  NW_ADT_Vector_Metric_t i;
  NW_ADT_Vector_Metric_t cellCount;
  NW_ADT_Vector_Metric_t colIndex = 0;
  NW_ADT_Vector_Metric_t cellIndex = 0;
  NW_ADT_Vector_Metric_t autoColCount = 0;
  NW_LMgr_Property_t prop;
  NW_GDI_Metric_t width = 0;
  NW_GDI_Metric_t fixedColWidth = 0;
  NW_GDI_Metric_t minAutoWidth = 0;
  NW_GDI_Metric_t autoColWidth = 0;
  NW_GDI_Metric_t minWidth, tableWidth;
  NW_LMgr_Box_t *cellBox = NULL;
  NW_LMgr_Box_t *firstRow = NULL;
  NW_GDI_Dimension2D_t boxSize;

  /* Get the first row. */
  tableContainer = NW_LMgr_ContainerBoxOf(thisObj->tableBox);
  firstRow = NW_LMgr_ContainerBox_GetChild(tableContainer, 0);
  colIndex = 0;

  /* Look at the children and determine the constraints */
  cellCount = NW_LMgr_ContainerBox_GetChildCount(
                NW_LMgr_ContainerBoxOf(firstRow));
  NW_ASSERT (cellCount >= 1);

  for (cellIndex = 0; cellIndex < cellCount; cellIndex++) { 

    /* We assume that the table is regular, i.e. our only children
       should be table-cell boxes */
    cellBox = NW_LMgr_ContainerBox_GetChild (
                  NW_LMgr_ContainerBoxOf(firstRow), cellIndex);

    /* Get the colSpan */
    prop.value.integer = 1;
    NW_LMgr_Box_GetPropertyFromList (cellBox, NW_CSS_Prop_colSpan, &prop);
    colSpan = (NW_ADT_Vector_Metric_t)prop.value.integer;

    /* Calculate the minimum cell size */
    NW_LMgr_StaticTableCellBox_GetMinimumCellSize (
                  NW_LMgr_StaticTableCellBoxOf(cellBox), &boxSize);
    width = boxSize.width;

    /* If the cell has a specified width, add it to the colConstraints;
     * otherwise, remember it as an auto column */
    for (i = 0; i < colSpan; i++) {
      if (!NW_LMgr_Box_HasFixedWidth(cellBox)) {
        autoColCount++;
        minAutoWidth = (NW_GDI_Metric_t)(minAutoWidth + width);
      }
      else {
        thisObj->colConstraints[colIndex + i] = width;
        fixedColWidth = (NW_GDI_Metric_t)(fixedColWidth + width);
      }
    }

    /* Increment the colIndex */
    colIndex = (NW_ADT_Vector_Metric_t)(colIndex + colSpan);
  }

  /* Now we have all we need to know in order to constrain the columns.  
     First, we must check whether we can fit within the constraint.  
     The minimum size for the table is the total width of all
     fixed width columns + the minimum width of the auto columns */
  minWidth = (NW_GDI_Metric_t)(minAutoWidth + fixedColWidth);

  /* What's the minimum table width? */
  NW_GDI_Rectangle_t tableBoxBounds = NW_LMgr_Box_GetFormatBounds( thisObj->tableBox ); 
  tableWidth = tableBoxBounds.dimension.width;
  if (minWidth > tableWidth) 
    {
    /* Reset the table width */
    tableWidth = minWidth;
    }

  /* Calculate the width for the auto columns */
  if (autoColCount > 0) {
    autoColWidth = (NW_GDI_Metric_t)((tableWidth - fixedColWidth) / autoColCount);
  }

  /* Set the width for the auto columns */
  for (colIndex = 0; colIndex < thisObj->numCols; colIndex++) {
    if (thisObj->colConstraints[colIndex] == 0) {
      thisObj->colConstraints[colIndex] = autoColWidth;
    }
  }

  /* Update the table width */
  tableBoxBounds.dimension.width = (NW_GDI_Metric_t)
    (fixedColWidth + autoColWidth * autoColCount);
  NW_LMgr_Box_SetFormatBounds( thisObj->tableBox, tableBoxBounds ); 

  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_FindConstraints
 * Parameters:    thisObj - the object
 * Description:   It calls either the fixed or the automatic algorithm depending on 
 *                automaticAlgorithm value
 * Returns:       KBrsrSuccess if it the calculation was success.
 */
TBrowserStatusCode
NW_LMgr_StaticTableContext_FindConstraints( NW_LMgr_StaticTableContext_t* thisObj, NW_Bool automaticAlgorithm )
{
  TBrowserStatusCode status;

  NW_ASSERT( thisObj != NULL );

  if( automaticAlgorithm == NW_TRUE ) {
    status = NW_LMgr_StaticTableContext_FindAutomaticConstraints( thisObj );
  }
  else {
    NW_LMgr_Property_t  prop;

    status = NW_LMgr_Box_GetProperty( thisObj->tableBox, NW_CSS_Prop_gridModeApplied, &prop );

    if ( status == KBrsrSuccess ) {
      status = NW_LMgr_StaticTableContext_FindGridModeConstraints( thisObj );
    }
    else {
      status = NW_LMgr_StaticTableContext_FindFixedConstraints( thisObj );
    }
  }
  return status;  
}

static
void
NW_LMgr_StaticTableContext_ResetEdges (NW_LMgr_StaticTableContext_t* thisObj)
{
  NW_LMgr_StaticTableBorderInfo_t *borderInfo;
  NW_ADT_Vector_Metric_t numStyles, k;

  NW_ASSERT( thisObj != NULL );

  if (thisObj->borderInfos != NULL) {
    numStyles = NW_ADT_Vector_GetSize (thisObj->borderInfos);
    for (k = 0; k < numStyles; k++) {
      borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
        NW_ADT_Vector_ElementAt (thisObj->borderInfos, k);
      NW_Mem_Free(borderInfo);
    }
  }

  NW_Object_Delete(thisObj->borderInfos);
  NW_Object_Delete(thisObj->horizontalEdges);
  NW_Object_Delete(thisObj->verticalEdges);

  thisObj->borderInfos = NULL;
  thisObj->horizontalEdges = NULL;
  thisObj->verticalEdges = NULL;
}


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_StaticTableContext_Construct (NW_Object_Dynamic_t* dynamicObject,
                        va_list* argp);

/* ------------------------------------------------------------------------- */
const
NW_LMgr_StaticTableContext_Class_t NW_LMgr_StaticTableContext_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Dynamic_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		           */ sizeof (NW_LMgr_StaticTableContext_t),
    /* construct		             */ _NW_LMgr_StaticTableContext_Construct,
    /* destruct			             */ _NW_LMgr_StaticTableContext_Destruct
  },
  { /* NW_LMgr_StaticTableContext	 */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_LMgr_StaticTableContext_Construct (NW_Object_Dynamic_t* dynamicObject,
                        va_list* argp)
{
  NW_REQUIRED_PARAM(dynamicObject);
  NW_REQUIRED_PARAM(argp);
  return KBrsrSuccess;
}

void
_NW_LMgr_StaticTableContext_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_StaticTableContext_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_StaticTableContextOf (dynamicObject);

  NW_Mem_Free(thisObj->colConstraints);

  NW_Object_Delete(thisObj->spans);
  NW_LMgr_StaticTableContext_ResetEdges(thisObj);
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_Initialize
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

TBrowserStatusCode
NW_LMgr_StaticTableContext_Initialize (NW_LMgr_StaticTableContext_t* thisObj,
                                       NW_LMgr_Box_t* thisTable)
{
  NW_TRY (status) {

    /* If this is the first time initialize is called,
     * allocate and populate all the persistent data structures. */
    if (thisObj->tableBox == NULL) {
      thisObj->tableBox = thisTable;

      status = NW_LMgr_StaticTableContext_GetTableStats(thisObj);
      NW_THROW_ON_ERROR(status);
      
      status = NW_LMgr_StaticTableContext_AddLocationInfoToMap(thisObj);
      NW_THROW_ON_ERROR(status);

      status = NW_LMgr_StaticTableContext_GenerateBorderInfo(thisObj);
      NW_THROW_ON_ERROR(status);
    
      status = NW_LMgr_StaticTableContext_CompressBorderInfo(thisObj);
      NW_THROW_ON_ERROR(status);
    }

    /* Otherwise reset the border and constraints info, unless this is the
     * second (final) pass in NSM. This block is needed to support screen
     * mode switching on pages that contain grid mode tables.
     */
    else {
      NW_LMgr_StaticTableBox_t*  staticTableBox = NW_LMgr_StaticTableBoxOf(thisTable);

      if (staticTableBox->automaticWidthPass != NW_LMgr_StaticTableBox_AutomaticWidth_FinalPass) {
        NW_LMgr_StaticTableContext_ResetEdges(thisObj);

        status = NW_LMgr_StaticTableContext_GenerateBorderInfo(thisObj);
        NW_THROW_ON_ERROR(status);
    
        status = NW_LMgr_StaticTableContext_CompressBorderInfo(thisObj);
        NW_THROW_ON_ERROR(status);
      }
    }

    /* Allocate the temporary colConstraints array */
    if (thisObj->colConstraints)
    { 
        NW_Mem_Free(thisObj->colConstraints);
    }
    thisObj->colConstraints = (NW_GDI_Metric_t*) NW_Mem_Malloc(
           sizeof(NW_GDI_Metric_t) * NW_LMgr_StaticTableContext_GetNumCols(thisObj));
    
    NW_THROW_OOM_ON_NULL(thisObj->colConstraints, status);
    NW_Mem_memset(thisObj->colConstraints, 0, sizeof(NW_GDI_Metric_t) * NW_LMgr_StaticTableContext_GetNumCols(thisObj));

    /* Initialize the formatting state vars */
    thisObj->y = 0;
    thisObj->leftMargin = 0;
    thisObj->contentHeight = 0;
      
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_Cleanup
 * Parameters:    thisObj - the object
 * Description:   Cleans up temporary variables used during layout.  This method 
 *                should be called after every layout.
 * Returns:
 */

TBrowserStatusCode
NW_LMgr_StaticTableContext_Cleanup (NW_LMgr_StaticTableContext_t* thisObj)
{
  thisObj->y = 0;
  thisObj->leftMargin = 0;
  thisObj->contentHeight = 0;
  NW_Mem_Free(thisObj->colConstraints);
  thisObj->colConstraints = NULL;

  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_LxLyToPxPy
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */
TBrowserStatusCode
NW_LMgr_StaticTableContext_LxLyToPxPy(NW_LMgr_StaticTableContext_t *thisObj, 
                                      NW_Uint16 lx, NW_Uint16 ly,
                                      NW_Uint16 *px, NW_Uint16 *py,
                                      NW_Uint16 *colSpan, NW_Uint16 *rowSpan)
{
  NW_Uint32 *mapEntry = 0;
  NW_Uint16 map_px;
  NW_Uint16 map_py = 0;
  NW_Uint16 map_colSpan;
  NW_Uint16 map_rowSpan;
  NW_Uint16 col;
  NW_Uint16 row;
  NW_Uint16 cell;
  NW_ADT_Vector_Metric_t vectorSize;
  NW_ADT_Vector_Metric_t index;
  NW_Bool done = NW_FALSE;
  NW_Bool found = NW_FALSE;

  /* Set the default values */
  /* Go through any mapped spans */
  if (thisObj->spans == NULL) {
    *px = lx;
    *py = ly;
    if (colSpan) {
      *colSpan = 1;
    }
    if (rowSpan) {
      *rowSpan = 1;
    }
  }
  else {

    vectorSize = NW_ADT_Vector_GetSize(thisObj->spans);
    col = 0;
    row = ly;
    cell = 0;
    index = 0;

    while (!done) {

      found = NW_FALSE;
      for (; index < vectorSize; index++) {

        mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->spans, index);
        NW_ASSERT(mapEntry);

        map_px = NW_LMGR_STATICTABLESPANENTRY_GETPX(*mapEntry);
        map_py = NW_LMGR_STATICTABLESPANENTRY_GETPY(*mapEntry);

        /* If we have passed our vert position, we're done */
        if (map_py > ly) {
          break;
        }        
        if (map_px == col) {
          found = NW_TRUE;
          break;
        }
      }

      if (found) {
        map_rowSpan = NW_LMGR_STATICTABLESPANENTRY_GETROWSPAN(*mapEntry);
        map_colSpan = NW_LMGR_STATICTABLESPANENTRY_GETCOLSPAN(*mapEntry);

        /* Check whether (px,py) falls within this cell */
        if (row == map_py) {
          if (cell < lx) {
            cell ++;
            col = NW_UINT16_CAST(col + map_colSpan);
            index = 0;
          }
          else {
            *px = col;
            *py = row;
            if (colSpan) {
              *colSpan = map_colSpan;
            }
            if (rowSpan) {
              *rowSpan = map_rowSpan;
            }
            done = NW_TRUE;
            break;
          }
        }
        else if ((row > map_py) && (row < map_py + map_rowSpan)) {
          col = NW_UINT16_CAST(col + map_colSpan);
          index = 0;
        }
        else {
          index++;
        }
      }
      else {
        if (cell == lx) {
          *px = col;
          *py = row;
          if (colSpan) {
            *colSpan = 1;
          }
          if (rowSpan) {
            *rowSpan = 1;
          }
          done = NW_TRUE;
          break;
        }
        else {
          cell++;
          col++;
          index = 0;
        }
      }
    }
  }

  if (*px >= thisObj->numCols || *py >= thisObj->numRows) {
    return KBrsrNotFound;
  }
  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_PxPyToLxLy
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */
TBrowserStatusCode
NW_LMgr_StaticTableContext_PxPyToLxLy(NW_LMgr_StaticTableContext_t *thisObj, 
                                      NW_Uint16 px, NW_Uint16 py,
                                      NW_Uint16 *lx, NW_Uint16 *ly)
{
  NW_Uint32 *mapEntry;
  NW_Uint16 map_px;
  NW_Uint16 map_py;
  NW_Uint16 map_colSpan;
  NW_Uint16 map_rowSpan;
  NW_Uint16 colCount = 0;
  NW_Uint16 spanx, spany;
  NW_ADT_Vector_Metric_t vectorSize;
  NW_ADT_Vector_Metric_t index;

  NW_TRY(status) {

    /* Are the locations within range? */
    if (px >= thisObj->numCols || py >= thisObj->numRows) {
      NW_THROW_STATUS(status, KBrsrNotFound);
    }

    if (thisObj->spans) {

      if (NW_LMgr_StaticTableContext_FindSpan(thisObj, px, py, &spanx, &spany, NULL, NULL)) {
        px = spanx;
        py = spany;
      }

      *lx = 0;
      colCount = 0;

      vectorSize = NW_ADT_Vector_GetSize(thisObj->spans);

      for (index = 0; index < vectorSize; index++) {

        mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->spans, index);

        map_px = NW_LMGR_STATICTABLESPANENTRY_GETPX(*mapEntry);
        map_py = NW_LMGR_STATICTABLESPANENTRY_GETPY(*mapEntry);

        /* If we have passed our vert position, we're done */
        if (map_py > py) {
          break;
        }

        map_rowSpan = NW_LMGR_STATICTABLESPANENTRY_GETROWSPAN(*mapEntry);
        map_colSpan = NW_LMGR_STATICTABLESPANENTRY_GETCOLSPAN(*mapEntry);

        if (py == map_py) {
          if (px <= map_px) {
            break;
          }
          (*lx)++;
          colCount = NW_UINT16_CAST(colCount + map_colSpan);
        }
        else if ((py > map_py) && (py < map_py + map_rowSpan) && (px >= map_px) ) {
          colCount = NW_UINT16_CAST(colCount + map_colSpan);
        }
      }

      /* illegal span case: overlapped span in map */      
	  if (colCount > (*lx + px))
	  {
		  *lx = 0;
	  } else
	  {
      *lx = NW_UINT16_CAST(*lx + px - colCount);
	  }
      *ly = py;
    }
    else {
      *lx = px;
      *ly = py;
    }

  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_GetCellPtrFromLxLy
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellPtrFromLxLy(NW_LMgr_StaticTableContext_t *thisObj, 
                                              NW_Uint16 lx, NW_Uint16 ly,
                                              NW_LMgr_Box_t **cell)
{
  NW_LMgr_Box_t* rowBox;
  NW_LMgr_ContainerBox_t* tableContainer = 
                            NW_LMgr_ContainerBoxOf(thisObj->tableBox);
  NW_LMgr_ContainerBox_t* rowContainer;

  rowBox = NW_LMgr_ContainerBox_GetChild(tableContainer, ly);
  if (rowBox != NULL) {
    rowContainer = NW_LMgr_ContainerBoxOf(rowBox);
    *cell = NW_LMgr_ContainerBox_GetChild(rowContainer, lx);
  }

  if (*cell == NULL || rowBox == NULL) {
    return KBrsrNotFound;
  }
  else {  
    return KBrsrSuccess;
  }
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_GetCellLxLyFromPtr
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellLxLyFromPtr(NW_LMgr_StaticTableContext_t *thisObj, 
                                              NW_LMgr_Box_t* cellBox,
                                              NW_Uint16 *lx, NW_Uint16 *ly)
{
  NW_LMgr_ContainerBox_t* rowContainer = cellBox->parent;
  NW_LMgr_Box_t* rowBox = NW_LMgr_BoxOf(rowContainer);
  NW_LMgr_ContainerBox_t* tableContainer = NULL;
  
    if (thisObj == NULL) {		
     return KBrsrNotFound;
  }

  tableContainer = NW_LMgr_ContainerBoxOf(thisObj->tableBox);

  *lx = NW_LMgr_ContainerBox_GetChildIndex(rowContainer, cellBox);
  *ly = NW_LMgr_ContainerBox_GetChildIndex(tableContainer, rowBox);

  if (*lx == NW_ADT_Vector_AtEnd || *ly == NW_ADT_Vector_AtEnd) {
    return KBrsrNotFound;
  }  else {  
    return KBrsrSuccess;
  }
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_GetCellPtrFromPxPy
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */

TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellPtrFromPxPy(NW_LMgr_StaticTableContext_t *thisObj, 
                                              NW_Uint16 px, NW_Uint16 py,
                                              NW_LMgr_Box_t** cellBox)
{
  NW_Uint16 lx, ly;

  NW_TRY(status) {

    status = NW_LMgr_StaticTableContext_PxPyToLxLy(thisObj, px, py, &lx, &ly);
    NW_THROW_ON_ERROR(status);

    status = NW_LMgr_StaticTableContext_GetCellPtrFromLxLy (thisObj, lx, ly, cellBox);

  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* -------------------------------------------------------------------------
 * Function:      NW_LMgr_StaticTableContext_GetCellBordersFromPtr
 * Parameters:    thisObj - the object
 * Description:   
 * Returns:
 */
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellBordersFromPtr(
                                         NW_LMgr_StaticTableContext_t *thisObj, 
                                         NW_LMgr_Box_t *cellBox,
                                         NW_LMgr_FrameInfo_t *widthInfo,
                                         NW_LMgr_FrameInfo_t *styleInfo,
                                         NW_LMgr_FrameInfo_t *colorInfo)
{
  //lint --e{704} Shift right of signed quantity

  NW_Uint32 *mapEntry;
  NW_Uint16 colSpan = 1;
  NW_Uint16 rowSpan = 1;
  NW_Uint16 lx, ly;
  NW_Uint16 px, py;
  NW_Uint16 map_px1, map_px2, map_px;
  NW_Uint16 map_py1, map_py2, map_py;
  NW_Uint16 topInfo, bottomInfo, leftInfo, rightInfo;
  NW_ADT_Vector_Metric_t vectorSize, index;
  NW_LMgr_FrameInfo_t dummy;
  NW_LMgr_StaticTableBorderInfo_t *borderInfo;
  NW_LMgr_Box_t* tempBox = NULL;
  NW_LMgr_PropertyValue_t visibilityVal;

  NW_TRY(status) {

    status = NW_LMgr_StaticTableContext_GetCellLxLyFromPtr(thisObj, cellBox, &lx, &ly);
    _NW_THROW_ON_ERROR(status);

    status = NW_LMgr_StaticTableContext_LxLyToPxPy(thisObj, lx, ly, 
                                                   &px, &py, 
                                                   &colSpan, &rowSpan);
    if (status == KBrsrNotFound) {
      NW_THROW(status);
    }
    _NW_THROW_ON_ERROR(status);

    /* Set the default value */
    topInfo = bottomInfo = leftInfo = rightInfo = thisObj->defaultInfo;

    /* Find the top and bottom edges */
    if (thisObj->horizontalEdges != NULL) {
      vectorSize = NW_ADT_Vector_GetSize(thisObj->horizontalEdges);

      for (index = 0; index < vectorSize; index++) {
        mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->horizontalEdges, index);

        map_px1 = NW_LMGR_STATICTABLEHEDGEENTRY_GETPX1(*mapEntry);
        map_px2 = NW_LMGR_STATICTABLEHEDGEENTRY_GETPX2(*mapEntry);
        map_py = NW_LMGR_STATICTABLEHEDGEENTRY_GETPY(*mapEntry);

        if ((px >= map_px1) && (px < map_px2)) {
          if (py == map_py) {
            topInfo = NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(*mapEntry);
          }
          else if (py + rowSpan == map_py) {
            bottomInfo = NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(*mapEntry);
          }
        }
      }
    }
    
    /* Find the left and right edges */
    if (thisObj->verticalEdges != NULL) {
      vectorSize = NW_ADT_Vector_GetSize(thisObj->verticalEdges);

      for (index = 0; index < vectorSize; index++) {
        mapEntry = (NW_Uint32*)NW_ADT_Vector_ElementAt(thisObj->verticalEdges, index);

        map_px = NW_LMGR_STATICTABLEVEDGEENTRY_GETPX(*mapEntry);
        map_py1 = NW_LMGR_STATICTABLEVEDGEENTRY_GETPY1(*mapEntry);
        map_py2 = NW_LMGR_STATICTABLEVEDGEENTRY_GETPY2(*mapEntry);

        if ((py >= map_py1) && (py < map_py2)) {
          if (px == map_px) {
            leftInfo = NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(*mapEntry);
          }
          else if (px + colSpan == map_px) {
            rightInfo = NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(*mapEntry);
          }
        }
      }
    }    

    if (widthInfo == NULL) {
      widthInfo = &dummy;
    }
    if (styleInfo == NULL) {
      styleInfo = &dummy;
    }
    if (colorInfo == NULL) {
      colorInfo = &dummy;
    }

    /* Get left border */
    borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
      NW_ADT_Vector_ElementAt(thisObj->borderInfos, leftInfo);
    NW_ASSERT (borderInfo != NULL);

    if (px == 0) {
      widthInfo->left = borderInfo->edge.width;
    }
    else {
      status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy(thisObj, (NW_Uint16)(px - colSpan), py, &tempBox);
      // If the visibility val is set to hidden, don't draw the 
      // Container -- but still try to draw the children 
      visibilityVal.token = NW_CSS_PropValue_visible;
      if(status == KBrsrSuccess && tempBox != NULL)
      {
        (void) NW_LMgr_Box_GetPropertyValue (tempBox, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
      }
      if(visibilityVal.token == NW_CSS_PropValue_visible)
      {
        widthInfo->left = NW_LMGR_CEIL_DIV2(borderInfo->edge.width);
      }
      else
      {
        widthInfo->left = borderInfo->edge.width;
      }
    }
    styleInfo->left = borderInfo->edge.style;
    colorInfo->left = borderInfo->edge.color;

    /* Get right border */
    borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
      NW_ADT_Vector_ElementAt(thisObj->borderInfos, rightInfo);
    NW_ASSERT (borderInfo != NULL);

    if (px + colSpan == thisObj->numCols) {
      widthInfo->right = borderInfo->edge.width;
    }
    else {

      status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy(thisObj, (NW_Uint16)(px + colSpan ), py, &tempBox);
      // If the visibility val is set to hidden, don't draw the 
      // Container -- but still try to draw the children 
      visibilityVal.token = NW_CSS_PropValue_visible;
      if(status == KBrsrSuccess && tempBox != NULL)
      {
        (void) NW_LMgr_Box_GetPropertyValue (tempBox, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
      }
      if(visibilityVal.token == NW_CSS_PropValue_visible)
      {
        widthInfo->right = NW_LMGR_FLOOR_DIV2(borderInfo->edge.width);
      }
      else
      {
        widthInfo->right = borderInfo->edge.width;
      }
    }
    styleInfo->right = borderInfo->edge.style;
    colorInfo->right = borderInfo->edge.color;

    /* Get top border */
    borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
      NW_ADT_Vector_ElementAt(thisObj->borderInfos, topInfo);
    NW_ASSERT (borderInfo != NULL);

    if (py == 0) {
      widthInfo->top = borderInfo->edge.width;
    }
    else {
      status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy(thisObj, px, (NW_Uint16)(py - rowSpan), &tempBox);
      // If the visibility val is set to hidden, don't draw the 
      // Container -- but still try to draw the children 
      visibilityVal.token = NW_CSS_PropValue_visible;
      if(status == KBrsrSuccess && tempBox != NULL)
      {
        (void) NW_LMgr_Box_GetPropertyValue (tempBox, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
      }
      if(visibilityVal.token == NW_CSS_PropValue_visible)
      {
        widthInfo->top = NW_LMGR_CEIL_DIV2(borderInfo->edge.width);
      }
      else
      {
        widthInfo->top = borderInfo->edge.width;
      }
    }
    styleInfo->top = borderInfo->edge.style;
    colorInfo->top = borderInfo->edge.color;

    /* Get bottom border */
    borderInfo = *(NW_LMgr_StaticTableBorderInfo_t**)
      NW_ADT_Vector_ElementAt(thisObj->borderInfos, bottomInfo);
    NW_ASSERT (borderInfo != NULL);

    if (py + rowSpan == thisObj->numRows) {
      widthInfo->bottom = borderInfo->edge.width;
    }
    else {
      status = NW_LMgr_StaticTableContext_GetCellPtrFromPxPy(thisObj, px, (NW_Uint16)(py + rowSpan ), &tempBox);
      // If the visibility val is set to hidden, don't draw the 
      // Container -- but still try to draw the children 
      visibilityVal.token = NW_CSS_PropValue_visible;
      if(status == KBrsrSuccess && tempBox != NULL)
      {
        (void) NW_LMgr_Box_GetPropertyValue (tempBox, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
      }
      if(visibilityVal.token == NW_CSS_PropValue_visible)
      {
        widthInfo->bottom = NW_LMGR_FLOOR_DIV2(borderInfo->edge.width);
      }
      else
      {
        widthInfo->bottom = borderInfo->edge.width;
      }
    }
    styleInfo->bottom = borderInfo->edge.style;
    colorInfo->bottom = borderInfo->edge.color;
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY    
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

NW_LMgr_StaticTableContext_t*
NW_LMgr_StaticTableContext_New ()
{
  return (NW_LMgr_StaticTableContext_t*)
    NW_Object_New (&NW_LMgr_StaticTableContext_Class);
}
