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
* Description:  Listens to callback events from CSS parser
*
*/



#ifndef MCSSEVENTLISTENER_H
#define MCSSEVENTLISTENER_H

// INCLUDES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "CSSReader.h"

struct TCSSSimpleSelector
{
  TCSSReaderUnit nameSpace;
  TCSSReaderUnit elementName;
  TUint32 specificity;
  TText8* conditionListHandle;
};

struct TCSSPropertyVal
{
  TCSSReaderUnit val;
  TInt8 type;
  TText8* next;
};

// CLASS DECLARATION

/**
*  Interface for callback events from CSS parser
*
*  @lib css.lib
*  @since 2.1
*/
class MCSSEventListener
{
  public: // New functions

    /**
    * Called when the CSS parser begins parsing the stylesheet 
    * @since 2.1
    * @param aStorage: Pointer to stylesheet
    * @return void
    */
    virtual void StartDocumentL(TText8* aStorage) = 0;

    /**
    * Called when the CSS parser ends parsing the stylesheet 
    * @since 2.1
    * @param aStorage: Pointer to stylesheet
    * @return void
    */
    virtual void EndDocumentL(TText8* aStorage) = 0;

    /**
    * Called when the CSS parser finishes parsing the selector 
    * @since 2.1
    * @param aSelector: Pointer to beginning of selector
    * @param aTargetElement: target element to which this selector is applied
    * @param aSpecificity: specificity of selector
    * @return void
    */
    virtual void SelectorL(TText8* aSelector, 
                                 TCSSReaderUnit *aTargetElement,
                                 TUint32 aSpecificity) = 0;

    /**
    * Called when the CSS parser begins parsing the simple selector 
    * @since 2.1
    * @param aTargetElement: target element to which this simple selector is applied
    * @return void
    */
    virtual void SimpleSelectorStartL(TCSSReaderUnit *aTargetElement) = 0;

    /**
    * Called when the CSS parser ends parsing the simple selector 
    * @since 2.1
    * @param aSimpleSelector: data of this simple selector
    * @param aFailed: ETrue is there was problem parsing the simpleselector
    * @return void
    */
    virtual void SimpleSelectorEndL(TCSSSimpleSelector* aSimpleSelector, TBool aFailed) = 0;

    /**
    * Called when the CSS parser ends parsing the condition 
    * @since 2.1
    * @param aValue1: value of condition
    * @param aValue2: name of condition
    * @return void
    */
    virtual void ConditionL(TUint8 aConditionType, 
                                  TCSSReaderUnit *aValue1,
                                  TCSSReaderUnit *aValue2) = 0;

    /**
    * Called when the CSS parser ends parsing the property 
    * @since 2.1
    * @param aName: name of property
    * @param aPropertyVal: value of property
    * @param aImportant: flag indicating if this property is important or not
    * @return void
    */
    virtual void PropertyL(TCSSReaderUnit* aName,  
                                 TCSSPropertyVal* aPropertyVal,
                                 TBool aImportant) = 0;

    /**
    * Called when the CSS parser ends parsing the import url 
    * @since 2.1
    * @param aUrl: url to be imported
    * @return KBrsrSuccess or KBrsrFailure
    */
    virtual TBrowserStatusCode Import(TCSSReaderUnit* aUrl) = 0;

    /**
    * Returns the encoding of the document being parsed
    * @since 2.1
    * @return encoding of the document being parsed
    */
    virtual TUint32 GetCurrentEncoding() = 0;

    /**
    * Sets the encoding of the document being parsed
    * @since 2.1
    * @param aEncoding: encoding of the document being parsed
    */
    virtual void SetCurrentEncoding(TUint32 aEncoding) = 0;
  };

#endif
