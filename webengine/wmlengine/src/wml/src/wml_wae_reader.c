/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines functions to read the native types (NW_Int32 , NW_Float32, etc..) from encoded 
               : bytecode. 
*
*/


#include "nw_wae_reader.h"

#include "nwx_defs.h"
#include "nwx_http_defs.h"
#include "wml_ops.h"

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/
static NW_Uint32 ReadInt16CharBuff(NW_Reader_t *r, NW_Ucs2 *c);

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
static NW_Uint32 ReadInt16CharBuff(NW_Reader_t *r, NW_Ucs2 *c)
{
  NW_Byte *buff = &r->readerBuff[r->readerPos + r->readerBytesRead];
  NW_ASSERT(c != NULL);
  NW_ASSERT(r != NULL);

  /* return 0, if an attempt to read beyond the alocated buffer */
  if((r->readerPos + r->readerBytesRead + 1) >= r->readerSize)
  {
    return 0;
  }
  *c = (NW_Ucs2)((buff[0] << 8) | buff[1]);
  return 2;
}

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
NW_Uint32 NW_Reader_ReadUtf8CharBuff(NW_Reader_t *r, NW_Ucs2 *c)
{
  NW_Byte *buff = &r->readerBuff[r->readerPos + r->readerBytesRead];
  NW_ASSERT(r != NULL);
  NW_ASSERT(c != NULL);
  
  /* return 0, if an attempt to read beyond the alocated buffer */
  if((r->readerPos + r->readerBytesRead) >= r->readerSize)
  {
    return 0;
  }

  switch (buff[0] >> 4)
  {
   case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
    /* 1 NW_Byte */
    *c = (NW_Ucs2) buff[0];
    return 1;

   case 12: case 13:
     /* return 0, if an attempt to read beyond the alocated buffer */
     if((r->readerPos + r->readerBytesRead + 1) >= r->readerSize)
     {
       return 0;
     }
     /* 2 bytes */
     if ( !((buff[1] & 0xC0) == 0x80) )
     {
       return 0;
     }
     *c = (NW_Ucs2) ( ((buff[0] & 0x1F) << 6) | (buff[1] & 0x3F) );
     return 2;
     
   case 14:
     /* return 0, if an attempt to read beyond the alocated buffer */
     if((r->readerPos + r->readerBytesRead + 2) >= r->readerSize)
     {
       return 0;
     }
     /* 3 bytes */
     if ( !(((buff[1] & 0xC0) == 0x80) || ((buff[2] & 0xC0) == 0x80)) )
       return 0;
     *c = (NW_Ucs2) ( ((buff[0] & 0x0F) << 12) |
                      ((buff[1] & 0x3F) << 6) |
                      ((buff[2] & 0x3F) << 0) );
     return 3;

   default:
     return 0; /* bad format */
  }
}


NW_Uint32 NW_Reader_WriteUcs2CharBuff(NW_Ucs2 c, NW_Byte *buff)
{
  NW_ASSERT(buff != NULL);
  buff[0] = (NW_Byte) ((0xff00 & c) >> 8);
  buff[1] = (NW_Byte) (0xff & c);
  return 2;
}

NW_Uint32 NW_Reader_WriteUtf8CharBuff(NW_Ucs2 c, NW_Byte *buff)
{
  NW_ASSERT(buff != NULL);

  if (/*(c >= 0x0000) && */ (c <= 0x007F))
  {
    /* 0x0000 - 0x007F: 1 NW_Byte UTF-8 encoding. */
    buff[0] = (NW_Byte) c;
    return 1;
  }
  else if (c > 0x07FF)
  {
    /* 0x0800 - 0xFFFF: 3 NW_Byte UTF-8 encoding. */
    buff[0] = (NW_Byte) (0xE0 | ((c >> 12) & 0x0F));
    buff[1] = (NW_Byte) (0x80 | ((c >>  6) & 0x3F));
    buff[2] = (NW_Byte) (0x80 | ((c >>  0) & 0x3F));
    return 3;
  }
  else
  {
    /* 0x0080 - 0x07ff: 2 NW_Byte UTF-8 encoding. */
    buff[0] = (NW_Byte) (0xC0 | ((c >>  6) & 0x1F));
    buff[1] = (NW_Byte) (0x80 | ((c >>  0) & 0x3F));
    return 2;
  }
}


/* This function returns the bytelength of ucs2 string encoded as UTF-8. */
NW_Uint32 NW_Reader_GetUtf8EncodedByteLength(const NW_Ucs2 *str)
{
  NW_Uint32 len = 0;
  while(*str != 0)
  {
    if( *str <= 0x07f )
    {
      len = len + 1;
    }
    else if (*str <= 0x07ff )
    {
      len = len + 2;
    }
    else 
    {
      len = len + 3;
    }
    str ++;
  }
  return len;
}


NW_Uint32  NW_Reader_ReadIso8859_1CharBuff(NW_Reader_t *r, NW_Ucs2 *c)
{
  NW_Byte *buff = &r->readerBuff[r->readerPos + r->readerBytesRead];
  NW_ASSERT(c != NULL);
  NW_ASSERT(r != NULL);

  /* return 0, if an attempt to read beyond the alocated buffer */
  if((r->readerPos + r->readerBytesRead) >= r->readerSize)
  {
    return 0;
  }
  *c = buff[0] ;
  return 1;
}

NW_Uint32 NW_Reader_ReadUsAsciiCharBuff(NW_Reader_t *r, NW_Ucs2 *c)
{
  NW_Byte *buff = &r->readerBuff[r->readerPos + r->readerBytesRead];
  NW_ASSERT(c != NULL);
  NW_ASSERT(r != NULL);

  /* return 0, if an attempt to read beyond the alocated buffer */
  if((r->readerPos + r->readerBytesRead) >= r->readerSize)
  {
    return 0;
  }

  *c = buff[0] ;
  return 1;
}


NW_Uint32  NW_Reader_ReadCharBuff(NW_Reader_t *r, NW_Ucs2 *c)
{
  NW_ASSERT(c != NULL);
  NW_ASSERT(r != NULL);

  if (r->readerEncoding == HTTP_iso_10646_ucs_2)
    return ReadInt16CharBuff(r, c);
  else if (r->readerEncoding == HTTP_utf_8)
    return NW_Reader_ReadUtf8CharBuff(r, c);
  else if (r->readerEncoding == HTTP_iso_8859_1)
    return NW_Reader_ReadIso8859_1CharBuff(r, c);
  else if (r->readerEncoding == HTTP_us_ascii)
    return NW_Reader_ReadUsAsciiCharBuff(r, c);
  else
    return 0;
}

NW_Bool NW_Reader_ReadChar(NW_Reader_t *r, NW_Ucs2 *c)
{
  NW_Uint32  byteCnt = 0;
  r->readerBytesRead = 0;
  byteCnt = NW_Reader_ReadCharBuff(r, c);
  if(byteCnt == 0)
  {
    return NW_FALSE;
  }
  r->readerPos = byteCnt + r->readerPos;
  return NW_TRUE;
}

void NW_Reader_InitializeBuffReader(NW_Reader_t *r, NW_Byte *buff, NW_Uint32  buff_size, NW_Uint16 encoding)
{
#ifdef WAE_DEBUG
  static NW_Bool logFlag = NW_FALSE;
  static const char* fileName = "C:\\Temp\\reader.bin";
  FILE* outFile;

  if ( logFlag )
  {
    outFile = fopen( fileName, "w" );
    fwrite( buff, 1, buff_size, outFile );
    fclose( outFile );
  }
#endif

  if ( encoding == HTTP_undefined )
  {
    /* Character set is undefined.  Default to Latin 1. */ 
    encoding = HTTP_iso_8859_1;
  }

  r->readerSize = buff_size;
  r->readerBuff = buff;
  r->readerPos = 0;
  r->readerEncoding = encoding;
  r->readerBytesRead = 0;
}

NW_Uint32 NW_Reader_GetPos(NW_Reader_t *r)
{
  NW_ASSERT(r != NULL);
  NW_ASSERT(r->readerPos <= r->readerSize);
  return r->readerPos;
}

NW_Uint32  NW_Reader_GetStrCharCount(NW_Reader_t *r, NW_Uint32  byte_len)
{
  NW_Uint32  chars_read = 0;
  NW_Uint32  byteCnt =0;
  NW_Ucs2    c;
  
  NW_ASSERT(r != NULL);
  r->readerBytesRead = 0;
  while ( r->readerBytesRead < byte_len )
  {
    byteCnt = NW_Reader_ReadCharBuff(r, &c);
    if ( byteCnt == 0 )
      return 0;

    r->readerBytesRead += byteCnt;
    chars_read++;
  }
  r->readerBytesRead = 0;
  return chars_read;
}

NW_Bool NW_Reader_ReadChars(NW_Reader_t *r, NW_Ucs2 *str, NW_Int32  len)
{
  NW_Int32  i;
  NW_Uint32  start = r->readerPos;
  NW_Uint32  byteCnt;

  if (len <= 0)
    return NW_TRUE;

  NW_ASSERT(str != NULL);
  NW_ASSERT(r != NULL);

  r->readerBytesRead = 0;
  for(i = 0;; i++)
  {
    byteCnt = NW_Reader_ReadCharBuff(r, &str[i]);

    if ( byteCnt == 0 )
    {
      /* Did not return a character.  Fail. */
      return NW_FALSE;
    }

    r->readerPos = byteCnt + r->readerPos;

    if (r->readerPos == (start + NW_UINT32_CAST(len)))
    {
      return NW_TRUE;
    }
    else if (r->readerPos > (start + NW_UINT32_CAST(len)))
    {
      return NW_FALSE;
    }
  }
  /* Cannot reach here */
}

/* Changed void return type to NW_Bool.  Removed NW_ASSERT's.  CHANGED: 2/16/2000, jac. */
NW_Bool NW_Reader_ReadBytes(NW_Reader_t *r, NW_Byte *buff, NW_Uint32  len)
{

  NW_ASSERT( r != NULL );
  if ( (r->readerPos + len) > r->readerSize)	/* REMOVED NW_ASSERT: report status if "run off end of buffer"*/
  {
	  return NW_FALSE;
  }
  (void)NW_Mem_memcpy(buff, &r->readerBuff[r->readerPos], len);
  r->readerPos += len;
  return NW_TRUE;
}

NW_Bool NW_Reader_ReadInt16(NW_Reader_t *r, NW_Int16 *res)
{
  NW_Byte b1;
  NW_Byte b2;

  NW_ASSERT(r != NULL);

  if (!NW_Reader_ReadNextByte(r, &b1))
  {
    return NW_FALSE;
  }
  if (!NW_Reader_ReadNextByte(r, &b2))
  {
    return NW_FALSE;
  }
  *res = (NW_Int16)((b1 << 8) | b2);
  return NW_TRUE;
}



NW_Bool  NW_Reader_ReadInt(NW_Reader_t *r,NW_Int32 *res)
{
  NW_Byte b1;
  NW_Byte b2;
  NW_Byte b3;
  NW_Byte b4;

  NW_ASSERT(r != NULL);

  if (!NW_Reader_ReadNextByte(r, &b1))
  {
    return NW_FALSE;
  }
  if (!NW_Reader_ReadNextByte(r, &b2))
  {
    return NW_FALSE;
  }
  if (!NW_Reader_ReadNextByte(r, &b3))
  {
    return NW_FALSE;
  }
  if (!NW_Reader_ReadNextByte(r, &b4))
  {
    return NW_FALSE;
  }

  *res = (b1 << 24) | (b2 << 16) | (b3 << 8)  |  b4;
  return NW_TRUE;
}


NW_Float32 NW_Reader_ReadFloat(NW_Reader_t *r)
{
  NW_Int32 tmp;
  /* "volatile" keyword is used to suppress compiler optimizer bug */
  /*       which changes store/load operation into a data-type cast */
  volatile union {
    NW_Int32    int_res;
    NW_Float32 float_res;
  } res;

  NW_ASSERT(r != NULL);

  if(!(NW_Reader_ReadInt(r, &tmp)))
  {
    return 0;
  }
  res.int_res = tmp;
  return res.float_res;
}


NW_Bool NW_Reader_ReadNextByte(NW_Reader_t *r, NW_Byte *b)
{

  if (!r || (r->readerBuff == 0) || (r->readerPos >= r->readerSize)) return NW_FALSE;

  *b = r->readerBuff[r->readerPos++];

  return NW_TRUE;
}


NW_Bool NW_Reader_ReadMbyteInt(NW_Reader_t *r, NW_Int32  *res)
{
  NW_ASSERT(r != NULL);
  if(!(NW_Reader_ReadMbyteBuff(res, r)))
  {
    return NW_FALSE;
  }
  if ( (r->readerBytesRead == 0) || (r->readerBytesRead > 5))
  {
    return NW_FALSE;
  }
  if( (r->readerPos + r->readerBytesRead) <= r->readerSize )
  {
    r->readerPos += r->readerBytesRead;
    return NW_TRUE;
  }
  return NW_FALSE;
}

NW_Bool  NW_Reader_ReadMbyteBuff(NW_Int32  *val, NW_Reader_t *r)
{
  NW_Int32  next;
  NW_Uint32 i;
  NW_Byte*  buf = &r->readerBuff[r->readerPos];

  NW_ASSERT(r);
  NW_ASSERT(val);

  for(i = 0, *val = 0;;i++, *val <<= 7)
  {
    if( (r->readerPos + i) >= r->readerSize )
    {
      r->readerBytesRead = 0;
      return NW_FALSE;
    }

    next = buf[i];
    if (!(next & 0x80))
    {
      *val |= next;
      break;
    }
    else *val |= (next & 0x7F);
  }
  r->readerBytesRead = i+1;
  return NW_TRUE;
}

