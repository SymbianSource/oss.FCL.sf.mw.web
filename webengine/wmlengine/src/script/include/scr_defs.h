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
    $Workfile: scr_defs.h $

    Purpose:

        This file contains generic definition needed by the ScriptServer core files.
 */

#ifndef SCR_DEFS_H
#define SCR_DEFS_H

#include "nwx_defs.h"

/* constant pool types */
#define CP_TYPE_8_BIT_SIGNED_INT        0
#define CP_TYPE_16_BIT_SIGNED_INT       1
#define CP_TYPE_32_BIT_SIGNED_INT       2
#define CP_TYPE_32_BIT_FLOAT            3
#define CP_TYPE_UTF8_STRING             4
#define CP_TYPE_EMPTY_STRING            5
#define CP_TYPE_EXT_STRING              6


#define SCR_MIN_INT           NW_INT32_MIN
#define SCR_MAX_INT           NW_INT32_MAX

#define SCR_MIN_FLOAT         NW_FLOAT32_MIN
#define SCR_MAX_FLOAT         NW_FLOAT32_MAX

#define MAX_CHAR_GROUP_LEN    16
#define MAX_STR_LEN           128

#define MAX_FRAME_SIZE        30
#define MAX_EXPR_DEPTH        20


/* Status Codes */

#define SCR_ERROR_NO_ERROR                          0x00
#define SCR_ERROR_VERIFICATION_FAILED               0x01
#define SCR_ERROR_FATAL_LIBRARY_FUNCTION_ERROR      0x02
#define SCR_ERROR_INVALID_FUNCTION_ARGUMENTS        0x03
#define SCR_ERROR_EXTERNAL_FUNCTION_NOT_FOUND       0x04
#define SCR_ERROR_UNABLE_TO_LOAD_COMPILATION_UNIT   0x05
#define SCR_ERROR_ACCESS_VIOLATION                  0x06
#define SCR_ERROR_STACK_UNDERFLOW                   0x07
#define SCR_ERROR_STACK_OVERFLOW                    0x08
#define SCR_ERROR_OUT_OF_MEMORY                     0x09

#define SCR_ERROR_USER_EXIT                         0x0a
#define SCR_ERROR_USER_ABORT                        0x0b

#endif  /*SCR_DEFS_H*/
