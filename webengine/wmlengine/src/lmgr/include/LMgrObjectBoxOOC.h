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


#ifndef NW_LMGR_OBJECTBOX_H
#define NW_LMGR_OBJECTBOX_H

#include "nw_lmgr_mediabox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ObjectBox_Class_s NW_LMgr_ObjectBox_Class_t;
typedef struct NW_LMgr_ObjectBox_s NW_LMgr_ObjectBox_t;

/* ------------------------------------------------------------------------- */
class CLMgrObjectBox;
class MLMgrObjectBoxListener;
class CCoeControl;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ObjectBox_ClassPart_s 
    {
    void** unused;
    } NW_LMgr_ObjectBox_ClassPart_t;

struct NW_LMgr_ObjectBox_Class_s 
    {
    NW_Object_Core_ClassPart_t NW_Object_Core;
    NW_Object_Base_ClassPart_t NW_Object_Base;
    NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
    NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
    NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
    NW_LMgr_ObjectBox_ClassPart_t NW_LMgr_ObjectBox;
    };

/* ------------------------------------------------------------------------- */
struct NW_LMgr_ObjectBox_s 
    {
    NW_LMgr_MediaBox_t super;

    /* PRIVATE memeber fields */
    CLMgrObjectBox* iObjectBox;
    };

/* ------------------------------------------------------------------------- */

#define NW_LMgr_ObjectBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ObjectBox))

#define NW_LMgr_ObjectBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ObjectBox))

#define NW_LMgr_ObjectBox_GetObjectBox(_box) \
  (((NW_LMgr_ObjectBox_t&) (_box)).iObjectBox)

/*------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_ObjectBox_Class_t NW_LMgr_ObjectBox_Class;

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_Bool
NW_LMgr_ObjectBox_IsActive(NW_LMgr_Box_t* box);

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_ObjectBox_t*
NW_LMgr_ObjectBox_New(NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_LMGR_OBJECTBOX_H */
