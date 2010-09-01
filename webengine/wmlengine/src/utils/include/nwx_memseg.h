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
**   File: nwx_memseg.h
**   Purpose:  Provides the interface to the memory segment management
**************************************************************************/

#ifndef NWX_MEMSEG_H
#define NWX_MEMSEG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include "nwx_defs.h"

/*
** Definitions
*/
#define NW_MEM_SEGMENT_INVALID   -1
#define NW_MEM_SEGMENT_MANUAL     0
#define NW_MEM_SEGMENT_LAST       1

/*
** Type Definitions
*/ 

typedef NW_Int32  NW_Mem_Segment_Id_t;

/*
** Global Function Declarations - Memory Segment
*/

/* Create a new memory segment */
NW_Mem_Segment_Id_t NW_Mem_Segment_New(void);

/* Free a memory segment and all the allocations made from it */
void NW_Mem_Segment_Free(const NW_Mem_Segment_Id_t sid);

/* Allocate memory from an existing segment */
void* NW_Mem_Segment_Malloc(const NW_Uint32  size, const NW_Mem_Segment_Id_t sid);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_MEMSEG_H */
