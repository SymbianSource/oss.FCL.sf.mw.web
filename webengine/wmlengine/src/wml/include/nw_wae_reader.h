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
* Description:  Declares structures and function to read native types from encoded bytecode.
                 The wae_reader iterates over the bytecode and reads or skips raw bytes, ints,
                 floats, boolean, and string types. 
 
*
*/


#ifndef WAE_READER_H
#define WAE_READER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"

typedef struct
{
  NW_Byte*   readerBuff;
  NW_Uint32  readerSize;       /* Allocated buffer size */
  NW_Uint32  readerPos;        /* To keep the position of the buffer */
  NW_Uint16  readerEncoding;   /* Type of encoding */
  NW_Uint32  readerBytesRead;  /* To hold number of bytes has been read */
} NW_Reader_t;


void NW_Reader_InitializeBuffReader(NW_Reader_t *r, NW_Byte *buff, 
                                    NW_Uint32  buff_size, NW_Uint16 encoding);
NW_Byte NW_Reader_UngetByte(NW_Reader_t *r);
NW_Bool NW_Reader_ReadMbyteInt(NW_Reader_t *r, NW_Int32  *res);
NW_Bool  NW_Reader_ReadMbyteBuff(NW_Int32  *val, NW_Reader_t *r);
NW_Uint32 NW_Reader_GetPos(NW_Reader_t *r);
NW_Bool NW_Reader_ReadBytes(NW_Reader_t *r, NW_Byte *buff, NW_Uint32  len); /* changed return type from void to NW_Bool. 2/16/2000,jac */
NW_Bool NW_Reader_ReadInt16(NW_Reader_t *r, NW_Int16 *res);
NW_Bool NW_Reader_ReadInt(NW_Reader_t *r, NW_Int32 *res);
NW_Float32 NW_Reader_ReadFloat(NW_Reader_t *r);
NW_Bool NW_Reader_ReadNextByte(NW_Reader_t *r, NW_Byte *b);
NW_Uint32 NW_Reader_GetStrCharCount(NW_Reader_t *r, NW_Uint32  byte_len);
NW_Bool NW_Reader_ReadChar(NW_Reader_t *r, NW_Ucs2 *c);
NW_Bool NW_Reader_ReadChars(NW_Reader_t *r, NW_Ucs2 *str, NW_Int32  len);
NW_Int32 NW_Reader_StrBuff(NW_Byte *buff, NW_Uint16 encoding);
NW_Uint32 NW_Reader_ReadUtf8CharBuff(NW_Reader_t *r, NW_Ucs2 *c);
NW_Uint32 NW_Reader_WriteUtf8CharBuff(NW_Ucs2 c, NW_Byte *buff);
NW_Uint32 NW_Reader_GetUtf8EncodedByteLength(const NW_Ucs2 *str);
NW_Uint32 NW_Reader_WriteUcs2CharBuff(NW_Ucs2 c, NW_Byte *buff);
NW_Uint32 NW_Reader_ReadIso8859_1CharBuff(NW_Reader_t *r, NW_Ucs2 *c);
NW_Uint32 NW_Reader_ReadUsAsciiCharBuff(NW_Reader_t *r, NW_Ucs2 *c);
NW_Uint32 NW_Reader_ReadCharBuff(NW_Reader_t *r, NW_Ucs2 *c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* WAE_READER_H */
