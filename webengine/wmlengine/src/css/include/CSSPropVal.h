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
* Description:  Applies CSS property values
*
*/


#ifndef TCSSPROPVAL_H
#define TCSSPROPVAL_H

//  INCLUDES

#include "CSSPropertyTable.h"
#include "nw_css_processor.h"
#include "nw_lmgr_box.h"
#include "CSSParser.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class applies the CSS property values
*
*  @lib css.lib
*  @since 2.1
*/
class TCSSPropVal
  {
   public:

    static TBool SetColorL(TCSSParser* aParser,
                                NW_LMgr_Property_t *aBoxVal,
                                TCSSPropertyVal* aPropertyVal,
                                TText16* aValStorage);

    static TBool SetNumberPropVal(NW_LMgr_PropertyName_t aPropName,
                                  NW_LMgr_Property_t *aProp,
                                  TUint8 aValueType,
                                  NW_Float32 aValue);

    static TBool SetImageValL(NW_LMgr_PropertyName_t aPropName,
                              NW_LMgr_Property_t *aVal,
                              TText16* aValStorage,
                              NW_CSS_Processor_t* aProcessor,
                              NW_LMgr_Box_t *box,
                              TInt aBufferIndex);

    static TBool SetTokenVal(NW_LMgr_PropertyName_t aPropName,
                                   NW_LMgr_Property_t *aProp,
                                   TUint32 aToken,
                                   NW_LMgr_Box_t* aBox);

   private:

     static TBool GetRGBValL(TCSSParser* aParser,
                             TCSSReaderUnit* aPropertyVal,
                             TInt8 aType,
                             TUint8* aVal);

     static TBool SetRGBColorL(TCSSParser* aParser,
                               NW_LMgr_Property_t *aProp,
                               TCSSPropertyVal* aPropertyVal);

     static TBool SetHexColor(NW_LMgr_Property_t *aProp, const TText16* aStorage);

};

#endif

