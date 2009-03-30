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


#ifndef NW_MEDIABOX_H
#define NW_MEDIABOX_H

#include "nw_lmgr_box.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/*-- Media box ----------------------------------------------------------------*/

typedef struct NW_LMgr_MediaBox_ClassPart_s {
  void** unused;
} NW_LMgr_MediaBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_MediaBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
} NW_LMgr_MediaBox_Class_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_MediaBox_s {
  NW_LMgr_Box_t super;
} NW_LMgr_MediaBox_t;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_MediaBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_MediaBox))

#define NW_LMgr_MediaBoxOf(_object) \
((NW_LMgr_MediaBox_t*) (_object))

/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_MediaBox_Class_t NW_LMgr_MediaBox_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_MEDIABOX_H */
