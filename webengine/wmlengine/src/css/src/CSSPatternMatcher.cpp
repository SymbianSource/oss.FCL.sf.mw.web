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
* Description:  Matches CSS Pattern
*
*/

// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "CSSPatternMatcher.h"
#include <nw_dom_document.h>
#include "nw_evt_activateevent.h"
#include "nw_evt_focusevent.h"
#include "nwx_assert.h"
#include "nwx_string.h"

#include "nw_hed_contenthandler.h"
#include "nw_hed_documentroot.h"
#include "nw_hed_historyvisitor.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "BrsrStatusCodes.h"
#include <MemoryManager.h>

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
// CCSSPatternMatcher::GetNextSimpleSelectorL
// This function gets the data associated with a simple selector (iterator method)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CCSSPatternMatcher::GetNextSimpleSelectorL(TCSSParser* aParser,
                                          NW_Byte* aSelector,
                                          TSimpleSelectorData& aData)
{
  TCSSSimpleSelector simpleSelector;
  NW_Uint32 index;

  NW_ASSERT(aParser != NULL);
  NW_ASSERT(aSelector != NULL);

  if (aData.simpleSelectorNum == 0)
  {
    return KBrsrIterateDone;
  }
  index = 0;
  aParser->SetPosition(aSelector);
  while (aParser->SimpleSelectorListIterate(&simpleSelector) == KBrsrIterateMore)
  {
    index++;
    if (index == aData.simpleSelectorNum)
      break;    
  }
  aData.elementToken = 0;
  aData.simpleSelectorNum--;
  /*nameSpace = simpleSelector.nameSpace; - deal later */
  aData.conditionListHandle = simpleSelector.conditionListHandle;
  if (simpleSelector.elementName.iLength != 0)
  {
    NW_Ucs2* ucs2Str = simpleSelector.elementName.GetUnicodeL(aParser->GetEncoding());
    aData.elementToken = (NW_Uint16)NW_WBXML_Dictionary_getTagToken(iDictionary,(NW_String_UCS2Buff_t *)ucs2Str, NW_FALSE);

    NW_Mem_Free(ucs2Str);
  }
  return KBrsrIterateMore;
}


// -----------------------------------------------------------------------------
// CCSSPatternMatcher::GetElementURLL
// This function gets the url associated with the element node
// -----------------------------------------------------------------------------
//
NW_Text_t*
CCSSPatternMatcher::GetElementURLL(NW_DOM_ElementNode_t* aElementNode)
{
  NW_String_t attrVal;
  NW_Text_t *retUrl = NULL;
 
  NW_ASSERT(aElementNode != NULL);

  /* Dom Helper always returns attribute as ucs2 */
  TBrowserStatusCode status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper, aElementNode, NW_XHTML_AttributeToken_href, &attrVal);  
 
  if (status == KBrsrSuccess)
  {
    /* Attribute storage is handed off to url text */
    NW_Text_t *url = NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2*)attrVal.storage, 
                       NW_Str_Strlen((NW_Ucs2*)attrVal.storage), NW_TRUE);

    if (!url)
    {
      User::Leave(KErrNoMemory);
    }

    /* resolve the URL object */
    status = NW_HED_ContentHandler_ResolveURL (iOwner, url, &retUrl);
    if (status != KBrsrSuccess)
    {
      NW_Object_Delete(url);
      return NULL;
    }
    return retUrl;
  }
  return NULL;
}


// -----------------------------------------------------------------------------
// CCSSPatternMatcher::UrlInCache
// This function checks if url is in cache
// -----------------------------------------------------------------------------
//
TBool
CCSSPatternMatcher::UrlInCache(NW_DOM_ElementNode_t* aElementNode)
{
  NW_Text_t *url;
  TBool inCache = EFalse;

  NW_ASSERT(aElementNode != NULL);

  url = GetElementURLL(aElementNode);
  if (url)
  {
    NW_Bool freeNeeded;
    NW_Ucs2* ucs2AttrVal1 = NULL;

    ucs2AttrVal1 = NW_Text_GetUCS2Buffer(url, 
                                         NW_Text_Flags_Aligned | NW_Text_Flags_NullTerminated,
                                         NULL, 
                                         &freeNeeded);
    if (ucs2AttrVal1 != NULL) 
    {
//R->ul      inCache = UrlLoader_IsUrlInCache(ucs2AttrVal1);
    }
  
    if (freeNeeded)
    {
      NW_Mem_Free(ucs2AttrVal1);
    }
    NW_Object_Delete(url);
  }
    
  return inCache;          
}


/* Match a list words (separated by space) against a pattern */
static
NW_Bool
NW_CSS_PatternMatcher_MatchWords(NW_Ucs2 * words, NW_Ucs2* pattern)
{
  const NW_Ucs2 *pStart = NULL;
  const NW_Ucs2 *pSpace = NULL;
  
  NW_ASSERT(words != NULL);
  NW_ASSERT(pattern != NULL);

  NW_Bool retVal = NW_TRUE;
  pStart = words;
 
  pSpace = NW_Str_Strchr(pStart, WAE_ASCII_SPACE);

  if (pSpace != NULL) {
    NW_Ucs2 *pParttern = NW_Str_Substr(pStart, 0, pSpace - pStart);
    if (!NW_Str_Stricmp(pattern, pParttern))
    {
      retVal = NW_TRUE;
    }
    else
    {
      retVal = NW_CSS_PatternMatcher_MatchWords((NW_Ucs2*)pSpace + 1, pattern);
    }
    NW_Mem_Free (pParttern);
    return retVal;
  }
  else {
    if (!NW_Str_Stricmp(pattern, words))
      return NW_TRUE;
  }
  return NW_FALSE;
}

// -----------------------------------------------------------------------------
// CCSSPatternMatcher::MatchConditionL
// This function matches condition list
// -----------------------------------------------------------------------------
//
TBool 
CCSSPatternMatcher::MatchConditionLazyL(NW_DOM_ElementNode_t* aElementNode, 
                                    NW_Byte* aConditionListHandle,
                                    TCSSParser* aParser,
                                    const NW_Evt_Event_t* aEvent,
                                    CCSSNodeData* aNodeData)
{
  TBrowserStatusCode status;
  NW_Uint8 conditionType;
  TCSSReaderUnit value1;
  TCSSReaderUnit value2;
  NW_String_t str;
  NW_String_t attrVal;
  NW_Uint16 attributeToken;
  NW_Ucs2* ucs2AttrVal1 = NULL;
  NW_Ucs2* ucs2AttrVal2 = NULL;
  NW_Ucs2* ucs2AttrName = NULL;
  TBool equals = NW_FALSE;

  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aConditionListHandle != NULL);
  NW_ASSERT(aParser != NULL);

  aParser->SetPosition(aConditionListHandle);
  while (aParser->ConditionListIterate(&conditionType, &value1, &value2) == KBrsrIterateMore)
  {
    attributeToken = 0;
    equals = EFalse;

    switch(conditionType)
      {
      case CLASS_CONDITION:
      { 
        if ((aNodeData == NULL) || aNodeData->classVal == NULL)
        { 
          /* Get the attribute from the dom. Dom helper always converts attribute to ucs2 */
          status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper,
                                                       aElementNode, 
                                                       NW_XHTML_AttributeToken_class, 
                                                       &attrVal);             
          if (status != KBrsrSuccess)
          {
            return EFalse;
          }
      
          ucs2AttrVal1 = (NW_Ucs2 *)attrVal.storage; 
          if (aNodeData)
          {
            aNodeData->classVal = ucs2AttrVal1;
          }
        }
        else 
        {
          ucs2AttrVal1 = aNodeData->classVal;
        }
        ucs2AttrVal2 = value1.GetUnicodeL(aParser->GetEncoding());

        if (NULL != NW_CSS_PatternMatcher_MatchWords(ucs2AttrVal1, ucs2AttrVal2))
        {
          equals = ETrue;
        }

        NW_Mem_Free(ucs2AttrVal2);
        break;
      }
        
      case ID_CONDITION:

        if ((aNodeData == NULL) || aNodeData->idVal == NULL)
        {
          /* Get the attribute from the dom. Dom helper always converts attribute to ucs2 */
          status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper,
                                                           aElementNode, 
                                                           NW_XHTML_AttributeToken_id, 
                                                           &attrVal);             
          if (status != KBrsrSuccess){
            return EFalse;
          }
      
          ucs2AttrVal1 = (NW_Ucs2 *)attrVal.storage; 
          if (aNodeData)
          {
            aNodeData->idVal = ucs2AttrVal1;
          }
        }
        else 
        {
          ucs2AttrVal1 = aNodeData->idVal;
        }
        ucs2AttrVal2 = value1.GetUnicodeL(aParser->GetEncoding());

        /* Compare the strings */
        if ( (NW_Str_Strlen(ucs2AttrVal1) == NW_Str_Strlen(ucs2AttrVal2)) &&
             (!NW_Mem_memcmp(ucs2AttrVal1, ucs2AttrVal2, NW_Str_Strlen(ucs2AttrVal1)*2)) )
        {
          equals = ETrue;
        }

        NW_Mem_Free(ucs2AttrVal2);
        break;
        
      case ATTRIBUTE_CONDITION:
        ucs2AttrName = value1.GetUnicodeL(aParser->GetEncoding());
        if (ucs2AttrName == NULL) 
        {
          return NW_FALSE;
        }
        str.storage = (NW_Byte*)ucs2AttrName;
        str.length = NW_Str_Strlen(ucs2AttrName);
        attributeToken = (NW_Uint16)NW_WBXML_Dictionary_getAttributeToken(iDictionary, &str, HTTP_iso_10646_ucs_2, NW_FALSE);

        /* Get the attribute from the dom. Dom helper always converts attribute to ucs2 */
        status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper,
                                                     aElementNode, 
                                                     attributeToken, 
                                                     &attrVal);             
        if (status != KBrsrSuccess)
        {
          NW_Str_Delete(ucs2AttrName);
          NW_String_deleteStorage(&attrVal);
          return EFalse;
        }
      
        ucs2AttrVal1 = (NW_Ucs2 *)attrVal.storage;
        CleanupStack::PushL(ucs2AttrName);
        ucs2AttrVal2 = value2.GetUnicodeL(aParser->GetEncoding());
        CleanupStack::Pop(ucs2AttrName);

        /* Compare the strings */
        if ( (NW_Str_Strlen(ucs2AttrVal1) == NW_Str_Strlen(ucs2AttrVal2)) &&
             !NW_Str_Stricmp(ucs2AttrVal1, ucs2AttrVal2) )
        {
          equals = ETrue;
        }

        NW_String_deleteStorage(&attrVal);
        NW_Mem_Free(ucs2AttrVal2);
        NW_Mem_Free(ucs2AttrName);
        break;
        
      case ACTIVE_PSEUDO_CLASS:
        if (!NW_Object_IsInstanceOf(aEvent, &NW_Evt_ActivateEvent_Class)){
          return NW_FALSE;
        }
        return EFalse;
        
      case FOCUS_PSEUDO_CLASS:
        if (NW_Object_IsInstanceOf(aEvent, &NW_Evt_FocusEvent_Class)){
          if (NW_Evt_FocusEvent_GetHasFocus(aEvent)){
            return ETrue;
          }
          return EFalse;
        }
        return EFalse;
        
      case LINK_PSEUDO_CLASS:
        {
          TBool isVisited = NW_FALSE;
          NW_Text_t* elementUrl = NULL; 

          if (aNodeData)
          {
            if (aNodeData->elementUrl == NULL)
              aNodeData->elementUrl = GetElementURLL(aElementNode);  
            elementUrl = aNodeData->elementUrl;
          }
          else
            elementUrl = GetElementURLL(aElementNode); 
          
          if (elementUrl == NULL)
            return EFalse;

          if (aNodeData)
          {
            if (aNodeData->isVisited)
            return NW_FALSE;
            if (aNodeData->isLink)
              return ETrue;
          }
          
          /* 
            Check if url has been visited - if yes return FALSE else return TRUE 
            "Visited" means that the link is either in the cache or in the history list
          */
          /* In the cache? */  
          isVisited = UrlInCache(aElementNode);
          if (aNodeData)
          {
            aNodeData->isVisited = isVisited;
            aNodeData->isCached = aNodeData->isVisited;         
            if (!aNodeData->isVisited)
              aNodeData->isLink = ETrue;
            return aNodeData->isLink;
          }
          if (isVisited)
            return EFalse;
          else
            return ETrue;
        }

      case VISITED_PSEUDO_CLASS:
        {
          TBool isVisited = NW_FALSE;
          
          if (aNodeData)
          {
            if (aNodeData->isLink)
              return EFalse;
            if (aNodeData->isVisited)
              return ETrue;
          }

          isVisited = UrlInCache(aElementNode);
          if (aNodeData)
          {
            aNodeData->isVisited = isVisited;
            aNodeData->isCached = aNodeData->isVisited;         
            if (!aNodeData->isVisited)
              aNodeData->isLink = ETrue;
            return aNodeData->isVisited;
          }
          return isVisited;
        }

      case CACHED_PSEUDO_CLASS:
        {
          return UrlInCache(aElementNode);
        }

      default:
        return NW_FALSE;
    } 

    if (!equals) {
      break;
      }
    }

  return equals;
}

// -----------------------------------------------------------------------------
// InitSimpleSelectorData
// Initializes SimpleSelectorData
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CCSSPatternMatcher::InitSimpleSelectorData(TCSSParser* aParser, NW_Byte* aSelector, TSimpleSelectorData& aData)
{
  TCSSSimpleSelector simpleSelector;
  TBrowserStatusCode status;

  NW_ASSERT(aParser != NULL);
  NW_ASSERT(aSelector != NULL);

  aData.simpleSelectorNum = 0;
  aData.conditionListHandle = NULL;
  aData.elementToken = 0;
  aData.nameSpace.Init(NULL, 0, 0);

  aParser->SetPosition(aSelector);
  while ((status = aParser->SimpleSelectorListIterate(&simpleSelector))
          == KBrsrIterateMore)
  {
    aData.simpleSelectorNum++;
  }
  if (status != KBrsrIterateDone){
    return KBrsrFailure;
  }
  return KBrsrSuccess;
}



// -----------------------------------------------------------------------------
// CCSSPatternMatcher::MatchPatternL
// This function matches pattern
// -----------------------------------------------------------------------------
//
TBool 
CCSSPatternMatcher::MatchPatternLazyL(NW_DOM_ElementNode_t* aElementNode,
                                  TCSSParser* aParser,
                                  NW_Byte* aSelector,
                                  const NW_Evt_Event_t* aEvent,
                                  CCSSNodeData* aNodeData)
{
  TBrowserStatusCode status;
  TSimpleSelectorData data;

  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aParser != NULL);
  NW_ASSERT(aSelector != NULL);

  status = InitSimpleSelectorData(aParser, aSelector, data);
  if (status!= KBrsrSuccess){
    return EFalse;
  }

  status = GetNextSimpleSelectorL(aParser, aSelector, data);
  if (data.conditionListHandle != NULL)
  {
    TBool matched = MatchConditionLazyL(aElementNode, data.conditionListHandle, aParser, aEvent, aNodeData);
    if (!matched)
    {
      return EFalse;
    }
  }
  while ((status = GetNextSimpleSelectorL(aParser, aSelector, data)) == KBrsrIterateMore)
  {
    NW_Uint16 elementToken;
    
    if (data.elementToken != 0)
    { /* match ancestor */
      do
      {
        aElementNode = NW_DOM_Node_getParentNode(aElementNode);
        if (aElementNode == NULL)
        {
          return EFalse;
        }
        elementToken = NW_DOM_ElementNode_getTagToken(aElementNode);
      }while (data.elementToken != elementToken);
    }
    /* empty data == wildcard selector. Go up a generation before starting the
      ancestor match */
    else
    {
      aElementNode = NW_DOM_Node_getParentNode(aElementNode);
      if (aElementNode == NULL)
      {
        return EFalse;
      }
    }

    if (data.conditionListHandle != NULL)
    {      
      TBool matched = MatchConditionLazyL(aElementNode, data.conditionListHandle, aParser, aEvent, NULL);
      if (!matched)
      {
        return EFalse;
      }
    }
  }
  if (status != KBrsrIterateDone){
    return EFalse;
  }
  return ETrue;
}


// -----------------------------------------------------------------------------
// CCSSPatternMatcher::MatchPatternL
// This function matches pattern
// -----------------------------------------------------------------------------
//
TBool 
CCSSPatternMatcher::MatchPatternL(NW_DOM_ElementNode_t* aElementNode,
                                  CCSSSelector* aSelector,
                                  const NW_Evt_Event_t* aEvent,
                                  CCSSNodeData* aNodeData)
{
  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aSelector != NULL);

  TInt size = aSelector->iSimpleSelectorList->Count();
  TInt index = size-1;
  CCSSSimpleSelector* simpleSelector = aSelector->iSimpleSelectorList->At(index);

  if (simpleSelector->iConditionList != NULL)
  {
    TBool matched = MatchConditionL(aElementNode, simpleSelector->iConditionList, 
                                    aEvent, aNodeData);
    if (!matched)
    {
      return EFalse;
    }
  }
  while (index > 0)
  {
    index--;
    simpleSelector = aSelector->iSimpleSelectorList->At(index);
    TUint16 elementToken = 0;
    
    if (simpleSelector->iElementToken != 0)
    { /* match ancestor */
      do
      {
        aElementNode = NW_DOM_Node_getParentNode(aElementNode);
        if (aElementNode == NULL)
        {
          return EFalse;
        }
        elementToken = NW_DOM_ElementNode_getTagToken(aElementNode);
      }while (simpleSelector->iElementToken != elementToken);
    }
    /* empty data == wildcard selector. Go up a generation before starting the
      ancestor match */
    else
    {
      aElementNode = NW_DOM_Node_getParentNode(aElementNode);
      if (aElementNode == NULL)
      {
        return EFalse;
      }
    }

    if (simpleSelector->iConditionList != NULL)
    {      
      TBool matched = MatchConditionL(aElementNode, simpleSelector->iConditionList,
                                      aEvent, NULL);
      if (!matched)
      {
        return EFalse;
      }
    }
  }
  if (index != 0){
    return EFalse;
  }
  return ETrue;
}

// -----------------------------------------------------------------------------
// CCSSPatternMatcher::MatchConditionL
// This function matches condition list
// -----------------------------------------------------------------------------
//
TBool 
CCSSPatternMatcher::MatchConditionL(NW_DOM_ElementNode_t* aElementNode, 
                                    CCSSConditionList* aConditionList,
                                    const NW_Evt_Event_t* aEvent,
                                    CCSSNodeData* aNodeData)
{
  TBrowserStatusCode status;
  NW_String_t str;
  NW_String_t attrVal;
  NW_Uint16 attributeToken;
  NW_Ucs2* ucs2AttrVal1 = NULL;
  TBool equals = NW_FALSE;
  TInt index = 0;
  TInt numConditions;

  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aConditionList != NULL);

  numConditions = aConditionList->Count();

  while (index < numConditions)
  {
    CCSSCondition* condition = aConditionList->At(index);

    index++;
    attributeToken = 0;
    equals = EFalse;

    switch(condition->iConditionType)
      {
      case CLASS_CONDITION:
      { 
        if ((aNodeData == NULL) || aNodeData->classVal == NULL)
        { 
          /* Get the attribute from the dom. Dom helper always converts attribute to ucs2 */
          status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper,
                                                       aElementNode, 
                                                       NW_XHTML_AttributeToken_class, 
                                                       &attrVal);             
          if (status != KBrsrSuccess)
          {
            return EFalse;
          }
      
          ucs2AttrVal1 = (NW_Ucs2 *)attrVal.storage; 
          if (aNodeData)
          {
            aNodeData->classVal = ucs2AttrVal1;
          }
        }
        else 
        {
          ucs2AttrVal1 = aNodeData->classVal;
        }

        if (NULL != NW_CSS_PatternMatcher_MatchWords(ucs2AttrVal1, condition->iValue1))
        {
          equals = ETrue;
        }

        // Free ucs2AttrVal1 if it wasn't adopted by aNodeData
        if ((aNodeData == NULL) || (ucs2AttrVal1 != aNodeData->classVal))
        {
          NW_Mem_Free(ucs2AttrVal1);
          ucs2AttrVal1 = NULL;
        }
        break;
      }
        
      case ID_CONDITION:

        if ((aNodeData == NULL) || aNodeData->idVal == NULL)
        {
          /* Get the attribute from the dom. Dom helper always converts attribute to ucs2 */
          status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper,
                                                           aElementNode, 
                                                           NW_XHTML_AttributeToken_id, 
                                                           &attrVal);             
          if (status != KBrsrSuccess){
            return EFalse;
          }
      
          ucs2AttrVal1 = (NW_Ucs2 *)attrVal.storage; 
          if (aNodeData)
          {
            aNodeData->idVal = ucs2AttrVal1;
          }
        }
        else 
        {
          ucs2AttrVal1 = aNodeData->idVal;
        }

        /* Compare the strings */
        if ( (NW_Str_Strlen(ucs2AttrVal1) == NW_Str_Strlen(condition->iValue1)) &&
             (!NW_Mem_memcmp(ucs2AttrVal1, condition->iValue1, NW_Str_Strlen(ucs2AttrVal1)*2)) )
        {
          equals = ETrue;
        }

        // Free ucs2AttrVal1 if it wasn't adopted by aNodeData
        if ((aNodeData == NULL) || (ucs2AttrVal1 != aNodeData->idVal))
        {
          NW_Mem_Free(ucs2AttrVal1);
          ucs2AttrVal1 = NULL;
        }
        break;
        
      case ATTRIBUTE_CONDITION:
        if (condition->iValue1 == NULL) {
          return EFalse;
        }
        str.storage = (NW_Byte*)condition->iValue1;
        str.length = NW_Str_Strlen(condition->iValue1);
        attributeToken = (NW_Uint16)NW_WBXML_Dictionary_getAttributeToken(iDictionary, &str, HTTP_iso_10646_ucs_2, NW_FALSE);

        /* Get the attribute from the dom. Dom helper always converts attribute to ucs2 */
        status = NW_HED_DomHelper_GetHTMLAttributeValue (iDomHelper,
                                                     aElementNode, 
                                                     attributeToken, 
                                                     &attrVal);             
        if (status != KBrsrSuccess)
        {
          NW_String_deleteStorage(&attrVal);
          return EFalse;
        }
      
        ucs2AttrVal1 = (NW_Ucs2 *)attrVal.storage; 
        /* Compare the strings */
        if ( (condition->iValue2) &&
             (NW_Str_Strlen(ucs2AttrVal1) == NW_Str_Strlen(condition->iValue2)) &&
              !NW_Str_Stricmp(ucs2AttrVal1, condition->iValue2) )
        {
          equals = ETrue;
        }

        NW_String_deleteStorage(&attrVal);
        break;
        
      case ACTIVE_PSEUDO_CLASS:
        return EFalse;
        
      case FOCUS_PSEUDO_CLASS:
        if (NW_Object_IsInstanceOf(aEvent, &NW_Evt_FocusEvent_Class)){
          if (NW_Evt_FocusEvent_GetHasFocus(NW_Evt_FocusEventOf(aEvent))){
            return ETrue;
          }
        }
        return EFalse;
        
      case LINK_PSEUDO_CLASS:
        {
          TBool isVisited = NW_FALSE;
          NW_Text_t* elementUrl = NULL; 

          if (aNodeData)
          {
            if (aNodeData->elementUrl == NULL)
              aNodeData->elementUrl = GetElementURLL(aElementNode);  
            elementUrl = aNodeData->elementUrl;
          }
          else
            elementUrl = GetElementURLL(aElementNode); 
          
          if (elementUrl == NULL)
            return EFalse;

          if (aNodeData)
          {
            if (aNodeData->isVisited)
            return NW_FALSE;
            if (aNodeData->isLink)
              return ETrue;
          }
          
          /* 
            Check if url has been visited - if yes return FALSE else return TRUE 
            "Visited" means that the link is either in the cache or in the history list
          */
          /* In the cache? */  
          isVisited = UrlInCache(aElementNode);
          if (aNodeData)
          {
            aNodeData->isVisited = isVisited;
            aNodeData->isCached = aNodeData->isVisited;         
            if (!aNodeData->isVisited)
              aNodeData->isLink = ETrue;
            return aNodeData->isLink;
          }
          if (isVisited)
            return EFalse;
          else
            return ETrue;
        }

      case VISITED_PSEUDO_CLASS:
        {
          TBool isVisited = NW_FALSE;
          
          if (aNodeData)
          {
            if (aNodeData->isLink)
              return EFalse;
            if (aNodeData->isVisited)
              return ETrue;
          }

          isVisited = UrlInCache(aElementNode);
          if (aNodeData)
          {
            aNodeData->isVisited = isVisited;
            aNodeData->isCached = aNodeData->isVisited;         
            if (!aNodeData->isVisited)
              aNodeData->isLink = ETrue;
            return aNodeData->isVisited;
          }
          return isVisited;
        }

      case CACHED_PSEUDO_CLASS:
        {
          return UrlInCache(aElementNode);
        }

      default:
        return NW_FALSE;
    } 

    if (!equals) {
      break;
      }
    }

  return equals;
}
