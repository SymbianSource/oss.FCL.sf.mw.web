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


#ifndef NW_LMGR_VERTICALTABLEROWBOX_H
#define NW_LMGR_VERTICALTABLEROWBOX_H

#include "nw_lmgr_bidiflowbox.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_VerticalTableRowBox_s NW_LMgr_VerticalTableRowBox_t;

typedef struct NW_LMgr_VerticalTableRowBox_ClassPart_s 
{
  void** unused;			/* empty class part */
} NW_LMgr_VerticalTableRowBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_VerticalTableRowBox_Class_s 
{
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  NW_LMgr_VerticalTableRowBox_ClassPart_t NW_LMgr_VerticalTableRowBox;
} NW_LMgr_VerticalTableRowBox_Class_t;

struct NW_LMgr_VerticalTableRowBox_s 
{
  NW_LMgr_BidiFlowBox_t super;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_VerticalTableRowBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_VerticalTableRowBox))

#define NW_LMgr_VerticalTableRowBoxOf(_object) \
((NW_LMgr_VerticalTableRowBox_t*) (_object))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_VerticalTableRowBox_Class_t NW_LMgr_VerticalTableRowBox_Class;

NW_LMGR_EXPORT
NW_LMgr_VerticalTableRowBox_t*
NW_LMgr_VerticalTableRowBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_LMGR_VERTICALTABLEROWBOX_H */