/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  font interface
*
*/


#ifndef _GDIFONT_H
#define _GDIFONT_H

// INCLUDES
#include "nw_object_dynamic.h"
#include "nw_gdi_types.h"
#include "NW_GDI_EXPORT.h"
#include "BrsrTypes.h"
#include "BrsrStatusCodes.h"
#include "e32base.h"

// DATA TYPES
struct TGDIFontInfo 
  {
  TUint8 height;
  TUint8 baseline;
  };

enum TGDIFontStyle
  {
  EGDIFontStyleNormal = 1,
  EGDIFontStyleItalic,
  EGDIFontStyleOblique
  };

enum TGDIFontVariant
  {
  EGDIFontVariantNormal = 1,
  EGDIFontVariantSmallCaps
  };

// CONSTANTS
const NW_Text_t* const KGDIFontDefaultFamily = NULL;
const TUint8 KGDIFontDefaultStyle  = EGDIFontStyleNormal;
const NW_Float32 KGDIFontDefaultSizeInPt = 6.0F;  // 6pt
const TUint8 KGDIFontZeroSize = 0;
const TUint16 KGDIFontDefaultWeight = 400;
const TUint8 KGDIFontDefaultVariant = EGDIFontVariantNormal;
const TUint16 KGDIDefaultFontSizeInTwips = 120;

// MACROS

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

typedef struct 
    {
    const char* iSpecifiedFontName;
    const char* iDefaultFontName;
    }TFontLookupArray;

/**
*  CGDIDeviceContext
*  
*  @lib BrowserEngine.dll
*  @since 2.1
*/
class CGDIFont : public CBase
  {
  public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    * @return CCSSHandler*
    */
    static CGDIFont* NewL( const CGDIDeviceContext* aDeviceContext,
                           const NW_Text_t* aFamily, 
                           TGDIFontStyle aStyle, 
                           TUint16 aSize,
                           TUint16 aWeight, 
                           TGDIFontVariant aVariant );
    
    /**
    * Destructor.
    */
    virtual ~CGDIFont();
    
  public: // New functions
    
    /**
    * This method 
    * @since 2.1
    * @param - this parameter is in twips
    * @return 
    */
    void SetFontSize( TUint16 aFontSize );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return
    */
    inline TDesC* FontFamily() { return iFontFamily; };

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return TUint16 - the font size in twips 
    */
    inline TUint16 FontSize() { return iFontSize; };

    /**
    * This method 
    * @since 2.x
    * @param 
    * @return TUint16 - the declared or default font size in twips
    */
    inline TUint16 DeclaredFontSize() { return iDeclaredFontSizeInTwips; };

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    inline TGDIFontVariant FontVariant() { return iFontVariant; };
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return TUint16  
    */
    inline TUint16 FontWeight() { return iFontWeight; };

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    inline TGDIFontStyle FontStyle() { return iFontStyle; };

    /**
    * This method 
    * @since 2.x
    * @param TGDIFontInfo* whose height and baseline data will be populated
    * @return TBrowserStatusCode
    */
    TBrowserStatusCode GetFontInfo( TGDIFontInfo* aFontInfo );

    /**
    * This method 
    * @since 2.x
    * @param 
    * @return TUint8 width of the average character in pixels (based off of the 'w' character)
    */
    TUint8 GetAverageCharWidthInPixels();

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    const void* GetEpocNativeFont();

  private: // New functions
    
    /**
    * C++ default constructor.
    */
    CGDIFont();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( const CGDIDeviceContext* aDeviceContext,
                     const NW_Text_t* aFamily, 
                     TGDIFontStyle aStyle, 
                     TUint16 aSize,
                     TUint16 aWeight, 
                     TGDIFontVariant aVariant );
    
  private:    // Data
    //
    HBufC*                      iFontFamily;
    // The font size value in twips 
    TUint16				        iFontSize;
    //
    TUint16                     iDeclaredFontSizeInTwips;
    //
    TGDIFontVariant     	    iFontVariant;
    //
    TGDIFontStyle     	        iFontStyle;
    //
    TUint16				        iFontWeight;
    //
    CGDIDeviceContext*          iDeviceContext;
    //
    TBool                       iFontFound;
  };


#endif // _GDIFONT_H
