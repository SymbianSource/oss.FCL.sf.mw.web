/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "nw_css_tokentointi.h"

#include "nw_lmgr_cssproperties.h"
#include "nw_text_ucs2.h"
#include <nwx_string.h>
#include <nwx_math.h>
#include "BrsrStatusCodes.h"
#include "AknUtils.h"

/* ------------------------------------------------------------------------- *
   private data type
 * ------------------------------------------------------------------------- */
static const
NW_CSS_TokenToInt_Entry_t _NW_CSS_TokenToInt_ColorVals[] = 
{
  {NW_CSS_PropValue_aqua,    0x00ffff},
  {NW_CSS_PropValue_black,   0x000000},
  {NW_CSS_PropValue_blue,    0x0000ff},
  {NW_CSS_PropValue_cyan,    0x00ffff},
  {NW_CSS_PropValue_fuchsia, 0xff00ff},
  {NW_CSS_PropValue_gray,    0x808080},
  {NW_CSS_PropValue_green,   0x008000},
  {NW_CSS_PropValue_lime,    0x00ff00},
  {NW_CSS_PropValue_maroon,  0x800000},
  {NW_CSS_PropValue_navy,    0x000080},
  {NW_CSS_PropValue_olive,   0x808000},
  {NW_CSS_PropValue_pink,    0xffc0cb},
  {NW_CSS_PropValue_purple,  0x800080},
  {NW_CSS_PropValue_red,     0xff0000},
  {NW_CSS_PropValue_silver,  0xc0c0c0},
  {NW_CSS_PropValue_teal,    0x008080},
  {NW_CSS_PropValue_white,   0xffffff},
  {NW_CSS_PropValue_yellow,  0xffff00}
};

/* ------------------------------------------------------------------------- */
// Font size in points
static const
NW_CSS_TokenToDec_Entry_t _NW_CSS_PropVal_FontSizeVals[] = 
{
  {NW_CSS_PropValue_xxSmall, 5.0F},
  {NW_CSS_PropValue_xSmall, 5.5F},
  {NW_CSS_PropValue_small, 6.0F},
  {NW_CSS_PropValue_medium, 7.0F},
  {NW_CSS_PropValue_large, 10.0F},
  {NW_CSS_PropValue_xLarge, 12.0F},
  {NW_CSS_PropValue_xxLarge, 14.0F},
};

/* ------------------------------------------------------------------------- */
static const
NW_CSS_TokenToInt_Entry_t _NW_CSS_PropVal_BorderWidthVals[] = 
{
  {NW_CSS_PropValue_thin, NW_LMgr_Box_MinBorderWidth},
  {NW_CSS_PropValue_medium, NW_LMgr_Box_MediumBorderWidth},
  {NW_CSS_PropValue_thick, NW_LMgr_Box_MaxBorderWidth}
};


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_CSS_TokenToInt_Class_t NW_CSS_TokenToInt_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Base_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_CSS_TokenToInt         */
    /* numFontSizeVals           */ sizeof(_NW_CSS_PropVal_FontSizeVals)/sizeof(NW_CSS_TokenToDec_Entry_t),
    /* fontSizeList              */ _NW_CSS_PropVal_FontSizeVals,
    /* numBorderWidthVals        */ sizeof(_NW_CSS_PropVal_BorderWidthVals)/sizeof(NW_CSS_TokenToInt_Entry_t),
    /* borderWidthList           */ _NW_CSS_PropVal_BorderWidthVals
  }
};

/* ------------------------------------------------------------------------- */
const NW_CSS_TokenToInt_t NW_CSS_TokenToInt = {
  {&NW_CSS_TokenToInt_Class},
};

/* -------------------------------------------------------------------------- *
    final methods
 * ---------------------------------------------------------------------------*/
/* ------------------------------------------------------------------------- */
NW_Int32 
NW_CSS_TokenToInt_GetColorVal(NW_CSS_TokenToInt_t* thisObj,
                              NW_Bool transparentAllowed,
                              NW_LMgr_PropertyValueToken_t token)
{
  NW_Uint32 numColorVals;
  const NW_CSS_TokenToInt_Entry_t* colorVals;
  NW_Uint32 index;

  NW_REQUIRED_PARAM(thisObj);

  /* If transparent is an allowed color, look for transparent token */
  if (transparentAllowed && (token == NW_CSS_PropValue_transparent)) {
    return NW_GDI_Color_Transparent;
  }

  numColorVals = sizeof(_NW_CSS_TokenToInt_ColorVals)/sizeof(NW_CSS_TokenToInt_Entry_t);
  colorVals = _NW_CSS_TokenToInt_ColorVals;

  /* traverse our list of token entries looking for a match for our token */
  for (index = 0; index < numColorVals; index++) {
    if (colorVals[index].token == token) {
      return colorVals[index].val;
    }
  }

  /* if no token entry is found we must return -1 */
  return -1;
}

/* ------------------------------------------------------------------------- */
NW_Int32
NW_CSS_TokenToInt_GetFontWeight(NW_CSS_TokenToInt_t* thisObj,
                                NW_LMgr_PropertyValueToken_t token,
                                NW_LMgr_Box_t* box)
{
  NW_LMgr_ContainerBox_t* parentBox;
  NW_LMgr_Property_t prop;

  (void)thisObj;
  switch (token)
  {
    case NW_CSS_PropValue_bold:
      return 700;

    case NW_CSS_PropValue_normal:
      return 400;

    case NW_CSS_PropValue_bolder:
    case NW_CSS_PropValue_lighter:
      
      /* Get the parent */
      parentBox = NW_LMgr_Box_GetParent(box);

      /* Get the parent font weight */
      prop.type = NW_CSS_ValueType_Integer;
      prop.value.integer = 400;
      (void) NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(parentBox), 
               NW_CSS_Prop_fontWeight, &prop);

      if (prop.type != NW_CSS_ValueType_Integer){
        return 0;
      }
      if (token == NW_CSS_PropValue_bolder){
        if (prop.value.integer >= 900){
          return 900;
        }
        return (prop.value.integer + 100);
      }
      /* return lighter val */
      if (prop.value.integer <= 100){
        return 100;
      }
      return (prop.value.integer - 100);

    default:
      return -1;
  }
}

/* ------------------------------------------------------------------------- */
NW_Bool 
NW_CSS_TokenToInt_GetFontSize(NW_CSS_TokenToInt_t* tokenToInt, 
                              NW_LMgr_PropertyValueToken_t token,
                              NW_LMgr_Box_t* box,
                              NW_Float32* value)
    {
    NW_Int32 numVals;
    const NW_CSS_TokenToDec_Entry_t* fontSizeList;
    NW_Int32 index;
    NW_LMgr_Property_t prop;

    // Look for the list of conversion values
    fontSizeList = NW_CSS_TokenToInt_GetClassPart(tokenToInt).fontSizeList;
    if (fontSizeList == NULL)
        {
        return NW_FALSE;
        }

    // Look for the number of conversion values
    numVals = NW_CSS_TokenToInt_GetClassPart(tokenToInt).numFontSizeVals;
    if (numVals <= 0) 
        {
        return NW_FALSE;
        }

    // Try to match "xx-small", "x-small", "medium" etc.
    for (index = 0; index < numVals; index++) 
        {
        if (fontSizeList[index].token == token)
            {
            *value = fontSizeList[index].val;
            return NW_TRUE;
            }
        }

    // deal with "larger" and "smaller"
    if ((token == NW_CSS_PropValue_larger) || (token == NW_CSS_PropValue_smaller))
        {
        NW_Uint32 currentFontSizePx;
        NW_Float32 currentFontSizePt;
        NW_Int32 twips;
        NW_Float32 closestFit;
        NW_Int32 closestFitIndex;

        // initialize the font size
        prop.type = NW_CSS_ValueType_Pt;
        prop.value.decimal = fontSizeList[3].val;   /* Default to medium */

        (void)NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_fontSize, &prop);

        // convert the font size in pixels to font size in points
        currentFontSizePx = prop.value.integer;

        // first convert from pixels to twips
        CEikonEnv* eikEnv = CEikonEnv::Static();
        CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();

	    CFbsScreenDevice* fbsScrDev = CFbsScreenDevice::NewL(_L("Dummy"), screenDev.DisplayMode());

        twips = fbsScrDev->VerticalPixelsToTwips(currentFontSizePx);

        // convert from twips to points
        currentFontSizePt = (NW_Float32)FontUtils::PointsFromTwips(twips);
        
        closestFit = NW_Math_fabs(currentFontSizePt - fontSizeList[0].val);
        closestFitIndex = 0;

        for (index = 1; index < numVals; index++) 
            {
            if (NW_Math_fabs(currentFontSizePt - fontSizeList[index].val) < closestFit)
                {
                closestFit = NW_Math_fabs(currentFontSizePt - fontSizeList[index].val);
                closestFitIndex = index;
                }
            }

        // adjust up or down by one font-size token 
        if (token == NW_CSS_PropValue_larger)
            {
            if (closestFitIndex < (numVals - 1)) 
                {
                closestFitIndex++;
                }
            }
        else if (token == NW_CSS_PropValue_smaller)
            {
            if (closestFitIndex > 0) 
                {
                closestFitIndex--;
                }
            }

        *value = fontSizeList[closestFitIndex].val;

        if (fbsScrDev)
            {
            delete fbsScrDev;
            }

        return NW_TRUE;
        }

    // Otherwise return NW_FALSE:  no match was found 
    return NW_FALSE;
    }

/* -------------------------------------------------------------------------*/
NW_Int32 
NW_CSS_TokenToInt_GetBorderWidth(NW_CSS_TokenToInt_t* tokenToInt, 
                                 NW_LMgr_PropertyValueToken_t token,
                                 NW_LMgr_Box_t* box)
{
  NW_Uint32 numVals;
  const NW_CSS_TokenToInt_Entry_t* borderWidthList;
  NW_Uint32 index;

  NW_REQUIRED_PARAM(box);

  /* Look for the list of conversion values */
  borderWidthList = NW_CSS_TokenToInt_GetClassPart(tokenToInt).borderWidthList;
  if (borderWidthList == NULL){
    return 0;
  }
  numVals = NW_CSS_TokenToInt_GetClassPart(tokenToInt).numBorderWidthVals;

  /* Look for a match */
  for (index = 0; index < numVals; index++)
  {
    if (borderWidthList[index].token == token){
      return borderWidthList[index].val;
    }
  }

  /* No match was found */
  return -1;
}

