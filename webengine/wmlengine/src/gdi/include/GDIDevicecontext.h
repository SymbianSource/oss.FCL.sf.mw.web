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
* Description:  gdi interface
*
*/


#ifndef _GDIDEVICECONTEXT_H
#define _GDIDEVICECONTEXT_H

// INCLUDES
#include <AknUtils.h>
#include "nw_object_dynamic.h"
#include "GDIFont.h"
#include "BrsrTypes.h"
#include "BrsrStatusCodes.h"

// CONSTANTS
enum TGDIDeviceContextFontSizeLevel
  {
  EGDIDeviceContextFontSizeLevelAllSmall,
  EGDIDeviceContextFontSizeLevelSmaller,
  EGDIDeviceContextFontSizeLevelNormal,
  EGDIDeviceContextFontSizeLevelLarger,    
  EGDIDeviceContextFontSizeLevelAllLarge
  };

// fonts to be created : italic12, bold-italic12, plain13, italic13,
// bold-italic13, plain17, italic17, bold-italic17
const TInt8 KNumberOfFonts = 8;

// Browser specific font types
//
enum TGDIDeviceContextFontNames
  {
  ELatinItalic12,
  ELatinBoldItalic12,
  ELatinPlain13,
  ELatinItalic13,
  ELatinBoldItalic13,
  ELatinPlain17,
  ELatinItalic17,
  ELatinBoldItalic17
  };

enum TGDISupportedLanguage
    {
    EElafLanguage,
    EThaiLanguage,
    EJapaneseLanguage,
    EChinesePRCLanguage,
    EChinaHKTWLanguage
    };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CGDIDeviceContext
*  
*  @lib BrowserEngine.dll
*  @since 2.1
*/
class CGDIDeviceContext : public CBase
{
  public:  // Constructors and destructor
        
   /**
    * Two-phased constructor.
    * @return CGDIDeviceContext*
	  */
    static CGDIDeviceContext* NewL( void* aGraphicsContext, CView* aOocEpoc32View );

    /**
    * Destructor.
    */
    virtual ~CGDIDeviceContext();

  public: // New functions
        
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void DrawImage( const NW_GDI_Point2D_t* aLocation, 
                    const NW_GDI_Rectangle_t* aRectangle,
                    NW_GDI_ImageBlock_t* aImageBlock );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode ScaleImage( const NW_GDI_Dimension3D_t* aSize,
                                   const NW_GDI_ImageBlock_t* aImageBlock,
                                   NW_GDI_ImageBlock_t* aScaledImageBlock );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void DrawPixel( const NW_GDI_Point2D_t* aLocation );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode DrawPolyline( NW_GDI_Metric_t aNumPoints,
                                     const NW_GDI_Point2D_t* aPoints,
                                     TBool aClosed );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void DrawRectangle( const NW_GDI_Rectangle_t* aRectangle );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */

    TBrowserStatusCode DrawText( const NW_GDI_Point2D_t* aLocation,
                                 const NW_Text_t* aText,
                                 CGDIFont* aGDIFont,
                                 NW_GDI_Metric_t aSpacing,
                                 NW_GDI_TextDecoration_t aDecoration,
                                 NW_GDI_FlowDirection_t aDir,
                                 NW_GDI_Metric_t aWidth );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void FillRectangle( const NW_GDI_Rectangle_t* aRectangle );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode GetColorPalette( NW_GDI_ColorPalette_t* aColPalette );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Color_t BackgroundColor () const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Rectangle_t ClipRect () const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    const NW_GDI_Rectangle_t* DisplayBounds () const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TUint8 DisplayDepth() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Pattern_t FillPattern() const;

    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode GetFlowDirection( NW_Text_t* aText, 
                                         NW_GDI_FlowDirection_t* aFlowDirection) const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Color_t ForegroundColor() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Pattern_t LinePattern() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Metric_t LineWidth() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    const NW_GDI_Point2D_t* Origin() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_PaintMode_t PaintMode() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void* GetScaledCFont( CGDIFont* aFont ) const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_TextDirection_t TextDirection() const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode GetTextExtent( const NW_Text_t* aText,
                                      CGDIFont* aGDIFont,
                                      NW_GDI_Metric_t aSpacing,
                                      NW_GDI_FlowDirection_t aFlowDirection,
                                      NW_GDI_Dimension2D_t* aExtent) const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void PreDraw();
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void PostDraw( TBool aDrawNow );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetBackgroundColor( NW_GDI_Color_t aColor );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetClipRect( const NW_GDI_Rectangle_t* aClipRect );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetFillPattern( NW_GDI_Pattern_t aFillPattern );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetForegroundColor( NW_GDI_Color_t aColor );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetLinePattern( NW_GDI_Pattern_t aLinePattern );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetLineWidth( NW_GDI_Metric_t aLineWidth );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetOrigin( const NW_GDI_Point2D_t* aOrigin );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetPaintMode( NW_GDI_PaintMode_t aPaintMode );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetTextDirection( NW_GDI_TextDirection_t aTextDirection );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetDisplayBounds( const NW_GDI_Rectangle_t* aDisplaybounds );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    CGDIFont* CreateFont( const NW_Text_t* aFamily, 
                          TGDIFontStyle aStyle, 
                          TUint8 aSizeInPixels,
                          TUint16 aWeight, 
                          TGDIFontVariant aVariant) const;
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void DrawLine( NW_GDI_Point2D_t aStartPoint, NW_GDI_Point2D_t aEndPoint );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBool SetFontSizeLevel();
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void ChangeFontSize( CGDIFont* aFont );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void GetHighlightColor( NW_GDI_Color_t* aColor );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SwitchAgainstBG( NW_GDI_Color_t* aColor );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode SplitText( NW_Text_t* aText,
                                  CGDIFont* aGDIFont,
                                  NW_GDI_Metric_t aConstraint,
                                  NW_GDI_Metric_t aSpacing,
                                  NW_GDI_FlowDirection_t aFlowDirection,
                                  NW_Text_Length_t* aLength,
                                  TUint8 aMode );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode GetSubstringExtent( const NW_Text_t* aText,
                                           CGDIFont* aGDIFont,
                                           NW_Text_Length_t aStart,
                                           NW_Text_Length_t aLength,
                                           NW_GDI_Metric_t aSpacing,
                                           NW_GDI_FlowDirection_t aFlowDirection,
                                           NW_GDI_Dimension2D_t* aExtent );

    /**
    * This method 
    * @since 2.1
    * @param TFontSpec which specifies the font family and the height of the font 
    *   the caller of this method wishes to find.
    * @return pointer to a CFont object which will have been found in the iFontCache 
    *   or have been created if the font was not already in the cache
    */
    CFont* FindFont( TFontSpec aFontSpec );

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetRemoveNbsp( TBool aFlag );

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBool RemoveNbsp();

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetGraphicsContext( void* aGraphicsContext );

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void SetFontSizeLevel( TGDIDeviceContextFontSizeLevel aFontSizeLevel );

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    EVariantFlag VariantFlag();

    /**
    * This method 
    * @since 2.x
    * @param 
    * @return 
    */
    inline CWsScreenDevice*  WsScreenDevice() { return iWsScreenDev; };

    /**
    * This method 
    * @since 2.x
    * @param 
    * @return 
    */
    TGDISupportedLanguage SupportedLanguage();

  private: // New functions

    /**
    * C++ default constructor.
    */
    CGDIDeviceContext();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    TBrowserStatusCode ConstructL( void* aGraphicsContext, CView* aOocEpoc32View );

    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    NW_GDI_Color_t SwapColorBytes( NW_GDI_Color_t aColor ); 
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    CBitmapContext* Epoc32GraphicsContext();
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode WordWrap ( NW_Text_t* aText,
                                  CGDIFont* aGDIFont,
                                  NW_GDI_Metric_t aConstraint,
                                  NW_GDI_Metric_t aSpacing,
                                  NW_GDI_FlowDirection_t aFlowDirection,
                                  NW_Text_Length_t* aLength );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode SplitAtNewline ( NW_Text_t* aText,
                                        CGDIFont* aGDIFont,
                                        NW_GDI_Metric_t aCconstraint,
                                        NW_GDI_Metric_t aSpacing,
                                        NW_GDI_FlowDirection_t aFlowDirection,
                                        NW_Text_Length_t* aLength );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode Truncate ( NW_Text_t* aText,
                                  CGDIFont* aGDIFont,
                                  NW_GDI_Metric_t aConstraint,
                                  NW_GDI_Metric_t aSpacing,
                                  NW_GDI_FlowDirection_t aFlowDirection,
                                  NW_Text_Length_t* aLength );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    TBrowserStatusCode ScaleBitmap( CFbsBitmap* aBitmap, 
                  CFbsBitmap** aScaledBitmap, 
                  TRect* aRect );
    
    /**
    * This method 
    * @since 2.1
    * @param 
    * @return 
    */
    void InitializeGraphicsContext();

    /**
    * Checks whether or not a line split (word wrap) can occur between the two
    * characters specified.
    * @since 2.6
    * @param aChar1 First character.
    * @param aChar2 Second character.
    * @return ETrue if split can occur between the two characters; else, EFalse.
    */
    TBool CanSplit(TText16 aChar1, TText16 aChar2);

    /**
    * Checks whether or not the two consecutive characters are within a single
    * Thai script "cell".
    * @since 2.6
    * @param aChar1 First character.
    * @param aChar2 Second character.
    * @return ETrue if is in a single Thai script "cell"; else, EFalse.
    */
    TBool
    IsThaiCell( TText16 aChar1, TText16 aChar2 );


  private:    // Data

    //
    EVariantFlag                          iVariantFlag;
    //
    CFontCache*                           iFontCache;
    //
    CWsScreenDevice*                      iWsScreenDev;
    // used for removing nbsp in vertical-layout mode
    TBool                                 iRemoveNbsp; 
    // is a CBitmapContext*
    void*                                 iGraphicsContext; 
    //
    NW_GDI_Point2D_t                      iOrigin;
    //
    TGDIDeviceContextFontSizeLevel        iFontSizeLevel;
    //
    NW_GDI_Pattern_t                      iFillPattern;
    //
    NW_GDI_Pattern_t                      iLinePattern;
    //
    NW_GDI_Metric_t                       iLineWidth;
    //
    NW_GDI_PaintMode_t                    iPaintMode;
    //
    NW_GDI_TextDirection_t                iTextDirection;
    //
    NW_GDI_Color_t                        iForegroundColor;
    //
    NW_GDI_Color_t                        iBackgroundColor;
    //
    NW_GDI_Rectangle_t                    iDisplayBounds;
    //
    NW_GDI_Rectangle_t                    iClipRect;
    //
    CView*                                iOocEpoc32View; 
    //
    NW_Int32                              iResourceFileOffset;
    //
    TGDISupportedLanguage                 iSupportedLanguage;
};

#endif // _GDIDEVICECONTEXT_H
