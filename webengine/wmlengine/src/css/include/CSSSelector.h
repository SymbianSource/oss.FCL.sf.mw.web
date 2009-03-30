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
* Description:  Represent the CSS Selector
*
*/


#ifndef CCSSSelector_H
#define CCSSSelector_H

//  INCLUDES
//#include "CSSEventListener.h"
//#include "CSSBufferList.h"
//#include "nw_wbxml_dictionary.h"
//#include "nw_css_processori.h"
//#include "CSSPatternMatcher.h"

#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

// CLASS DECLARATION


/**
*  This class represents the CSS Condition
*  @lib css.lib
*  @since 2.1
*/
class CCSSCondition : public CBase
{
public:
  // constructor
	CCSSCondition(TUint8 aConditionType) 
    {  
      iConditionType = aConditionType;
      iValue1 = NULL;
      iValue2 = NULL;
    }

  // destructor
	virtual ~CCSSCondition()
	{
    if (iValue1)
    {
      NW_Mem_Free(iValue1);
    }
    if (iValue2)
    {
      NW_Mem_Free(iValue2);
    }
  }
  public: // New functions
        
  public:
 
  // condition type
  NW_Uint8 iConditionType;

  // value1 - in case of class and id it is the value of attribute
  // in case of attribute selector, it is attribute name
  NW_Ucs2 *iValue1;

  // NULL in case of id and class selectors, in case of attribute selector
  // it is the value of the attribute
  NW_Ucs2 *iValue2;
};

typedef CArrayPtrFlat<CCSSCondition> CCSSConditionList;

/**
*  This class represents the CSS Simple Selector
*  @lib css.lib
*  @since 2.1
*/
class CCSSSimpleSelector : public CBase
{
public:
  // constructor
	CCSSSimpleSelector(TUint16 aElementToken) 
    {  
      iElementToken = aElementToken;
      iConditionList = NULL;
    }

  // destructor
	virtual ~CCSSSimpleSelector()
	{
    if (iConditionList)
    {
      iConditionList->ResetAndDestroy();
      delete iConditionList;
    }
  }
  public: // New functions
        
  public:

  // element token
  TUint16 iElementToken;
  // array of rules that apply to element represented by iElementToken
  CCSSConditionList* iConditionList;
};

typedef CArrayPtrFlat<CCSSSimpleSelector> CCSSSimpleSelectorList;

// CLASS DECLARATION

/**
*  This class represents the CSS Selector
*
*  @lib css.lib
*  @since 2.1
*/
class CCSSSelector : public CBase
{
  public:  // Constructors and destructor
        
    // constructor
	  CCSSSelector(TText8* aSelector, TUint16 aSpecificity, TInt aBufferIndex) 
    {  
      iSelector = aSelector;
      iSpecificity = aSpecificity;
      iBufferIndex = aBufferIndex;
      iSimpleSelectorList = NULL;
    }

        // constructor
	  CCSSSelector() 
    {  
      iSelector = NULL;
      iSpecificity = 0;
      iBufferIndex = 0;
      iSimpleSelectorList = NULL;
    }


    /**
    * Destructor.
    */
    virtual ~CCSSSelector()
	  {
      if (iSimpleSelectorList)
      {
        iSimpleSelectorList->ResetAndDestroy();
        delete iSimpleSelectorList;
      }
    }

  public: // New functions

    // constructor
	  void Init(TText8* aSelector, TUint16 aSpecificity, TInt aBufferIndex) 
    {  
      iSelector = aSelector;
      iSpecificity = aSpecificity;
      iBufferIndex = aBufferIndex;
    }
    
  private:
 
  public:    // Data

    // pointer to selector start in the CSS buffer. This is not owned by this class
    TText8* iSelector;

    // selector specoficity
    TUint16 iSpecificity; 

    // index of the CSS buffer in the buffer lits
    TInt iBufferIndex;

    // simple selector list in the selector
    CCSSSimpleSelectorList* iSimpleSelectorList;
};

#endif /* CCSSRule_H */
