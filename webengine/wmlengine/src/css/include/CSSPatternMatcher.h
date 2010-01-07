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
* Description:  Matches CSS patterns
*
*/


#ifndef CCSSPatternMatcher_H
#define CCSSPatternMatcher_H

//  INCLUDES
#include "CSSParser.h"
#include "CSSSelector.h"
#include <nw_dom_element.h>
#include <nw_wbxml_dictionary.h>
#include "nw_evt_event.h"
#include "nw_hed_documentnode.h"
#include "nw_hed_domhelper.h"

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class contains information about the node
*  @lib css.lib
*  @since 2.1
*/
class CCSSNodeData : public CBase
{
public:
  // class Value of node
  NW_Ucs2* classVal;
  // idValue of node
  NW_Ucs2* idVal;
  // if node is link
  TBool isLink;
  // if associated url os visited
  TBool isVisited;
  // if url is cached
  TBool isCached;
  // associated url;
  NW_Text_t* elementUrl;

  // constructor
	CCSSNodeData() 
    {  
      classVal = NULL;
      idVal = NULL;
      elementUrl = NULL;
      isCached = EFalse;
      isLink = EFalse;
      isVisited = EFalse;
    }
  // destructor
	virtual ~CCSSNodeData()
	{
    if (classVal)
    {
      NW_Mem_Free(classVal);
    }
    if (idVal)
    {
      NW_Mem_Free(idVal);
    }
    if (elementUrl)
    {
      NW_Object_Delete(elementUrl);
    }
  }

};

// CLASS DECLARATION

/**
*  This class matches patterns
*
*  @lib css.lib
*  @since 2.1
*/
class CCSSPatternMatcher : public CBase
{
  public:  // Constructors and destructor
        
    /**
    * Destructor.
    */
    virtual ~CCSSPatternMatcher(){};

    CCSSPatternMatcher(NW_WBXML_Dictionary_t* aDictionary, 
                       NW_HED_DocumentNode_t* aOwner,
                       NW_HED_DomHelper_t* aDomHelper)
    {
      iDictionary = aDictionary;
      iOwner = aOwner;
      iDomHelper = aDomHelper;
    }


  public: // New functions
        
    /**
    * This function matches selector pattern to element node using lazy parsing
    * @since 2.1
    * @param aElementNode: element node for which pattern needs to be matched
    * @param aParser: parser initialized to beginning of selector
    * @param aSelector: selector to be matched
    * @param aEvent: event 
    * @param aNodeData: node specific data containing class, id information
    * @return NW_FALSE if pattern not matched or NW_TRUE if pattern matches
    */
    TBool MatchPatternLazyL(NW_DOM_ElementNode_t* aElementNode,
                        TCSSParser* aParser,
                        NW_Byte* aSelector,
                        const NW_Evt_Event_t* aEvent,
                        CCSSNodeData* aNodeData);
    /**
    * This function matches selector pattern to element node
    * @since 2.1
    * @param aElementNode: element node for which pattern needs to be matched
    * @param aParser: parser initialized to beginning of selector
    * @param aSelector: selector to be matched
    * @param aEvent: event 
    * @param aNodeData: node specific data containing class, id information
    * @return NW_FALSE if pattern not matched or NW_TRUE if pattern matches
    */
    TBool MatchPatternL(NW_DOM_ElementNode_t* aElementNode,
                        CCSSSelector* aSelector,
                        const NW_Evt_Event_t* aEvent,
                        CCSSNodeData* aNodeData);

  private: // New functions

    // data associated with a simple selector
    struct TSimpleSelectorData
    {
      TCSSReaderUnit nameSpace;
      TUint16 elementToken;
      TText8* conditionListHandle;
      TUint16 simpleSelectorNum;
    };

    /**
    * This function matches selector pattern to element node
    * using lazy parsing
    * @since 2.1
    * @param aElementNode: element node for which pattern needs to be matched
    * @param aParser: parser initialized to beginning of selector
    * @param aEvent: event 
    * @param aNodeData: node specific data containing class, id information
    * @param aConditionListHandle pointer to beginning of conditions
    * @return NW_FALSE if conditions not matched or NW_TRUE if conditions matches
    */
    TBool MatchConditionLazyL(NW_DOM_ElementNode_t* aElementNode, 
                            NW_Byte* aConditionListHandle,
                            TCSSParser* aParser,
                            const NW_Evt_Event_t* aEvent,
                            CCSSNodeData* aNodeData);

    /**
    * This function matches selector pattern to element node
    * @since 2.1
    * @param aElementNode: element node for which pattern needs to be matched
    * @param aParser: parser initialized to beginning of selector
    * @param aEvent: event 
    * @param aNodeData: node specific data containing class, id information
    * @param aConditionList: list of conditions
    * @return NW_FALSE if conditions not matched or NW_TRUE if conditions matches
    */
    TBool MatchConditionL(NW_DOM_ElementNode_t* aElementNode, 
                            CCSSConditionList* aConditionList,
                            const NW_Evt_Event_t* aEvent,
                            CCSSNodeData* aNodeData);

    /**
    * This function gets the data associated with a simple selector (iterator method)
    * @since 2.1
    * @param aParser: parser to parse selector
    * @param aSelector: selector to be matched
    * @param aEvent: event 
    * @param aNodeData(OUT): node specific data containing class, id information
    * @return KBrsrIterateDone (if all simple selectors are parsed)
    *         KBrsrIterateMore (if more simple selectors need to be parsed)
    */
    TBrowserStatusCode GetNextSimpleSelectorL(TCSSParser* aParser,
                                      NW_Byte* aSelector,
                                      TSimpleSelectorData& aData);


    /**
    * This function matches selector pattern to element node
    * @since 2.1
    * @param aElementNode: element node whose url is to be matched
    * @return url associated with href
    */
    NW_Text_t* GetElementURLL(NW_DOM_ElementNode_t* aElementNode);
    
    /**
    * This function returns true if url is in history stack
    * @since 2.1
    * @param aElementNode: element node whose url is to be searched
    * @return true (if url is in history stack), otherwise false
    */
    TBool UrlInHistory(NW_DOM_ElementNode_t* aElementNode);

    /**
    * This function returns true if url is in cache
    * @since 2.1
    * @param aElementNode: element node whose url is to be searched
    * @return true (if url is in cache), otherwise false
    */
    TBool UrlInCache(NW_DOM_ElementNode_t* aElementNode);

    /**
    * This function returns true if url is in cache
    * @since 2.1
    * @param aParser: parser to parse selector
    * @param aSelector: selector to be matched
    * @param aData: simpleselector data
    * @return KBrsrSuccess or KBrsrFailure
    */
    TBrowserStatusCode InitSimpleSelectorData(TCSSParser* aParser, NW_Byte* aSelector, TSimpleSelectorData& aData);

  private:    // Data

    // dictionary for document
    NW_WBXML_Dictionary_t* iDictionary;

    // document 
    NW_HED_DocumentNode_t* iOwner;

    // domHelper for resolving entities
    NW_HED_DomHelper_t* iDomHelper;
};

#endif /* CCSSPatternMatcher_H */
