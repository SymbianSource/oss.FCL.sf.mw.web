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
    $Workfile: scr_lang.h $

    Purpose:

        This file is used by the scr_lang.c.  It defines the prototypes 
        for all the functions used by the Lang standard library.
 */
 
#ifndef SCR_LANG_H
#define SCR_LANG_H 

#include "scr_core.h"


void populate_lang_lib(lib_function_table_t *table);
NW_Byte size_lang_lib(void);

#endif /*SCR_LANG_H*/

