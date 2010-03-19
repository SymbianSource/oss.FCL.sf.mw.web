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
* Description:  Parser for CSS
*
*/



// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "CSSParser.h"
#include "nw_string_string.h"
#include <nwx_assert.h>
#include "nwx_http_defs.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// FORWARD DECLARATIONS

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

/* ------------------------------------------------------------------------- */
TBool TCSSParser::ParseElementName(TCSSReaderUnit* aNameSpace, TCSSReaderUnit* aElementName)
{
  TInt8 tokenType;
  TCSSReaderUnit token;

  switch(tokenType = iReader.ReadNextToken(&token))
  {
    case ALPHA:
      if (!iReader.ReadIdentifier(aElementName))
      {
        return EFalse;
      }
      tokenType = iReader.ReadNextToken(&token);
      break;
    case ASTERISK:
      tokenType = iReader.ReadNextToken(&token);
      break;
    case VERTICAL_LINE:
      break;
    default:
      SetPosition(token.iStorage);
      return ETrue;
  }
  if ((tokenType != VERTICAL_LINE) && (tokenType != -1))
  {
    SetPosition(token.iStorage);
    return ETrue;
  }
  /* There is a VERTICAL_LINE token - implies nameSpace present */
  aNameSpace->Init(aElementName->iStorage, aElementName->iLength, aElementName->iNumChars);
  aElementName->Init(NULL, 0, 0);

  switch(iReader.ReadNextToken(&token))
  {
    case ALPHA:
      return iReader.ReadIdentifier(aElementName);
    case ASTERISK:
      return ETrue;
    default:
      SetPosition(token.iStorage);
      return ETrue;
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode TCSSParser::SelectorListIterate(TCSSReaderUnit* aTargetElement,
                                             TUint32 *aSpecificity)
{
  TBrowserStatusCode status;
  TCSSReaderUnit token;
  TCSSSimpleSelector simpleSelector;
  TUint8 numSimpleSelectors;
  
  *aSpecificity = 0;
  numSimpleSelectors = 0;
  while ((status = SimpleSelectorListIterate(&simpleSelector)) == KBrsrIterateMore)
  {
    *aSpecificity += simpleSelector.specificity;
    numSimpleSelectors++;
  }
  /* Error in parsing selector means that ignore this rule */
  if ((status != KBrsrIterateDone) || (numSimpleSelectors == 0)){ 
    return KBrsrFailure;
  }
  /* status is KBrsrIterateDone */
  aTargetElement->iStorage = simpleSelector.elementName.iStorage;
  aTargetElement->iLength = simpleSelector.elementName.iLength;
  aTargetElement->iNumChars = simpleSelector.elementName.iNumChars;

  switch(iReader.ReadNextToken(&token))
  {
    case COMMA:
      return KBrsrIterateMore;
    case LEFT_BRACE:
      return KBrsrIterateDone;
    default:
      return KBrsrFailure;
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode TCSSParser::ParseRule()
{
  TBrowserStatusCode status;
  TCSSReaderUnit targetElement;
  TUint32 specificity = 0;

  /* parse selector */
  do
  {
    TText8* bufferPtr;

    iReader.SkipWhiteSpaces();
    bufferPtr = iReader.GetBufferPointer();
    status = SelectorListIterate(&targetElement, &specificity);  
    if (iEventListener != NULL)
    {
      TRAPD(ret, iEventListener->SelectorL(bufferPtr, &targetElement, specificity));
      if (ret == KErrNoMemory)
      {
        return KBrsrOutOfMemory;
      }
   }
  }while (status == KBrsrIterateMore);

  /* parse declaration list */
  do
  {
    TCSSReaderUnit property;
    TCSSPropertyVal propertyVal;
    TBool important;
    TUint8 numVals;

    status = DeclarationListIterate(&property, &propertyVal, &numVals, 1, &important);   

  }while (status == KBrsrIterateMore);

  if (status != KBrsrIterateDone)
  {
    TText8 *position;
    return iReader.GoToToken(RIGHT_BRACE, &position);
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
/* parse comma separated list of media */
TBool TCSSParser::HasValidMedia()
{
  TCSSReaderUnit medium;
  TInt8 tokenType;
  TCSSReaderUnit unit;
  TBool applies;

  applies = EFalse;

  do
  {
    iReader.SkipWhiteSpaces();
    /* Read medium */
    if (!iReader.ReadIdentifier(&medium))
    {
      return EFalse;
    }
    if (!applies)
    {/* Is handleheld mentioned ? */
      applies = iReader.IsValidMedia(&medium);
    }
    iReader.SkipWhiteSpaces();
    tokenType = iReader.ReadNextToken(&unit);
    if ((tokenType == SEMI_COLON) || (tokenType == LEFT_BRACE))
    {
      SetPosition(unit.iStorage);
      return applies;
    }
  }while (tokenType == COMMA);

  return EFalse;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode TCSSParser::ParseImportRule()
{
  TCSSReaderUnit uri;
  TCSSReaderUnit unit;
  TBool applies;
  TInt8 tokenType;

  /* Skip any spaces */
  iReader.SkipWhiteSpaces();
  tokenType = iReader.ReadNextToken(&uri);
  if (tokenType == URI)
  {
    if (!iReader.ReadURI(&uri))
    {
      return KBrsrFailure;
    }
  }
  if ((tokenType != URI) && (tokenType != STRING))
  {
    return KBrsrFailure;
  }
  
  iReader.SkipWhiteSpaces();
  switch(iReader.ReadNextToken(&unit))
  {
    case SEMI_COLON:
      break;
    case ALPHA:
      applies = HasValidMedia();
      if (iReader.ReadNextToken(&unit) != SEMI_COLON)
      {
        return KBrsrFailure;
      }
      if (!applies)
      {
        return KBrsrSuccess;
      }
      break;
    default:
    /* return advance to semi-colon */
      return KBrsrFailure;
  }
  if (iEventListener)
  {
    return iEventListener->Import(&uri);
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode TCSSParser::ParseMediaRule()
{
  TCSSReaderUnit unit;

  if (!HasValidMedia()){
    return KBrsrFailure;
  }
  if (iReader.ReadNextToken(&unit) != LEFT_BRACE)
  {
    return KBrsrFailure;
  }
  while(iReader.ReadNextToken(&unit) != RIGHT_BRACE)
  {
    SetPosition(unit.iStorage);
    if (ParseRule() != KBrsrSuccess)
    {
      return KBrsrFailure;
    }
    iReader.SkipWhiteSpaces();
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
/* parse the header which can be comments, whitespaces, or import rules 
 * [S|CDO|CDC]* [import [S|CDO|CDC]* ]*
 */
TBrowserStatusCode TCSSParser::ParseStyleRules()
{
  TBrowserStatusCode status;
  TCSSReaderUnit token;
  TUint32 readerPosition;
  TText8 *position;
  TBool atRuleParsed = EFalse;

  status = KBrsrSuccess;
  iReader.SkipWhiteSpaces();
  while(status == KBrsrSuccess)
  {
    switch(iReader.ReadNextToken(&token))
    {
      case -1:
      case BUFFER_END:
        return KBrsrBufferEnd;
      case CDO:
        /* Skip SGML comments */
        break;
      case CDC:
        /* Skip SGML comments */
        break;
      case ATKEYWORD:
      case CHARSET_RULE:
        readerPosition = iReader.GetPosition();
        atRuleParsed = iReader.IgnoreAtRule(readerPosition);
        if (!atRuleParsed)
        {
          status = KBrsrFailure;
        }
        break;
      case IMPORT_RULE:
        readerPosition = iReader.GetPosition();
        if (ParseImportRule() != KBrsrSuccess)
        {
            atRuleParsed = iReader.IgnoreAtRule(readerPosition);
            if (!atRuleParsed)
            {
                status = KBrsrFailure;
            }
        }   
       break;
      case MEDIA_RULE:
        readerPosition = iReader.GetPosition();
        if (ParseMediaRule() != KBrsrSuccess)
        {
          atRuleParsed = iReader.IgnoreAtRule(readerPosition);
          if (!atRuleParsed)
          {
            status = KBrsrFailure;
          }
        }
        break;
      default:
        SetPosition(token.iStorage);
        if (ParseRule() != KBrsrSuccess)
        {
          status = iReader.GoToToken( RIGHT_BRACE, &position);          
        }
        break;
    }
    iReader.SkipWhiteSpaces();
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode TCSSParser::ParseHeader()
{
  TBrowserStatusCode status;
  TText8 *position;
  TCSSReaderUnit token;
  TUint32 thisPosition;

    // read the next token for @charset rule 
  TInt tok = iReader.ReadNextToken(&token) ;
  thisPosition = iReader.GetPosition();
 
  if(tok == -1 )
  {
    // various failures in read token return -1 
    return KBrsrFailure;
  }
  if (tok == CHARSET_RULE)
  {
    if (iReader.GoToToken(SEMI_COLON, &position) != KBrsrSuccess)
    {
        return KBrsrFailure;
    }
  }

  //If it is comment in the begginning then continue to read. Do not set
  //position again. Otherwise in the embedded style sheet, <style> element
  //is skipped.

  if(tok != CDO)
  {
   iReader.SetPosition(thisPosition - token.iLength);
  }

  /* parse the header which can be comments, whitespaces, or import rules 
   * [S|CDO|CDC]* [import [S|CDO|CDC]* ]*
   */
  status = KBrsrSuccess;
  while (status == KBrsrSuccess)
  {
    switch(iReader.ReadNextToken(&token))
    {
      case -1:
      case BUFFER_END:


        return KBrsrBufferEnd;
      case SPACE:
        break;
      case CDO:
        /* Skip SGML comments */
        break;
      case CDC:
        /* Skip SGML comments */
        break;
      case IMPORT_RULE:
        if (ParseImportRule() != KBrsrSuccess){
          status = iReader.GoToToken(SEMI_COLON, &position); 
        }
        break;
      default:
        SetPosition(token.iStorage);
        return KBrsrSuccess;
    }
  }
  return status;
}

/* -------------------------------------------------------------------------*/
TBrowserStatusCode TCSSParser::ParserPropertyVal(TCSSPropertyVal* aPropertyVal)
{
  TCSSReaderUnit token;
  TUint32 thisPosition;

  NW_ASSERT(aPropertyVal);

  aPropertyVal->next = NULL;

  iReader.SkipWhiteSpaces();
  aPropertyVal->val.Init(NULL, 0, 0);
  thisPosition = iReader.GetPosition();
  switch (aPropertyVal->type = iReader.ReadNextToken(&aPropertyVal->val))
  {
    case URI:
      if (!iReader.ReadURI(&aPropertyVal->val))
      {
        if (!iReader.ReadIdentifier(&aPropertyVal->val))
        {
          return KBrsrFailure;
        }
        aPropertyVal->type = IDENTIFIER;
      }
      break;
    case STRING:
      break;
    case HASH:
      if (!iReader.ReadName(&aPropertyVal->val))
      {
        return KBrsrFailure;
      }
      aPropertyVal->type = HASH;
      break;
    case ALPHA:
      if (!iReader.ReadIdentifier(&aPropertyVal->val))
      {
        return KBrsrFailure;
      }
      aPropertyVal->type = IDENTIFIER;
      break;
    case HYPHEN:
      {
        thisPosition = iReader.GetPosition();
        aPropertyVal->type = iReader.ReadNumberToken(&aPropertyVal->val);
        if (aPropertyVal->type == -1)
        {
          iReader.SetPosition(thisPosition);
          if (!iReader.ReadIdentifier(&aPropertyVal->val))
          {
            return KBrsrFailure;
          }
          aPropertyVal->type = IDENTIFIER;
        }
        break;
      }
    case RGB:
      {
        TCSSPropertyVal rVal;
        TCSSPropertyVal gVal;
        TCSSPropertyVal bVal;
        aPropertyVal->val.iStorage = GetPosition();
        if (ParserRGB(&rVal, &gVal, &bVal) != KBrsrSuccess)
        {
          return KBrsrFailure;
        }
      }
      break;
    case DOT:
      iReader.SetPosition(thisPosition);
      //Fall through to the default and read the number now
      /*lint -fallthrough */

    default:
      aPropertyVal->type = iReader.ReadNumberToken(&aPropertyVal->val);
      if (aPropertyVal->type == -1)
      {
        return KBrsrFailure;
      }
      break;
  }

  iReader.SkipWhiteSpaces();
  thisPosition = iReader.GetPosition();
  switch(iReader.ReadNextToken(&token))
  {
    case IMPORTANT:
    case RIGHT_BRACE:
    case SEMI_COLON:
    case BUFFER_END:
      SetPosition(token.iStorage); 
      return KBrsrSuccess;
    case URI:
      iReader.SetPosition(thisPosition);
      aPropertyVal->next = iReader.GetBufferPointer();
      return KBrsrSuccess;
    default:
      SetPosition(token.iStorage); 
      aPropertyVal->next = iReader.GetBufferPointer();
      return KBrsrSuccess;
  }
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------*/
TBrowserStatusCode TCSSParser::ParserRGB(TCSSPropertyVal* aRVal,
                                  TCSSPropertyVal* aGVal,
                                  TCSSPropertyVal* aBVal)
{
  TCSSReaderUnit token;

  NW_ASSERT(aRVal);
  NW_ASSERT(aGVal);
  NW_ASSERT(aBVal);

  iReader.SkipWhiteSpaces();
  aRVal->type = iReader.ReadNumberToken(&aRVal->val);
  iReader.SkipWhiteSpaces();
  if (iReader.ReadNextToken(&token) != COMMA)
  {
    return KBrsrFailure;
  }

  iReader.SkipWhiteSpaces();
  aGVal->type = iReader.ReadNumberToken(&aGVal->val);
  iReader.SkipWhiteSpaces();
  if (iReader.ReadNextToken(&token) != COMMA)
  {
    return KBrsrFailure;
  }

  iReader.SkipWhiteSpaces();
  aBVal->type = iReader.ReadNumberToken(&aBVal->val);
  iReader.SkipWhiteSpaces();
  if (iReader.ReadNextToken(&token) != RIGHT_PARENTHESIS)
  {
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}

/* ----------------------------------------------------------------------- */
/* declaration := property ':' S* expr prio?
   property := IDENT S*
   prio: ! S* important S*
   */
TBrowserStatusCode 
TCSSParser::DeclarationListIterate(TCSSReaderUnit* aProperty,
                                   TCSSPropertyVal* aPropertyVal,
                                   TUint8 *aNumVals,
                                   TUint8 aMaxVals,
                                   TBool* aImportant)
{
  TBrowserStatusCode status;
  TInt8 tokenType;
  TUint8 valIndex = 0;
  TCSSReaderUnit token;
  TCSSPropertyVal tempVal;

  *aImportant = NW_FALSE;
  *aNumVals = 0;
  iReader.SkipWhiteSpaces();

  switch(iReader.ReadNextToken(&token))
  {
    case RIGHT_BRACE:
    case BUFFER_END:
      return KBrsrIterateDone;
    case ALPHA:
    case HYPHEN:
      if (!iReader.ReadIdentifier(aProperty))
      {
        return KBrsrFailure;
      }
      break;
    default:
      return KBrsrFailure;
  }
  iReader.SkipWhiteSpaces();
  if (iReader.ReadNextToken(&token) != COLON)
  {
    return KBrsrFailure;
  }

  /* Read property value */
  status = ParserPropertyVal(&tempVal); 
  if (status != KBrsrSuccess)
  {
    return status;
  }

  if (valIndex < aMaxVals) 
  {
    aPropertyVal[valIndex++] = tempVal;
    (*aNumVals)++;
  }

  /* if there is more than one value read the others e.g. fontborder */
  while (tempVal.next != NULL)
  {
    /* if need be skip over the comma delimiter */
    tokenType = iReader.ReadNextToken(&token);
    if (tokenType != COMMA)
    {
      /* "undo" ReadNextToken */
      SetPosition(token.iStorage);
    }
 
    status = ParserPropertyVal(&tempVal); 
    if (status != KBrsrSuccess)
    {
      return status;
    }

    if (valIndex < aMaxVals) 
    {
      aPropertyVal[valIndex++] = tempVal;
      (*aNumVals)++;
    }
  }
  tokenType = iReader.ReadNextToken(&token) ;

  if (tokenType == IMPORTANT)
  {
    *aImportant = NW_TRUE;
    iReader.SkipWhiteSpaces();
    tokenType = iReader.ReadNextToken(&token);
  }
  /* Consume semi-colons (there might be more than one (not in spec)*/
  if (tokenType == SEMI_COLON)
  {
    do
    {
      iReader.SkipWhiteSpaces();
    }while(iReader.ReadNextToken(&token) == SEMI_COLON);
  }
  SetPosition(token.iStorage);
  return KBrsrIterateMore;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
TCSSParser::ConditionListIterate(TText8* aConditionType,
                                 TCSSReaderUnit *aValue1,
                                 TCSSReaderUnit *aValue2)
{
  TCSSReaderUnit token;
  TInt8 tokenType;
  TUint32 readerPosition;
  
  NW_ASSERT(aValue1);
  NW_ASSERT(aValue2);
  NW_ASSERT(aConditionType);

  aValue1->Init(NULL, 0, 0);
  aValue2->Init(NULL, 0, 0);
  *aConditionType = 0;

  switch(tokenType = iReader.ReadNextToken(&token))
  {
    case SPACE:
    case COMMA:
    case LEFT_BRACE:
      SetPosition(token.iStorage);
      return KBrsrIterateDone;
    case -1:
      return KBrsrBufferEnd;
    case DOT:
      if (!iReader.ReadIdentifier(aValue1))
      {
        return KBrsrFailure;
      }
      *aConditionType = CLASS_CONDITION;      
      break;
    case HASH:
      *aConditionType = ID_CONDITION;
      if (!iReader.ReadName(aValue1))
      {
        return KBrsrFailure;
      }
      break;
    case COLON:
      if ((tokenType = iReader.GetPseudoClass()) == -1)
      {
        return KBrsrFailure;
      }
      *aConditionType = tokenType;
      break;
    case LEFT_BRACKET:
      if (!iReader.ReadIdentifier(aValue1))
      {
        return KBrsrFailure;
      }
      iReader.SkipWhiteSpaces();    
      if (iReader.ReadNextToken(&token) != EQUALS) 
      {
        return KBrsrFailure;
      }
      iReader.SkipWhiteSpaces();
      readerPosition = iReader.GetPosition();
      if (iReader.ReadNextToken(&token) == STRING) 
      {      
        iReader.SetPosition(readerPosition);
        if (!iReader.ReadString(aValue2))
        {
          return KBrsrFailure;
        }
      } 
      else 
      {
        if (!iReader.ReadName(aValue2))
        {
          return KBrsrFailure;
        }
      }
      iReader.SkipWhiteSpaces();    
      if (iReader.ReadNextToken(&token) != RIGHT_BRACKET) 
      {
        return KBrsrFailure;
      }
      *aConditionType = ATTRIBUTE_CONDITION;      
      break;

    default:
      return KBrsrFailure;
  }
  if (iEventListener)
  {
    TRAPD(ret, iEventListener->ConditionL(*aConditionType, aValue1, aValue2));
    if (ret == KErrNoMemory)
    {
      return KBrsrOutOfMemory;
    }

  }
  return KBrsrIterateMore;
}

/* -------------------------------------------------------------------------*/ 
TBrowserStatusCode  
TCSSParser::SimpleSelectorListIterate(TCSSSimpleSelector* aSimpleSelector)
{
  TBrowserStatusCode status;
  TCSSReaderUnit token;
  TCSSReaderUnit value1, value2;
  TUint8 conditionType = 0;
  
  NW_ASSERT(aSimpleSelector);

  iReader.SkipWhiteSpaces();    

  switch(iReader.ReadNextToken( &token))
  {
    case LEFT_BRACE:
    case COMMA:
      SetPosition(token.iStorage);
      return KBrsrIterateDone;
    case -1:
      return KBrsrBufferEnd;
    default:
      SetPosition(token.iStorage);
      break;
  }

  /* initialize simple selector */
  aSimpleSelector->nameSpace.Init(NULL, 0, 0);
  aSimpleSelector->elementName.Init(NULL, 0, 0);
  aSimpleSelector->conditionListHandle = NULL;
  aSimpleSelector->specificity = 0;

  /* Get namespace and elementName */
  TBool parsed = ParseElementName(&aSimpleSelector->nameSpace, &aSimpleSelector->elementName);
  if (!parsed)
  {
    return KBrsrFailure;
  }
  if (iEventListener)
  {
    TRAPD(ret, iEventListener->SimpleSelectorStartL(&aSimpleSelector->elementName));
    if (ret == KErrNoMemory)
    {
      return KBrsrOutOfMemory;
    }
  }

  if (aSimpleSelector->elementName.iLength != 0)
  {
    aSimpleSelector->specificity = 1;
  }
  /* parse conditions */
  aSimpleSelector->conditionListHandle = iReader.GetBufferPointer();
  status = ConditionListIterate(&conditionType, &value1, &value2);

  if (status == KBrsrIterateDone)
  {
    aSimpleSelector->conditionListHandle = NULL;
  }

  while (status == KBrsrIterateMore)
  {
    if (conditionType)
    {
     if (conditionType == ID_CONDITION)
     {
       aSimpleSelector->specificity += 100;
     }
     else
     {
       aSimpleSelector->specificity += 10;
     }
    }
    status = ConditionListIterate(&conditionType, &value1, &value2);
  }
  if (iEventListener)
  {
      TBool failed = (status != KBrsrIterateDone)? ETrue:EFalse;
    TRAPD(ret, iEventListener->SimpleSelectorEndL(aSimpleSelector, failed));
    if (ret == KErrNoMemory)
    {
      return KBrsrOutOfMemory;
    }
  }
  if (status != KBrsrIterateDone)
  {
    return status;
  }
  return KBrsrIterateMore;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode  
TCSSParser::ParseStyleSheet(MCSSEventListener* aEventListener)
{
  TBrowserStatusCode status;

  if (aEventListener != NULL)
  {
    iEventListener = aEventListener;
    iEventListener->SetCurrentEncoding(iReader.GetEncoding());
  }
  status = ParseHeader();
  if (status == KBrsrBufferEnd)
  {
    return KBrsrSuccess;
  }
  if (status != KBrsrSuccess)
  {
    return status;
  }
  /* Parse style rules 
   * [ruleset | media | [S|CDO|CDC]* ]*
   */
  status = ParseStyleRules();
  if (status == KBrsrBufferEnd)
  {
    return KBrsrSuccess;
  }
  return status;
}

/* ------------------------------------------------------------------------- */
TUint32 TCSSParser::ParseCharset()
{
  TUint32 prevEncoding = GetEncoding(); 
  if (iReader.ReadCharset() != KBrsrSuccess)
  {
    return prevEncoding;
  }
  return GetEncoding();  
}

/* ------------------------------------------------------------------------- */
void TCSSParser::SetPosition(TText8* aPosition)
{
  iReader.SetPosition(aPosition - iReader.GetBuffer());
}


