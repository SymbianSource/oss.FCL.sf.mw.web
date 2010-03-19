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


/*

Lexer for HTML parser.

*/
#ifndef FEA_RME_NOHTMLPARSER

#include "nwx_defs.h"
#include "nw_htmlp_lexer.h"
#include "nw_string_char.h"
#include "BrsrStatusCodes.h"

/* Does not copy the buffer */
TBrowserStatusCode NW_HTMLP_Lexer_InitFromBuffer(NW_HTMLP_Lexer_t* pL,
                                          NW_Uint32 byteCount,
                                          NW_Uint8* pBuf,
                                          NW_HTMLP_ElementTableIndex_t elementCount,
                                          NW_HTMLP_ElementDescriptionConst_t* pElementDictionary)
{
  if ((byteCount == 0) || (pBuf == NULL)) {
    return KBrsrFailure;
  }
  pL->encoding = 0;
  pL->endianness = NW_NATIVE_ENDIAN;
  pL->readPosition = 0;
  pL->charPosition = 0;
  pL->lineColumn.crCount = 0;
  pL->lineColumn.lfCount = 0;
  pL->lineColumn.charsSinceLastCR = 0;
  pL->lineColumn.charsSinceLastLF = 0;
  pL->end = NW_FALSE;
  pL->byteCount = byteCount;
  pL->pBuf = pBuf;
  pL->elementCount = elementCount;
  pL->pElementDictionary = pElementDictionary;
  return KBrsrSuccess;
}

/* Returns a pointer in *ppData into pBuf at some byte position
and byte count, byte count is truncated to fit in pBuf if required. */
TBrowserStatusCode NW_HTMLP_Lexer_DataAddressFromBuffer(NW_HTMLP_Lexer_t* pL,
                                                 NW_Uint32 byteIndex,
                                                 NW_Uint32* pByteCount,
                                                 NW_Uint8** ppData)
{
  NW_ASSERT(byteIndex < pL->byteCount);
  NW_ASSERT(*pByteCount <= pL->byteCount);
  NW_ASSERT((byteIndex + *pByteCount) <= pL->byteCount);
  *ppData = NULL;
  if (byteIndex < pL->byteCount) {
    *ppData = pL->pBuf + byteIndex;
    if ((byteIndex + *pByteCount) > pL->byteCount) {
      *pByteCount = pL->byteCount - byteIndex;
    }
    return KBrsrSuccess;
  }
  return KBrsrFailure;
}

/* peekOrAdvance: first arg "advance": peek = NW_FALSE, advance = NW_TRUE */
static
TBrowserStatusCode NW_HTMLP_Lexer_PeekOrAdvanceOffset(NW_Bool advance,
                                               NW_HTMLP_Lexer_t* pL,
                                               NW_Uint32 offsetCharCount,
                                               NW_Uint32* pC,
                                               NW_Bool* pEOF)
{
  NW_Uint32 i;
  NW_Uint32 charCount = 0;
  NW_Int32 byteCount = 0;
  NW_Uint32 crCount = 0;
  NW_Uint32 lfCount = 0;
  NW_Uint32 charsPastCR = 0;
  NW_Uint32 charsPastLF = 0;
  NW_Ucs2 c_ucs2;
  NW_Bool resetPastCR = 0;
  NW_Bool resetPastLF = 0;

  /* it makes no sense to advance by 0 */
  NW_ASSERT((advance == NW_FALSE) || (offsetCharCount > 0));

  if (NW_HTMLP_Lexer_AtEnd(pL)) {
    *pEOF = NW_TRUE;
    return KBrsrSuccess;
  }
  *pEOF = NW_FALSE;
  i = pL->readPosition;
  do {
    /* It is assumed that this func returns UNICODE code points. */
    byteCount = NW_String_readChar(&(pL->pBuf[i]),
                                   &c_ucs2, pL->encoding);
    /* This catches NW_String_readChar() reading past end of buffer
    and can be removed when the readChar function does proper
    error checking (requires passing in buf length). */
    if ((i + byteCount) > pL->byteCount) {
      return KBrsrFailure;
    }
    *pC = c_ucs2;
    if (byteCount == -1) {
      return KBrsrFailure;
    }
    if (charCount == offsetCharCount) {
      break;
    }
    charCount++;
    charsPastCR++;
    charsPastLF++;
    if (c_ucs2 == 0xd /* CR */) {
      crCount++;
      resetPastCR = 1;
      charsPastCR = 0;
    } else if (c_ucs2 == 0xa /* LF */) {
      lfCount++;
      resetPastLF = 1;
      charsPastLF = 0;
    }
    i += byteCount;
    if (pL->encoding == HTTP_iso_10646_ucs_2 &&
      i == (pL->byteCount - 1))
    {
      *pEOF = NW_TRUE;
      break;
    }
    if (pL->encoding == HTTP_utf_8 &&
        (i + 3) > pL->byteCount)
      {
        *pEOF = NW_TRUE;
        break;
      }
  } while (i < pL->byteCount);
  if (i >= pL->byteCount) {
    *pEOF = NW_TRUE;
  }
  if (advance == NW_TRUE) {
    if (*pEOF == NW_TRUE) {
      pL->readPosition = pL->byteCount;
      pL->end = NW_TRUE;
    } else {
      pL->readPosition = i;
    }
    pL->charPosition += charCount;
    pL->lineColumn.crCount += crCount;
    pL->lineColumn.lfCount += lfCount;
    if (resetPastCR) {
      pL->lineColumn.charsSinceLastCR = charsPastCR;
    } else {
      pL->lineColumn.charsSinceLastCR += charsPastCR;
    }
    if (resetPastLF) {
      pL->lineColumn.charsSinceLastLF = charsPastLF;
    } else {
      pL->lineColumn.charsSinceLastLF += charsPastLF;
    }
  }
  return KBrsrSuccess;
}

TBrowserStatusCode NW_HTMLP_Lexer_PeekOffset(NW_HTMLP_Lexer_t* pL,
                                      NW_Uint32 offsetCharCount,
                                      NW_Uint32* pC,
                                      NW_Bool* pEOF)
{
  return NW_HTMLP_Lexer_PeekOrAdvanceOffset(NW_FALSE, pL,
                                            offsetCharCount, pC, pEOF);
}

TBrowserStatusCode NW_HTMLP_Lexer_AdvanceOffset(NW_HTMLP_Lexer_t* pL,
                                         NW_Uint32 offsetCharCount)
{
  NW_Uint32 c;
  NW_Bool eof;
  return NW_HTMLP_Lexer_PeekOrAdvanceOffset(NW_TRUE, pL,
                                            offsetCharCount, &c, &eof);
}

void NW_HTMLP_Lexer_GetPosition(NW_HTMLP_Lexer_t* pL, NW_HTMLP_Lexer_Position_t* pPosition)
{
  pPosition->readPosition = pL->readPosition;
  pPosition->charPosition = pL->charPosition;
  pPosition->crCount = pL->lineColumn.crCount;
  pPosition->lfCount = pL->lineColumn.lfCount;
  pPosition->charsSinceLastCR = pL->lineColumn.charsSinceLastCR;
  pPosition->charsSinceLastLF = pL->lineColumn.charsSinceLastLF;
  pPosition->end = pL->end;
}

/* Note: Setting the position (similar to seeking in a file) is in general
not possible without reading the characters (usually reading forward) because
character encoding may use a variable numbers of bytes per character. This is
here so that if you have defined a valid interval, then you can reposition to
the beginning of the interval. Setting to the position to a bad value will
not always be caught immediately. Don't forget to also save and set line
and column with position. */
TBrowserStatusCode NW_HTMLP_Lexer_SetPosition(NW_HTMLP_Lexer_t* pL, NW_HTMLP_Lexer_Position_t* pPosition)
{
  if ((pPosition->readPosition > pL->byteCount)
      || (pPosition->end && (pPosition->readPosition != pL->byteCount))) {
    return KBrsrFailure;
  }
  pL->readPosition = pPosition->readPosition;
  pL->charPosition = pPosition->charPosition;
  pL->lineColumn.crCount = pPosition->crCount;
  pL->lineColumn.lfCount = pPosition->lfCount;
  pL->lineColumn.charsSinceLastCR = pPosition->charsSinceLastCR;
  pL->lineColumn.charsSinceLastLF = pPosition->charsSinceLastLF;
  pL->end = pPosition->end;

  return KBrsrSuccess;
}

TBrowserStatusCode NW_HTMLP_Lexer_IsSpace(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch)
{
  NW_Uint32 c;
  TBrowserStatusCode e;
  NW_Bool eof;

  *pMatch = NW_FALSE;
  e = NW_HTMLP_Lexer_Peek(pL, &c, &eof);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
  }
  if (eof == NW_TRUE) {
    return KBrsrSuccess;
  }
  /* "space" is defined in HTML to be the following codepoints:
  0x20 (space), 0x9 (tab), 0xc (form feed), 0x200b (zero-width space),
  0xd (cr), 0xa (lf) */
  if ((c == 0x20U) || (c == 0x9U) || (c == 0xcU)
      || (c== 0x200bU) || (c == 0xdU) || (c == 0xaU)) {
    *pMatch = NW_TRUE;
  }
  return KBrsrSuccess;
}

TBrowserStatusCode NW_HTMLP_Lexer_IsCRLF(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch)
{
  NW_Uint32 c;
  TBrowserStatusCode e;
  NW_Bool eof;

  *pMatch = NW_FALSE;
  e = NW_HTMLP_Lexer_Peek(pL, &c, &eof);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
  }
  if (eof == NW_TRUE) {
    return KBrsrSuccess;
  }
  /* 0xd (CR), 0xa (LF) */
  if ((c == 0xdU) || (c == 0xaU)) {
    *pMatch = NW_TRUE;
  }
  return KBrsrSuccess;
}

/* on return: *pMatch == NW_TRUE if character is in [a-zA-Z] */
TBrowserStatusCode NW_HTMLP_Lexer_IsAsciiLetter(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch)
{
  NW_Uint32 c;
  TBrowserStatusCode e;
  NW_Bool eof;

  *pMatch = NW_FALSE;
  e = NW_HTMLP_Lexer_Peek(pL, &c, &eof);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
  }
  if (eof == NW_TRUE) {
    return KBrsrSuccess;
  }
  if ( ( (c >= (NW_Uint32)'a') && (c <= (NW_Uint32)'z') )
      || ( (c >= (NW_Uint32)'A') && (c <= (NW_Uint32)'Z') ) ) {
    *pMatch = NW_TRUE;
  }
  return KBrsrSuccess;
}

/* on return: *pMatch == NW_TRUE if character is in [0-9] */
TBrowserStatusCode NW_HTMLP_Lexer_IsAsciiDigit(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch)
{
  NW_Uint32 c;
  TBrowserStatusCode e;
  NW_Bool eof;

  *pMatch = NW_FALSE;
  e = NW_HTMLP_Lexer_Peek(pL, &c, &eof);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
  }
  if (eof == NW_TRUE) {
    return KBrsrSuccess;
  }
  if ((c >= (NW_Uint32)'0') && (c <= (NW_Uint32)'9')) {
    *pMatch = NW_TRUE;
  }
  return KBrsrSuccess;
}

/* *pMatch is NW_TRUE iff ASCII string matches the text in its encoding */
TBrowserStatusCode NW_HTMLP_Lexer_AsciiCharCompare(NW_HTMLP_Lexer_t* pL,
                                            NW_Uint8 asciiChar,
                                            NW_Bool* pMatch)
{
  NW_Uint32 c_text;
  TBrowserStatusCode e;
  NW_Bool eof;

  *pMatch = NW_FALSE;
  e = NW_HTMLP_Lexer_Peek(pL, &c_text, &eof);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
  }
  if (eof == NW_TRUE) {
    return KBrsrSuccess;
  }
  if (c_text == asciiChar) {
    *pMatch = NW_TRUE;
  }
  return KBrsrSuccess;
}

/*
on entry: no assumptions
on return: If matched string, then *pMatch == NW_TRUE.
.........: If did not match string, then *pMatch == NW_FALSE.
.........: In either case, lexer read position is unchanged
eof handling: if encounters EOF while attempting operation then returns
............: *pMatch == NW_FALSE and KBrsrSuccess, and
............: lexer read position is unchanged
on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
...............: and lexer read position is unchanged
*/
TBrowserStatusCode NW_HTMLP_Lexer_AsciiStringCompare(NW_HTMLP_Lexer_t* pL,
                                              NW_Uint32 asciiCharCount,
                                              const NW_Uint8* pString,
                                              NW_Bool* pMatch)
{
  return NW_HTMLP_Lexer_AsciiStringCompareCase(pL,
                                              asciiCharCount,
                                              pString,
                                              NW_TRUE,
                                              pMatch);
}

/* perform the same functionality as NW_HTMLP_Lexer_AsciiStringCompareCase
   except for the ability to perform both case insensitive check and 
   case sensitive check
*/
TBrowserStatusCode NW_HTMLP_Lexer_AsciiStringCompareCase(NW_HTMLP_Lexer_t* pL,
                                              NW_Uint32 asciiCharCount,
                                              const NW_Uint8* pString,
                                              NW_Bool CaseSensitive,
                                              NW_Bool* pMatch)
{
  NW_Uint32 c_text;
  NW_Uint32 i;
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Bool eof;

  *pMatch = NW_FALSE;
  NW_ASSERT(asciiCharCount);
  for (i = 0; i < asciiCharCount; i++) {

    e = NW_HTMLP_Lexer_PeekOffset(pL, i, &c_text, &eof);
    if (BRSR_STAT_IS_FAILURE(e)) {
      break;
    }
    if (eof == NW_TRUE) {
      break;
    }
    if (c_text != pString[i]) {
      if (!CaseSensitive)
      {
        if (c_text + 'A' - 'a' == pString[i]) //Small case to Upper
          continue;
		if(c_text + 'a' - 'A' == pString[i]) //Upper case to small 
          continue;
      }
      break;
    }
  }
  if (i == asciiCharCount) {
    *pMatch = NW_TRUE;
  }
  return e;
}

/* Sets start, stop, charStart, charStop to current read position. */
void NW_HTMLP_Interval_Start(NW_HTMLP_Interval_t* pI, NW_HTMLP_Lexer_t* pL)
{
  /* set both start and stop for safety in later use */
  pI->start = pI->stop = pL->readPosition;
  pI->charStart = pI->charStop = pL->charPosition;
}

/* Sets stop to current reader position. */
void NW_HTMLP_Interval_Stop(NW_HTMLP_Interval_t* pI, NW_HTMLP_Lexer_t* pL)
{
  pI->stop = pL->readPosition;
  pI->charStop = pL->charPosition;
}

/* Returns an estimate of the current line and column position in the text.
It is an estimate because it has to guess at what the intended line ending
sequence is using a count of CR and LF characters.  Line and Column indices
are 1-based not 0-based. */
void
NW_HTMLP_Lexer_GetLineColumn(NW_HTMLP_Lexer_t* pT, NW_Uint32* pLine,
                             NW_Uint32* pColumn)
{
    NW_Uint32 crCount, lfCount, charsSinceCR, charsSinceLF;
    crCount = pT->lineColumn.crCount;
    lfCount = pT->lineColumn.lfCount;
    charsSinceCR = pT->lineColumn.charsSinceLastCR;
    charsSinceLF = pT->lineColumn.charsSinceLastLF;
    if (crCount == lfCount) {
        /* assume CR, LF, DOS style */
        /* use a bias in favor of CR followed by LF
        which will give the correct column for DOS */
        *pLine = lfCount + 1;
        *pColumn = charsSinceLF + 1;
    } else if (lfCount == 0) {
        /* assume CR only, Unix style */
        *pLine = crCount + 1;
        *pColumn = charsSinceCR + 1;
    } else if (crCount == 0) {
        /* assume LF only, Mac style */
        *pLine = lfCount + 1;
        *pColumn = charsSinceLF + 1;
    } else {
        /* an unclear situation so use
        thresholds on the ratio to guess */
        NW_Uint32 ratio;
        ratio = ((crCount * 100) / lfCount);
        if (ratio > 300) {/* more than 3 to 1 crCount to lfCount */
            /* assume CR only, Unix style */
            *pLine = crCount + 1;
            *pColumn = charsSinceCR + 1;
        } else if (ratio < 33) {/* less than 1 to 3 crCount to lfCount */
            /* assume LF only, Mac style */
            *pLine = lfCount + 1;
            *pColumn = charsSinceLF + 1;
        } else {
            /* assume CR, LF, DOS style */
            /* use a bias in favor of CR, LF sequence (DOS style)
            which will give the correct column */
            *pLine = lfCount + 1;
            *pColumn = charsSinceLF + 1;
        }
    }
}
#else

void FeaRmeNoHTMLParser_htmlp_lexer(){
   int i = 0;
   i+=1;
}
#endif /* FEA_RME_NOHTMLPARSER */
