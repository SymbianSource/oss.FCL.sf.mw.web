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


#ifndef NW_LMGR_VERTICALTABLEBOX_H
#define NW_LMGR_VERTICALTABLEBOX_H

#include "nw_lmgr_bidiflowbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_VerticalTableBox_s NW_LMgr_VerticalTableBox_t;

typedef struct NW_LMgr_VerticalTableBox_ClassPart_s 
{
  void** unused;			/* empty class part */
} NW_LMgr_VerticalTableBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_VerticalTableBox_Class_s 
{
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  NW_LMgr_VerticalTableBox_ClassPart_t NW_LMgr_VerticalTableBox;
} NW_LMgr_VerticalTableBox_Class_t;

struct NW_LMgr_VerticalTableBox_s 
{
  NW_LMgr_BidiFlowBox_t super;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_VerticalTableBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_VerticalTableBox))

#define NW_LMgr_VerticalTableBoxOf(_object) \
((NW_LMgr_VerticalTableBox_t*) (_object))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_VerticalTableBox_Class_t NW_LMgr_VerticalTableBox_Class;

NW_LMGR_EXPORT
NW_LMgr_VerticalTableBox_t*
NW_LMgr_VerticalTableBox_New (NW_ADT_Vector_Metric_t numProperties);

NW_LMGR_EXPORT
NW_LMgr_Box_t* 
NW_LMgr_VerticalTableBox_SwitchBox (NW_LMgr_Box_t* box);

/* specialProperty value is meant to reflect that it is handled differently for
   Vertical layout */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_VerticalTableBox_GetPropertyFromList(NW_LMgr_Box_t* box,
                                           NW_LMgr_PropertyName_t name,
                                           NW_LMgr_Property_t* property,
                                           NW_Bool* specialProperty);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_LMGR_VERTICALTABLEBOX_H */
