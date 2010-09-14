/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

  HTML Parser
  
*/
#ifndef FEA_RME_NOHTMLPARSER

#include "nwx_defs.h"
#include "CHtmlpParser.h"
#include <nw_string_char.h>
#include "BrsrStatusCodes.h"
#include "nwx_string.h"
#include "nw_htmlp_to_wbxml.h"
#include "nw_htmlp_html_dict.h"
#include "nw_htmlp_wml_dict.h"
#include <nw_wbxml_dictionary.h>
#include <nw_encoder_stringtable.h>

/* "<?" len 2 */
#define NW_HTMLP_String_PiFormStartLength 2
static
const NW_Uint8 NW_HTMLP_String_PiFormStart[NW_HTMLP_String_PiFormStartLength] =
  {
  '<', '?'
  };

/* "?>" len 2 */
#define NW_HTMLP_String_PiFormStopLength 2
static
const NW_Uint8 NW_HTMLP_String_PiFormStop[NW_HTMLP_String_PiFormStopLength] =
  {
  '?', '>'
  };

/* "<!DOCTYPE" len 9 */
#define NW_HTMLP_String_DoctypeStartLength 9
static
const NW_Uint8 NW_HTMLP_String_DoctypeStart[NW_HTMLP_String_DoctypeStartLength] =
  {
  '<', '!', 'D', 'O', 'C', 'T', 'Y', 'P', 'E'
  };

/* "<!--" len 4 comment start */
#define NW_HTMLP_String_CommentStartLength 4
static
const NW_Uint8 NW_HTMLP_String_CommentStart[NW_HTMLP_String_CommentStartLength] =
  {
  '<', '!', '-', '-'
  };

/* "-->" len 3 comment end */
#define NW_HTMLP_String_CommentStopLength 3
static
const NW_Uint8 NW_HTMLP_String_CommentStop[NW_HTMLP_String_CommentStopLength] =
  {
  '-', '-', '>'
  };

/* "<!--" len 3 comment start */
#define NW_HTMLP_String_ImodeCommentStartLength 3
static
const NW_Uint8 NW_HTMLP_String_ImodeCommentStart[NW_HTMLP_String_ImodeCommentStartLength] =
  {
  '<', '!', '-'
  };

/* "-->" len 3 comment end */
#define NW_HTMLP_String_ImodeCommentStopLength 2
static
const NW_Uint8 NW_HTMLP_String_ImodeCommentStop[NW_HTMLP_String_ImodeCommentStopLength] =
  {
  '-', '>'
  };

/* "-->" len 3 comment end */
#define NW_HTMLP_String_Comment2StopLength 4
static
const NW_Uint8 NW_HTMLP_String_Comment2Stop[NW_HTMLP_String_Comment2StopLength] =
  {
  '-', '-','!','>'
  };

/* "<!" len 2 comment start */
#define NW_HTMLP_String_LooseCommentStartLength 2
static
const NW_Uint8 NW_HTMLP_String_LooseCommentStart[NW_HTMLP_String_LooseCommentStartLength] =
  {
  '<', '!'
  };

/* "<![CDATA[" len 9 */
#define NW_HTMLP_String_CdataStartLength 9
static
const NW_Uint8 NW_HTMLP_String_CdataStart[NW_HTMLP_String_CdataStartLength] =
  {
  '<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['
  };

/* "]]>" len 3 */
#define NW_HTMLP_String_CdataStopLength 3
static
const NW_Uint8 NW_HTMLP_String_CdataStop[NW_HTMLP_String_CdataStopLength] =
  {
  ']', ']', '>'
  };

/* "/>" len 2 */
#define NW_HTMLP_String_MiniEndLength 2
static
const NW_Uint8 NW_HTMLP_String_MiniEnd[NW_HTMLP_String_MiniEndLength] =
  {
  '/', '>'
  };

/* ">" len 1 */
#define NW_HTMLP_String_TagEndLength 1
static
const NW_Uint8 NW_HTMLP_String_TagEnd[NW_HTMLP_String_TagEndLength] =
  {
  '>'
  };

/* "</script>" len 9 */

#define NW_HTMLP_String_EndScriptLength 9
static
const NW_Uint8 NW_HTMLP_String_EndScript[NW_HTMLP_String_EndScriptLength] =
  {
  '<', '/','s','c','r','i','p','t','>'
  };

// =" or =' len 2
#define NW_HTMLP_String_EqualQuoteLength 2
static
const NW_Uint8 NW_HTMLP_String_EqualDblQuote[NW_HTMLP_String_EqualQuoteLength] =
  {
  '=', '\"'
  };
static
const NW_Uint8 NW_HTMLP_String_EqualSngQuote[NW_HTMLP_String_EqualQuoteLength] =
  {
  '=', '\''
  };

// TODO: Throughout this class iCBs is checked against null.  It appears that iCBs is 
//       created in the constructor and not deallocated until the destructor.  If 
//       this is the case all of the iCBs null check should be removed.

//lint -esym(794, CHtmlpParser::iCBs) Conceivable use of null pointer

/* constructors and destructor of CHtmlpParser */

CHtmlpParser* CHtmlpParser::NewL(     
                                 NW_Uint32 inputByteCount, 
                                 NW_Uint8* pInputBuf, 
                                 NW_HTMLP_ElementTableIndex_t elementCount,
                                 NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                                 NW_Uint32 encoding,
                                 NW_Bool consumeSpaces,
                                 void * parser,
                                 NW_Bool isScript)
  {
  CHtmlpParser * self = (CHtmlpParser *)parser;
  if (!self)
    {
    self = new (ELeave) CHtmlpParser(consumeSpaces);
    CleanupStack::PushL(self);
    self->ConstructL(inputByteCount, pInputBuf, elementCount, pElementDictionary, encoding, isScript);
    CleanupStack::Pop(); //instance
    return self;
    }
  else
    {
    User::LeaveIfError(
      NW_HTMLP_Lexer_InitFromBuffer(self->iLexer, inputByteCount, pInputBuf, elementCount, pElementDictionary));
    NW_HTMLP_Lexer_SetEncoding(self->iLexer, encoding);
    return self;
    }
  }

CHtmlpParser::CHtmlpParser( NW_Bool consumeSpaces)
  {
  iLastTextBuf = NULL;
  iLastValid = -1;
  iPreviousValidOutput = NULL;
  iConsumeSpaces = consumeSpaces;
  iRestarted = NW_FALSE;
  iWithinNoscript = NW_FALSE;
  }

void CHtmlpParser::ConstructL(NW_Uint32 inputByteCount, 
                              NW_Uint8* pInputBuf, 
                              NW_HTMLP_ElementTableIndex_t elementCount,
                              NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                              NW_Uint32 encoding,
                              NW_Bool isScript)
  {
  iSPLElemHandling = (NW_HTMLP_SPL_Elem_Handling_t*)NW_Mem_Malloc(sizeof(NW_HTMLP_SPL_Elem_Handling_t));
  User::LeaveIfNull( iSPLElemHandling );
  
  NW_HTMLP_ElementParseState_New();
  User::LeaveIfNull( iElementParseState );
  
  iCBs = (NW_HTMLP_EventCallbacks_t*)NW_Mem_Malloc(sizeof(NW_HTMLP_EventCallbacks_t));
  User::LeaveIfNull( iCBs );
  NW_Mem_memset(iCBs, 0, sizeof(NW_HTMLP_EventCallbacks_t));
  
  iLexer = (NW_HTMLP_Lexer_t* )NW_Mem_Malloc(sizeof(NW_HTMLP_Lexer_t));
  /* Reregister the document text with the Reader in preparation for parsing. */
  User::LeaveIfError(
    NW_HTMLP_Lexer_InitFromBuffer(iLexer, inputByteCount, pInputBuf, elementCount, pElementDictionary));
  
  NW_HTMLP_Lexer_SetEncoding(iLexer, encoding);
  
  /* Null out all WBXML generation callback addresses. */
  NW_Mem_memset(iCBs, 0, sizeof(NW_HTMLP_EventCallbacks_t));
  
  iOrigEncoding = -1;
  iLeftBytes = NULL;
  iIsScript = isScript;
  }

CHtmlpParser::~CHtmlpParser()
  {
  NW_Mem_Free(iSPLElemHandling);
  
  if (iElementParseState)
    {
    NW_HTMLP_ElementParseState_Delete(&iElementParseState);
    }
  if (iLastValidStack)
    {
    NW_HTMLP_ElementParseState_Delete(&iLastValidStack);
    }
  
  if (iCBs->charsetContext)
    {
    NW_Mem_Free(iCBs->charsetContext);
    }
  NW_Mem_Free(iCBs);
  
  NW_Mem_Free ((void *)iLexer);
  NW_Buffer_Free(iLastTextBuf);
  NW_Buffer_Free(iPreviousValidOutput);
  NW_Buffer_Free(iVisitedHeadText);
  }


  /*
  on entry: lexer read position is at the first character of keyword
  on return: If matched keyword, then *pMatch == NW_TRUE, interval marks
  .........: keyword in doc and lexer read position is just after keyword.
  .........: NOTE: Keyword match just means the string of keyword chars
  .........: exists at the read position so it does not mean that the keyword
  .........: is delimited at the end---it might be followed by more name chars.
  .........: If did not match keyword, then *pMatch == NW_FALSE and lexer
  .........: read position is unchanged from on entry.
  eof handling: if encounters EOF while attempting operation then returns
  ............: *pMatch == NW_FALSE and KBrsrSuccess
  on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
  ...............: and lexer read position is unspecified
*/
TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfExistsConsumeKeywordCase(NW_HTMLP_Interval_t* pI,
                                                                     NW_Uint32 asciiCharCount,
                                                                     const NW_Uint8* pKeyword,
                                                                     NW_Bool CaseSensitive,
                                                                     NW_Bool* pMatch)
  {
  NW_HTMLP_Lexer_Position_t position;
  TBrowserStatusCode e;
  NW_Bool match;
  
  *pMatch = NW_FALSE;
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  NW_HTMLP_Interval_Init(pI);
  e = NW_HTMLP_Lexer_AsciiStringCompareCase(iLexer, asciiCharCount, pKeyword, CaseSensitive, &match);
  if (e == KBrsrSuccess) {
    if (match == NW_TRUE) {
      NW_HTMLP_Interval_Start(pI, iLexer);
      e = NW_HTMLP_Lexer_AdvanceOffset(iLexer, asciiCharCount);
      if (BRSR_STAT_IS_FAILURE(e)) {
        NW_HTMLP_Interval_Init(pI);
        return e;
        }
      NW_HTMLP_Interval_Stop(pI, iLexer);
      *pMatch = NW_TRUE;
      } else {
      NW_HTMLP_Lexer_SetPosition(iLexer, &position);
        }
    }
  return e;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfExistsConsumeKeyword(NW_HTMLP_Interval_t* pI,
                                                                 NW_Uint32 asciiCharCount,
                                                                 const NW_Uint8* pKeyword,
                                                                 NW_Bool* pMatch)
  {
  
  return NW_HTMLP_IfExistsConsumeKeywordCase( pI,
    asciiCharCount,
    pKeyword,
    NW_TRUE,
    pMatch);
  }

  /*
  on entry: no assumptions
  on return: Lexer read position moved ahead until character at current
  .........: read position is not whitespace.  Return value is KBrsrSuccess.
  eof handling: if encounters EOF then stops at that point and returns
  ............: KBrsrSuccess
  on error return: return value is not KBrsrSuccess and lexer read position
  ...............: is unspecified
*/

TBrowserStatusCode CHtmlpParser::NW_HTMLP_SkipSpace()
  {
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Bool match = NW_FALSE;
  
  while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
    e = NW_HTMLP_Lexer_IsSpace(iLexer, &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      } else {
      break;
        }
    }
  return e;
  }

  /*
  on entry: no assumptions
  on return: Lexer read position moved ahead until character at current
  .........: read position is not whitespace or a 'junk' character.
  .........: The junk characters were found on live websites.
  .........: Return value is KBrsrSuccess.
  eof handling: if encounters EOF then stops at that point and returns
  ............: KBrsrSuccess
  on error return: return value is not KBrsrSuccess and lexer read position
  ...............: is unspecified
*/

TBrowserStatusCode CHtmlpParser::NW_HTMLP_SkipJunk()
{
TBrowserStatusCode e = KBrsrSuccess;
NW_Bool match = NW_FALSE;

while (!NW_HTMLP_Lexer_AtEnd(iLexer)) 
    {
    
    e = NW_HTMLP_Lexer_IsSpace(iLexer, &match);
    if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
    
    if (match == NW_FALSE) 
        {
        e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, ';', &match);
        if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
        }
    
    if (match == NW_FALSE) 
        {
        e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '+', &match);
        if (BRSR_STAT_IS_FAILURE(e)) 
            {
            return e;
            }
        }
    
    if (match == NW_TRUE) 
        {
        e = NW_HTMLP_Lexer_Advance(iLexer);
        if (BRSR_STAT_IS_FAILURE(e)) 
            {
            return e;
            }
        } 
    else 
        {
        break;
        }
    }
return e;
}

  /*
  on entry: no assumptions
  on return: Lexer read position moved ahead until character at current
  .........: read position is not CR or LF.  Return value is KBrsrSuccess.
  eof handling: if encounters EOF then stops at that point and returns
  ............: KBrsrSuccess
  on error return: return value is not KBrsrSuccess and lexer read position
  ...............: is unspecified
*/

TBrowserStatusCode CHtmlpParser::NW_HTMLP_SkipCRLF()
  {
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Bool match = NW_FALSE;
  
  while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
    e = NW_HTMLP_Lexer_IsCRLF(iLexer, &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      } else {
      break;
        }
    }
  return e;
  }

  /*
  A name (e.g., tag name) matches the pattern "[a-zA-Z][a-zA-Z0-9.-_:]*\s"
  Assumes starting read position is at the first character of name.
  On return: *pMatch = NW_TRUE if found name, NW_FALSE if no name parsed
  On return: If *pMatch == NW_TRUE, then *pI marks the name.
  
    TBD: loosen this up to allow the pattern [a-zA-Z][^/?]*\s
    don't allow ? due to ?> on PI forms
    don't allow / due to /> on empty element
*/

TBrowserStatusCode CHtmlpParser::NW_HTMLP_ParseName(NW_Bool* pMatch,
                                                    NW_HTMLP_Interval_t* pI)
  {
  NW_HTMLP_Lexer_Position_t position;
  TBrowserStatusCode sl = KBrsrSuccess;
  NW_Bool isLetter = NW_FALSE;
  TBrowserStatusCode sd = KBrsrSuccess;
  NW_Bool isDigit = NW_FALSE;
  TBrowserStatusCode sp = KBrsrSuccess;
  NW_Bool isPeriod = NW_FALSE;
  TBrowserStatusCode sh = KBrsrSuccess;
  NW_Bool isHyphen = NW_FALSE;
  TBrowserStatusCode su = KBrsrSuccess;
  NW_Bool isUnderscore = NW_FALSE;
  TBrowserStatusCode sc = KBrsrSuccess;
  NW_Bool isColon = NW_FALSE;
  TBrowserStatusCode s = KBrsrSuccess;
  
  *pMatch = NW_FALSE;
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  NW_HTMLP_Interval_Init(pI);
  sl = NW_HTMLP_Lexer_IsAsciiLetter(iLexer, &isLetter);
  if (!BRSR_STAT_IS_FAILURE(sl)) {
    if (isLetter) {
      NW_HTMLP_Interval_Start(pI, iLexer);
      while ((!BRSR_STAT_IS_FAILURE(sl) && !BRSR_STAT_IS_FAILURE(su)
        && !BRSR_STAT_IS_FAILURE(sc) && !BRSR_STAT_IS_FAILURE(sd)
        && !BRSR_STAT_IS_FAILURE(sp) && !BRSR_STAT_IS_FAILURE(sh)
        && !BRSR_STAT_IS_FAILURE(s))
        && (isLetter || isDigit || isPeriod || isHyphen
        || isUnderscore || isColon)) {
        s = NW_HTMLP_Lexer_Advance(iLexer);
        sl = NW_HTMLP_Lexer_IsAsciiLetter(iLexer, &isLetter);
        sd = NW_HTMLP_Lexer_IsAsciiDigit(iLexer, &isDigit);
        sp = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '.', &isPeriod);
        sh = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '-', &isHyphen);
        su = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '_', &isUnderscore);
        sc = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, ':', &isColon);
        }
      NW_HTMLP_Interval_Stop(pI, iLexer);
      *pMatch = NW_TRUE;
      }
    }
  if (BRSR_STAT_IS_FAILURE(sl) || BRSR_STAT_IS_FAILURE(su)
    || BRSR_STAT_IS_FAILURE(sc) || BRSR_STAT_IS_FAILURE(sd)
    || BRSR_STAT_IS_FAILURE(sp) || BRSR_STAT_IS_FAILURE(sh)
    || BRSR_STAT_IS_FAILURE(s)) {
    return KBrsrFailure;
    }
  if (*pMatch == NW_FALSE) {
    s = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
    if (BRSR_STAT_IS_FAILURE(s)) {
      return s;
      }
    }
  return KBrsrSuccess;
  }

  /*
  on entry: lexer read position is just after '<!--'
  on return: If skipped comment, then *pMatch == NW_TRUE  and
  .........: lexer read position is just after closing '>'.
  .........: If did not skip comment, then *pMatch == NW_FALSE and
  .........: lexer read position is unspecified.
  .........: In either case return value is KBrsrSuccess.
  eof handling: if encounters EOF while attempting operation then returns
  ............: *pMatch == NW_FALSE and KBrsrSuccess with lexer read
  ............: position at EOF
  on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
  ...............: and lexer read position is unspecified
*/
TBrowserStatusCode CHtmlpParser::NW_HTMLP_FinishComment(NW_Bool* pMatch, 
                                                        NW_Bool imodeComment, 
                                                        NW_Bool looseComment)
  {
  NW_HTMLP_Lexer_Position_t position;
  NW_HTMLP_Interval_t interval;
  TBrowserStatusCode e;
  NW_Bool match = NW_FALSE;
  NW_Uint8* pKeyword = NULL;
  NW_Uint8* pKeyword2 = NULL;
  NW_Uint32 count = 0;
  NW_Uint32 count2 = 0;
  NW_Bool foundFirstElement = NW_FALSE; 
  NW_HTMLP_Lexer_Position_t fistElementPosition;
  TBrowserStatusCode status = KBrsrSuccess;
  
  /* Does three forms of comment:
  "strict" where comment must end with exactly '-->'
  (no whitespace allowed between '--' and '>' as in specification)
  "imode" where comments ends at "->"
  "loose" where comment ends at first '>'
  First apply "strict" and if that fails to match, then apply "loose".
  
    Note: This is slightly different than the specifications but conforms
  to common browser behavior. */
  
  *pMatch = NW_FALSE;
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  
  if(looseComment)
    {
    pKeyword =(NW_Uint8*)&NW_HTMLP_String_TagEnd;
    count    = NW_HTMLP_String_TagEndLength;
    }
  else if(imodeComment)
    {
    pKeyword = (NW_Uint8*) &NW_HTMLP_String_ImodeCommentStop;
    count    = NW_HTMLP_String_ImodeCommentStopLength;
    }
  else
    {
    pKeyword = (NW_Uint8*) &NW_HTMLP_String_CommentStop;
    count    = NW_HTMLP_String_CommentStopLength;
    pKeyword2 = (NW_Uint8*) &NW_HTMLP_String_Comment2Stop;
    count2    = NW_HTMLP_String_Comment2StopLength;
    
    }
  
  while (!NW_HTMLP_Lexer_AtEnd(iLexer)) 
    {
    // If the loose, imode, or normal end-comment is found comsume  
    // it and set pMatch to true. 
    e = NW_HTMLP_IfExistsConsumeKeyword(&interval, count, 
        pKeyword, &match); 
    if (BRSR_STAT_IS_FAILURE(e)) 
      {
      status = e;
      break;
      }
    
    if (match == NW_TRUE) 
      {
      *pMatch = NW_TRUE;
      break;
      }

    // If the "--!>" end-comment is found comsume it and set 
    // pMatch to true.
    if (pKeyword2)
      {
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval, count2, 
          pKeyword2, &match);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        status = e;
        break;
        }
      }
    if (match == NW_TRUE) 
      {
      *pMatch = NW_TRUE;
      break;
      }

    // As it searches for the end-comment it also keeps track of  
    // the first embeded tag.  This tag is later used to force end 
    // the comment if it reaches the end of the file before finding 
    // a end-comment. 
    if (!foundFirstElement) 
      { 
      e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '<', &match); 
      if (BRSR_STAT_IS_FAILURE(e))  
        { 
        status = e; 
        break; 
        } 
      if (match == NW_TRUE) 
        { 
        foundFirstElement = NW_TRUE; 
        NW_HTMLP_Lexer_GetPosition(iLexer, &fistElementPosition); 
        } 
      } 

    // Otherwise advance a char and test for end-comment again. 
    e = NW_HTMLP_Lexer_Advance(iLexer);
    
    if (BRSR_STAT_IS_FAILURE(e))
      {
      status = e;
      break;
      }
    }/*end while*/
  

  // If end-comment wasn't found in this block, it may be missing.
  if ((match == NW_FALSE) && NW_HTMLP_Lexer_AtEnd(iLexer) && foundFirstElement) 
    { 
    // If the document is completed then terminate the comment just 
    // before the first start tag after begin-comment.
    if (iDocComplete)
      {
      NW_HTMLP_Lexer_SetPosition(iLexer, &fistElementPosition); 
      *pMatch = NW_TRUE; 
      }
    
    // Otherwise note that we may be tracking a unterminated comment.
    else
      {
      iTrackingUnTerminatedComment = NW_TRUE;
      }
    }

  
  return status;
  }


  /*
  on entry: assumes lexer read position is character following '<![CDATA[?'
  on return: If consumed a well-formed CDATA, then *pMatch == NW_TRUE and
  .........: lexer read position is just after closing ']]>'.
  .........: If did not consume CDATA, then *pMatch == NW_FALSE and
  .........: lexer read position is unspecified.
  .........: In either case return value is KBrsrSuccess.
  eof handling: if encounters EOF while attempting operation then returns
  ............: *pMatch == NW_FALSE and KBrsrSuccess with lexer read
  ............: position at EOF
  on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
  ...............: and lexer read position is unspecified
  */
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_FinishCdata(NW_HTMLP_Interval_t* pInterval,
    NW_Bool* pMatch)
    {
    TBrowserStatusCode e;
    NW_HTMLP_Interval_t interval;
    NW_Bool match;
    
    *pMatch = NW_FALSE;
    NW_HTMLP_Interval_Start(pInterval, iLexer);
    
    while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
      NW_HTMLP_Interval_Stop(pInterval, iLexer);
      /* end with match if see ']]>' */
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
        NW_HTMLP_String_CdataStopLength,
        NW_HTMLP_String_CdataStop,
        &match);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (match == NW_TRUE) {
        *pMatch = NW_TRUE;
        break;
        }
      /* advance */
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    return KBrsrSuccess;
    }
  
    /*
    on entry: assumes lexer read position is character following '<!DOCTYPE'
    on return: If consumed a well-formed DOCTYPE, then *pMatch == NW_TRUE and
    .........: lexer read position is just after closing '>'.
    .........: If did not consume a DOCTYPE, then *pMatch == NW_FALSE and
    .........: lexer read position is unspecified.
    .........: In either case return value is KBrsrSuccess.
    eof handling: if encounters EOF while attempting operation then returns
    ............: *pMatch == NW_FALSE and KBrsrSuccess with lexer read
    ............: position at EOF
    on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
    ...............: and lexer read position is unspecified
  */
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_FinishDoctype(NW_Bool* pMatch,
    NW_HTMLP_Interval_t* doctypeInterval)
    {
    /* Because DOCTYPE attributes may include strings and inside these strings
    the characters '>' and '<' may be used, strings must be parsed.  Strings are
    quoted either with '\'' or '\"'. */
    TBrowserStatusCode e;
    NW_HTMLP_Interval_t interval;
    NW_HTMLP_Interval_t arguments;
    NW_HTMLP_Lexer_Position_t position;
    
    *pMatch = NW_FALSE;
    e = NW_HTMLP_SkipSpace();
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    
    NW_HTMLP_Interval_Start(&arguments, iLexer);
    *pMatch = NW_FALSE;
    
    while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
      /* end with match if see '>' */
      NW_HTMLP_Interval_Stop(&arguments, iLexer); /* keep moving along end */
      
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
        NW_HTMLP_String_TagEndLength,
        NW_HTMLP_String_TagEnd,
        pMatch);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (*pMatch == NW_TRUE){
        if ((iCBs != NULL) && (iCBs->piFormCB != NULL)) {
          NW_Uint32 cp_cnt = 0;
          e = (*(iCBs->piFormCB))(iLexer, doctypeInterval, &arguments, iCBs->pClientPointer, &cp_cnt);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          updateCurrentCP(cp_cnt);
          }
        break;
        }
      /* advance */
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    if (*pMatch == NW_FALSE) {
      e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    return KBrsrSuccess;
    }
  
    /*
    on entry: lexer read position is at '<'
    on return: If consumed a comment, then *pMatch == NW_TRUE and
    .........: lexer read position is just after closing '>'.
    .........: If did not consume a comment, then *pMatch == NW_FALSE and
    .........: lexer read position is unchanged.
    .........: In either case return value is KBrsrSuccess.
    .........: There are three types of comments are handled here:
    .........: (1) Exact syntax comment (<!-- Some Text --> )
    .........: (2) Imode Comments       (<!- Some Text ->   )
    .........: (3) Loose Comments       (<! Some Text >     )
    .........: Note: Loose Comments must avoid consuming <!DOCTYPE ...> and <![CDATA ...]]>.
    eof handling: if encounters EOF while attempting operation then returns
    ............: *pMatch == NW_FALSE and KBrsrSuccess with lexer read
    ............: position unchanged
    on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
    ...............: and lexer read position is unspecified
  */
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumeComment(NW_Bool* pMatch)
    {
    NW_HTMLP_Lexer_Position_t position;
    NW_HTMLP_Interval_t interval;
    TBrowserStatusCode e;
    NW_Bool match;
    NW_Bool imodeComment = NW_FALSE;
    NW_Bool looseComment = NW_FALSE;
    NW_Bool isNotComment = NW_FALSE;
    
    iTrackingUnTerminatedComment = NW_FALSE;
    *pMatch = NW_FALSE;
    NW_HTMLP_Lexer_GetPosition(iLexer, &position);
    
    // Check first for correct syntax comments.
    e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
      NW_HTMLP_String_CommentStartLength,
      NW_HTMLP_String_CommentStart,
      &match);
    if (BRSR_STAT_IS_FAILURE(e)) 
      {
      return e;
      }
    
    // Check for IMode type of comments.
    if (match == NW_FALSE)
      {
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
        NW_HTMLP_String_ImodeCommentStartLength,
        NW_HTMLP_String_ImodeCommentStart,
        &match);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      if (match == NW_TRUE)
        imodeComment = NW_TRUE;
      }
    
    
    // Check for <!DOCTYPE before loose comment
    if (match == NW_FALSE)
      {
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
        NW_HTMLP_String_DoctypeStartLength,
        NW_HTMLP_String_DoctypeStart,
        &match);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      if (match == NW_TRUE) 
        {         
        isNotComment = NW_TRUE;      
        e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
        }
      }
    }

    // Check for <![CDATA before loose comment
    if (match == NW_FALSE && isNotComment == NW_FALSE)
      {
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
        NW_HTMLP_String_CdataStartLength,
        NW_HTMLP_String_CdataStart,
        &match);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      if (match == NW_TRUE) 
        { 
        isNotComment = NW_TRUE;
        e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
        }
      }
    }

    // Check for loose comments
    if (match == NW_FALSE && isNotComment == NW_FALSE)
      {
      e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
        NW_HTMLP_String_LooseCommentStartLength,
        NW_HTMLP_String_LooseCommentStart,
        &match);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      if (match == NW_TRUE)
        looseComment = NW_TRUE;
      }

    if (match == NW_TRUE && isNotComment == NW_FALSE) 
      {
      e = NW_HTMLP_FinishComment(&match, imodeComment,
          looseComment);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      if (match == NW_FALSE) 
        {
        if (NW_HTMLP_Lexer_AtEnd(iLexer))
          {
          *pMatch = NW_FALSE;
          iLastTextBegin = position.readPosition;
          if (iLastScriptStart == -1)
            {
            setValidMarks();
          }
          }
        } 
      else 
        {
        *pMatch = NW_TRUE;
          }
      }
    return KBrsrSuccess;
    }
  
    /*
    on entry: assumes lexer read position is at "<"
    on return: If consumed a well-formed PI, then *pMatch == NW_TRUE and
    .........: lexer read position is just after closing '?>'.
    .........: If did not consume a PI, then *pMatch == NW_FALSE and
    .........: lexer read position is unchanged
    eof handling: if encounters EOF while attempting operation then returns
    ............: *pMatch == NW_FALSE and KBrsrSuccess with lexer read
    ............: position unchanged
    on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
    ...............: and lexer read position is unspecified
  */
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumePi(NW_Bool* pMatch)
    {
    TBrowserStatusCode e;
    NW_HTMLP_Interval_t interval;
    NW_HTMLP_Interval_t name;
    NW_HTMLP_Interval_t arguments;
    NW_HTMLP_Lexer_Position_t position;
    NW_Bool isXml = NW_FALSE;
    
    /* The XML spec says that PI forms may not use '?>' anywhere except to end
    the PI form.  This means that quoted strings should not be recognized, but
    this may be something that should be relaxed. */
    NW_HTMLP_Lexer_GetPosition(iLexer, &position);
    e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
      NW_HTMLP_String_PiFormStartLength,
      NW_HTMLP_String_PiFormStart,
      pMatch);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (*pMatch == NW_TRUE) {
      /* parse PITarget */
      e = NW_HTMLP_ParseName(pMatch, &name);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (*pMatch == NW_TRUE) {
        /* Find xml charset definition */
        if (iNeedCharsetDetect && (name.charStop - name.charStart == 3))
          {
          static const NW_Ucs2 xmlStr[] = {'x','m','l','\0'};
          
          if (NW_Byte_Strnicmp((const NW_Byte*)(iLexer->pBuf + name.start), (const NW_Byte*)xmlStr, NW_Str_Strlen(xmlStr)*sizeof(NW_Ucs2)) == 0)
            {
            isXml = NW_TRUE;
            }
          }
        /* skip any whitespace */
        e = NW_HTMLP_SkipSpace();
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        NW_HTMLP_Interval_Start(&arguments, iLexer);
        *pMatch = NW_FALSE;
        while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
          /* End with match if see '?>' */
          NW_HTMLP_Interval_Stop(&arguments, iLexer); /* keep moving along end */
          e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
            NW_HTMLP_String_PiFormStopLength,
            NW_HTMLP_String_PiFormStop,
            pMatch);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          if (*pMatch == NW_TRUE) {
            if ((iCBs != NULL) && (iCBs->piFormCB != NULL)) {
              NW_Uint32 cp_cnt = 0;
              e = (*(iCBs->piFormCB))(iLexer, &name, &arguments, iCBs->pClientPointer, &cp_cnt);
              if (BRSR_STAT_IS_FAILURE(e)) {
                return e;
                }
              updateCurrentCP(cp_cnt);
              }
            break;
            }
          /* advance */
          e = NW_HTMLP_Lexer_Advance(iLexer);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          
          
          if (isXml)
            {
            e = NW_HTMLP_SPL_Elem_Handle_xml_charset();
            if (BRSR_STAT_IS_FAILURE(e)) {
              return e;
              }
            }
          
          }
        }
      }
    if (*pMatch == NW_FALSE) {
      e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    return KBrsrSuccess;
    }
  
    /*
    assumes read position is at "<"
    on return: *pMatch == NW_TRUE if consumed a well-formed CDATA
    if *pMatch == NW_FALSE then read position is unchanged
  */
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumeCdata(NW_Bool* pMatch)
    {
    TBrowserStatusCode e;
    NW_HTMLP_Interval_t interval;
    NW_HTMLP_Lexer_Position_t position;
    
    /* If element is ill-formed:
    1. save location before opening markup
    2. if processing reaches end of doc before ending element,
    then reset to starting markup and return with *pMatch == NW_FALSE */
    
    NW_HTMLP_Lexer_GetPosition(iLexer, &position);
    e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
      NW_HTMLP_String_CdataStartLength,
      NW_HTMLP_String_CdataStart,
      pMatch);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (*pMatch == NW_TRUE) {
      e = NW_HTMLP_FinishCdata(&interval, pMatch);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (*pMatch == NW_FALSE) {
        e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        } else {
        if (iCBs->cdataCB != NULL) {
          e = (*(iCBs->cdataCB))(iLexer, &interval, iCBs->pClientPointer);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          }
          }
      }
    return KBrsrSuccess;
    }
  
    /*
    assumes read position is at "<". This is used to handle the 
    special case such as when comments appears inside the script 
    element.
    
      on return: *pMatch == NW_TRUE if consumed comment type
      if *pMatch == NW_FALSE then read position is unchanged
  */
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_HandleSpecialCases(NW_Bool* pMatch)
    {
    TBrowserStatusCode e;
    
    *pMatch = NW_FALSE;
    
    if(iSPLElemHandling->splHandle == NW_TRUE) 
      {
      switch(iSPLElemHandling->type)
        {
        case NW_HTMLP_SPL_SCRIPT:
          
          //Don't loose the contents inside the script as ECMA script
          //requires these. Also, at the same time consume all of these 
          //so that funny characters are not appreared on the screen.
          //A counter is used for this purpose that will keep track 
          //of all open/close comments ("<--" and "-->" inside the script).
          // It has been found that there can be nested comments inside the
          // <script> or starting <script> can end with </SCRIPT> element.
          
          if(!NW_HTMLP_Lexer_AtEnd(iLexer) )
            {
            NW_Int32 iCommentCnt = 0;
            NW_HTMLP_Interval_t interval;
            NW_HTMLP_Interval_t intervalTotal;
            TBrowserStatusCode s_hyphen = KBrsrSuccess; //Hypen
            NW_Bool isHyphen = NW_FALSE;
            TBrowserStatusCode s_lt = KBrsrSuccess; // '<' for starting tag
            NW_Bool isLT = NW_FALSE;
            NW_Bool isStartComment = NW_FALSE;
            NW_Bool isEndComment   = NW_FALSE;
            NW_Bool isEndScript    = NW_FALSE;
            NW_Bool isTrueEndScript = NW_FALSE;
            NW_Uint32 scriptContentLen = 0;
            
            NW_HTMLP_Interval_Start(&intervalTotal, iLexer);
            
            do
              {
              
              
              //Check comment starting ("<!--") is there in the script
              
              e = NW_HTMLP_IfExistsConsumeKeyword(&interval,
                NW_HTMLP_String_CommentStartLength,
                NW_HTMLP_String_CommentStart,
                &isStartComment);
              
              
              if (BRSR_STAT_IS_FAILURE(e)) 
                {
                return e;
                }
              
              if(isStartComment == NW_TRUE) //It is comment starting ("<"
                {
                // to handle bug WLIU-5Y4LP2: washingtonpost.com where the first comment is not appropriately closed
                // once we see a new comment start, we assume the previous comment ends, and therefore 
                // iCommentCnt can only be 0 or 1
                iCommentCnt = 1;
                isStartComment = NW_FALSE;
                }
              
              /*Check for end of </script> here */
              
              e = NW_HTMLP_Lexer_AsciiStringCompareCase(iLexer,
                NW_HTMLP_String_EndScriptLength,
                NW_HTMLP_String_EndScript,NW_FALSE,
                &isEndScript);
              
              
              if (BRSR_STAT_IS_FAILURE(e)) 
                {
                return e;
                }
              
              if(isEndScript == NW_TRUE && (iCommentCnt == 0))
                {
                //Actual end of the </script>, so we return
                isTrueEndScript = NW_TRUE;
                NW_HTMLP_Interval_Stop(&intervalTotal, iLexer);
                *pMatch = NW_TRUE;
                break;
                }
              
              //Look for following case 
              // (1) Starting comment ("<--")
              // (2) Closing comment ("-->")
              // (3) End tag ("/script") outside comment. If it is 
              //     inside then we don't do anything. Keep reading the
              //     data.
              
              
              
              while ( (!NW_HTMLP_Lexer_AtEnd(iLexer)) &&
                !BRSR_STAT_IS_FAILURE(s_lt) &&
                !BRSR_STAT_IS_FAILURE(s_hyphen)  && 
                (!isHyphen && !isLT)
                )
                {
                
                e = NW_HTMLP_Lexer_Advance(iLexer);
                if (BRSR_STAT_IS_FAILURE(e)) 
                  {
                  return e;
                  }
                
                s_hyphen = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '-', &isHyphen);
                s_lt     = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '<', &isLT);
                }//end while
              
              
              if( BRSR_STAT_IS_FAILURE(s_hyphen) || BRSR_STAT_IS_FAILURE(s_lt) )	
                {
                return KBrsrFailure;
                }
              
              
              if(isLT == NW_TRUE) //Starting '<'
                {
                isLT = NW_FALSE; //For next cycle
                //Check for another starting comment will be taken care in 
                //beginning of loop.
                
                //Check for end script tag
                
                e = NW_HTMLP_Lexer_AsciiStringCompareCase(iLexer,
                  NW_HTMLP_String_EndScriptLength,
                  NW_HTMLP_String_EndScript,NW_FALSE,
                  &isEndScript);
                
                
                if (BRSR_STAT_IS_FAILURE(e)) 
                  {
                  return e;
                  }
                
                
                /* Not actual </script> */
                
                if(isEndScript == NW_TRUE && (iCommentCnt >= 1) )
                  {
                  //The end tag might be in comment, skip 
                  e = NW_HTMLP_Lexer_AdvanceOffset(iLexer, 
                    NW_HTMLP_String_EndScriptLength);
                  if (BRSR_STAT_IS_FAILURE(e)) 
                    {
                    return e;
                    }
                  continue;
                  }
                else if(isEndScript == NW_TRUE && (iCommentCnt == 0))
                  {
                  //Actual end of the </script>, so we return
                  isTrueEndScript = NW_TRUE;
                  NW_HTMLP_Interval_Stop(&intervalTotal, iLexer);
                  *pMatch = NW_TRUE;
                  break;
                  }
                }//end if(s_lt == NW_TRUE) //Starting '<'
              
              if(isHyphen == NW_TRUE)
                {
                isHyphen = NW_FALSE; //For next cycle
                //Check for end of comments "-->"
                e =	NW_HTMLP_IfExistsConsumeKeyword(&interval,
                  NW_HTMLP_String_CommentStopLength,
                  NW_HTMLP_String_CommentStop,
                  &isEndComment);
                
                
                if (BRSR_STAT_IS_FAILURE(e)) 
                  {
                  return e;
                  }
                
                if( (isEndComment == NW_TRUE) && (iCommentCnt > 0) )
                  {
                  iCommentCnt--;
                  /*For next cycle */
                  s_hyphen = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '-', &isHyphen);
                  s_lt     = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '<', &isLT);
                  }//end if(isEndComment == NW_TRUE)
                }//end if(isHyphen == NW_TRUE)
              
          } while (!NW_HTMLP_Lexer_AtEnd(iLexer));
          
          //Now make call back to write these contents as whole.
          //The end of tag will be handled separately in main routine.
          
          
          scriptContentLen = NW_HTMLP_Interval_ByteCount(&intervalTotal);
          
          if( (isTrueEndScript == NW_TRUE) && (scriptContentLen > 0) )
            {
            if (iCBs->contentCB != NULL)
              {
              
              e = (*(iCBs->contentCB))(iLexer, &intervalTotal, iCBs->pClientPointer);
              if (BRSR_STAT_IS_FAILURE(e)) 
                {
                return e;
                }
              }
            iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
            iElementParseState->readPosition = iLexer->readPosition; 
            }//end if(isTrueEndScript == NW_TRUE)
          else
            {
            iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
            }	  
        } //if(!NW_HTMLP_Lexer_AtEnd(iLexer) )
        
        break;
     default:
       break;
    }//end swicth()
    }//end if(iSPLElemHandling->splHandle == NW_TRUE) 
    
    return KBrsrSuccess;
}

/*
assumes read position is at "<"
on return: *pMatch == NW_TRUE if consumed a well-formed DOCTYPE
if *pMatch == NW_FALSE then read position is unchanged
*/

TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumeDoctype(NW_Bool* pMatch)
  {
  TBrowserStatusCode e;
  NW_HTMLP_Interval_t interval;
  NW_HTMLP_Lexer_Position_t position;
  NW_HTMLP_Interval_t doctypeInterval;
  
  /* If element is ill-formed:
  1. save location before opening markup
  2. if processing reaches end of doc before ending element,
  then reset to starting markup and return with *pMatch == NW_FALSE */
  
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  NW_HTMLP_Interval_Start(&doctypeInterval, iLexer);
  
  e = NW_HTMLP_IfExistsConsumeKeywordCase(&interval,
    NW_HTMLP_String_DoctypeStartLength,
    NW_HTMLP_String_DoctypeStart,
    NW_FALSE,
    pMatch);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  if (*pMatch == NW_TRUE) {
    NW_HTMLP_Interval_Stop(&doctypeInterval, iLexer);
    e = NW_HTMLP_FinishDoctype(pMatch, &doctypeInterval);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (*pMatch == NW_FALSE) {
      e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    }
  return KBrsrSuccess;
  }


TBrowserStatusCode CHtmlpParser::NW_HTMLP_SPL_Elem_Before_HTML_tag(NW_HTMLP_Lexer_t* pL, const NW_HTMLP_Interval_t* pI_content, 
                                                     NW_Bool* onlySplChar)
{
  NW_Uint32 length;
  NW_Uint32 byteLength;
  NW_Uint8* pContent;
  TBrowserStatusCode s;
  NW_Uint32 i =0;

  *onlySplChar = NW_FALSE;

    if (!NW_HTMLP_Interval_IsWellFormed(pI_content)) 
    {
     return KBrsrFailure;
    }

    length = pI_content->stop - pI_content->start;
    byteLength = length;

      s = NW_HTMLP_Lexer_DataAddressFromBuffer(pL, pI_content->start,
                                               &byteLength,
                                               &pContent);
      if (BRSR_STAT_IS_FAILURE(s)) 
      {
        return s;
      }

      if (byteLength != length) 
      {
        return KBrsrFailure;
      }

      for(i=0; i < byteLength; i++)
       {
  
        if(pContent[i] <= 0x20)
        {
            continue;
        }
        else
        {
            break;
        }
       }//end for(..)

      if(i == byteLength )
      {
       *onlySplChar = NW_TRUE;
      }

    return KBrsrSuccess;

}//end CHtmlpParser::NW_HTMLP_SPL_Elem_Before_HTML_tag


TBrowserStatusCode CHtmlpParser::NW_HTMLP_ConsumeContent(NW_Bool* pMatch)
  {
  TBrowserStatusCode e;
  NW_HTMLP_Interval_t interval;
  NW_Bool match, matchspace, hasTable, hasTr, hasTd, hasPre, hasCaption, 
    allSpaces;  // if the comment only contains white spaces
  NW_Int32 i;
  
  *pMatch = NW_FALSE;
  
  hasTable = hasTr = hasTd = hasPre = hasCaption = NW_FALSE;
  allSpaces = NW_TRUE;
  
  if (NW_HTMLP_Lexer_AtEnd(iLexer))
    return KBrsrSuccess;
  /* if just after element start then skip one line break if found */
  if (iElementParseState->readPosition == iLexer->readPosition) {
    /* skip over one instance of line break: 0xd, 0xa, or 0xd+0xa */
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, 0xd, &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    if (NW_HTMLP_Lexer_AtEnd(iLexer)) {
      return KBrsrSuccess;
      }
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, 0xa, &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    if (NW_HTMLP_Lexer_AtEnd(iLexer)) {
      return KBrsrSuccess;
      }
    /* Did we advance over anything? */
    if (iElementParseState->readPosition != iLexer->readPosition) {
      /* check at new beginning for possible start of markup */
      e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '<', &match);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (match == NW_TRUE) {
        return KBrsrSuccess;
        }
      }
    }
  
  for (i = 0;  iIsHtml && (i <= iElementParseState->openElementStackPointer); i++)
    {
    switch ((iElementParseState->pOpenElementStack)[i]) 
      {
      case HTMLP_HTML_TAG_INDEX_TABLE:
        hasTable = NW_TRUE;
        hasTr = hasTd = NW_FALSE;
        break;
      case HTMLP_HTML_TAG_INDEX_TR:
        hasTr = NW_TRUE;
        hasTd = NW_FALSE;
        break;
      case HTMLP_HTML_TAG_INDEX_TD:
      case HTMLP_HTML_TAG_INDEX_TH:
        hasTd = NW_TRUE;
        break;
      case HTMLP_HTML_TAG_INDEX_PRE:
        hasPre = NW_TRUE;
        break;
      case HTMLP_HTML_TAG_INDEX_CAPTION:
        hasCaption = NW_TRUE;
        break;
        
      }
    }
  
  NW_HTMLP_Interval_Start(&interval, iLexer);
  do
    {
    e = NW_HTMLP_Lexer_Advance(iLexer);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    
    match = NW_FALSE;
    if (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
      // temporary hack for "my yahoo" page containing 0 byte in text
      e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, 0x0, &match);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (match)
        {
        e = NW_HTMLP_Lexer_Advance(iLexer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        return KBrsrSuccess;
        }
      /* look for start of markup */
      e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '<', &match);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if (!match)
        {
        e = NW_HTMLP_Lexer_IsSpace(iLexer, &matchspace);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        if (!matchspace)
          allSpaces = NW_FALSE;
        }
      
      }
    if (NW_HTMLP_Lexer_AtEnd(iLexer) || (match == NW_TRUE)) {
      NW_HTMLP_Interval_Stop(&interval, iLexer);
      *pMatch = NW_TRUE;
      /* set the mark (iLastInvlid) in the output so that the start of the invalid dom can be recorded 
      for input buffer, we remember this text (iLastTextBuf) so it can be inserted at
      begining of the next segment */
      if (NW_HTMLP_Lexer_AtEnd(iLexer) && !iDocComplete)
        {
        iLastTextBegin = interval.start;
        if (iLastScriptStart == -1)
          setValidMarks();
        }
      else
        {
        if (hasTable && !hasCaption && (hasPre || !allSpaces))
          {
          if ((!hasTr) && (iCBs->startTagCB))
            {
            e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TR].tag[0],
              &((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TR].tag[1]),
              iCBs->pClientPointer, NW_FALSE);
            updateCurrentCP();
            if (BRSR_STAT_IS_FAILURE(e)) {
              return e;
              }
            }
          if ((!hasTd) && (iCBs->startTagCB))
            {
            e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TD].tag[0],
              &((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TD].tag[1]),
              iCBs->pClientPointer, NW_FALSE);
            updateCurrentCP();
            if (BRSR_STAT_IS_FAILURE(e)) {
              return e;
              }
            }
          }
        /* content callback */
          if (iCBs->contentCB != NULL && (iLastTextBegin == -1))
          {
           /*Handle special case for any content before <HTML> tag. This causes
            *the false page switch so be careful.
            */
           if( (firstSegment == NW_TRUE) && (htmlTagFound == NW_FALSE) )
           {
            NW_Bool   onlySplChar;
            e = NW_HTMLP_SPL_Elem_Before_HTML_tag(iLexer, &interval,&onlySplChar);
            if( (e == KBrsrSuccess) && (onlySplChar == NW_TRUE) )
             {
              return KBrsrSuccess;
             }
           }
           e = (*(iCBs->contentCB))(iLexer, &interval, iCBs->pClientPointer);
           if (BRSR_STAT_IS_FAILURE(e)) {
             return e;
            }
          } /*end if (iCBs->contentCB != NULL && (iLastTextBegin == -1))*/
        }
      
      break;
      }
    } while (!NW_HTMLP_Lexer_AtEnd(iLexer));
    
    if (hasTable && !hasCaption && (hasPre || !allSpaces))
      {
      if ((!hasTd) && (iCBs->endTagCB))
        {
        e = (*(iCBs->endTagCB))((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TD].tag[0], &((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TD].tag[1]), NW_FALSE, /* not empty */
          iCBs->pClientPointer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        }
      if ((!hasTr) && (iCBs->endTagCB))
        {
        e = (*(iCBs->endTagCB))((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TR].tag[0], &((iLexer->pElementDictionary)[HTMLP_HTML_TAG_INDEX_TR].tag[1]), NW_FALSE, /* not empty */
          iCBs->pClientPointer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        }
      }
    
    return KBrsrSuccess;
}

TBrowserStatusCode CHtmlpParser::NW_HTMLP_CheckTagEndOrSpace (NW_Bool* aMatch)
  {
  TBrowserStatusCode e;
  
  e = NW_HTMLP_Lexer_IsSpace(iLexer, aMatch);
  if (BRSR_STAT_IS_FAILURE(e)) 
    {
    return e;
    } 
  if (*aMatch)
    {
    return e;
    }
  
  e = NW_HTMLP_Lexer_AsciiStringCompare (iLexer, 
    NW_HTMLP_String_MiniEndLength,
    NW_HTMLP_String_MiniEnd,
    aMatch);
  if (BRSR_STAT_IS_FAILURE(e)) 
    {
    return e;
    }
  if (*aMatch)
    {
    return e;
    }
  
  e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '>', aMatch);
  return e;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_CheckTagEnd (NW_Bool* aMatch)
  {
  TBrowserStatusCode e;
  
  e = NW_HTMLP_Lexer_AsciiStringCompare (iLexer, 
    NW_HTMLP_String_MiniEndLength,
    NW_HTMLP_String_MiniEnd,
    aMatch);
  if (BRSR_STAT_IS_FAILURE(e)) 
    {
    return e;
    }
  if (*aMatch)
    {
    return e;
    }
  
  e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '>', aMatch);
  return e;
  }

  /*
  on entry: assumes lexer position is just after '='
  on return: If consumed a value, then *pMatch == NW_TRUE and
  .........: lexer read position is just after last value
  .........: related char: if quotes are used then it
  .........: is the character just after the closing quote.
  .........: If no value is consumed then *pMatch == NW_FALSE
  .........: and lexer read position is unchanged.
  .........: In either case return value is KBrsrSuccess.
  .........: When *pMatch == NW_TRUE then the interval bounds the
  .........: value and does not include enclosing quote chars, if used.
  .........: NOTE: Interval may be zero length if value is the empty
  .........: string or value is missing.  To differentiate the two the
  .........: out param *pMissingValue, if true indicates a missing value.
  .........: *pMissingValue is only valid if *pMatch == NW_TRUE.
  eof handling: If encounters EOF while attempting operation, then returns
  ............: *pMatch == NW_FALSE if value is not well-formed with lexer read
  ............: position unchanged, or *pMatch == NW_TRUE if value is well-formed
  ............: with lexer read position at EOF.
  ............: In either case, return value is KBrsrSuccess.
  on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE,
  ...............: *pMissingValue and lexer read position is unspecified
*/

TBrowserStatusCode CHtmlpParser::NW_HTMLP_AttributeValueConsume(NW_Bool* pMissingValue,
                                                                NW_HTMLP_Interval_t* pI,
                                                                NW_Bool* pMatch,NW_Bool splAttrFound)
  {
  /*
  If no quotes are used around the value, then value is delimited by
  whitespace (not including CR/LF) or on encountering either "/>" or '>'.
  This differs from the HTML 4.01 specification which states:
  
    "The attribute value may only contain letters (a-z and A-Z),
    digits (0-9), hyphens (ASCII decimal 45), periods (ASCII decimal 46),
    underscores (ASCII decimal 95), and colons (ASCII decimal 58)."
    
  However, most browsers follow a looser syntax such as the one implemented
  here.  To see this, try browsing this weird doc:
    <a href=foo"" title=a~!@#$%^&*()+={}[];:?<"`'|\/>the link</a>
      
  Otherwise, the value may be any quoted string not containing the
  quotation mark used as a delimiter for that string.
  */
  
  NW_HTMLP_Lexer_Position_t position;
  TBrowserStatusCode e;
  NW_Bool match;
  NW_Bool skippingSpace;
  // added for bug DCAN-5Q9PK8
  // quotedValue is True in the case: attrName='attrVal'
  // quotedValue is False in the case: attrName=attr'xxx'Val
  NW_Bool quotedValue = NW_FALSE; 
  // used only after a Quote is find
  NW_HTMLP_Lexer_Position_t spaceBfEqualQuo;
  // set to TRUE only when NW_HTMLP_Lexer_GetPosition(iLexer, &spaceBfEqualQuo) is called 
  NW_Bool spaceBfEqualQuoSet = NW_FALSE; 
  // match for double quote
  NW_Bool matchD;
  // match for single quote
  NW_Bool matchS;
  // quote could be double " or single '
  NW_Uint8 quote = 0;
  // set to NW_TRUE when value consumed
  NW_Bool valConsumed = NW_FALSE;
  
  *pMatch = NW_FALSE;
  *pMissingValue = NW_TRUE;
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  
  /* optional whitespace */
  e = NW_HTMLP_SkipSpace();
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  
  NW_HTMLP_Interval_Start(pI, iLexer);
  while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
    /* optional CRs or LFs */
    e = NW_HTMLP_SkipCRLF();
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    /* end with match if see '>' */
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '>', &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      NW_HTMLP_Interval_Stop(pI, iLexer);
      *pMatch = NW_TRUE;
      break;
      }
    /* end with match if see "/>" */
    if(splAttrFound != NW_TRUE)
      {
      e = NW_HTMLP_Lexer_AsciiStringCompare(iLexer, NW_HTMLP_String_MiniEndLength,
        NW_HTMLP_String_MiniEnd,
        &match);
      }
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      NW_HTMLP_Interval_Stop(pI, iLexer);
      *pMatch = NW_TRUE;
      break;
      }
    /* end with match if see whitespace */
    e = NW_HTMLP_Lexer_IsSpace(iLexer, &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      NW_HTMLP_Interval_Stop(pI, iLexer);
      *pMatch = NW_TRUE;
      break;
      }
    matchD = NW_FALSE;
    matchS = NW_FALSE;
    // check for string beginning with double quote '\"' 
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '\"', &matchD);
    if (BRSR_STAT_IS_FAILURE(e)) 
      {
      return e;
      }
    // check for string beginning with single quote '\'' 
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '\'', &matchS);
    if (BRSR_STAT_IS_FAILURE(e)) 
      {
      return e;
      }
    if (matchD) 
      {
      match = NW_TRUE;
      quote = '\"';
      }
    else if (matchS) 
      {
      match = NW_TRUE;
      quote = '\'';
      }
    if (match) 
      {
      *pMissingValue = NW_FALSE;
      if (iLexer->readPosition == pI->start)
        {
        quotedValue = NW_TRUE;
        }
      e= NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      // skip leading white space in quoted attribute value
      e = NW_HTMLP_SkipSpace();
      if (BRSR_STAT_IS_FAILURE(e)) 
        {
        return e;
        }
      NW_HTMLP_Interval_Start(pI, iLexer);
      // begin with zero length interval
      NW_HTMLP_Interval_Stop(pI, iLexer);
      skippingSpace = NW_FALSE;
      do 
        {
        /* optional CRs or LFs */
        e = NW_HTMLP_SkipCRLF();
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        if (NW_HTMLP_Lexer_AtEnd(iLexer)) 
          {
          // not well-formed at EOF
          return KBrsrSuccess;
          }
        if (!skippingSpace) 
          {
          NW_HTMLP_Interval_Stop(pI, iLexer);
          }
        e = NW_HTMLP_Lexer_IsSpace(iLexer, &skippingSpace);
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
          }
        //////////////////////////////////////////////////////////////////////////////////
        // update spaceBfEqualQuo once new space is found
        if (skippingSpace)
          {
          NW_HTMLP_Lexer_GetPosition(iLexer, &spaceBfEqualQuo);
          spaceBfEqualQuoSet = NW_TRUE;
          }
        //////////////////////////////////////////////////////////////////////////////////
        // for the case <a href=/url-trx.jsp?title=driver's>Renew driver's license</a>
        // quote is part of real content
        // stop parsing value if seeing > or />
        // however, this is checked only when it's unquoted value
        // in the case <a href='bracket > in quote'>, which is quoted value
        // NW_HTMLP_CheckTagEnd shouldn't be called
        if (!quotedValue)
          {
          e = NW_HTMLP_CheckTagEnd (&match);
          if (BRSR_STAT_IS_FAILURE(e)) 
            {
            return e;
            }
          if (match)
            {
            // value consumed
            valConsumed = NW_TRUE;
            break;
            }
          }
        //////////////////////////////////////////////////////////////////////////////////
        // special handling the case: name1="val1 name2="val2"
        // if seeing =", we can assume that the ending quote for the value is missing
        if (matchD)
          {
          e = NW_HTMLP_Lexer_AsciiStringCompare (iLexer, 
            NW_HTMLP_String_EqualQuoteLength,
            NW_HTMLP_String_EqualDblQuote,
            &match);
          }
        else if (matchS)
          {
          e = NW_HTMLP_Lexer_AsciiStringCompare (iLexer, 
            NW_HTMLP_String_EqualQuoteLength,
            NW_HTMLP_String_EqualSngQuote,
            &match);
          }
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
          }
        if (match) 
          {
          if (spaceBfEqualQuoSet)
            {
   	        // set the lexer to the position of the right most space before ="
            NW_HTMLP_Lexer_SetPosition (iLexer, &spaceBfEqualQuo);
            }
          else
            {
            //////////////////////////////////////////////////////////////////////////////////
            // special handling the case: <a href='b='>b</a>
            // if seeing =' and spaceBfEqualQuo is not set, 
            // we can assume that consumption of value is over
            // important to advance two steps so iLexer will point to '>'
            e = NW_HTMLP_Lexer_Advance(iLexer);
            NW_HTMLP_Interval_Stop(pI, iLexer);
            e = NW_HTMLP_Lexer_Advance(iLexer);
            if (BRSR_STAT_IS_FAILURE(e)) 
              {
              return e;
              }
            }
          // value consumed
          valConsumed = NW_TRUE;
          break;
          }
        //////////////////////////////////////////////////////////////////////////////////
        e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, quote, &match);
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
          }
        e = NW_HTMLP_Lexer_Advance(iLexer);
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
          }
        if (match) // to handle the attrbite="value"" case 
          {
          e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, quote, &match);
          if (BRSR_STAT_IS_FAILURE(e)) 
            {
            return e;
            }
          if (match)
            {
            e = NW_HTMLP_Lexer_Advance(iLexer);
            }
          valConsumed = NW_TRUE;
          }
        //////////////////////////////////////////////////////////////////////////////////
        } while (!valConsumed);
        
        *pMatch = NW_TRUE;
        // two cases would break out the above do-while loop: either quotedValue is true or false
        // true quotedValue means value consumed finished, we should break the outter loop as well
        // speical case: <a href='a.com/name=driver's license'> where iLexer is pointing to s after '
        // this is true quotedValue, we consume value right after driver as IE does so
        // ignore the part of: s license'
        if (quotedValue)
          {
          break;
          }
        
        //////////////////////////////////////////////////////////////////////////////////
        // false quotedValue needs more check before deciding breaking the outter loop or not
        // case: <a href=preQuo'val'aftrQuo>
        // continue to parse the attribute value if check fails
        e = NW_HTMLP_CheckTagEndOrSpace (&match);
        if (BRSR_STAT_IS_FAILURE(e)) 
          {
          return e;
          }
        if (match)
          {
          break;
          }
        else
          {
          // avoid calling NW_HTMLP_Lexer_Advance below, since we have called it
          continue;
          }
        //////////////////////////////////////////////////////////////////////////////////
        } // end of match==NW_TRUE of either ' or "
        
        /* advance in unquoted value */
        *pMissingValue = NW_FALSE;
        e = NW_HTMLP_Lexer_Advance(iLexer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
  } /* end of loop */
  /* catch EOF for values without quotes */
  if (NW_HTMLP_Lexer_AtEnd(iLexer)) {
    *pMatch = NW_TRUE;
    NW_HTMLP_Interval_Stop(pI, iLexer);
    }
  return KBrsrSuccess;
}

/*
on entry: Function parses the name and comapre for "href" tag. If found then
........: *splAttrFound=TRUE otherwise false.
on return: void

  */
  
  void CHtmlpParser::NW_HTMLP_SPL_Elem_Handle_Attr(NW_HTMLP_Interval_t* pI_name, 
    NW_Bool* splAttrFound)
    {
    NW_Uint32 length;
    NW_Uint32 nameLength;
    NW_Uint8* pName;
    NW_Ucs2 c_ucs2;
    NW_Uint32 i;
    NW_Int32 bytesRead;
    TBrowserStatusCode s;
    static const NW_Ucs2 hrefStr[] = {'h','r','e','f','\0'};
    
    *splAttrFound = NW_FALSE;
    
    if (!NW_HTMLP_Interval_IsWellFormed(pI_name)) {
      return;
      }
    
    /* var name setup */
    
    length = NW_HTMLP_Interval_ByteCount(pI_name);
    nameLength = length; /* byte count */
    s = NW_HTMLP_Lexer_DataAddressFromBuffer(iLexer, pI_name->start,
      &nameLength, &pName);
    if (BRSR_STAT_IS_FAILURE(s)) {
      return;
      }
    if (nameLength != length) {
      return;
      }
    
    nameLength = NW_HTMLP_Interval_CharCount(pI_name); /* char count */
    
                                                       /* force attribute name to lower case for A-Z only,
    this alters the doc in-place */
    
    /* FUTURE: unfortunately, there is no writeChar to go with the readChar
    so the work around until there is better encoding support is to only
    work with ASCII, 8859-1, UTF-8 and UCS-2.  In these encodings we can
    handle writing because only UCS-2 uses two bytes for an ASCII char. */
    NW_ASSERT((iLexer->encoding == HTTP_us_ascii)
      || (iLexer->encoding == HTTP_iso_8859_1)
      || (iLexer->encoding == HTTP_utf_8)
      || (iLexer->encoding == HTTP_iso_10646_ucs_2));
    if (!((iLexer->encoding == HTTP_us_ascii)
      || (iLexer->encoding == HTTP_iso_8859_1)
      || (iLexer->encoding == HTTP_utf_8)
      || (iLexer->encoding == HTTP_iso_10646_ucs_2))) {
      return;
      }
    for (i = 0; i < length;) {
      bytesRead = NW_String_readChar(&(pName[i]), &c_ucs2, iLexer->encoding);
      if (bytesRead == -1) {
        return;
        }
      /* force doc ascii uppercase to lowercase */
      if ((c_ucs2 >= (NW_Ucs2)'A') && (c_ucs2 <= (NW_Ucs2)'Z')) {
        c_ucs2 += 0x20; /* offset in ascii from upper to lower */
        }
      if (iLexer->encoding == HTTP_iso_10646_ucs_2) {
        NW_ASSERT(bytesRead == 2);
        /* accomodate either endianness */
        if (pName[i] == 0) {
          pName[i+1] = (NW_Uint8)c_ucs2;
          } else {
          pName[i] = (NW_Uint8)c_ucs2;
            }
        } else {
        NW_ASSERT(bytesRead == 1);
        NW_ASSERT((iLexer->encoding == HTTP_us_ascii)
          || (iLexer->encoding == HTTP_iso_8859_1)
          || (iLexer->encoding == HTTP_utf_8));
        pName[i] = (NW_Uint8)c_ucs2;
          }
        i += bytesRead;
      }//end for(..)
    
    if (NW_Byte_Strnicmp((const NW_Byte*)pName, (const NW_Byte*)hrefStr, NW_Str_Strlen(hrefStr)*sizeof(NW_Ucs2)) == 0)   
      {
      *splAttrFound = NW_TRUE;
      }
    
    return;
    }//end NW_HTMLP_SPL_Elem_Handle_Attr(..)
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_SPL_Elem_Handle_Meta(NW_HTMLP_Interval_t* pI_name, 
    NW_HTMLP_Interval_t* pI_attvalue,
    NW_HTMLP_SPL_Elem_Handling_t* splElem)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_Int32 numUnconvertible, indexFirstUnconvertible;
    NW_Buffer_t* outBuf;
    NW_Bool charsetSpecified = NW_FALSE;
    
    NW_ASSERT(iLexer != NULL);
    NW_ASSERT(pI_name != NULL);
    NW_ASSERT(pI_attvalue != NULL);
    NW_ASSERT(splElem != NULL);
    
    NW_Uint32 i = pI_attvalue->start;
    if (iCBs == NULL)
      return status;
    switch (splElem->type) {
      case NW_HTMLP_SPL_META:
        {
        /* After detecting meta, try to detect content=... */
        static const NW_Ucs2 attrNameStr[] = {'c','o','n','t','e','n','t','\0'};
        static const NW_Ucs2 attrValStr[] = {'c','h','a','r','s','e','t','=','\0'};
        
        /* Assuming latin-1 charset. If it was not latin-1, no need to look for meta charset */
        if (pI_name->charStop - pI_name->charStart == NW_Str_Strlen(attrNameStr) && /* check that the attribute name is content */
          NW_Byte_Strnicmp((const NW_Byte*)(iLexer->pBuf + pI_name->start), (const NW_Byte*)attrNameStr, NW_Str_Strlen(attrNameStr)*sizeof(NW_Ucs2)) == 0)
          {
          while (i <= pI_attvalue->stop - NW_Str_Strlen(attrValStr)*sizeof(NW_Ucs2)) /* Find charset in the attribute value */
            {
            if (NW_Byte_Strnicmp((const NW_Byte*)(iLexer->pBuf + i), (const NW_Byte*)attrValStr, NW_Str_Strlen(attrValStr)*sizeof(NW_Ucs2)) == 0)
              {
              i += NW_Str_Strlen(attrValStr)*sizeof(NW_Ucs2); // Move after charset=
              charsetSpecified = NW_TRUE;
              break;
              }
            i += sizeof(NW_Ucs2);
            }
          }
        // street html support:
        // handle <meta content="text/html" charset="xxxx"> case
        // in this case, attrName = charset and attrVal = xxxx
        else if (pI_name->charStop - pI_name->charStart == NW_Str_Strlen(attrValStr) - 1 && /* check that the attribute name is charset */
          NW_Byte_Strnicmp((const NW_Byte*)(iLexer->pBuf + pI_name->start), (const NW_Byte*)attrValStr, (NW_Str_Strlen(attrValStr) -1)*sizeof(NW_Ucs2)) == 0)
          {
          charsetSpecified = NW_TRUE;
          }
        
        // if we found the structure in meta that indicate charset specification, 
        // call charsetConvertCallback to perform the charset lookup.
        if (charsetSpecified)
          {
          NW_Buffer_t body;
          NW_Uint32 selectedCharset;
          NW_ASSERT(iCBs->charsetConvertCallback != NULL);
          NW_ASSERT(iCBs->charsetContext != NULL);
          
          /* Set the buffer that needs to be converted */
          body.data = iLexer->pBuf;
          body.allocatedLength = iLexer->byteCount;
          body.length = iLexer->byteCount;
          
          /* In case of empty charset declaration */
          if ( pI_attvalue->stop - i == 0)
            {
            status = KBrsrSuccess;
            goto finish_handleMeta;
            }
          status = iCBs->charsetConvertCallback(iCBs->charsetContext, pI_attvalue->stop - i, i, &body, 
            &numUnconvertible, &indexFirstUnconvertible, &outBuf,
            &selectedCharset);
          
          if (status != KBrsrSuccess)
            {
            goto finish_handleMeta;
            }
          /* Was the buffer converted? */
          NW_HTMLP_Lexer_SetEncoding(iLexer, selectedCharset);
          iOrigEncoding = selectedCharset;
          status = KBrsrRestartParsing;
          goto finish_handleMeta;
          } // if (charsetSpecified)
        break;
        } // case NW_HTMLP_SPL_META
      default:
        break;
      } //switch (splElem->type)
finish_handleMeta:
      return status;
    }
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_SPL_Elem_Handle_xml_charset()
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_Int32 numUnconvertible, indexFirstUnconvertible;
    NW_Buffer_t* outBuf = NULL;
    NW_Uint32 i=0,offset =0;
    static const NW_Ucs2 encodingStr[] = {'e','n','c','o','d','i','n','g','=','\0'};
    NW_Uint32 encodingLen = NW_Str_Strlen(encodingStr)*sizeof(NW_Ucs2);
    for(i = iLexer->readPosition + encodingLen - sizeof(NW_Ucs2);(iLexer->pBuf[i] != '"' && iLexer->pBuf[i] != '\'' && iLexer->pBuf[i] != '?' && i < iLexer->byteCount ) ;i+=sizeof(NW_Ucs2))
        {
       if(iLexer->pBuf[i] == ' ')
           offset+= sizeof(NW_Ucs2);
        }
    if (NW_Byte_Strnicmp((const NW_Byte*)(iLexer->pBuf + iLexer->readPosition), (const NW_Byte*)encodingStr, encodingLen - sizeof(NW_Ucs2 )) == 0 && 
      (iLexer->pBuf[iLexer->readPosition + encodingLen] == '"' || 
      iLexer->pBuf[iLexer->readPosition + encodingLen] == '\'' || offset))
      {
      NW_Buffer_t body;
      NW_Uint32 selectedCharset;
      NW_ASSERT(iCBs->charsetConvertCallback != NULL);
      NW_ASSERT(iCBs->charsetContext != NULL);
      for (i = iLexer->readPosition + encodingLen + sizeof(NW_Ucs2) + offset;  
      i < iLexer->byteCount && iLexer->pBuf[i] != '"' && iLexer->byteCount && iLexer->pBuf[i] != '\'' && iLexer->pBuf[i] != '?'; 
      i += sizeof(NW_Ucs2))
        ;
      if (((iLexer->pBuf[i] == '"') || (iLexer->pBuf[i] == '\'')) && i > iLexer->readPosition + encodingLen)
        {  
        encodingLen += sizeof(NW_Ucs2); //Skip the quotes
        /* Set the buffer that needs to be converted */
        body.data = iLexer->pBuf;
        body.allocatedLength = iLexer->byteCount;
        body.length = iLexer->byteCount;
        
        /* In case of empty XML declaration, ignore it */
        if (i - iLexer->readPosition - encodingLen ==0)
          {
          status = KBrsrSuccess;
          goto finish_xml_charset;
          }
        status = iCBs->charsetConvertCallback(iCBs->charsetContext, i - iLexer->readPosition - encodingLen - offset, iLexer->readPosition + encodingLen + offset, &body,
          &numUnconvertible, &indexFirstUnconvertible, &outBuf, &selectedCharset);
        if (status != KBrsrSuccess)
          {
          goto finish_xml_charset;
          }        
        
        NW_HTMLP_Lexer_SetEncoding(iLexer, selectedCharset);
        iOrigEncoding = selectedCharset;
        status = KBrsrRestartParsing;
        goto finish_xml_charset;
        }
      }
finish_xml_charset:
      return status;
    }
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_SPL_Elem_Handle_BodyStart(NW_HTMLP_ElementDescriptionConst_t* pElement)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_Int32 numUnconvertible, indexFirstUnconvertible;
    NW_Buffer_t * outBuf = NULL;
    
    NW_REQUIRED_PARAM( pElement );
    
    NW_ASSERT(iLexer != NULL);
    NW_ASSERT(pElement != NULL);
    
    
    NW_Buffer_t body;
    NW_Uint32 selectedCharset = 0;
    NW_ASSERT(iCBs->charsetConvertCallback != NULL);
    NW_ASSERT(iCBs->charsetContext != NULL);
    
    /* Set the buffer that needs to be converted */
    body.data = iLexer->pBuf;
    body.allocatedLength = iLexer->byteCount;
    body.length = iLexer->byteCount;
    status = iCBs->charsetConvertCallback(iCBs->charsetContext, 0, 0, &body, 
      &numUnconvertible, &indexFirstUnconvertible, &outBuf, &selectedCharset);
    
    if (status != KBrsrSuccess)
      {
      return status;
      }
    /* Was the buffer converted? */
    NW_HTMLP_Lexer_SetEncoding(iLexer, selectedCharset);
    iOrigEncoding = selectedCharset;
    return KBrsrRestartParsing;
    
    }
  
    /*
    on entry: assumes lexer read position is in whitespace or at first character
    ........: of an attribute name
    on return: If consumes an attribute with (optional) value, then *pMatch == NW_TRUE
    .........: and lexer read position is just after the attribute value pair
    .........: which could be whitespace, a '>', or EOF.
    .........: If no attribute with (optional) value consumed, then
    .........: *pMatch == NW_FALSE and lexer read position unchanged.
    .........: In either case return value is KBrsrSuccess.
    .........: When *pMatch == NW_TRUE then the *pI_name bounds the attribute name
    .........: and pI_value bounds the value which does not include enclosing quote
    .........: chars, if used.
    .........: NOTE: *pI_value may be zero length if value is the empty
    .........: string or value is missing.  To differentiate the two the
    .........: out param *pMissingValue, if true indicates a missing value.
    .........: *pMissingValue is only valid if *pMatch == NW_TRUE.
    eof handling: If encounters EOF while attempting operation, then returns
    ............: *pMatch == NW_FALSE only if result up to EOF could not form either
    ............: a name without a value or a value that is not well-formed.
    ............: If *pMatch == NW_FALSE on EOF then lexer read position is unchanged,
    ............: If *pMatch == NW_TRUE on EOF then with lexer read position at EOF.
    ............: In either case, return value is KBrsrSuccess.
    on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE,
    ...............: *pMissingValue and lexer read position is unspecified
  */
  
  TBrowserStatusCode CHtmlpParser::NW_HTMLP_AttributeNameAndValueConsume(
    NW_HTMLP_Interval_t* pI_name,
    NW_Bool* pMissingValue,
    NW_HTMLP_Interval_t* pI_attvalue,
    NW_Bool* pMatch,
    NW_HTMLP_SPL_Elem_Handling_t* splElem)
    {
    /*
    Looks for the following patterns:
    1. name (form with no value)
    2. name= (form with missing value, degenerate but accepted)
    3. name=value (unquoted value)
    4. name='value' (single quoted value)
    5. name="value" (double quoted value)
    
      The forms are terminated by: matching quotes, whitespace, "/>", '>' or EOF.
    */
    NW_HTMLP_Lexer_Position_t position;
    TBrowserStatusCode e;
    NW_Bool match;
    NW_Bool splAttrFound = NW_FALSE;
    
    *pMatch = NW_FALSE;
    *pMissingValue = NW_TRUE;
    NW_HTMLP_Lexer_GetPosition(iLexer, &position);
    
    NW_HTMLP_Interval_Init(pI_name);
    NW_HTMLP_Interval_Init(pI_attvalue);
    
    /* optional whitespace */
    e = NW_HTMLP_SkipSpace();
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    
    /* Name */
    e = NW_HTMLP_ParseName(&match, pI_name);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return KBrsrFailure;
      }
    if (match == NW_FALSE) {
      // added check for quote if attr name is not present, if
      // quote is next, skip to after next quote
      e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '\"', &match);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      // skip to next quote
      if (match== NW_TRUE)
        {   
        match = NW_FALSE;
        do {
          e = NW_HTMLP_Lexer_Advance(iLexer);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          if (NW_HTMLP_Lexer_AtEnd(iLexer)) {
            return KBrsrSuccess;
            }
          e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '\"', &match);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          } while (match == NW_FALSE);
        e = NW_HTMLP_Lexer_Advance(iLexer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          } 
        *pMatch = NW_TRUE;
        return KBrsrSuccess;
        } 
      else
        {
        return KBrsrSuccess;
        }
      }
    
    if(splElem && (splElem->splHandle == NW_TRUE))
      {
      if( (splElem->type == NW_HTMLP_SPL_ANCHOR_ATTR ) ||
          (splElem->type == NW_HTMLP_SPL_BASE_ATTR) )
        {
        NW_HTMLP_SPL_Elem_Handle_Attr(pI_name,&splAttrFound);
        }
      }//endif if(splElem && (splElem->splHandle == NW_TRUE))
    
    
    /* optional whitespace */
    e = NW_HTMLP_SkipSpace();
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    /* '=' */
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '=', &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      e = NW_HTMLP_AttributeValueConsume(pMissingValue, pI_attvalue, &match,splAttrFound);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      if(splElem && (splElem->splHandle == NW_TRUE))
        {
        if( splElem->type == NW_HTMLP_SPL_META )
          {
          e = NW_HTMLP_SPL_Elem_Handle_Meta(pI_name,pI_attvalue, splElem);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          }
        }//endif if(splElem && (splElem->splHandle == NW_TRUE))
      
      if (match == NW_FALSE) {
        return KBrsrSuccess;
        }
      }
    *pMatch = NW_TRUE;
    return KBrsrSuccess;
}

/*
on entry: no assumptions about lexer read position
on return: If finds a match for regular expression "/\s*>",
.........: then *pMatch == NW_TRUE and the lexer read position
.........: is just after the '>' and return value is KBrsrSuccess.
eof handling: If encounters EOF while attempting operation, then
............: *pMatch == NW_FALSE and return value is KBrsrSuccess.
on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
...............: and lexer read position is unspecified
*/
TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfExistsConsumeMinEmptyElementSyntax(NW_Bool* pMatch)
  {
  NW_HTMLP_Lexer_Position_t position;
  NW_Bool match;
  TBrowserStatusCode e;
  
  *pMatch = NW_FALSE;
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '/', &match);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  if (match == NW_TRUE) {
    e = NW_HTMLP_Lexer_Advance(iLexer);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    e = NW_HTMLP_SkipSpace();
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '>', pMatch);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (*pMatch == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      } else {
      e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
        }
    }
  return e;
  }

  /*
  on entry: assumes lexer read position is in whitespace or at first character
  ........: of an attribute name
  ........: This function special cases the callback pointer.  It *iCBs == NULL,
  ........: then no callbacks are done.  This is so that attribute lists on
  ........: unknown tags and on end tags can be consumed but ignored.
  on return: If either the attribute list is well-formed, including an empty
  .........: attribute list, then *pMatch == NW_TRUE and the lexer read position
  .........: is just after the closing '>' or '/>' (for xhtml empty tag processing).
  .........: If the end is '/>', then *pIsEmptyTagEnd == NW_TRUE.
  .........: If no attribute list is consumed, due a malformed attribute
  .........: name or value, then *pMatch == NW_FALSE, *pIsEmptyTagEnd == NW_FALSE
  .........: and the lexer read position is unchanged.
  .........: In either case return value is KBrsrSuccess.
  eof handling: If encounters EOF while attempting operation, then
  ............: *pMatch == NW_FALSE and return value is KBrsrSuccess.
  on error return: return value is not KBrsrSuccess, *pMatch == NW_FALSE
  ...............: and lexer read position is unspecified
*/
TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumeAttributes(
                                                                   NW_Bool* pMatch,
                                                                   NW_Bool* pIsEmptyTagEnd,
                                                                   const struct NW_HTMLP_EventCallbacks_s* pCBs,
                                                                   NW_HTMLP_SPL_Elem_Handling_t* splElem)
  {
  NW_HTMLP_Interval_t I_attName;
  NW_HTMLP_Interval_t I_attValue;
  NW_HTMLP_Lexer_Position_t position;
  NW_Uint32 attributeCount;
  TBrowserStatusCode e;
  NW_Bool missingValue;
  NW_Bool match;
  NW_Uint32 cp_cnt;
  NW_Bool malformated = NW_FALSE;
  
  *pMatch = NW_FALSE;
  *pIsEmptyTagEnd = NW_FALSE;
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  
  for (attributeCount = 0;;) {

    /* optional space and junk. */
    e = NW_HTMLP_SkipJunk();
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }

    /* try '>' end */
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '>', &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      e = NW_HTMLP_Lexer_Advance(iLexer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      *pMatch = NW_TRUE;
      break;
      }
    else if (NW_HTMLP_Lexer_AtEnd(iLexer))
      {
      return KBrsrSuccess;
      }
    /* try /\s*> end */
    e = NW_HTMLP_IfExistsConsumeMinEmptyElementSyntax(&match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_TRUE) {
      *pMatch = NW_TRUE;
      *pIsEmptyTagEnd = NW_TRUE;
      break;
      }
    /* look for attributes */
    e = NW_HTMLP_AttributeNameAndValueConsume(&I_attName, &missingValue,
      &I_attValue, &match, splElem);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    if (match == NW_FALSE) {
      // street html support, if parsing attr nv pair fails and
      // it's not end of buffer yet, that indicates that it's likely
      // to be a mal-formated attr list. In this case, we want to 
      // advance to where the end tag is.
      if (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
        do {
          e = NW_HTMLP_Lexer_Advance(iLexer);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          if (NW_HTMLP_Lexer_AtEnd(iLexer)) {
            return KBrsrSuccess;
            }
          e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '>', &match);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          } while (match == NW_FALSE);
        if (match)
          malformated = NW_TRUE;
        }
      else
        return KBrsrSuccess;
      }
    // if I_AttName byte count is 0, that means we don't
    // have attr name but an attr value within quote.
    if (NW_HTMLP_Interval_ByteCount(&I_attName) !=0 && !malformated) 
      {
      if (!iIsHtml && pCBs != NULL && (pCBs->attributeStartCB != NULL)) { // the content is WML
        e = NW_HTMLP_ValidateWMLAttribute(iLexer, &I_attName,
            pCBs->pClientPointer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          continue;                     // attempt to get the NEXT attribute
        }
      }
      attributeCount++;
      /* note: attribute value may be zero length interval */
      if ((pCBs != NULL) && (pCBs->attributeStartCB != NULL)) {
        e = (*(pCBs->attributeStartCB))(pCBs->pClientPointer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        if (pCBs->attributeNameAndValueCB != NULL) {
          e = (*(pCBs->attributeNameAndValueCB))(iLexer, &I_attName,
            missingValue, &I_attValue,
            pCBs->pClientPointer, &cp_cnt);
          updateCurrentCP(cp_cnt);
          if (BRSR_STAT_IS_FAILURE(e)) {
            return e;
            }
          }
        }
      }
    } /* end of loop */
  if ((attributeCount > 0) && (*pMatch == NW_TRUE)) {
    if ((pCBs != NULL) && (pCBs->attributesEndCB != NULL)) {
      e = (*(pCBs->attributesEndCB))(attributeCount, pCBs->pClientPointer);
      if (BRSR_STAT_IS_FAILURE(e)) {
        return e;
        }
      }
    } 
  return KBrsrSuccess;
}

NW_HTMLP_ElementDescriptionConst_t* CHtmlpParser::NW_HTMLP_ElementFindTagFromDoc(
                                                                                 NW_HTMLP_Interval_t* pI,
                                                                                 NW_HTMLP_ElementTableIndex_t* pIndex)
  {
  NW_Uint8* pTagInDoc;
  NW_Uint32 byteCount;
  NW_Uint32 charCount;
  NW_Int32 bytesRead;
  NW_Uint32 j;
  NW_Int32 tagIndex;
  NW_Ucs2 c_ucs2;
  NW_HTMLP_ElementTableIndex_t i;
  TBrowserStatusCode e;
  NW_Bool match = NW_FALSE;
  
  byteCount = NW_HTMLP_Interval_ByteCount(pI);
  e = NW_HTMLP_Lexer_DataAddressFromBuffer(iLexer, pI->start,
    &byteCount,
    &pTagInDoc);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return NULL;
    }
  NW_ASSERT(byteCount == NW_HTMLP_Interval_ByteCount(pI));
  if (byteCount != NW_HTMLP_Interval_ByteCount(pI)) {
    return NULL;
    }
  charCount = NW_HTMLP_Interval_CharCount(pI);
  for (i = 0; i < iLexer->elementCount; i++) {
    match = NW_FALSE;
    if (charCount == (iLexer->pElementDictionary)[i].tag[0]) {
      match = NW_TRUE;
      tagIndex = 0;
      for (j = 0; j < charCount; j++) {
        bytesRead = NW_String_readChar(&(pTagInDoc[tagIndex]),
          &c_ucs2, iLexer->encoding);
        if (bytesRead == -1) {
          return NULL;
          }
        tagIndex += bytesRead;
        /* force doc ascii uppercase to lowercase */
        if ((c_ucs2 >= (NW_Ucs2)'A') && (c_ucs2 <= (NW_Ucs2)'Z')) {
          c_ucs2 += 0x20; /* offset in ascii from upper to lower */
          }
        if (c_ucs2 != (NW_Ucs2)((iLexer->pElementDictionary)[i].tag[j+1])) {
          match = NW_FALSE;
          break;
          }
        }
      }
    if (match == NW_TRUE) {
      break;
      }
    }
  *pIndex = i;
  if (match == NW_TRUE) {
    return &((iLexer->pElementDictionary)[i]);
    } else {
    return NULL;
      }
  }


TBrowserStatusCode CHtmlpParser::NW_HTMLP_ElementParseState_New()
  {
  iElementParseState = ((NW_HTMLP_ElementParseState_t*)
    NW_Mem_Malloc(sizeof(NW_HTMLP_ElementParseState_t)));
  if (iElementParseState == NULL) {
    return KBrsrOutOfMemory;
    }
  iElementParseState->openElementStackPointer = -1;
  iElementParseState->openElementStackCapacity = NW_HTMLP_OPEN_ELEMENT_INITIAL_STACK_CAPACITY;
  iElementParseState->pOpenElementStack = ((NW_HTMLP_ElementTableIndex_t*)
    NW_Mem_Malloc(sizeof(NW_HTMLP_ElementTableIndex_t)
    * iElementParseState->openElementStackCapacity));
  if (iElementParseState->pOpenElementStack == NULL) {
    NW_Mem_Free(iElementParseState);
    iElementParseState = NULL;
    return KBrsrOutOfMemory;
    }
  iElementParseState->inPCDATA = NW_FALSE;
  iElementParseState->readPosition = 0;
  return KBrsrSuccess;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_ElementParseState_Clone(NW_HTMLP_ElementParseState_t** ppS)
  {
  NW_Int32 sp;
  NW_HTMLP_ElementParseState_Delete(ppS);
  *ppS = ((NW_HTMLP_ElementParseState_t*)
    NW_Mem_Malloc(sizeof(NW_HTMLP_ElementParseState_t)));
  if (*ppS == NULL) {
    return KBrsrOutOfMemory;
    }
  **ppS = *iElementParseState;
  (*ppS)->pOpenElementStack = ((NW_HTMLP_ElementTableIndex_t*)
    NW_Mem_Malloc(sizeof(NW_HTMLP_ElementTableIndex_t)
    * (*ppS)->openElementStackCapacity));
  
  if ((*ppS)->pOpenElementStack == NULL) {
    NW_Mem_Free(*ppS);
    *ppS = NULL;
    return KBrsrOutOfMemory;
    }
  
  for (sp = 0; sp <= iElementParseState->openElementStackPointer; sp++) {
    (*ppS)->pOpenElementStack[sp] = iElementParseState->pOpenElementStack[sp];
    }
  return KBrsrSuccess;
  }

void CHtmlpParser::NW_HTMLP_ElementParseState_Delete(NW_HTMLP_ElementParseState_t** ppS)
  {
  if (*ppS)
    NW_Mem_Free((*ppS)->pOpenElementStack);
  NW_Mem_Free(*ppS);
  *ppS = NULL;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_ElementParseState_Push(NW_HTMLP_ElementTableIndex_t index)
  {
  NW_ASSERT(iElementParseState->openElementStackPointer >= -1);
  NW_ASSERT(iElementParseState->openElementStackPointer < iElementParseState->openElementStackCapacity);
  
  iElementParseState->openElementStackPointer += 1;
  
  if (iElementParseState->openElementStackPointer == iElementParseState->openElementStackCapacity) {
    /* realloc stack */
    NW_Int32 newStackCapacity = (iElementParseState->openElementStackCapacity
      + NW_HTMLP_OPEN_ELEMENT_STACK_GROW_BY);
    NW_HTMLP_ElementTableIndex_t* pNewStack
      = ((NW_HTMLP_ElementTableIndex_t*)
      NW_Mem_Malloc(sizeof(NW_HTMLP_ElementTableIndex_t) * newStackCapacity));
    if (pNewStack == NULL) {
      return KBrsrOutOfMemory;
      }
    NW_Mem_memcpy(pNewStack, iElementParseState->pOpenElementStack, (sizeof(NW_HTMLP_ElementTableIndex_t)
      * iElementParseState->openElementStackCapacity));
    NW_Mem_Free(iElementParseState->pOpenElementStack);
    iElementParseState->pOpenElementStack = pNewStack;
    iElementParseState->openElementStackCapacity = newStackCapacity;
    }
  
  (iElementParseState->pOpenElementStack)[iElementParseState->openElementStackPointer] = index;
  
  return KBrsrSuccess;
  }

NW_HTMLP_ElementTableIndex_t CHtmlpParser::NW_HTMLP_ElementParseState_Pop()
  {
  NW_ASSERT(iElementParseState->openElementStackPointer >= 0);
  NW_ASSERT(iElementParseState->openElementStackPointer < iElementParseState->openElementStackCapacity);
  
  if (iElementParseState->openElementStackPointer >= 0) {
    return (iElementParseState->pOpenElementStack)[iElementParseState->openElementStackPointer--];
    }
  /* BUG: this is a bogus value but something needs to be returned */
  return 0;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_ElementParseState_ImpliedClosings(NW_HTMLP_ElementDescriptionConst_t* pElement)
  {
  const NW_Uint8* pBuf;
  NW_Int32 sp;
  NW_Uint32 i;
  NW_HTMLP_ElementTableIndex_t stackTopElement;
  TBrowserStatusCode e;
  NW_Uint8 l;
  
  if ((pElement == NULL) || (pElement->closes == NULL)) {
    return KBrsrSuccess;
    }
    /* Logic here is that if the close list says close something on the open
    element stack, no matter the position on the open element stack then close
    all elements through the one matched on the open element stack. Repeat
    until there is nothing on the open element stack that matches an item in
  the closes list. However, abort if item on element stack is in blocks list. */
  for (sp = iElementParseState->openElementStackPointer; sp >= 0; sp--) {
    /* check blocks, if any */
    if (pElement->blocks != NULL) {
      for (i = 0; i < pElement->blocks[0]; i++) {
        if (pElement->blocks[i+1] == iElementParseState->pOpenElementStack[sp]) {
          return KBrsrSuccess;
          }
        }
      }
    /* check closes */
    for (i = 0; i < pElement->closes[0]; i++) {
      if (pElement->closes[i+1] == iElementParseState->pOpenElementStack[sp]) {
        while (sp <= iElementParseState->openElementStackPointer) {
          stackTopElement = NW_HTMLP_ElementParseState_Pop();
          if(stackTopElement < NW_HTMLP_DTD_ElementTableMask)
            {
            /* end tag callback (close element) */
            l = (iLexer->pElementDictionary)[stackTopElement].tag[0];
            pBuf = &((iLexer->pElementDictionary)[stackTopElement].tag[1]);
            if (iCBs->endTagCB != NULL) 
              {
              e = (*(iCBs->endTagCB))(l, pBuf, NW_FALSE, /* not empty */
                iCBs->pClientPointer);
              if (BRSR_STAT_IS_FAILURE(e)) 
                {
                return e;
                }
              }
            }//end if(stackTopElement < NW_HTMLP_DTD_ElementTableMask)
          }//end while(..)
        }//end if(..)
      }//end for(..)
    }
  return KBrsrSuccess;
  }

//In this function if(index >= NW_HTMLP_DTD_ElementTableMask) that implies that
//do closing of the Non DTD tag and use parameter "tagInterval" to find the 
//name. So, parameter "index" is used to distinguish between the dictionary and 
//non-dictionary elements. 

TBrowserStatusCode CHtmlpParser::NW_HTMLP_ElementParseState_ExplicitClosing(
                                                                            NW_HTMLP_ElementTableIndex_t index,
                                                                            NW_HTMLP_ElementDescriptionConst_t* pElement)
  {
  const NW_Uint8* pBuf;
  NW_Int32 sp;
  NW_HTMLP_ElementTableIndex_t stackTopElement;
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Uint8 l;
  
  // pElement is NULL case of Non DTD Elements.
  
  if(index < NW_HTMLP_DTD_ElementTableMask)
    {
    if( pElement == NULL)
      {
      return KBrsrSuccess;
      }
    }
  
  /* ignore the html and body close tags since there may be more than one of them */
  if (iIsHtml && (index == HTMLP_HTML_TAG_INDEX_HTML || index == HTMLP_HTML_TAG_INDEX_BODY))
    return KBrsrSuccess;
  
    /* If matching oe on stack at any position, then close all open
    through and including match.  If no matching oe on stack then
  ignore end tag. */
  for (sp = iElementParseState->openElementStackPointer; sp >= 0; sp--) {
    if((iIsHtml && index == HTMLP_HTML_TAG_INDEX_TD) && 
      (iElementParseState->pOpenElementStack[sp] == HTMLP_HTML_TAG_INDEX_TR || 
      iElementParseState->pOpenElementStack[sp] == HTMLP_HTML_TAG_INDEX_TABLE))
      {
      break;
      }
    if((iIsHtml && index == HTMLP_HTML_TAG_INDEX_TR) && 
      (iElementParseState->pOpenElementStack[sp] == HTMLP_HTML_TAG_INDEX_TD || 
      iElementParseState->pOpenElementStack[sp] == HTMLP_HTML_TAG_INDEX_TABLE))
      {
      break;
      }
    if (index == iElementParseState->pOpenElementStack[sp]) {
      while (sp <= iElementParseState->openElementStackPointer) 
        {
        stackTopElement = NW_HTMLP_ElementParseState_Pop();
        
        if(stackTopElement < NW_HTMLP_DTD_ElementTableMask)
          {
          /* end tag callback (close element) */
          l = (iLexer->pElementDictionary)[stackTopElement].tag[0];
          pBuf = &((iLexer->pElementDictionary)[stackTopElement].tag[1]);
          if (iCBs->endTagCB != NULL) {
            e = (*(iCBs->endTagCB))(l, pBuf, NW_FALSE, /* not empty */
              iCBs->pClientPointer);
            if (BRSR_STAT_IS_FAILURE(e)) {
              return e;
              }
            }
          }//end if(stackTopElement < NW_HTMLP_DTD_ElementTableMask)
        }//end while
      /* stop when first match found */
      break;
      }
    }
  return KBrsrSuccess;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_ElementParseState_CloseAll()
  {
  NW_HTMLP_ElementTableIndex_t stackTopElement;
  const NW_Uint8* pBuf;
  NW_Int32 sp;
  TBrowserStatusCode e;
  NW_Uint8 l;
  
  for (sp = iElementParseState->openElementStackPointer; sp >= 0; sp--) 
    {
    stackTopElement = NW_HTMLP_ElementParseState_Pop();
    
    if(stackTopElement < NW_HTMLP_DTD_ElementTableMask)
      {
      
      /* end tag callback (close element) */
      l = (iLexer->pElementDictionary)[stackTopElement].tag[0];
      pBuf = &((iLexer->pElementDictionary)[stackTopElement].tag[1]);
      if (iCBs->endTagCB != NULL) {
        e = (*(iCBs->endTagCB))(l, pBuf, NW_FALSE, /* not empty */
          iCBs->pClientPointer);
        if (BRSR_STAT_IS_FAILURE(e)) {
          return e;
          }
        }
      }
    }//end for
  
  return KBrsrSuccess;
  }

  /*
reset all the special element handling */
void CHtmlpParser::NW_HTMLP_SPL_Elem_Initialize()
  {
  NW_Mem_memset(iSPLElemHandling->tagName,0,128);
  iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
  iSPLElemHandling->splHandle = NW_FALSE;
  }

  /*
  on entry: This function checks if any spcial handling is required for the 
  ........: the elements if so then set the flag for the this and copy the 
  ........: tag name for the latter use.
  ........: 
  ........: 
  on return: void
  
*/
void CHtmlpParser::NW_HTMLP_SPL_Elem_Setup(NW_HTMLP_ElementTableIndex_t elementIndex,
                                           NW_Bool findCharset)
  {
  NW_ASSERT(iSPLElemHandling->type == NW_HTMLP_SPL_NONE);
  NW_ASSERT(iSPLElemHandling->splHandle == NW_FALSE);
  
  iSPLElemHandling->splHandle = NW_TRUE;
  if (iIsHtml)
    {
    switch (elementIndex)
      {
      case HTMLP_HTML_TAG_INDEX_A:
        iSPLElemHandling->type = NW_HTMLP_SPL_ANCHOR_ATTR;
        break;
      case HTMLP_HTML_TAG_INDEX_META:
        if (findCharset)
          iSPLElemHandling->type = NW_HTMLP_SPL_META;
        break;
      case HTMLP_HTML_TAG_INDEX_SCRIPT:
        iSPLElemHandling->type = NW_HTMLP_SPL_SCRIPT;
        break;
      case HTMLP_HTML_TAG_INDEX_NOSCRIPT:
        iSPLElemHandling->type = NW_HTMLP_SPL_NOSCRIPT;
        break;
      case HTMLP_HTML_TAG_INDEX_BODY:
        iSPLElemHandling->type = NW_HTMLP_SPL_BODY;
        break;
      case HTMLP_HTML_TAG_INDEX_TITLE:
        iSPLElemHandling->type = NW_HTMLP_SPL_TITLE;
        break;
      case HTMLP_HTML_TAG_INDEX_BASE:
        iSPLElemHandling->type = NW_HTMLP_SPL_BASE_ATTR;
        break;
      default:
        iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
      }
    }
  else
    {
    switch (elementIndex)
      {
      case HTMLP_WML_TAG_INDEX_A:
        iSPLElemHandling->type = NW_HTMLP_SPL_ANCHOR_ATTR;
        break;
      case HTMLP_WML_TAG_INDEX_META:
        if (findCharset)
          iSPLElemHandling->type = NW_HTMLP_SPL_META;
        break;
      default:
        iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
      }
    }
  }//end NW_HTMLP_SPL_Elem_Setup()

   
/*
 * Function to handle the special case after <br> element. If the lot of 
 * spaces are there in the document after the <br> then this gives 
 * the false code page switch. 
 */

TBrowserStatusCode CHtmlpParser::NW_HTMLP_BR_SPL_Handle()
{
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Bool match = NW_TRUE;
  NW_Bool matchSpl = NW_TRUE;
  
  while(match)
  {
   e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, 0x0, &matchSpl);
   if (BRSR_STAT_IS_FAILURE(e)) 
   {
     return e;
   }
   if (matchSpl == NW_FALSE) 
    {
     // check for '0xa' for "nl"
     e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, 0xa, &matchSpl);
     if (BRSR_STAT_IS_FAILURE(e)) 
      {
       return e;
      }
     }

    if( (matchSpl == NW_TRUE) && !NW_HTMLP_Lexer_AtEnd(iLexer) )
    {
     e = NW_HTMLP_Lexer_Advance(iLexer);   
     if (BRSR_STAT_IS_FAILURE(e)) 
     {
      return e;
     }
    }
    else
    {
     match = NW_FALSE;
    }
  }//end while

  return e;

}//end CHtmlpParser::NW_HTMLP_BR_SPL_Handle()
   
   
   TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumeElementStart(NW_Bool* pMatch)
     {
     TBrowserStatusCode e = KBrsrSuccess;
     NW_HTMLP_Interval_t interval;
     NW_Bool match;
     NW_Bool emptyElement;
     NW_HTMLP_Lexer_Position_t position;
     NW_HTMLP_Lexer_Position_t startPosition;
     NW_HTMLP_Lexer_Position_t positionAttributeList;
     NW_HTMLP_ElementDescriptionConst_t* pElement;
     NW_HTMLP_ElementTableIndex_t elementIndex;
     NW_HTMLP_ElementTableIndex_t insertedIndex = 0;
     NW_Bool inserted = NW_FALSE;
     NW_Bool hasTable, hasTr, hasTd, needTable, needTr, needTd;
     NW_Int32 i;
     
     *pMatch = NW_FALSE;
     hasTable = hasTr = hasTd = needTr = needTd = needTable =NW_FALSE;
     
     NW_HTMLP_SPL_Elem_Initialize();
     NW_HTMLP_Interval_Init(&interval);
     NW_HTMLP_Lexer_GetPosition(iLexer, &position);
     NW_HTMLP_Lexer_GetPosition(iLexer, &startPosition);
     /* advance past '<' */
     e = NW_HTMLP_Lexer_Advance(iLexer);
     if (BRSR_STAT_IS_FAILURE(e)) {
       return e;
       }
     /* get tag */
     e = NW_HTMLP_ParseName(&match, &interval);
     if (BRSR_STAT_IS_FAILURE(e)) {
       return e;
       }
     if (match == NW_FALSE) {
       // This isn't a start tag of an element.
       e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '%', &match);
       if (BRSR_STAT_IS_FAILURE(e)) 
         {
         return e;
         }
       if (match == NW_TRUE) 
         {
         // if page contains <html><%response.cachecontrol=public%><head>...
         // ignore <%response.cachecontrol=public%>
         // for more detail, see JHAN-5XSR7Y
         // if tag starts with <%, then ignore it by forcing match to NW_TRUE,
         // and setting iLexer position to where <% ends
         do 
           {
           e = NW_HTMLP_Lexer_Advance(iLexer);
           if (BRSR_STAT_IS_FAILURE(e)) 
             {
             return e;
             }
           if (NW_HTMLP_Lexer_AtEnd(iLexer)) 
             {
             /* not well-formed at EOF */
             match = NW_TRUE;
             return KBrsrSuccess;
             }
           e = NW_HTMLP_Lexer_AsciiCharCompare (iLexer, '>', &match);
           if (BRSR_STAT_IS_FAILURE(e)) 
             {
             return e;
             }
           } while (!match);
           NW_HTMLP_Lexer_Advance(iLexer);
         }
       else
         {
         // no <% found
         e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         }
       return KBrsrSuccess;
       }
     
     // look up the tag in our table and convert from using an interval to
     // a table entry
     pElement = NW_HTMLP_ElementFindTagFromDoc(&interval, &elementIndex);

     // If tag is unknown, consume it, ignoring it as though it wasn't
     // there. Only the tag is consumed. Everything between it and its
     // closing tag are kept and processed as if the tag weren't there
     // in the first place.
     if (!pElement)
       {
       match = NW_FALSE;
       do 
         {
         if (NW_HTMLP_Lexer_AtEnd(iLexer)) 
           {
           // This could be illegal tag or end of chunk in the middle of a tag
           if (!iDocComplete)
               {
                TInt lastTextLen = iLexer->byteCount - startPosition.readPosition;
    
                iLastTextBuf = (NW_Buffer_t *)NW_Buffer_New(lastTextLen);
                if (iLastTextBuf == NULL) {
                  return KBrsrOutOfMemory;
                  }
                else
                  {
                  (void)NW_Mem_memcpy(iLastTextBuf->data, iLexer->pBuf + startPosition.readPosition, lastTextLen);
                  iLastTextBuf->length = lastTextLen;
                  }
               }
           match = NW_TRUE;
           return KBrsrSuccess;
           }
         e = NW_HTMLP_Lexer_AsciiCharCompare (iLexer, '>', &match);
         if (BRSR_STAT_IS_FAILURE(e)) 
           {
           return e;
       }
         e = NW_HTMLP_Lexer_Advance(iLexer);
         if (BRSR_STAT_IS_FAILURE(e)) 
       {
           return e;
           }
         } while (!match);
         *pMatch = NW_TRUE;
         return KBrsrSuccess;
       }
     

       if (iIsHtml && elementIndex == HTMLP_HTML_TAG_INDEX_NOSCRIPT)
         {
         if (iWithinNoscript)
           {
           // already within the noscript, treat this noscript as </noscript>
           e = NW_HTMLP_IfLegalConsumeAttributes(&match, &emptyElement, NULL, NULL);
           if (BRSR_STAT_IS_FAILURE(e)) {
             return e;
             }
           if (match == NW_FALSE) 
             {
             /* This is a malformed attribute list. */
             e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
             if (BRSR_STAT_IS_FAILURE(e)) 
               {
               return e;
               }
             *pMatch = NW_FALSE;
             return KBrsrSuccess;
             }
           
           e = NW_HTMLP_ElementParseState_ExplicitClosing(elementIndex, pElement);
           if (BRSR_STAT_IS_FAILURE(e)) 
             {
             return e;
             }
           iWithinNoscript = NW_FALSE;
           *pMatch = NW_TRUE;
           iElementParseState->readPosition = iLexer->readPosition;
           return KBrsrSuccess;
           }
         else
           {
           // set within noscript to be true
           iWithinNoscript = NW_TRUE; 
           }
         }
     
    /*  Some contents are there before the <html> tags. Flag to handle these contents. Special case
     *  for handling the page switch. This is checked in the first segment only.
     */

       if( iIsHtml && firstSegment && iCBs->startTagCB && (elementIndex == HTMLP_HTML_TAG_INDEX_HTML) )
       {
        htmlTagFound = NW_TRUE;
       }

     //If some special handling of the element is required.
     if(pElement && (pElement->splHandling == NW_TRUE) )
       {
       NW_HTMLP_SPL_Elem_Setup(elementIndex, iNeedCharsetDetect);
       
       if (iSPLElemHandling->type == NW_HTMLP_SPL_SCRIPT)
         {
         iLastScriptStart = position.readPosition;
         setValidMarks();
         } 
       else if(iSPLElemHandling->type == NW_HTMLP_SPL_NOSCRIPT )
         {
         iLastScriptStart = position.readPosition;
         setValidMarks();
         }
       else if (iSPLElemHandling->type == NW_HTMLP_SPL_BODY)
         {
         if (iNeedCharsetDetect)
           {
           e = NW_HTMLP_SPL_Elem_Handle_BodyStart(pElement);
           return e;
           }
         }
       }
     
       /*
       now have a tag but must check attribute list before executing callbacks
     */
     
     /* first pass over attribute list just checks syntax and is
     done without callbacks */
     NW_HTMLP_Lexer_GetPosition(iLexer, &positionAttributeList);
     e = NW_HTMLP_IfLegalConsumeAttributes(&match, &emptyElement, NULL, iSPLElemHandling);
     if (BRSR_STAT_IS_FAILURE(e)) {
       return e;
       }
     if (match == NW_FALSE) {
       /* This is a malformed attribute list. */
       e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
       if (BRSR_STAT_IS_FAILURE(e)) {
         return e;
         }
       return KBrsrSuccess;
       }
     
     /* do implied closing of open elements */
     e = NW_HTMLP_ElementParseState_ImpliedClosings(pElement);
     if (BRSR_STAT_IS_FAILURE(e)) {
       return e;
       }
     /* tag CB */
     if (iCBs->startTagCB != NULL) 
       {
       // first handle the case of insert necessary tr or td
       if (iIsHtml)
         {
         for (i = 0; i <= iElementParseState->openElementStackPointer; i++)
           {
           switch ((iElementParseState->pOpenElementStack)[i]) 
             {
             case HTMLP_HTML_TAG_INDEX_TABLE:
               hasTable = NW_TRUE;
               hasTr = hasTd = NW_FALSE;
               break;
             case HTMLP_HTML_TAG_INDEX_TR:
               hasTr = NW_TRUE;
               hasTd = NW_FALSE;
               break;
             case HTMLP_HTML_TAG_INDEX_TD:
             case HTMLP_HTML_TAG_INDEX_TH:
               hasTd = NW_TRUE;
               break;
             }
           }  
         }
       else
         {  // !iIsHtml  == WML content
         // if content is WML make sure <table>, <tr> and <td> are present or needed.
         // if one or more is needed, they will be added to stack (openElementStack), below.
         for (i = 0; i <= iElementParseState->openElementStackPointer; i++)
           {
           switch ((iElementParseState->pOpenElementStack)[i]) 
             {
             case HTMLP_WML_TAG_INDEX_TABLE:
               hasTable = NW_TRUE;
               hasTr = hasTd = NW_FALSE;
               break;
             case HTMLP_WML_TAG_INDEX_TR:
               hasTr = NW_TRUE;
               hasTd = NW_FALSE;
               break;
             case HTMLP_WML_TAG_INDEX_TD:
               hasTd = NW_TRUE;
               break;
             }
           } 
         }
       if (hasTable)
         {
         if ( (iIsHtml && (elementIndex == HTMLP_HTML_TAG_INDEX_TD || elementIndex == HTMLP_HTML_TAG_INDEX_TH))
           || (!iIsHtml && (elementIndex == HTMLP_WML_TAG_INDEX_TD)) )
           { 
           if (!hasTr)
             needTr = NW_TRUE;
           }
           /* try to use the form direct under the table, because it's very comman and table
         do handles it */
         else if ( iIsHtml && (elementIndex != HTMLP_HTML_TAG_INDEX_TR
             && elementIndex != HTMLP_HTML_TAG_INDEX_FORM
             && elementIndex != HTMLP_HTML_TAG_INDEX_CAPTION) )
           {
           if (!hasTr)
             needTr = NW_TRUE;
           if (!hasTd)
             needTd = NW_TRUE;
           }
         }
       else 
         {
         if ( (iIsHtml && elementIndex == HTMLP_HTML_TAG_INDEX_TD)
           || (!iIsHtml && elementIndex == HTMLP_WML_TAG_INDEX_TD) )
           {
           needTable = NW_TRUE;
           
           if (!hasTr)
             needTr = NW_TRUE;
           }
         else if ( (iIsHtml && elementIndex == HTMLP_HTML_TAG_INDEX_TR)
               || (!iIsHtml && elementIndex == HTMLP_WML_TAG_INDEX_TR) )
           needTable = NW_TRUE;
         }
       if (needTable)
         {
         TUint16 temp = HTMLP_HTML_TAG_INDEX_TABLE;
         if (!iIsHtml)  {
           temp = HTMLP_WML_TAG_INDEX_TABLE;
           }
         e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[temp].tag[0],
           &((iLexer->pElementDictionary)[temp].tag[1]),
           iCBs->pClientPointer, NW_FALSE);
         updateCurrentCP();
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         e = NW_HTMLP_ElementParseState_Push(temp);
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         }
       if (needTr)
         {
         TUint16 temp = HTMLP_HTML_TAG_INDEX_TR;
         if (!iIsHtml)  {
           temp = HTMLP_WML_TAG_INDEX_TR;
           }
         e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[temp].tag[0],
           &((iLexer->pElementDictionary)[temp].tag[1]),
           iCBs->pClientPointer, NW_FALSE);
         updateCurrentCP();
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         e = NW_HTMLP_ElementParseState_Push(temp);
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         }
       if (needTd)
         {
         TUint16 temp = HTMLP_HTML_TAG_INDEX_TD;
         if (!iIsHtml)  {
           temp = HTMLP_WML_TAG_INDEX_TD;
           }
         e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[temp].tag[0],
           &((iLexer->pElementDictionary)[temp].tag[1]),
           iCBs->pClientPointer, NW_FALSE);
         updateCurrentCP();
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           
           }
         e = NW_HTMLP_ElementParseState_Push(temp);
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         }
       
       if (iIsHtml)
         {
         switch (elementIndex)
           {
           case HTMLP_HTML_TAG_INDEX_FORM:
             if (!iHasForm)
               {
               iHasForm = NW_TRUE;
               }
             break;
           case HTMLP_HTML_TAG_INDEX_INPUT:
           case HTMLP_HTML_TAG_INDEX_SELECT:
           case HTMLP_HTML_TAG_INDEX_TEXTAREA:
           case HTMLP_HTML_TAG_INDEX_LABEL:
           case HTMLP_HTML_TAG_INDEX_FIELDSET:
             // if the content is from script, no need to create fake form element
             // this is due to some live web site page (such as mlb.com) where the
             // input controls are in script, but form is outside the script.
             if (!iHasForm && !iIsScript)  
               {
               inserted = NW_TRUE;
               insertedIndex = HTMLP_HTML_TAG_INDEX_FORM;
               e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[insertedIndex].tag[0],
                 &((iLexer->pElementDictionary)[insertedIndex].tag[1]),
                 iCBs->pClientPointer, NW_FALSE);
               updateCurrentCP();
               if (BRSR_STAT_IS_FAILURE(e)) {
                 return e;
                 }
               iHasForm = NW_TRUE;
               }
           }
         }
       
       if (pElement)
         {
         e = (*(iCBs->startTagCB))((iLexer->pElementDictionary)[elementIndex].tag[0],
           &((iLexer->pElementDictionary)[elementIndex].tag[1]),
           iCBs->pClientPointer, NW_FALSE);
         updateCurrentCP();
         if (BRSR_STAT_IS_FAILURE(e)) {
           return e;
           }
         }
     }
   
   /* reposition for a second pass over attribute list */
   e = NW_HTMLP_Lexer_SetPosition(iLexer, &positionAttributeList);
   if (BRSR_STAT_IS_FAILURE(e)) {
     return e;
     }
   /* second pass over attribute list with callbacks */
   e = NW_HTMLP_IfLegalConsumeAttributes(&match, &emptyElement, iCBs, iSPLElemHandling);
   if (BRSR_STAT_IS_FAILURE(e)) {
     return e;
     }
   if (match == NW_FALSE) {
   /* Something is unexpectedly wrong.  We just did a second
   pass over the attribute list (this time with callbacks enabled) and
   it should always match because the first pass results check should
     have caught a syntax error */
     return KBrsrFailure;
     }
   
   /* accept this opening markup */
   
   if (inserted)
     {
     e = NW_HTMLP_ElementParseState_Push( insertedIndex );
     if (BRSR_STAT_IS_FAILURE(e)) {
       return e;
       }
     }  
   
     /* Sometimes  spaces (0x00) after <br> tag gives false switch 
      *  code page in the wbxml buffer. 
      */

   if(pElement && (elementIndex == HTMLP_HTML_TAG_INDEX_BR) && (iIsHtml == NW_TRUE) )
   {
       e = NW_HTMLP_BR_SPL_Handle();
       if (BRSR_STAT_IS_FAILURE(e))
       {
        return e;
       }
   }

   if (pElement && (pElement->contentType != EMPTY) && (emptyElement != NW_TRUE))
     {
     e = NW_HTMLP_ElementParseState_Push(elementIndex);
     if (BRSR_STAT_IS_FAILURE(e)) 
       {
       return e;
       }
     if (pElement->contentType == PCDATA) 
       {
       iElementParseState->inPCDATA = NW_TRUE;
       }
     }
   
   if (pElement && (pElement->contentType == EMPTY) || (emptyElement == NW_TRUE) )
     {
     /* This takes care of Empty DTD element Tags closing */
     if (iCBs->endTagCB != NULL) 
       {
       e = (*(iCBs->endTagCB))(0, NULL, NW_TRUE, /* empty */
         iCBs->pClientPointer);
       if (BRSR_STAT_IS_FAILURE(e))
         {
         return e;
         }
       }
     }
   /* <plaintext> handler */
   if (pElement && (pElement->contentType == PLAINTEXT ) ) 
     {
     NW_HTMLP_Interval_Start(&interval, iLexer);
     while (!NW_HTMLP_Lexer_AtEnd(iLexer)) 
       {
       e = NW_HTMLP_Lexer_Advance(iLexer);
       if (BRSR_STAT_IS_FAILURE(e)) 
         {
         return e;
         }
       }
     
     NW_HTMLP_Interval_Stop(&interval, iLexer);
     if (iCBs->contentCB != NULL) 
       {
       e = (*(iCBs->contentCB))(iLexer, &interval, iCBs->pClientPointer);
       if (BRSR_STAT_IS_FAILURE(e))
         {
         return e;
         }
       }
     }
   *pMatch = NW_TRUE;
   iElementParseState->readPosition = iLexer->readPosition;
   return KBrsrSuccess;
}//end CHtmlpParser::NW_HTMLP_IfLegalConsumeElementStart(NW_Bool* pMatch)

TBrowserStatusCode CHtmlpParser::NW_HTMLP_IfLegalConsumeElementEnd(NW_Bool* pMatch)
  {
  NW_HTMLP_Interval_t interval;
  NW_HTMLP_Lexer_Position_t position;
  NW_HTMLP_ElementDescriptionConst_t* pElement;
  NW_HTMLP_ElementTableIndex_t elementIndex;
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Bool match;
  NW_Bool emptyElement;
  
  *pMatch = NW_FALSE;
  NW_HTMLP_Interval_Init(&interval);
  NW_HTMLP_Lexer_GetPosition(iLexer, &position);
  /* advance past '<' */
  e = NW_HTMLP_Lexer_Advance(iLexer);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  /* check for '/' */
  e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '/', &match);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }

  if (match == NW_FALSE) {
    /* This isn't an end tag of an element. */
    e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
    }
    return KBrsrSuccess;
  }

  /* advance past '/' */
  e = NW_HTMLP_Lexer_Advance(iLexer);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  /* get tag */
  e = NW_HTMLP_ParseName(&match, &interval);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  if (match == NW_FALSE) {
    /* This isn't a start tag of an element. */
    e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    return KBrsrSuccess;
    }
  
    /* look up the tag in our table and convert from using
  an interval to a table entry */
  pElement = NW_HTMLP_ElementFindTagFromDoc(&interval, &elementIndex);
  
  /* If end of head, and charset was not detected, need to default to variant specific,
  and do the conversion now */
  if(pElement && (pElement->splHandling == NW_TRUE) )
    {
    if (iIsHtml)
      {
      if( elementIndex == HTMLP_HTML_TAG_INDEX_SCRIPT)
        {
        iLastScriptStart = -1;
        } 
      else if(elementIndex == HTMLP_HTML_TAG_INDEX_NOSCRIPT)
        {
        iLastScriptStart = -1;
        iWithinNoscript = NW_FALSE;
        }
      }
    }
  
  
  /* like normal attribute list parse but don't use callbacks */
  e = NW_HTMLP_IfLegalConsumeAttributes(&match, &emptyElement, NULL, NULL);
  if (BRSR_STAT_IS_FAILURE(e)) {
    return e;
    }
  if (match == NW_FALSE) {
    /* This is a malformed attribute list. */
    e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    return KBrsrSuccess;
    }
  /* Catch PCDATA mode */
  if (iElementParseState->inPCDATA == NW_TRUE) 
    {
    NW_ASSERT(iElementParseState->openElementStackPointer >= 0);
    NW_ASSERT(iElementParseState->openElementStackPointer < iElementParseState->openElementStackCapacity);
    if ((pElement != NULL)
      && (elementIndex==(iElementParseState->pOpenElementStack)[iElementParseState->openElementStackPointer]))
      {
      iElementParseState->inPCDATA = NW_FALSE;
      if( (iSPLElemHandling->splHandle == NW_TRUE) && 
        (iSPLElemHandling->type == NW_HTMLP_SPL_TITLE) )
        {
        iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
        }
      } 
    else if( (iSPLElemHandling->splHandle == NW_TRUE) && 
      (iSPLElemHandling->type == NW_HTMLP_SPL_TITLE) )
      {
      /*Handle mis-spelled </title> end tag */
      iElementParseState->inPCDATA = NW_FALSE;
      iSPLElemHandling->type = NW_HTMLP_SPL_NONE;
      }
    else 
      {
      /* reject this ending markup as not matching open element */
      e = NW_HTMLP_Lexer_SetPosition(iLexer, &position);
      if (BRSR_STAT_IS_FAILURE(e))
        {
        return e;
        }
      return KBrsrSuccess;
      }
    } //end if (iElementParseState->inPCDATA == NW_TRUE) 
  
  /* unknown tag filter */
  if (pElement != NULL) {
    e = NW_HTMLP_ElementParseState_ExplicitClosing(elementIndex, pElement);
    if (BRSR_STAT_IS_FAILURE(e)) {
      return e;
      }
    }
  
  *pMatch = NW_TRUE;
  return KBrsrSuccess;
}

TBrowserStatusCode CHtmlpParser::setValidMarks()
  {
  NW_HTMLP_ElementParseState_Clone(&iLastValidStack);
  if (iWbxmlWriter)
    {
    iLastValid = ((NW_HTMLP_WbxmlEncoder_t*)iCBs->pClientPointer)->pE->index;
    iValidTagCP = iCurrentTagCP;
    iValidAttrCP = iCurrentAttrCP;
    iValidCPCount = iCurrentCPCount;
    }
  return KBrsrSuccess;
  }

TBrowserStatusCode CHtmlpParser::commitValidMarks()
  {
  NW_Buffer_t * tmp_buf = NULL;
  NW_Uint32 lastTextLen = 0;
  NW_Uint32 previousBufLen = 0;
  
  /* trim the output if necessary */
  if (iWbxmlWriter && iWbxmlWriter->index != (NW_Uint32)iLastValid)
    iWbxmlWriter->index = iLastValid;
  iElementParseState->openElementStackPointer = iLastValidStack->openElementStackPointer;
  iElementParseState->readPosition = iLastValidStack->readPosition;
  
  /* reform the valid output */
  if (iPreviousValidOutput)
    iLastValid += iPreviousValidOutput->length;
  
  // only generate output wbxml buffer when real parsing starts 
  if (!iNeedCharsetDetect)
    {
    tmp_buf = iPreviousValidOutput;
    iPreviousValidOutput = NW_Buffer_New(iLastValid);
    if (!iPreviousValidOutput)
      {
      return KBrsrOutOfMemory;
      }
    if (tmp_buf)
      {
      previousBufLen = tmp_buf->length;
      (void)NW_Mem_memcpy(iPreviousValidOutput->data, tmp_buf->data, previousBufLen);
      }
    (void)NW_Mem_memcpy(iPreviousValidOutput->data + previousBufLen, 
      iWbxmlWriter->pBuf, 
      iLastValid - previousBufLen);
    iPreviousValidOutput->length = iLastValid;
    
    if (tmp_buf)
      NW_Buffer_Free(tmp_buf);
    }
  
  /* record the last text kept from the chunk */
  if (iLastScriptStart != -1)
    {
    iLastTextBegin = iLastScriptStart;
    }
  
  if (iLastTextBegin != -1)
    {
    lastTextLen = iLexer->byteCount - iLastTextBegin;
    
    iLastTextBuf = (NW_Buffer_t *)NW_Buffer_New(lastTextLen);
    if (iLastTextBuf == NULL) {
      return KBrsrOutOfMemory;
      }
    else
      {
      (void)NW_Mem_memcpy(iLastTextBuf->data, iLexer->pBuf + iLastTextBegin, lastTextLen);
      iLastTextBuf->length = lastTextLen;
      }
    }
  return KBrsrSuccess;
  }

NW_Buffer_t* CHtmlpParser::getLastTextBuf()
  {
  return iLastTextBuf;
  }

NW_Int32 CHtmlpParser::getLastValid()
  {
  return iLastValid;
  }

NW_Uint32 CHtmlpParser::getCodePageSwitchCount()
  {
  return iValidCPCount;
  }

NW_Buffer_t* CHtmlpParser::getPreviousValidOutput()
  {
  return iPreviousValidOutput;
  }

NW_HTMLP_EventCallbacks_t * CHtmlpParser::getEventCallbacks()
  {
  return iCBs;
  }

NW_HTMLP_Lexer_t* CHtmlpParser::getLexer()
  {
  return iLexer;
  }

TBrowserStatusCode CHtmlpParser::NW_HTMLP_Parse(NW_Bool isFirstSegment, 
    NW_Bool docComplete, NW_Bool needCharsetDetect)
  {
  TBrowserStatusCode e = KBrsrSuccess;
  NW_Bool match = NW_FALSE;
  
  iDocComplete = docComplete;
  iNeedCharsetDetect = needCharsetDetect;
  if (iNeedCharsetDetect)
    {
    NW_ASSERT(iCBs->charsetConvertCallback);
    NW_ASSERT(iCBs->charsetContext);
    }
  if (isFirstSegment)
    {
    // initialize iIsHtml
    iIsHtml = NW_FALSE;
    iHasForm = NW_FALSE;
    firstSegment = NW_TRUE;
    htmlTagFound = NW_FALSE;
    if (iLexer->pElementDictionary == NW_HTMLP_Get_ElementDescriptionTable())
      {
      iIsHtml = NW_TRUE;
      }
    if (iLastValidStack)
      {
      NW_HTMLP_ElementParseState_Delete(&iLastValidStack);
      NW_HTMLP_ElementParseState_New();
      }
    }
  else
  {
   firstSegment = NW_FALSE;
   htmlTagFound = NW_FALSE;
  }
  
  if ((iLexer == NULL) || (iCBs == NULL)) {
    return KBrsrFailure;
    }
  
  iSPLElemHandling->splHandle = NW_FALSE;
  iSPLElemHandling->type      = NW_HTMLP_SPL_NONE;
  
  NW_Buffer_Free(iLastTextBuf);
  iLastTextBuf = NULL;
  
  if (iCBs->pClientPointer != NULL)
    {
    iWbxmlWriter = ((NW_HTMLP_WbxmlEncoder_t*)iCBs->pClientPointer)->pE;
    iWbxmlWriter->tagCodePage = iValidTagCP;
    iWbxmlWriter->attributeCodePage = iValidAttrCP;
    // WLIU_DEBUG: iWbxmlWriter->cp_count = iValidCPCount;
    iCurrentTagCP = iValidTagCP;
    iCurrentAttrCP = iValidAttrCP;
    iCurrentCPCount = iValidCPCount;
    }
  
  if (iLastValidStack)
    {
    iElementParseState = iLastValidStack;
    iLastValidStack = NULL;
    }
  
  iLastTextBegin = -1;
  iLastScriptStart = -1;
  
  /* start of document callback */ 
  if (isFirstSegment && iCBs->beginDocumentCB != NULL) {
    e = (*(iCBs->beginDocumentCB))(iLexer, iCBs->pClientPointer);
    if (BRSR_STAT_IS_FAILURE(e)) {
      goto htmlp_parse_error;
      }
    }
  /* ignore any leading whitespace */
  e = NW_HTMLP_SkipSpace();
  if (BRSR_STAT_IS_FAILURE(e)) {
    goto htmlp_parse_error;
    }
  
  while (!NW_HTMLP_Lexer_AtEnd(iLexer)) {
    
    if (iConsumeSpaces)
      {
      /* Consume spaces between tags only for WML text */
      e = NW_HTMLP_SkipSpace();
      }
    
    /* Take care of special cases */
    
    e = NW_HTMLP_HandleSpecialCases(&match);
    
    if (BRSR_STAT_IS_FAILURE(e)) {
      goto htmlp_parse_error;
      }
    if (match == NW_TRUE) {
      continue;
      }
    
    /* look for start of markup */
    e = NW_HTMLP_Lexer_AsciiCharCompare(iLexer, '<', &match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      goto htmlp_parse_error;
      }
    if (match == NW_TRUE) {
      
      e = NW_HTMLP_IfLegalConsumeElementEnd(&match);
      if (BRSR_STAT_IS_FAILURE(e)) {
        goto htmlp_parse_error;
        }
      if (match == NW_TRUE) {
        continue;
        }
      
      if (iElementParseState->inPCDATA == NW_FALSE)
        {
        e = NW_HTMLP_IfLegalConsumeComment(&match);
        if (BRSR_STAT_IS_FAILURE(e)) {
          goto htmlp_parse_error;
          }
        if (match == NW_TRUE) {
          continue;
          }
        
        e = NW_HTMLP_IfLegalConsumeDoctype(&match);
        if (BRSR_STAT_IS_FAILURE(e)) {
          goto htmlp_parse_error;
          }
        if (match == NW_TRUE) {
          continue;
          }
        
        e = NW_HTMLP_IfLegalConsumePi(&match);
        if (BRSR_STAT_IS_FAILURE(e)) {
          goto htmlp_parse_error;
          }
        if (match == NW_TRUE) {
          continue;
          }
        
        e = NW_HTMLP_IfLegalConsumeCdata(&match);
        if (BRSR_STAT_IS_FAILURE(e)) {
          goto htmlp_parse_error;
          }
        if (match == NW_TRUE) {
          continue;
          }
        
        e = NW_HTMLP_IfLegalConsumeElementStart(&match);
        if (BRSR_STAT_IS_FAILURE(e)) {
          goto htmlp_parse_error;
          }
        if (match == NW_TRUE) {
          continue;
          }
        }
      }
    
    /* process character data up to start of possible markup '<' */
    e = NW_HTMLP_ConsumeContent(&match);
    if (BRSR_STAT_IS_FAILURE(e)) {
      goto htmlp_parse_error;
      }
    }
  
  if (iLastValid == -1 && iWbxmlWriter)
    {
    iLastValid = iWbxmlWriter->index;
    }
  
  /* remember the valid stack before close all is called */
  if ((iLastScriptStart == -1) && (iLastTextBegin == -1))
    {
    setValidMarks();
    }
  commitValidMarks();
  
  e = NW_HTMLP_ElementParseState_CloseAll();
  if (BRSR_STAT_IS_FAILURE(e)) {
    goto htmlp_parse_error;
    }
  
  /* end of document callback */
  if (iCBs->endDocumentCB != NULL /*&& e != KBrsrRestartParsing*/) 
    {
    e = (*(iCBs->endDocumentCB))(iLexer, e, iCBs->pClientPointer);
    }
  
htmlp_parse_error:
    
    NW_HTMLP_ElementParseState_Delete(&iElementParseState);
    if (e == KBrsrRestartParsing)
      {
      NW_HTMLP_ElementParseState_Delete(&iLastValidStack);
      iLastValidStack = NULL;
      NW_HTMLP_ElementParseState_New();
      }
    
    // iVisitedHeadText is no longer needed unless it is tracking an
    // unterminated comment.  In this cases iVisitedHeadText is needed
    // because NW_HTMLP_NotifyDocComplete calls NW_HTHMLP_RealParse
    // which needs it.
    if (isFirstSegment && iRestarted && !iTrackingUnTerminatedComment)
      {
      NW_Buffer_Free(iVisitedHeadText);
      iVisitedHeadText = NULL;
      }
    
    return e;
}

TBrowserStatusCode CHtmlpParser::appendVisitedHeadText(NW_Byte* new_data, NW_Uint32 new_len)
  {
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Buffer_t * tmp_buf;
  NW_Uint32 old_len = 0;
  
  tmp_buf = iVisitedHeadText;
  if (tmp_buf)
    old_len = tmp_buf->length;
  iVisitedHeadText = NULL;
  iVisitedHeadText = (NW_Buffer_t *)NW_Buffer_New(old_len + new_len);
  if (!iVisitedHeadText)
    {
    status = KBrsrOutOfMemory;
    goto cleanup;
    }
  
  if (tmp_buf)
    {
    (void)NW_Mem_memcpy(iVisitedHeadText->data, tmp_buf->data, old_len);
    }
  (void)NW_Mem_memcpy(iVisitedHeadText->data + old_len, new_data, new_len);
  iVisitedHeadText->length = old_len + new_len;

cleanup:
  NW_Buffer_Free(tmp_buf);
  return status;
  }

NW_Buffer_t* CHtmlpParser::getVisitedHeadText()
  {
  return iVisitedHeadText;
  }

void CHtmlpParser::updateCurrentCP()
  {
  if (iCurrentTagCP != iWbxmlWriter->tagCodePage)
    {
    // WLIU_DEBUG: NW_ASSERT(iCurrentCPCount == iWbxmlWriter->cp_count -1);
    iCurrentCPCount++;
    iCurrentTagCP = iWbxmlWriter->tagCodePage;
    }
  else if(iCurrentAttrCP != iWbxmlWriter->attributeCodePage)
    {
    // WLIU_DEBUG: NW_ASSERT(iCurrentCPCount == iWbxmlWriter->cp_count -1);
    iCurrentCPCount++;
    iCurrentAttrCP = iWbxmlWriter->attributeCodePage;
    }
  }

void CHtmlpParser::updateCurrentCP(NW_Uint32 switches)
  {
  if (switches != 0)
    {
    iCurrentCPCount += switches;
    iCurrentTagCP = iWbxmlWriter->tagCodePage;
    iCurrentAttrCP = iWbxmlWriter->attributeCodePage;
    }
  // WLIU_DEBUG: NW_ASSERT(iCurrentCPCount == iWbxmlWriter->cp_count);
  
  }

#else

void FeaRmeNoHTMLParser_htmlp_parser(){
  int i = 0;
  i+=1;
  }
#endif /* FEA_RME_NOHTMLPARSER */



// For WML purposes, which requires stricter checking of DTD, than HTML, 
// return the status of checking the attribute name
// against the appropriate attribute dictionary.
TBrowserStatusCode CHtmlpParser::NW_HTMLP_ValidateWMLAttribute(
  NW_HTMLP_Lexer_t* apL,
  const NW_HTMLP_Interval_t* apI_name,
  void* apV)
{
  NW_HTMLP_WbxmlEncoder_t* pTE = (NW_HTMLP_WbxmlEncoder_t*)apV;
  NW_WBXML_Writer_t* pW = pTE->pE;
  NW_Uint32 length;
  NW_Uint32 nameLength;
  NW_Uint32 sizeChar = 1;
  NW_Uint8* pName;
  TBrowserStatusCode s;

  NW_ASSERT(!iIsHtml);

  // make sure can handle the encoding used
  if (!((pTE->encoding == HTTP_us_ascii)
        || (pTE->encoding == HTTP_iso_8859_1)
        || (pTE->encoding == HTTP_utf_8)
        || (pTE->encoding == HTTP_iso_10646_ucs_2))) {
    return KBrsrFailure;
  }

  // establish the size of a character
  if (apL->encoding == HTTP_iso_10646_ucs_2) {
    sizeChar = 2;
  }

  // name well-formed?
  if (!NW_HTMLP_Interval_IsWellFormed(apI_name)) {
    return KBrsrFailure;
  }

  // var name setup 
  length = NW_HTMLP_Interval_ByteCount(apI_name);
  nameLength = length; // byte count 
  s = NW_HTMLP_Lexer_DataAddressFromBuffer(apL, apI_name->start,
                                           &nameLength, &pName);
  if (BRSR_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (nameLength != length) {
    return KBrsrFailure;
  }
  nameLength = NW_HTMLP_Interval_CharCount(apI_name); // char count

  // copy the name into a zero terminated string for dictionary checking
  NW_Uint8* pByteName = NULL;
  NW_String_t nameString;
  pByteName = (NW_Uint8*)NW_Mem_Malloc( length + sizeChar );  // allocating bytes
  if (!pByteName) {
    return KBrsrOutOfMemory;
  }
  NW_Mem_memset(pByteName, NULL, length + sizeChar);  // enough extra space for terminating null characters
  NW_Mem_memcpy(pByteName, pName, length);
  NW_String_initialize( &nameString, pByteName, pTE->encoding);
  NW_Int16 retVal = -1;          // initialize return to be -1 (meaning: attribute not found)
  retVal = NW_WBXML_Dictionary_getAttributeToken( pW->pAttributeDictionary, //NW_WBXML_Dictionary_t* dictionary,
                                       &nameString, //const NW_String_t* pNameString,
                                       pTE->encoding, //NW_Uint32 encoding,
                                       NW_FALSE ); //NW_Bool matchCase
  NW_Mem_Free( pByteName);
  pByteName = NULL;

  // the attribute was not found in the dictionary - as this is for WML - send back bad content message
  if (retVal == -1) {
    return KBrsrWmlbrowserBadContent;
  }

  return KBrsrSuccess;
}
