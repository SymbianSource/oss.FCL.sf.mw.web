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


#ifndef FRSTACK_H
#define FRSTACK_H

#include "scr_val.h"
#include "scr_fcns.h"
#include "scr_ptrstack.h"

typedef struct
{
  val_t     *vars;
  NW_Ucs2   *url;
  NW_Ucs2   *postfields;
  NW_Ucs2   *referer;
  NW_Uint32 chkSum;
  NW_Int16  code_offset;
  NW_Int16  var_cnt;
  NW_Byte   func_index;
} frame_t;


#define frstack_t pstack_t

void init_frames(void);
NW_Bool push_frame(NW_Ucs2 *url, NW_Ucs2 *postfields, NW_Uint32 chkSum,
                   NW_Int32 func_index, NW_Ucs2 *referer);
void NW_Wmls_PopFrame(void);
NW_Bool is_frstack_empty(void);

void jump_opcode(NW_Int32  offset);
void set_code_offset(NW_Uint16 s);
NW_Uint16 get_code_offset(void);
NW_Bool get_opcode(NW_Byte *b);
NW_Bool get_next_opcode_byte(NW_Byte *b);
NW_Bool get_next_opcode_int16(NW_Uint16 *i16);

NW_Ucs2 *get_curr_referer(void);
NW_Ucs2 *get_first_referer(void);
NW_Ucs2 *get_curr_url(void);
NW_Uint32 get_curr_chkSum(void);
NW_Ucs2 *get_curr_postfields(void);

function_t *get_curr_func(void);
NW_Int16 get_curr_func_size(void);

val_t get_func_var(NW_Byte i);
void set_func_var(NW_Byte i, val_t v);
void free_frstack(frstack_t *st);

/* frees all the frames in stack.*/
void free_all_frames(void);

#endif
