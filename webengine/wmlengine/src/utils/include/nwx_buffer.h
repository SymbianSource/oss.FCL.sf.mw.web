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
**   File: nwx_buffer.h
**   Purpose:  Provides the interface to a managerd buffer of bytes.
**              The buffer contains an allocated size and an in use size (length)
**              and also a pointer to a data array.
**************************************************************************/
#ifndef NWX_BUFFER_H
#define NWX_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include "nwx_defs.h"
#include "BrsrStatusCodes.h"

/*
** Type Declarations
*/
typedef struct {
  NW_Uint32  length;           /* length of the used part of the buffer */
  NW_Uint32  allocatedLength;  /* How much space was really allocated */
  NW_Byte    *data;            /* the real data */
} NW_Buffer_t;

/*
** Global Function Declarations
*/

/* create a buffer struct and allocate some space */
NW_Buffer_t *NW_Buffer_New(const NW_Uint32 size);

/* free the space allocated for the buffer */
void NW_Buffer_Free(NW_Buffer_t *buffer);

/* free the space allocated for the buffer, do not free the data */
void NW_Buffer_FreeNotData(NW_Buffer_t *buffer);

/*copy string into buffer */
TBrowserStatusCode NW_Buffer_CopyStr(NW_Buffer_t *buffer, const NW_Ucs2 *str);

/*copy NW_buffer to NW_Buffer*/
TBrowserStatusCode NW_Buffer_CopyBuffers(NW_Buffer_t *to, const NW_Buffer_t *from);

/*append NW_buffer to NW_Buffer*/
TBrowserStatusCode NW_Buffer_AppendBuffers(NW_Buffer_t *to, const NW_Buffer_t *from);

/*set ascii string into buffer */
void NW_Buffer_SetData(NW_Buffer_t *buffer, char *str);


#define NW_Buffer_Len(buf)  ((buf)->length)
#define NW_Buffer_Data(buf) ((buf)->data)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_BUFFER_H */
