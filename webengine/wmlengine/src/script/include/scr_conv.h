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
    $Workfile: scr_conv.h $

    Purpose:
 */

#ifndef SCR_CONV_H
#define SCR_CONV_H

#include "scr_val.h"

NW_Ucs2 *str2ucs2(str_t str);
str_t ucs22str(NW_Ucs2 *str);
str_t ucs2ch2str(NW_Ucs2 ch);
char *str2ascii(str_t s);
NW_Bool str2int(str_t s, NW_Int32  *res, NW_Bool *consumeAll);
NW_Bool str2float(str_t s, NW_Float32 *res, NW_Bool *consumeAll);
str_t int2str(NW_Int32  i);
str_t float2str(NW_Float32 f);
str_t bool2str(NW_Bool b);

str_t scr_format(NW_Ucs2 *format, val_t v, NW_Bool *retval);
#endif  

