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
* Description:  CSS Lexer
*
*/


#ifndef TCSSReader_H
#define TCSSReader_H

//  INCLUDES
#include "nwx_defs.h"
#include "nw_string_string.h"
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

enum TCSSReaderTokenType{
  SPACE = 1,
  ATKEYWORD,
  HASH,
  ASTERISK,
  COMMA,
  DOT,
  COLON,
  HYPHEN,
  SEMI_COLON,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  EQUALS,
  CHARSET_RULE,
  IMPORT_RULE,
  MEDIA_RULE,
  LINK_PSEUDO_CLASS,
  VISITED_PSEUDO_CLASS,
  FOCUS_PSEUDO_CLASS,
  ACTIVE_PSEUDO_CLASS,
  CACHED_PSEUDO_CLASS,
  IMPORTANT,
  VERTICAL_LINE,
  CDO,
  CDC,
  BUFFER_END,
  IDENTIFIER,
  EMS,
  EXS,
  PXS,
  INS,
  CMS,
  MMS,
  PTS,
  PERCENTAGE,
  PCS,
  NUMBER,
  URI,
  RGB,
  STRING,
  RIGHT_PARENTHESIS,
  DIGIT,
  ALPHA,
  INVALID
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class is a unit of text
*  @lib css.lib
*  @since 2.1
*/
class TCSSReaderUnit
{
  public:
    // constructor
	  TCSSReaderUnit(TText8* aStorage, TUint32 aLength, TUint32 aNumChars)
      {
        iStorage = aStorage;
        iLength = aLength;
        iNumChars = aNumChars;
      }

    TCSSReaderUnit()
      {
        iStorage = NULL;
        iLength = 0;
        iNumChars = 0;
      }

	  void Init(TText8* aStorage, TUint32 aLength, TUint32 aNumChars)
      {
        iStorage = aStorage;
        iLength = aLength;
        iNumChars = aNumChars;
      }
    /**
    * Converts the text to unicode
    * @since 2.1
    * @param aEncoding: encoding of the document being parsed
    * return unicode String
    */
    TText16* GetUnicodeL(TUint32 aEncoding);
    TText16* GetUnicode(TUint32 aEncoding);

  public: //data
    // pointer to beginning of text
    TText8* iStorage;
    // length of storage
    TUint32 iLength;
    // number of character
    TUint32 iNumChars;
};
// CLASS DECLARATION

/**
*  This class is the lexer for CSS syntax
*  @lib css.lib
*  @since 2.1
*/
class TCSSReader
{
  public:
    // constructor
	  TCSSReader(TText8* aBuffer, TUint32 aLength, TUint32 aEncoding)
      {
        iBuffer = aBuffer;
        iLength = aLength;
        iEncoding = aEncoding;
        iPosition = 0;
      }

    TCSSReader()
      {
        iBuffer = NULL;
        iLength = 0;
        iEncoding = 0;
        iPosition = 0;
      }

    void Init(TText8* aBuffer, TUint32 aLength, TUint32 aEncoding)
      {
        iBuffer = aBuffer;
        iLength = aLength;
        iEncoding = aEncoding;
        iPosition = 0;
      }

    /**
    * Skips white space as defined by CSS2 Spec
    * @since 2.1
    * return void
    */
    void SkipWhiteSpaces();

    /**
    * Finds if the media type is valid (all or handheld)
    * @since 2.1
    * @param aMedia: media to be evaluated
    * return ETrue if media is valid else EFalse
    */
    TBool IsValidMedia(TCSSReaderUnit* aMedia);

    TInt8 ReadNextToken(TCSSReaderUnit* aStr);

    TInt8 GetPseudoClass();

    TInt8 ReadNumberToken(TCSSReaderUnit* aStr);

    TBool ReadIdentifier(TCSSReaderUnit* aStr);

    TBool ReadString(TCSSReaderUnit* aStr);

    TBool ReadName(TCSSReaderUnit* aStr);

    TBool ReadURI(TCSSReaderUnit* aStr);

    TBool ReadCharset();

    TBrowserStatusCode GoToToken(TUint8 aTokenType, TText8** aPosition);

    /**
    * Ignores the At-rule of CSS starting at given position
    * @since 2.1
    * @param aPosition - starting of At-Rule
    * return void
    */
    TBool IgnoreAtRule(TUint32 aPosition);

    /**
    * Sets position of the reader
    * @since 2.1
    * @param aPosition - position of reader
    * return void
    */
    inline void SetPosition(TUint32 aPosition){iPosition = aPosition;}

    /**
    * Returns pointer to the buffer
    * @since 2.1
    * return pointer to the buffer
    */
    inline TText8* GetBuffer(){return iBuffer;}

    /**
    * Returns length of buffer
    * @since 2.1
    * return length of buffer
    */
    inline TUint32 GetLength(){return iLength;}

    /**
    * Returns current position of reader
    * @since 2.1
    * return current position of reader
    */
    inline TUint32 GetPosition(){return iPosition;}

    /**
    * Returns encoding of buffer
    * @since 2.1
    * return encoding of buffer
    */
    inline TUint32 GetEncoding(){return iEncoding;}

    /**
    * Advances the buffer by certain number of bytes
    * @since 2.1
    * @param aNumBytes - number of bytes to advance
    * return void
    */
    TBool Advance(TUint32 aNumBytes);

    /**
    * Sets encoding of the document
    * @since 2.1
    * @param aEncoding - encoding of buffer
    * return void
    */
    inline void SetEncoding(TUint32 aEncoding){iEncoding = aEncoding;}

    /**
    * Returns pointer to the buffer storage where reader is positioned
    * @since 2.1
    * return pointer to the buffer storage where reader is positioned
    */
    inline TText8* GetBufferPointer(){return (iBuffer+iPosition);}

    /**
    * Gets the encoding associated with the text
    * @since 2.1
    * return encoding of the document being parsed
    */
    TUint32 GetCharsetVal();

    /**
    * Reads a character
    * @since 2.1
    * return encoding of the document being parsed
    */
    TInt32 ReadChar(TText16* aChar);

    TUint16 ReadEscape(TText16* aOutChar);

  private:    // Functions

    TBool Equals(const TText16* aStr, TBool aCaseInsensitive);

    TBool ReadBlock(TText16 aClosingChar);

  private:    // Data

    // pointer to the CSS buffer
    TText8* iBuffer;
    // length of buffer
    TUint32 iLength;
    // encoding of buffer
    TUint32 iEncoding;
    // position in the buffer
    TUint32 iPosition;
};

#endif /* TCSSReader_H */

