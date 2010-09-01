/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


 /*
    $Workfile: scr_val.h $

    Purpose:

  representing typed values in 4 bytes
 */

#ifndef SCR_VAL_H
#define SCR_VAL_H

#include "scr_defs.h"
#include "scr_str.h"

typedef enum 
{
    VAL_TYPE_INT,
    VAL_TYPE_FLOAT,
    VAL_TYPE_STR,
    VAL_TYPE_BOOL,
    VAL_TYPE_INVALID,
    VAL_TYPE_UNINITIALIZED
} SCR_VAL_TYPE;

/* Note: use "volatile" keyword to suppress compiler optimizer bug */
/*       which changes store/load operation into a data-type cast */
typedef struct 
{
   volatile union {
      NW_Int32    i;
      NW_Float32  f;
      str_t       s;
      NW_Bool     b;
   }value;

   SCR_VAL_TYPE type;
} val_t;

NW_Ucs2* GetUcs2Val(void);
NW_Ucs2* GetUcs2Val_notrim(void);
NW_Bool GetIntVal(NW_Int32 *value);
NW_Bool GetBoolVal(NW_Bool *value);
NW_Opaque_t* GetOpaqueVal(void);

val_t new_int_val(NW_Int32  i);
val_t new_float_val(NW_Float32 f);
val_t new_bool_val(NW_Bool b);
val_t new_empty_str_val(void);
val_t new_str_val(str_t s);
val_t new_str_val_and_free(str_t *sptr);
val_t new_invalid_val(void);
val_t uninitialize_val(void);
val_t dup_val(val_t);

NW_Bool val2float(val_t v, NW_Float32 *f);
NW_Bool val2int(val_t v, NW_Int32  *res);
NW_Bool val2bool(val_t v, NW_Bool *res);
NW_Bool val2str(val_t v, str_t *str);
NW_Bool val2str_and_free(val_t *vptr, str_t *str);

NW_Ucs2 *val2ucs2(val_t v);
NW_Ucs2 *val2ucs2_trim(val_t v);

SCR_VAL_TYPE ScriptVarType(val_t v);

void free_val(val_t v);

 
#define IS_INT(v) ((v).type == VAL_TYPE_INT)

#define IS_FLOAT(v) ((v).type == VAL_TYPE_FLOAT)

#define IS_BOOL(v) ((v).type == VAL_TYPE_BOOL)

#define IS_STR(v) ((v).type == VAL_TYPE_STR)

#define IS_INVALID(v) ((v).type == VAL_TYPE_INVALID)

#endif  /*SCR_VAL_H*/

