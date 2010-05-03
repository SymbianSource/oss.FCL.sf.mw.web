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
#include "GDIDevicecontext.h"
#include "nw_object_dynamici.h"
#include "GDIFont.h"
#include "nw_gdi_utils.h"
#include "nw_text_ucs2.h"
#include <nwx_string.h>
#include <nwx_settings.h>
#include "nwx_logger.h"
#include "nwx_http_defs.h"
#include "MVCView.h"
#include <AknUtils.h>
#include <gdi.h>
#include <w32std.h>
#include <eikenv.h>
#include <barsread.h>
#include <gulutil.h>
#include <bautils.h>
#include <webkit.rsg>
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>
#include <biditext.h>
#include "BrsrStatusCodes.h"
#include "featmgr.h"
#include <AknFontAccess.h>

#include <bldvariant.hrh>
#include <data_caging_path_literals.hrh>
#include "MemoryManager.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

/*
 The Thai language is unique in that it has certain characters that combine
 into a single entity that will be referred to as a Thai script "cell" or just
 "cell" for short.  Furthermore, the language uses white space very sparingly,
 frequently not having any space at all between individual words. These unique
 features create additional complexity when truncating or word-wrapping Thai
 text.

 Truncation should not occur in the middle of a "cell" if possible. The
 exception is if all that remains to be displayed is a single cell but
 there isn't space for the entire cell.

 As for word-wrapping, in optimal case, Thai text ought to be broken into lines
 at word boundaries. However, because white space is seldom used, word
 boundaries cannot be found unless a dictionary or a heavy statistical/
 morphological analysis is run on the text. Therefore, a less perfect approach
 is used.

 Line breaking is done according to Thai script "cells", which can be
 thought of as the smallest indivisible units of the script.

 The cells consist of combinations of characters from the following classes
 of characters, which are divided into two types -- spacing and combining:
   CONS  Consonant       (spacing)
   AD    Above Diacritic (combining)
   AV    Above Vowel     (combining)
   BD    Below Diacritic (combining)
   BV    Below Vowel     (combining)
   FV    Following Vowel (spacing)
   LV    Leading Vowel   (spacing)
   NON   Not a Thai letter, i.e. is a number, currency, etc (spacing)
   TONE  Tone mark       (combining)

 Graphically, the "cells" are built around a consonant, which functions as a
 base for a stack of combining elements, optionally preceded and/or followed
 by spacing vowels. The graph below illustrates how the bits and pieces of a
 cell are placed around the consonant:

             [TONE]
          [AV/AD/TONE]
_______[LV]__[CONS]__[FV]________  (baseline)
            [BV/BD]

 In the encoded text, you can anticipate sequences like:
 ... [LV] [CONS] [AV/BV/AD/BD] [TONE] [FV] ...

 If such is a prototype of a Thai script "cell", lines can be split without
 interfering with the most crucial structures of the script
   * between subsequent [FV] and [LV]
   * between subsequent [CONS] and [LV]
   * between subsequent [AV/BV/AD/BD] and [LV]
   * between subsequent [TONE] and [LV]
   * between subsequent [CONS] and [CONS]
   * between subsequent [AV/BV/AD/BD] and [CONS]
   * between subsequent [TONE] and [CONS]
   * between subsequent [FV] and [CONS]

 This can be reduced to the following three rules:
   1. Splits can occur at whitespace.
   2. Splits can occur immediately preceding any LV.
   3. Splits can occur immediately preceding any CONS that is not
     immediately preceded by an LV.

 Unfortunately, this approach does not guarantee individual words, or even
 individual syllables, from being broken across lines. All that can be said is
 that the outer structure of the script is preserved - combining characters as
 well as the leading/following vowels stick with the base character.
*/

// Thai unicode char range: 0x0E00 - 0x0E7F. Note, only the first 0x60
// characters are used.
#define THAI_CHAR_BASE 0x0E00
// Charcter classes and types.
#define THAI_CHAR_COMBINING_TYPE_FLAG 0x80
#define THAI_CHAR_SPACING_TYPE_FLAG   0x40
#define THAI_CHAR_CONS (1 | THAI_CHAR_SPACING_TYPE_FLAG)
#define THAI_CHAR_AD   (2 | THAI_CHAR_COMBINING_TYPE_FLAG)
#define THAI_CHAR_AV   (3 | THAI_CHAR_COMBINING_TYPE_FLAG)
#define THAI_CHAR_BD   (4 | THAI_CHAR_COMBINING_TYPE_FLAG)
#define THAI_CHAR_BV   (5 | THAI_CHAR_COMBINING_TYPE_FLAG)
#define THAI_CHAR_FV   (6 | THAI_CHAR_SPACING_TYPE_FLAG)
#define THAI_CHAR_LV   (7 | THAI_CHAR_SPACING_TYPE_FLAG)
#define THAI_CHAR_NON  (8 | THAI_CHAR_SPACING_TYPE_FLAG)
#define THAI_CHAR_TONE (9 | THAI_CHAR_COMBINING_TYPE_FLAG)

// Map that defines the class and type for each Thai character.
// Character range
const TUint8 ThaiCharMap[0x60] =
    {
    // 0x0E00
    THAI_CHAR_NON,
    // 0x0E01 - 0x0E23
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    // 0x0E24
    THAI_CHAR_FV,
    // 0x0E25
    THAI_CHAR_CONS,
    // 0x0E26
    THAI_CHAR_FV,
    // 0x0E27 - 0x0E2E
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    THAI_CHAR_CONS, THAI_CHAR_CONS, THAI_CHAR_CONS,
    // 0x0E2F
    THAI_CHAR_NON,
    // 0x0E30
    THAI_CHAR_FV,
    // 0x0E31
    THAI_CHAR_AV,
    // 0x0E32 - 0x0E33
    THAI_CHAR_FV, THAI_CHAR_FV,
    // 0x0E34 - 0x0E37
    THAI_CHAR_AV, THAI_CHAR_AV, THAI_CHAR_AV, THAI_CHAR_AV,
    // 0x0E38 - 0x0E39
    THAI_CHAR_BV, THAI_CHAR_BV,
    // 0x0E3A
    THAI_CHAR_BD,
    // 0x0E3B - 0x0E3F
    THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON,
    // 0x0E40 - 0x0E44
    THAI_CHAR_LV, THAI_CHAR_LV, THAI_CHAR_LV, THAI_CHAR_LV, THAI_CHAR_LV,
    // 0x0E45
    THAI_CHAR_FV,
    // 0x0E46
    THAI_CHAR_NON,
    // 0x0E47
    THAI_CHAR_AD,
    // 0x0E48 - 0x0E4B
    THAI_CHAR_TONE, THAI_CHAR_TONE, THAI_CHAR_TONE, THAI_CHAR_TONE,
    // 0x0E4C - 0x0E4E
    THAI_CHAR_AD, THAI_CHAR_AD, THAI_CHAR_AD,
    // 0x0E4F - 0x0E5F
    THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON,
    THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON,
    THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON, THAI_CHAR_NON,
    THAI_CHAR_NON, THAI_CHAR_NON
    };

// MACROS
#define IS_THAI_CHAR(c) ((c >= 0x0E00) && (c <= 0x0E60))
//#define IS_THAI_LEADING_VOWEL(c) ((c >= 0x0E40) && (c <= 0x0E44))
#define IS_THAI_LEADING_VOWEL(c) (ThaiCharMap[c - THAI_CHAR_BASE] == THAI_CHAR_LV)
//#define IS_THAI_CONSONANT(c) ((c >= 0x0E01) && (c <= 0x0E2E) && (c != 0x0E24) && (c != 0x0E26))
#define IS_THAI_CONSONANT(c) (ThaiCharMap[c - THAI_CHAR_BASE] == THAI_CHAR_CONS)
//#define IS_THAI_FOLLOWING_VOWEL(c) ((c == 0x0E24) || (c == 0x0E26) || (c == 0x0E30) || (c == 0x0E32) || (c == 0x0E33) || (c == 0x0E45))
#define IS_THAI_FOLLOWING_VOWEL(c) (ThaiCharMap[c - THAI_CHAR_BASE] == THAI_CHAR_FV)
#define IS_THAI_COMBINING(c) (ThaiCharMap[c - THAI_CHAR_BASE] & THAI_CHAR_COMBINING_TYPE_FLAG)


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CGDIDeviceContext::CGDIDeviceContext
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGDIDeviceContext::CGDIDeviceContext()
  {
  }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::CGDIDeviceContext
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::ConstructL (void* aGraphicsContext,
                               CView* aOocEpoc32View)
  {
  // initialize the member variables
  iGraphicsContext = aGraphicsContext;
  if( !iGraphicsContext )
    {
    return KBrsrFailure;
    }

  iOocEpoc32View = aOocEpoc32View;
  if( !iOocEpoc32View )
    {
    return KBrsrFailure;
    }

  iVariantFlag = AknLayoutUtils::Variant();

  _LIT( KResourceFile, "\\resource\\Webkit.rsc");
  _LIT( KResourceFileZ, "Z:Webkit.rsc");  
  TFileName resourceFileName; 
  TParse parse;   
  Dll::FileName (resourceFileName);          
  parse.Set(KResourceFile, &resourceFileName, NULL);
  resourceFileName = parse.FullName();
    
  CCoeEnv* coeEnv=CCoeEnv::Static();
  BaflUtils::NearestLanguageFile(coeEnv->FsSession(), resourceFileName);
  if ( !BaflUtils::FileExists( coeEnv->FsSession(), resourceFileName ) )          
      {
      // Use resource file on the Z drive instead 
      parse.Set( KResourceFileZ, &KDC_RESOURCE_FILES_DIR, NULL );
      resourceFileName = parse.FullName(); 
      BaflUtils::NearestLanguageFile( coeEnv->FsSession(),resourceFileName );         	
      }
  iResourceFileOffset = coeEnv->AddResourceFileL(resourceFileName);
  
  // Get the array of available fonts on the phone
  CEikonEnv* eikEnv = CEikonEnv::Static();
  iWsScreenDev = eikEnv->ScreenDevice();

  // Create the font cache
  iFontCache = new (ELeave) CFontCache(30);  // 30 items in cache

  // Find out which language we support
  if ( FeatureManager::FeatureSupported(KFeatureIdAvkonELaf) )
      {
      iSupportedLanguage = EElafLanguage;
      }
  else if ( FeatureManager::FeatureSupported(KFeatureIdAvkonApac) )
    {
    if ( FeatureManager::FeatureSupported(KFeatureIdThai) )
        {
        iSupportedLanguage = EThaiLanguage;
        }
    else if ( FeatureManager::FeatureSupported(KFeatureIdJapanese) )
        {
        iSupportedLanguage = EJapaneseLanguage;
        }
    else if ( FeatureManager::FeatureSupported(KFeatureIdChineseTaiwanHkFonts) )
        {
        iSupportedLanguage = EChinaHKTWLanguage;
        }
    else  if ( FeatureManager::FeatureSupported(KFeatureIdChinesePrcFonts) )
        {
        iSupportedLanguage = EChinesePRCLanguage;
        }
    }

  iFillPattern=NW_GDI_Pattern_Solid;
  iLinePattern=NW_GDI_Pattern_Solid;
  iPaintMode=(NW_GDI_PaintMode_e)CGraphicsContext::EDrawModePEN;
  iLineWidth=1;
  iFontSizeLevel = EGDIDeviceContextFontSizeLevelNormal;
  // successful completion
  return KBrsrSuccess;
  }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGDIDeviceContext* CGDIDeviceContext::NewL (void* aGraphicsContext,
                                            CView* aOocEpoc32View)
  {
  CGDIDeviceContext* self = new( ELeave ) CGDIDeviceContext();

  CleanupStack::PushL( self );
  self->ConstructL(aGraphicsContext, aOocEpoc32View);
  CleanupStack::Pop();

  return self;
  }

// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CGDIDeviceContext::~CGDIDeviceContext()
    {
    CFont* font = NULL;
    CBitmapContext* gc = Epoc32GraphicsContext();
    while ((font = iFontCache->RemoveFirstEntry()) != NULL)
        {
        gc->DiscardFont();
        iWsScreenDev->ReleaseFont( font );
        }

    delete iFontCache;

    // screen device
    iWsScreenDev = NULL;

    // unload resource file
    CCoeEnv* coeEnv=CCoeEnv::Static();
    coeEnv->DeleteResourceFile(iResourceFileOffset);
    }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::SwapColorBytes
//
// -----------------------------------------------------------------------------
//
NW_GDI_Color_t
CGDIDeviceContext::SwapColorBytes(NW_GDI_Color_t aColor)
{
  return NW_GDI_RGB(NW_GDI_GetBValue(aColor), NW_GDI_GetGValue(aColor), NW_GDI_GetRValue(aColor));
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::Epoc32GraphicsContext
//
// -----------------------------------------------------------------------------
//
CBitmapContext*
CGDIDeviceContext::Epoc32GraphicsContext ()
{
  return (CBitmapContext*)(iGraphicsContext);
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::WordWrap
// If a zero length string is passed in, KBrsrSuccess is returned and *length = 0
// If constraint is -1 return the smallest string that can split and KBrsrSuccess
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::WordWrap (NW_Text_t* aText,
                             CGDIFont* aGDIFont,
                             NW_GDI_Metric_t aConstraint,
                             NW_GDI_Metric_t aSpacing,
                             NW_GDI_FlowDirection_t aFlowDirection,
                             NW_Text_Length_t* aLength)
  {
  NW_REQUIRED_PARAM(aSpacing);
  NW_Text_Length_t strLen;
  NW_Text_Length_t i;
  TText16 ch1 = 0;
  TText16 ch2 = 0;
  TText16* string = NULL;
  TBool freeNeeded = NW_FALSE;
  TPtrC ptr;
  TBidiText *bidiText = NULL;
  CFont* epocFont = NULL;

  strLen = NW_Text_GetCharCount (aText);
  if (strLen == 0)
    {
    *aLength = 0;
    return KBrsrSuccess;
    }

  if (aConstraint == -1)
    {
    if (strLen > 1)
      {
      ch1 = (TText16)NW_Text_GetAt (aText, 0);
      for (i = 1; i < strLen; i++)
        {
        ch2 = (TText16)NW_Text_GetAt (aText, i);
        if (CanSplit(ch1, ch2))
          {
          *aLength = (NW_Text_Length_t)(i);
          return KBrsrSuccess;
          }
        ch1 = ch2;
        }
      }
    *aLength = strLen;
    return KBrsrSuccess;
    }

  if (aConstraint == 0)
    {
    *aLength = 0;
    return KBrsrLmgrNoSplit;
    }

  NW_TRY(status)
    {
    string = NW_Text_GetUCS2Buffer (aText,
        NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
        &strLen, (unsigned char*) &freeNeeded );

    for (i = 0; i < strLen; i++)
      {
      if (!NW_Str_Isspace((TText16)string[i]) && (string[i] != 0))
        {
        break;
        }
      }

    if (i == strLen)
      {
      *aLength = i;
      NW_THROW_SUCCESS(status);
      }

    ptr.Set(string, strLen);
    TRAPD(ret, bidiText = TBidiText::NewL(ptr, 1, (aFlowDirection == NW_GDI_FlowDirection_LeftRight) ?
        TBidiText::ELeftToRight : TBidiText::ERightToLeft));
    if (ret != KErrNone)
      {
      NW_THROW_STATUS (status, KBrsrOutOfMemory);
      }

    epocFont = (CFont*)aGDIFont->GetEpocNativeFont();
    bidiText->WrapText(aConstraint, *epocFont, 0);

    *aLength = bidiText->DisplayText().Length();

	//Solution for bug in TBidiText::WrapText for long text
	if(aFlowDirection == NW_GDI_FlowDirection_LeftRight)
	{
		NW_GDI_Dimension2D_t extent;
		status = GetSubstringExtent (aText,
									 aGDIFont,
									 0,
									 *aLength,
									 aSpacing,
									 aFlowDirection,
									 &extent);
		NW_THROW_ON_ERROR(status);
		if(extent.width > aConstraint)
		{
			extent.width = 0;
			NW_Text_Length_t len = 0;
			while(extent.width < aConstraint)
			{
				len++;
				status = GetSubstringExtent(aText, aGDIFont, 0, len, aSpacing, aFlowDirection, &extent);
				NW_THROW_ON_ERROR(status);
			}
			*aLength = len -1;
		}
	}
	//The above solution is to fix a Symbian bug in TBidiText::WrapText for long text

    delete bidiText;
    if (*aLength == 0)
      {
      NW_THROW_STATUS(status, KBrsrLmgrNoSplit);
      }
    if (*aLength < strLen)
      {
      // If the next character is a soft hyphen, add it to the aText
      if (string[*aLength] == NW_TEXT_UCS2_SOFT_HYPHEN)
        {
        (*aLength)++;
        NW_THROW_SUCCESS(status);
        }

      // TBidiText always omits the last space. see if we can add it back
      if ((*aLength) + 1 == strLen && NW_Str_Isspace(string[*aLength]))
        {
        NW_GDI_Dimension2D_t extent;
        status = GetSubstringExtent (aText,
                                     aGDIFont,
                                     0,
                                     *aLength,
                                     aSpacing,
                                     aFlowDirection,
                                     &extent);
        NW_THROW_ON_ERROR(status);
        if (extent.width <= aConstraint)
          {
          (*aLength)++;
          NW_THROW_SUCCESS(status);
          }
        }

      // Make sure we do the split at a valid position
      TInt len = *aLength;
      ch2 = (TText16)((len < (TInt)strLen) ? NW_Text_GetAt (aText, len) : 0);
      while (--len >= 0)
        {
        ch1 = (TText16)NW_Text_GetAt (aText, len);
        if (CanSplit(ch1, ch2))
          {
          break;
          }
        ch2 = ch1;
        }

	  CGDIDeviceContext* self = const_cast<CGDIDeviceContext*> (this);
      if(NW_Str_Isspace(ch2)||
		  len < 0	||
		  ch1 == NW_TEXT_UCS2_SOFT_HYPHEN ||
		  (self->VariantFlag() == EApacVariant && ch2 < 255 && ch1 > 255) //Doule bytes language word followed by a latin word
		  )
		{
			*aLength = len + 1;
		}
	  else
		{
			*aLength = len;
		}
      }
    if (*aLength == 0)
      {
      NW_THROW_STATUS(status, KBrsrLmgrNoSplit);
      }
    else
      {
      status =  KBrsrSuccess;
      }
    }

  NW_CATCH(status)
    {
    } // empty
  NW_FINALLY
    {
    if (freeNeeded)
      {
      NW_Mem_Free(string);
      }
    return status;
    }
  NW_END_TRY;
  }

// -------------------------------------------------------------------------
// CGDIDeviceContext::SplitAtNewline
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::SplitAtNewline(NW_Text_t* aText,
                                  CGDIFont* aGDIFont,
                                  NW_GDI_Metric_t aConstraint,
                                  NW_GDI_Metric_t aSpacing,
                                  NW_GDI_FlowDirection_t aFlowDirection,
                                  NW_Text_Length_t* aLength)
  {
  NW_Text_Length_t strLen;
  NW_Text_Length_t i;
  TText16 ch = 0;
  NW_GDI_Dimension2D_t size;

  NW_TRY(status)
    {
    // Get the aText aLength
    strLen = NW_Text_GetCharCount (aText);
    if (strLen == 0)
      {
      *aLength = 0;
      NW_THROW_SUCCESS(status);
      }

    // Look for the first LF character
    for (i=0; i<strLen; i++)
      {
      ch = (TText16) NW_Text_GetAt (aText, i);
      if (ch == NW_TEXT_UCS2_LF)
        {
        break;
        }
      }
    if (aConstraint < 0)
      {
      if (ch == NW_TEXT_UCS2_LF)
        {
        *aLength = (NW_Text_Length_t)(i + 1);
        }
      else
        {
        *aLength = strLen;
        }
      }
    else
      {
      status = CGDIDeviceContext::GetSubstringExtent( aText,
                                                      aGDIFont,
                                                      0,
                                                      i,
                                                      aSpacing,
                                                      aFlowDirection, &size );
      NW_THROW_ON_ERROR(status);

      if (size.width <= aConstraint)
        {
        if (ch == NW_TEXT_UCS2_LF)
          {
          *aLength = (NW_Text_Length_t)(i + 1);
          NW_THROW_SUCCESS(status);
          }
        else
          {
          *aLength = strLen;
          NW_THROW_SUCCESS(status);
          }
        }
      else
        {
        *aLength = 0;
        NW_THROW_STATUS(status, KBrsrLmgrNoSplit);
        }
      }
    }
  NW_CATCH(status)
    {
    }
  NW_FINALLY
    {
    return status;
    }
  NW_END_TRY;
  }

// -------------------------------------------------------------------------
// CGDIDeviceContext::Truncate
// NOTE:  This function never fails!  Even if a single character is too
// big for the constraint, it will return length = 1, and STAT_SUCCESS
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::Truncate (NW_Text_t* aText,
                             CGDIFont* aGDIFont,
                             NW_GDI_Metric_t aConstraint,
                             NW_GDI_Metric_t aSpacing,
                             NW_GDI_FlowDirection_t aFlowDirection,
                             NW_Text_Length_t* aLength)
{
  NW_Text_Length_t strLen;
  TText16* string = NULL;
  TBool freeNeeded = NW_FALSE;
  TPtrC ptr;
  TBidiText *bidiText = NULL;
  CFont* epocFont = NULL;

  NW_REQUIRED_PARAM(aSpacing);

  if (NW_Text_GetCharCount(aText) == 0)
    {
    *aLength = 1; // Must return at least 1
    return KBrsrSuccess;
    }

  NW_TRY(status)
    {
    string =
      NW_Text_GetUCS2Buffer (aText,
                             NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                             &strLen,
                             (unsigned char*) &freeNeeded);

    ptr.Set(string, strLen);
    TUint newLen = ptr.Locate('\n');
    if (newLen < strLen)
      {
      ptr.Set(ptr.Ptr(), newLen);
      }

    TRAPD(ret, bidiText = TBidiText::NewL(ptr, 1, (aFlowDirection == NW_GDI_FlowDirection_LeftRight) ?
        TBidiText::ELeftToRight : TBidiText::ERightToLeft));
    if (ret != KErrNone)
      {
      NW_THROW_STATUS (status, KBrsrOutOfMemory);
      }

    epocFont = (CFont*)aGDIFont->GetEpocNativeFont();
    bidiText->WrapText(aConstraint, *epocFont, 0);

    *aLength = bidiText->DisplayText().Length();

    //Solution for bug in TBidiText::WrapText for long text
	  if(aFlowDirection == NW_GDI_FlowDirection_LeftRight)
	    {
		    NW_GDI_Dimension2D_t extent;
		    status = GetSubstringExtent (aText,
									     aGDIFont,
									     0,
									     *aLength,
									     aSpacing,
									     aFlowDirection,
									     &extent);
		    NW_THROW_ON_ERROR(status);
		    if (extent.width > aConstraint)
		      {
          NW_Text_Length_t len = *aLength;
          while(extent.width > aConstraint)
            {
            len--;
            status = GetSubstringExtent (aText,
									     aGDIFont,
									     0,
									     len,
									     aSpacing,
									     aFlowDirection,
									     &extent);
		        NW_THROW_ON_ERROR(status);
            }
          *aLength = len;
		      }
	    }

    delete bidiText;

    // For the Thai language, truncation should not occur within the middle of
    // a "cell" unless all that is left is a single "cell". The text was
    // truncated above and now it will be walked in reverse, looking for a
    // spot that is not within a "cell", further reducing the returned length
    // if needed.
    TInt len = *aLength;
    if (len > 1)
      {
      TText16 ch1;
      TText16 ch2;
      ch2 = (TText16)((len < (TInt)strLen) ? NW_Text_GetAt (aText, len) : 0);
      while (--len >= 0)
        {
        ch1 = (TText16)NW_Text_GetAt (aText, len);
        if ( !IS_THAI_CHAR(ch1) || !IS_THAI_CHAR(ch2) )
          {
          break;
          }
        if (!IsThaiCell(ch1, ch2))
          {
          break;
          }
        ch2 = ch1;
        }
      len += 1;
      }
    if (len)
      {
      *aLength = len;
      }
    status = KBrsrSuccess;
    }
  NW_CATCH(status)
    {
    } // empty
  NW_FINALLY
    {
    if (freeNeeded)
      {
      NW_Mem_Free(string);
      }
    if (*aLength == 0)
      {
      // As described above, this function must not return aLength==0.
      // This happens when aConstraint is 0, as has occured in some tables, when
      // NW_LMgr_FormatContext->lineStart > NW_LMgr_FormatContext->lineEnd

      *aLength = 1;
      }
    return status;
    }
  NW_END_TRY;
  }

// -------------------------------------------------------------------------
// CGDIDeviceContext::ScaleBitmap
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::ScaleBitmap (CFbsBitmap* aBitmap,
                                CFbsBitmap** aScaledBitmap,
                                TRect* aRect)
{
  CBitmapContext* bitmapContext = NULL;
  CBitmapDevice* bitmapDevice = NULL;

  // parameter assertion block
  NW_ASSERT (aBitmap);
  NW_ASSERT (aScaledBitmap);
  NW_ASSERT (aRect);

  NW_TRY (status)
  {
    // create scaled aBitmap
    *aScaledBitmap = new CFbsBitmap();
    NW_THROW_OOM_ON_NULL( aScaledBitmap, status );

    TInt err = (*aScaledBitmap)->Create( aRect->Size(), aBitmap->DisplayMode() );
    if ( err != KErrNone )
    {
      NW_THROW_STATUS (status, KBrsrFailure);
    }

    // create aBitmap device and context
    TRAP( err, bitmapDevice = CFbsBitmapDevice::NewL( *aScaledBitmap ) );
    NW_THROW_OOM_ON_NULL( bitmapDevice, status );
    if ( err != KErrNone )
    {
      NW_THROW_STATUS (status, KBrsrFailure);
    }

    err = bitmapDevice->CreateBitmapContext( bitmapContext );
    if ( err != KErrNone )
    {
      NW_THROW_STATUS (status, KBrsrFailure);
    }

    // scale aBitmap
    bitmapContext->DrawBitmap( *aRect, aBitmap );
  }
  NW_CATCH (status)
  {
    delete *aScaledBitmap;
    *aScaledBitmap = NULL;
  }
  NW_FINALLY
  {
    delete bitmapContext;
    delete bitmapDevice;

    return status;
  }
  NW_END_TRY
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::InitializeGraphicsContext
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::InitializeGraphicsContext ()
{
  CBitmapContext* gc = Epoc32GraphicsContext();

  if (iClipRect.dimension.width!=0 && iClipRect.dimension.height!=0)
  {
    gc->SetClippingRect( TRect(
        iClipRect.point.x,
        iClipRect.point.y,
        iClipRect.point.x+iClipRect.dimension.width,
        iClipRect.point.y+iClipRect.dimension.height ) );
  }
  else
  {
	  gc->CancelClippingRect();
  }

  //gc->SetDrawMode((CGraphicsContext::TDrawMode)(iPaintMode));
  gc->SetPenSize(TSize(iLineWidth, iLineWidth));
  gc->SetPenStyle((CGraphicsContext::TPenStyle)(iLinePattern));
  gc->SetPenColor(iForegroundColor);
  gc->SetBrushStyle((CGraphicsContext::TBrushStyle)(iFillPattern));
  gc->SetBrushColor(iBackgroundColor);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::DrawImage
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::DrawImage (const NW_GDI_Point2D_t* aLocation,
                             const NW_GDI_Rectangle_t* aRectangle,
                             NW_GDI_ImageBlock_t* aImageBlock)
{
  NW_GDI_Rectangle_t imageRect;
  NW_GDI_Rectangle_t clipRect;

  // parameter assertion block
  NW_ASSERT (aLocation);

  NW_REQUIRED_PARAM(aRectangle);

  InitializeGraphicsContext();

  CBitmapContext* gc = Epoc32GraphicsContext() ;
  CFbsBitmap* bitmap = (CFbsBitmap*) aImageBlock->data;

  NW_ASSERT( bitmap );

  TPoint point( aLocation->x - iOrigin.x + iDisplayBounds.point.x,
                aLocation->y - iOrigin.y + iDisplayBounds.point.y );


  TRect rect( aRectangle->point.x, aRectangle->point.y,
				aRectangle->dimension.width + aRectangle->point.x,
				aRectangle->dimension.height + aRectangle->point.y );

  imageRect.point.x = aLocation->x;
  imageRect.point.y = aLocation->y;
  imageRect.dimension.width = aRectangle->dimension.width;
  imageRect.dimension.height = aRectangle->dimension.height;

  clipRect = ClipRect();
  if ( NW_GDI_Rectangle_Cross( &imageRect, &clipRect, NULL ) == NW_TRUE )
  {
    // Set brush style to NULL to prevent filling of aRectangle
    gc->SetBrushStyle (CGraphicsContext::ENullBrush);

    if ( !aImageBlock->mask ){
      gc->BitBlt( point, bitmap, rect );
    }
    else {
      CFbsBitmap* mask = (CFbsBitmap*) aImageBlock->mask;
      gc->BitBltMasked( point, bitmap, rect, mask, ((aImageBlock->invertedMask == NW_TRUE) ? ETrue : EFalse) );
    }
  }
}


// -------------------------------------------------------------------------
// CGDIDeviceContext::ScaleImage
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::ScaleImage (const NW_GDI_Dimension3D_t* size,
                               const NW_GDI_ImageBlock_t* imageBlock,
                               NW_GDI_ImageBlock_t* scaledImageBlock)
{
  CFbsBitmap* scaledBitmap = NULL;
  CFbsBitmap* scaledMask = NULL;

  // parameter assertion block
  NW_ASSERT (imageBlock);
  NW_ASSERT (size);
  NW_ASSERT (scaledImageBlock);
  NW_ASSERT (imageBlock->data);

  NW_TRY (status)
  {
    TRect rect (0, 0, size->width, size->height);

    // scale bitmap
    status = ScaleBitmap ((CFbsBitmap*)imageBlock->data, &scaledBitmap, &rect);
    _NW_THROW_ON_ERROR (status);
    scaledImageBlock->data = scaledBitmap;

    // scale mask, if any
    if ( imageBlock->mask )
    {
      status = ScaleBitmap((CFbsBitmap*)imageBlock->mask, &scaledMask, &rect);
      _NW_THROW_ON_ERROR (status);
      scaledImageBlock->mask = scaledMask;
    }
    else
    {
      scaledImageBlock->mask = NULL;
    }

    // set the rest of the scaled image parameters
    scaledImageBlock->size = *size;
    scaledImageBlock->conversionStatus = imageBlock->conversionStatus;
    scaledImageBlock->transparencyIndex = imageBlock->transparencyIndex;
    scaledImageBlock->isTransparent = imageBlock->isTransparent;
    scaledImageBlock->colPalette = imageBlock->colPalette;
  }
  NW_CATCH (status)
  {
    delete scaledBitmap;
    delete scaledMask;
  }
  NW_FINALLY
  {
    return status;
  }
  NW_END_TRY
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::DrawPixel
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::DrawPixel (const NW_GDI_Point2D_t* aLocation)
{
  // parameter assertion block
  NW_ASSERT (aLocation);

  // for convenience
  InitializeGraphicsContext();
  Epoc32GraphicsContext()->Plot
      ( TPoint( aLocation->x - iOrigin.x + iDisplayBounds.point.x,
                aLocation->y - iOrigin.y + iDisplayBounds.point.y ) );
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::DrawPolyline
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::DrawPolyline (NW_GDI_Metric_t aNumPoints,
                                 const NW_GDI_Point2D_t* aPoints,
                                 TBool aClosed)
{
  NW_GDI_Color_t initialColor = 0;

  // parameter assertion block
  NW_ASSERT (aPoints);

  // for convenience
  if (iLineWidth == 0){
    return KBrsrSuccess;
  }

  initialColor = iForegroundColor;

  CBitmapContext* gc = Epoc32GraphicsContext();

  InitializeGraphicsContext();

  // convert NW_Point2D array into TPoint array
  TPoint* epocPoints=new TPoint[aNumPoints];

  if (epocPoints)
  {
    for (TInt ii=0; ii<aNumPoints; ii++)
    {
      epocPoints[ii]=TPoint( aPoints[ii].x - iOrigin.x + iDisplayBounds.point.x,
                             aPoints[ii].y - iOrigin.y + iDisplayBounds.point.y);
    }
    gc->DrawPolyLine(epocPoints, aNumPoints);
    delete[] epocPoints;
  }

  if (aClosed && aNumPoints>2)
  {
    DrawLine(aPoints[0], aPoints[aNumPoints-1]);
  }
  iForegroundColor = initialColor;
  // successful completion
  return KBrsrSuccess;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::DrawaRectangle
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::DrawRectangle (const NW_GDI_Rectangle_t* aRectangle)
{
  NW_GDI_Rectangle_t clipRect;
  NW_GDI_Color_t initialColor = 0;

  CBitmapContext* gc = Epoc32GraphicsContext();

  clipRect = ClipRect();

  initialColor = iForegroundColor;

  if ( NW_GDI_Rectangle_Cross( aRectangle, &clipRect, NULL ) == NW_TRUE )
  {
    InitializeGraphicsContext();

    // Style to null for NO-FILL in aRectangle
    gc->SetBrushStyle (CGraphicsContext::ENullBrush);
    gc->DrawRect( TRect( aRectangle->point.x - iOrigin.x + iDisplayBounds.point.x,
                  aRectangle->point.y - iOrigin.y + iDisplayBounds.point.y,
                  aRectangle->point.x - iOrigin.x + iDisplayBounds.point.x + aRectangle->dimension.width,
                  aRectangle->point.y - iOrigin.y + iDisplayBounds.point.y + aRectangle->dimension.height));
  }
  iForegroundColor = initialColor;
}


// -------------------------------------------------------------------------
// CGDIDeviceContext::DrawText
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::DrawText (const NW_GDI_Point2D_t* aLocation,
                             const NW_Text_t* aText,
                             CGDIFont* aGDIFont,
                             NW_GDI_Metric_t aSpacing,
                             NW_GDI_TextDecoration_t aDecoration,
                             NW_GDI_FlowDirection_t aGlobalDir,
                             NW_GDI_Metric_t aWidth)
  {
  NW_Text_Length_t length;
  NW_Text_Length_t trimmedLen;
  NW_Text_Length_t i;
  TText16* string = NULL;
  TBool freeNeeded = NW_FALSE;
  NW_GDI_Color_t initialColor = 0;

  // parameter assertion block
  NW_ASSERT (aLocation);
  NW_ASSERT (aText);

  NW_TRY (status)
    {
    // Get the aText as unicode
    string =
      NW_Text_GetUCS2Buffer (aText,
                             NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                             &length,
                             (unsigned char*) &freeNeeded);
    if (length == 0)
      {
      NW_THROW_STATUS (status, KBrsrSuccess);
      }

    if (!string)
      {
      NW_THROW_STATUS (status, KBrsrFailure);
      }

    // Trim the string
    trimmedLen = 0;
    for (i = 0; i < length; i++)
      {
      if ((string[i] == NW_TEXT_UCS2_SOFT_HYPHEN) || (string[i] == NW_TEXT_UCS2_LF)
          || (string[i] == NW_TEXT_UCS2_CR))
        {
          continue;
        }
      else if ((string[i] == NW_TEXT_UCS2_TAB) || (string[i] == NW_TEXT_UCS2_NBSP))
        {
        // In vertical-layout mode we want to ignore nbsp within a table cell
        if (iRemoveNbsp)
          {
          continue;
          }
          string[trimmedLen++] = ' ';
        }
      else
        {
        string[trimmedLen++] = string[i];
        }
      }
    string[trimmedLen] = 0;

    if (trimmedLen == 0)
      {
      NW_THROW_STATUS (status, KBrsrSuccess);
      }

    initialColor = iForegroundColor;

    CBitmapContext* gc = Epoc32GraphicsContext();

    CFont* epocFont = NULL;
    epocFont = (CFont*)aGDIFont->GetEpocNativeFont();

 	// Lowercase letters will be rendered as uppercase characters
    // Font size remains intact (according to spec)

    if(aGDIFont->FontVariant() == EGDIFontVariantSmallCaps) {
      if(!freeNeeded) {
        string = NW_Text_GetUCS2Buffer(aText, NW_Text_Flags_Copy, NULL, NULL);
       NW_THROW_OOM_ON_NULL (string, status);

       freeNeeded = NW_TRUE;
      }
      NW_Str_Strupr((TText16*)string);
      }

    CGraphicsContext::TDrawTextParam params;
    params.iCharJustExcess=aSpacing;
    InitializeGraphicsContext();
    gc->UseFont(epocFont);

    //Calculate Baseline offset
    //Baseline is calculated as (Height + ascent in pixels - descent in pixels) /2
    //iDisplayBounds and iOrigin are kept as it is to make sure drawing is respective to
    //display bound
    TInt heightInPixels = epocFont->HeightInPixels();
    TInt descentInPixels = epocFont->DescentInPixels();
    TInt acsentInPixels = epocFont->FontMaxAscent();
    TInt baseLineOffset = (iDisplayBounds.point.y) + (heightInPixels + acsentInPixels - descentInPixels ) / (2) - iOrigin.y; 
    
    TPtrC ptrC(string);
    TPoint point( aLocation->x - iOrigin.x + iDisplayBounds.point.x,
                  aLocation->y + baseLineOffset);

    if (aDecoration & NW_GDI_TextDecoration_Underline) {
      gc->SetUnderlineStyle(EUnderlineOn);
    }

    if (aDecoration & NW_GDI_TextDecoration_Strikeout) {
      gc->SetStrikethroughStyle(EStrikethroughOn);
    }

    // Draw the aText either using the TBidiText sophicated method
    // or the simple CGraphicsContext method if the orginal encoding is 8859-8
    CGraphicsContext::TTextAlign alignment = CGraphicsContext::ELeft;
    TInt ret = KErrNone;

    if (aGlobalDir == NW_GDI_FlowDirection_RightLeft) {
      alignment = CGraphicsContext::ERight;
    }

    // When aText is encoded with HTTP_iso_8859_8 CGraphicsContext::DrawText
    // is used instead of TBidiText::DrawText.  This is done because this encoding
    // implies that the buffer should be rendered as is if aGlobalDir is left to right
    // or reversed and rendered if aGlobalDir is right to left (including latin chars).
    if (NW_Settings_GetOriginalEncoding() == HTTP_iso_8859_8) {
      if (aGlobalDir == NW_GDI_FlowDirection_RightLeft) {
        NW_Text_Length_t len = NW_Str_Strlen(string);

        // Reverse the aText first then draw it
        for (NW_Text_Length_t i = 0; i < (len / 2); i++) {
          TText16 temp;

          temp = string[i];
          string[i] = string[len - 1 - i];
          string[len - 1 - i] = temp;
        }
      }

      gc->DrawText(ptrC, point);
      iOocEpoc32View->DrawPictographsInText(*gc, *epocFont, ptrC, point);
   }

    // Otherwise use TBidiText to render the aText which correctly renders
    // hebrew chars right to left or left to right depending on aGlobalDir --
    // embedded latin chars are always rendered left to right.
    else {
      TBidiText *bidiText = NULL;

      // Wrap the aText in a TBidiText to allow bi-directional flow.
      TRAP(ret, bidiText = TBidiText::NewL(ptrC, 1,
        (aGlobalDir == NW_GDI_FlowDirection_LeftRight) ?
        TBidiText::ELeftToRight : TBidiText::ERightToLeft));

      if (ret != KErrNone) {
        NW_THROW_STATUS (status, KBrsrFailure);
      }

      bidiText->WrapText(aWidth, *epocFont, 0);
      bidiText->DrawText(*gc, point, aSpacing, alignment);
      iOocEpoc32View->DrawPictographsInText(*gc, *epocFont, ptrC, point);

      delete bidiText;
    }

    if (aDecoration & NW_GDI_TextDecoration_Underline) {
      gc->SetUnderlineStyle(EUnderlineOff);
    }

    if (aDecoration & NW_GDI_TextDecoration_Strikeout) {
      gc->SetStrikethroughStyle(EStrikethroughOff);
    }

    gc->DiscardFont();

    iForegroundColor = initialColor;

  } NW_CATCH (status) {
  } NW_FINALLY {

    if (freeNeeded){
      NW_Mem_Free (string);
    }

    return status;
  } NW_END_TRY;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::FillaRectangle
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::FillRectangle (const NW_GDI_Rectangle_t* aRectangle)
{
  NW_GDI_Rectangle_t clipRect;

  // parameter assertion block
  NW_ASSERT (aRectangle);

  // for convenience
  clipRect = ClipRect();
  if ( NW_GDI_Rectangle_Cross( aRectangle, &clipRect, NULL ) == NW_TRUE )
  {
    CBitmapContext* gc = Epoc32GraphicsContext();

    InitializeGraphicsContext ();

    gc->SetPenColor(iBackgroundColor);
    gc->DrawRect(TRect(aRectangle->point.x - iOrigin.x + iDisplayBounds.point.x,
             aRectangle->point.y - iOrigin.y + iDisplayBounds.point.y,
             aRectangle->point.x - iOrigin.x + iDisplayBounds.point.x + aRectangle->dimension.width,
             aRectangle->point.y - iOrigin.y + iDisplayBounds.point.y + aRectangle->dimension.height));
  }
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::GetColorPalette
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::GetColorPalette (NW_GDI_ColorPalette_t* aColPalette)
{
  // parameter assertion block
  NW_ASSERT (aColPalette);
  NW_REQUIRED_PARAM (aColPalette);

	// Get the system palette entries

  /***********************************************************
   *  PORTING NOTE:
   *
   *  Write some statements similar to the next lines (NT
   *  implementation reference) based on your platform properties:
   *
   *  GetSystemPaletteEntries(this->hdc,0,256,pe);
   *
   *  for(j = 0; j< 256; j++)
   *  {
   *	 aColPalette[j].Red   = pe[j].peRed;
   *	 aColPalette[j].Green = pe[j].peGreen;
   *	 aColPalette[j].Blue  = pe[j].peBlue;
   *  }
   ************************************************************/

  // successful completion
  return KBrsrNotImplemented;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::BackgroundColor
//
// -----------------------------------------------------------------------------
//
NW_GDI_Color_t
CGDIDeviceContext::BackgroundColor () const
{
  CGDIDeviceContext* self = const_cast<CGDIDeviceContext*> (this);
  return self->SwapColorBytes(iBackgroundColor);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::ClipRect
//
// -----------------------------------------------------------------------------
//
NW_GDI_Rectangle_t
CGDIDeviceContext::ClipRect () const
{
  NW_GDI_Rectangle_t clip;

  clip.point.x = (NW_GDI_Metric_t)
                  (iClipRect.point.x + iOrigin.x - iDisplayBounds.point.x);

  clip.point.y = (NW_GDI_Metric_t)
                  (iClipRect.point.y + iOrigin.y - iDisplayBounds.point.y);

  clip.dimension.width = (NW_GDI_Metric_t)(iClipRect.dimension.width);

  clip.dimension.height = (NW_GDI_Metric_t)(iClipRect.dimension.height);

  return clip;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::DisplayBounds
//
// -----------------------------------------------------------------------------
//
const NW_GDI_Rectangle_t*
CGDIDeviceContext::DisplayBounds () const
{
  // displayBounds
  return &(iDisplayBounds);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::DisplayDepth
//
// -----------------------------------------------------------------------------
//
TUint8
CGDIDeviceContext::DisplayDepth () const
{
  // return the desired property

  // Gets 'bits per color' property of the display
  CGDIDeviceContext* self = const_cast<CGDIDeviceContext*> (this);
  switch(self->Epoc32GraphicsContext()->Device()->DisplayMode())
  {
    case EGray2:
	    return 1;
    case EGray4:
	    return 2;
    case EGray16:case EColor16:
	    return 4;
    case EGray256:case EColor256:
	    return 8;
    case EColor64K:
	    return 16;
    case EColor16M:
	    return 24;
    case EColor4K:
	    return 12;
    default:
	    return 0;
  }
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::FillPattern
//
// -----------------------------------------------------------------------------
//
NW_GDI_Pattern_t
CGDIDeviceContext::FillPattern () const
{
  // return the desired property
  return iFillPattern;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::GetFlowDirection
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::GetFlowDirection (NW_Text_t* aText,
                                     NW_GDI_FlowDirection_t* aFlowDirection) const
{
  TText16* string;
  NW_Text_Length_t strLen = 0;
  TBool freeNeeded = NW_FALSE;
  TPtrC textPtr;

  NW_ASSERT ( aFlowDirection );

  NW_TRY (status) {

    // Set the default response
    *aFlowDirection = NW_GDI_FlowDirection_LeftRight;

    // Set the aText string*/
    string =
      NW_Text_GetUCS2Buffer (aText,
                             NW_Text_Flags_Aligned | NW_Text_Flags_NullTerminated,
                             &strLen,
                             (unsigned char*) &freeNeeded);
    NW_THROW_OOM_ON_NULL (string, status);

    // If no aText, we're done
    if (strLen == 0) {
      return KBrsrSuccess;
    }
    textPtr.Set(string, strLen);
    TBool found;
    TBidiText::TDirectionality bidiDir = TBidiText::TextDirectionality(textPtr, &found);
    *aFlowDirection = (bidiDir == TBidiText::ELeftToRight) ? NW_GDI_FlowDirection_LeftRight : NW_GDI_FlowDirection_RightLeft;

  } NW_CATCH (status) {
  } NW_FINALLY {
    if (freeNeeded) NW_Mem_Free (string);
    return status;
  } NW_END_TRY
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::ForegroundColor
//
// -----------------------------------------------------------------------------
//
NW_GDI_Color_t
CGDIDeviceContext::ForegroundColor () const
{
  CGDIDeviceContext* self = const_cast<CGDIDeviceContext*> (this);
  return self->SwapColorBytes(iForegroundColor);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::LinePattern
//
// -----------------------------------------------------------------------------
//
NW_GDI_Pattern_t
CGDIDeviceContext::LinePattern () const
{
  return iLinePattern;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::LineWidth
//
// -----------------------------------------------------------------------------
//
NW_GDI_Metric_t
CGDIDeviceContext::LineWidth () const
{
  return iLineWidth;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::Origin
//
// -----------------------------------------------------------------------------
//
const NW_GDI_Point2D_t*
CGDIDeviceContext::Origin () const
{
  // return the desired property
  return &iOrigin;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::PaintMode
//
// -----------------------------------------------------------------------------
//
NW_GDI_PaintMode_t
CGDIDeviceContext::PaintMode () const
{
  return iPaintMode;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::GetScaledCFont
// This method takes GDI font as a parameter, updates its size according
// to font size level and returns corresponding CFont*
//
// -----------------------------------------------------------------------------
//
void*
CGDIDeviceContext::GetScaledCFont (CGDIFont* aFont) const
    {
    // parameter assertion block
    NW_ASSERT (aFont);

  return (CFont*)aFont->GetEpocNativeFont();
    }

// -------------------------------------------------------------------------
// CGDIDeviceContext::TextDirection
//
// -----------------------------------------------------------------------------
//
NW_GDI_TextDirection_t
CGDIDeviceContext::TextDirection () const
{
  // return the desired property
  return iTextDirection;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::GetTextExtent
// The purpose of this function is to calculate the length of a string (in
// pixels
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::GetTextExtent (const NW_Text_t* aText,
                                  CGDIFont* aGDIFont,
                                  NW_GDI_Metric_t aSpacing,
                                  NW_GDI_FlowDirection_t aFlowDirection,
                                  NW_GDI_Dimension2D_t* aExtent) const
  {
  TText16* string = NULL;
  NW_Text_Length_t length;
  NW_Text_Length_t trimmedLen;
  NW_Text_Length_t i;
  TBool freeNeeded = NW_FALSE;

  NW_REQUIRED_PARAM(aSpacing);

  // parameter assertion block
  NW_ASSERT (aText);
  NW_ASSERT (aExtent);

  NW_TRY (status)
    {
    (void) NW_Mem_memset (aExtent, 0, sizeof (*aExtent));

    // Get the aText as unicode
    string =
      NW_Text_GetUCS2Buffer (aText,
                             NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                             &length,
                             (unsigned char*) &freeNeeded);

    if (length == 0)
      {
      NW_THROW_STATUS (status, KBrsrSuccess);
      }

    if (!string)
      {
      NW_THROW_STATUS (status, KBrsrOutOfMemory);
      }

    // Trim the string
    trimmedLen = 0;
    for (i = 0; i < length; i++)
      {
      if ((string[i] == NW_TEXT_UCS2_SOFT_HYPHEN) || (string[i] == NW_TEXT_UCS2_CR))
        {
        continue;
        }
      else if ((string[i] == NW_TEXT_UCS2_TAB) || (string[i] == NW_TEXT_UCS2_NBSP))
        {
        // In vertical-layout mode we want to ignore nbsp within a table cell
        if (iRemoveNbsp)
          {
          continue;
          }
        string[trimmedLen++] = ' ';
        }
      else
        {
        string[trimmedLen++] = string[i];
   	    }
      }
    string[trimmedLen] = 0;

 	// Lowercase letters will be rendered as uppercase characters
    // Font size remains intact (according to spec)

    if(aGDIFont->FontVariant() == EGDIFontVariantSmallCaps)
      {
      if(!freeNeeded)
        {
        string = NW_Text_GetUCS2Buffer(aText, NW_Text_Flags_Copy, NULL, NULL);
        NW_THROW_OOM_ON_NULL (string, status);

        freeNeeded = NW_TRUE;
        }
        NW_Str_Strupr((TText16*)string);
      }

    CFont* epocFont = (CFont*)aGDIFont->GetEpocNativeFont();

    TPtrC ptrC(string);

    TInt width = epocFont->TextWidthInPixels(ptrC);
    width += (length-1)*aSpacing;

    TInt height = 0;
    if (width)
      {
      height = epocFont->HeightInPixels();
      }

    if (aFlowDirection == NW_GDI_FlowDirection_LeftRight ||
        aFlowDirection == NW_GDI_FlowDirection_RightLeft)
      {
      aExtent->width=(NW_GDI_Metric_t)width;
      aExtent->height=(NW_GDI_Metric_t)height;
      }
    else
      {
      aExtent->width=(NW_GDI_Metric_t)height;
      aExtent->height=(NW_GDI_Metric_t)width;
      }

    if (aExtent->width)
      {
      aExtent->width = (NW_GDI_Metric_t)(aExtent->width + 1);
      }
    if (aExtent->height)
      {
      if (iVariantFlag == EApacVariant)
        {
        aExtent->height = (NW_GDI_Metric_t)(aExtent->height + 3);
        }
      else
        {
		aExtent->height = (NW_GDI_Metric_t)(aExtent->height + 1);
		}
      }

    }
  NW_CATCH (status)
    {
    }
  NW_FINALLY
    {
    // cleanup!
    if (freeNeeded)
      {
      NW_Mem_Free (string);
      }
    // successful completion
    return status;
    }
  NW_END_TRY;
  }

// -------------------------------------------------------------------------
// CGDIDeviceContext::PreDraw
// The purpose of this function is to clean up the screen for next draw
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::PreDraw ()
{
  NW_GDI_Rectangle_t rect;

  rect.point.x = (NW_GDI_Metric_t)(iClipRect.point.x + iOrigin.x);
  rect.point.y = (NW_GDI_Metric_t)(iClipRect.point.y + iOrigin.y);
  rect.dimension.width = iClipRect.dimension.width;
  rect.dimension.height = iClipRect.dimension.height;

  FillRectangle(&rect);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::PostDraw
// The purpose of this function is to redraw the screen partially.  redraw
// area should be given
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::PostDraw ( TBool aDrawNow)
{
  iOocEpoc32View->EpocDrawNow ((unsigned char)aDrawNow);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetBackgroundColor
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetBackgroundColor (NW_GDI_Color_t aColor)
{
  iBackgroundColor = SwapColorBytes(aColor);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetClipRect
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetClipRect (const NW_GDI_Rectangle_t* aRectangle)
{
  if(aRectangle) {
    iClipRect.point.x = (NW_GDI_Metric_t)
                                ((aRectangle->point.x - iOrigin.x + iDisplayBounds.point.x) > 0 ?
                                 (aRectangle->point.x - iOrigin.x + iDisplayBounds.point.x) : 0);

    iClipRect.point.y = (NW_GDI_Metric_t)
                                ((aRectangle->point.y - iOrigin.y + iDisplayBounds.point.y) > 0 ?
                                 (aRectangle->point.y - iOrigin.y + iDisplayBounds.point.y) : 0);
    iClipRect.dimension.width = (NW_GDI_Metric_t)((aRectangle->dimension.width + 1) >
                                        iDisplayBounds.dimension.width ?
                                        iDisplayBounds.dimension.width :
                                        aRectangle->dimension.width);
    iClipRect.dimension.height = (NW_GDI_Metric_t)((aRectangle->dimension.height + 1) >
                                         iDisplayBounds.dimension.height ?
                                         iDisplayBounds.dimension.height :
                                         aRectangle->dimension.height);
  }
  else {
    iClipRect = iDisplayBounds;
  }
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetFillPattern
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetFillPattern (NW_GDI_Pattern_t aFillPattern)
{
  iFillPattern = aFillPattern;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetForegroundColor
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetForegroundColor (NW_GDI_Color_t aColor)
{
  iForegroundColor = SwapColorBytes(aColor);
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetLinePattern
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetLinePattern (NW_GDI_Pattern_t aLinePattern)
{
  iLinePattern = aLinePattern;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetLineWidth
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetLineWidth (NW_GDI_Metric_t aLineWidth)
{
  // set the property
  iLineWidth = aLineWidth;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetOrigin
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetOrigin (const NW_GDI_Point2D_t* aOrigin)
{
  NW_ASSERT(aOrigin);

  // set the property
  iOrigin=*aOrigin;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetPaintMode
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetPaintMode (NW_GDI_PaintMode_t aPaintMode)
{
  // set the property
  iPaintMode = aPaintMode;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetTextDirection
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetTextDirection (NW_GDI_TextDirection_t aTextDirection)
{
  NW_REQUIRED_PARAM (aTextDirection);

  // set the property
  iTextDirection = aTextDirection;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetDisplayBounds
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetDisplayBounds (const NW_GDI_Rectangle_t* aRectangle)
{
  // set the desired property
  iDisplayBounds = *aRectangle;
  iClipRect = iDisplayBounds;
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::CreateFont
//
// -----------------------------------------------------------------------------
//
CGDIFont*
CGDIDeviceContext::CreateFont (const NW_Text_t* aFamily,
                               TGDIFontStyle aStyle,
                               TUint8 aSizeInPixels,
                               TUint16 aWeight,
                               TGDIFontVariant aVariant) const
    {
    // ignore err, because function is handling the error when font created is NULL
    CGDIFont* font = NULL;

    // convert font size from pixels to twips
    TUint16 sizeInTwips = (TUint16)iWsScreenDev->VerticalPixelsToTwips((TInt)aSizeInPixels);

	TInt err;
    TRAP( err, font = CGDIFont::NewL( this, aFamily, aStyle, sizeInTwips, aWeight, aVariant ) );
    return font;
    }

// -------------------------------------------------------------------------
// CGDIDeviceContext::DrawLine
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::DrawLine (NW_GDI_Point2D_t aStartPoint,
                                      NW_GDI_Point2D_t aEndPoint)
{
  CBitmapContext* gc = Epoc32GraphicsContext();
	if (iLinePattern != NW_GDI_Pattern_None)
  {
		gc->SetPenStyle((CGraphicsContext::TPenStyle)(iLinePattern));
  }

	gc->DrawLine( TPoint( aStartPoint.x - iOrigin.x + iDisplayBounds.point.x,
                          aStartPoint.y - iOrigin.y + iDisplayBounds.point.y),
                  TPoint( aEndPoint.x - iOrigin.x + iDisplayBounds.point.x,
                          aEndPoint.y - iOrigin.y + iDisplayBounds.point.y ) );
}

// -------------------------------------------------------------------------
// CGDIDeviceContext::SetFontSizeLevel
//
// -----------------------------------------------------------------------------
//
TBool
CGDIDeviceContext::SetFontSizeLevel ()
{
  TGDIDeviceContextFontSizeLevel fontSizeLevel;

  fontSizeLevel = (TGDIDeviceContextFontSizeLevel)NW_Settings_GetFontSizeLevel();
  if (iFontSizeLevel == fontSizeLevel){
    return NW_FALSE;
  }

  switch(fontSizeLevel)
  {
    case EGDIDeviceContextFontSizeLevelAllSmall:
    case EGDIDeviceContextFontSizeLevelSmaller:
    case EGDIDeviceContextFontSizeLevelNormal:
    case EGDIDeviceContextFontSizeLevelLarger:
    case EGDIDeviceContextFontSizeLevelAllLarge:
      iFontSizeLevel = fontSizeLevel;
      return NW_TRUE;

    default:
      return NW_FALSE;
  }

}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::ChangeFontSize
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::ChangeFontSize (CGDIFont* aFont)
{
  TInt16 fontSize;
  TInt16 difference;
  TInt16 newFontSize;

  switch(iFontSizeLevel)
  {
    case EGDIDeviceContextFontSizeLevelAllSmall:
      // show text 40% smaller than the declared size
      fontSize = aFont->DeclaredFontSize();
      difference = (TInt16)((float)fontSize / 2.5);
      newFontSize = (TInt16)(fontSize - difference);

      aFont->SetFontSize((TUint16)newFontSize);
      return;
    case EGDIDeviceContextFontSizeLevelSmaller:
      // show text 20% smaller than the declared size
      fontSize = aFont->DeclaredFontSize();
      difference = (TInt16)((float)fontSize / 5);
      newFontSize = (TInt16)(fontSize - difference);

      aFont->SetFontSize((TUint16)newFontSize);
      return;
    case EGDIDeviceContextFontSizeLevelNormal:
      // show text to be the declared size
      fontSize = aFont->DeclaredFontSize();

      aFont->SetFontSize((TUint16)fontSize);
      return;
    case EGDIDeviceContextFontSizeLevelLarger:
      // show text 20% larger than the declared size
      fontSize = aFont->DeclaredFontSize();
      difference = (TInt16)((float)fontSize / 5);
      newFontSize = (TInt16)(fontSize + difference);

      aFont->SetFontSize((TUint16)newFontSize);
      return;
    case EGDIDeviceContextFontSizeLevelAllLarge:
      // show text 40% larger than the declared size
      fontSize = aFont->DeclaredFontSize();
      difference = (TInt16)((float)fontSize / 2.5);
      newFontSize = (TInt16)(fontSize + difference);

      aFont->SetFontSize((TUint16)newFontSize);
      return;
    default:
      return;
  }
}

// ----------------------------------------------------------------------------
// CGDIDeviceContext::GetHighlightColor
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::GetHighlightColor (NW_GDI_Color_t* aColor)
{
  NW_ASSERT (aColor);
  TRgb col = CEikonEnv::Static()->Color(EColorControlHighlightBackground);

  *aColor = NW_GDI_RGB(col.Red(), col.Green(), col.Blue());
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::SwitchAgainstBG
// switch the color to white if the background color is dark enough,
// i.e. darker than half of the white color indensity.
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SwitchAgainstBG( NW_GDI_Color_t* aColor)
{
  NW_GDI_Color_t rValue = 0;
  NW_GDI_Color_t gValue = 0;
  NW_GDI_Color_t bValue = 0;
  NW_GDI_Color_t bgColor = 0;

  bgColor = BackgroundColor();

  rValue = NW_GDI_GetRValue(bgColor);
  gValue = NW_GDI_GetGValue(bgColor);
  bValue = NW_GDI_GetBValue(bgColor);

  if ((rValue+gValue+bValue) < 384){
    *aColor = SwapColorBytes(NW_GDI_Color_White);
  }
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::SplitText
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::SplitText (NW_Text_t* aText,
                              CGDIFont* aGDIFont,
                              NW_GDI_Metric_t aConstraint,
                              NW_GDI_Metric_t aSpacing,
                              NW_GDI_FlowDirection_t aFlowDirection,
                              NW_Text_Length_t* aLength,
                              TUint8 aMode)
  {
  if (aMode == NW_GDI_SplitMode_Newline)
    {
    return SplitAtNewline (aText, aGDIFont, aConstraint, aSpacing, aFlowDirection, aLength);
    }
  else if (aMode == NW_GDI_SplitMode_Clip)
    {
    return Truncate (aText, aGDIFont, aConstraint, aSpacing, aFlowDirection, aLength);
    }
  else
    {
    return WordWrap(aText, aGDIFont, aConstraint, aSpacing, aFlowDirection, aLength);
    }
  }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::GetSubstringExtent
//
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
CGDIDeviceContext::GetSubstringExtent (const NW_Text_t* aText,
                                       CGDIFont* aGDIFont,
                                       NW_Text_Length_t aStart,
                                       NW_Text_Length_t aLength,
                                       NW_GDI_Metric_t aSpacing,
                                       NW_GDI_FlowDirection_t aFlowDirection,
                                       NW_GDI_Dimension2D_t* aExtent)
{
  NW_Text_t* frag = NULL;
  NW_Text_Length_t totalLength;
  TText16* storage = NULL;
  TBool freeNeeded = NW_FALSE;

  NW_Mem_memset(aExtent, 0, sizeof(NW_GDI_Dimension2D_t));

  NW_TRY(status) {

    storage = NW_Text_GetUCS2Buffer (aText, 0, &totalLength, (unsigned char*) &freeNeeded);
    NW_THROW_OOM_ON_NULL(storage, status);

    if (totalLength < aStart + aLength) {
      NW_THROW_STATUS(status, KBrsrFailure);
    }

    if (aLength == 0) {
      NW_THROW_SUCCESS(status);
    }

    frag = (NW_Text_t*)NW_Text_UCS2_New(&storage[aStart], aLength, 0);
    NW_THROW_OOM_ON_NULL(frag, status);

    status = GetTextExtent (frag, aGDIFont, aSpacing, aFlowDirection, aExtent);
    NW_THROW_ON_ERROR(status);
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {

    if (freeNeeded) {
      NW_Str_Delete(storage);
    }
    NW_Object_Delete(frag);

    return status;
  } NW_END_TRY;
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::FindFont
//
// -----------------------------------------------------------------------------
//
CFont*
CGDIDeviceContext::FindFont (TFontSpec aFontSpec)
    {
	CFont* font = (CFont*)iFontCache->Search(aFontSpec);

    if (!font) // not found in cache
        {
        // we should always get a font back from the Avkon
        font = AknFontAccess::GetClosestFont( *iWsScreenDev,
                                              aFontSpec.iFontStyle,
                                              aFontSpec.iHeight,
                                              aFontSpec.iTypeface.iName );

        CFont* discardFont = iFontCache->AddEntryL( (CFont*&)font, aFontSpec );
        if (discardFont)
            {
            iWsScreenDev->ReleaseFont( discardFont );
            }
        }

    return font;
    }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::SetRemoveNbsp
//
// -----------------------------------------------------------------------------
void
CGDIDeviceContext::SetRemoveNbsp( TBool aFlag )
    {
    iRemoveNbsp = aFlag;
    }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::RemoveNbsp
//
// -----------------------------------------------------------------------------
//
TBool
CGDIDeviceContext::RemoveNbsp()
{
  return iRemoveNbsp;
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::SetGraphicsContext
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetGraphicsContext( void* aGraphicsContext )
{
  iGraphicsContext = aGraphicsContext;
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::SetFontSizeLevel
//
// -----------------------------------------------------------------------------
//
void
CGDIDeviceContext::SetFontSizeLevel( TGDIDeviceContextFontSizeLevel aFontSizeLevel )
{
  iFontSizeLevel = aFontSizeLevel;
}

// -----------------------------------------------------------------------------
// CGDIDeviceContext::VariantFlag
//
// -----------------------------------------------------------------------------
//
EVariantFlag
CGDIDeviceContext::VariantFlag()
{
  return iVariantFlag;
}

TGDISupportedLanguage
CGDIDeviceContext::SupportedLanguage()
    {
    return iSupportedLanguage;
    }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::CanSplit
//
// -----------------------------------------------------------------------------
//
TBool
CGDIDeviceContext::CanSplit( TText16 aChar1, TText16 aChar2 )
    {
    CGDIDeviceContext* self = const_cast<CGDIDeviceContext*> (this);
    if ( (aChar1 == 0) || (aChar2 == 0) ||
          NW_Str_Isspace( aChar1 ) || NW_Str_Isspace( aChar2 ) ||
         (aChar1 == NW_TEXT_UCS2_HYPHEN) ||
         (aChar1 == NW_TEXT_UCS2_SOFT_HYPHEN) )
        {
        return ETrue;
        }

    // Special case the Thai language, unicode chars 0x0E00 - 0x0E7F. For more
    // details, see description of Thai characters at the top of this file.
    //
    // The description at the top of this file defined 3 concise rules for
    // splitting:
    //   1. Splits can occur at whitespace.
    //   2. Splits can occur immediately preceding any LV.
    //   3. Splits can occur immediately preceding any CONS that is not
    //     immediately preceded by an LV.
    //
    // Rule #1, dealing with whitespace, was handled above. Here, rules 2 and
    // 3 are handled.
    if ( IS_THAI_CHAR( aChar1) && IS_THAI_CHAR( aChar2 ) )
        {
        // Rule #2: Splits can occur immediately preceding any LV.
        if ( IS_THAI_LEADING_VOWEL( aChar2 ) )
            {
            return ETrue;
            }
        // Rule #3: Splits can occur immediately preceding any CONS that is not
        //     immediately preceded by an LV.
        if ( !IS_THAI_LEADING_VOWEL( aChar1 ) && IS_THAI_CONSONANT( aChar2 ) )
            {
            return ETrue;
            }
        }

    //For Chinese string and other character-based language string can be broke between any two characters.
    //Also we considered the case if some western words are embeded in a APAC string.
        else if (self->VariantFlag() == EApacVariant && (aChar2 > 255 || aChar1 > 255) )
	{
        return ETrue;
      }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CGDIDeviceContext::IsThaiCell
//
// -----------------------------------------------------------------------------
//
TBool
CGDIDeviceContext::IsThaiCell( TText16 aChar1, TText16 aChar2 )
    {
    if ( IS_THAI_CHAR( aChar1 ) && IS_THAI_CHAR( aChar2) )
        {
        // The following "if" statements are not arranged how one would
        // traverse a Thai script "cell" from left to right. Instead, they
        // are arranged by which is most likely to occur; thereby maximizing
        // performance, albeit only slightly.
        if ( (IS_THAI_COMBINING( aChar1 ) || IS_THAI_CONSONANT( aChar1 ))
            && (IS_THAI_COMBINING( aChar2 ) || IS_THAI_FOLLOWING_VOWEL( aChar2 )) )
            {
            return ETrue;
            }
        if ( IS_THAI_LEADING_VOWEL( aChar1 ) && IS_THAI_CONSONANT( aChar2 ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
