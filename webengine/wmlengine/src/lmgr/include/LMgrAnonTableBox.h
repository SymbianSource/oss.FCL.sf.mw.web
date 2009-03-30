/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class represents the anonymous table box 
*                that wraps the caption and the table
                 See CSS2 Spec from WWW.W3C.ORG (page 251)
*
*/

#ifndef _LMGRANONTABLEBOX_H
#define _LMGRANONTABLEBOX_H

#include "LMgrAnonBlock.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Bidi Flow boxes manage bidirectional flows */

typedef struct LMgrAnonTableBox_s LMgrAnonTableBox_t;

typedef struct LMgrAnonTableBox_ClassPart_s {
  void** unused;			/* empty class part */
} LMgrAnonTableBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct LMgrAnonTableBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  LMgrAnonBlock_ClassPart_t LMgrAnonBlock;
  LMgrAnonTableBox_ClassPart_t LMgrAnonTableBox;
} LMgrAnonTableBox_Class_t;

/* ------------------------------------------------------------------------- */
struct LMgrAnonTableBox_s {
  LMgrAnonBlock_t super;
};

/* ------------------------------------------------------------------------- */

#define LMgrAnonTableBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, LMgrAnonTableBox))


#define LMgrAnonTableBoxOf(_object) \
((LMgrAnonTableBox_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const LMgrAnonTableBox_Class_t LMgrAnonTableBox_Class;


NW_LMGR_EXPORT
LMgrAnonTableBox_t*
LMgrAnonTableBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 

