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
* Description:  Implementation a simple file system interface for EPOC platform.
*
*/


/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <e32def.h>
#include "nwx_string.h"
#include "nwx_osu_file.h"
#include "nwx_logger.h"
#include "nwx_settings.h"

#include <f32file.h>
#include <sysutil.h>
#include "BrsrStatusCodes.h"


/*
**-------------------------------------------------------------------------
**  Defines 
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/
typedef struct {
  RFile*  file;
  RFs*    fs;
} NW_File_Handle_t;

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/


/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
/*****************************************************************
**  Name:   NW_File_Handle_Free
**  Description:  Free File Handle structure and its contents 
**  Parameters:   file_handle 
**  Return Value: none 
**                
******************************************************************/
void  NW_File_Handle_Free(NW_File_Handle_t *file_handle)
{
  if (file_handle)
  {
    if (file_handle->file)
    {
      file_handle->file->Close();
      delete file_handle->file;
    }
    if (file_handle->fs)
    {
      file_handle->fs->Close();
      delete file_handle->fs;
    }
    NW_Mem_Free(file_handle); 
  }
}

/*****************************************************************
**  Name:   NW_Bad_Handle
**  Description:  Check for valid File Handle pointer and contents 
**  Parameters:   file_handle 
**  Return Value: NW_TRUE - if handle is not valid
**                NW_FALSE - if handle is valid 
******************************************************************/
NW_Bool NW_Bad_Handle(NW_Osu_File_t handle)
{
  NW_File_Handle_t *file_handle = (NW_File_Handle_t *) handle;

  if (!file_handle)
    return NW_TRUE;

  if (file_handle == NW_INVALID_FILE_HANDLE)
    return NW_TRUE;

  if (!file_handle->fs)
    return NW_TRUE;
    
  if (!file_handle->file)
    return NW_TRUE;

  return NW_FALSE;
}


/*****************************************************************
**  Name:   NW_File_Handle_new
**  Description:  Allocate File Handle structure and its contents 
**  Parameters:   none 
**  Return Value: NULL - if allocation failed
**                pointer to alloc'd file handle structure 
******************************************************************/
NW_File_Handle_t* NW_File_Handle_New(void)
{
  NW_File_Handle_t  *file_handle;

  file_handle = (NW_File_Handle_t *) NW_Mem_Malloc(sizeof(NW_File_Handle_t));
  if (file_handle == NULL)
  {
    return NULL;
  }

  file_handle->fs = new RFs;
  if (file_handle->fs == NULL)
  {
    NW_Mem_Free(file_handle);
    return NULL;
  }

  file_handle->file = new RFile;
  if (file_handle->file == NULL)
  {
    delete file_handle->fs;
    NW_Mem_Free(file_handle);
    return NULL;
  }

  return file_handle;
}


/*****************************************************************
**  Name:   NW_ConvertFilename
**  Description:  Convert all "/" in filename to "\" since EPOC
**                File operations don't like "/" chars. Prepends a
**                '\' character if the filename includes directories,
**                and doesn't already start with one.
**  Parameters:   pointer to incoming filename string 
**  Return Value: NULL - if allocation failed
**                pointer - to new filename string **needs to be freed by caller
******************************************************************/
NW_Ucs2 *NW_ConvertFilename(const NW_Ucs2 *filename)
  {
  NW_Ucs2 *tmp = NULL;
  
  if (filename == NULL)
    return NULL;
  
  // If there is already a '\' or '/' or a 'c:\' at the front, or there are
  // no directories in the path, then just convert the '/' characters.
  // Otherwise first prepend a '\'.
  if ( filename[0] == '\\'  || filename[0] == '/' || NW_Str_Strchr(filename, ':') ||
    ( !NW_Str_Strchr(filename, '\\') && !NW_Str_Strchr(filename, '/') ) )
    {
    tmp = NW_Str_Newcpy(filename);
    if (tmp == NULL)
      return NULL;
    }
  else
    {
    tmp = NW_Str_New(NW_Str_Strlen(filename) + 1);
    if (tmp == NULL)
      return NULL;
    tmp[0] = '\\';
    NW_Str_Strcpy(&(tmp[1]), filename);
    }
  
  NW_Str_Strcharreplace(tmp, '/', '\\');
  return tmp;
  }

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:   NW_Osu_CreateNewFile
**  Description:  Create a new file and open it for write access.
**  Parameters:   *name - pathname of file
**  Return Value: file handle if file created, otherwise NW_INVALID_FILE_HANDLE
******************************************************************/
NW_Osu_File_t NW_Osu_CreateNewFile(const NW_Ucs2 *name)
{

  NW_Ucs2 *tmp = NW_ConvertFilename(name);
  if (tmp == NULL)
  {
    return NW_INVALID_FILE_HANDLE;
  }

  NW_Osu_File_t handle = NW_INVALID_FILE_HANDLE ;
  TPtrC filename(tmp);

  NW_File_Handle_t  *file_handle = NW_File_Handle_New();
  if (file_handle == NULL)
  {
    NW_Str_Delete(tmp);
    return NW_INVALID_FILE_HANDLE;
  }

  RFs *fs = file_handle->fs;
  RFile *file = file_handle->file;
  
  TInt  result = fs->Connect();
  if (result)
  {
    NW_File_Handle_Free(file_handle);
    NW_Str_Delete(tmp);
    return NW_INVALID_FILE_HANDLE;
  }

  if (( result = file->Create(*fs, filename, EFileShareExclusive)) == KErrNone)
    handle = (NW_Osu_File_t) file_handle;
  else
    NW_File_Handle_Free(file_handle);
  
  NW_Str_Delete(tmp);
  return handle;
}

/*****************************************************************
**  Name:   NW_Osu_CloseFile
**  Description:  Close an open file.
**  Parameters:   handle - file handle
**  Return Value: void
******************************************************************/
void NW_Osu_CloseFile(NW_Osu_File_t handle)
{
  if (NW_Bad_Handle(handle))
    return;

  NW_File_Handle_t *file_handle = (NW_File_Handle_t *) handle;
  
  NW_File_Handle_Free(file_handle);

  return;
}

/*****************************************************************
**  Name:   NW_Osu_WriteFile
**  Description:  Write bytes to a file.
**  Parameters:   handle - file handle returned from NW_Osu_CreateNewFile
**                *buffer - pointer to buffer to write from
**                numToWrite - number of bytes to write
**  Return Value: KBrsrSuccess if all bytes written, else failure status
******************************************************************/
TBrowserStatusCode NW_Osu_WriteFile(NW_Osu_File_t handle, NW_Byte *buffer, const NW_Uint32 numToWrite)
{
  if (NW_Bad_Handle(handle))
    return KBrsrFailure;

  NW_File_Handle_t *file_handle = (NW_File_Handle_t *) handle;

  NW_LOG1(NW_LOG_LEVEL1, "===writeFile===: before checking critical level. numToWrite=%d", numToWrite);
  /* Disk CL check */
  TBool statusCL = EFalse;
  RFs *fs = file_handle->fs;

  TRAPD(ret, statusCL = SysUtil::DiskSpaceBelowCriticalLevelL (fs, numToWrite, EDriveC) )
  if (ret != KErrNone || statusCL)
  {
	  NW_LOG3(NW_LOG_LEVEL1, "===writeFile===: critical level reached. ret=%d, statusCL=%d, numToWrite=%d", ret, statusCL, numToWrite);
	  return KBrsrFileDiskFullError;
  }
  NW_LOG3(NW_LOG_LEVEL1, "===writeFile===: critical level NOT reached. ret=%d, statusCL=%d, numToWrite=%d", ret, statusCL, numToWrite);

  NW_ASSERT(buffer != NULL);

  if (numToWrite == 0)
    return KBrsrSuccess;

  RFile *file = file_handle->file;

  /* Note: if buffer size < numToRead, then "write_buffer" will panic 
  */
  TPtrC8 write_buffer(buffer,numToWrite);
  TInt result = file->Write( write_buffer);
  if (result == KErrNone)
  {
    result = file->Flush();  // ensure data is commited to file before closing
  }

  switch (result) {
  case KErrNone:
    return KBrsrSuccess;
  case KErrDiskFull:
    return KBrsrFileDiskFullError;
  default:
    return KBrsrFileWriteError;
  }
}

/*****************************************************************
**  Name:   NW_Osu_Delete
**  Description:  Delete an existing file.
**  Parameters:   *pathname - pathname of file
**  Return Value: KBrsrSuccess if file deleted, else KBrsrFailure
******************************************************************/
TBrowserStatusCode NW_Osu_Delete(const NW_Ucs2 *pathname)
{
  NW_ASSERT(pathname != NULL);


  NW_Ucs2 *tmp = NW_ConvertFilename(pathname);
  if (tmp == NULL)
    return KBrsrFailure;

  RFs   fs;
  TInt  result = fs.Connect();
  if (result)
  {
    NW_Str_Delete(tmp);
    return KBrsrFailure;
  }

  TPtrC filename(tmp);

  result = fs.Delete(filename);
  fs.Close();
  if (result)
  {
    NW_Str_Delete(tmp);
    return KBrsrFailure;
  }
  else
  {
    NW_Str_Delete(tmp);
    return KBrsrSuccess;
  }
}



