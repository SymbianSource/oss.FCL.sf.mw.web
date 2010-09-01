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


#ifndef NW_POSFLOWBOX_H
#define NW_POSFLOWBOX_H

#include "nw_lmgr_flowbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Pos Flow boxes keep track the saved positioned boxes */

typedef struct NW_LMgr_PosFlowBox_s NW_LMgr_PosFlowBox_t;

typedef struct NW_LMgr_PosFlowBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_PosFlowBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_PosFlowBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  NW_LMgr_PosFlowBox_ClassPart_t NW_LMgr_PosFlowBox;
} NW_LMgr_PosFlowBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_PosFlowBox_s {
  NW_LMgr_FlowBox_t super;
  NW_Int32 iLeft;
  NW_Int32 iRight;
  NW_Int32 iTop;
  NW_Int32 iBottom;
  NW_Bool iHasLeft;
  NW_Bool iHasRight;
  NW_Bool iHasTop;
  NW_Bool iHasBottom;
  NW_LMgr_Box_t* iContainingBlock;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_PosFlowBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_PosFlowBox))


#define NW_LMgr_PosFlowBoxOf(_object) \
((NW_LMgr_PosFlowBox_t*) (_object))

#define NW_LMgr_PosFlowBox_Split(_object, _space, _split, _flags) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).split ((_object), (_space), (_split), (_flags)))

#define NW_LMgr_PosFlowBox_Resize(_object, _context) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).resize ((_object),(_context)))

#define NW_LMgr_PosFlowBox_PostResize(_object) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).postResize (_object))


#define NW_LMgr_PosFlowBox_GetMinimumContentSize(_object, _size) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).getMinimumContentSize ((_object), (_size)))

#define NW_LMgr_PosFlowBox_HasFixedContentSize(_object) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).hasFixedContentSize (_object))

#define NW_LMgr_PosFlowBox_Constrain(_object, _constraint) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).constrain ((_object), (_constraint)))

#define NW_LMgr_PosFlowBox_Draw(_object, _deviceContext, _hasFocus) \
  (NW_LMgr_PosFlowBox_GetClassPart(_object). \
     draw ((_object), (_deviceContext), (_hasFocus)))

#define NW_LMgr_PosFlowBox_Render(_object, _deviceContext, _clipRect, _currentBox, _flags, _hasFocus, _skipChildren, _rootBox ) \
  (NW_LMgr_PosFlowBox_GetClassPart(_object). \
     render ((_object), (_deviceContext), (_clipRect), (_currentBox), (_flags), (_hasFocus), (_skipChildren), (_rootBox) ))

#define NW_LMgr_PosFlowBox_GetBaseline(_object) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).getBaseline (_object))

#define NW_LMgr_PosFlowBox_Shift(_object, _shift) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).shift ((_object), (_shift)))

#define NW_LMgr_PosFlowBox_Clone(_object) \
  (NW_LMgr_PosFlowBox_GetClassPart (_object).clone ((_object)))

#define NW_LMgr_PosFlowtBox_ApplyFormatProps(_object, _context) \
  (NW_LMgr_PosFlowtBox_GetClassPart (_object).applyFormatProps ((_object), (_context)))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_PosFlowBox_Class_t NW_LMgr_PosFlowBox_Class;


NW_LMGR_EXPORT
NW_LMgr_PosFlowBox_t*
NW_LMgr_PosFlowBox_New (NW_ADT_Vector_Metric_t numProperties);

void NW_LMgr_PosFlowBox_CalculatePosition(NW_LMgr_PosFlowBox_t* aPosFlowBox, NW_LMgr_Box_t* aBox);

void NW_LMgr_PosFlowBox_AdjustPosition(NW_LMgr_PosFlowBox_t* aPosFlowBox);

TBrowserStatusCode NW_LMgr_PosFlowBox_AddTabItem(NW_LMgr_PosFlowBox_t* aPosFlowBox);

TBrowserStatusCode NW_LMgr_PosFlowBox_HandlePostFormat(NW_LMgr_PosFlowBox_t* aPosFlowBox);

void NW_LMgr_PosFlowBox_AddPositionedBox(NW_LMgr_PosFlowBox_t* aPosFlowBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 

