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


#ifndef _NW_LMgr_MarqueeBox_h_
#define _NW_LMgr_MarqueeBox_h_

#include "nw_lmgr_bidiflowbox.h"
#include "nw_system_timer.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_MarqueeBox_Class_s NW_LMgr_MarqueeBox_Class_t;
typedef struct NW_LMgr_MarqueeBox_s NW_LMgr_MarqueeBox_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_MarqueeBox_ClassPart_s {
  void** unused;
} NW_LMgr_MarqueeBox_ClassPart_t;

struct NW_LMgr_MarqueeBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  NW_LMgr_MarqueeBox_ClassPart_t NW_LMgr_MarqueeBox;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_MarqueeBox_s {
  NW_LMgr_FormatBox_t super;

  /* Member variables */
  NW_Bool started;
  NW_System_Timer_t* timer;
  NW_Bool loopFinished;
  NW_Uint16 loopCount;
  NW_Uint16 pause;
  NW_LMgr_PropertyValueToken_t marqueeStyle;
  NW_LMgr_PropertyValueToken_t marqueeDir;
  NW_GDI_Metric_t marqueeAmount;
  NW_Uint16 marqueeSpeed;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_MarqueeBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_MarqueeBox))

#define NW_LMgr_MarqueeBoxOf(_object) \
((NW_LMgr_MarqueeBox_t*) (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_MarqueeBox_Class_t NW_LMgr_MarqueeBox_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_MarqueeBox_Initialize (NW_LMgr_MarqueeBox_t *marquee);

NW_LMGR_EXPORT
NW_LMgr_MarqueeBox_t*
NW_LMgr_MarqueeBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_LMgr_MarqueeBox_h_ */

