/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  device context
*
*/



// INCLUDE FILES
#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "nw_object_dynamici.h"
#include "GDIFont.h"
#include "GDIDeviceContext.h"
#include "nw_text_ucs2.h"
#include "nwx_string.h"
#include <fbs.h>
#include "BrsrStatusCodes.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Average char width in pixels (as stipulated in the UI spec).
const TUint KAverageChineseCharWidthInPixels = 12;

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT(KEmptyFontName,"");

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

 
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CGDIFont::CGDIFont
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGDIFont::CGDIFont()
  {
  }

// -----------------------------------------------------------------------------
// CGDIFont::CGDIFont
// Symbian 2nd phase constructor can leave.
// aSize is in Twips
// -----------------------------------------------------------------------------
//
void CGDIFont::ConstructL (const CGDIDeviceContext* aDeviceContext,
				           const NW_Text_t* aFamily,
                           TGDIFontStyle aStyle,
                           TUint16 aSize,
                           TUint16 aWeight,
                           TGDIFontVariant aVariant)
    {
    NW_Uint16* storage = NULL;
    HBufC* buf = NULL;
    NW_Text_Length_t length;
    NW_Uint8 freeNeeded = 0;

    iDeviceContext = const_cast<CGDIDeviceContext*> (aDeviceContext);
    iFontStyle = aStyle;
    iFontVariant = aVariant;
    iFontWeight = aWeight;
    iFontFound = EFalse;
    
    if (aStyle == EGDIFontStyleOblique)
        {
        iFontStyle = EGDIFontStyleItalic;
        }

    if (aFamily)
        {
        // convert font family name from UCS2 to TPtr16
        storage = (NW_Uint16*)NW_Text_GetUCS2Buffer (aFamily, 0, &length, &freeNeeded);
        NW_ASSERT(storage);

        TPtrC16 requestedFontPtr16(storage, length);
        
        // convert font family name from UCS2 to HBufC
        iFontFamily = requestedFontPtr16.AllocLC();

        CleanupStack::Pop(iFontFamily); 

        if (freeNeeded) 
            {
            NW_Mem_Free ((NW_Ucs2*)storage);
            }
        }
    // no font family was specified so use empty font family name
    // underlying font API should find variant dependent font
    else
        {    
        buf = KEmptyFontName().AllocLC();
        iFontFamily = buf;

        CleanupStack::Pop(buf); 
        }
 
    if (aSize == 0)
        {
        iDeclaredFontSizeInTwips = KGDIDefaultFontSizeInTwips;
        }
    else
        {
        iDeclaredFontSizeInTwips = aSize;
        }

    // initialize the font size 
    iDeviceContext->ChangeFontSize(const_cast<CGDIFont*>(this));
    }


// -----------------------------------------------------------------------------
// CGDIFont::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGDIFont* CGDIFont::NewL (const CGDIDeviceContext* aDeviceContext,
				          const NW_Text_t* aFamily,
                          TGDIFontStyle style,
                          TUint16 aSize,
                          TUint16 aWeight,
                          TGDIFontVariant aVariant)
  {
  CGDIFont* self = new( ELeave ) CGDIFont();
  
  CleanupStack::PushL( self );
  self->ConstructL(aDeviceContext, aFamily, style, aSize, aWeight, aVariant);
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CGDIFont::~CGDIFont ()
{
  if (iFontFamily)
    {
    delete iFontFamily;
    }
}

// -----------------------------------------------------------------------------
// CGDIFont::GetEpocNativeFont
// returns const pointer to a CFont object */
//
// -----------------------------------------------------------------------------
const void* CGDIFont::GetEpocNativeFont ()
    {
    const CFont* epocCFont = NULL;
    CGDIDeviceContext* epoc32dc = iDeviceContext;

    // create the font spec
    TFontSpec fs(*iFontFamily, (TInt)iFontSize);
    TBool bold( EFalse );
	if (iFontWeight > 400)
	    {
		bold = ETrue;
	    }

	TBool italic( EFalse );
	if ( iFontStyle == EGDIFontStyleItalic || iFontSize == EGDIFontStyleItalic )
	    {
		italic = ETrue;
	    }
	fs.iFontStyle.SetStrokeWeight(bold ? EStrokeWeightBold : EStrokeWeightNormal);
    fs.iFontStyle.SetPosture(italic ? EPostureItalic : EPostureUpright);

    // we should always get a font back from the device context
	epocCFont = epoc32dc->FindFont(fs);
    NW_ASSERT(epocCFont);
        
	return epocCFont;
    }

// -----------------------------------------------------------------------------
// CGDIFont::SetFontSize
//
// -----------------------------------------------------------------------------
void
CGDIFont::SetFontSize (TUint16 aFontSize)
{
  iFontSize = aFontSize;
}

// ------------------------------------------------------------------------- 
// CGDIFont::GetFontInfo
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIFont::GetFontInfo (TGDIFontInfo* aFontInfo)
{
  // parameter assertion block 
  NW_ASSERT (aFontInfo);

  // change font size 
  iDeviceContext->ChangeFontSize(const_cast<CGDIFont*>(this));

  CFont* epocFont = (CFont*)GetEpocNativeFont();

  // initialize the aFontInfo structure
  aFontInfo->baseline = (TUint8)(epocFont->AscentInPixels() - epocFont->BaselineOffsetInPixels());
  aFontInfo->height = (TUint8)(epocFont->HeightInPixels());

  // successful completion 
  return KBrsrSuccess;
}

TUint8
CGDIFont::GetAverageCharWidthInPixels()
  {
  TUint8 width;
  CFont* epocFont = (CFont*)GetEpocNativeFont();

  if (AknLayoutUtils::Variant() == EApacVariant)
    {
    width = KAverageChineseCharWidthInPixels;
    }
  else
    {
    width = (TUint8)(epocFont->CharWidthInPixels('w'));
    }
  return width;
  }
