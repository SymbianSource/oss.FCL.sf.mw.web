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


#ifndef NW_LMgr_ICollapsibleBox_h
#define NW_LMgr_ICollapsibleBox_h

#include "nw_object_interface.h"
#include "nw_evt_event.h"
#include "NW_LMgr_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ICollapsibleBox_Class_s NW_LMgr_ICollapsibleBox_Class_t;
typedef struct NW_LMgr_ICollapsibleBox_s NW_LMgr_ICollapsibleBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_LMgr_ICollapsibleBox_Collapse_t) (NW_LMgr_ICollapsibleBox_t* collapsibleBox);

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ICollapsibleBox_ClassPart_s {
  NW_LMgr_ICollapsibleBox_Collapse_t collapse;
} NW_LMgr_ICollapsibleBox_ClassPart_t;
  
struct NW_LMgr_ICollapsibleBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_LMgr_ICollapsibleBox_ClassPart_t NW_LMgr_ICollapsibleBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_LMgr_ICollapsibleBox_s {
  NW_Object_Interface_t super;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ICollapsibleBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ICollapsibleBox))

#define NW_LMgr_ICollapsibleBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ICollapsibleBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_ICollapsibleBox_Class_t NW_LMgr_ICollapsibleBox_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ICollapsibleBox_Collapse(_collapsibleBox) \
  (NW_Object_Invoke(_collapsibleBox, NW_LMgr_ICollapsibleBox, collapse) \
    (NW_LMgr_ICollapsibleBoxOf (_collapsibleBox)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_LMgr_ICollapsibleBox_h */
