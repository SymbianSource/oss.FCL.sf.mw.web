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
    $Workfile: scr_valexpr.h $

    Purpose:

       This file is used by the scr_valexpr.h.  It defines the val_expr structure 
       and all the prototypes needed for the scr_val_expr.c
 */

#ifndef SCR_VALEXPR_H
#define SCR_VALEXPR_H

#include "scr_defs.h"
#include "scr_val.h"

val_t add_string_vals(val_t v1, val_t v2);
val_t add_vals(val_t v1, val_t v2);
val_t sub_vals(val_t v1, val_t v2);
val_t mul_vals(val_t v1, val_t v2);
val_t div_vals(val_t v1, val_t v2);
val_t idiv_vals(val_t v1, val_t v2);
val_t rem_vals(val_t v1, val_t v2);

val_t incr_val(val_t v);
val_t decr_val(val_t v);

val_t l_not_val(val_t v);
val_t b_not_val(val_t v);
val_t uminus_val(val_t v);

val_t comp_vals(val_t v1, val_t v2, NW_Byte op);
NW_Bool comp_string_vals(str_t s1, str_t s2, NW_Byte op);
NW_Bool comp_int_vals(NW_Int32  i1, NW_Int32  i2, NW_Byte op);
NW_Bool comp_float_vals(NW_Float32 f1, NW_Float32 f2, NW_Byte op);

val_t lshift_vals(val_t v1, val_t v2);
val_t rsshift_vals(val_t v1, val_t v2);
val_t rszshift_vals(val_t v1, val_t v2);

val_t bit_and_vals(val_t v1, val_t v2);
val_t bit_or_vals(val_t v1, val_t v2);
val_t bit_xor_vals(val_t v1, val_t v2);

#endif  /*SCR_VALEXPR_H*/

