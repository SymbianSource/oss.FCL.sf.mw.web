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
* Description:  Lexer for CSS
*
*/

// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "nwx_defs.h"

#include "CSSReader.h"
#include "nw_text_ucs2.h"
#include "nwx_assert.h"
#include "nwx_http_defs.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

#define ASCII_NULL                0x00
#define ASCII_EXCLAMATION_MARK    '!'
#define ASCII_QUOTATION_MARK      '"'
#define ASCII_HASH                '#'
/*#define ASCII_PERCENT             '%'*/
#define ASCII_APOSTROPHE          '\''
#define ASCII_LEFT_PARENTHESIS    '('
#define ASCII_RIGHT_PARENTHESIS   ')'
#define ASCII_ASTERISK            '*'
#define ASCII_EQUALS              '='
#define ASCII_COMMA               ','
#define ASCII_HYPHEN              '-'
#define ASCII_UNDERSCORE          '_'
#define ASCII_DOT                 '.'
#define ASCII_SLASH               '/'
#define ASCII_BACKSLASH           '\\'
#define ASCII_COLON               ':'
#define ASCII_SEMI_COLON          ';'
#define ASCII_LESS_THAN           '<'
/*#define ASCII_GREATER_THAN        '>'*/
#define ASCII_AT                  '@'
#define ASCII_LEFT_BRACE          '{'
#define ASCII_VERTICAL_LINE       '|'
#define ASCII_RIGHT_BRACE         '}'
#define ASCII_LEFT_BRACKET        '['
#define ASCII_RIGHT_BRACKET       ']'

#define ASCII_LOWER_R             'r'
#define ASCII_LOWER_U             'u'

static const TText16 importRule[] = {'i','m','p','o','r','t','\0'};
static const TText16 mediaRule[] = {'m','e','d','i','a','\0'};
static const TText16 charsetRule[] = {'@','c','h','a','r','s','e','t','\0'};
static const TText16 activeClass[] = {'a','c','t','i','v','e','\0'};
static const TText16 focusClass[] = {'f','o','c','u','s','\0'};
static const TText16 linkClass[] = {'l','i','n','k','\0'};
static const TText16 visitedClass[] = {'v','i','s','i','t','e','d','\0'};
static const TText16 cachedClass[] = {'c','a','c','h','e','d','\0'};
static const TText16 important[] = {'i','m','p','o','r','t','a','n','t','\0'};

static const TText16 all[] = {'a','l','l','\0'};
/*static const TText16 screen[] = {'s','c','r','e','e','n','\0'};*/
static const TText16 handheld[] = {'h','a','n','d','h','e','l','d','\0'};

static const TText16 cdo[] = { '<', '!', '-', '-','\0' };
static const TText16 cdc[] = { '-', '-','>','\0' };
static const TText16 em[] = { 'e','m','\0' };
static const TText16 ex[] = { 'e','x','\0' };
static const TText16 px[] = { 'p','x','\0' };
static const TText16 in[] = { 'i','n','\0' };
static const TText16 cm[] = { 'c','m','\0' };
static const TText16 mm[] = { 'm','m','\0' };
static const TText16 pt[] = { 'p','t','\0' };
static const TText16 pc[] = { 'p','c','\0' };
static const TText16 percent[] = {'%','\0'};
static const TText16 rgb[] = {'r','g','b','(','\0'};

/* following strings need to be changed */
static const TText16 latin1[] = {'i','s','o','-','8','8','5','9','-','1','\0'};
static const TText16 ucs2[]= {'i','s','o','-','1','0','6','4','6','-','u','c','s','-','2','\0'};
static const TText16 utf8[]= {'u','t','f','-','8','\0'};
static const TText16 usAscii[]= {'u','s','-','a','s','c','i','i','\0'};


// MODULE DATA STRUCTURES

struct TCSSKeyword
{
  const TText16* name;
  TUint16 tokenType;
};

// Array of charsets
static const TCSSKeyword TCSSReaderCharsets[] =
{
  {latin1, HTTP_iso_8859_1},
  {ucs2, HTTP_iso_10646_ucs_2},
  {utf8, HTTP_utf_8},
  {usAscii, HTTP_us_ascii}
};

// Array of at-rules
static const TCSSKeyword TCSSReaderAtRuleKeywords[] =
{
  {importRule, IMPORT_RULE},
  {mediaRule, MEDIA_RULE},
  {charsetRule, CHARSET_RULE},
};

// Array of pseudo-class keywords
static const TCSSKeyword TCSSPseudoClassKeywords[] =
{
  {activeClass, ACTIVE_PSEUDO_CLASS},
  {linkClass, LINK_PSEUDO_CLASS},
  {visitedClass, VISITED_PSEUDO_CLASS},
  {focusClass, FOCUS_PSEUDO_CLASS},
  {cachedClass, CACHED_PSEUDO_CLASS}
};

// Array of units
static const TCSSKeyword TCSSUnitKeywords[] =
{
  {em, EMS},
  {ex, EXS},
  {px, PXS},
  {in, INS},
  {cm, CMS},
  {mm, MMS},
  {pt, PTS},
  {pc, PCS},
  {percent, PERCENTAGE}
};

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES

/* -------------------------------------------------------------------------*/
static void NW_CSS_Reader_Ucs2_ntoh(NW_Byte* pBuf, TUint32 byteCount)
{
  TUint32 i;
  NW_Uint16 c_ucs2 = 1;

  if (((NW_Uint8*)&c_ucs2)[0] == 1) { /* test for little endian host */
    for (i = 0; i < byteCount; i += 2)
    {
      (void)NW_Mem_memcpy(&c_ucs2, pBuf + i, sizeof(TUint16));
      pBuf[i] = (TUint8)((c_ucs2 >> 8) & 0xff);
      pBuf[i+1] = (TUint8)(c_ucs2 & 0xff);
    }
  }

}

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TCSSReader::ReadCharL
// Reads a character as a unicode character
// -----------------------------------------------------------------------------
//
TInt32 TCSSReader::ReadChar(TText16* c)
{
  TInt32 byteCnt;

  *c = 0;
  if (iPosition == iLength)
  {
    return -1;
  }
  if ((byteCnt = NW_String_readChar(iBuffer+iPosition, c, iEncoding)) == -1)
  {
    *c = 0;
    return -1;
  }
  if (!Advance(byteCnt))
  {
    *c = 0;
    return -1;
  }
  return byteCnt;
}


/* ------------------------------------------------------------------------- */
TBool TCSSReader::Equals(const TText16* aStr, TBool aCaseInsensitive)
{
  TUint32 position;
  TUint32 len;

  position = iPosition;
  len = NW_Str_Strlen(aStr);
  for (TUint32 index=0; index < len; index++)
  {
    TText16 strCh;
    TText16 c;

    if (ReadChar(&c) == -1)
    {
      SetPosition(position);
      return EFalse;
    }
    if (aCaseInsensitive)
    {
      strCh = NW_Str_ToLower(*aStr);
	    c = NW_Str_ToLower(c);
    }
    else
    {
      strCh = *aStr;
    }
    if (c != strCh)
    {
      SetPosition(position);
      return EFalse;
    }
    aStr++;
  }
  return ETrue;
}

/* ------------------------------------------------------------------------- */
TBool TCSSReader::ReadString(TCSSReaderUnit *aStr)
{
  TText16 c;
  TText16 strStartChar;
  TText16 previousChar;
  TInt32 byteCnt;
  TInt32 escLen;

  byteCnt = ReadChar(&c);
  if ((c != ASCII_QUOTATION_MARK) && (c != ASCII_APOSTROPHE))
  {
    return EFalse;
  }
  strStartChar = previousChar = c;
  aStr->Init(iBuffer + iPosition, 0, 0);

  while ((byteCnt = ReadChar(&c)) != -1)
  {
    /* check for escape character */
    if (c == strStartChar)
    {
      if (previousChar != ASCII_BACKSLASH)
      {
        return ETrue;
      }
    }
    else if (c == ASCII_BACKSLASH)
    {
      escLen = ReadEscape(&c);
      if (escLen == -1)
      {
        return EFalse;
      }
      byteCnt += escLen;
      previousChar = ' ';
    }
    else
    {
      previousChar = c;
    }
    aStr->iLength += byteCnt;
    aStr->iNumChars ++;
  }
  return EFalse;
}

/* ------------------------------------------------------------------------- */
TBool TCSSReader::ReadURI(TCSSReaderUnit *aStr)
{
  static const TText16 url[] = { 'u','r','l','(','\0' };
  TInt32 byteCnt;
  TText16 c;
  TUint32 position;

  NW_ASSERT(aStr);

  if (!Equals((TText16*)url, EFalse))
  {
    return EFalse;
  }
  SkipWhiteSpaces();

  position = iPosition;
  if (ReadString(aStr))
  {
    SkipWhiteSpaces();
    ReadChar(&c);
    if (c == ASCII_RIGHT_PARENTHESIS)
    {
      return ETrue;
    }
   return EFalse;
  }
  SetPosition(position);
  aStr->Init(iBuffer+iPosition, 0,0);
  /* read url */
  while ((byteCnt = ReadChar(&c)) != -1)
  {
    if (c == ASCII_RIGHT_PARENTHESIS)
    {
      return ETrue;
    }
    aStr->iLength += byteCnt;
    aStr->iNumChars++;
  }
  return EFalse;
}

/* -------------------------------------------------------------------------*/
TBool TCSSReader::ReadBlock(TText16 aClosingChar)
{
  TInt32 byteCnt;
  TText16 c;
  TCSSReaderUnit unit;
  TBool status = ETrue;

  status = ETrue;
  while ((byteCnt = ReadChar(&c)) != -1)
  {
    if (c == aClosingChar)
    {
      return ETrue;
    }
    switch(c)
    {
      case ASCII_LEFT_BRACE:
        status = ReadBlock(ASCII_RIGHT_BRACE);
        break;
      case ASCII_LEFT_PARENTHESIS:
        status = ReadBlock(ASCII_RIGHT_PARENTHESIS);
        break;
      case ASCII_LEFT_BRACKET:
        status = ReadBlock(ASCII_RIGHT_BRACKET);
        break;
      case ASCII_APOSTROPHE:
      case ASCII_QUOTATION_MARK:
        SetPosition(iPosition - byteCnt);
        status = ReadString(&unit);
        break;
      default:
        break;
    }
    if (!status)
    {
      return EFalse;
    }
  }
  return EFalse;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBool TCSSReader::IsValidMedia(TCSSReaderUnit *aStr)
{
  TCSSReader r(aStr->iStorage, aStr->iLength, iEncoding);

  return (r.Equals((TText16 *)all, EFalse) || r.Equals((TText16 *)handheld, EFalse));
}

/* ------------------------------------------------------------------------- */
TInt8 TCSSReader::ReadNextToken(TCSSReaderUnit* aToken)
{
  TText16 c;
  TInt32 byteCnt;
  TCSSReaderUnit str;
  TUint32 thisPosition;

  NW_ASSERT(aToken);

  if (iPosition == iLength)
  {
    return -1;
  }
  thisPosition = iPosition;

  aToken->Init(iBuffer+iPosition, 0,0);

  if ((byteCnt = ReadChar(&c)) == -1)
  {
    return -1;
  }
  if (c == ASCII_BACKSLASH)
  {
    byteCnt = ReadEscape(&c);
  }

  aToken->iLength = byteCnt;
  aToken->iNumChars++;

  if (NW_Str_Isspace(c))
  {
    return SPACE;
  }

  if (NW_Str_Isdigit(c))
  {
    SetPosition(iPosition - byteCnt);
    return DIGIT;
  }

  switch(c)
  {
  case ASCII_NULL:
    return BUFFER_END;

  case ASCII_LEFT_BRACE:
    return LEFT_BRACE;

  case ASCII_RIGHT_BRACE:
    return RIGHT_BRACE;

  case ASCII_LEFT_BRACKET:
    return LEFT_BRACKET;

  case ASCII_RIGHT_BRACKET:
    return RIGHT_BRACKET;

  case ASCII_DOT:
    return DOT;

  case ASCII_COMMA:
    return COMMA;

  case ASCII_SEMI_COLON:
    return SEMI_COLON;

  case ASCII_ASTERISK:
    return ASTERISK;

  case ASCII_EQUALS:
    return EQUALS;

  case ASCII_VERTICAL_LINE:
    return VERTICAL_LINE;

  case ASCII_RIGHT_PARENTHESIS:
    return RIGHT_PARENTHESIS;

  case ASCII_HASH:
    return HASH;

  case ASCII_SLASH:
      return 0;

  case ASCII_LESS_THAN:
    SetPosition(thisPosition);
    if (Equals((TText16 *)cdo, EFalse))
    {
      aToken->iLength += NW_Str_Strlen((TText16 *)cdo) -1 ;
      return CDO;
    }
    return -1;

  case ASCII_HYPHEN:
    SetPosition(thisPosition);
    if (Equals((TText16 *)cdc, EFalse))
    {
      return CDC;
    }
    SetPosition(thisPosition);
    return HYPHEN;

  case ASCII_APOSTROPHE:
  case ASCII_QUOTATION_MARK:
    SetPosition(thisPosition);
    if (ReadString(aToken))
    {
      return STRING;
    }
    return -1;

  case ASCII_AT:
    {
      TUint32 index = 0;
      while (index < sizeof(TCSSReaderAtRuleKeywords)/sizeof(TCSSKeyword))
      {
        if (Equals(TCSSReaderAtRuleKeywords[index].name, EFalse))
        {
          TUint32 length = NW_Str_Strlen(TCSSReaderAtRuleKeywords[index].name);
          if (iEncoding == HTTP_iso_10646_ucs_2)
          {
             // Ucs2 chars are 2 bytes.
             aToken->iLength += (2 * length);
          }
          else
          {
             aToken->iLength += length;
          }
          return (TInt8)TCSSReaderAtRuleKeywords[index].tokenType;
        }
        index++;
      }
      if (ReadIdentifier(&str))
      {
        aToken->iLength += str.iNumChars;
        return ATKEYWORD;
      }
      return -1;
    }

  case ASCII_COLON:
    return COLON;

  case ASCII_EXCLAMATION_MARK:
    SkipWhiteSpaces();
    if (Equals((TText16 *)important, EFalse))
    {
      aToken->iLength += iPosition - thisPosition;
      return IMPORTANT;
    }
    return -1;

  case ASCII_LOWER_R:
    SetPosition(thisPosition);
    if (Equals((TText16 *)rgb, EFalse))
    {
      aToken->iLength += iPosition - thisPosition;
      return RGB;
    }
    break;

  case ASCII_LOWER_U:
    if (iEncoding == HTTP_iso_10646_ucs_2)
    {
      static const NW_Ucs2 url[] = { 'u','r','l','(','\0' };
      if (NW_Byte_Strnicmp((const NW_Byte*)(iBuffer+thisPosition), (const NW_Byte*)url, NW_Str_Strlen(url)*sizeof(NW_Ucs2)) != 0)
      {
        aToken->iLength = 8;
        aToken->iNumChars = 4;
        break;
      }
    }
    else
    {
      if (NW_Asc_strnicmp((char *)(iBuffer+thisPosition), "url(",4) !=0)
      {
          aToken->iLength = 4;
          aToken->iNumChars = 4;
        break;
      }
    }
    SetPosition(thisPosition);
    return URI;
  default:
    break;
  }

  if (NW_Str_Isalpha(c))
  {
    SetPosition(thisPosition);
    aToken->iLength = 0;
    return ALPHA;
  }
  SetPosition(thisPosition);
  aToken->iLength = 0;
  return 0;
}

/* ------------------------------------------------------------------------- */
TInt8 TCSSReader::ReadNumberToken(TCSSReaderUnit *aStr)
{
  TText16 c;
  TInt32 byteCnt;
  TUint32 index;
  TUint32 startPosition;

  startPosition = iPosition;
  aStr->Init(GetBufferPointer(), 0,0);

  byteCnt = ReadChar(&c);
  if ((c == '-') || (c == '+'))
  {
    aStr->iNumChars++;
    byteCnt = ReadChar(&c);
  }
  if (!NW_Str_Isdigit(c) && (c!= ASCII_DOT))
  {
    return -1;
  }
  while ((NW_Str_Isdigit(c)) && (byteCnt != -1))
  {
    aStr->iNumChars++;
    byteCnt = ReadChar(&c);
  }
  if (c == ASCII_DOT)
  {
    aStr->iNumChars++;
    byteCnt = ReadChar(&c);
    if (!NW_Str_Isdigit(c))
    {
      return -1;
    }
    while ((NW_Str_Isdigit(c)) && (byteCnt != -1))
    {
      aStr->iNumChars++;
      byteCnt = ReadChar(&c);
    }
  }
  if (byteCnt == -1)
  {
    return -1;
  }
  SetPosition(iPosition-byteCnt);
  aStr->iLength = iPosition - startPosition;

  SkipWhiteSpaces();
  for (index = 0;index < sizeof(TCSSUnitKeywords)/sizeof(TCSSKeyword); index++)
  {
    if (Equals(TCSSUnitKeywords[index].name, EFalse))
    {
      return (NW_Int8)TCSSUnitKeywords[index].tokenType;
    }
  }
  return NUMBER;
}

/* ------------------------------------------------------------------------- */
TInt8 TCSSReader::GetPseudoClass()
{
  TUint32 index = 0;

  while (index < sizeof(TCSSPseudoClassKeywords)/sizeof(TCSSKeyword))
  {
    if (Equals(TCSSPseudoClassKeywords[index].name, EFalse))
    {
      return (NW_Int8)TCSSPseudoClassKeywords[index].tokenType;
    }
    index++;
  }
  return -1;
}

/* ------------------------------------------------------------------------- */
TBool TCSSReader::ReadIdentifier(TCSSReaderUnit *aStr)
{
  TText16 c;
  TInt32 byteCnt;
  TUint32 thisPosition;
  TCSSReaderUnit name;

  NW_ASSERT(aStr);
  thisPosition = iPosition;

  aStr->Init(GetBufferPointer(), 0,0);

  byteCnt = ReadChar(&c);

  if (c == ASCII_BACKSLASH)
  {
    byteCnt = ReadEscape(&c);
  }

  if ((!NW_Str_Isalpha(c)) && (c != ASCII_HYPHEN))
  {
    SetPosition(thisPosition);
    return EFalse;
  }

  aStr->iLength = byteCnt;
  aStr->iNumChars =1;
  thisPosition = iPosition;

  if (ReadName(&name))
  {
    aStr->iLength += name.iLength;
    aStr->iNumChars += name.iNumChars;
  }
  else
  {
    SetPosition(thisPosition);
  }
  return ETrue;
}

TUint16 TCSSReader::ReadEscape(TText16* outChar)
{
  TInt32  tempLen = 0;
  TInt32  size = 0;
  TText16 hex;
  TText16 c;
  TUint16 hexIndex = 0;
  TUint16 escLen = 0;


  TUint32 prevPosition = iPosition;
  size = ReadChar(&c);    // ReadChar returns TInt32-sized byteLen read
  if (size < 0)
  {
    return 0;
  }
  tempLen = size;

  switch (c)
  {
  case '\n':
    *outChar = 0;
    break;
  default:
    if ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F'))
    {
      hex = 0;
      hexIndex = 0;
      do {
        if (c >= '0' && c <= '9') {
          hex = (TText16)((hex << 4) + (c - '0'));
        }
        else if (c >= 'a' && c <= 'f') {
          hex = (TText16)((hex << 4) + (c - 'a' + 10));
        }
        else if (c >= 'A' && c <= 'F') {
          hex = (TText16)((hex << 4) + (c - 'A' + 10));
        }
        else {
          break;
        }
        prevPosition = iPosition;
        if ((size = ReadChar(&c)) < 0)
        {
          return 0;
        }
        hexIndex ++;
        tempLen = tempLen + size;
      } while (hexIndex < 6);
      if (!NW_Str_Isspace(c))
      {
        SetPosition(prevPosition);
        tempLen -= size;
      }
      *outChar = (TText16)hex;
    }
    else {
      *outChar = c;
    }
    break;
  }

  escLen = (TUint16) tempLen;
  return escLen;
}
/* ------------------------------------------------------------------------- */
TBool TCSSReader::ReadName(TCSSReaderUnit *aStr)
{
  TText16 c;
  TInt32 byteCnt;
  TUint32 startPosition;

  aStr->Init(GetBufferPointer(), 0, 0);
  startPosition = iPosition;

  byteCnt = ReadChar(&c);

  while (((NW_Str_Isalpha(c) || NW_Str_Isdigit(c)  ||
            (c >= 0x80 && c <= 0xff) ||
            (c == ASCII_HYPHEN) ||
            (c == ASCII_BACKSLASH) ||
            (c == ASCII_UNDERSCORE))) &&
          (byteCnt != -1))
  {
    if (c == ASCII_BACKSLASH)
    {
      ReadEscape(&c);
    }
    aStr->iNumChars++;
    byteCnt = ReadChar(&c);
  }
  if (byteCnt == -1)
  {
    return EFalse;
  }

  aStr->iLength = iPosition - startPosition;

  SetPosition(iPosition - byteCnt);
  return ETrue;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode TCSSReader::GoToToken(NW_Uint8 aTokenType, TText8** aPosition)
{
  NW_Int8 tok;
  TCSSReaderUnit token;

  *aPosition = NULL;
  while ((tok = ReadNextToken(&token)) != aTokenType)
  {
    TText16 c;

    if (tok == -1)
    {
      return KBrsrBufferEnd;
    }
    if (tok == URI)
    {
        Advance(token.iLength);
    }
    else if ((tok == 0) || (tok == ALPHA) || (tok == DIGIT) || (tok == HYPHEN))
    {
      if (ReadChar(&c) == -1)
      {
        return KBrsrFailure;
      }
    }
  }
  *aPosition = GetBufferPointer();
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void TCSSReader::SkipWhiteSpaces()
{
  TInt32 numBytes;
  TText16 c;

  do
  {
    numBytes = ReadChar (&c);
    while (c == ASCII_SLASH)
    {
      /* process the trailing '*' */
      numBytes = ReadChar (&c);
      if (numBytes == -1 || c != ASCII_ASTERISK)
      {
        SetPosition (iPosition - numBytes);
        c = ASCII_SLASH;
        break;
      }

      /* process all characters up to the next '*' */
      while ((numBytes = ReadChar (&c)) != -1)
      {
        if (c == ASCII_ASTERISK)
        { /* look for comment end */
          if (ReadChar (&c) != -1 && c == ASCII_SLASH)
          {
            numBytes = ReadChar (&c);
            break;
          }
        }
      }

      if (numBytes == -1) {
        return;
      }
    }
  }while (NW_Str_Isspace(c));
  SetPosition (iPosition - numBytes);
}

/* -------------------------------------------------------------------------*/
TBool TCSSReader::IgnoreAtRule(TUint32 aPosition)
{
  TText16 c;

  SetPosition(aPosition);
  while (ReadChar(&c) != -1)
  {
    if (c == ASCII_SEMI_COLON)
    {
      return ETrue;
    }
    if (c == ASCII_LEFT_BRACE)
      break;
  }
  return ReadBlock(ASCII_RIGHT_BRACE);
}

/* -------------------------------------------------------------------------*/
TBool TCSSReader::ReadCharset()
{
  TText16 c;
  TCSSReaderUnit unit;
  NW_Byte* data = iBuffer;

  if (iEncoding == HTTP_utf_8)
  {
    /* look for BOM and remove if found */
    if (iLength >= 3)
    {
      if ((data[0] == 0xef)
          && (data[1] == 0xbb)
          && (data[2] == 0xbf))
      {
        Advance(3);
      }
    }
  }
  if (iEncoding == HTTP_iso_10646_ucs_2)
  {
    /* WARNING: we are assuming network byte order (i.e., big) for
       the input document */

    /* verify the doc has an even number of bytes, check LSB != 1 */
    if ((iLength & 1) == 1)
    {
      return EFalse;
    }

    /* WARNING skip next call in debug if the server doesn't understand
       serving UCS2 and doesn't change byte order to big endian */
    /* make a pass over the doc, forcing byte order to native byte order */
    NW_CSS_Reader_Ucs2_ntoh(iBuffer, iLength);

    /* now look for BOM and remove if found */
    if (iLength >= 2)
    {
      if (((data[0] == 0xfe) && (data[1] == 0xff))
          || ((data[0] == 0xff) && (data[1] == 0xfe)))
      {
        Advance(2);
      }
    }
  }

  if (Equals(charsetRule, EFalse))
  {
    (void) ReadChar(&c);
    if (!NW_Str_Isspace(c))
    {
      return EFalse;
    }
    SkipWhiteSpaces();
    if (!ReadString(&unit))
    {
      return EFalse;
    }
  }
  else
  {
    return EFalse;
  }
  TCSSReader reader(unit.iStorage, unit.iLength, iEncoding);
  iEncoding = reader.GetCharsetVal();
  SkipWhiteSpaces();

  if (ReadNextToken(&unit) != SEMI_COLON)
  {
    return EFalse;
  }
  return ETrue;
}




// -----------------------------------------------------------------------------
// TCSSReader::Advance:
// Sets encoding
// -----------------------------------------------------------------------------
//
TBool TCSSReader::Advance(TUint32 aNumBytes)
{
  if ((iPosition+aNumBytes) > iLength)
  {
    return EFalse;
  }
  SetPosition(iPosition+aNumBytes);
  return ETrue;
}

// -----------------------------------------------------------------------------
// TCSSReader::GetCharsetVal
// Gets Encoding of document
// -----------------------------------------------------------------------------
//
TUint32 TCSSReader::GetCharsetVal()
{
  TUint32 index = 0;

  while (index < sizeof(TCSSReaderCharsets)/sizeof(TCSSKeyword))
  {
    if (Equals(TCSSReaderCharsets[index].name, ETrue))
    {
      return TCSSReaderCharsets[index].tokenType;
    }
    index++;
  }
  return 0;

}

// -----------------------------------------------------------------------------
// TCSSReaderUnit::GetUnicode
// Converts to unicode
// -----------------------------------------------------------------------------
//
TText16*
TCSSReaderUnit::GetUnicodeL(TUint32 aEncoding)
{
  TText16* ucs2Str = NULL;
  TText16* outStr;
  TText16 c;
  TUint32 escLen;

  if ((iLength != 0) && (iStorage != NULL))
  {
      TCSSReader reader(iStorage, iLength, aEncoding);
      if (iNumChars == 0)
      {
        while (reader.ReadChar(&c) != -1)
        {
          iNumChars++;
        }
        reader.Init(iStorage, iLength, aEncoding);
      }
      ucs2Str = (TText16 *)NW_Mem_Malloc((iNumChars+1)*sizeof(TText16));
      if (ucs2Str == NULL)
      {
        User::Leave(KErrNoMemory);
      }
      outStr = ucs2Str;
      c = 0;
      for (TUint32 index = 0; index < iNumChars; index++)
      {
        if (reader.ReadChar(&c) < 0)
        {
          NW_Mem_Free(ucs2Str);
          return NULL;
        }
        if (c == '\\')
        {
          escLen = reader.ReadEscape(&c);
          if (escLen == 0)
          {
            NW_Mem_Free(ucs2Str);
            return NULL;
          }
          if (c != 0)
          {
            *outStr++ = c;
          }
        }
        else
        {
          *outStr++ = c;
        }
      }
      *outStr = 0;
  }
  return (TText16 *)ucs2Str;
}

  //This method is identical to the above one except it does not throw
  //an exception (e.g. Leave). This is faster and routines that call
  //this need to handle NULL anyway. As it turns out they DON'T. When
  //all that code is finally fixed the above method should be removed.

TText16*
TCSSReaderUnit::GetUnicode(TUint32 aEncoding)
{
  TText16* ucs2Str = NULL;
  TText16* outStr;
  TText16 c;
  TUint32 escLen;

  if ((iLength != 0) && (iStorage != NULL))
  {
      TCSSReader reader(iStorage, iLength, aEncoding);
      if (iNumChars == 0)
      {
        while (reader.ReadChar(&c) != -1)
        {
          iNumChars++;
        }
        reader.Init(iStorage, iLength, aEncoding);
      }
      ucs2Str = (TText16 *)NW_Mem_Malloc((iNumChars+1)*sizeof(TText16));
      if (ucs2Str == NULL)
      {
          return NULL;
      }
      outStr = ucs2Str;
      c = 0;
      for (TUint32 index = 0; index < iNumChars; index++)
      {
        if (reader.ReadChar(&c) < 0)
        {
          NW_Mem_Free(ucs2Str);
          return NULL;
        }
        if (c == '\\')
        {
          escLen = reader.ReadEscape(&c);
          if (escLen == 0)
          {
            NW_Mem_Free(ucs2Str);
            return NULL;
          }
          if (c != 0)
          {
            *outStr++ = c;
          }
        }
        else
        {
          *outStr++ = c;
        }
      }
      *outStr = 0;
  }
  return (TText16 *)ucs2Str;
}

