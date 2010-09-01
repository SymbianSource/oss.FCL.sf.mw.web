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


#ifndef NW_STATICTABLEROWBOX_H
#define NW_STATICTABLEROWBOX_H

#include "nw_lmgr_formatbox.h"
#include "nw_lmgr_statictablebox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* LR Flow boxes manage left-to-right flows */

typedef struct NW_LMgr_StaticTableRowBox_s NW_LMgr_StaticTableRowBox_t;

typedef struct NW_LMgr_StaticTableRowBox_ClassPart_s {
  void** unused;			/* empty class part */
} NW_LMgr_StaticTableRowBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_StaticTableRowBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_StaticTableRowBox_ClassPart_t NW_LMgr_StaticTableRowBox;
} NW_LMgr_StaticTableRowBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_StaticTableRowBox_s {
  NW_LMgr_FormatBox_t super;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_StaticTableRowBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_StaticTableRowBox))


#define NW_LMgr_StaticTableRowBoxOf(_object) \
((NW_LMgr_StaticTableRowBox_t*) (_object))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_StaticTableRowBox_Class_t NW_LMgr_StaticTableRowBox_Class;

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */

#define NW_LMgr_StaticTableRowBox_GetContext(_rowBox) \
  (NW_LMgr_StaticTableBoxOf( \
    NW_LMgr_BoxOf(NW_LMgr_BoxOf(_rowBox)->parent)->parent)->ctx)

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableRowBox_ConstrainRow (NW_LMgr_StaticTableRowBox_t* rowBox);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableRowBox_ResizeRow (NW_LMgr_StaticTableRowBox_t* rowBox);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_StaticTableRowBox_PlaceRow (NW_LMgr_StaticTableRowBox_t* rowBox,
                                    NW_GDI_Metric_t atX, NW_GDI_Metric_t atY);
NW_LMGR_EXPORT
NW_LMgr_StaticTableRowBox_t*
NW_LMgr_StaticTableRowBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_STATICTABLEROWBOX_H */
