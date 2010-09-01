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


#ifndef NW_LINEBOX_H
#define NW_LINEBOX_H

#include "nw_lmgr_box.h"
#include "nw_lmgr_linebox_defn.h"
#include "nw_object_dynamic.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */




/* Class declarations -------------------------------------------------------*/
  
typedef struct NW_LMgr_LineBox_ClassPart_s {
  void** unused;
} NW_LMgr_LineBox_ClassPart_t;
  
typedef struct NW_LMgr_LineBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_LineBox_ClassPart_t NW_LMgr_LineBox;
} NW_LMgr_LineBox_Class_t;


  
/* ------------------------------------------------------------------------- */
#define NW_LMgr_LineBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_LineBox))

#define NW_LMgr_LineBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_LineBox))

/* ---------------------------------------------------------------------------*/

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_LineBox_Clear(NW_LMgr_LineBox_t *lineBox); 

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_LineBox_Append(NW_LMgr_LineBox_t *lineBox, 
                       NW_LMgr_Box_t* box,
                       NW_GDI_FlowDirection_t dir);

NW_LMGR_EXPORT
NW_ADT_Vector_Metric_t
NW_LMgr_LineBox_GetCount(NW_LMgr_LineBox_t *lineBox);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_LineBox_GetBox(NW_LMgr_LineBox_t *lineBox, NW_ADT_Vector_Metric_t index);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_LineBox_HAlign(NW_LMgr_LineBox_t *lineBox, 
                       NW_LMgr_PropertyValueToken_t type, 
                       NW_GDI_Metric_t space,
                       NW_GDI_FlowDirection_t dir);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_LineBox_VAlign(NW_LMgr_LineBox_t *lineBox, NW_GDI_Metric_t contextY, 
                       NW_GDI_Metric_t* lineBase, NW_GDI_Metric_t *lineHeight);

NW_LMGR_EXPORT
NW_GDI_Metric_t
NW_LMgr_LineBox_GetEm(NW_LMgr_LineBox_t *lineBox);


/* ---------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

NW_LMGR_EXPORT
NW_LMgr_LineBox_t*
NW_LMgr_LineBox_New (void);

#define NW_LMgr_LineBox_Delete(_object) \
  NW_Object_Delete (NW_Object_DynamicOf (_object))

NW_LMGR_EXPORT const NW_LMgr_LineBox_Class_t NW_LMgr_LineBox_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 







































