/*
* Copyright (c) 1999 Nokia Corporation and/or its subsidiary(-ies).
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

/**************************************************
File: nwx_defs_symbian.h
Description:	This file contains data type 
				information for EPOC platform.
				This file should be include in
				the main defs file, nwx_defs.h
Added by:		WMS
Date:			2nd Aug, 99
**************************************************/
#ifndef _NWX_DEFS_SYMBIAN_H_
#define _NWX_DEFS_SYMBIAN_H_

#include <e32def.h>

/*
This is the EPOC standard C header file.
Function such as 'memcpy' is here.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Rainbow has re-defined TRUE & FALSE. No longer need to undef. */
#if 0

#ifdef TRUE
#undef TRUE
/* Allow EPOC rainbow to define its own value*/
#endif

#ifdef FALSE
#undef FALSE
/* Allow EPOC rainbow to define its own value*/
#endif
#endif /* 0 */

#ifdef memcpy
#undef memcpy
/* Allow EPOC rainbow to define its own memcpy function*/
#endif

#define MAX_PATH				100

/* typedef from nwx_osu.h */
typedef void*  NW_Osu_Thread_t;
typedef void*  NW_Osu_SemaphoreHandle_t;
typedef void*  NW_Osu_Mutex_t;
typedef void*  NW_Osu_File_t;
typedef void*  NW_Osu_Signal_t;
typedef TUint32 NW_Osu_ThreadId_t;
typedef void*  NW_Osu_Hwnd_t;

/*helper funtion*/
//void NW_Trace(char* string);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif _NWX_DEFS_SYMBIAN_H_
