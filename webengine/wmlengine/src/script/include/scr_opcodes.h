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
    $Workfile: scr_opcodes.h $

    Purpose:

        This file is used by the scr_inter.c.  
        This file lists all the valid opcodes in the WLScriptParser.
 */
 
#ifndef SCR_OPCODES_H
#define SCR_OPCODES_H

#define JUMP_FW_S   0x80
#define JUMP_FW     0x01
#define JUMP_FW_W   0x02
#define JUMP_BW_S   0xA0
#define JUMP_BW     0x03
#define JUMP_BW_W   0x04
#define TJUMP_FW_S  0xC0
#define TJUMP_FW    0x05
#define TJUMP_FW_W  0x06
#define TJUMP_BW    0x07
#define TJUMP_BW_W  0x08

#define CALL_S      0x60
#define CALL        0x09
#define CALL_LIB_S  0x68
#define CALL_LIB    0x0A
#define CALL_LIB_W  0x0B
#define CALL_URL    0x0C
#define CALL_URL_W  0x0D

#define LOAD_VAR_S  0xE0
#define LOAD_VAR    0x0E
#define STORE_VAR_S 0x40
#define STORE_VAR   0x0F
#define INCR_VAR_S  0x70
#define INCR_VAR    0x10
#define DECR_VAR    0x11


#define LOAD_CONST_S  0x50
#define LOAD_CONST    0x12
#define LOAD_CONST_W  0x13
#define CONST_0       0x14
#define CONST_1       0x15
#define CONST_M1      0x16
#define CONST_ES      0x17
#define CONST_INVALID 0x18
#define CONST_TRUE    0x19
#define CONST_FALSE   0x1A

#define INCR    0x1B
#define DECR    0x1C
#define ADD_ASG 0x1D
#define SUB_ASG 0x1E
#define UMINUS  0x1F
#define ADD     0x20
#define SUB     0x21
#define MUL     0x22
#define DIV     0x23
#define IDIV    0x24
#define REM     0x25

#define B_AND       0x26
#define B_OR        0x27
#define B_XOR       0x28
#define B_NOT       0x29
#define B_LSHIFT    0x2A
#define B_RSSHIFT   0x2B
#define B_RSZSHIFT  0x2C

#define EQ  0x2D
#define LE  0x2E
#define LT  0x2F
#define GE  0x30
#define GT  0x31
#define NE  0x32

#define L_NOT  0x33
#define SCAND  0x34
#define SCOR   0x35
#define TOBOOL 0x36


#define POP    0x37

#define TYPEOF     0x38
#define ISVALID    0x39

#define RETURN     0x3A
#define RETURN_ES  0x3B

#define DEBUG_OP  0x3C


void parse_opcode(NW_Byte *opcode, NW_Byte *param);

#endif


