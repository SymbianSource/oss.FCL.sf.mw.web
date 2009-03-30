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


#ifndef NW_NOBRBOX_H
#define NW_NOBRBOX_H

#include "nw_lmgr_containerbox.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



/* Class declarations -------------------------------------------------------*/
typedef struct NW_LMgr_NobrBox_s NW_LMgr_NobrBox_t;
  
typedef struct NW_LMgr_NobrBox_ClassPart_s 
{
  void** unused;			/* empty class part */
} NW_LMgr_NobrBox_ClassPart_t;
  
typedef struct NW_LMgr_NobrBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_NobrBox_ClassPart_t NW_LMgr_NobrBox;
} NW_LMgr_NobrBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_NobrBox_s {
  NW_LMgr_ContainerBox_t super;
};
  
/* ------------------------------------------------------------------------- */

#define NW_LMgr_NobrBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_NobrBox))

#define NW_LMgr_NobrBoxOf(_object) \
((NW_LMgr_NobrBox_t*) (_object))

/* ---------------------------------------------------------------------------*/

NW_LMGR_EXPORT const NW_LMgr_NobrBox_Class_t NW_LMgr_NobrBox_Class;

NW_LMGR_EXPORT
NW_LMgr_NobrBox_t*
NW_LMgr_NobrBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 







































