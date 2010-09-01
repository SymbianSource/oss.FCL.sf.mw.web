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
    $Workfile: scr_str.h $

    Purpose:
 */

#ifndef SCR_STR_H
#define SCR_STR_H

#include "scr_defs.h"


typedef NW_Ucs2* str_t;

/* Create a new length-counted ucs2 string */
str_t new_str(NW_Int32 length, NW_Ucs2 *buf);

/* Free a length-counted ucs2 string */
void free_str(str_t str);

/* Return length of length-counted ucs2 string */
NW_Int32  str_len(str_t str);

/* Return pointer to storage for length-counted ucs2 string */
NW_Ucs2* str_buf_ptr(str_t str);

/* Concatinate 2 length-counted ucs2 strings */
str_t str_cat(str_t str1, str_t str2);

/* Concatinate 3 length-counted ucs2 strings */
str_t str_cat3(str_t str1, str_t str2, str_t str3);

/* Duplicate a length-counted ucs2 string */
str_t dup_str(str_t str);

/* Compare 2 length-counted ucs2 strings */
NW_Int32  str_cmp(str_t str1, str_t str2);

/* Return the character at the given index */
NW_Ucs2 scr_charat(str_t str, NW_Int32  index);

/* Extract a substring from a length-counted ucs2 string */
str_t scr_substr(str_t str, NW_Int32  start, NW_Int32  len);

/* Find position of 2nd string in 1st string */
NW_Int32  str_find(str_t str1, str_t str2, NW_Int32  start);

NW_Int32 str_times(str_t s, NW_Ucs2 separator);
str_t str_replace(str_t s, str_t old_str, str_t newstr);
str_t str_elementat(str_t s, NW_Int32  index, NW_Ucs2 separator);
str_t str_removeat(str_t s, NW_Int32  index, NW_Ucs2 separator);
str_t str_replaceat(str_t s, str_t newstr, NW_Int32  index, NW_Ucs2 separator);
str_t str_insertat(str_t s, str_t newstr, NW_Int32  index, NW_Ucs2 separator);
str_t str_squeeze(str_t s);
str_t str_trim(str_t s);

NW_Bool str_is_empty(str_t s);
NW_Bool str_is_alpha(NW_Ucs2 c);
NW_Bool str_is_digit(NW_Ucs2 c);
NW_Bool str_is_reserved(str_t s);

#endif  

