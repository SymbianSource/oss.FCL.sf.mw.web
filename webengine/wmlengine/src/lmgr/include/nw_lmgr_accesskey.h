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


#ifndef NW_LMGR_ACCESSKEY_H
#define NW_LMGR_ACCESSKEY_H

#include "nw_object_dynamic.h"
#include "nw_text_ucs2.h"
#include "nw_adt_dynamicvector.h"
#include "NW_LMgr_EXPORT.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AccessKey_Class_s NW_LMgr_AccessKey_Class_t;
typedef struct NW_LMgr_AccessKey_s NW_LMgr_AccessKey_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

typedef
NW_Bool
(*NW_LMgr_AccessKey_IsValidForOptionsList_t) (NW_LMgr_AccessKey_t* accessKey, 
                                      NW_Uint32 val);

typedef
NW_Bool
(*NW_LMgr_AccessKey_IsValidKeyPress_t) (NW_LMgr_AccessKey_t* accessKey, 
                                      NW_Uint32 val);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AccessKey_ClassPart_s {
  NW_LMgr_AccessKey_IsValidForOptionsList_t isValidForOptionsList;
  NW_LMgr_AccessKey_IsValidKeyPress_t isValidKeyPress;
} NW_LMgr_AccessKey_ClassPart_t;

struct NW_LMgr_AccessKey_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_AccessKey_ClassPart_t NW_LMgr_AccessKey;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_AccessKey_s {
  NW_Object_Dynamic_t super;

  NW_ADT_DynamicVector_t* valueList;
  NW_Text_t* label;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_AccessKey_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_AccessKey))

#define NW_LMgr_AccessKeyOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_AccessKey))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_AccessKey_Class_t NW_LMgr_AccessKey_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_AccessKey_SetFromTextAttr(NW_LMgr_AccessKey_t* accessKey, 
                                  NW_Text_t* val);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_AccessKey_AddVal(NW_LMgr_AccessKey_t* accessKey, 
                         NW_Ucs2* val,
                         NW_Bool andOp);

NW_LMGR_EXPORT
NW_Bool
NW_LMgr_AccessKey_MatchKey(NW_LMgr_AccessKey_t* accessKey, 
                           NW_Uint32 v,
                           NW_Bool isControlKey);

NW_Bool
NW_LMgr_AccessKey_GetDirectKey(NW_LMgr_AccessKey_t* accessKey, 
                           NW_Uint32 *v);

NW_LMGR_EXPORT
NW_Bool
NW_LMgr_AccessKey_SetAvailableKey(NW_LMgr_AccessKey_t* accessKey, 
                                  NW_ADT_DynamicVector_t* activeBoxList);

#define NW_LMgr_AccessKey_GetSupportedKeys(_accesskey) \
  (NW_LMgr_AccessKeyOf(_accesskey)->valueList)

#define NW_LMgr_AccessKey_IsValidForOptionsList(_accesskey, _val) \
  (NW_Object_Invoke (_accesskey, NW_LMgr_AccessKey, isValidForOptionsList) \
   ((_accesskey), (_val)))

#define NW_LMgr_AccessKey_IsValidKeyPress(_accesskey, _val) \
  (NW_Object_Invoke (_accesskey, NW_LMgr_AccessKey, isValidKeyPress) \
   ((_accesskey), (_val)))

#define NW_LMgr_AccessKey_GetLabel(_accesskey) \
  (NW_LMgr_AccessKeyOf(_accesskey)->label)

#define NW_LMgr_AccessKey_SetLabel(_accesskey, _label) \
  (NW_LMgr_AccessKeyOf(_accesskey)->label = _label)

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMgr_AccessKey_t*
NW_LMgr_AccessKey_New();

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ACCESSKEY_H */
