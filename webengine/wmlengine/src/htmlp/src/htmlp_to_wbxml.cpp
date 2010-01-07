/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nwx_defs.h"
#include <nw_encoder_wbxmlwriter.h>
#include "CHtmlpParser.h"
#include "nw_htmlp_to_wbxml.h"
#include "nw_htmlp_dict.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_statuscodeconvert.h"
#include "BrsrStatusCodes.h"
#include "nw_wml1x_wml_1_3_tokens.h"
#include <nw_wbxml_dictionary.h>
#include <nw_encoder_stringtable.h>
#include "nwx_logger.h"
#include "nwx_http_defs.h"

//lint -save -esym(1401, TSignal::iStatus) not initialized by constructor
#include <flogger.h>
//lint -restore

/* When encoding is UCS-2, the parser has changed the byte order to
native, however, the WBXML should be written in network (i.e., big
endian) order.  The way this is done is to change the byte order
in-place to network order, write the WBXML, and then put the byte
order back to native in-place. */
static
void
NW_HTMLP_WbxmlEncoder_Ucs2NativeToNetworkByteOrder(NW_Uint32 byteCount,
                                                   NW_Uint8* pBuf)
{
  NW_Uint32 i;
  NW_Uint16 c_ucs2 = 1;

  if (((NW_Uint8*)&c_ucs2)[0] == 1) { /* test for little endian host */
    for (i = 0; i < byteCount; i += 2) {
      (void)NW_Mem_memcpy(&c_ucs2, pBuf + i, sizeof(NW_Uint16));
      pBuf[i] = (NW_Uint8)((c_ucs2 >> 8) & 0xff);
      pBuf[i+1] = (NW_Uint8)(c_ucs2 & 0xff);
    }
  }
}

static
void
NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder(NW_Uint32 byteCount,
                                                   NW_Uint8* pBuf)
{
  NW_Uint32 i;
  NW_Uint16 c_ucs2 = 1;

  if ((byteCount & 1) == 1)
    byteCount--;
  if (((NW_Uint8*)&c_ucs2)[0] == 1) { /* test for little endian host */
    for (i = 0; i < byteCount; i += 2) {
      /* this is a clever trick: pick up bytes in big endian order,
         force the result to memory via taking address of result which
         will put the 16-bits into native byte order, then copy the
         result over the original bytes */
      c_ucs2 = (NW_Uint16)((pBuf[i] << 8) | pBuf[i+1]);
      (void)NW_Mem_memcpy(pBuf + i, &c_ucs2, sizeof(NW_Uint16));
    }
  }
}

/* end of line normalization replaces DOS or Mac style end of line
with Unix style end of line.  If no normalization is needed, then on
return *ppTextOut == pTextIn (i.e., no malloc was done), otherwise you
need to call NW_Mem_Free(*ppTextOut) when you are done with the output
NOTE: only works for native byte order!
*/
static
TBrowserStatusCode
NW_HTMLP_WbxmlEncoder_EndOfLineNormalization(NW_Uint32 encoding,
                                             NW_Uint32* pCharCount,
                                             NW_Uint32* pByteCount,
                                             const NW_Uint8* pTextIn,
                                             NW_Uint8** ppTextOut)
{
  NW_Uint32 i;
  NW_Uint32 j;
  NW_Uint32 elideCharCount;
  NW_Uint32 elideByteCount;
  NW_Int32 byteCount;
  NW_Ucs2 cUCS2;
  NW_Uint8 crHit;
  NW_Uint8 needsNormalization = 0;

  /* the output is the input unless it must be normalized */
  *ppTextOut = (NW_Uint8*)pTextIn;

  /* scan to count #xD's that need translating */
  elideCharCount = 0;
  elideByteCount = 0;
  crHit = 0;
  for (i = 0; i < *pByteCount; i += byteCount) {
    byteCount = NW_String_readChar((NW_Uint8*)&(pTextIn[i]), &cUCS2, encoding);
    if (byteCount < 0) {
      return KBrsrFailure;
    }
    /* catch DOS 0xd 0xa eol */
    if (crHit && (cUCS2 == 0xa)) {
      elideCharCount++;
      elideByteCount += byteCount;
    }
    if (cUCS2 == 0xd) {
      needsNormalization = 1;
      crHit = 1;
    } else {
      crHit = 0;
    }
  }
  if (needsNormalization) {
    /* alloc a new string */
    *ppTextOut = (NW_Uint8*)NW_Mem_Malloc(*pByteCount - elideByteCount);
    if (*ppTextOut == NULL) {
      *ppTextOut = (NW_Uint8*)pTextIn;
      return KBrsrOutOfMemory;
    }

    /* make a normalized copy of input */
    crHit = 0;
    j = 0;
    for (i = 0; i < *pByteCount; i += byteCount) {
      byteCount = NW_String_readChar((NW_Uint8*)&(pTextIn[i]), &cUCS2, encoding);
      if (byteCount < 0) {
        NW_Mem_Free(*ppTextOut);
        *ppTextOut = (NW_Uint8*)pTextIn;
        return KBrsrFailure;
      }
      /* check that readChar isn't trying to read off end of buffer */
      NW_ASSERT((i + byteCount - 1) < *pByteCount);
      if (cUCS2 == 0xd) {
        crHit = 1;
        if (byteCount == 1) {
          (*ppTextOut)[j++] = 0xa;
        } else if (byteCount == 2) {
          /* this looks odd but takes into account either endianess */
          if (pTextIn[i] == 0xd) {
            (*ppTextOut)[j++] = (NW_Uint8)0xa;
            (*ppTextOut)[j++] = 0;
          } else {
            (*ppTextOut)[j++] = 0;
            (*ppTextOut)[j++] = (NW_Uint8)0xa;
          }
        } else {
          NW_ASSERT(byteCount <= 2); /* code bug, force debug stop here */
          NW_Mem_Free(*ppTextOut);
          *ppTextOut = (NW_Uint8*)pTextIn;
          return KBrsrFailure;
        }
      } else {
        if (!(crHit && (cUCS2 == 0xa))) {
          (void)NW_Mem_memcpy(&((*ppTextOut)[j]), &(pTextIn[i]), byteCount);
          j += byteCount;
        }
        crHit = 0;
      }
    }
    *pByteCount -= elideByteCount;
    *pCharCount -= elideCharCount;
  }
  return KBrsrSuccess;
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_New(NW_Uint32 publicID,
                                      NW_Uint32 encoding, void** ppV)
{
  NW_WBXML_Dictionary_t* pDictionary;
  NW_HTMLP_WbxmlEncoder_t** ppTE = (NW_HTMLP_WbxmlEncoder_t**)ppV;
  *ppTE = (NW_HTMLP_WbxmlEncoder_t*)NW_Mem_Malloc(sizeof(NW_HTMLP_WbxmlEncoder_t));
  if (*ppTE == NULL) {
    return KBrsrOutOfMemory;
  }
  (*ppTE)->lastTagTokenIndex = 0;
  (*ppTE)->publicID = publicID;
  (*ppTE)->encoding = encoding;
  (*ppTE)->pE = (NW_WBXML_Writer_t *)NW_Mem_Malloc(sizeof(NW_WBXML_Writer_t));
  if ((*ppTE)->pE == NULL) {
    NW_Mem_Free(*ppTE);
    *ppTE = NULL;
    return KBrsrOutOfMemory;
  }
  /* Note that we only have access to one dictionary and we
  should really have two: one for tags and one for attributes. */
  pDictionary = NW_WBXML_Dictionary_getByPublicId((*ppTE)->publicID);

  NW_WBXML_Writer_Initialize((*ppTE)->pE,
                             0 /* byte count */,
                             NULL, NULL, /* no mem needed for sizing */
                             pDictionary, pDictionary,
                             NULL, NULL, NULL, NULL, NULL,
                             NW_TRUE /* do sizing only */);
    return KBrsrSuccess;
}

static
void NW_HTMLP_WbxmlEncoder_SetToWrite(
  NW_HTMLP_WbxmlEncoder_t* pTE,
  NW_Uint32 byteLength, NW_Uint8* pBuf,
  NW_WBXML_Writer_GrowBuf_t growBufCallback)
{
  if( (pTE->publicID == NW_xhtml_1_0_PublicId) && 
      ( CXML_Additional_Feature_Supprted() & CXML_DTD_SUPPORT_ON) )
  {
   NW_Encoder_StringTable_t* strTable = NW_Encoder_StringTable_new();
   

   NW_WBXML_Writer_Initialize(pTE->pE,
                             byteLength, pBuf,
                             growBufCallback,
                             pTE->pE->pTagDictionary,
							 pTE->pE->pAttributeDictionary,
                             NW_Encoder_StringTable_getStringTableOffset,
							 NW_Encoder_StringTable_addToStringTable,
                             strTable, 
							 NW_Encoder_StringTable_StringTableIterateInit,
							 NW_Encoder_StringTable_StringTableIterateNext,
                             NW_FALSE /* do sizing only */);
  }
  else
  {
   NW_WBXML_Writer_Initialize(pTE->pE,
                             byteLength, pBuf,
                             growBufCallback,
                             pTE->pE->pTagDictionary,
                             pTE->pE->pAttributeDictionary,
                             NULL, NULL, NULL, NULL, NULL,
                             NW_FALSE /* don't do sizing, write */);
}
}

static
void NW_HTMLP_WbxmlEncoder_Delete(void* pV)
{
  if (pV)
  {
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  NW_Mem_Free(pE); /* note that this doesn't free WBXML buf and string table*/
  NW_Mem_Free(pTE);
}
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_BeginDocumentCB(NW_HTMLP_Lexer_t* pL,
                                                  void* pV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;

  NW_REQUIRED_PARAM(pL);
  
  /* Note: We do not build a string table because it isn't clear if
  it is worthwhile. */

  return StatusCodeConvert(NW_WBXML_Writer_Header(pE, 3 /* wbxml version */,
                                pTE->publicID, pTE->encoding,
                                0 /* string table byte count */));
}

// The function writes the start of tag for elements name in the dictionary
// as well as non DTD elements. If the element is in the HTML dictionary then
// ASCII encoding is passed and corresponding token is written the WBXML buffer.
// If it is non dictionary element name then actual encoding is passed and WBXML 
// encoder takes care of this. Note, inside the HTML parser coding is always 
// UCs-2 so it is safe to assume charcount = bytecount/2. 

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_StartTagCB(
  NW_Uint8 byteCount,
  const NW_Uint8* pBuf,
  void* pV, NW_Bool isLiteral)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  TBrowserStatusCode s;

  if(isLiteral == NW_FALSE)
  {
  s = StatusCodeConvert(NW_WBXML_Writer_TagString(pE, HTTP_us_ascii, byteCount,
                                byteCount, (NW_Uint8*)pBuf,
                                &(pTE->lastTagTokenIndex)));
  }
  else
  {
   //
   //For handling the literals or non dictionary elements.
   //
   s = StatusCodeConvert(NW_WBXML_Writer_TagString(pE, HTTP_iso_10646_ucs_2, 
                                                   byteCount/2, byteCount, (NW_Uint8*)pBuf,
                                                   &(pTE->lastTagTokenIndex)));
  }
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  /* default to content but erase if see empty tag end */
  return StatusCodeConvert(NW_WBXML_Writer_TagSetContentFlag(pE, pTE->lastTagTokenIndex));
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_AttributeStartCB(void* pV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  return StatusCodeConvert(NW_WBXML_Writer_TagSetAttributesFlag(pE, pTE->lastTagTokenIndex));
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_AttributeNameAndValueCB(
  NW_HTMLP_Lexer_t* pL,
  const NW_HTMLP_Interval_t* pI_name,
  NW_Bool missingValue,
  const NW_HTMLP_Interval_t* pI_value,
  void* pV,
  NW_Uint32* cp_cnt)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  NW_Uint32 length;
  NW_Uint32 nameLength;
  NW_Uint8* pName;
  NW_Uint32 valueByteLength;
  NW_Uint32 valueCharCount;
  NW_Uint8* pValue;
  NW_Uint8* pValueCopy = NULL;
  NW_Uint32 i;
  NW_Int32 bytesRead;
  NW_Ucs2 c_ucs2;
  TBrowserStatusCode s;

  if (!NW_HTMLP_Interval_IsWellFormed(pI_name)) {
    return KBrsrFailure;
  }

  /* var name setup */
  length = NW_HTMLP_Interval_ByteCount(pI_name);
  nameLength = length; /* byte count */
  s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pI_name->start,
                                           &nameLength, &pName);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (nameLength != length) {
    return KBrsrFailure;
  }
  nameLength = NW_HTMLP_Interval_CharCount(pI_name); /* char count */

  /* force attribute name to lower case for A-Z only,
  this alters the doc in-place */

  /* FUTURE: unfortunately, there is no writeChar to go with the readChar
  so the work around until there is better encoding support is to only
  work with ASCII, 8859-1, UTF-8 and UCS-2.  In these encodings we can
  handle writing because only UCS-2 uses two bytes for an ASCII char. */
  NW_ASSERT((pL->encoding == HTTP_us_ascii)
            || (pL->encoding == HTTP_iso_8859_1)
            || (pL->encoding == HTTP_utf_8)
            || (pL->encoding == HTTP_iso_10646_ucs_2));
  if (!((pL->encoding == HTTP_us_ascii)
        || (pL->encoding == HTTP_iso_8859_1)
        || (pL->encoding == HTTP_utf_8)
        || (pL->encoding == HTTP_iso_10646_ucs_2))) {
    return KBrsrFailure;
  }
  for (i = 0; i < length;) {
    bytesRead = NW_String_readChar(&(pName[i]), &c_ucs2, pL->encoding);
    if (bytesRead == -1) {
      return KBrsrFailure;
    }
    /* force doc ascii uppercase to lowercase */
    if ((c_ucs2 >= (NW_Ucs2)'A') && (c_ucs2 <= (NW_Ucs2)'Z')) {
      c_ucs2 += 0x20; /* offset in ascii from upper to lower */
    }
    if (pL->encoding == HTTP_iso_10646_ucs_2) {
      NW_ASSERT(bytesRead == 2);
      /* accomodate either endianness */
      if (pName[i] == 0) {
        pName[i+1] = (NW_Uint8)c_ucs2;
      } else {
        pName[i] = (NW_Uint8)c_ucs2;
      }
    } else {
      NW_ASSERT(bytesRead == 1);
      NW_ASSERT((pL->encoding == HTTP_us_ascii)
                || (pL->encoding == HTTP_iso_8859_1)
                || (pL->encoding == HTTP_utf_8));
      pName[i] = (NW_Uint8)c_ucs2;
    }
    i += bytesRead;
  }

  /* value setup */
  if (missingValue == NW_TRUE) {
    /* assume this attribute is a boolean and force the value
    to be the same as the attribute name */
    pI_value = pI_name;
  }
  length = NW_HTMLP_Interval_ByteCount(pI_value);
  valueByteLength = length;
  s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pI_value->start,
                                           &valueByteLength, &pValue);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (valueByteLength != length) {
    return KBrsrFailure;
  }
  valueCharCount = NW_HTMLP_Interval_CharCount(pI_value);
  if (pL->encoding == HTTP_iso_10646_ucs_2) {
    // in the case that name and value are same thing (e.g "noshade")
    // we have to make an extra copy of pValue, since only pValue needs to 
    // be converted to big endian
    if (pValue == pName)
    {
      pValueCopy = (NW_Byte*)NW_Mem_Malloc(valueByteLength);
      if (pValueCopy == NULL)
        return KBrsrOutOfMemory;
      (void)NW_Mem_memcpy(pValueCopy, pValue, valueByteLength);
      pValue = pValueCopy;
    }
    NW_HTMLP_WbxmlEncoder_Ucs2NativeToNetworkByteOrder(valueByteLength,
                                                       pValue);
  }

  // !!! Note, pValue is epected to be UCS2 and big endian!!!
  //
  // The rest of the code you see around here that checks the encoding is code
  // that has not been cleaned up properly since the conversion to always use
  // UCS2.
  //
  // Make a copy of the value, if it's not already a copy, and remove
  // extraneous CRs and LFs.
  if (!pValueCopy) {
    pValueCopy = (NW_Byte*)NW_Mem_Malloc(valueByteLength);
    if (pValueCopy == NULL)
      return KBrsrOutOfMemory;
    (void)NW_Mem_memcpy(pValueCopy, pValue, valueByteLength);
    pValue = pValueCopy;
  }
  NW_Ucs2* pSrc = (NW_Ucs2*)pValue;
  NW_Ucs2* pDst = pSrc;
  NW_Uint32 newCharCount = 0;
  for (i = 0; i < valueCharCount; i++) {
    c_ucs2 = *pSrc;
    // Check for big endian CRs and LFs.
    if ((c_ucs2 != 0x0d00) && (c_ucs2 != 0x0a00)) {
      *pDst++ = c_ucs2;
      newCharCount++;
    }
    pSrc++;
  }
  valueCharCount = newCharCount;
  // Byte count is always twice the char count since chars are always UCS2.
  valueByteLength = valueCharCount << 1;

  // !!! Note, here we expect pName to be little Endian and pValue to be big endian!!!
  s = StatusCodeConvert(NW_WBXML_Writer_AttributeAndValue2(pE, pTE->encoding, nameLength,
                                        pName, valueCharCount,
                                        valueByteLength, pValue, cp_cnt));
  if (BRSR_STAT_IS_FAILURE(s)) {
    goto finish_attrNameVal;
  }
  if (pL->encoding == HTTP_iso_10646_ucs_2) {
    NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder(valueByteLength,
                                                       pValue);
  }
  s = KBrsrSuccess;
finish_attrNameVal:
  if (pValueCopy != 0)
    NW_Mem_Free(pValueCopy);
  return s;
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_AttributesEndCB(NW_Uint32 attributeCount,
                                                  void* pV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  TBrowserStatusCode s = KBrsrSuccess;
  /* if there were any attributes, then must write end token */
  if (attributeCount) {
    s = StatusCodeConvert(NW_WBXML_Writer_End(pE));
    if (BRSR_STAT_IS_FAILURE(s)) {
      return s;
    }
  }
  return s;
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_CoreTextHandler(NW_Uint32 encoding,
                                                  NW_Uint32 charCount,
                                                  NW_Uint32 byteLength,
                                                  NW_Uint8* pText,
                                                  NW_WBXML_Writer_t* pE)
{
  NW_Uint8* pNormalizedText;
  TBrowserStatusCode s;

  s = NW_HTMLP_WbxmlEncoder_EndOfLineNormalization(encoding,
                                                   &charCount,
                                                   &byteLength,
                                                   pText,
                                                   &pNormalizedText);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (encoding == HTTP_iso_10646_ucs_2) {
    NW_HTMLP_WbxmlEncoder_Ucs2NativeToNetworkByteOrder(byteLength,
                                                       pNormalizedText);
  }
  s = StatusCodeConvert(NW_WBXML_Writer_Text(pE, encoding, byteLength, pNormalizedText));
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (pText == pNormalizedText) {
    if (encoding == HTTP_iso_10646_ucs_2) {
      NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder(byteLength,
                                                         pNormalizedText);
    }
  } else {
    NW_Mem_Free(pNormalizedText);
  }
  return KBrsrSuccess;
}

/*
 * Handles the special case of all NULLs in the content. All NULLs would cause
 * false code page switch. It is important that this function not filter out
 * whitespace chars (0x09, 0x0A, 0x0D, 0x20, ...) as it had done in the past
 * by filtering out all control chars (<= 0x20). It only needs to check for
 * NULLs, which is the only thing that can cause a code-page switch.
 */
static void NW_HTMLP_SPL_Handle(
    NW_Uint32 charLength, NW_Ucs2* pContent, NW_Bool* onlySplChar)
{
  *onlySplChar = NW_TRUE;
  for (NW_Uint32 i = 0; i < charLength; i++, pContent++)
  {
    if (*pContent)
    {
      *onlySplChar = NW_FALSE;
      break;
    }
  }
 return;
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_ContentCB(
  NW_HTMLP_Lexer_t* pL,
  const NW_HTMLP_Interval_t* pI_content,
  void* pV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  TBrowserStatusCode s;
  NW_Uint32 length;
  NW_Uint32 byteLength;
  NW_Uint32 charCount;
  NW_Uint8* pContent;
  NW_Bool   onlySplChar;

  if (!NW_HTMLP_Interval_IsWellFormed(pI_content)) {
    return KBrsrFailure;
  }
  length = pI_content->stop - pI_content->start;
  byteLength = length;
  s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pI_content->start,
                                           &byteLength,
                                           &pContent);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (byteLength != length) {
    return KBrsrFailure;
  }

  /* A NULL char in the contents can sometimes cause false code-page switch. 
   * Therefore, do not write them in the WBXML buffer.
   */
  NW_ASSERT((byteLength & 0x0001) == 0); // byte count must be even (always UCS2)
  NW_HTMLP_SPL_Handle((byteLength >> 1), (NW_Ucs2*)pContent, &onlySplChar);

  if(onlySplChar == NW_TRUE)
  {
   return KBrsrSuccess;
  }

  charCount = pI_content->charStop - pI_content->charStart;

  
  return NW_HTMLP_WbxmlEncoder_CoreTextHandler(pL->encoding,
                                               charCount,
                                               byteLength,
                                               pContent,
                                               pE);
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_EndTagCB(
  NW_Uint8 asciiCharCount,
  const NW_Uint8* pBuf,
  NW_Bool emptyTag,
  void* pV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  TBrowserStatusCode s;
  NW_REQUIRED_PARAM(asciiCharCount);
  NW_REQUIRED_PARAM(pBuf);
  if (emptyTag) {
    s = StatusCodeConvert(NW_WBXML_Writer_TagClearContentFlag(pE, pTE->lastTagTokenIndex));
  } else {
    s = StatusCodeConvert(NW_WBXML_Writer_End(pE));
  }
  return s;
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_CdataCB(
  NW_HTMLP_Lexer_t* pL,
  const NW_HTMLP_Interval_t* pI_cdata,
  void* pV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  NW_Uint8* pText;
  NW_Uint8* pQuotedText;
  NW_Uint8* pSrc;
  NW_Uint8* pDest;
  NW_Uint32 encoding = pL->encoding;
  NW_Uint32 length;
  NW_Uint32 byteLength;
  NW_Uint32 charCount;
  NW_Uint32 ampCount;
  NW_Uint32 i;
  NW_Uint32 j;
  NW_Ucs2 cUCS2;
  TBrowserStatusCode s;

  /* code only supports the following charset values */
  NW_ASSERT((encoding == HTTP_us_ascii)
            || (encoding == HTTP_iso_8859_1)
            || (encoding == HTTP_utf_8)
            || (encoding == HTTP_iso_10646_ucs_2));

  charCount = pI_cdata->charStop - pI_cdata->charStart;
  length = pI_cdata->stop - pI_cdata->start;
  if (length < 1) {
    return KBrsrSuccess;
  }
  byteLength = length;
  s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pI_cdata->start,
                                           &byteLength,
                                           &pText);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (byteLength != length) {
    return KBrsrFailure;
  }

  /* Cdata is not supposed to have any entity expansion done on it.
     Since we don't alter the original document text in general, we
     have done the entity expansion outside of cXML.  Therefore, we
     must quote any ampersands in the Cdata to prevent later entity
     expansion. */

  /* count ampersands */
  pSrc = pText;
  ampCount = 0;
  for (i = 0; i < charCount; i++) {
    j = (NW_Uint32)NW_String_readChar(pSrc, &cUCS2, pL->encoding);
    pSrc += j;
    /* don't need to check j < 1, text has already been validated */
    if (cUCS2 == '&') {
      ampCount++;
    }
  }

  pQuotedText = pText;
  if (ampCount) {
    /* allocate a new buffer

    need incremental length for: & => &amp;
    start with single byte charset value */
    ampCount *= 4;
    /* double for ucs2 */
  if (pL->encoding == HTTP_iso_10646_ucs_2) {
      ampCount *= 2;
    }
    pQuotedText = (NW_Uint8*)NW_Mem_Malloc(byteLength + ampCount);
    if (pQuotedText == NULL) {
      return KBrsrOutOfMemory;
    }

    /* copy and quote ampersands */
    pSrc = pText;
    pDest = pQuotedText;
    for (i = 0; i < charCount; i++) {
      j = (NW_Uint32)NW_String_readChar(pSrc, &cUCS2, encoding);
      /* don't need to check for byteCount < 1, text has already been
         validated */
      NW_Mem_memcpy(pDest, pSrc, j);
      pSrc += j;
      pDest += j;
      if (cUCS2 == '&') {
        if (encoding == HTTP_iso_10646_ucs_2) {
          /* double byte */
          *((NW_Uint16*)pDest) = 'a';
          *((NW_Uint16*)(pDest + 2)) = 'm';
          *((NW_Uint16*)(pDest + 4)) = 'p';
          *((NW_Uint16*)(pDest + 6)) = ';';
          pDest += 8;
        } else {
          /* single byte encodings */
          NW_Mem_memcpy(pDest, "amp;", 4);
          pDest += 4;
        }
     }
  }
    byteLength += ampCount;
  if (pL->encoding == HTTP_iso_10646_ucs_2) {
      charCount += (ampCount / 2);
    } else {
      charCount = byteLength;
    }
  }

  /* output the text */
  s = NW_HTMLP_WbxmlEncoder_CoreTextHandler(pL->encoding,
                                            charCount,
                                            byteLength,
                                            pQuotedText,
                                            pE);
  if (pQuotedText != pText) {
    NW_Mem_Free(pQuotedText);
  }
  return s;
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_PiFormCB(NW_HTMLP_Lexer_t* pL,
                                           const NW_HTMLP_Interval_t* pPITarget,
                                           const NW_HTMLP_Interval_t* pArguments,
                                           void* pV,
                                           NW_Uint32* cp_cnt)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)pV;
  NW_WBXML_Writer_t* pE = pTE->pE;
  NW_Uint32 length;
  NW_Uint32 pitargetByteCount;
  NW_Uint32 pitargetCharCount;
  NW_Uint8* pPITargetData;
  NW_Uint32 argumentsByteCount;
  NW_Uint32 argumentsCharCount;
  NW_Uint8* pArgumentsData;
  TBrowserStatusCode s;

  /* prepare PITarget */
  length = NW_HTMLP_Interval_ByteCount(pPITarget);
  if (length < 1) {
    return KBrsrSuccess;
  }
  pitargetByteCount = length;
  s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pPITarget->start,
                                           &length,
                                           &pPITargetData);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (pitargetByteCount != length) {
    return KBrsrFailure;
  }
  pitargetCharCount = NW_HTMLP_Interval_CharCount(pPITarget);

  /* prepare Arguements */
  argumentsByteCount = argumentsCharCount = 0;
  pArgumentsData = NULL;
  length = NW_HTMLP_Interval_ByteCount(pArguments);
  if (length > 0) {
    argumentsByteCount = length;
    s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pArguments->start,
                                             &length,
                                             &pArgumentsData);
    if (BRSR_STAT_IS_FAILURE(s)) {
      return s;
    }
    if (argumentsByteCount != length) {
      return KBrsrFailure;
    }
    argumentsCharCount = NW_HTMLP_Interval_CharCount(pArguments);
  }

  /* start PI sequence */
  s = StatusCodeConvert(NW_WBXML_Writer_PI(pE));
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (pL->encoding == HTTP_iso_10646_ucs_2) {
    NW_HTMLP_WbxmlEncoder_Ucs2NativeToNetworkByteOrder(argumentsByteCount,
                                                       pArgumentsData);
  }
  s = StatusCodeConvert(NW_WBXML_Writer_AttributeAndValue2(pE, pTE->encoding, 
                                        pitargetCharCount, pPITargetData,
                                        argumentsCharCount,
                                        argumentsByteCount, pArgumentsData, cp_cnt));
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (pL->encoding == HTTP_iso_10646_ucs_2) {
    NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder(argumentsByteCount,
                                                       pArgumentsData);
  }
  return StatusCodeConvert(NW_WBXML_Writer_End(pE));
}

static
TBrowserStatusCode NW_HTMLP_WbxmlEncoder_EndDocumentCB(NW_HTMLP_Lexer_t* pL,
                                                       TBrowserStatusCode  e,
                                                       void* pV)
{
  /* When debugging, this func provides a place to capture the final WBXML. */
  NW_REQUIRED_PARAM(pL);
  
   if (e != KBrsrSuccess) 
   {
    return e;
   }

  NW_REQUIRED_PARAM(pV);
  return KBrsrSuccess;
}

static 
void NW_HTHMLP_StrTbl(NW_HTMLP_EventCallbacks_t* eventCBs,
                                                           void** WBXMLEncStrTbl)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*) eventCBs->pClientPointer;

  //Keep the copy of the WBXML string table, so that it can be added to the
  //DOM later. 
   if( ( (NW_Bool) (CXML_Additional_Feature_Supprted() & CXML_DTD_SUPPORT_ON) ) &&
       (pTE->publicID == NW_xhtml_1_0_PublicId) )
   {
     NW_Uint32 stringTableByteCount = 0;
     NW_Encoder_StringTable_t* strTable = (NW_Encoder_StringTable_t*)
           ((NW_HTMLP_WbxmlEncoder_t*)eventCBs->pClientPointer)->pE->pStringTableObject;
     if(strTable)
     {
       stringTableByteCount = NW_Encoder_StringTable_getTotalBytes(strTable);
       if( (stringTableByteCount == 0) || (WBXMLEncStrTbl == NULL) )
       {
        //Empty Table
        NW_Encoder_StringTable_delete(strTable);
        if(WBXMLEncStrTbl)
        {
         *WBXMLEncStrTbl = NULL;
        }
       }
       else
       {
        if(WBXMLEncStrTbl)
        {
        *WBXMLEncStrTbl = (void*) strTable;
        }
       }//end else
    }//end if(strTable)
     else
     {
      if(WBXMLEncStrTbl)
        {
         *WBXMLEncStrTbl = NULL;
        }
     }
   }//end if((NW_Bool) CXML_Additional_Feature_Supprted() ) 
   else
   {
    if(WBXMLEncStrTbl)
    {
     *WBXMLEncStrTbl = NULL;
    }
   }
 return;
}//end NW_HTHMLP_StrTbl()


// install the event callbacks for real parsing
static void installEventCBs(NW_HTMLP_EventCallbacks_t* eventCBs)
{
  eventCBs->beginDocumentCB = NW_HTMLP_WbxmlEncoder_BeginDocumentCB;
  eventCBs->endDocumentCB = NW_HTMLP_WbxmlEncoder_EndDocumentCB;
  eventCBs->startTagCB = NW_HTMLP_WbxmlEncoder_StartTagCB;
  eventCBs->endTagCB = NW_HTMLP_WbxmlEncoder_EndTagCB;
  eventCBs->contentCB = NW_HTMLP_WbxmlEncoder_ContentCB;
  eventCBs->cdataCB = NW_HTMLP_WbxmlEncoder_CdataCB;
  eventCBs->attributeStartCB = NW_HTMLP_WbxmlEncoder_AttributeStartCB;
  eventCBs->attributeNameAndValueCB
    = NW_HTMLP_WbxmlEncoder_AttributeNameAndValueCB;
  eventCBs->attributesEndCB = NW_HTMLP_WbxmlEncoder_AttributesEndCB;
  eventCBs->piFormCB = NW_HTMLP_WbxmlEncoder_PiFormCB;
}

static TBrowserStatusCode NW_HTHMLP_RealParse(CHtmlpParser * parser, 
                                               NW_Bool docComplete, void** WBXMLEncStrTbl);

TBrowserStatusCode NW_HTMLP_NotifyDocComplete(void * pParser, NW_Uint32* encoding, 
    void** WBXMLEncStrTbl, NW_HTMLP_NotifyDocCompleteCB* callBack, void* callBackContext)
  {
  CHtmlpParser*       parser = (CHtmlpParser *) pParser;
  NW_Uint8*           pWbxmlNewBuf = NULL;
  NW_Buffer_t*        residueWbxml = NULL;
  TBrowserStatusCode  s = KBrsrSuccess;
  
  // If parser is NULL nothing else needs to be done.
  if (parser == NULL)
    { 
    return s;
    }
  
  // If the previous wbxml buffer was not generated (meaning
  // the real parsing never started) or it needs to fix up an unterminated comment then
  // parse it (again) and extract the convert wbxml.
  if (!parser->getPreviousValidOutput() || parser->iTrackingUnTerminatedComment)
    {
    s = NW_HTHMLP_RealParse(parser, NW_TRUE, WBXMLEncStrTbl);

    if (BRSR_STAT_IS_SUCCESS(s) && parser->iWbxmlWriter->index)
      {
      pWbxmlNewBuf = (NW_Uint8*)NW_Mem_Malloc(parser->iWbxmlWriter->index);
      if (pWbxmlNewBuf != NULL) 
        {
        // Copy data from pWbxmlBuf to pWbxmlNewBuf.
        (void)NW_Mem_memcpy(pWbxmlNewBuf, parser->iWbxmlWriter->pBuf, parser->iWbxmlWriter->index);

        // Free pWbxmlBuf.
        NW_Mem_Free(parser->iWbxmlWriter->pBuf);
        residueWbxml = (NW_Buffer_t*)NW_Mem_Malloc(sizeof(NW_Buffer_t));
        residueWbxml->data = pWbxmlNewBuf;
        residueWbxml->length = parser->iWbxmlWriter->index;
        *encoding = parser->getLexer()->encoding;
        }
      else 
        {
        s = KBrsrOutOfMemory;
        }
      }
    }

  if (BRSR_STAT_IS_SUCCESS(s) && residueWbxml)
    {
    s = (*callBack)(callBackContext, residueWbxml, *encoding, 
        parser->getCodePageSwitchCount(), parser->getLastValid(), *WBXMLEncStrTbl);

    residueWbxml = NULL;
    }

  // Release parser
  NW_HTMLP_EventCallbacks_t* eventCBs = parser->getEventCallbacks();
  if (eventCBs)
      {
      if (eventCBs->pClientPointer)
          {
            NW_Mem_Free(((NW_HTMLP_WbxmlEncoder_t*)(eventCBs->pClientPointer))->pE);
          }
      NW_Mem_Free(eventCBs->pClientPointer);
      }
  delete parser;
  return s;
  }

TBrowserStatusCode HTMLP_HtmlToWbxml(NW_Buffer_t* pInBuf,
                                 NW_Uint32 encoding, NW_Uint8 byteOrder,
                                 NW_Buffer_t** ppOutBuf, NW_Uint32* pLine,
                                 NW_Uint32 publicID,
                                 NW_HTMLP_ElementTableIndex_t elementCount,
                                 NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                                 NW_Bool consumeSpaces,
                                 NW_Bool needCharsetDetect,
                                 void* charsetConvertContext,
                                 NW_HTMLP_CharsetConvertCB* charsetConvertCallback,
                                 NW_Bool isScript)
{
  NW_Int32 lastValid = 0;
  NW_Uint32 cp_count = 0;
  void* pParser = 0;
  return NW_HTMLP_SegToWbxml(pInBuf, encoding, byteOrder, ppOutBuf, pLine, publicID, elementCount,
                            pElementDictionary, consumeSpaces, needCharsetDetect, NW_TRUE, NW_TRUE, &lastValid, &pParser, &cp_count,
                            charsetConvertContext, charsetConvertCallback, NULL, isScript);
}


TBrowserStatusCode NW_HTMLP_SegToWbxml(NW_Buffer_t* pInBuf, 
                                       NW_Uint32 encoding,
                                       NW_Uint8 byteOrder,
                                       NW_Buffer_t** ppOutBuf, 
                                       NW_Uint32* pLine,
                                       NW_Uint32 publicID,
                                       NW_HTMLP_ElementTableIndex_t elementCount,
                                       NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                                       NW_Bool consumeSpaces,
                                       NW_Bool needCharsetDetect,  /* need to detect charset or not */
                                       NW_Bool isFirstSegment,     /* is this the first segment of a document? */
                                       NW_Bool isLastSegment,      /* is this the last segment of a document? */
                                       NW_Int32 *pLastValid,
                                       void** ppParser,
                                       NW_Uint32 *pCodePageSwitchCount,
                                       void* charsetConvertContext,
                                       NW_HTMLP_CharsetConvertCB* charsetConvertCallback,
                                       void** WBXMLEncStrTbl,
                                       NW_Bool isScript)
{
  NW_Uint8* pWbxmlBuf = NULL;             // wbxml buffer as output
  NW_Uint32 wbxmlBufLength = 0;           // wbxml buffer's length
  NW_Uint32 allocatedLength = 0;
  TBrowserStatusCode s = KBrsrSuccess;
  NW_Buffer_t* lastTextBuff = NULL;       // last piece of text left from previous chunck
  NW_Uint32 lastTextBuffLen = 0;          // length of lastTextBuff
  NW_Buffer_t* previousValidOutput = NULL;  // previously generated wbxml buffer, not including the manually added close tags
  NW_Uint32 previousValidOutputLen = 0;   // length of previousValidOutput
  NW_Buffer_t* visitedHeadText = NULL;    // concatinated web content segment that has been visited by fake parse
  // should be original content (without charconv)
  CHtmlpParser * parser = NULL;           // the parser instance that can be reused for parsing continuous chunks
  NW_Bool in_parse = NW_FALSE;
  NW_Bool freeNeeded = NW_FALSE;
  NW_HTMLP_EventCallbacks_t* eventCBs = NULL;
  NW_Int32 numUnconvertible, indexFirstUnconvertible;
  NW_Buffer_t * origBuf = NULL;           // left bytes + incoming buffer (before charconv)
  NW_Buffer_t* newInBuf = NULL;           // left bytes + incoming buffer (after charconv) 
  NW_Uint8* docStartAddress = NULL;       // web content segment that parser needs to parse
  // i.e. previousLeftText + left bytes + incoming buffer (after charconv) 
  NW_Uint32 docByteLength = 0;            // web content segment length
  NW_Uint32 numLeftBytes = 0;              // indicate if number of bytes left from incomplete multi-byte character
  
  NW_Byte*  restoreInData = pInBuf->data;
  *pLine = 0;
  *ppOutBuf = NULL;
  
  if(WBXMLEncStrTbl)
  {
    *WBXMLEncStrTbl = NULL;
  }
  
  
  NW_ASSERT(ppParser);
  
  // make sure sender sends along charsetConvertCallback and charsetConvertContext 
  // if encoding is not ucs2 (in this case charset transcoding is needed
  // or need charset detection (in this case charset determination is needed
  if (encoding != HTTP_iso_10646_ucs_2 || needCharsetDetect)
  {
    NW_ASSERT(charsetConvertCallback);
    NW_ASSERT(charsetConvertContext);
  }

  if ((pInBuf == NULL) || (ppOutBuf == NULL) || (pLine == NULL)) {
    return KBrsrBufferTooSmall;
  }
  if ((pInBuf->length == 0) || (pInBuf->data == NULL)) {
    return KBrsrBufferTooSmall;
  }
  
  origBuf = pInBuf;  // initialize origBuf to pInBuf, later may need to insert left bytes
  
  if (!isFirstSegment)
  {
    // if it's not the first segement, make sure that parser instance is passed in
    NW_ASSERT(*ppParser);
    parser = (CHtmlpParser *) *ppParser;
    
    // since content handler does not know if charset is detected by parser from previous chunk,
    // check if parser's iOrigEncoding is set.
    if (needCharsetDetect && parser->iOrigEncoding != -1)
    {
      encoding = parser->iOrigEncoding;
      needCharsetDetect = NW_FALSE;
    }
    
    /* get the last text from previous segment and append it the current segment */
    lastTextBuff = parser->getLastTextBuf();
    if (lastTextBuff) 
      lastTextBuffLen = lastTextBuff->length;
    previousValidOutput = parser->getPreviousValidOutput();
    if (previousValidOutput)
      previousValidOutputLen = previousValidOutput->length;
    
    
    // if there were some bytes left by the incomplete character, insert them in the origBuf
    if (parser->iLeftBytes != NULL)
    {
      
      origBuf = (NW_Buffer_t *)NW_Buffer_New(pInBuf->length + parser->iLeftBytes->length);
      if (origBuf == NULL)
      {
        s = KBrsrOutOfMemory;
        goto handle_error;
      }
      NW_Mem_memcpy(origBuf->data, parser->iLeftBytes->data, parser->iLeftBytes->length);
      NW_Mem_memcpy(origBuf->data + 1, pInBuf->data, pInBuf->length);
      origBuf->length = pInBuf->length + parser->iLeftBytes->length;
      NW_Buffer_Free(parser->iLeftBytes);
      parser->iLeftBytes = NULL;
    }
  }
  
  // if needed, do the charset conversion
  newInBuf = origBuf;  // initialize outBuf to same as origBuf, i.e. left bytes
  
  if (encoding == HTTP_iso_10646_ucs_2)
  {
    if (origBuf->length & 1)
    {
      origBuf->length--;
      numLeftBytes = 1;
    }
  }
  
  // now try to convert the web content buffer to UCS2 encoded content
  if (encoding != HTTP_iso_10646_ucs_2) 
  {
    // to get rid of BOM for encodings other than UCS2
    s = NW_HTMLP_HandleBOM(encoding, byteOrder, &(origBuf->data), &(origBuf->length));
    if (s != KBrsrSuccess)
      goto handle_error;

    numUnconvertible = 0;
    indexFirstUnconvertible = -1;
    
    s = charsetConvertCallback(charsetConvertContext, 0, 0, origBuf, &numUnconvertible, 
      &indexFirstUnconvertible, &newInBuf, &encoding);
    byteOrder = NW_BYTEORDER_BIG_ENDIAN;
    if (s != KBrsrSuccess)
    {
      // if encoding was indicated as utf-8 but the conversion to UCS2 failed, that probably means
      // it's actually Latin-1 (a lot of Finish pages are in this case, so an assumption is made
      // and content is converted from Latin-1 to UCS2.
      if (encoding == HTTP_utf_8 || s == KBrsrMvcConverterNotFound)
      {
        if (s == KBrsrMvcConverterNotFound)
        {
          NW_LOG1(NW_LOG_LEVEL2, "SegToWbxml, charsetConvertCallback returns ConverterNotFound status while converting from encoding: %x", encoding);
        }
        NW_LOG1(NW_LOG_LEVEL2, "SegToWbxml, failed while converting from encoding: %x, now treat text as latin-1 and retry.", encoding);
        encoding = HTTP_iso_8859_1; 
        s = charsetConvertCallback(charsetConvertContext, 0, 0, origBuf, &numUnconvertible, 
          &indexFirstUnconvertible, &newInBuf, &encoding);
        byteOrder = NW_BYTEORDER_BIG_ENDIAN;
        if (s != KBrsrSuccess)
          goto handle_error;
        else if (parser)
        {
          parser->iOrigEncoding = HTTP_iso_8859_1;
        }
      }
      else 
      {
        goto handle_error;
      }
    }
    NW_ASSERT(newInBuf);
    
    // check for unconvertable bytes for possible last imcomplete character at end of chunk
    if (numUnconvertible && indexFirstUnconvertible >= (NW_Int32)(origBuf->length-2))
    {
      //pInBuf->length -= numUnconvertible;
      numLeftBytes = numUnconvertible;
    }
  }
  
  if (origBuf != pInBuf && origBuf != newInBuf)
  {
    NW_Buffer_Free(origBuf);
    origBuf = NULL;
  }
  
  // this time it will perform the byte conversion
  s = NW_HTMLP_HandleBOM(HTTP_iso_10646_ucs_2, byteOrder, &(newInBuf->data), &(newInBuf->length));
  if (s != KBrsrSuccess)
    goto handle_error;
  
  docByteLength = newInBuf->length + lastTextBuffLen;
  if (lastTextBuff)
  {
    docStartAddress = (NW_Uint8*)NW_Mem_Malloc(docByteLength);
    if (!docStartAddress)
    {
      s = KBrsrOutOfMemory;
      goto handle_error;
    }
    freeNeeded = NW_TRUE;
    (void)NW_Mem_memcpy(docStartAddress, lastTextBuff->data, lastTextBuff->length);
    (void)NW_Mem_memcpy(docStartAddress + lastTextBuff->length, newInBuf->data, newInBuf->length);
  }
  else 
  {
    docStartAddress = newInBuf->data;
  }
  
  /* malloc the output buffer */
  *ppOutBuf = (NW_Buffer_t*)NW_Mem_Malloc(sizeof(NW_Buffer_t));
  if (*ppOutBuf == NULL) {
    s = KBrsrOutOfMemory;
    goto handle_error;
  }
  
  (*ppOutBuf)->length = 0;
  (*ppOutBuf)->allocatedLength = 0;
  (*ppOutBuf)->data = NULL;
  
  /* Create new buffer to write to, only when real parsing start */
  if (!needCharsetDetect)
  {
    /* ASSUMPTION: the wbxmlBufLength is <= the document length + 1000bytes*/
    wbxmlBufLength = docByteLength + previousValidOutputLen + 1000;
    
    /* Create new buffer to write to */
    pWbxmlBuf = (NW_Uint8*)NW_Mem_Malloc(wbxmlBufLength);
    if (pWbxmlBuf == NULL) {
      s = KBrsrOutOfMemory;
      goto handle_error;
    }
    else if (previousValidOutput)
    {
      (void)NW_Mem_memcpy(pWbxmlBuf, previousValidOutput->data, previousValidOutputLen);
    }
  }
  
  /* Parse and generate WBXML. */
  TRAP(s, parser = CHtmlpParser::NewL(docByteLength, docStartAddress, elementCount,
    pElementDictionary, HTTP_iso_10646_ucs_2, consumeSpaces, parser, isScript));
  if (s != KBrsrSuccess) 
    goto handle_error;
  *ppParser = parser;
  
  if (numLeftBytes)
  {
    // allocate a new buffer to hold bytes for incomplete characters and keep it in parser
    parser->iLeftBytes = NW_Buffer_New(numLeftBytes);
    (void)NW_Mem_memcpy(parser->iLeftBytes->data, pInBuf->data + pInBuf->length - numLeftBytes, numLeftBytes);
    parser->iLeftBytes->length = numLeftBytes;
  }
  
  // append the plain web content segement to visitedHeadText
  if (needCharsetDetect)
  {
    parser->appendVisitedHeadText(pInBuf->data, pInBuf->length);
  }
  
  if (isFirstSegment && !needCharsetDetect)
  {
    parser->iOrigEncoding = encoding;
  }
  
  eventCBs = parser->getEventCallbacks();
  // if charsetConvertCallback is set, we don't actually generate wbxml output,
  // instead, we keep all other callbacks to be null
  if (!needCharsetDetect && isFirstSegment)
  {
    installEventCBs(eventCBs);
  }
  
  /* The following are not WBXML callbacks, but CompositeContentHandler callback and context */
  eventCBs->charsetConvertCallback = charsetConvertCallback;
  if (charsetConvertContext )
  {
        NW_Htmlp_CharsetConvContext * ctx = (NW_Htmlp_CharsetConvContext*)eventCBs->charsetContext;
        if (ctx == NULL)
            {
    // create a ctx instance so that it will persists for next chunks as well
            ctx = (NW_Htmlp_CharsetConvContext *)NW_Mem_Malloc(sizeof(NW_Htmlp_CharsetConvContext));
        if (!ctx)
        {
          s = KBrsrOutOfMemory;
          goto handle_error;
        }
            }
    
    ctx->contentHandler =  ((NW_Htmlp_CharsetConvContext*)charsetConvertContext)->contentHandler;
    ctx->response = ((NW_Htmlp_CharsetConvContext*)charsetConvertContext)->response;
    eventCBs->charsetContext = ctx;
  }
  else
  {
        NW_Mem_Free(eventCBs->charsetContext);
    eventCBs->charsetContext = charsetConvertContext;
  }
  /* The encoder contains a WBXML_Writer that defaults to a sizing pass. */
  if (!needCharsetDetect)
  {
    NW_HTMLP_WbxmlEncoder_New(publicID, HTTP_iso_10646_ucs_2, &(eventCBs->pClientPointer));
    if (!eventCBs->pClientPointer)
    {
      s = KBrsrOutOfMemory;
      goto handle_error;
    }
    
    NW_HTMLP_WbxmlEncoder_SetToWrite(
      (NW_HTMLP_WbxmlEncoder_t*)eventCBs->pClientPointer,
      wbxmlBufLength, pWbxmlBuf + previousValidOutputLen, NULL /* no growth callback */);
  }
  
  in_parse = NW_TRUE;
  s = parser->NW_HTMLP_Parse(isFirstSegment, isLastSegment, needCharsetDetect);
  if (s != KBrsrSuccess && s != KBrsrRestartParsing)
    goto handle_error;
  in_parse = NW_FALSE;
  
  if (freeNeeded)
  {
    NW_Mem_Free(docStartAddress);
    docStartAddress = 0;
  }
  if (newInBuf != pInBuf)
  {
    NW_Buffer_Free(newInBuf);
    newInBuf = NULL;
  }
  
  /* if status is restartParsing, restart parse from document start with all CBs set */
  if (s == KBrsrRestartParsing)
  {
    numLeftBytes = 0;
    
    parser->iRestarted = NW_TRUE;
    encoding = parser->getLexer()->encoding;
    visitedHeadText = parser->getVisitedHeadText();
    
    // if needed, do the charset conversion
    newInBuf = visitedHeadText;  // initialize outBuf to same as origBuf
    
    if (encoding == HTTP_iso_10646_ucs_2)
    {
      if (visitedHeadText->length & 1)
      {
        newInBuf = visitedHeadText;
        newInBuf->length--;
        numLeftBytes = 1;
          parser->iLeftBytes = NW_Buffer_New(numLeftBytes);
          (void)NW_Mem_memcpy(parser->iLeftBytes->data, 
                              visitedHeadText->data + visitedHeadText->length - numLeftBytes, 
                              numLeftBytes);
          parser->iLeftBytes->length = numLeftBytes;
      }
    }
    else
    {
      // remove the possible BOM bytes
      numUnconvertible = 0;
      indexFirstUnconvertible = -1;
      s = NW_HTMLP_HandleBOM(encoding, byteOrder, &(visitedHeadText->data), &(visitedHeadText->length));      
      if (s != KBrsrSuccess)
        goto handle_error;
  
      s = charsetConvertCallback(charsetConvertContext, 0, 0, visitedHeadText, &numUnconvertible, 
        &indexFirstUnconvertible, &newInBuf, &encoding);
      byteOrder = NW_BYTEORDER_BIG_ENDIAN;
      if (s != KBrsrSuccess)
      {
        if (encoding == HTTP_utf_8 || s == KBrsrMvcConverterNotFound)
        {
          if (s == KBrsrMvcConverterNotFound)
          {
            NW_LOG1(NW_LOG_LEVEL2, "SegToWbxml, charsetConvertCallback returns ConverterNotFound status while converting from encoding: %x", encoding);
          }
          NW_LOG1(NW_LOG_LEVEL2, "SegToWbxml, failed while converting from encoding: %x, now treat text as latin-1 and retry.", encoding);
          encoding = HTTP_iso_8859_1;
          s = charsetConvertCallback(charsetConvertContext, 0,0, origBuf, &numUnconvertible, &indexFirstUnconvertible, 
                                      &newInBuf, &encoding);
          byteOrder = NW_BYTEORDER_BIG_ENDIAN;
          if (s!= KBrsrSuccess)
            goto handle_error;
          else if (parser)
          {
            parser->iOrigEncoding = HTTP_iso_8859_1;
          }
        }
        else
        {
          goto handle_error;
        }
        NW_ASSERT(newInBuf);  
      }
      else if (numUnconvertible && indexFirstUnconvertible >= (NW_Int32)(visitedHeadText->length-2))
      {
        numLeftBytes = numUnconvertible;
        //visitedHeadText->length -= numLeftBytes;
        if (numLeftBytes)
        {
          // allocate a new buffer to hold bytes for incomplete characters and keep it in parser
          parser->iLeftBytes = NW_Buffer_New(numLeftBytes);
          (void)NW_Mem_memcpy(parser->iLeftBytes->data, 
                              visitedHeadText->data + visitedHeadText->length - numLeftBytes, 
                              numLeftBytes);
          parser->iLeftBytes->length = numLeftBytes;
        }
      }
    }
    NW_ASSERT(newInBuf);  
    
    s = NW_HTMLP_HandleBOM(HTTP_iso_10646_ucs_2, byteOrder, &(newInBuf->data), &(newInBuf->length));
    if (s != KBrsrSuccess) 
      goto handle_error;

    
    wbxmlBufLength = newInBuf->length + 1000;
    pWbxmlBuf = (NW_Uint8*)NW_Mem_Malloc(wbxmlBufLength);
    if (pWbxmlBuf == NULL) {
      s = KBrsrOutOfMemory;
      goto handle_error;
    }
   
    TRAP(s, parser = CHtmlpParser::NewL(newInBuf->length, newInBuf->data, elementCount,
      pElementDictionary, HTTP_iso_10646_ucs_2, consumeSpaces, parser, isScript));
    *ppParser = parser;
    if (s != KBrsrSuccess) 
      goto handle_error;
    
    // now that real parse starts, set all the CBs, and reset charsetConvertCB
    installEventCBs(eventCBs);
    
    //eventCBs->charsetConvertCallback = NULL;
    //eventCBs->charsetContext = NULL;
    
    // now recreate and set the encode and writer
    NW_HTMLP_WbxmlEncoder_New(publicID, HTTP_iso_10646_ucs_2, &(eventCBs->pClientPointer));
    if (!eventCBs->pClientPointer)
    {
      s = KBrsrOutOfMemory;
      goto handle_error;
    }
    NW_HTMLP_WbxmlEncoder_SetToWrite(
      (NW_HTMLP_WbxmlEncoder_t*)eventCBs->pClientPointer,
      wbxmlBufLength, pWbxmlBuf, NULL /* no growth callback needed */);
    
    in_parse = NW_TRUE;
    s = parser->NW_HTMLP_Parse(NW_TRUE, isLastSegment, NW_FALSE);
    if (s != KBrsrSuccess)
      goto handle_error;
    in_parse = NW_FALSE;
    
    if (newInBuf != visitedHeadText)
    {
      NW_Buffer_Free(newInBuf);
      newInBuf = NULL;
    }
  }
  
  allocatedLength = wbxmlBufLength;
  // only when real parse starts, do we generate wbxml buffer
  if (allocatedLength)
  {
  wbxmlBufLength = previousValidOutputLen +
    ((NW_HTMLP_WbxmlEncoder_t*)eventCBs->pClientPointer)->pE->index;
  }
  if (wbxmlBufLength < allocatedLength)
  {
    NW_Uint8* pWbxmlNewBuf;
    
    pWbxmlNewBuf = (NW_Uint8*)NW_Mem_Malloc(wbxmlBufLength);
    if (pWbxmlNewBuf != NULL) 
    {
      /* copy data from pWbxmlBuf to pWbxmlNewBuf */
      (void)NW_Mem_memcpy(pWbxmlNewBuf, pWbxmlBuf, wbxmlBufLength);
      /* free pWbxmlBuf */
      NW_Mem_Free(pWbxmlBuf);
      pWbxmlBuf = pWbxmlNewBuf;
      allocatedLength = wbxmlBufLength;
    }
    else 
    {
      s = KBrsrOutOfMemory;
      goto handle_error;
    }
  }
  
  if (wbxmlBufLength != NULL)
  {
  (*ppOutBuf)->length = wbxmlBufLength;
  (*ppOutBuf)->allocatedLength = allocatedLength;
  (*ppOutBuf)->data = pWbxmlBuf;

   NW_HTHMLP_StrTbl(eventCBs, WBXMLEncStrTbl);

  }//end if (wbxmlBufLength != NULL)
  else
  {
    NW_Buffer_Free(*ppOutBuf);
    *ppOutBuf = NULL;
    s = KBrsrBufferTooSmall;
  }
  NW_HTMLP_WbxmlEncoder_Delete(eventCBs->pClientPointer);
  eventCBs->pClientPointer = NULL;

#ifdef _DEBUG1
if (*ppOutBuf)
  RFileLogger::HexDump( _L("Browser"), _L("Wbxml_buffer.txt"), EFileLoggingModeAppend, 
    NULL, NULL, (*ppOutBuf)->data, (*ppOutBuf)->length );
#endif // _DEBUG1
  
  *pLastValid = parser->getLastValid();
  *pCodePageSwitchCount = parser->getCodePageSwitchCount();
  //set the lexer's encoding the original foreign encoding so next parse would know.
  parser->getLexer()->encoding = encoding;
  
  if (isLastSegment)
  {
    delete parser;
    *ppParser = NULL;
  }
  pInBuf->data = restoreInData;
  return s;
handle_error:
  NW_Mem_Free(*ppOutBuf);
  *ppOutBuf = NULL;
  NW_Mem_Free(pWbxmlBuf);
  if ( in_parse)
  {
    NW_Uint32 column;
    NW_HTMLP_Lexer_GetLineColumn(parser->getLexer(), pLine, &column);
    NW_LOG2(NW_LOG_LEVEL1, "HTML Parse error:  line %d column %d\n",
      NW_INT32_CAST(*pLine), NW_INT32_CAST(column));
  }
  delete parser;
  *ppParser = NULL;
  parser = NULL;
  if (freeNeeded) 
  {
  if (docStartAddress && (docByteLength > pInBuf->length))
  {
    NW_Mem_Free(docStartAddress);
  }
  }
  pInBuf->data = restoreInData;
  return s;
}

TBrowserStatusCode NW_HTMLP_HtmlToWbxml(NW_Buffer_t* pInBuf, NW_Uint32 encoding,
                                 NW_Uint8 byteOrder,
                                 NW_Buffer_t** ppOutBuf, NW_Uint32* pLine,
                                 NW_Uint32 publicID,
                                 NW_HTMLP_ElementTableIndex_t elementCount,
                                 NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                                 NW_Bool consumeSpaces, 
                                 NW_Bool needCharsetDetect,
                                 void* charsetConvertContext,
                                 NW_HTMLP_CharsetConvertCB* charsetConvertCallback,
                                 NW_Bool isScript)
{
  TBrowserStatusCode status;
  
  status = HTMLP_HtmlToWbxml(pInBuf, encoding, byteOrder, ppOutBuf, pLine, publicID, elementCount, 
      pElementDictionary, consumeSpaces, needCharsetDetect, charsetConvertContext,
      charsetConvertCallback, isScript);

  if (status == KBrsrRestartParsing || *ppOutBuf == NULL)
  {
    status = HTMLP_HtmlToWbxml(pInBuf, encoding, byteOrder, ppOutBuf, pLine, publicID, elementCount, 
        pElementDictionary, consumeSpaces, NW_FALSE, charsetConvertContext, charsetConvertCallback, isScript);

#ifdef _DEBUG1
    if (*ppOutBuf)
    {
      RFileLogger::HexDump( _L("Browser"), _L("ParserDump.txt"), EFileLoggingModeAppend, 
          NULL, NULL, (*ppOutBuf)->data, (*ppOutBuf)->length );
    }
#endif // _WINS_
  }
  return status;
}


// For UTF-8 and UCS-2 handle BOM (byte order mark) -- remove BOM.  Also, for UCS-2, handle
// network (big endian) to native (little endian) byte order conversion.
TBrowserStatusCode NW_HTMLP_HandleBOM(NW_Uint32 encoding, NW_Uint8 byteOrder,
  NW_Byte** pDocStartAddress, NW_Uint32* pDocByteLength)
{  
  NW_Byte* docStartAddress = *pDocStartAddress;
  NW_Uint32 docByteLength = *pDocByteLength;

  if (encoding == HTTP_utf_8)
  {
    // Look for BOM and remove if found
    if (docByteLength >= 3)
    {
      if ((docStartAddress[0] == 0xef) &&
          (docStartAddress[1] == 0xbb) &&
          (docStartAddress[2] == 0xbf))
      {
        docByteLength -= 3;
        docStartAddress += 3;
      }
    }
  }
  else if (encoding == HTTP_iso_10646_ucs_2)
  {
    // Verify the doc has an even number of bytes, check LSB != 1.
    if ((docByteLength & 1) == 1)
    {
      return KBrsrFailure;
    }

    // Remove the BOM if present.
    if (docByteLength >= 2)
    {
      if ((docStartAddress[0]== 0xff && docStartAddress[1] == 0xfe)
        || (docStartAddress[0]== 0xfe && docStartAddress[1] == 0xff))
      {
        docByteLength -= 2;
        docStartAddress += 2;
      }
    }

    // Convert to little-endian (native byte order) if content is big-endian.
    if (byteOrder == NW_BYTEORDER_BIG_ENDIAN)
    {
      // Make sure there is something to convert.
      if (docByteLength >= 2)
      {
        // Sanity check before converting. Make sure the data looks like big
        // endian.  This is very rudimentary and does not guarantee that it is
        // big endian but on a percentage basis will cover a lot.
        NW_ASSERT(docStartAddress[1] != 0);

        // Convert the bytes.
        NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder(
            docByteLength, docStartAddress);
      }
    }
  }

  // Verify there is some doc remaining after BOM processing */
  if (docByteLength == 0)
  {
    return KBrsrFailure;
  }
  *pDocStartAddress = docStartAddress;
  *pDocByteLength = docByteLength;
  return KBrsrSuccess;
}

static TBrowserStatusCode NW_HTHMLP_RealParse(CHtmlpParser * parser, NW_Bool docComplete,
                                                           void** WBXMLEncStrTbl)
{
  NW_Uint32 encoding;
  NW_Buffer_t* visitedHeadText = NULL;
  NW_Buffer_t* newInBuf = NULL;           // ucs2 after convert the visited text 
  NW_Uint32 wbxmlBufLength = 0;
  NW_Uint8* pWbxmlBuf = NULL;
  NW_HTMLP_EventCallbacks_t* eventCBs = NULL;
  TBrowserStatusCode s = KBrsrSuccess;
  NW_HTMLP_Lexer_t* pL = NULL;
  NW_Int32 numUnconvertible, indexFirstUnconvertible; 

  NW_ASSERT(parser);

  pL = parser->getLexer();
  
  parser->iRestarted = NW_TRUE;
  encoding = parser->getLexer()->encoding;
  visitedHeadText = parser->getVisitedHeadText();
  if (!visitedHeadText)
  {
    s = KBrsrFailure;
    goto cleanup;
  }
  newInBuf = visitedHeadText;
  
  eventCBs = parser->getEventCallbacks();
  
  NW_ASSERT(eventCBs->charsetConvertCallback);
  NW_ASSERT(eventCBs->charsetContext);
  
  // if the encoding coming is anything other than ucs2, we need to do
  // the char conversion.
  if (encoding != HTTP_iso_10646_ucs_2)
  {
    numUnconvertible = 0;
    indexFirstUnconvertible = -1;
    s = NW_HTMLP_HandleBOM(encoding, NW_BYTEORDER_SINGLE_BYTE, &(visitedHeadText->data), &(visitedHeadText->length));      
    if (s != KBrsrSuccess)
      goto cleanup;
    
    s = eventCBs->charsetConvertCallback(eventCBs->charsetContext, 0, 0, visitedHeadText, &numUnconvertible, 
      &indexFirstUnconvertible, &newInBuf, &encoding);
    if (s != KBrsrSuccess)
    {
      if (encoding == HTTP_utf_8 || s == KBrsrMvcConverterNotFound)
      {
        if (s == KBrsrMvcConverterNotFound)
        {
          NW_LOG1(NW_LOG_LEVEL2, "SegToWbxml, charsetConvertCallback returns ConverterNotFound status while converting from encoding: %x", encoding);
        }
        NW_LOG1(NW_LOG_LEVEL2, "SegToWbxml, failed while converting from encoding: %x, now treat text as latin-1 and retry.", encoding);
        encoding = HTTP_iso_8859_1;
        s = eventCBs->charsetConvertCallback(eventCBs->charsetContext, 0,0, visitedHeadText, &numUnconvertible, &indexFirstUnconvertible, 
          &newInBuf, &encoding);
        if (s!= KBrsrSuccess)
          goto cleanup;
        else if (parser)
        {
          parser->iOrigEncoding = HTTP_iso_8859_1;
        }
      }
      else
      {
        goto cleanup;
      }
    }
  }
  NW_ASSERT(newInBuf);
  s = NW_HTMLP_HandleBOM(HTTP_iso_10646_ucs_2, NW_BYTEORDER_BIG_ENDIAN, &(newInBuf->data), &(newInBuf->length));
  if (s != KBrsrSuccess) 
    goto cleanup;
  
  wbxmlBufLength = newInBuf->length + 1000; // add additional 50 bytes so that plaintext (without tags) can be handled
  pWbxmlBuf = (NW_Uint8*)NW_Mem_Malloc(wbxmlBufLength);
  if (pWbxmlBuf == NULL) {
    s = KBrsrOutOfMemory;
    goto cleanup;
  }
  
  TRAP(s, parser = CHtmlpParser::NewL(newInBuf->length, newInBuf->data, pL->elementCount,
    pL->pElementDictionary, HTTP_iso_10646_ucs_2, parser->iIsHtml?NW_FALSE:NW_TRUE, parser, parser->iIsScript));
  if (s != KBrsrSuccess) 
    goto cleanup;
  
  // now that real parse starts, set all the CBs, and reset charsetConvertCB
  installEventCBs(eventCBs);
  
  // now recreate and set the encode and writer
  NW_HTMLP_WbxmlEncoder_New(parser->iIsHtml?NW_xhtml_1_0_PublicId:NW_Wml_1_3_PublicId, 
    HTTP_iso_10646_ucs_2, &(eventCBs->pClientPointer));
  if (!eventCBs->pClientPointer)
  {
    s = KBrsrOutOfMemory;
    goto cleanup;
  }
  NW_HTMLP_WbxmlEncoder_SetToWrite(
    (NW_HTMLP_WbxmlEncoder_t*)eventCBs->pClientPointer,
    wbxmlBufLength, pWbxmlBuf, NULL /* no growth callback needed */);
  
  s = parser->NW_HTMLP_Parse(NW_TRUE, docComplete, NW_FALSE);
  if (s != KBrsrSuccess)
    goto cleanup;
  
  NW_HTHMLP_StrTbl(eventCBs, WBXMLEncStrTbl);

cleanup:
  if (newInBuf != visitedHeadText)
  {
    NW_Buffer_Free(newInBuf);
  }
  return s;
}
