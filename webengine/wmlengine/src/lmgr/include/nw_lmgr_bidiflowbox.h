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


#ifndef NW_BIDIFLOWBOX_H
#define NW_BIDIFLOWBOX_H

#include "nw_lmgr_flowbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Bidi Flow boxes manage bidirectional flows */

typedef struct NW_LMgr_BidiFlowBox_s NW_LMgr_BidiFlowBox_t;

typedef struct NW_LMgr_BidiFlowBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_BidiFlowBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_BidiFlowBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
} NW_LMgr_BidiFlowBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_BidiFlowBox_s {
  NW_LMgr_FlowBox_t super;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_BidiFlowBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_BidiFlowBox))


#define NW_LMgr_BidiFlowBoxOf(_object) \
((NW_LMgr_BidiFlowBox_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_BidiFlowBox_Class_t NW_LMgr_BidiFlowBox_Class;


NW_LMGR_EXPORT
NW_LMgr_BidiFlowBox_t*
NW_LMgr_BidiFlowBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 

