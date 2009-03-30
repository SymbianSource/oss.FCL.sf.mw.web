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
    $Workfile: scr_reader.h $

    Purpose:

       This file is used by the scr_reader.c.  It defines the prototypes 
       for all the functions needed to read the ScriptServer's bytecode.
 */
 
#ifndef SCR_READER_H
#define SCR_READER_H

#include "nw_wae_reader.h"

NW_Bool read_script(NW_Reader_t *r, const NW_Ucs2 *referer, const NW_Ucs2 *url);


#endif /*SCR_READER_H*/
