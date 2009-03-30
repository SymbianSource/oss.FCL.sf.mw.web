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
* Description:  Map containing the CSS rules
*
*/


#ifndef CCSSRuleList_H
#define CCSSRuleList_H

//  INCLUDES
#include "CSSEventListener.h"
#include "CSSBufferList.h"
#include "nw_wbxml_dictionary.h"
#include "nw_css_processori.h"
#include "CSSPatternMatcher.h"
#include "CSSSelector.h"

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

// CLASS DECLARATION

typedef CArrayPtrFlat<CCSSSelector> CCSSRules;

/**
*  This class contains information about an element entry in the CSSRuleList (iElementList)
*  @lib css.lib
*  @since 2.1
*/
class CCSSRuleListElementEntry : public CBase
{
public:
  // constructor
  CCSSRuleListElementEntry(TUint16 aElementToken)
    {
      iElementToken = aElementToken;
      iRules = NULL;
    }

  // destructor
  virtual ~CCSSRuleListElementEntry()
    {
      if (iRules)
        {
          iRules->ResetAndDestroy();
          delete iRules;
        }
    }

  // element token
  TUint16 iElementToken;

  // array of rules that apply to element represented by iElementToken
  CCSSRules* iRules;
};




// CLASS DECLARATION

/**
*  This class contains the CSS Rules
*
*  @lib css.lib
*  @since 2.1
*/
class CCSSRuleList : public CBase, public MCSSEventListener
{
  public:  // Constructors and destructor

   /**
    * Two-phased constructor.
    * @param aProcessor A pointer to processor object that owns this object
    * @return CCSSHandler*
	  */
    static CCSSRuleList* NewL(NW_CSS_Processor_t* aProcessor);

    /**
    * Destructor.
    */
    virtual ~CCSSRuleList();

  public: // New functions

    /**
    * This function parses a stylesheet
    * @since 2.1
    * @param aData: Pointer to stylesheet
    * @param aLength: Length of stylesheet
    * @param aEncoding: encoding of stylesheet
    * @param aDoNotDelete: flag indicating if stylesheet shoud be deleted
    * @return KBrsrSuccess or KBrsrFailure
    */
    TBrowserStatusCode ProcessBuffer(TText8* aData,
                              TUint32 aLength,
                              TUint32 aEncoding,
                              TBool aDoNotDelete);

    /**
    * Returns the buffer list containing different stylesheets
    * @since 2.1
    * @return the buffer list
    */
    CCSSBufferList* GetBufferList();

    /**
    * Returns the size of the rules list. This corresponds to the number of
    * element entries in the iElementList
    * @since 2.1
    * @return the style list
    */
    TInt GetSize();

    /**
    * Returns the index of the buffer being parsed
    * @since 2.1
    * @return -1 in case of error otherwise index of stylesheet
    */
    TInt GetCurrentBufferIndex();

    /**
    * Returns dictionary
    * @since 2.1
    * @return the dictionary
    */
    NW_WBXML_Dictionary_t* GetDictionary();

    /**
    * Sets the index of the buffer being parsed
    * @since 2.1
    * @param aIndex - index of stylesheet
    */
    void SetCurrentBufferIndex(TInt aIndex);

    /**
    * Sets the dictionary
    * @since 2.1
    * @param aDictionary - dictionary
    */
    void SetDictionary(NW_WBXML_Dictionary_t* aDictionary);

  public: // Functions from base classes

    /**
    * Called when the CSS parser begins parsing the stylesheet
    * @since 2.1
    * @param aStorage: Pointer to stylesheet
    * @return void
    */
    void StartDocumentL(TText8* aStorage);

    /**
    * Called when the CSS parser ends parsing the stylesheet
    * @since 2.1
    * @param aStorage: Pointer to stylesheet
    * @return void
    */
    void EndDocumentL(TText8* aStorage);

    /**
    * This function parses a stylesheet
    * @since 2.1
    * @param aData: Pointer to stylesheet
    * @param aLength: Length of stylesheet
    * @param aEncoding: encoding of stylesheet
    * @param aDoNotDelete: flag indicating if stylesheet shoud be deleted
    * @return void
    */
    void SelectorL(TText8* aSelector,
                   TCSSReaderUnit *aTargetElement,
                   TUint32 aSpecificity);

    /**
    * Called when the CSS parser begins parsing the simple selector
    * @since 2.1
    * @param aTargetElement: target element to which this simple selector is applied
    * @return void
    */
    void SimpleSelectorStartL(TCSSReaderUnit *aTargetElement);

    /**
    * Called when the CSS parser ends parsing the simple selector
    * @since 2.1
    * @param aSimpleSelector: data of this simple selector
    * @param aFailed: ETrue is there was problem parsing the simpleselector
    * @return void
    */
    void SimpleSelectorEndL(TCSSSimpleSelector* aSimpleSelector, TBool aFailed);

     /**
    * Called when the CSS parser ends parsing the condition
    * @since 2.1
    * @param aValue1: value of condition
    * @param aValue2: name of condition
    * @return void
    */
   void ConditionL(TUint8 aConditionType,
                         TCSSReaderUnit *aValue1,
                         TCSSReaderUnit *aValue2);

    /**
    * Called when the CSS parser ends parsing the property
    * @since 2.1
    * @param aName: name of property
    * @param aPropertyVal: value of property
    * @param aImportant: flag indicating if this property is important or not
    * @return void
    */
    void PropertyL(TCSSReaderUnit* aName,
                         TCSSPropertyVal* aPropertyVal,
                         TBool aImportant);

    /**
    * Called when the CSS parser ends parsing the import url
    * @since 2.1
    * @param aUrl: url to be imported
    * @return KBrsrSuccess or KBrsrFailure
    */
    TBrowserStatusCode Import(TCSSReaderUnit* url);

    /**
    * Returns the encoding of the document being parsed
    * @since 2.1
    * @return encoding of the document being parsed
    */
    TUint32 GetCurrentEncoding();

    /**
    * Sets the encoding of the document being parsed
    * @since 2.1
    * @param aEncoding: encoding of the document being parsed
    */
    void SetCurrentEncoding(TUint32 aEncoding);

    /**
    * Applies styles for a given element node in the document after
    * pattern matching
    * @since 2.1
    * @param aElementNode: element node for which pattern matching needs to be done
    * @param aBox: box to which CSS is applied
    * @param aEvent: event resulting in style application (for e.g. focus)
    *                This is mostly NULL
    * @param aDomHelper: for resolving entities
    * @return KBrsrSuccess or KBrsrOutOfMemory
    */
    void ApplyStylesL(NW_DOM_ElementNode_t* aElementNode,
                      NW_LMgr_Box_t* aBox,
                      const NW_Evt_Event_t* aEvent,
                      NW_HED_DomHelper_t* aDomHelper);
  private:

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(NW_CSS_Processor_t* aProcessor);

    /**
    * Returns the set of rules that applies to a given element
    * @since 2.1
    * @param aToken: token of an element
    * @return the set of rules that applies to a given element
    */
    CCSSRules* GetElementRules(TUint16 aToken);

    /**
    * Returns the rule list in the array based on the index. If the index is
    * no longer valid (i.e. no more entries in the list) a NULL value is returned.
    * @param aIndex Index into array of rule lists.
    * @since 2.7
    * @return next rule list
    */
    CCSSRuleListElementEntry* GetElementRulesByIndex(TInt aIndex);

    /**
    * Applies styles for a given rule list entry after pattern matching
    * @since 2.1
    * @param aElementNode: element node for which pattern matching needs to be done
    * @param aEntry: rule list entry to be evaluated
    * @param aBox: box to which CSS is applied
    * @param aEvent: event resulting in style application (for e.g. focus)
    *                This is mostly NULL
    * @param aDomHelper: for resolving entities
    * @param aNodeData: contains class and id value for a given node
    * @return KBrsrSuccess or KBrsrOutOfMemory
    */
    void ApplyEntryStylesL(NW_DOM_ElementNode_t* aElementNode,
                           CCSSSelector* aEntry,
                           NW_LMgr_Box_t* aBox,
                           const NW_Evt_Event_t* aEvent,
                           NW_HED_DomHelper_t* aDomHelper,
                           CCSSNodeData* aNodeData);

  typedef CArrayPtrFlat<CCSSRuleListElementEntry> CCSSElementList;

  private:    // Data

    // element list of all styles
    CCSSElementList* iElementList;

    // buffer list containing different stylesheets
    CCSSBufferList* iBufferList;

    // document dictionary
    NW_WBXML_Dictionary_t* iDictionary;

    // index in the buffer list of the document being parsed
    TInt iCurrentBufferIndex;

    // encoding of the document being parsed
    TUint32 iCurrentEncoding;

    // processor whose styles need to be applied
    NW_CSS_Processor_t* iProcessor;

    // represents the current selector being parsed
    CCSSSelector* iSelector;

    //Allow friend class access to private data and methods
    friend class CCSSVariableStyleSheet;
};

#endif /* CCSSRuleList_H */
