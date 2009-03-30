/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBox_Validator_h
#define NW_FBox_Validator_h

#include "nw_object_dynamic.h"
#include "nw_lmgr_box.h"
#include "NW_Text_Abstract.h"
#include "NW_FBox_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */
typedef enum NW_FBox_Validator_EmptyOk_e {
  NW_FBox_Validator_EmptyOk_None,
  NW_FBox_Validator_EmptyOk_True,
  NW_FBox_Validator_EmptyOk_False
} NW_FBox_Validator_EmptyOk_t;

typedef enum NW_FBox_Validator_Mode_e {
  NW_FBox_Validator_Mode_None,
  NW_FBox_Validator_Mode_Hiragana,
  NW_FBox_Validator_Mode_Katakana,
  NW_FBox_Validator_Mode_Hankakukana,
  NW_FBox_Validator_Mode_Alphabet,
  NW_FBox_Validator_Mode_Numeric
} NW_FBox_Validator_Mode_t;

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Validator_Class_s NW_FBox_Validator_Class_t;
typedef struct NW_FBox_Validator_s NW_FBox_Validator_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_FBox_Validator_Validate_t) (NW_FBox_Validator_t* validator,
                                 const NW_Text_t* string);

typedef
TBrowserStatusCode
(*NW_FBox_Validator_GetFormat_t) (NW_FBox_Validator_t* validator,
                                  const NW_Ucs2** format,
                                  NW_FBox_Validator_Mode_t* mode,
                                  NW_FBox_Validator_EmptyOk_t* emptyOk);


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_Validator_ClassPart_s {
  NW_FBox_Validator_Validate_t validate;
  NW_FBox_Validator_GetFormat_t getFormat;
} NW_FBox_Validator_ClassPart_t;

struct NW_FBox_Validator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Validator_ClassPart_t NW_FBox_Validator;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_FBox_Validator_s {
  NW_Object_Dynamic_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_Validator_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_Validator))

#define NW_FBox_ValidatorOf(object) \
  (NW_Object_Cast (object, NW_FBox_Validator))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_Validator_Class_t NW_FBox_Validator_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_FBox_Validator_Validate(_validator, _string) \
  (NW_Object_Invoke (_validator, NW_FBox_Validator, validate) \
     ((_validator), (_string)))

#define NW_FBox_Validator_GetFormat(_validator, _format, _mode, _emptyok) \
  (NW_Object_Invoke (_validator, NW_FBox_Validator, getFormat) \
     ((_validator), (_format), (_mode), (_emptyok)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Tempest_FBox_FBoxValidator_h */




