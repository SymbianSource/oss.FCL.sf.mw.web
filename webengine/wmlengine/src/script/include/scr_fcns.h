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
    $Workfile: scr_fcns.h $

    Purpose:
        This file is used by the scr_fcns.c.  It defines both the function structure 
        and the function pool structure as well as the function prototype used by 
        the function pool.
        
 */

#ifndef SCR_FUNCTIONS_H
#define SCR_FUNCTIONS_H

#include "scr_defs.h"

typedef struct {
  NW_Byte nbr_of_args;  /* no of args */
  NW_Byte nbr_of_local_vars;  /* no of total local vars, including args !!! */
  NW_Uint16 code_size;  
  NW_Byte *code;
} function_t;


typedef struct {
  function_t **funcs;
  NW_Int32  capacity;
  NW_Int32  size;
} funcpool_t;

typedef struct {
  NW_Byte index;
  NW_Ucs2 *name;
} fname_t;

typedef struct {
  fname_t *names;
  NW_Int32  capacity;
  NW_Int32  size;
} fname_table_t;

void add_fname(NW_Byte index, NW_Ucs2 *name);
void add_func_fp(function_t *f);
NW_Bool create_func_pool(NW_Int32  c) ;
NW_Bool create_fname_table(NW_Int32  c);

NW_Byte no_of_args(function_t *f);
NW_Byte no_of_local_vars(function_t *f);
NW_Uint16 get_code_size(function_t *f);


NW_Bool fu_get_opcode(NW_Byte func_index, NW_Uint16 offset, NW_Byte *b);
function_t *get_func(NW_Int32  funcIndex);

NW_Bool fname_find_function_index(fname_table_t *fn_table, NW_Ucs2 *fname, NW_Byte *index);

void free_funcpool(funcpool_t *fp);
void free_fname_table(fname_table_t *fn_table);

NW_Bool valid_func_index(NW_Int32  index);

#endif  /*SCR_FUNCTIONS_H*/
