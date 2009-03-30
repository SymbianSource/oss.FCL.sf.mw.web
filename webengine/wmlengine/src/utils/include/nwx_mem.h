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


/***************************************************************************
**   File: nwx_mem.h
**   Purpose:  Provides the interface to memory management
**************************************************************************/
#ifndef NWX_MEM_H
#define NWX_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include "nwx_defs.h"

/*****************************************************************
** Subsystem Name: Memory
** Description: Provides allocation and free routines for memory
******************************************************************/

/*
** Global Function Declarations
*/

/* Uncomment for memory profiling.
   This ALSO needs to be defined in rb_wae\bld\utils\include\nwx_mem.h
   so that the rbinterpreter_memprofu.def file will be used.
*/

  /*
#ifdef _DEBUG
#define FEATURE_MEMORY_PROFILE 1
#endif
*/

#ifdef FEATURE_MEMORY_PROFILE

#define NW_Mem_Malloc(x) NW_Mem_Malloc_Pro(x,__FILE__,__LINE__)
#define NW_Mem_Free(x) NW_Mem_Free_Pro(x,__FILE__,__LINE__)
#define NW_Mem_Malloc_Address &NW_Mem_Malloc_Pro
#define NW_Mem_Free_Address &NW_Mem_Free_Pro

void SendStringToLogger2(char *strFormat, NW_Ucs2 *strValue);
void SendStringToLogger1(char *strMsg);
void *NW_Mem_Malloc_Pro(NW_Uint32 nbytes, char * file, NW_Uint32 line);
void NW_Mem_Free_Pro(void *buffer, char * file, NW_Uint32 line);

#else

#define NW_Mem_Malloc(x) NW_Mem_Malloc_No_Pro(x)
#define NW_Mem_Free(x) NW_Mem_Free_No_Pro(x)
#define NW_Mem_Malloc_Address &NW_Mem_Malloc_No_Pro
#define NW_Mem_Free_Address &NW_Mem_Free_No_Pro

void *NW_Mem_Malloc_No_Pro(NW_Uint32 memorySize);
void NW_Mem_Free_No_Pro(void *mem);

#endif /* FEATURE_MEMORY_PROFILE */

void *NW_Mem_memset(void *s, NW_Uint32 c, NW_Uint32 n);
void *NW_Mem_memcpy(void *s1, const void *s2, NW_Uint32 n);
void *NW_Mem_memmove(void *s1, const void *s2, NW_Uint32 n);
NW_Int32 NW_Mem_memcmp(const void *s1, const void *s2, NW_Uint32 n);

#ifdef _DEBUG

#define NW_MEM_OOMMODE_NEVER  0
#define NW_MEM_OOMMODE_AFTER  1
#define NW_MEM_OOMMODE_RANDOM 2

NW_Uint32 NW_Mem_GetMemInUse();
NW_Uint32 NW_Mem_GetHighWaterMark();
void NW_Mem_ResetHighWaterMark();
NW_Bool NW_Mem_IsHeapConstrained();
void NW_Mem_ConstrainHeap(NW_Bool constrain);
NW_Uint32 NW_Mem_GetHeapSize();
void NW_Mem_SetHeapSize(NW_Uint32 size);
NW_Uint8 NW_Mem_GetOOMMode();
void NW_Mem_SetOOMMode(NW_Uint8 mode);
NW_Uint32 NW_Mem_GetMaxAllocs();
void NW_Mem_SetMaxAllocs(NW_Uint32 allocs);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_MEM_H */


