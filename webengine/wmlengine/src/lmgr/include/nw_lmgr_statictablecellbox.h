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


#ifndef NW_STATICTABLECELLBOX_H
#define NW_STATICTABLECELLBOX_H

#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_statictablebox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


typedef struct NW_LMgr_StaticTableCellBox_s NW_LMgr_StaticTableCellBox_t;

typedef struct NW_LMgr_StaticTableCellBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_StaticTableCellBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableCellBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  NW_LMgr_StaticTableCellBox_ClassPart_t NW_LMgr_StaticTableCellBox;
} NW_LMgr_StaticTableCellBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_StaticTableCellBox_s {
  NW_LMgr_BidiFlowBox_t super;
  
  /* member variables */
  NW_GDI_Metric_t minWidth;
  NW_GDI_Metric_t maxWidth; 
  NW_GDI_Rectangle_t cellClip;
 
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_StaticTableCellBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_StaticTableCellBox))


#define NW_LMgr_StaticTableCellBoxOf(_object) \
((NW_LMgr_StaticTableCellBox_t*) (_object))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_StaticTableCellBox_Class_t NW_LMgr_StaticTableCellBox_Class;

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */

NW_LMGR_EXPORT
void
NW_LMgr_StaticTableCellBox_GetPadding(NW_LMgr_StaticTableCellBox_t *thisObj,
                                      NW_LMgr_FrameInfo_t *paddingInfo);
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_CalculateBorders(NW_LMgr_StaticTableCellBox_t *thisObj,
                                            NW_Uint16 currentCol, NW_Uint16 currentRow, 
                                            NW_Uint16 colSpan, NW_Uint16 rowSpan,
                                            NW_LMgr_StaticTableBorderEdge_t *leftEdge,
                                            NW_LMgr_StaticTableBorderEdge_t *rightEdge,
                                            NW_LMgr_StaticTableBorderEdge_t *topEdge,
                                            NW_LMgr_StaticTableBorderEdge_t *bottomEdge);
NW_LMGR_EXPORT
void
NW_LMgr_StaticTableCellBox_GetBackground(NW_LMgr_StaticTableCellBox_t *thisObj,
                                         NW_LMgr_Property_t *backgroundProp);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_DrawCell(NW_LMgr_StaticTableCellBox_t *thisObj,
                                    CGDIDeviceContext *deviceContext,
                                    NW_Bool hasFocus,
                                    NW_GDI_Rectangle_t *viewBounds,
                                    NW_GDI_Rectangle_t *clipRect);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_Stretch (NW_LMgr_StaticTableCellBox_t *thisObj, 
                                    NW_GDI_Rectangle_t *newRect);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableCellBox_GetMinimumCellSize(NW_LMgr_StaticTableCellBox_t *thisObj,
                                              NW_GDI_Dimension2D_t *minSize);
/* ------------------------------------------------------------------------- */
#define NW_LMgr_StaticTableCellBox_GetMaxWidth( _cellBox ) \
  ((_cellBox)->maxWidth)
#define NW_LMgr_StaticTableCellBox_SetMaxWidth( _cellBox, _maxWidth ) \
  ((_cellBox)->maxWidth = (_maxWidth))
#define NW_LMgr_StaticTableCellBox_GetMinWidth( _cellBox ) \
  ((_cellBox)->minWidth)
#define NW_LMgr_StaticTableCellBox_SetMinWidth( _cellBox, _minWidth ) \
  ((_cellBox)->minWidth = (_minWidth))


NW_LMGR_EXPORT
NW_LMgr_StaticTableCellBox_t*
NW_LMgr_StaticTableCellBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_STATICTABLECELLBOX_H */

