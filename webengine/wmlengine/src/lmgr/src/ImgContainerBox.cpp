/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Box class associated with the handling image maps
*
*/


// INCLUDE FILES
#include "nw_lmgr_imgcontainerboxi.h"
#include "nw_image_cannedimages.h"
#include "nw_image_epoc32cannedimage.h"
#include "nw_image_virtualimage.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_imagemapbox.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nwx_settings.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

// CONSTANTS
const NW_Ucs2 NW_LMgr_BoxUtils_Truncate[] = {'.','.','.','\0'};
const NW_Text_Length_t NW_LMgr_BoxUtils_Truncate_Len = 3;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NW_FBox_ImgContainerBox_GetDirection
// Get the dir attribute 
// Returns: void
// -----------------------------------------------------------------------------
//
static
void
NW_FBox_ImgContainerBox_GetDirection(NW_LMgr_Box_t *box,
                                     NW_GDI_FlowDirection_t* dir)
{
  NW_LMgr_Property_t dirProp;
  NW_ASSERT(box != NULL);

  dirProp.type = NW_CSS_ValueType_Token;
  dirProp.value.token = NW_CSS_PropValue_ltr;
  NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_textDirection, &dirProp);
  if (dirProp.value.token == NW_CSS_PropValue_ltr) {
    *dir = NW_GDI_FlowDirection_LeftRight;
  }
  else {
    *dir = NW_GDI_FlowDirection_RightLeft;
  }
}

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_LMgr_ImgContainerBox_DrawBorder(NW_LMgr_Box_t* box,
                                   CGDIDeviceContext* deviceContext,
                                   NW_Bool isBroken,
                                   NW_Uint8 hasFocus)
{
  NW_TRY(status)
  {
    NW_LMgr_FrameInfo_t borderWidth;
    NW_LMgr_FrameInfo_t borderStyle;
    NW_LMgr_FrameInfo_t borderColor;
    NW_LMgr_PropertyValue_t focusBehavior;
    NW_GDI_Color_t initialBgColor = 0;
    NW_GDI_Color_t color = 0;
    NW_LMgr_Property_t backgroundProp;
    NW_GDI_Color_t backgroundColor = 0x00;
    NW_LMgr_Box_t* parentBox = NULL;
    NW_LMgr_IEventListener_t* eventListener;    
    
    NW_ASSERT(box != NULL);

    NW_LMgr_Box_GetBorderWidth (box, &borderWidth, ELMgrFrameAll );
    if( isBroken == NW_TRUE )  
      {
      // to make sure we always draw the border for the broken image link or button,
      // check the border width, if it is 0, set it to 1 
      if (borderWidth.top == 0) 
        {
        borderWidth.top = (NW_GDI_Metric_t)(borderWidth.top + 1);
        borderWidth.bottom = (NW_GDI_Metric_t)(borderWidth.bottom + 1);
        borderWidth.left = (NW_GDI_Metric_t)(borderWidth.left + 1);
        borderWidth.right = (NW_GDI_Metric_t)(borderWidth.right + 1);
        }
      }

    NW_LMgr_Box_GetBorderColor (box, &borderColor);
    NW_LMgr_Box_GetBorderStyle (box, &borderStyle, ELMgrFrameAll );
    initialBgColor = deviceContext->BackgroundColor();
 
    // Modify the look of the box if we're in focus 
    if (hasFocus)
    {
      focusBehavior.token = NW_CSS_PropValue_none;
      (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_focusBehavior,
                                           NW_CSS_ValueType_Token,
                                           &focusBehavior);

      if (focusBehavior.token == NW_CSS_PropValue_growBorder) 
      {
        borderWidth.top = (NW_GDI_Metric_t)(borderWidth.top + 1);
        borderWidth.bottom = (NW_GDI_Metric_t)(borderWidth.bottom + 1);
        borderWidth.left = (NW_GDI_Metric_t)(borderWidth.left + 1);
        borderWidth.right = (NW_GDI_Metric_t)(borderWidth.right + 1);

        if ((borderStyle.top == NW_CSS_PropValue_none) || 
            (borderStyle.top == NW_CSS_PropValue_hidden)) 
        {
          borderStyle.top = NW_CSS_PropValue_solid;
        }

        if ((borderStyle.bottom == NW_CSS_PropValue_none) || 
          (borderStyle.bottom == NW_CSS_PropValue_hidden)) 
        {
          borderStyle.bottom = NW_CSS_PropValue_solid;
        }

        if ((borderStyle.left == NW_CSS_PropValue_none) || 
          (borderStyle.left == NW_CSS_PropValue_hidden)) 
        {
          borderStyle.left = NW_CSS_PropValue_solid;
        }

        if ((borderStyle.right == NW_CSS_PropValue_none) || 
          (borderStyle.right == NW_CSS_PropValue_hidden)) 
        {
          borderStyle.right = NW_CSS_PropValue_solid;
        }
      }
    }

    eventListener = static_cast<NW_LMgr_IEventListener_t*>
      (NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class));

    if ((!hasFocus) && (!eventListener))
    {
      backgroundProp.type = NW_CSS_ValueType_Token;
      backgroundProp.value.token = NW_CSS_PropValue_transparent;

      parentBox = NW_LMgr_BoxOf(box->parent);
      while(parentBox != NULL)
      {
        if (NW_Object_IsClass(parentBox, &NW_LMgr_ActiveContainerBox_Class))
        {
          break; //we don't want to swap color for anchors
        }

        status = NW_LMgr_Box_GetProperty(parentBox, NW_CSS_Prop_backgroundColor, 
          &backgroundProp);

        if (status == KBrsrSuccess)
        {
          break;
        }
        parentBox = NW_LMgr_BoxOf(parentBox->parent);
      }

      if (backgroundProp.type == NW_CSS_ValueType_Color) 
      {
        backgroundColor = backgroundProp.value.integer;
        deviceContext->SetBackgroundColor( backgroundColor);
      }
    }

    if (!hasFocus)
    {
    // draw the border 
    color = borderColor.bottom;
    deviceContext->SwitchAgainstBG( &color);
    borderColor.bottom = color;
    color = borderColor.left;
    deviceContext->SwitchAgainstBG( &color);
    borderColor.left = color;
    color = borderColor.right;
    deviceContext->SwitchAgainstBG( &color);
    borderColor.right = color;
    color = borderColor.top;
    deviceContext->SwitchAgainstBG( &color);
    borderColor.top = color;
    }

    status = NW_LMgr_Box_DrawBorder( NW_LMgr_Box_GetDisplayBounds(box), deviceContext, &borderWidth, &borderStyle, &borderColor);
    NW_THROW_ON_ERROR(status);

    deviceContext->SetBackgroundColor( initialBgColor);
  }
  NW_CATCH (status)
  {}
  NW_FINALLY
  {
    return status;
  }
  NW_END_TRY
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_LMgr_ImgContainerBox_DisplayAltText(NW_LMgr_Box_t* box,
                                       NW_Text_t* altText,
                                       CGDIDeviceContext* deviceContext,
                                       NW_GDI_Metric_t space,
                                       NW_GDI_Point2D_t *textPos,
                                       CGDIFont *boxFont) 
{
  NW_Ucs2* storage = NULL;
  NW_Bool freeNeeded = NW_FALSE;
  NW_GDI_Color_t initialBgColor = 0;
  NW_GDI_Color_t initialFgColor = 0;
  NW_LMgr_Property_t backgroundProp;
  NW_GDI_Color_t backgroundColor = 0x00;
  NW_LMgr_Box_t* parentBox = NULL;
  NW_GDI_Color_t color = 0x00;
  NW_GDI_FlowDirection_t dir = NW_GDI_FlowDirection_LeftRight;

  NW_TRY(status)
  {
    NW_Text_Length_t length;
    CGDIFont* font;

    if (altText == NULL)
      {
      NW_THROW_SUCCESS(status);
    }

    /* Get the box font */
    font = boxFont;
    NW_THROW_OOM_ON_NULL(font, status);

    status = deviceContext->SplitText(altText, 
                                      font,
                                            space,
                                            0,
                                            NW_GDI_FlowDirection_LeftRight, 
                                            &length, 
                                            NW_GDI_SplitMode_Clip);

    NW_THROW_ON(status, KBrsrOutOfMemory);

    if ((status == KBrsrSuccess) && /* split succeeded */
         (length < NW_Text_GetCharCount(altText)))
      {
      NW_Ucs2* newAltText;
      NW_Uint16 flags = 0;
      NW_Text_Length_t len = 0;
      NW_Text_Length_t displayLength = 0;
      NW_Text_UCS2_t* truncateText;
      NW_GDI_Dimension2D_t truncateExtent;

      truncateText = NW_Text_UCS2_New ((NW_Ucs2*) NW_LMgr_BoxUtils_Truncate, 
                                       NW_LMgr_BoxUtils_Truncate_Len, 0);
      NW_THROW_OOM_ON_NULL(truncateText, status);

      truncateExtent.height = truncateExtent.width = 0;
      status =
        deviceContext->GetTextExtent ( NW_TextOf(truncateText),
                                       font,
                                       0, 
                                       NW_GDI_FlowDirection_LeftRight, 
                                            &truncateExtent);
      NW_Object_Delete(truncateText);

      length = 0;
      status = deviceContext->SplitText(altText, 
                                        font,
                                              (NW_GDI_Metric_t)(space-truncateExtent.width),
                                              0,
                                              NW_GDI_FlowDirection_LeftRight, 
                                              &length, 
                                              NW_GDI_SplitMode_Clip);
      NW_THROW_ON(status, KBrsrOutOfMemory);

      newAltText = NW_Str_New(length + NW_LMgr_BoxUtils_Truncate_Len);
      NW_THROW_OOM_ON_NULL(newAltText, status);      
    
      flags = NW_Text_Flags_Aligned | NW_Text_Flags_Copy | NW_Text_Flags_NullTerminated;
      storage = NW_Text_GetUCS2Buffer (altText, flags, &len, &freeNeeded);
      NW_THROW_OOM_ON_NULL(storage, status);

      NW_Mem_memset(newAltText, 0, (length+1+NW_LMgr_BoxUtils_Truncate_Len)*sizeof(NW_Ucs2));
      NW_Mem_memcpy(newAltText, storage, length*sizeof(NW_Ucs2));

      NW_Mem_memcpy(newAltText + length, NW_LMgr_BoxUtils_Truncate, 
                    NW_LMgr_BoxUtils_Truncate_Len*sizeof(NW_Ucs2));

      displayLength = (NW_Text_Length_t)(length+NW_LMgr_BoxUtils_Truncate_Len);
      status = NW_Text_SetStorage(altText, newAltText, displayLength, NW_TRUE);
      NW_THROW_ON_ERROR(status);
    }

    initialBgColor = deviceContext->BackgroundColor();
    initialFgColor = deviceContext->ForegroundColor();
    backgroundProp.type = NW_CSS_ValueType_Token;
    backgroundProp.value.token = NW_CSS_PropValue_transparent;

    parentBox = NW_LMgr_BoxOf(box->parent);
    while(parentBox != NULL)
    {
      status = NW_LMgr_Box_GetProperty(parentBox, NW_CSS_Prop_backgroundColor, &backgroundProp);
      if (status == KBrsrSuccess)
        {
        break;
      }
      parentBox = NW_LMgr_BoxOf(parentBox->parent);
    }

    if (backgroundProp.type == NW_CSS_ValueType_Color) 
      {
      backgroundColor = backgroundProp.value.integer;
      deviceContext->SetBackgroundColor( backgroundColor);
    }
    
    /* If the background color of the page is darker than half of the white color 
    indensity, the broken image description text should be white.
    */
    color = deviceContext->ForegroundColor();

    deviceContext->SwitchAgainstBG( &color);

    deviceContext->SetForegroundColor( color);

    NW_FBox_ImgContainerBox_GetDirection(box, &dir);

    status = deviceContext->DrawText( textPos, altText, font, 0, 
      (NW_GDI_TextDecoration_e)0, dir, space);
    NW_THROW_ON_ERROR (status);

    deviceContext->SetForegroundColor( initialFgColor);

    deviceContext->SetBackgroundColor( initialBgColor);
  }
  NW_CATCH (status)
    {
    }
  NW_FINALLY
  {
    if ((storage != NULL) && freeNeeded)
    {
      NW_Mem_Free(storage);
    }
    return status;
  }
  NW_END_TRY
}

// -----------------------------------------------------------------------------
// NW_LMgr_ImgContainerBox_HandleVerticalLayout
// This method implements the image handling rules of vertical layout algorithm 
//   1. scale the image maintaining the aspect ratio if width is greater than display width
//   2. scale the image maintaining the aspect ratio if height is greater than display height
//   3. If image is too small we do not want it to participate in the layout 
// Returns: void
// -----------------------------------------------------------------------------
//
static
void
NW_LMgr_ImgContainerBox_HandleVerticalLayout( NW_LMgr_Box_t *box,
                                              NW_GDI_Dimension2D_t *size,
                                              CGDIDeviceContext* deviceContext )
  {
  const NW_GDI_Rectangle_t* rectangle;
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_FrameInfo_t borderWidth;
  NW_GDI_Metric_t maxWidth = 0;
  NW_GDI_Metric_t maxHeight = 0;

  /* If image is too small we do not want it to participate in the layout */
  if ((size->width < NW_VerticalLayout_MinImageWidth1) || (size->height < NW_VerticalLayout_MinImageHeight1) ||
      ((size->width < NW_VerticalLayout_MinImageWidth2) && (size->height > NW_VerticalLayout_MinImageHeight2)))
    {
    /* if box is within <a> do not reduce size */
    /* if the box is an imagemap, do not reduce the size */
    if ((NW_LMgr_Box_FindBoxOfClass (box, &NW_LMgr_ActiveContainerBox_Class, NW_FALSE) == NULL)
        && ( !NW_Object_IsInstanceOf(box, &NW_LMgr_ImageMapBox_Class)))
      {
      size->width = 0;
      size->height = 0;    
      }
    }
  else
    {
    rectangle = deviceContext->DisplayBounds();
    NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );
    maxWidth = (NW_GDI_Metric_t)(rectangle->dimension.width - padding.left - padding.right - borderWidth.right - borderWidth.left - 6);
    maxHeight = (NW_GDI_Metric_t)(rectangle->dimension.width - padding.top - padding.bottom - borderWidth.top - borderWidth.bottom);

    /* scale the image maintaining the aspect ratio if width is greater than display width */
    if (size->width > maxWidth)
      {
      size->height = (NW_GDI_Metric_t)((maxWidth*size->height)/size->width);
      size->width = maxWidth;
      }
    /* scale the image maintaining the aspect ratio if height is greater than display height */
    if (size->height > maxHeight)
      {
      size->width = (NW_GDI_Metric_t)((maxHeight*size->width)/size->height);
      size->height = maxHeight;
      }
    }
  }

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

const
NW_LMgr_ImgContainerBox_Class_t  NW_LMgr_ImgContainerBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_MediaBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_ImgContainerBox_t),
    /* construct                 */ _NW_LMgr_ImgContainerBox_Construct,
    /* destruct                  */ _NW_LMgr_ImgContainerBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_ImgContainerBox_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_ImgContainerBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_ImgContainerBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_ImgContainerBox   */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// _NW_LMgr_ImgContainerBox_Construct
// Description:  The constructor.  
// Returns:      TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp)
  {
  NW_LMgr_ImgContainerBox_t* imageBox;

  NW_ASSERT( dynamicObject != NULL );

  /* for convenience */
  imageBox = NW_LMgr_ImgContainerBoxOf( dynamicObject );

  NW_TRY (status) 
    {
    NW_Ucs2* storage = NULL;
    NW_Bool freeNeeded = NW_FALSE;
    NW_Uint16 flags = 0;
    NW_Text_Length_t len = 0;


    /* invoke our superclass constructor */
    status = _NW_LMgr_Box_Construct( dynamicObject, argp );
    NW_THROW_ON_ERROR( status );
   
    /* initialize the member variables */
    imageBox->virtualImage = va_arg( *argp, NW_Image_AbstractImage_t* );
    NW_ASSERT( imageBox->virtualImage != NULL );

    if( imageBox->virtualImage ) 
      {
      NW_GDI_Rectangle_t bounds;
      NW_GDI_Dimension3D_t imageSize;

      bounds.point.x = 0;
      bounds.point.y = 0;
      if( NW_Image_AbstractImage_GetSize( imageBox->virtualImage, &imageSize ) == KBrsrSuccess )
        {
        bounds.dimension.width = imageSize.width;
        bounds.dimension.height = imageSize.height;
        }
      else
        {
        // default image box sizes
        bounds.dimension.width = NW_LMGR_BOX_MIN_WIDTH;
        bounds.dimension.height = NW_LMGR_BOX_MIN_HEIGHT;
        }
      NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( imageBox ), bounds );
      }
    // alt text
    imageBox->altText = va_arg( *argp, NW_Text_t* );
    if( imageBox->altText )
      {
      /* compress whitespaces */
      flags = NW_Text_Flags_Aligned | NW_Text_Flags_Copy | NW_Text_Flags_NullTerminated;
      flags |= NW_Text_Flags_CompressWhitespace;
      storage = NW_Text_GetUCS2Buffer (imageBox->altText, flags, &len, &freeNeeded);
      NW_THROW_OOM_ON_NULL(storage, status);
      status = NW_Text_SetStorage(imageBox->altText, storage, len, freeNeeded);
      NW_THROW_ON_ERROR(status);
      }
    // broken flag
    imageBox->isBroken = (NW_Bool) va_arg( *argp, NW_Uint32 );
    } 
  NW_CATCH (status) 
    {
    // image container does not take ownership if the construct failed.
    imageBox->virtualImage = NULL;
    imageBox->altText = NULL;
    } 
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }

// -----------------------------------------------------------------------------
// _NW_LMgr_ImgContainerBox_Destruct
// Description:  The destructor.  
// Returns:      void
// -----------------------------------------------------------------------------
//
void
_NW_LMgr_ImgContainerBox_Destruct( NW_Object_Dynamic_t* dynamicObject )
  {
  NW_LMgr_ImgContainerBox_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_ImgContainerBoxOf (dynamicObject);
  // destroy members
  NW_Object_Delete( thisObj->virtualImage );
  NW_Object_Delete( thisObj->altText );
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_GetMinimumContentSize( NW_LMgr_Box_t *box,
                                                NW_GDI_Dimension2D_t *size )
  {
  NW_TRY (status) 
    {
    NW_LMgr_ImgContainerBox_t* imageContainer;
    NW_GDI_Dimension3D_t imageSize;
    CGDIDeviceContext* deviceContext = NULL;
    NW_GDI_Dimension2D_t textSize;
    NW_LMgr_PropertyValue_t sizeVal;
    CGDIFont* font;
    NW_Bool cssHeightSpecified = NW_TRUE;
    NW_Bool cssWidthSpecified = NW_TRUE;
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

    size->height = 0;
    size->width = 0;
    textSize.height = 0;
    textSize.width = 0;

    /* for convenience */
    imageContainer = NW_LMgr_ImgContainerBoxOf( box );

    // image alt text first
    if( imageContainer->altText && imageContainer->isBroken == NW_TRUE )
      {
      // Get the box font
      font = NW_LMgr_Box_GetFont( box );
      NW_ASSERT( font != NULL );

      deviceContext = NW_LMgr_RootBox_GetDeviceContext( rootBox );

      status = deviceContext->GetTextExtent(  imageContainer->altText, 
                                            font,
                                            0, 
                                            NW_GDI_FlowDirection_LeftRight, 
                                            &textSize );
      NW_THROW_ON_ERROR( status );
      }   
    
    // Get image size
    if( imageContainer->virtualImage )
      {
      status = NW_Image_AbstractImage_GetSize( imageContainer->virtualImage, &imageSize );
      
      NW_THROW_ON_ERROR(status);

      status = NW_LMgr_Box_GetPropertyValue( box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal );
      
      if( status == KBrsrSuccess )
        {
        size->height = (NW_GDI_Metric_t)sizeVal.integer;
        }
      else
        {
        cssHeightSpecified = NW_FALSE;
        }

      status = NW_LMgr_Box_GetPropertyValue( box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal );

      if( status == KBrsrSuccess )
        {
        size->width = (NW_GDI_Metric_t)sizeVal.integer;
        }
      else
        {
        cssWidthSpecified = NW_FALSE;
        }
        
      // Resize images in grid mode tables to fit in their parent's box.
      NW_LMgr_Property_t prop;

      status = NW_LMgr_Box_GetProperty( box, NW_CSS_Prop_gridModeApplied, &prop );
      if( ( status == KBrsrSuccess ) && NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
        {
        NW_Int32        parentWidth = 0;
        NW_LMgr_Box_t*  parent;

        // Get the width of the parent.
        parent = box;
        while( ( parent = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent( parent ) ) != NULL )
          {
          if ( NW_Object_IsClass( parent, &NW_LMgr_StaticTableCellBox_Class ) )
            {
            NW_LMgr_FrameInfo_t  padding;
            NW_LMgr_FrameInfo_t  border;

            parentWidth = parent->iFormatBounds.dimension.width;

            // Subtract the padding and border widths.
            NW_LMgr_StaticTableCellBox_GetPadding( ( NW_LMgr_StaticTableCellBox_t* ) parent,
                &padding);

            NW_LMgr_Box_GetBorderWidth( parent, &border, ELMgrFrameAll );

            parentWidth -= (padding.left + padding.right + border.left + border.right);
            break;
            }
          }

        // If the image is widder than the parent then resize it to fit.
        // The aspect ratio is maintained.
        if( ( parentWidth > 0 ) && ( size->width > parentWidth ) )
          {
          NW_Float32 temp;

          temp = ( ( NW_Float32 ) parentWidth / size->width )  * size->height;

          size->width = parentWidth;
          size->height = ( NW_GDI_Metric_t ) temp;
          }
        }

      if( ( size->height != 0 ) && ( !cssWidthSpecified ) && ( imageSize.height != 0 ) )
        {
        size->width = (NW_GDI_Metric_t)( ( size->height * imageSize.width ) / imageSize.height );
        }
      else if( ( size->height != 0 ) && ( size->width == 0 ) && ( imageSize.height != 0 ) )
        {
        // In order to conform to IE and Opera look and feel, display a 1 pixel wide line
        size->width = 1;
        }

      if( ( size->width != 0) && ( !cssHeightSpecified ) && ( imageSize.width != 0 ) )
        {
        size->height = (NW_GDI_Metric_t)( ( size->width * imageSize.height ) / imageSize.width );
        }
      else if( ( size->width != 0) && ( size->height == 0 ) && ( imageSize.width != 0 ) )
        {
        // In order to conform to IE and Opera look and feel, display a 1 pixel high line
        size->height = 1;
        }
      
      if( ( !cssWidthSpecified ) && ( !cssHeightSpecified ) )
        {
        size->width = (NW_GDI_Metric_t)( imageSize.width + textSize.width );
      
        if( imageSize.height > textSize.height )
          {
          size->height = imageSize.height;
          }
        else
          {
          size->height = textSize.height;
          }
        }
      }  
    // In case of vertical layout is enabled implement the image handling algorithm 
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
      {
      if( deviceContext == NULL )
        {
        deviceContext = NW_LMgr_RootBox_GetDeviceContext( NW_LMgr_Box_GetRootBox( box ) );
        }
      
      NW_LMgr_ImgContainerBox_HandleVerticalLayout( box, size, deviceContext );
      }
    } 
  NW_CATCH (status) 
    {  
    } 
  NW_FINALLY 
    {
    return status;  
    } 
  NW_END_TRY
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_Split(NW_LMgr_Box_t* box, 
                               NW_GDI_Metric_t space, 
                               NW_LMgr_Box_t* *splitBox,
                               NW_Uint8 flags)
  {
  NW_LMgr_PropertyValue_t sizeVal;
  TBrowserStatusCode status;
  NW_LMgr_ImgContainerBox_t* thisObj;

  NW_ASSERT(box != NULL);

  /* for convenience */
  thisObj = NW_LMgr_ImgContainerBoxOf( box );

  if( thisObj->isBroken )
    {
    *splitBox = NULL;
    /* If box does not fit in the space and the box is not
     on the new line the input box needs to be pushed on the new line */
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
    if ((boxBounds.dimension.width > space) && !(flags & NW_LMgr_Box_SplitFlags_AtNewLine))
      {
      return KBrsrLmgrNoSplit;
      }

    if(boxBounds.dimension.width <= space) 
      {
      return KBrsrSuccess;
      }

    /* we are more than space */

    /* if width was specified we do not want to change it */
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
    if (status == KBrsrSuccess)
      {
      return status;
      }
    boxBounds.dimension.width = space;
    NW_LMgr_Box_SetFormatBounds( box, boxBounds );
    }
  else
    {
    // real image split is handled by base class    
    return NW_LMgr_Box_Class.NW_LMgr_Box.split(box, space, splitBox, flags );
    }
  return KBrsrSuccess;
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ImgContainerBox_Draw( NW_LMgr_Box_t* box,
                               CGDIDeviceContext* deviceContext,
                               NW_Uint8 hasFocus )
  {
  NW_TRY( status ) 
    {
    NW_LMgr_ImgContainerBox_t* imageContainer;

    NW_ASSERT( box != NULL );
    
    imageContainer = NW_LMgr_ImgContainerBoxOf( box );
    // height or width == 0 implies that there is no need to draw the image
    // as the box does not occupy any space on displa.  The box is also not drawn
    // if the box hasn't been "placed".
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );

    if ((boxBounds.dimension.width == 0) || (boxBounds.dimension.height == 0) ||
        !imageContainer->iBoxPlaced)
      {
      NW_THROW_SUCCESS(status);
      }
    
    /* invoke our superclass draw method first */
    status = _NW_LMgr_Box_Draw( box, deviceContext, hasFocus );
    NW_THROW_ON_ERROR( status );

    status = NW_LMgr_ImgBox_DrawContent( box, deviceContext, imageContainer->virtualImage,
      imageContainer->isBroken, imageContainer->altText, hasFocus ); 
    NW_THROW_ON_ERROR( status );
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

/* ------------------------------------------------------------------------- *
   public final methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
void
NW_LMgr_ImgContainerBox_ReplaceBrokenImage( NW_LMgr_Box_t* box, NW_Image_AbstractImage_t* image )
  {
  NW_LMgr_ImgContainerBox_t* thisObj;

  NW_ASSERT( box != NULL );

  /* for convenience */
  thisObj = NW_LMgr_ImgContainerBoxOf( box );

  // free old image
  NW_Object_Delete( thisObj->virtualImage );
  // and replace it
  thisObj->virtualImage = image;  
  // update broken flag
  thisObj->isBroken = NW_FALSE;
  }

NW_Bool
NW_LMgr_ImgContainerBox_ImageIsBroken( NW_LMgr_Box_t* box )
  {
  NW_LMgr_ImgContainerBox_t* thisObj;

  NW_ASSERT( box != NULL );

  /* for convenience */
  thisObj = NW_LMgr_ImgContainerBoxOf( box );

  return thisObj->isBroken;
  }

void
NW_LMgr_ImgContainerBox_ImageIsPlaced(NW_LMgr_Box_t* box)
  {
  NW_LMgr_ImgContainerBox_t* thisObj;

  NW_ASSERT( box != NULL );

  /* for convenience */
  thisObj = NW_LMgr_ImgContainerBoxOf( box );

  thisObj->iBoxPlaced = NW_TRUE;
  }


/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
NW_LMgr_ImgContainerBox_t*
NW_LMgr_ImgContainerBox_New (NW_ADT_Vector_Metric_t numProperties, NW_Image_AbstractImage_t* virtualImage, 
                             NW_Text_t* altText, NW_Bool brokenImage )
{
  return (NW_LMgr_ImgContainerBox_t*)
    NW_Object_New (&NW_LMgr_ImgContainerBox_Class, numProperties, virtualImage, altText, (NW_Uint32)brokenImage );
}

/* ------------------------------------------------------------------------- *
  global utility functions
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_LMgr_ImgBox_DrawContent( NW_LMgr_Box_t* box,
                            CGDIDeviceContext* deviceContext,
                            NW_Image_AbstractImage_t* image,
                            NW_Bool isBroken,
                            NW_Text_t* altText,
                            NW_Uint8 hasFocus)
{
  NW_TRY( status ) 
  {
    NW_GDI_Point2D_t imagePos;
    NW_GDI_Point2D_t textPos;
    NW_GDI_Dimension3D_t imageSize;
    NW_GDI_Rectangle_t innerRect;
    CGDIFont* font = NW_LMgr_Box_GetFont (box);

    NW_ASSERT( box != NULL );
    NW_ASSERT( image != NULL );

    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(box);
    NW_ASSERT(rootBox);

    NW_HED_AppServices_t* appServices = rootBox->appServices;
    NW_ASSERT(appServices);

    // if we are in image map view, with a broken image, just draw the border
    if (appServices->browserAppApi.IsImageMapView() && isBroken)
      {
        status = NW_LMgr_ImageMapBox_DrawBorder( box, deviceContext);
        NW_THROW_ON_ERROR(status);     
      }
    else
      {
        // draw image
        if( image )
        {
          // make our image draw itself at the specified location
          deviceContext->SetPaintMode(  NW_GDI_PaintMode_Copy );
          // get image size 
          NW_LMgr_Property_t hProp;
          NW_LMgr_Property_t wProp;
          
          NW_LMgr_Box_GetInnerRectangle( box, &innerRect );

          if( (NW_LMgr_Box_GetProperty( box, NW_CSS_Prop_height, &hProp ) == KBrsrSuccess) &&
              (NW_LMgr_Box_GetProperty( box, NW_CSS_Prop_width, &wProp ) == KBrsrSuccess) )
            {
            imageSize.width  = wProp.value.integer;        
            imageSize.height = hProp.value.integer;
            }
          else
            {
            status = NW_Image_AbstractImage_GetSize( image, &imageSize );
            NW_THROW_ON_ERROR(status);
            }
  
          NW_LMgr_Box_GetInnerRectangle( box, &innerRect );
        
          // do not stretch out broken images
          if( isBroken == NW_TRUE )  
          {
            if( innerRect.dimension.height < imageSize.height )
            {
              imageSize.height = innerRect.dimension.height;
            }
            if( innerRect.dimension.width < imageSize.width )
            {
              imageSize.width = innerRect.dimension.width;
            }
          }
          else
          {
            imageSize.width = innerRect.dimension.width;
            imageSize.height = innerRect.dimension.height;
            imageSize.depth = 0; 
          }

          imagePos.x = innerRect.point.x;
          imagePos.y = innerRect.point.y;
    
          status = NW_Image_AbstractImage_DrawScaled( image, 
                                                      deviceContext,
                                                      &imagePos, 
                                                      &imageSize );
          NW_THROW_ON_ERROR(status);

          // draw altext
          if( isBroken == NW_TRUE && 
              altText && innerRect.dimension.width > imageSize.width )
          {
            textPos.x = (NW_GDI_Metric_t)(imagePos.x + imageSize.width);
            textPos.y = imagePos.y;

              status = NW_LMgr_ImgContainerBox_DisplayAltText(box, 
                                                              altText, 
                        deviceContext, 
                        (NW_GDI_Metric_t)(innerRect.dimension.width - imageSize.width),
                                                              &textPos,
                                                              font);
            NW_THROW_ON_ERROR (status);
          }
        }

        /* draw the border */
        if( NW_Object_IsClass(box, &NW_LMgr_ImageMapBox_Class))
        {
          status = NW_LMgr_ImageMapBox_DrawBorder(box, deviceContext);
          NW_THROW_ON_ERROR(status);
        }
        else
        {
          status = NW_LMgr_ImgContainerBox_DrawBorder( box, deviceContext, isBroken, hasFocus );
          NW_THROW_ON_ERROR(status);    
        } 
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
