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



// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "CSSPropVal.h"
#include "CSSImageList.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_rootbox.h"
#include "nw_css_tokentoint.h"
#include "nw_lmgr_propertylist.h"
#include "nwx_math.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

/*struct TCSSPropValColorEntry
{
  const TText16 *strVal;
  TUint32 color;
};*/

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

TBool TCSSPropVal::GetRGBValL(TCSSParser* aParser,
                              TCSSReaderUnit* aPropertyVal,
                              TInt8 aType,
                              TUint8* aVal)
{
  if ((aType == NUMBER) || (aType == PERCENTAGE))
  {
    TText16 *endPtr = NULL;
    TInt32 number = 0;

    TText16* ucs2Str = aPropertyVal->GetUnicodeL(aParser->GetEncoding());
    number = NW_Str_Strtol (ucs2Str, &endPtr, 10 );
    NW_Mem_Free(ucs2Str);

    if (aType == NUMBER)
    {
      if (number > 255)
      {
        number = 255;
      }
    }
    else  //type == PERCENTAGE
    {
      if (number > 100)
      {
        number = 100;
      }
      number = (TInt32)(number*255/100);
    }
    *aVal = (TUint8)number;
    return ETrue;
  }
  return EFalse;
}

/* ------------------------------------------------------------------------- */
TBool TCSSPropVal::SetRGBColorL(TCSSParser* aParser,
                                NW_LMgr_Property_t *aProp,
                                TCSSPropertyVal* aPropertyVal)
{
  TUint32 color = 0;
  TUint8 val = 0;

  TCSSPropertyVal rVal;
  TCSSPropertyVal gVal;
  TCSSPropertyVal bVal;

  NW_ASSERT(aProp);

  TText8* savedPosition = aParser->GetPosition();

  aParser->SetPosition(aPropertyVal->val.iStorage);
  if (aParser->ParserRGB(&rVal, &gVal, &bVal) != KBrsrSuccess)
  {
    aParser->SetPosition(savedPosition);
    return EFalse;
  }
  aParser->SetPosition(savedPosition);

  if (!GetRGBValL(aParser, &rVal.val, rVal.type, &val))
  {
    return EFalse;
  }
  color = color | val;
  if (!GetRGBValL(aParser, &gVal.val, gVal.type,&val))
  {
    return EFalse;
  }
  color = (color << 8)| val;
  if (!GetRGBValL(aParser, &bVal.val, bVal.type,&val))
  {
    return EFalse;
  }
  color = (color << 8)| val;
  aProp->value.integer = color;
  aProp->type = NW_CSS_ValueType_Color;

  return ETrue;
}

/* -------------------------------------------------------------------------*/
TBool TCSSPropVal::SetHexColor(NW_LMgr_Property_t *aProp, const TText16* aStorage)
{
  TUint8 v = 0;
  TUint32 color = 0;
  TText16 ch = 0;

  TUint32 length = NW_Str_Strlen(aStorage);
  if ((length != 3) && (length != 6))
  {
    return EFalse;
  }
  for (TUint8 index = 0; index <length; index++)
  {
    ch = *(aStorage+index);

    if (NW_Str_Isdigit(ch))
    {
      v = (TUint8)(ch - '0');
    }
    else if ((ch >= 'A') && (ch <= 'F'))
    {
      v = (TUint8)(10 + (ch - 'A'));
    }else if ((ch >= 'a') && (ch <= 'f'))
    {
      v = (TUint8)(10 + (ch - 'a'));
    }else
    {
      return EFalse;
    }
    if (index == 0)
    {
      color = color | v;
    }
    else
    {
      color = (color << 4) | v;
    }
    if (length == 3)
    {
      color = (color << 4) | v;
    }
  }
  aProp->value.integer = color;
  aProp->type = NW_CSS_ValueType_Color;
  return ETrue;
}


/* -------------------------------------------------------------------------*/
TBool TCSSPropVal::SetColorL(TCSSParser* aParser,
                            NW_LMgr_Property_t *aBoxVal,
                            TCSSPropertyVal* aPropertyVal,
                            TText16* aValStorage)
{
  switch(aPropertyVal->type)
  {
    case HASH:
      return TCSSPropVal::SetHexColor(aBoxVal, aValStorage);
    case RGB:
      return TCSSPropVal::SetRGBColorL(aParser, aBoxVal, aPropertyVal);
    default:
      return EFalse;
  }
}

/* ------------------------------------------------------------------------- */
TBool
TCSSPropVal::SetNumberPropVal(NW_LMgr_PropertyName_t aPropName,
                              NW_LMgr_Property_t *aProp,
                              TUint8 aValueType,
                              NW_Float32 aValue)
{
  aProp->value.integer = 0;
  switch(aValueType)
  {
  case NUMBER:
    switch (aPropName) {
    case NW_CSS_Prop_font:
    case NW_CSS_Prop_fontWeight:
    case NW_CSS_Prop_marqueeSpeed:
    case NW_CSS_Prop_marqueeLoop:
      aProp->value.integer = (TInt32)aValue;
      aProp->type = NW_CSS_ValueType_Integer;
      break;

    default:
      aProp->value.decimal = aValue;
      aProp->type = NW_CSS_ValueType_Number;
      break;
    }
    break;

  case PXS:
    aProp->value.integer = (TInt32)aValue;
    aProp->type = NW_CSS_ValueType_Px;
    break;

  case EMS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Em;
    break;

  case EXS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Ex;
    break;

  case INS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_In;
    break;

  case CMS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Cm;
    break;

  case MMS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Mm;
    break;

  case PCS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Pc;
    break;

  case PTS:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Pt;
    break;

  case PERCENTAGE:
    aProp->value.decimal = aValue;
    aProp->type = NW_CSS_ValueType_Percentage;
    break;

  default:
    return EFalse;
  }
  return ETrue;
}

/* ------------------------------------------------------------------------- */
TBool TCSSPropVal::SetImageValL(NW_LMgr_PropertyName_t aPropName,
                                NW_LMgr_Property_t *aVal,
                                TText16* aValStorage,
                                NW_CSS_Processor_t* aProcessor,
                                NW_LMgr_Box_t *box, 
                                TInt aBufferIndex)
{
  NW_Image_AbstractImage_t* image = NULL;
  TBrowserStatusCode status = KBrsrSuccess;

  NW_REQUIRED_PARAM(aVal);
  NW_REQUIRED_PARAM(aPropName);

  if (aValStorage == NULL)
  {
    return EFalse;
  }
  NW_Text_UCS2_t* textVal =
      NW_Text_UCS2_New(aValStorage, NW_Str_Strlen(aValStorage), NW_Text_Flags_Copy );
  if (textVal == NULL)
  {
    User::Leave(KErrNoMemory);
  }

  /* loads an image only in the case of list-style-image property, later
  we might want to load background-image.  Note that the url text gets
  deleted inside the processor. */
  status =
    NW_CSS_Processor_GetImage(aProcessor, NW_TextOf (textVal), box, aPropName, &image , aBufferIndex);
  if (status == KBrsrOutOfMemory)
  {
    User::Leave(KErrNoMemory);
  }

  /* Remove ownership of the entry. It has been handed off to the simple
     property list which will handle deletion.*/
  if (image)
  {
    CCSSImageList* imageList = (CCSSImageList*)(aProcessor->imageList);
    NW_DOM_ElementNode_t* imageNode = NULL;
    CCSSImageListEntry* imageEntry = imageList->EntryByVirtualImage( image, &imageNode );
    if (imageEntry)
    {
      imageList->RemoveVirtualImageOwnership( *imageEntry, image );
    }
  }


  /* Note that val can be zero */
  aVal->value.object = (NW_Object_t*)image;
  aVal->type = NW_CSS_ValueType_Image;

  return ETrue;
}

/* ------------------------------------------------------------------------- */
TBool TCSSPropVal::SetTokenVal(NW_LMgr_PropertyName_t aPropName,
                               NW_LMgr_Property_t *aProp,
                               TUint32 aToken,
                               NW_LMgr_Box_t* aBox)
{
  TInt32 color, fontWeight, borderWidth;
  NW_Float32    fontSize;
  NW_CSS_TokenToInt_t* tokenToInt = (NW_CSS_TokenToInt_t *)(&NW_CSS_TokenToInt);

  if (aPropName == NW_CSS_Prop_fontSize || aPropName == NW_CSS_Prop_font)
  {
    if (NW_CSS_TokenToInt_GetFontSize(tokenToInt, aToken, aBox, &fontSize))
        {
        aProp->value.decimal = fontSize;
        aProp->type = NW_CSS_ValueType_Pt;
        return ETrue;
        }
  }

  if (aPropName == NW_CSS_Prop_fontWeight || aPropName == NW_CSS_Prop_font)
  {
    fontWeight = NW_CSS_TokenToInt_GetFontWeight(tokenToInt, aToken, aBox);
    if (fontWeight != -1)
    {
      aProp->value.integer = fontWeight;
      aProp->type = NW_CSS_ValueType_Integer;
      return ETrue;
    }
  }

  if (aPropName == NW_CSS_Prop_border || aPropName == NW_CSS_Prop_borderColor ||
      aPropName == NW_CSS_Prop_borderTop || aPropName == NW_CSS_Prop_topBorderColor ||
      aPropName == NW_CSS_Prop_borderBottom || aPropName == NW_CSS_Prop_bottomBorderColor ||
      aPropName == NW_CSS_Prop_borderLeft || aPropName == NW_CSS_Prop_leftBorderColor ||
      aPropName == NW_CSS_Prop_borderRight || aPropName == NW_CSS_Prop_rightBorderColor) {
    color = NW_CSS_TokenToInt_GetColorVal(tokenToInt, NW_TRUE, aToken);
  } else {
    color = NW_CSS_TokenToInt_GetColorVal(tokenToInt, NW_FALSE, aToken);
  }

  if (color != -1)
  {
    aProp->value.integer = (TUint32)color;
    aProp->type = NW_CSS_ValueType_Color;
    return ETrue;
  }

  borderWidth = NW_CSS_TokenToInt_GetBorderWidth(tokenToInt, aToken, aBox);
  if (borderWidth != -1)
  {
    aProp->value.integer = borderWidth;
    aProp->type = NW_CSS_ValueType_Px;
    return ETrue;
  }
  /* Otherwise just leave the aToken as is */
  aProp->value.token = aToken;
  aProp->type = NW_CSS_ValueType_Token;

  return ETrue;
}

