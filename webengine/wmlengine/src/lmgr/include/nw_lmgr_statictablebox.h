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


#ifndef NW_STATICTABLEBOX_H
#define NW_STATICTABLEBOX_H

#include "nw_lmgr_statictableboxi.h"
#include "nw_lmgr_formatbox.h"
#include "nw_lmgr_statictablecontext.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



/* ------------------------------------------------------------------------- */
#define NW_LMgr_StaticTableBox_DefaultCaptionWidth ((NW_GDI_Metric_t)40)
#define NW_LMgr_StaticTableBox_MinimumTableWidth ((NW_GDI_Metric_t)40)

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableBox_s NW_LMgr_StaticTableBox_t;

typedef enum NW_LMgr_StaticTableBox_AutomaticWidth_PassTypes_e{
  NW_LMgr_StaticTableBox_AutomaticWidth_UndefinedPass,
  // NW_LMgr_StaticTableBox_AutomaticWidth_MinimumPass, note: 
  //  minimum pass is eliminated by some performance improvement
  NW_LMgr_StaticTableBox_AutomaticWidth_MaximumPass,
  NW_LMgr_StaticTableBox_AutomaticWidth_FinalPass
  }NW_LMgr_StaticTableBox_AutomaticWidth_PassTypes_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_StaticTableBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_StaticTableBox_ClassPart_t NW_LMgr_StaticTableBox;
} NW_LMgr_StaticTableBox_Class_t;

/* ------------------------------------------------------------------------- *
   private types
 * ------------------------------------------------------------------------- */

struct NW_LMgr_StaticTableBox_s {
  NW_LMgr_FormatBox_t super;
  NW_LMgr_StaticTableContext_t *ctx;
  NW_LMgr_StaticTableBox_AutomaticWidth_PassTypes_t automaticWidthPass;
  NW_Bool         needsRelayout;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_StaticTableBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_StaticTableBox))


#define NW_LMgr_StaticTableBoxOf(_object) \
((NW_LMgr_StaticTableBox_t*) (_object))

/* ------------------------------------------------------------------------- */
#define NW_LMgr_StaticTableBox_GetContext(_table) \
  ((_table)->ctx)
#define NW_LMgr_StaticTableBox_GetRow(_table, _i) \
  (NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(_table), _i))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_StaticTableBox_Class_t NW_LMgr_StaticTableBox_Class;

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
void
NW_LMgr_StaticTableBox_SetAutomaticTableWidthPass( NW_LMgr_StaticTableBox_t* thisObj, 
                                                   NW_LMgr_StaticTableBox_AutomaticWidth_PassTypes_t passType );

NW_LMGR_EXPORT
void
NW_LMgr_StaticTableBox_TableSizeChanged( NW_LMgr_StaticTableBox_t* thisObj );

NW_LMGR_EXPORT
void
NW_LMgr_StaticTableBox_UpdatesTableMinMaxWidth( NW_LMgr_StaticTableBox_t* thisObj );

NW_LMGR_EXPORT
NW_LMgr_StaticTableBox_t*
NW_LMgr_StaticTableBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_STATICTABLEBOX_H */
