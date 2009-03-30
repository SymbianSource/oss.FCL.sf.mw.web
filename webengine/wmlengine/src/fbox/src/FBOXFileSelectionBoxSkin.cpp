/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementation for the FileSelectionBox skin, the skin being
*   the displayed element within the rendered XHTML page.
*
*/


// INCLUDE FILES
#include <E32STD.h>
#include <AknBidiTextUtils.h>
#include <AknUtils.h>
#include <webkit.rsg>

#include "BrCtl.h"
#include "BrsrStatusCodes.h"
#include "GDIDevicecontext.h"
#include "GDIFont.h"
#include "MVCShell.h"
#include "MVCView.h"
#include "nw_fbox_fileselectionbox.h"
#include "nw_fbox_fileselectionboxskin.h"
#include "nw_fbox_fileselectionboxskini.h"
#include "nw_fbox_formboxutils.h"
#include "nw_fbox_skini.h"
#include "nw_gdi_types.h"
#include "nw_image_epoc32cannedimage.h"
#include "nw_lmgr_imgcontainerboxi.h"
#include "nw_lmgr_rootbox.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// Arbitrary limit, specified in bytes, on amount of tolerated wasted space
// within file-name buffer. If the wasted space exceeds this amount, the buffer
// is reallocated. Must not be less than 2 (size of a 2-byte NULL terminator).
const TInt KFileNameWasteThreshold = 50;

// Amount of padding in between text and icon.
const TUint KImageHorizontalPadding = 2;


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
const
NW_FBox_FileSelectionBoxSkin_Class_t NW_FBox_FileSelectionBoxSkin_Class =
    {
        { /* NW_Object_Core           */
            /* super                    */ &NW_FBox_Skin_Class,
            /* queryInterface           */ _NW_Object_Base_QueryInterface
        },
        { /* NW_Object_Base           */
            /* interfaceList            */ NULL
        },
        { /* NW_Object_Dynamic        */
            /* instanceSize             */ sizeof ( NW_FBox_FileSelectionBoxSkin_t ),
            /* construct                */ _NW_FBox_FileSelectionBoxSkin_Construct,
            /* destruct                 */ _NW_FBox_FileSelectionBoxSkin_Destruct
        },
        { /* NW_FBox_Skin             */
            /* draw                     */ _NW_FBox_FileSelectionBoxSkin_Draw,
            /* getSize                  */ _NW_FBox_FileSelectionBoxSkin_GetSize
        },
        { /* NW_FBox_FileSelectionBoxSkin */
            /* getBaseline              */ _NW_FBox_FileSelectionBoxSkin_GetBaseline,
            /* reset                    */ _NW_FBox_FileSelectionBoxSkin_Reset,
            /* setActive                */ _NW_FBox_FileSelectionBoxSkin_SetActive,
            /* split                    */ _NW_FBox_FileSelectionBoxSkin_Split
        }
    };


// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::GetDirection
// Gets the layout direction, LTR or RTL.
// @param aLmgrBox Ptr to the parent LMgr OOC box class for this skin's box object.
// @param aDir Ptr to desitination into which direction is returned.
// -----------------------------------------------------------------------------
//
static
void
NW_FBox_FileSelectionBoxSkin_GetDirection(
    NW_LMgr_Box_t* aLmgrBox,
    NW_GDI_FlowDirection_t* aDir )
    {
    NW_LMgr_Property_t dirProp;

    // Parameter assertion block.
    NW_ASSERT( aLmgrBox );
    NW_ASSERT( aDir );

    dirProp.type = NW_CSS_ValueType_Token;
    dirProp.value.token = NW_CSS_PropValue_ltr;
    NW_LMgr_Box_GetProperty( aLmgrBox, NW_CSS_Prop_textDirection, &dirProp );
    *aDir = ( dirProp.value.token == NW_CSS_PropValue_ltr )
        ? NW_GDI_FlowDirection_LeftRight : NW_GDI_FlowDirection_RightLeft;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::GetDisplayText
// Extract the name and extension, no path, from the filename and truncate that
// to fit within the displayable area.
// @param aThisObj Ptr to the OOC class structure for this class.
// @param aDir Text direction.
// @param aImageSize Ptr to image size.
// @param aFont Font being used to display the text.
// @param aInnerRectangle Ptr to rectangle of the box available for text & icon.
// @param aDisplayText Ptr to destination into which the allocated text buffer
//      for the display text is returned, which if not NULL, the caller is
//      responsible for taking ownership and deallocating when appropriate.
// @return Browser status code:
//      KBrsrSuccess if no errors
//      KBrsrOutOfMemory if not enough memory
//      KBrsrFailure for all other failure conditions
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_FBox_FileSelectionBoxSkin_GetDisplayText(
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_GDI_FlowDirection_t aDir,
    NW_GDI_Dimension3D_t* aImageSize,
    const CFont& aFont,
    NW_GDI_Rectangle_t* aInnerRectangle,
    HBufC** aDisplayText )
    {

    // Parameter assertion block.
    NW_ASSERT( aThisObj );
    NW_ASSERT( aImageSize );
    NW_ASSERT( aInnerRectangle );
    NW_ASSERT( aDisplayText );

    *aDisplayText = NULL;

    // If no name yet, then nothing left to do.
    if ( aThisObj->iFileNameAndExt.Length() == 0 )
        {
        return KBrsrSuccess;
        }

    // Determine the maxiumum width available for the text.
    TInt maxWidthInPixels = aInnerRectangle->dimension.width - aImageSize->width
        - KImageHorizontalPadding;
    // Must be at least 2 pixels wide to display any text.
    if ( maxWidthInPixels < 2 )
        {
        return KBrsrSuccess;
        }

    // Get text direction.
    AknBidiTextUtils::TParagraphDirectionality directionality =
        (aDir == NW_GDI_FlowDirection_LeftRight)
            ? AknBidiTextUtils::ELeftToRight : AknBidiTextUtils::ERightToLeft;

    // Allocate buffer for the truncated text.
    HBufC* truncatedText = HBufC::New( aThisObj->iFileNameAndExt.Length()
        + KAknBidiExtraSpacePerLine + 1 ); // +1 for NULL terminator
    if ( !truncatedText )
        {
        return KBrsrOutOfMemory;
        }
    // The buffer is an HBufC, which is a 'constant' class. However, it is
    // on the heap and is modifiable if accessed through its pointer.
    TPtr ptrTruncatedText( truncatedText->Des() );

    // Truncate.
    AknBidiTextUtils::ConvertToVisualAndClip( aThisObj->iFileNameAndExt, 
        ptrTruncatedText, aFont, maxWidthInPixels, maxWidthInPixels, 
        directionality );

    // Make sure it is NULL terminated for legacy text handling.
    ptrTruncatedText.ZeroTerminate();

    // Return the pointer to the truncated text.
    *aDisplayText = truncatedText;

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::GetImageSize
// Gets the size of the icon used for indicating file-browse.
// @param aFileSelectionBox Ptr to OOC box to which this skin belongs.
// @param aImageSize Ptr to destination for returned image size.
// @return Browser status code, indicating status of operation.
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_FBox_FileSelectionBoxSkin_GetImageSize(
    NW_FBox_FileSelectionBox_t* aFileSelectionBox,
    NW_GDI_Dimension3D_t* aImageSize )
    {
    NW_LMgr_RootBox_t* rootBox;
    const NW_Image_AbstractImage_t* image;

    // Parameter assertion block.
    NW_ASSERT( aFileSelectionBox );
    NW_ASSERT( aImageSize );

    NW_TRY( status )
        {
        // Get root box.
        rootBox = NW_LMgr_Box_GetRootBox( NW_LMgr_BoxOf( aFileSelectionBox ) );
        NW_ASSERT( rootBox );

        // Get image from set of canned images.
        NW_ASSERT( rootBox->cannedImages );
        image = CONST_CAST( NW_Image_AbstractImage_t*,
            NW_Image_CannedImages_GetImage(
                rootBox->cannedImages, NW_Image_SelectFile ) );
        NW_THROW_ON_NULL( image, status, KBrsrFailure );

        // Get image size.
        status = NW_Image_AbstractImage_GetSize( image, aImageSize );
        _NW_THROW_ON_ERROR( status );
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::GetMaxCharExtent
// Gets the max extents (width & height) of a character for the current font.
// @param aFileSelectionBox Ptr to OOC box to which this skin belongs.
// @param aDeviceContext Ptr to GDI device context from which the current font
//      and its extents are obtained.
// @param aExtent Ptr to destination into which the extents are returned.
// @return Browser status code, indicating status of operation.
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_FBox_FileSelectionBoxSkin_GetMaxCharExtent(
    NW_FBox_FileSelectionBox_t* aFileSelectionBox,
    CGDIDeviceContext* aDeviceContext,
    NW_GDI_Dimension2D_t* aExtent )
    {
    CFont* font;
    CGDIFont* gdiFont;

    // Parameter assertion block.
    NW_ASSERT( aFileSelectionBox );
    NW_ASSERT( aDeviceContext );
    NW_ASSERT( aExtent );

    // Get the box font.
    gdiFont = NW_LMgr_Box_GetFont( NW_LMgr_BoxOf( aFileSelectionBox ) );
    NW_ASSERT( gdiFont );
    font = ( CFont* )aDeviceContext->GetScaledCFont( gdiFont );
    NW_ASSERT( font );

    // Get character extents.
    aExtent->width = (NW_GDI_Metric_t)font->MaxNormalCharWidthInPixels();
    aExtent->height = (NW_GDI_Metric_t)font->HeightInPixels();

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::DrawText
// Draws the text in the box, the text being the file name selected, if any.
// @param aThisObj Ptr to the OOC class structure for this class.
// @param aFileSelectionBox Ptr to OOC box to which this skin belongs.
// @param aDeviceContect Ptr to GDI device context to use for drawing.
// @param aImageSize Ptr to image size.
// @param aDir Direction of drawing, LTR or RTL.
// @return Browser status code, indicating status of operation.
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_FBox_FileSelectionBoxSkin_DrawText(
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_FBox_FileSelectionBox_t* aFileSelectionBox,
    CGDIDeviceContext* aDeviceContext,
    NW_GDI_Dimension3D_t* aImageSize,
    NW_GDI_FlowDirection_t aDir )
    {
    NW_GDI_Point2D_t start;
    const CFont* font;
    CGDIFont* gdiFont;
    NW_GDI_Rectangle_t innerRect;
    NW_LMgr_PropertyValue_t colorVal;
    NW_GDI_Color_t oldFgColor;
    NW_GDI_Dimension2D_t extent;
    HBufC* displayText = NULL;
    NW_Text_t* legacyText = NULL;

    // Parameter assertion block.
    NW_ASSERT( aThisObj );
    NW_ASSERT( aFileSelectionBox );
    NW_ASSERT( aDeviceContext );
    NW_ASSERT( aImageSize );

    // Get ALL items that will need to be restored in the "FINALLY" clause:
    oldFgColor = aDeviceContext->ForegroundColor();

    NW_TRY( status )
        {
        NW_LMgr_Box_t* lmgrBox = NW_LMgr_BoxOf( aFileSelectionBox );

        // Get box's font.
        gdiFont = NW_LMgr_Box_GetFont( lmgrBox );
        NW_ASSERT( gdiFont );
        font = (CFont*)aDeviceContext->GetScaledCFont( gdiFont );
        NW_ASSERT( font );

        // Get and set the box foreground color.
        colorVal.integer = 0;
        NW_LMgr_Box_GetPropertyValue( lmgrBox, NW_CSS_Prop_color,
            NW_CSS_ValueType_Color, &colorVal );
        aDeviceContext->SetForegroundColor( ( NW_GDI_Color_t ) colorVal.integer );

        // Get start coordinates.
        NW_LMgr_Box_GetInnerRectangle( lmgrBox, &innerRect );
        start = innerRect.point;

        // Text set every time in order to truncate according to current
        // settings. Note, GetDisplayText() may need to allocate a new
        // buffer for the truncated text.  If that happens, the returned
        // descriptor will point to the new buffer, which will need to be
        // deallocated below when it is no longer needed.
        status = NW_FBox_FileSelectionBoxSkin_GetDisplayText( aThisObj,
            aDir, aImageSize, *font, &innerRect, &displayText );
        _NW_THROW_ON_ERROR( status );

        if ( displayText )
            {
            if ( displayText->Length() )
                {
                // Convert from Symbian data structure to legacy data
                // structure. For efficiency, this was delayed as long as
                // possible but now it's needed.  Unfortunately, this will
                // allocate another buffer for the text.
                legacyText = (NW_Text_t*)NW_Text_UCS2_New(
                    (NW_Ucs2*)(displayText->Ptr()),
                    displayText->Length(),
                    NW_Text_Flags_NullTerminated );
                NW_THROW_OOM_ON_NULL( legacyText, status );

                // Get extents.
                status = aDeviceContext->GetTextExtent( legacyText, gdiFont, 0, aDir, &extent );
                _NW_THROW_ON_ERROR( status );

                // If RTL, change drawing coordinates.
                if ( aDir == NW_GDI_FlowDirection_RightLeft )
                    {
                    start.x = innerRect.point.x + innerRect.dimension.width - extent.width;
                    }
                status = aDeviceContext->DrawText( &start, legacyText, gdiFont, 0, 
                    NW_GDI_TextDecoration_None, aDir, extent.width );
                _NW_THROW_ON_ERROR( status );
                }
            }
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        // Free any allocated resources.
        delete displayText;
        NW_Object_Delete( legacyText );

        // Restore fg color.
        aDeviceContext->SetForegroundColor( oldFgColor );
        return status;
        }
    NW_END_TRY
    }


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::NW_FBox_FileSelectionBoxSkin
// OOC constructor.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Construct(
    NW_Object_Dynamic_t* aDynamicObject,
    va_list* aArgp )
    {
    TBrowserStatusCode status;
    NW_FBox_FileSelectionBoxSkin_t* thisObj;

    // For convenience.
    thisObj = NW_FBox_FileSelectionBoxSkinOf( aDynamicObject );

    // Invoke superclass constructor.
    status = _NW_FBox_Skin_Construct( aDynamicObject, aArgp );
    if ( status != KBrsrSuccess )
        {
        return status;
        }

    // Init member variables.
    thisObj->iFullFileName = NULL;
    thisObj->iFileNameAndExt.Set( NULL, 0 );

    return KBrsrSuccess;
    }

// Destructor
void
_NW_FBox_FileSelectionBoxSkin_Destruct( NW_Object_Dynamic_t* aDynamicObject )
    {
    NW_FBox_FileSelectionBoxSkin_t* thisObj;

    // For convenience.
    thisObj = NW_FBox_FileSelectionBoxSkinOf( aDynamicObject );

    delete thisObj->iFullFileName;
    thisObj->iFullFileName = NULL;
    thisObj->iFileNameAndExt.Set( NULL, 0 );
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::Draw
// Draws the box, including border, icon, and text of file name.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Draw(
    NW_FBox_Skin_t* aSkin,
    NW_LMgr_Box_t* aLmgrBox,
    CGDIDeviceContext* aDeviceContext,
    NW_Uint8 aHasFocus )
    {
    NW_FBox_FileSelectionBox_t* fileSelectionBox;
    NW_FBox_FileSelectionBoxSkin_t* thisObj;
    NW_GDI_Rectangle_t innerRectangle;
    NW_GDI_Point2D_t imagePoint;
    NW_GDI_Dimension3D_t imageSize;
    NW_LMgr_RootBox_t* rootBox;
    NW_Image_AbstractImage_t* image;
    NW_GDI_FlowDirection_t dir;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aSkin, &NW_FBox_FileSelectionBoxSkin_Class ) );
    NW_ASSERT( NW_Object_IsInstanceOf( aLmgrBox, &NW_FBox_FileSelectionBox_Class ) );
    NW_ASSERT( aDeviceContext );

    // For convenience.
    fileSelectionBox = NW_FBox_FileSelectionBoxOf( aLmgrBox );
    NW_ASSERT( fileSelectionBox );
    thisObj = NW_FBox_FileSelectionBoxSkinOf( aSkin );
    NW_ASSERT( thisObj );

    NW_TRY( status )
        {
        // Standard paint mode.
        aDeviceContext->SetPaintMode( NW_GDI_PaintMode_Copy );

        // Let base class draw the box -- borders, padding, background color, etc.
        status = _NW_LMgr_Box_Draw( aLmgrBox, aDeviceContext, aHasFocus );
        _NW_THROW_ON_ERROR( status );

        // Get root box.
        rootBox = NW_LMgr_Box_GetRootBox( NW_LMgr_BoxOf( fileSelectionBox ) );
        NW_ASSERT( rootBox );

        // Get image from set of canned images.
        NW_ASSERT( rootBox->cannedImages );
        image = CONST_CAST( NW_Image_AbstractImage_t*,
            NW_Image_CannedImages_GetImage(
                rootBox->cannedImages, NW_Image_SelectFile ) );
        NW_THROW_ON_NULL( image, status, KBrsrFailure );

        // Get image size.
        status = NW_Image_AbstractImage_GetSize( image, &imageSize );
        _NW_THROW_ON_ERROR( status );

        // Get current direction.
        NW_FBox_FileSelectionBoxSkin_GetDirection( aLmgrBox, &dir );

        // Draw text first.
        status = NW_FBox_FileSelectionBoxSkin_DrawText( thisObj, fileSelectionBox,
            aDeviceContext, &imageSize, dir );
        _NW_THROW_ON_ERROR( status );

        // ==========================================
        // Position image within the inner rectangle.
        // ==========================================

        // Get inner rectangle.
        status = NW_LMgr_Box_GetInnerRectangle( aLmgrBox, &innerRectangle );
        _NW_THROW_ON_ERROR( status );

        // In x dimension, position image to either side; depends on direction.
        if ( dir == NW_GDI_FlowDirection_RightLeft )
            {
            imagePoint.x = innerRectangle.point.x;
            }
        else
            {
            imagePoint.x = ( NW_GDI_Metric_t ) ( innerRectangle.point.x
                + innerRectangle.dimension.width - imageSize.width );
            }
        // In y dimension, center image.
        if ( innerRectangle.dimension.height > imageSize.height )
            {
            imagePoint.y = ( NW_GDI_Metric_t ) ( innerRectangle.point.y
                + ((innerRectangle.dimension.height - imageSize.height) / 2) );
            }
        else
            {
            imagePoint.y = ( NW_GDI_Metric_t ) innerRectangle.point.y;
            }

        // Draw image.
        status = NW_Image_AbstractImage_Draw( image, aDeviceContext, &imagePoint );
        _NW_THROW_ON_ERROR( status );

        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::GetBaseline
// Gets the baseline for the content of this box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_GDI_Metric_t
_NW_FBox_FileSelectionBoxSkin_GetBaseline(NW_FBox_FileSelectionBoxSkin_t* aThisObj,
                                          NW_LMgr_Box_t* aLmgrBox )
  {
  TBrowserStatusCode status;
  CGDIFont* font;
  NW_GDI_Metric_t baseline;

  NW_REQUIRED_PARAM( aThisObj );

  // Parameter assertion block.
  NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBoxSkin_Class ) );
  NW_ASSERT( NW_Object_IsInstanceOf( aLmgrBox, &NW_FBox_FileSelectionBox_Class ) );

  // Set font to match parent.
  font = NW_LMgr_Box_GetFont( aLmgrBox );
  if ( font )
    {
    TGDIFontInfo fontInfo;
    NW_LMgr_FrameInfo_t padding;
    NW_LMgr_FrameInfo_t borderWidth;
    NW_GDI_Dimension3D_t imageSize;

    status = font->GetFontInfo( &fontInfo );
    NW_ASSERT( status == KBrsrSuccess );

    // Get padding and border width.
        NW_LMgr_Box_GetPadding( aLmgrBox, &padding, ELMgrFrameTop );
        NW_LMgr_Box_GetBorderWidth( aLmgrBox, &borderWidth, ELMgrFrameTop );

    // Get size of file-selection icon.
    status = NW_FBox_FileSelectionBoxSkin_GetImageSize(
        NW_FBox_FileSelectionBoxOf( aLmgrBox ), &imageSize );
    NW_ASSERT( status == KBrsrSuccess );
    if ( status == KBrsrSuccess )
      {
      // Use bigger of font baseline or image height and then adjust
      // for border and padding.
      baseline = (NW_GDI_Metric_t)((fontInfo.baseline > imageSize.height)
          ? fontInfo.baseline : imageSize.height) + borderWidth.top + padding.top;
      }
    else
      {
      // Unable to get icon size so font baseline is used, adjusted
      // for border and padding.
      baseline = fontInfo.baseline + borderWidth.top + padding.top;
      }
    }
  else
    {
    // Unable to get FontInfo so box height is used.
    baseline = NW_LMgr_Box_GetFormatBounds( aLmgrBox ).dimension.height;
    }

  return baseline;
  }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::GetSize
// Gets the size (width & height) needed for the content.  The returned width is
// bounded by the display width, adjusted for padding.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_GetSize(
    NW_FBox_Skin_t* aSkin,
    NW_LMgr_Box_t* aLmgrBox,
    NW_GDI_Dimension3D_t* aSize )
    {
    NW_FBox_FileSelectionBox_t* fileSelectionBox;
    NW_Uint16 desiredTextAreaWidthInChars;
    NW_GDI_Metric_t maxWidth;
    NW_GDI_Metric_t desiredTextAreaWidthInPixels;
    NW_GDI_Dimension2D_t charExtent;
    NW_GDI_Dimension3D_t imageSize;
    CGDIDeviceContext* deviceContext;
    const NW_GDI_Rectangle_t* rectangle;
    NW_LMgr_FrameInfo_t padding;
    NW_LMgr_FrameInfo_t borderWidth;

    NW_REQUIRED_PARAM( aSkin );

    // Parameter assertion block.
    NW_ASSERT( aSkin );
    NW_ASSERT( aSize );
    NW_ASSERT( NW_Object_IsInstanceOf( aLmgrBox, &NW_FBox_FileSelectionBox_Class ) );

    NW_TRY( status )
        {
        // For convenience.
        fileSelectionBox = NW_FBox_FileSelectionBoxOf( aLmgrBox );

        // Set to zero as default in case error occurs.
        aSize->width = 0;
        aSize->height = 0;

        // Get device context.
        deviceContext = NW_LMgr_RootBox_GetDeviceContext( NW_LMgr_Box_GetRootBox( aLmgrBox ) );
        NW_ASSERT( deviceContext );

        // Get display bounds.
        rectangle = deviceContext->DisplayBounds();

        // Get max char extents.
        status = NW_FBox_FileSelectionBoxSkin_GetMaxCharExtent(
            fileSelectionBox, deviceContext, &charExtent );
        _NW_THROW_ON_ERROR( status );

        // Get size of file-selection icon.
        status = NW_FBox_FileSelectionBoxSkin_GetImageSize(
            fileSelectionBox, &imageSize );
        _NW_THROW_ON_ERROR( status );

        // Get padding and border width.
        NW_LMgr_Box_GetPadding( aLmgrBox, &padding, ELMgrFrameLeft|ELMgrFrameRight );
        NW_LMgr_Box_GetBorderWidth( aLmgrBox, &borderWidth, ELMgrFrameLeft|ELMgrFrameRight );

        aSize->height = ( NW_GDI_Metric_t ) ( charExtent.height );
        // Height must not be smaller than the icon height.
        if ( aSize->height < imageSize.height )
            {
            aSize->height = imageSize.height;
            }

        // Max width is bounded by display width, adjusted for padding and borders.
        maxWidth = rectangle->dimension.width - padding.left - padding.right
            - borderWidth.left - borderWidth.right;

        // Determine the width based on desired width. A desired width of 0
        // indicates to use width of the display, adjusted for borders, etc.
        desiredTextAreaWidthInChars =
            NW_FBox_FileSelectionBox_GetDesiredTextAreaWidthInChars( fileSelectionBox );
        
        CGDIFont* font = NW_LMgr_Box_GetFont(aLmgrBox);

        desiredTextAreaWidthInPixels = (NW_GDI_Metric_t)desiredTextAreaWidthInChars
            * font->GetAverageCharWidthInPixels();

        if ( desiredTextAreaWidthInPixels )
            {
            aSize->width = desiredTextAreaWidthInPixels + KImageHorizontalPadding
                + imageSize.width;
            if ( aSize->width > maxWidth )
                {
                aSize->width = maxWidth;
                }
            }
        else
            {
            aSize->width = maxWidth;
            }
        }
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::Reset
// Resets this box; i.e., resets the value of the filename to an empty string
// but does not redraw the field.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Reset( NW_FBox_FileSelectionBoxSkin_t* aThisObj )
    {
    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBoxSkin_Class ) );

    // Clear the field.
    if ( aThisObj->iFullFileName )
        {
        TPtr ptrBuf( aThisObj->iFullFileName->Des() );
        ptrBuf.Zero();
        // For legacy text handling, always keep a null on end of string.
        ptrBuf.ZeroTerminate();
        }
    aThisObj->iFileNameAndExt.Set( NULL, 0 );

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::SetActive
// Sets this box to be the active box, which launches the file-selection dialogs.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_SetActive( NW_FBox_FileSelectionBoxSkin_t* aThisObj )
    {
    TInt err = KErrNone;
    TBool selected = EFalse;
    TBool useNewBuf;
    HBufC* buf = NULL;
    TPtrC ptrPath( NULL, 0 );
    TPtrC ptrRoot( NULL, 0 );
    TPtrC ptrMostRecentPath( NULL, 0 );

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBoxSkin_Class ) );

    NW_FBox_FileSelectionBox_t* fileSelectionBox = NW_FBox_FileSelectionBoxSkin_GetFileSelectionBox( aThisObj );
    NW_FBox_FormLiaison_t* formLiaison =  NW_FBox_FormBox_GetFormLiaison(fileSelectionBox);

    NW_TRY( status )
        {
        // If there is already a file selected for this field then use that as
        // starting point.
        if ( aThisObj->iFullFileName && aThisObj->iFileNameAndExt.Length() )
            {
            ptrMostRecentPath.Set( aThisObj->iFullFileName->Des() );
            }
        // Else check for existing form-liaison path.
        else
            {
            NW_FBox_FileSelectionBox_t* fileSelectionBox = 
                NW_FBox_FileSelectionBoxSkin_GetFileSelectionBox( aThisObj );
            NW_FBox_FormLiaison_t* formLiaison =
                NW_FBox_FormBox_GetFormLiaison( fileSelectionBox );
            status = NW_FBox_FormLiaison_GetMostRecentFileSelectionPath(
                formLiaison, ptrMostRecentPath );
            _NW_THROW_ON_ERROR( status );
            }

        // If a recent path exists, use that as the starting point.
        if ( ptrMostRecentPath.Length() )
            {
            TParsePtrC fileNameParser( ptrMostRecentPath );
            ptrPath.Set( fileNameParser.DriveAndPath() );
            }

        // Get dialog provider.
        NW_LMgr_Box_t* lmgrBox = NW_LMgr_BoxOf( fileSelectionBox );
        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( lmgrBox );
        MBoxTreeListener* listener = NW_LMgr_RootBox_GetBoxTreeListener( rootBox );
        NW_ASSERT(listener);
        MBrCtlDialogsProvider* dialogProvider = ((CView*)listener)->BrCtl()->brCtlDialogsProvider();

        // Launch the file-selection dialog.
        TRAP( err,
            selected = dialogProvider->DialogFileSelectLC(
                ptrPath, ptrRoot, buf );
            if ( buf )
                CleanupStack::Pop();    // buf
            else
                CleanupStack::PopAndDestroy();
            );

        if ( err )
            {
            // Map the system error to browser error: out-of-memory or failure.
            NW_THROW_STATUS( status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure ) );
            }

        // Delegate the ECMA onBlur event when the file selection dialog is closed.
        // Note, error is ignored. But why?!!!
        (void)NW_FBox_FormLiaison_DelegateEcmaEvent( formLiaison, 
            NW_FBox_FormBox_GetFormCntrlID(fileSelectionBox),
            NW_Ecma_Evt_OnBlur );
//        _NW_THROW_ON_ERROR( status );

        // If no file selected, nothing else to do.  The value is unchanged.
        if ( selected == EFalse )
            {
            NW_THROW_STATUS( status, KBrsrSuccess );
            }

        // The buffer is an HBufC, which is a 'constant' class. However, it is
        // on the heap and is modifiable if accessed through its pointer.
        TPtr ptrNewBuf( buf->Des() );

        // If the user selected the same file as the last time (i.e., the value
        // hasn't changed) then nothing else to do.
        if ( aThisObj->iFullFileName )
            {
            if ( ptrNewBuf.Compare( aThisObj->iFullFileName->Des() ) == 0 )
                {
                NW_THROW_STATUS( status, KBrsrSuccess );
                }
            }

        // =============================
        // Conserve on memory usage.
        // =============================

        // Clear file name and extension for now.
        aThisObj->iFileNameAndExt.Set( NULL, 0 );

        // Re-use old file-name buffer if it is big enough.
        useNewBuf = ETrue;  // assume using a new buffer
        if ( aThisObj->iFullFileName )
            {
            TPtr ptrOldBuf( aThisObj->iFullFileName->Des() );
            if ( ptrOldBuf.MaxSize() >= ptrNewBuf.Size() )
                {
                useNewBuf = EFalse;
                // Copy newly selected file-name into old buffer.
                ptrOldBuf.Copy( ptrNewBuf );
                }
            }

        // If using new buffer, delete old one and save reference to new one.
        if ( useNewBuf )
            {
            delete aThisObj->iFullFileName;
            aThisObj->iFullFileName = buf;
            buf = NULL;
            }

        // Re-allocate buffer that is in use if it's wasting too much space.
        TPtr tmp( aThisObj->iFullFileName->Des() );
        if ( (tmp.MaxSize() - tmp.Size()) > KFileNameWasteThreshold )
            {
            // +2 for 2-byte NULL terminator
            aThisObj->iFullFileName = aThisObj->iFullFileName->ReAlloc( tmp.Size() + 2 );
            NW_THROW_OOM_ON_NULL( aThisObj->iFullFileName, status );
            }
        TPtr tmpNew( aThisObj->iFullFileName->Des() );
        // Always zero terminate in order to allow for legacy text handling.
        tmpNew.ZeroTerminate();

        // Extract the name and extension from the fully-qualified file name.
        TPtrC ptr( aThisObj->iFullFileName->Des() );
        TParsePtrC fileNameParser( ptr );
        aThisObj->iFileNameAndExt.Set( fileNameParser.NameAndExt() );
	
        // =============================
        // Re-draw.
        // =============================
        CGDIDeviceContext* deviceContext = 
            NW_LMgr_RootBox_GetDeviceContext( rootBox );

        // Have the skin class draw the box, passing TRUE to indicate focus.
        status = _NW_FBox_FileSelectionBoxSkin_Draw(
            NW_FBox_SkinOf( aThisObj ), lmgrBox, deviceContext, NW_TRUE );
        _NW_THROW_ON_ERROR( status );

        // Make the draw happen now.
        deviceContext->PostDraw( ETrue );

        // Delegate the ECMA onChange event when a different file is selected.
        status = NW_FBox_FormLiaison_DelegateEcmaEvent( formLiaison, 
            NW_FBox_FormBox_GetFormCntrlID(fileSelectionBox), 
            NW_Ecma_Evt_OnChange );
        _NW_THROW_ON_ERROR( status );
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        delete buf;
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::Split
// Checks to see if this box needs to be split and if so, "splits" it; however, 
// this box cannot be split. It is moved to a new line if not already on one.
// That is the best that can be done to split this box.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_FBox_FileSelectionBoxSkin_Split(
    NW_FBox_FileSelectionBoxSkin_t* aThisObj,
    NW_GDI_Metric_t aSpace,
    NW_LMgr_Box_t** aSplitBox,
    NW_Uint8 aFlags )
    {
    NW_LMgr_Box_t* lmgrBox;
    NW_FBox_FormBox_t* formBox;

    // Parameter assertion block.
    NW_ASSERT( NW_Object_IsInstanceOf( aThisObj, &NW_FBox_FileSelectionBoxSkin_Class ) );
    NW_ASSERT( aSplitBox );

    formBox = NW_FBox_Skin_GetFormBox( NW_FBox_SkinOf( aThisObj ) );
    NW_ASSERT( formBox );
    lmgrBox = NW_LMgr_BoxOf( formBox );
    NW_ASSERT( NW_Object_IsInstanceOf( lmgrBox, &NW_FBox_FileSelectionBox_Class ) );

    *aSplitBox = NULL;

    // If box does not fit in the space and the box is not on the new line then
    // the input box needs to be pushed on the new line.
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( lmgrBox );
    if ( ( boxBounds.dimension.width > aSpace )
         && !( aFlags & NW_LMgr_Box_SplitFlags_AtNewLine ) )
        {
        return KBrsrLmgrNoSplit;
        }

    if ( boxBounds.dimension.width <= aSpace )
        {
        return KBrsrSuccess;
        }

    boxBounds.dimension.width = aSpace;
    NW_LMgr_Box_SetFormatBounds( lmgrBox, boxBounds );

    return KBrsrSuccess;
    }


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBoxSkin::New
// Creates a new instance of this box skin class.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_FBox_FileSelectionBoxSkin_t*
NW_FBox_FileSelectionBoxSkin_New( NW_FBox_FormBox_t* aFormBox )
    {
    NW_ASSERT( aFormBox );
    return ( NW_FBox_FileSelectionBoxSkin_t* )
           NW_Object_New( &NW_FBox_FileSelectionBoxSkin_Class, aFormBox );
    }
