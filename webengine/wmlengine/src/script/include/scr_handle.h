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
    $Workfile: scr_handle.h $

    Purpose:

       This file is used by the scr_handle.c.  It defines the prototypes 
       for all the functions used by the scr_handle.c file.
        
 */
 
#ifndef SCR_HANDLE_H
#define SCR_HANDLE_H

#include "scr_defs.h"

void handle_jump_fw_s(NW_Byte offset);
void handle_jump_fw(void);
void handle_jump_fw_w(void);
void handle_jump_bw_s(NW_Byte offset);
void handle_jump_bw(void);
void handle_jump_bw_w(void);
void handle_tjump_fw_s(NW_Byte offset);
void handle_tjump_fw(void);
void handle_tjump_fw_w(void);
void handle_tjump_bw(void);
void handle_tjump_bw_w(void);
void handle_call_s(NW_Byte findex);
void handle_call(void);
NW_Bool handle_call_lib_s(NW_Byte findex, NW_Bool *async);
NW_Bool handle_call_lib(NW_Bool *async);
NW_Bool handle_call_lib_w(NW_Bool *async);
void handle_load_var_s(NW_Byte vindex);
void handle_load_var(void);
void handle_store_var_s(NW_Byte vindex);
void handle_store_var(void);
void handle_load_const_s(NW_Byte cindex);
void handle_load_const(void);
void handle_load_const_w(void);
void handle_incr_var_s(NW_Byte vindex);
void handle_incr_var(void);
void handle_decr_var(void);
void handle_incr(void);
void handle_decr(void);
void handle_add_asg(void);
void handle_sub_asg(void);
void handle_uminus(void);
void handle_add(void);
void handle_sub(void);
void handle_mul(void);
void handle_div(void);
void handle_idiv(void);
void handle_rem(void);
void handle_b_and(void);
void handle_b_or(void);
void handle_b_xor(void);
void handle_lshift(void);
void handle_rsshift(void);
void handle_rszshift(void);
void handle_comp(NW_Byte op);
void handle_l_not(void);/*bang*/
void handle_b_not(void);/*compl*/
void handle_pop(void);
void handle_load_const_int(NW_Int32  val);
void handle_load_const_bool(NW_Bool val);
void handle_load_const_es(void);
void handle_load_invalid(void);
void handle_typeof(void);
void handle_isvalid(void);
void handle_tobool(void);
void handle_scand(void);
void handle_scor(void);

NW_Bool handle_call_url(void);
NW_Bool handle_call_url_w(void);

#endif /*SCR_HANDLE_H*/


