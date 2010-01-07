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
* Description:  Applies CSS property 
*
*/

#ifndef TCSSDECLLISTHANDLE_H
#define TCSSDECLLISTHANDLE_H

//  INCLUDES

#include "nw_lmgr_propertylist.h"
#include "CSSParser.h"
#include "CSSPropertyTable.h"
#include <nw_dom_element.h>
#include "nw_css_processor.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class applies the CSS property for a declaration list 
* For e.g {color:blue, text-decoration: underline} is a declaration list
*
*  @lib css.lib
*  @since 2.1
*/
class TCSSDeclListHandle
  {
   public:  
  
    /* Also invoked by NW_CSS_DefaultStyleSheet */
    static void ApplyPropertyL(NW_LMgr_PropertyName_t aPropName, 
                                   NW_LMgr_Property_t *aProps,
                                   TUint8 aNumVals,
                                   TBool aImportant,
                                   NW_LMgr_PropertyList_t* aAppliedList,
                                   NW_LMgr_Box_t* aBox,
                                   TBool aDefaultStyle);

    /* invoked by - NW_CSS_RuleList_ApplyStyles or NW_XHTML_ElementHandler_ApplyStyles
    This method goes through a declaration list (e.g {color:blue;margin:1px})
    and applies the styles to the propList
    */
    static void ApplyStylesL(NW_LMgr_Box_t* aBox,
                                   TCSSParser* aParser,
                                   NW_CSS_Processor_t* aProcessor,
                                   TInt aBufferIndex);
   private:

     static void ApplyBorderPropertyL(NW_LMgr_PropertyName_t aPropName, 
                                       NW_LMgr_Property_t *aPropertyVals,
                                       TUint8 aNumVals,
                                       TBool aImportant,
                                       NW_LMgr_PropertyList_t* aAppliedList,
                                       NW_LMgr_Box_t* aBox,
                                       TBool DefaultStyle);

    static void ApplyBoxModelPropertyL(NW_LMgr_PropertyName_t aPropName, 
                           const NW_LMgr_PropertyName_t *aProps,
                           NW_LMgr_Property_t *aPropertyVals,
                           TUint8 aNumVals,
                           TBool aImportant,
                           NW_LMgr_PropertyList_t* aAppliedList,
                           NW_LMgr_Box_t* aBox,
                           TBool aDefaultStyle);

    static void ApplyCommonPropertyL(NW_LMgr_PropertyName_t aPropName, 
				                   NW_LMgr_Property_t *aProperty,
				                   TBool aImportant,
				                   NW_LMgr_PropertyList_t* aAppliedList,
				                   NW_LMgr_Box_t* aBox,
                           TBool aDefaultStyle);

    static TBool ConvertValueL(NW_LMgr_PropertyName_t aPropName, 
                           TCSSPropertyVal *aInValue,
                           NW_LMgr_Property_t *aOutProp,
                           TCSSParser* aParser,
                           NW_CSS_Processor_t* aProcessor,
                           NW_LMgr_Box_t *aBox,
                           TInt aBufferIndex);

    static void ApplyFontPropertyL(NW_LMgr_Property_t *aProps,
                           TUint8 aNumVals,
                           TBool aImportant,
                           NW_LMgr_PropertyList_t* aAppliedList,
                           NW_LMgr_Box_t* aBox,
                           TBool aDefaultStyle);

    static void ApplyListStylePropertyL(NW_LMgr_Property_t *aProps,
                           TUint8 aNumVals,
                           TBool aImportant,
                           NW_LMgr_PropertyList_t* aAppliedList,
                           NW_LMgr_Box_t* aBox,
                           TBool aDefaultStyle);

    static void ApplyBackgroundPropertyL(NW_LMgr_PropertyName_t aPropName, 
                               NW_LMgr_Property_t *aProps,
                               TUint8 aNumVals,
                               TBool aImportant,
                               NW_LMgr_PropertyList_t* aAppliedList,
                               NW_LMgr_Box_t* aBox,
                               TBool aDefaultStyle);

};

#endif

