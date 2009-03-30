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


#ifndef NW_LMGR_BOX_DEFN_H
#define NW_LMGR_BOX_DEFN_H
  
#include "nw_object_dynamic.h"
#include "nw_lmgr_propertylist.h"
#include "nw_gdi_types.h"
#include "nw_adt_vector.h"
#include "NW_LMgr_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Box_s NW_LMgr_Box_t;
typedef struct NW_LMgr_ContainerBox_s NW_LMgr_ContainerBox_t;


/* ------------------------------------------------------------------------- *
   object definition
 * ------------------------------------------------------------------------- */
struct NW_LMgr_Box_s{
  NW_Object_Dynamic_t super;

  /* member variables */
  // format uses format bounds
  NW_GDI_Rectangle_t iFormatBounds;
  // rendering uses display bounds. These two bounds could be different,
  // while the page is being formatted. 
  NW_GDI_Rectangle_t iDisplayBounds;
  NW_LMgr_PropertyList_t* propList;
  NW_LMgr_ContainerBox_t* parent;
  // rootBox must be accessed only via NW_LMgr_Box_GetRootBox
  // as if it is NULL, then it gets the rootBox from its parent.
  // ZAL: temp: change it back to NW_LMgr_RootBox_t*
  void* rootBox;
  NW_Bool hasFocus;
};


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_BOX_H */



