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
* Description:  Contains all CSS styles
*
*/



// INCLUDE FILES
#include "CSSRuleList.h"
#include <nwx_assert.h>
#include "BrsrStatusCodes.h"
#include "nwx_settings.h"

#include "CSSParser.h"
#include "nw_text_ucs2.h"
#include <nwx_string.h>
#include "nw_hed_contenthandler.h"
#include "CSSDeclListHandle.h"
#include "TKeyArrayPtr.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCSSRuleList::CCSSRuleList
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSRuleList::ConstructL(NW_CSS_Processor_t* aProcessor)
  {
    /* initialize the element map */
    iElementList = new (ELeave) CCSSElementList(2);
    /* initialize the bufferList */
    iBufferList = CCSSBufferList::NewL();
    iCurrentBufferIndex = -1;
    iProcessor = aProcessor;
  }

// -----------------------------------------------------------------------------
// CCSSRuleList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSRuleList* CCSSRuleList::NewL(NW_CSS_Processor_t* aProcessor)
  {
  CCSSRuleList* self = new( ELeave ) CCSSRuleList();

  CleanupStack::PushL( self );
  self->ConstructL(aProcessor);
  CleanupStack::Pop();

  return self;
  }


// Destructor
CCSSRuleList::~CCSSRuleList()
  {
    if (iElementList != NULL)
    {
      iElementList->ResetAndDestroy();
      delete iElementList;
    }
	
	if (iBufferList != NULL)
		delete iBufferList;

	if (iSelector != NULL)
		delete iSelector;
  }

// -----------------------------------------------------------------------------
// CCSSRuleList::ProcessBuffer
// parses a stylesheet
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CCSSRuleList::ProcessBuffer(TText8* aData, TUint32 aLength, TUint32 aEncoding, TBool aDoNotDelete)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(aData != NULL);

  TRAPD(ret, iBufferList->AddL(aData, aLength, aEncoding, aDoNotDelete, NULL));
  if (ret == KErrNoMemory)
  {
    status = KBrsrOutOfMemory;
  }
  if (status == KBrsrSuccess)
  {
    iCurrentBufferIndex = iBufferList->GetSize() - 1;
    TCSSParser parser(aData, aLength, aEncoding);
    status =  parser.ParseStyleSheet((MCSSEventListener*)(this));
    iCurrentBufferIndex = -1;
  }
  return status;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::GetElementRules
// Returns ruleList (a dynamic vector sorted by precedence) for a given element
// -----------------------------------------------------------------------------
//
CCSSRules*
CCSSRuleList::GetElementRules(TUint16 aToken)
{
  CCSSRules* rules = NULL;
  TInt index = 0;
  CCSSRuleListElementEntry* tempEntry = NULL;
  TRAPD( status, tempEntry = new(ELeave)CCSSRuleListElementEntry(aToken) );

  if( status == KErrNone )
    {
    TNumKeyArrayPtr matchkey(_FOFF(CCSSRuleListElementEntry, iElementToken), ECmpTUint16);
    if (iElementList->FindIsq( tempEntry, matchkey, index) == 0)
      {
      CCSSRuleListElementEntry* entry = iElementList->At(index);
      rules = entry->iRules;
      }
    delete tempEntry;
    }
  return rules;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::GetNextRule
// Returns rule in the list referenced by aIndex. Returns NULL if not valid.
// -----------------------------------------------------------------------------
//
CCSSRuleListElementEntry*
CCSSRuleList::GetElementRulesByIndex(TInt aIndex)
{
  if (aIndex >= iElementList->Count())
    return NULL;

  CCSSRuleListElementEntry* entry = iElementList->At(aIndex);
  return entry;
}


// -----------------------------------------------------------------------------
// CCSSRuleList::GetDictionary
// Returns dictionary
// -----------------------------------------------------------------------------
//
NW_WBXML_Dictionary_t*
CCSSRuleList::GetDictionary()
{
  return iDictionary;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::GetBufferList
// Returns the buffer list
// -----------------------------------------------------------------------------
//
CCSSBufferList* CCSSRuleList::GetBufferList()
{
  return iBufferList;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::SetCurrentBufferIndex
// Sets the index of the buffer being parsed
// -----------------------------------------------------------------------------
//
void CCSSRuleList::SetCurrentBufferIndex(TInt aIndex)
{
  iCurrentBufferIndex = aIndex;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::GetSize
// Returns the size of the rules list. This corresponds to the number of
// element entries in the iElementList
// -----------------------------------------------------------------------------
//
TInt CCSSRuleList::GetSize()
{
  return iElementList->Count();
}

// -----------------------------------------------------------------------------
// CCSSRuleList::SetDictionary
// Set dictionary
// -----------------------------------------------------------------------------
//
void CCSSRuleList::SetDictionary(NW_WBXML_Dictionary_t* aDictionary)
{
  iDictionary = aDictionary;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::StartDocument
//
// -----------------------------------------------------------------------------
//
void CCSSRuleList::StartDocumentL(TText8*)
{
}

// -----------------------------------------------------------------------------
// CCSSRuleList::EndDocument
//
// -----------------------------------------------------------------------------
//
void CCSSRuleList::EndDocumentL(TText8*)
{
}

// -----------------------------------------------------------------------------
// CCSSRuleList::Selector
// Creates a new rule entry for each selector
// -----------------------------------------------------------------------------
//
void CCSSRuleList::SelectorL(TText8* aSelector,
                           TCSSReaderUnit* aTargetElement,
                           TUint32 aSpecificity)
{
  TInt currentBufferPriority = iBufferList->GetPriority(iCurrentBufferIndex);

  NW_REQUIRED_PARAM( aTargetElement );

  NW_ASSERT(aSelector != NULL);
  NW_ASSERT(aTargetElement != NULL);

  if (iSelector)
  {
  // last simpleSelector will tell us the target token
  TInt numSimpleSelectors = iSelector->iSimpleSelectorList->Count();
  CCSSSimpleSelector* simpleSelector = iSelector->iSimpleSelectorList->At(numSimpleSelectors-1);
  TUint16 elementToken = simpleSelector->iElementToken;

  // we need to know where to insert this selector
  CCSSRules* elementRules = GetElementRules(elementToken);
  if (elementRules == NULL)
  {
    CCSSRuleListElementEntry* tempElementEntry
                       = new(ELeave)CCSSRuleListElementEntry(elementToken);
    CleanupStack::PushL( tempElementEntry );

    tempElementEntry->iRules = new (ELeave) CCSSRules(2);

    // we want to insert the element entry in key order
    TNumKeyArrayPtr matchkey(_FOFF(CCSSRuleListElementEntry, iElementToken), ECmpTUint16);
    /* set the entry in the map  */
    iElementList->InsertIsqL(tempElementEntry, matchkey);

    CleanupStack::Pop(); // tempElementEntry & tempElementEntry->iRules
    elementRules = tempElementEntry->iRules;
  }

  /* Insert the element into Vector according to the specificity */
  /* First find the index */
  TInt size = elementRules->Count();
  TInt index =0;
  for(index =0; index < size; index++)
  {
    CCSSSelector *selector = elementRules->At(index);

    TInt selectorBufferPriority = iBufferList->GetPriority(selector->iBufferIndex);

    /* Found the index to insert the new rule */
    /* if there are 2 rules with same specificity, then the rule from the
       latter buffer gets precedence */
    if ((aSpecificity > selector->iSpecificity) ||
       ((aSpecificity == selector->iSpecificity) &&
        (selectorBufferPriority <= currentBufferPriority)))
      break;
  }

  /* create a new entry for selector and initialize it */
  iSelector->Init(aSelector, (TUint16)aSpecificity, iCurrentBufferIndex);
  CCSSSelector* tempSelector = iSelector;
  iSelector = NULL;
  elementRules->InsertL(index, tempSelector);
  }
}

// -----------------------------------------------------------------------------
// CCSSRuleList::SimpleSelector
//
// -----------------------------------------------------------------------------
//
void CCSSRuleList::SimpleSelectorEndL(TCSSSimpleSelector* , TBool aFailed)
{
  NW_ASSERT(iSelector != NULL);

  if (aFailed)
  {
   // there was and error removed last simple selector
  TInt numSimpleSelectors = iSelector->iSimpleSelectorList->Count();

  // removed last simple selector
  if (numSimpleSelectors > 0 )
  {
  CCSSSimpleSelector* simpleSelector = iSelector->iSimpleSelectorList->At(numSimpleSelectors-1);
  iSelector->iSimpleSelectorList->Delete(numSimpleSelectors-1);
  delete simpleSelector;
  }
  if (iSelector->iSimpleSelectorList->Count() == 0)
    {
    delete iSelector;
    iSelector = NULL;
    }

  }
}

// -----------------------------------------------------------------------------
// CCSSRuleList::SimpleSelectorStart
//
// -----------------------------------------------------------------------------
//
void CCSSRuleList::SimpleSelectorStartL(TCSSReaderUnit *aTargetElement)
{
  TUint16 elementToken = 0;

  NW_ASSERT(aTargetElement != NULL);

  // get element token
  if (aTargetElement->iLength != 0)
  {
    TText16* ucs2Str = aTargetElement->GetUnicodeL(iCurrentEncoding);
    User::LeaveIfNull(ucs2Str);

    elementToken =
      (NW_Uint16)NW_WBXML_Dictionary_getTagToken(iDictionary, (NW_String_UCS2Buff_t *)ucs2Str, NW_FALSE);
    NW_Mem_Free(ucs2Str);
  }
  if (iSelector == NULL)
  {
    iSelector = new (ELeave) CCSSSelector();
    iSelector->iSimpleSelectorList = new (ELeave) CCSSSimpleSelectorList(1);
  }
  // create new Simple Selector entry
  CCSSSimpleSelector* simpleSelector = new (ELeave) CCSSSimpleSelector(elementToken);
  CleanupStack::PushL(simpleSelector);

  iSelector->iSimpleSelectorList->AppendL(simpleSelector);
  CleanupStack::Pop();// pop simpleselector
}

// -----------------------------------------------------------------------------
// CCSSRuleList::Condition
//
// -----------------------------------------------------------------------------
//
void CCSSRuleList::ConditionL(TUint8 aConditionType,
                      TCSSReaderUnit *aValue1,
                      TCSSReaderUnit *aValue2)
{
  TInt numSimpleSelectors = iSelector->iSimpleSelectorList->Count();

  // get last simple selector
  CCSSSimpleSelector* simpleSelector =
    iSelector->iSimpleSelectorList->At(numSimpleSelectors-1);

  if (simpleSelector->iConditionList == NULL)
  {
    simpleSelector->iConditionList = new (ELeave) CCSSConditionList(1);
  }
  // create a new condition entry
  CCSSCondition* condition = new (ELeave) CCSSCondition(aConditionType);
  CleanupStack::PushL(condition);

  // in case of CLASS or ID condition value1 is the value of attribute
  // in case of attribute condition - it is the name of the attribute
  if ((aConditionType == CLASS_CONDITION) ||
      (aConditionType == ID_CONDITION) ||
      (aConditionType == ATTRIBUTE_CONDITION))
  {
    if (aValue1)
    {
      condition->iValue1 = aValue1->GetUnicodeL(iCurrentEncoding);
    }
    else
    {
      delete condition;// no point of adding condition if it's not proper structure
      condition = NULL;
    }
  }
  if (aValue1 && aValue2 && (aConditionType == ATTRIBUTE_CONDITION))
  {
    //lint -e{644} Variable may not have been initialized
    condition->iValue2 = aValue2->GetUnicodeL(iCurrentEncoding);
  }
  // add it to simple selector list
  if (condition)
  {
    simpleSelector->iConditionList->AppendL(condition);
  }

  // Pop condition
  CleanupStack::Pop();
}

// -----------------------------------------------------------------------------
// CCSSRuleList::Property
//
// -----------------------------------------------------------------------------
//
void CCSSRuleList::PropertyL(TCSSReaderUnit* aName,
                                   TCSSPropertyVal* aPropertyVal,
                                   TBool aImportant)
{
  NW_REQUIRED_PARAM(aName);
  NW_REQUIRED_PARAM(aPropertyVal);
  NW_REQUIRED_PARAM(aImportant);
}

// -----------------------------------------------------------------------------
// CCSSRuleList::Import
// Issues load request for stylesheet pointed by aUrl
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CCSSRuleList::Import(TCSSReaderUnit* aUrl)
{
  TText16* ucs2Str;
  NW_Text_t* url;
  NW_Text_UCS2_t* tempUrl;
  TBrowserStatusCode status;
  TText16* urlStr = NULL;
  NW_Text_Length_t urlLen;
  NW_Bool freeNeeded;
  TInt cssBufferIndex = 0;

  NW_ASSERT(aUrl != NULL);

  if (aUrl == NULL){
    return KBrsrSuccess;
  }

  ucs2Str = aUrl->GetUnicodeL(iCurrentEncoding);
  if (ucs2Str == NULL)
  {
    return KBrsrFailure;
  }
  tempUrl = NW_Text_UCS2_New (ucs2Str, (TUint16)NW_Str_Strlen(ucs2Str),
                      NW_Text_Flags_TakeOwnership);
  if (tempUrl == NULL)
  {
    return KBrsrOutOfMemory; /* NW_Text_UCS2_New() takes care of deleting string when fails */
  }
  /* find url to import */
  /* NOTE: if ResolveURL() returns a non-NULL pointer then the text object
   * passed in is not leaked; if it returns a NULL pointer then the passed
   * text object must be freed by the caller
   */
  status =
    NW_HED_ContentHandler_ResolveURL (iProcessor->owner,
                                      NW_TextOf (tempUrl), &url);
  if (status != KBrsrSuccess)
  {
    NW_Object_Delete(tempUrl);
    return KBrsrSuccess; /* ignore import */
  }
  NW_ASSERT(iCurrentBufferIndex >= 0);

  /* Get a string for the resolved url. If the creation fails, it will still be
   * NULL, which won't cause any major problems. */
  urlStr = NW_Text_GetUCS2Buffer (url, 0, &urlLen, &freeNeeded);

  /* add a new entry to the bufferList */
  TRAPD(ret, iBufferList->AddL(NULL, 0, iCurrentEncoding, NW_FALSE, urlStr));
  cssBufferIndex = iBufferList->GetSize() - 1;
  if (ret == KErrNoMemory)
  {
    status = KBrsrOutOfMemory;
  }
  if (freeNeeded)
  {
    NW_Str_Delete(urlStr);
  }
  if (status == KBrsrSuccess)
  {
    iBufferList->UpdatePriority(iCurrentBufferIndex);
    /* finally load the url */
    status = NW_CSS_Processor_ProcessLoadRequest( iProcessor,
                                                  NW_TextOf(url),
                                                  NW_CSS_Processor_LoadCSS,
                                                  iProcessor->isSavedDeck,
                                                  iProcessor->isHistLoad,
												  iProcessor->isReLoad,
                                                  cssBufferIndex);
  }
  else
  {
    status = KBrsrSuccess; /* ignore import */
  }
  
  NW_Object_Delete(url);
  url = NULL;

  return status;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::GetCurrentEncoding
// Returns Returns the encoding of the document being parsed
// -----------------------------------------------------------------------------
//
TUint32 CCSSRuleList::GetCurrentEncoding()
{
  return iCurrentEncoding;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::SetCurrentEncoding
// Sets the encoding of the document being parsed
// -----------------------------------------------------------------------------
//
void CCSSRuleList::SetCurrentEncoding(TUint32 aEncoding)
{
  iCurrentEncoding = aEncoding;
}

// -----------------------------------------------------------------------------
// CCSSRuleList::ApplyStyles
// Applies styles for a given element node in the document after pattern matching
// -----------------------------------------------------------------------------
//
void CCSSRuleList::ApplyStylesL(NW_DOM_ElementNode_t* aElementNode,
                           NW_LMgr_Box_t* aBox,
                           const NW_Evt_Event_t* aEvent,
                           NW_HED_DomHelper_t* aDomHelper)
{
  TInt univRuleIndex = 0;
  CCSSNodeData nodeData;

  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aBox != NULL);
  NW_ASSERT(aDomHelper != NULL);

  TUint16 elementToken = NW_DOM_ElementNode_getTagToken(aElementNode);

  /* apply styles for the given elementToken */
  CCSSRules* elementRules = GetElementRules(elementToken);
  CCSSRules* universalRules = GetElementRules(0);

  if (elementRules!= NULL)
  {
    TInt elementRulesSize = elementRules->Count();

    for(TInt index =0;index < elementRulesSize;index++)
    {
      CCSSSelector* entry1 = elementRules->At(index);

      TInt priority1 = iBufferList->GetPriority(entry1->iBufferIndex);

      if (universalRules)
      {
        /* first apply any universal rule which has a higher weight than this rule */
        TInt universalRulesSize = universalRules->Count();
        while (univRuleIndex < universalRulesSize)
        {
          CCSSSelector *entry2 = universalRules->At(univRuleIndex);
          TInt priority2 = iBufferList->GetPriority(entry2->iBufferIndex);

          if ((entry2->iSpecificity < entry1->iSpecificity) ||
             ((entry2->iSpecificity == entry1->iSpecificity) && (priority2 < priority1)))
            break;

          ApplyEntryStylesL(aElementNode, entry2, aBox, aEvent, aDomHelper, &nodeData);

          univRuleIndex++;
        } // while
      }// if
      ApplyEntryStylesL(aElementNode, entry1, aBox, aEvent, aDomHelper, &nodeData);

    }// for
  }// if (elementRules!= NULL)

  /* apply any remaining universal rules */
  if (universalRules)
  {
    TInt universalRulesSize = universalRules->Count();
    while(univRuleIndex < universalRulesSize)
    {
      CCSSSelector *entry = universalRules->At(univRuleIndex);
      ApplyEntryStylesL(aElementNode, entry, aBox, aEvent, aDomHelper, &nodeData);
      univRuleIndex++;
    }// while
  }// if
}

// -----------------------------------------------------------------------------
// CCSSRuleList::ApplyEntryStyles
// Applies styles for a given rule list entry after pattern matching
// -----------------------------------------------------------------------------
//
void CCSSRuleList::ApplyEntryStylesL(NW_DOM_ElementNode_t* aElementNode,
                                CCSSSelector* aEntry,
                                NW_LMgr_Box_t* aBox,
                                const NW_Evt_Event_t* aEvent,
                                NW_HED_DomHelper_t* aDomHelper,
                                CCSSNodeData* aNodeData)
{
  TText8* declarationList;
  TCSSParser parser;

  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aEntry != NULL);
  NW_ASSERT(aBox != NULL);
  NW_ASSERT(aDomHelper != NULL);

  /* first match pattern */
  iBufferList->InitializeParser(&parser, aEntry->iBufferIndex);
  CCSSPatternMatcher patternMatcher(iDictionary, iProcessor->owner, aDomHelper);

  TBool matched = patternMatcher.MatchPatternL(aElementNode, aEntry, aEvent, aNodeData);
  if (matched)
  {
    // initialize the parser to beginning of selector
    parser.SetPosition(aEntry->iSelector);

    TCSSReader* reader = parser.GetReader();
    // go to beginning of declaration list
    reader->GoToToken(LEFT_BRACE, &declarationList);

    /* pattern matches - apply styles */
    TCSSDeclListHandle::ApplyStylesL(aBox, &parser, iProcessor, aEntry->iBufferIndex );
  }
}

