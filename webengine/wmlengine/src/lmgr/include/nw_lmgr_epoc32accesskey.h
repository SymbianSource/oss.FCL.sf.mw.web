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


#ifndef NW_LMGR_EPOC32ACCESSKEY_H
#define NW_LMGR_EPOC32ACCESSKEY_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include "nw_lmgr_accesskey.h"


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Epoc32AccessKey_Class_s NW_LMgr_Epoc32AccessKey_Class_t;
typedef struct NW_LMgr_Epoc32AccessKey_s NW_LMgr_Epoc32AccessKey_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_Epoc32AccessKey_ClassPart_s {
  void** unused;
}NW_LMgr_Epoc32AccessKey_ClassPart_t;

struct NW_LMgr_Epoc32AccessKey_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_AccessKey_ClassPart_t NW_LMgr_AccessKey;
  NW_LMgr_Epoc32AccessKey_ClassPart_t NW_LMgr_Epoc32AccessKey;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_Epoc32AccessKey_s {
  NW_LMgr_AccessKey_t super;

};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_Epoc32AccessKey_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_Epoc32AccessKey))

#define NW_LMgr_Epoc32AccessKeyOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_Epoc32AccessKey))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_LMgr_Epoc32AccessKey_Class_t NW_LMgr_Epoc32AccessKey_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_LMGR_EPOC32ACCESSKEY_H */
