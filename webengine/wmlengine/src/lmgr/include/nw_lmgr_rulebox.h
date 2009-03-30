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


#ifndef NW_RULEBOX_H
#define NW_RULEBOX_H

#include "nw_lmgr_mediabox.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- */
#define NW_LMGR_RULEBOX_ALIGN_LEFT   (NW_GDI_Metric_t)0
#define NW_LMGR_RULEBOX_ALIGN_CENTER (NW_GDI_Metric_t)1
#define NW_LMGR_RULEBOX_ALIGN_RIGHT  (NW_GDI_Metric_t)2

#define NW_LMGR_RULEBOX_COLOR_DGRAY  (NW_GDI_Color_t)0x555555
#define NW_LMGR_RULEBOX_COLOR_LGRAY  (NW_GDI_Color_t)0xbbbbbb
#define NW_LMGR_RULEBOX_COLOR_BLACK  (NW_GDI_Color_t)0x000000
/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_RuleBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_RuleBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_RuleBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_RuleBox_ClassPart_t NW_LMgr_RuleBox;
} NW_LMgr_RuleBox_Class_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_RuleBox_s {
  NW_LMgr_MediaBox_t super;
} NW_LMgr_RuleBox_t;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_RuleBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_RuleBox))
  
#define NW_LMgr_RuleBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_RuleBox))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_RuleBox_Class_t NW_LMgr_RuleBox_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_RuleBox_t*
NW_LMgr_RuleBox_New ();

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_RULEBOX_H */
