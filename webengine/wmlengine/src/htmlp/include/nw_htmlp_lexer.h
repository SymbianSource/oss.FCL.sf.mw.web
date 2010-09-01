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


/** ----------------------------------------------------------------------- **
    @package:     NW_HTMLP

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

/*
Lexer for HTML parser.
*/

#ifndef NW_HTMLP_LEXER_H
#define NW_HTMLP_LEXER_H


#include "nw_htmlp_dict.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @enum:        NW_HTMLP_Endianness

    @synopsis:    Endian indication.

    @scope:       public
    @names:
       [NW_NATIVE_ENDIAN]
                  Use native endianness.

       [NW_BIG_ENDIAN]
                  The leftmost bytes (those with a lower address) are 
                  most significant. 

       [NW_LITTLE_ENDIAN]
                  The rightmost bytes (those with a higher address) are 
                  most significant

    @description: Endian indication.
 ** ----------------------------------------------------------------------- **/
typedef enum NW_HTMLP_Endianness_e{
    NW_NATIVE_ENDIAN,
    NW_BIG_ENDIAN,
    NW_LITTLE_ENDIAN
} NW_HTMLP_Endianness_t;

/* Deprecated - Here for backwards compatibility */
typedef enum NW_HTMLP_Endianness_e NW_Endianness_t;

/** ----------------------------------------------------------------------- **
    @struct:      NW_HTMLP_LineColumn

    @synopsis:    Line and column cursor.

    @scope:       public
    @variables:
       NW_Uint32 crCount
                  Carriage return count.

       NW_Uint32 lfCount
                  Line feed count.

       NW_Uint32 charsSinceLastCR
                  Chars since last CR

       NW_Uint32 charsSinceLastLF
                  Chars since last LF

    @description: Line and column cursor.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_HTMLP_LineColumn_s {
  NW_Uint32 crCount;
  NW_Uint32 lfCount;
  NW_Uint32 charsSinceLastCR;
  NW_Uint32 charsSinceLastLF;
  
} NW_HTMLP_LineColumn_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_HTMLP_Lexer_Position

    @synopsis:    Lexer position state.

    @scope:       public
    @variables:
       NW_Uint32 readPosition
                  Read position.

       NW_Uint32 charPosition
                  Character position.

       NW_Uint32 crCount
                  CR count.

       NW_Uint32 lfCount
                  LF count.

       NW_Uint32 charsSinceLastCR
                  Chars since last CR.

       NW_Uint32 charsSinceLastLF
                  Chars since last LF.

       NW_Bool end
                  End.

    @description: This struct holds all information relevant to saving and 
                  restoring the lexer's read position.  It encapsulates
                  discrete bits of information and is not used otherwise.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_HTMLP_Lexer_Position_s {
  NW_Uint32 readPosition;
  NW_Uint32 charPosition;
  NW_Uint32 crCount;
  NW_Uint32 lfCount;
  NW_Uint32 charsSinceLastCR;
  NW_Uint32 charsSinceLastLF;
  NW_Bool end;  
} NW_HTMLP_Lexer_Position_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_HTMLP_Lexer

    @synopsis:    Lexer state.

    @scope:       volatile
    @variables:
       NW_Uint32 encoding
                  Encoding.

       NW_Endianness_t endianness
                  Big endian, little endian or native.

       NW_Uint32 readPosition
                  Read position.

       NW_Uint32 charPosition
                  Character position.

       NW_HTMLP_LineColumn_t lineColumn
                  Line column position.

       NW_Uint32 byteCount
                  Byte count.

       NW_Uint8* pBuf
                  Buffer size.

       NW_Bool end
                  End.

       NW_HTMLP_ElementTableIndex_t elementCount
                  Element count.

       NW_HTMLP_ElementDescriptionConst_t* pElementDictionary
                  Element dictionary.

    @description: Users of the lexer should not look directly inside this 
                  structure.  Instead, use access functions and macros.
 ** ----------------------------------------------------------------------- **/
typedef volatile struct NW_HTMLP_Lexer_s {
  /* character set and encoding currently applicable */
  NW_Uint32 encoding;
  NW_Endianness_t endianness;

  /* current read position in pBuf, this is a byte index */
  NW_Uint32 readPosition;

  /* like readPosition but in terms of characters not bytes */
  NW_Uint32 charPosition;

  /* information to track line and column position in text */
  NW_HTMLP_LineColumn_t lineColumn;

  /* text is stored in pBuf which is byteCount bytes long */
  NW_Uint32 byteCount;
  NW_Uint8* pBuf;

  /* similar to EOF detection */
  NW_Bool end;

  /* count of entries in the element description dictionary */
  NW_HTMLP_ElementTableIndex_t elementCount;

  /* pointer to the element description dictionary */
  NW_HTMLP_ElementDescriptionConst_t* pElementDictionary;
  
} NW_HTMLP_Lexer_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_HTMLP_Interval_t

    @synopsis:    Interval state.

    @scope:       public
    @variables:
       NW_Uint32 start
                  Start position.

       NW_Uint32 stop
                  Stop position.

       NW_Uint32 charStart
                  Start character.

       NW_Uint32 charStop
                  Stop character.

    @description: Users of the this structure should use use the defined
                  access functions and macros where appropriate.

                  This structure holds byte indicies defining a
                  substring in the text buffer.  If the text is "this is
                  a sample" and the interval has start = 0, stop = 4,
                  then the substring is "this".  Byte length is stop -
                  start.  If both values are UINT_MAX, then these are
                  sentinel values meaning the interval has been
                  initializaed but not defined.  If both values are the
                  same, it means start has been set but stop hasn't been
                  set.

                  Because characters may be encoded in a variable number
                  of bytes, a separate but parallel set of numbers
                  tracks character counts and "positions".

                  Note: Along with an interval, you may also need to
                  record the encoding that applies when working with
                  mixed character sets and encodings.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_HTMLP_Interval_s {
  /* the byte indices */
  NW_Uint32 start;
  NW_Uint32 stop;

  /* the char indices */
  NW_Uint32 charStart;
  NW_Uint32 charStop;
  
} NW_HTMLP_Interval_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_InitFromBuffer

    @synopsis:    Initialize lexer from a buffer.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_Uint32 byteCount
                  Byte count.

       [in] NW_Uint8* pBuf
                  Buffer.

       [in] NW_HTMLP_ElementTableIndex_t elementCount
                  Element count.

       [in] NW_HTMLP_ElementDescriptionConst_t* pElementDictionary
                  Element dictionary.

    @description: Initialize lexer from a buffer. Does not copy the buffer.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  initialized

       [KBrsrFailure]
                  initialization failed

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_InitFromBuffer(NW_HTMLP_Lexer_t* pL,
                                          NW_Uint32 byteCount,
                                          NW_Uint8* pBuf,
                                          NW_HTMLP_ElementTableIndex_t elementCount,
                                          NW_HTMLP_ElementDescriptionConst_t* pElementDictionary);

#define NW_HTMLP_Lexer_GetEncoding(pL) ((const)((NW_HTMLP_Lexer_t*)pL)->encoding)
#define NW_HTMLP_Lexer_SetEncoding(pL, e) ((pL)->encoding = (e))

#define NW_HTMLP_Lexer_GetEndianness(pL) ((const)((NW_HTMLP_Lexer_t*)pL)->endianness)
#define NW_HTMLP_Lexer_SetEndianness(pL, e) ((pL)->endianness = (e))


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_DataAddressFromBuffer

    @synopsis:    Create data address from offset.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  Lexer.

       [in] NW_Uint32 byteIndex
                  Byte index.

       [in] NW_Uint32* pByteCount
                  Byte count.

       [out] NW_Uint8** ppData
                  Pointer into buffer.

    @description: Returns a pointer in *ppData into pBuf at some byte position
                  and byte count. Byte count is truncated to fit in pBuf if required.


    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Address created.

       [KBrsrFailure]
                  Index out of bounds.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_DataAddressFromBuffer(NW_HTMLP_Lexer_t* pL,
                                                 NW_Uint32 byteIndex,
                                                 NW_Uint32* pByteCount,
                                                 NW_Uint8** ppData);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_GetPosition

    @synopsis:    Gets position state.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [out] NW_HTMLP_Lexer_Position_t* pPosition
                  Current position.

    @description: Provides everything needed to save (and then restore) current
                  read position.

 ** ----------------------------------------------------------------------- **/
void NW_HTMLP_Lexer_GetPosition(NW_HTMLP_Lexer_t* pL, NW_HTMLP_Lexer_Position_t* pPosition);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_SetPosition

    @synopsis:    Set position in lexer.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_HTMLP_Lexer_Position_t* pPosition
                  Desired position.

    @description: Setting the position (similar to seeking in a file) is
                  in general not possible without reading the characters
                  (usually reading forward) because character encoding
                  may use a variable numbers of bytes per character.  If
                  you need to save and restore a position, say because
                  you have defined an interval, then use this function.
                  Setting the position to a misaligned location will not
                  always be caught immediately. You must also save and set
                  line and column with position. 

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Position set.

       [KBrsrFailure]
                  Index out of bounds.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_SetPosition(NW_HTMLP_Lexer_t* pL, NW_HTMLP_Lexer_Position_t* pPosition);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_GetLineColumn

    @synopsis:    Get line column.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [out] NW_Uint32* pLine
                  Line position.

       [out] NW_Uint32* pColumn
                  Column position.

    @description: Returns an estimate of the current line and column position 
                  in the text. It is an estimate because it has to guess at 
                  what the intended line ending sequence is using a count of 
                  CR and LF characters.  Line and Column indices are 1-based 
                  not 0-based.
 ** ----------------------------------------------------------------------- **/
void NW_HTMLP_Lexer_GetLineColumn(NW_HTMLP_Lexer_t* pL,
                                  NW_Uint32* pLine,
                                  NW_Uint32* pColumn);

#define NW_HTMLP_Lexer_AtEnd(pL) (((NW_HTMLP_Lexer_t*)pL)->end == NW_TRUE)


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_PeekOffset

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_Uint32 offsetCharCount
                  Offset into buffer.

       [out] NW_Uint32* pChar
                  Character position of cursor.

       [out] NW_Bool* pEOF
                  NW_TRUE if at end of buffer, otherwise NW_FALSE.

    @description: Fails on encountering illegal char anywhere from current 
                  read position to offset position or if position is illegal 
                  (e.g., past end).  Leaves read position unchanged.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Position returned.

       [KBrsrFailure]
                  Index out of bound or invalid character.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_PeekOffset(NW_HTMLP_Lexer_t* pL,
                                      NW_Uint32 offsetCharCount,
                                      NW_Uint32* pChar,
                                      NW_Bool* pEOF);

#define NW_HTMLP_Lexer_Peek(pL, pChar, pEOF) NW_HTMLP_Lexer_PeekOffset((pL), 0, (pChar), pEOF)


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_AdvanceOffset

    @synopsis:    Advance offset.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_Uint32 offsetCharCount
                  Offset to advance.

    @description: Fails on encountering illegal char anywhere from current 
                  read position to offset position or if position is illegal 
                  (e.g., past end).  Moves read position to current + offset.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Position advanced.

       [KBrsrFailure]
                  Index out of bound or invalid character.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_AdvanceOffset(NW_HTMLP_Lexer_t* pL,
                                         NW_Uint32 offsetCharCount);

#define NW_HTMLP_Lexer_Advance(pL) NW_HTMLP_Lexer_AdvanceOffset((pL), 1)


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_AsciiCharCompare

    @synopsis:    Compare character.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_Uint8 c
                  Character to compare.

       [out] NW_Bool* pMatch
                  NW_TRUE if characters match, otherwise NW_FALSE.

    @description: *pMatch is NW_TRUE if ASCII character c matches Reader 
                  char in its encoding.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Characters were compared. pMatch has results.

       [KBrsrFailure]
                  Characters were not compared. Index out of bound or 
                  invalid character.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_AsciiCharCompare(NW_HTMLP_Lexer_t* pL,
                                            NW_Uint8 c,
                                            NW_Bool* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_AsciiStringCompare

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_Uint32 asciiCharCount
                  Character count for comparison.

       [in] const NW_Uint8* pString
                  Characters to compare.

       [out] NW_Bool* pMatch
                  NW_TRUE if string matches.

    @description: *pMatch is NW_TRUE if ASCII string matches buffer text 
                  (in charset and encoding).

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Strings were compared. pMatch has results.

       [KBrsrFailure]
                  Strings were not compared. Index out of bound or 
                  invalid character.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_AsciiStringCompare(NW_HTMLP_Lexer_t* pL,
                                              NW_Uint32 asciiCharCount,
                                              const NW_Uint8* pString,
                                              NW_Bool* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_AsciiStringCompareCase

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [in] NW_Uint32 asciiCharCount
                  Character count for comparison.

       [in] const NW_Uint8* pString
                  Characters to compare.
       
       [in] NW_Bool CaseSensitive
                  Compare with case sensitive or not

       [out] NW_Bool* pMatch
                  NW_TRUE if string matches.

    @description: *pMatch is NW_TRUE if ASCII string matches buffer text 
                  (in charset and encoding).

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Strings were compared. pMatch has results.

       [KBrsrFailure]
                  Strings were not compared. Index out of bound or 
                  invalid character.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_AsciiStringCompareCase(NW_HTMLP_Lexer_t* pL,
                                              NW_Uint32 asciiCharCount,
                                              const NW_Uint8* pString,
                                              NW_Bool CaseSensitive,
                                              NW_Bool* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_IsSpace

    @synopsis:    Checks for space.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [out] NW_Bool* pMatch
                  NW_TRUE if a space, otherwise NW_FALSE.

    @description: Checks for space.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Character checked.

       [KBrsrFailure]
                  Character not checked. Cursor at end of file.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_IsSpace(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch);

/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_IsCRLF

    @synopsis:    Checks for CR or LF.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [out] NW_Bool* pMatch
                  NW_TRUE if a CR or LF, otherwise NW_FALSE.

    @description: Checks for CR or LF.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Character checked.

       [KBrsrFailure]
                  Character not checked. Cursor at end of file.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_IsCRLF(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch);

/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_IsAsciiLetter

    @synopsis:    Checks for letter.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [out] NW_Bool* pMatch
                  NW_TRUE if a letter, otherwise NW_FALSE.

    @description: Checks for letter.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Character checked.

       [KBrsrFailure]
                  Character not checked. Cursor at end of file.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_IsAsciiLetter(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Lexer_IsAsciiDigit

    @synopsis:    Checks for digit.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* pL
                  The lexer.

       [out] NW_Bool* pMatch
                  NW_TRUE if an ASCII digit, otherwise NW_FALSE.

    @description: Checks for ASCII digit.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Character checked.

       [KBrsrFailure]
                  Character not checked. Cursor at end of file.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Lexer_IsAsciiDigit(NW_HTMLP_Lexer_t* pL, NW_Bool* pMatch);


/* Interval Functions */

/* Always initialize the interval. */
#define NW_HTMLP_Interval_Init(pI) \
((pI)->start = (pI)->stop = (pI)->charStart = (pI)->charStop = NW_UINT32_MAX)

/* Useful in error tests and asserts. */
#define NW_HTMLP_Interval_IsWellFormed(pI) \
(((pI)->stop > (pI)->start) && ((pI)->charStop > (pI)->charStart))

/* Sets start, stop, charStart, charStop to current read position. */
void NW_HTMLP_Interval_Start(NW_HTMLP_Interval_t* pI, NW_HTMLP_Lexer_t* pL);

/* Sets stop to current reader position. */
void NW_HTMLP_Interval_Stop(NW_HTMLP_Interval_t* pI, NW_HTMLP_Lexer_t* pL);

#define NW_HTMLP_Interval_ByteCount(pI) ((pI)->stop - (pI)->start)
#define NW_HTMLP_Interval_CharCount(pI) ((pI)->charStop - (pI)->charStart)

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* NW_HTMLP_LEXER_H */
#endif
