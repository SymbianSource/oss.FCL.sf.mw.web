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
* Description:  Parses CSS styles
*
*/


#ifndef TCSSParser_H
#define TCSSParser_H

//  INCLUDES
#include "nw_object_dynamic.h"
#include "nw_css_export.h"
#include "CSSEventListener.h"
#include "CSSReader.h"
  
// CONSTANTS

// MACROS

// DATA TYPES

enum TCSSConditionType
{
  ID_CONDITION = 1,
  CLASS_CONDITION,
  ATTRIBUTE_CONDITION,
  ACTIVE_CONDITION,
  LINK_CONDITION,
  VISITED_CONDITION,
  FOCUS_CONDITION
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class is the parser for CSS
*  @lib css.lib
*  @since 2.1
*/
class TCSSParser
{
  public:
    // constructor
	  TCSSParser(TText8* aBuffer, TUint32 aLength, TUint32 aEncoding) 
      {  
        iReader.Init(aBuffer, aLength, aEncoding);
        iEventListener = NULL;
      }

    TCSSParser()
      {
        iEventListener = NULL;
      } 

    void Init(TText8* aBuffer, TUint32 aLength, TUint32 aEncoding) 
      {  
        iReader.Init(aBuffer, aLength, aEncoding);
        iEventListener = NULL;
      }

  public: // New functions

    TBrowserStatusCode ParseStyleSheet(MCSSEventListener* aEventListener);

    TBrowserStatusCode SimpleSelectorListIterate(TCSSSimpleSelector* aSimpleSelector);

    TBrowserStatusCode ConditionListIterate(TText8* aConditionType,
                                     TCSSReaderUnit *aValue1,
                                     TCSSReaderUnit *aValue2);

    TBrowserStatusCode DeclarationListIterate(TCSSReaderUnit* aProperty,
                                   TCSSPropertyVal* aPropertyVal,
                                   TUint8 *aNumVals,
                                   TUint8 maxVals,
                                   TBool* aImportant);


    TBrowserStatusCode ParserRGB(TCSSPropertyVal* aRVal,
                          TCSSPropertyVal* aGVal,
                          TCSSPropertyVal* aBVal);


    TUint32 ParseCharset();

    void SetPosition(TText8* aPosition);

    inline TUint32 GetEncoding(){return iReader.GetEncoding();}

    inline TCSSReader* GetReader() {return &iReader;}

    inline TText8* GetPosition(){ return iReader.GetBufferPointer();}

    private: //methods

      TBool ParseElementName(TCSSReaderUnit* aNameSpace, TCSSReaderUnit* aElementName);

      TBrowserStatusCode SelectorListIterate(TCSSReaderUnit* aTargetElement,
                                      TUint32 *aSpecificity);

      TBrowserStatusCode ParseRule();

      TBool HasValidMedia();

      TBrowserStatusCode ParseImportRule();

      TBrowserStatusCode ParseMediaRule();

      TBrowserStatusCode ParseStyleRules();

      TBrowserStatusCode ParseHeader();

      TBrowserStatusCode ParserPropertyVal(TCSSPropertyVal* aPropertyVal);

    private:    // Data

      // lexer
      TCSSReader iReader;
      // event listener to get the CSS events like start Selector
      MCSSEventListener* iEventListener;
};
#endif /* TCSSParser_H */
