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


#ifndef NW_EMPTYBOX_H
#define NW_EMPTYBOX_H

#include "nw_lmgr_box.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Line boxes are currently implemented as if they were simply container boxes.
 * However, they have not been subclassed from container because they can, and
 * will eventually be, implemented in a much more light-weight manner
 */

typedef struct NW_LMgr_EmptyBox_s NW_LMgr_EmptyBox_t;


/* Class declarations -------------------------------------------------------*/
  
typedef struct NW_LMgr_EmptyBox_ClassPart_s {
  void** unused;
} NW_LMgr_EmptyBox_ClassPart_t;
  
typedef struct NW_LMgr_EmptyBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_EmptyBox_ClassPart_t NW_LMgr_EmptyBox;
} NW_LMgr_EmptyBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_EmptyBox_s {
  NW_LMgr_Box_t super;
};
  
/* ------------------------------------------------------------------------- */

#define NW_LMgr_EmptyBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_EmptyBox))

#define NW_LMgr_EmptyBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_EmptyBox))

/* ---------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

NW_LMGR_EXPORT
NW_LMgr_EmptyBox_t*
NW_LMgr_EmptyBox_New (NW_ADT_Vector_Metric_t numProperties);

#define NW_LMgr_EmptyBox_Delete(_object) \
  NW_Object_Delete (NW_Object_DynamicOf (_object))

NW_LMGR_EXPORT const NW_LMgr_EmptyBox_Class_t NW_LMgr_EmptyBox_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_EMPTYBOX_H */
