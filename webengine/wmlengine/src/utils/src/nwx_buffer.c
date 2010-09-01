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


/*****************************************************************
**    File name:  NW_Buffer.c
**    Part of: Buffer
**    Version: 3/16/99      Initial version
**    Description:  Provides the interface to a managerd buffer of bytes.
**                  The buffer contains an allocated size and an in use 
**                  size (length)  and also a pointer to a data array.
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_buffer.h"
#include "nwx_mem.h"
#include "nwx_assert.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

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
static TBrowserStatusCode Resize_Data(NW_Buffer_t *buffer, const NW_Uint32 len)
{
  void *temp;

  if (buffer->allocatedLength < len) {
    temp = NW_Mem_Malloc(len);
    if (temp == NULL)
      return KBrsrOutOfMemory;
    NW_Mem_Free(buffer->data);
    buffer->length = 0;
    buffer->allocatedLength = len;
    buffer->data = (NW_Byte*) temp;
  }
  return KBrsrSuccess;
}


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:  NW_Buffer_New
**  Description:  creates a new buffer
**  Parameters:   size  - size to allocate
**  Return Value: pointer to the new buffer
******************************************************************/
NW_Buffer_t *NW_Buffer_New(const NW_Uint32 size)    
{
  NW_Buffer_t *buffer;

  buffer = (NW_Buffer_t*) NW_Mem_Malloc(sizeof(NW_Buffer_t));
  if (buffer != NULL) {
    buffer->length = 0;
    buffer->allocatedLength = size;
    buffer->data = NULL;
    if (size > 0) {
      buffer->data = (NW_Byte*) NW_Mem_Malloc(size);
      if (buffer->data == NULL) {
        NW_Mem_Free(buffer);
        return NULL;
      }
    }
  }
  return buffer;
}

/*****************************************************************
**  Name:  NW_Buffer_Free
**  Description:  frees the space allocated for a buffer
**  Parameters:   *buffer - the buffer to free
**  Return Value: void 
******************************************************************/
void NW_Buffer_Free(NW_Buffer_t *buffer)    
{
  if (buffer != NULL) {
    if (buffer->data != NULL) {
      NW_Mem_Free(buffer->data);
      buffer->data = NULL;
    }
    NW_Mem_Free(buffer);
  }
  return;
}

/*****************************************************************
**  Name:  NW_Buffer_FreeNotData
**  Description:  free the space allocated for a buffer. 
**  Parameters:   *buffer - the buffer to free
**  Return Value: void 
**  Notes:  This does not free the data
******************************************************************/
void NW_Buffer_FreeNotData(NW_Buffer_t *buffer)
{
  if (buffer != NULL) {
    buffer->data = NULL;
    NW_Mem_Free(buffer);
  }
  return;
}

/*****************************************************************
**  Name:  NW_Buffer_CopyStr
**  Description:  Copy NULL terminated string into buffer and adjust 
**                length.  If necessary, reallocate space.
**  Parameters:   *buffer - the buffer to copy to
**                *str - the string to copy from
**  Return Value: KBrsrSuccess if succeeds, else KBrsrOutOfMemory 
******************************************************************/
TBrowserStatusCode NW_Buffer_CopyStr(NW_Buffer_t *buffer, const NW_Ucs2 *str)
{
  NW_Uint16 len;

  NW_ASSERT(buffer != NULL);
  NW_ASSERT(str != NULL);

  len = (NW_Uint16)NW_Str_Strsize(str);
  if (Resize_Data(buffer, len) == KBrsrOutOfMemory)
    return KBrsrOutOfMemory;
  NW_Mem_memcpy(buffer->data, str, len);
  buffer->length = len;
  return KBrsrSuccess;
}

/*****************************************************************
**  Name:  NW_Buffer_CopyBuffers
**  Description:  Copy NW_Buffer into buffer (NW_Buffer) 
**                and adjust length. If necessary, reallocate space.
**  Parameters:   *to - the buffer to copy to
**                *from - the buffer to copy from
**  Return Value: KBrsrSuccess if succeeds, else KBrsrOutOfMemory 
******************************************************************/
TBrowserStatusCode NW_Buffer_CopyBuffers(NW_Buffer_t *to, const NW_Buffer_t *from)
{
  NW_ASSERT(to != NULL);
  NW_ASSERT(from != NULL);

  if (Resize_Data(to, from->length) == KBrsrOutOfMemory)
    return KBrsrOutOfMemory;
  NW_Mem_memcpy(to->data, from->data, from->length);
  to->length = from->length;
  return KBrsrSuccess;
}

/*****************************************************************
**  Name:  NW_Buffer_AppendBuffers
**  Description:  Append NW_Buffer into buffer (NW_Buffer) 
**                and adjust length. If necessary, reallocate space.
**  Parameters:   *to - the buffer to copy to
**                *from - the buffer to copy from
**  Return Value: KBrsrSuccess if succeeds, else KBrsrOutOfMemory 
******************************************************************/
TBrowserStatusCode NW_Buffer_AppendBuffers(NW_Buffer_t *to, const NW_Buffer_t *from)
  {
  NW_ASSERT( to != NULL );
  NW_ASSERT( from != NULL );

  if( to->allocatedLength < to->length + from->length ) 
    {
    void* temp;
    // resize
    temp = NW_Mem_Malloc( to->length + from->length );
    if( temp == NULL )
      {
      return KBrsrOutOfMemory;
      }
    // copy old buffer
    NW_Mem_memcpy( temp, to->data, to->length );
    // free old buffer
    NW_Mem_Free( to->data );
    // set new buffer
    to->data = (NW_Byte*)temp;
    to->allocatedLength = to->length + from->length;
    }
  NW_Mem_memcpy( to->data + to->length, from->data, from->length );
  to->length = to->length + from->length;
  return KBrsrSuccess;
  }

/*****************************************************************
**  Name:  NW_Buffer_SetData
**  Description:  Set NULL terminated ascii string into buffer and  
**                adjust length.
**  Parameters:   *buffer - the buffer to copy to
**                *str - the string to set into buffer
**  Return Value: void 
******************************************************************/
void NW_Buffer_SetData(NW_Buffer_t *buffer, char *str)
{
  NW_Uint32 len;

  NW_ASSERT(buffer != NULL);
  NW_ASSERT(str != NULL);

  if(buffer->data != NULL) {
    NW_Mem_Free(buffer->data);
  } 
  len = (NW_Asc_strlen(str) + 1) * sizeof(char);
  buffer->length = len;
  buffer->allocatedLength = len;
  buffer->data = (NW_Byte *)str;
  return;
}
