/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NWX_OSU_FILE_H
#define NWX_OSU_FILE_H

#ifdef __cplusplus
extern "C" {
#endif


/*
** Includes
*/

#include "nwx_defs.h"
#include "nwx_datastruct.h"
#include "nwx_time.h"
#include "nwx_osu.h"
#include "BrsrStatusCodes.h"

/*
** Type Definitions
*/ 

/* 
**  These OSU types should be opaque to the user.
**  They are returned from, and passed to, the OSU functions
**  but they are never manipulated directly by the user.
*/


#define NW_INVALID_FILE_HANDLE ((void *)0xFFFFFFFF)


typedef struct {
  NW_Time_t creationTime;
  NW_Time_t lastAccessTime;
  NW_Time_t lastWriteTime;
  NW_Uint32    fileSize;
} NW_Osu_FileInfo_t;

/*
** Global Function Declarations
*/

/* create a new file and open it for write access */
NW_Osu_File_t NW_Osu_CreateNewFile(const NW_Ucs2 *name);

/* close an open file */
void NW_Osu_CloseFile(NW_Osu_File_t handle);

/* write bytes to a file */
TBrowserStatusCode NW_Osu_WriteFile(NW_Osu_File_t handle, NW_Byte *buffer, 
                             const NW_Uint32 numToWrite);

/* delete an existing file */
TBrowserStatusCode NW_Osu_Delete(const NW_Ucs2 *pathname);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*NWX_OSU_FILE_H*/

