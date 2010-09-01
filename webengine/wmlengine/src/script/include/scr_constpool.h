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
    $Workfile: scr_constpool.h $

    Purpose:

        This file is used by the scr_constpool.c.  It defines the prototypes 
        for all the functions used by the constant pool.
*/

#ifndef SCR_CONSTPOOL_H
#define SCR_CONSTPOOL_H

#include "scr_val.h"

typedef struct {
  val_t     *vals;
  NW_Uint16 size;
  NW_Uint16 charset;
}  constpool_t;

constpool_t *create_cp(NW_Int32  size, NW_Int32  charset);

void append_cp_int(NW_Int32  index, NW_Int32  v);
void append_cp_float(NW_Int32  index, NW_Float32 f);
void append_cp_string(NW_Int32  index, str_t s);

val_t get_const(NW_Int32  index);
NW_Bool valid_const_index(NW_Int32  index);
NW_Bool is_const_string(NW_Int32  index);

void free_cp(constpool_t *cp);

#endif  /* SCR_CONSTPOOL_H */
