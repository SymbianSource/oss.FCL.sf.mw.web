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


#ifndef NW_LMGR_STATICTABLECONTEXT_H
#define NW_LMGR_STATICTABLECONTEXT_H

#include "nw_object_dynamic.h"
#include "nw_lmgr_box.h"
#include "nw_adt_resizablevector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableContext_Class_s NW_LMgr_StaticTableContext_Class_t;
typedef struct NW_LMgr_StaticTableContext_s NW_LMgr_StaticTableContext_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableContext_ClassPart_s {
  void** unused;
} NW_LMgr_StaticTableContext_ClassPart_t;

struct NW_LMgr_StaticTableContext_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_StaticTableContext_ClassPart_t NW_LMgr_StaticTableContext;
};

/* ------------------------------------------------------------------------- */
typedef enum NW_LMgr_StaticTableBorderSide_e{
  left = 0,
  right,
  top,
  bottom
} NW_LMgr_StaticTableBorderSide_t;

typedef struct NW_LMgr_StaticTableBorderEdge_s {
  NW_Int32 width;
  NW_Int32 style;
  NW_Int32 color;
} NW_LMgr_StaticTableBorderEdge_t;

#define NW_LMGR_STATICTABLECONTEXT_CMPEDGE(e1, e2) \
  ((e1.width == e2.width) && (e1.style == e2.style) && (e1.color == e2.color))

/* ------------------------------------------------------------------------- */

/* The NW_LMgr_StaticTableMapEntry_t corresponds to the following
 * bit field:

union NW_LMgr_StaticTableMapEntry_u {
  
  struct {
    unsigned py            : 9;
    unsigned px            : 9;
    unsigned colSpan       : 7;
    unsigned rowSpan       : 7;
  } data;

  NW_Uint32 hash;
};

*/

#define NW_LMGR_STATICTABLESPANENTRY_GETPY(_hash) \
  (NW_Uint16)(((NW_Uint32)(0xff800000) & (_hash)) >> 23)
#define NW_LMGR_STATICTABLESPANENTRY_GETPX(_hash) \
  (NW_Uint16)(((NW_Uint32)(0x007fc000) & (_hash)) >> 14)
#define NW_LMGR_STATICTABLESPANENTRY_GETROWSPAN(_hash) \
  (NW_Uint16)(((NW_Uint32)(0x00003f80) & (_hash)) >> 7)
#define NW_LMGR_STATICTABLESPANENTRY_GETCOLSPAN(_hash) \
  (NW_Uint16)((NW_Uint32)(0x0000007f) & (_hash))

#define NW_LMGR_STATICTABLESPANENTRY_INITIALIZE(_hash, _px, _py, _colSpan, _rowSpan) \
  ((_hash) = ((0x1ff & (_py)) << 23) | ((0x1ff & (_px)) << 14) \
             | ((0x7f & (_rowSpan)) << 7) | (0x7f & (_colSpan)))

/* This is the BorderInfo structure that is used to store a calculated border
 * setting.  It is referenced by the StaticTableBorderEntry structure above.
 */
typedef struct NW_LMgr_StaticTableBorderInfo_s NW_LMgr_StaticTableBorderInfo_t;

struct NW_LMgr_StaticTableBorderInfo_s {
  NW_LMgr_StaticTableBorderEdge_t edge;
  NW_Uint16 count;
};
  

/* ------------------------------------------------------------------------- */
#define NW_LMGR_STATICTABLEHEDGEENTRY_GETPY(_hash) \
  (NW_Uint16)(((NW_Uint32)(0xff800000) & (_hash)) >> 23)
#define NW_LMGR_STATICTABLEHEDGEENTRY_GETPX1(_hash) \
  (NW_Uint16)(((NW_Uint32)(0x007fc000) & (_hash)) >> 14)
#define NW_LMGR_STATICTABLEHEDGEENTRY_GETPX2(_hash) \
  (NW_Uint16)(((NW_Uint32)(0x00003fe0) & (_hash)) >> 5)
#define NW_LMGR_STATICTABLEHEDGEENTRY_GETINFO(_hash) \
  (NW_Uint16)((NW_Uint32)(0x0000001f) & (_hash))
#define NW_LMGR_STATICTABLEHEDGEENTRY_SETPX2(_hash, _px2) \
  (_hash = ((_hash & 0xffffc01f) | (((_px2) & 0x1ff) << 5)))

#define NW_LMGR_STATICTABLEHEDGEENTRY_INITIALIZE(_hash, _py, _px1, _px2, _info) \
  ((_hash) = ((0x1ff & (_py)) << 23) | ((0x1ff & (_px1)) << 14) \
             | ((0x1ff & (_px2)) << 5) | (0x1f & (_info)))

#define NW_LMGR_STATICTABLEVEDGEENTRY_GETPX(_hash) \
  (NW_Uint16)(((NW_Uint32)(0xff800000) & (_hash)) >> 23)
#define NW_LMGR_STATICTABLEVEDGEENTRY_GETPY1(_hash) \
  (NW_Uint16)(((NW_Uint32)(0x007fc000) & (_hash)) >> 14)
#define NW_LMGR_STATICTABLEVEDGEENTRY_GETPY2(_hash) \
  (NW_Uint16)(((NW_Uint32)(0x00003fe0) & (_hash)) >> 5)
#define NW_LMGR_STATICTABLEVEDGEENTRY_GETINFO(_hash) \
  (NW_Uint16)((NW_Uint32)(0x0000001f) & (_hash))
#define NW_LMGR_STATICTABLEVEDGEENTRY_SETPY2(_hash, _py2) \
  (_hash = ((_hash & (NW_Uint32)(0xffffc01f)) | (((_py2) & 0x1ff) << 5)))

#define NW_LMGR_STATICTABLEVEDGEENTRY_INITIALIZE(_hash, _px, _py1, _py2, _info) \
  ((_hash) = ((0x1ff & (_px)) << 23) | ((0x1ff & (_py1)) << 14) \
             | ((0x1ff & (_py2)) << 5) | (0x1f & (_info)))

/* ------------------------------------------------------------------------- */
struct NW_LMgr_StaticTableContext_s {

  /* Base class */
  NW_Object_Dynamic_t super;
  
  /* The dimensions of the table */
  NW_ADT_Vector_Metric_t numRows, numCols;

  /* Convenience pointers */
  NW_LMgr_Box_t *tableBox;

  /* Grid mode */
  NW_Int32 gridModeImageCount;

  /* Table map */
  NW_ADT_ResizableVector_t *spans;
  NW_ADT_ResizableVector_t *borderInfos;
  NW_ADT_ResizableVector_t *horizontalEdges;
  NW_ADT_ResizableVector_t *verticalEdges;
  NW_Uint16 defaultInfo;

  /* Temporary format vars */
  NW_GDI_Metric_t y;
  NW_GDI_Metric_t leftMargin;
  NW_GDI_Metric_t contentHeight;
  NW_GDI_Metric_t* colConstraints;

  NW_GDI_Metric_t tableMinWidth;
  NW_GDI_Metric_t tableMaxWidth;



};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_StaticTableContext_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_StaticTableContext))

#define NW_LMgr_StaticTableContextOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_StaticTableContext))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_StaticTableContext_Class_t NW_LMgr_StaticTableContext_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_Initialize (NW_LMgr_StaticTableContext_t* thisObj,
                                       NW_LMgr_Box_t* tableBox);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_Cleanup (NW_LMgr_StaticTableContext_t* thisObj);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_LxLyToPxPy(NW_LMgr_StaticTableContext_t *thisObj, 
                                      NW_Uint16 lx, NW_Uint16 ly,
                                      NW_Uint16 *px, NW_Uint16 *py,
                                      NW_Uint16 *colSpan, NW_Uint16 *rowSpan);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_PxPyToLxLy(NW_LMgr_StaticTableContext_t *thisObj, 
                                      NW_Uint16 px, NW_Uint16 py,
                                      NW_Uint16 *lx, NW_Uint16 *ly);

#define NW_LMgr_StaticTableContext_GetCellPxPyFromLxLy(_thisObj, _lx, _ly, _px, _py, _colSpan, _rowSpan) \
  NW_LMgr_StaticTableContext_LxLyToPxPy(_thisObj, _lx, _ly, _px, _py, _colSpan, _rowSpan)

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellPtrFromPxPy(NW_LMgr_StaticTableContext_t *thisObj, 
                                              NW_Uint16 px, NW_Uint16 py,
                                              NW_LMgr_Box_t** cellBox);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellPxPyAndSpansFromPtr(NW_LMgr_StaticTableContext_t *thisObj, 
                                                      NW_LMgr_Box_t* cellBox,
                                                      NW_Uint16 *px, NW_Uint16 *py,
                                                      NW_Uint16 *colSpan, NW_Uint16 *rowSpan);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_GetCellBordersFromPtr(
                                         NW_LMgr_StaticTableContext_t *thisObj, 
                                         NW_LMgr_Box_t *cellBox,
                                         NW_LMgr_FrameInfo_t *widthInfo,
                                         NW_LMgr_FrameInfo_t *styleInfo,
                                         NW_LMgr_FrameInfo_t *colorInfo);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableContext_FindConstraints (NW_LMgr_StaticTableContext_t* thisObj, NW_Bool automaticAlgorithm );

/* ------------------------------------------------------------------------- *
   accessor methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_StaticTableContext_GetNumCols(_ctx) \
  ((_ctx)->numCols)
#define NW_LMgr_StaticTableContext_SetNumCols(_ctx, _numCols) \
  ((_ctx)->numCols = (_numCols))
#define NW_LMgr_StaticTableContext_GetNumRows(_ctx) \
  ((_ctx)->numRows)
#define NW_LMgr_StaticTableContext_SetNumRows(_ctx, _numRows) \
  ((_ctx)->numRows = (_numRows))
#define NW_LMgr_StaticTableContext_GetColConstraint(_ctx, _col) \
  ((_ctx)->colConstraints[_col])
#define NW_LMgr_StaticTableContext_SetColConstraint(_ctx, _col, _val) \
  ((_ctx)->colConstraints[_col] = (_val))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

NW_LMGR_EXPORT
NW_LMgr_StaticTableContext_t*
NW_LMgr_StaticTableContext_New ();

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_BOXVISITOR_H */
