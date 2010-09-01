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


#ifndef SCR_WTA_H
#define SCR_WTA_H

#include "scr_core.h"


void populate_wta_lib(lib_function_table_t *table);

NW_Byte size_wp_lib(void);     /* public */
NW_Byte size_nokia_lib(void);  /* public */

#endif /*SCR_WTA_H*/

