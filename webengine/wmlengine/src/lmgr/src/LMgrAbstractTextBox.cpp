/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_abstracttextboxi.h"
#include "nw_lmgr_splittextbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_lmgr_nobrbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_text_ucs2.h"
#include "nwx_settings.h"
#include "nwx_string.h"
#include "nw_lmgr_activecontainerbox.h"
#include "urlloader_urlloaderint.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"
#include "nwx_http_defs.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

static
NW_LMgr_Box_t*
NW_LMgr_AbstractTextBox_SplitAt (NW_LMgr_AbstractTextBox_t* thisObj, 
                                 NW_Text_t *displayText,
                                 NW_Text_Length_t splitLength) 
  {
  NW_Text_Length_t totalLength = NW_Text_GetCharCount(displayText);
  NW_LMgr_SplitTextBox_t *splitBox;

  NW_ASSERT(totalLength - splitLength > 0);

  /* Create the split box */
  splitBox = NW_LMgr_SplitTextBox_New(thisObj, 
                                      (NW_Text_Length_t)(totalLength - splitLength));
  if (splitBox == NULL) 
    {
    return NULL;
  }

  /* Update our member variables */
  NW_LMgr_AbstractTextBox_SetLength(thisObj, splitLength);

  return (NW_LMgr_Box_t*)splitBox;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetTextProperties (NW_LMgr_Box_t *box, 
                                           CGDIFont **font, 
                                           NW_GDI_Color_t *color, 
                                           NW_ADT_Token_t *textDecoration,
                                           NW_Bool *compressWS,
                                           NW_ADT_Token_t *textTransformation) 
{
  NW_LMgr_PropertyValue_t textProp;
  NW_LMgr_Box_t* parentBox = NW_LMgr_BoxOf(box->parent);

  NW_ASSERT (font != NULL);

  NW_TRY(status) 
    {
    *font = NW_LMgr_Box_GetFont (parentBox);
    NW_ASSERT (*font != NULL);

    if (color) 
      {
      textProp.integer = 0;

      status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_color, NW_CSS_ValueType_Color, &textProp);
      if (status != KBrsrNotFound) 
        {
        NW_THROW_ON_ERROR(status);
      }

      *color = textProp.integer;
    }

    if (textDecoration) 
      {
      textProp.token = 0;

      status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_textDecoration, NW_CSS_ValueType_Token, &textProp);
      if (status != KBrsrNotFound) 
        {
        NW_THROW_ON_ERROR(status);
      }

      *textDecoration = textProp.token;
    }

    if (compressWS) 
      {
      textProp.token = NW_CSS_PropValue_normal;

      status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_whitespace, NW_CSS_ValueType_Token, &textProp);
      if (status != KBrsrNotFound) 
        {
        NW_THROW_ON_ERROR(status);
      }

      *compressWS = (NW_Bool)((textProp.token == NW_CSS_PropValue_pre) ? NW_FALSE : NW_TRUE);
    }

    if (textTransformation) 
      {
      textProp.token = NW_CSS_PropValue_none;

      status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_textTransform, NW_CSS_ValueType_Token, &textProp);
      if (status != KBrsrNotFound) 
        {
        NW_THROW_ON_ERROR(status);
      }

      *textTransformation = textProp.token;
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

/* ------------------------------------------------------------------------- */
static
void
NW_LMgr_AbstractTextBox_GetDeviceContext (CGDIDeviceContext * deviceContext,
                                          NW_GDI_Color_t *oldColor, 
                                          NW_GDI_PaintMode_t *oldPaintMode)
{
  NW_ASSERT(oldColor != NULL);
  NW_ASSERT(oldPaintMode != NULL);
  
  *oldColor = deviceContext->ForegroundColor ();
  *oldPaintMode = deviceContext->PaintMode ();
}

/* ------------------------------------------------------------------------- */
static
void
NW_LMgr_AbstractTextBox_SetDeviceContext (CGDIDeviceContext *deviceContext,
                                          NW_GDI_Color_t color, 
                                          NW_GDI_PaintMode_t paintMode)
{
    deviceContext->SetForegroundColor( color);

    deviceContext->SetPaintMode ( paintMode);
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_AbstractTextBox_ApplyTransform (NW_LMgr_AbstractTextBox_t *textBox,
                                        NW_Ucs2 *inputString,
                                        NW_Ucs2 **outputString)
{
  NW_LMgr_PropertyValue_t textTransformation;

  NW_TRY(status) {

    /* Check for the possible existence of transformations on the text style */
    textTransformation.token = NW_CSS_PropValue_none;
    status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(textBox), 
                                          NW_CSS_Prop_textTransform, 
                                          NW_CSS_ValueType_Token,
                                          &textTransformation);
    NW_THROW_ON(status, KBrsrOutOfMemory);
  
    *outputString = inputString;
    switch (textTransformation.token)
    {
    case NW_CSS_PropValue_capitalize:
      status = NW_Str_Ucs2StrTitle(outputString);
      NW_THROW_ON_ERROR(status);
      break;
  
    case NW_CSS_PropValue_uppercase:
      status = NW_Str_Ucs2StrUpr(outputString);
      NW_THROW_ON_ERROR(status);
      break;

    case NW_CSS_PropValue_lowercase:
      status = NW_Str_Ucs2StrLwr(outputString);
      NW_THROW_ON_ERROR(status);
      break;

    default:
      break;
    }
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY;
}
    
/* ------------------------------------------------------------------------- */
/* The static instance of the Text_Box class object */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_AbstractTextBox_Class_t  NW_LMgr_AbstractTextBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_MediaBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_AbstractTextBox_t),
    /* construct                 */ _NW_LMgr_Box_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_AbstractTextBox_Split,
    /* resize                    */ _NW_LMgr_AbstractTextBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_AbstractTextBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_AbstractTextBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_AbstractTextBox_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_AbstractTextBox   */
    /* getText                   */ NULL,
    /* getStart                  */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Resize (NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context)
{
  NW_REQUIRED_PARAM(context);
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  TBrowserStatusCode status = NW_LMgr_AbstractTextBox_GetDimensions(NW_LMgr_AbstractTextBoxOf(box), &(boxBounds.dimension) );
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );
  return status;
}

/* ------------------------------------------------------------------------- */
/* TODO: contains LR-specific code */
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_GetMinimumContentSize(NW_LMgr_Box_t* box,
                                               NW_GDI_Dimension2D_t* size )
{      
  NW_LMgr_PropertyValue_t value;
  NW_ADT_Token_t whitespace;

  NW_TRY(status) {
    /* Get the "white-space" property */
    value.token = NW_CSS_PropValue_normal;
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_whitespace, NW_CSS_ValueType_Token, &value);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

    // The nowrap property may come from a tablecell.
    if (status == KBrsrNotFound && !NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) ) 
        {
        NW_LMgr_Box_t* parentBox = NW_LMgr_BoxOf(box->parent);
        if (NW_Object_IsInstanceOf(parentBox, &NW_LMgr_StaticTableCellBox_Class))
            {
            status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_whitespace, NW_CSS_ValueType_Token, &value);
            NW_THROW_ON(status, KBrsrOutOfMemory);
            }
        }    
    
    whitespace = value.token;
    
    status = NW_LMgr_AbstractTextBox_GetDimensions(NW_LMgr_AbstractTextBoxOf(box), size);

    if( NW_LMgr_RootBox_GetWrapEnabled( rootBox ) || (whitespace != NW_CSS_PropValue_nowrap)) 
        {
        size->width = NW_LMGR_BOX_MIN_WIDTH;
        }
  }

  NW_CATCH(status) { /* empty */ }

  NW_FINALLY {
    return status;

  } NW_END_TRY;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Draw (NW_LMgr_Box_t* box,
                       CGDIDeviceContext* deviceContext,
                       NW_Uint8 hasFocus)
{
  NW_LMgr_AbstractTextBox_t* textBox = NULL;
  CGDIFont *font = NULL;
  NW_GDI_PaintMode_t paintMode, oldPaintMode;  
  NW_GDI_Color_t color, oldColor;
  NW_ADT_Token_t textDecoration;
  NW_Bool compressWS;
  NW_GDI_Point2D_t point;
  NW_LMgr_RootBox_t *rootBox = NULL;
  NW_Text_t* displayText = NULL;
  NW_GDI_TextDecoration_t drawDecor = NW_GDI_TextDecoration_None;
  NW_GDI_FlowDirection_t globalDir;
  NW_LMgr_PropertyValue_t dirValue;
  NW_LMgr_Box_t* parentBox = NULL;

  /* For convenience */
  textBox = NW_LMgr_AbstractTextBoxOf (box);

  /* Get the DeviceContext state */
  NW_LMgr_AbstractTextBox_GetDeviceContext ( deviceContext, &oldColor, &oldPaintMode);

  NW_TRY(status) 
    {
    /* Get the displayable text */
    displayText = NW_LMgr_AbstractTextBox_GetDisplayText(textBox, NW_TRUE);
    NW_THROW_OOM_ON_NULL(displayText, status);

    /* Get the text direction */
    dirValue.token = NW_CSS_PropValue_ltr;
    status = NW_LMgr_Box_GetPropertyValue(box, 
                                          NW_CSS_Prop_textDirection, 
                                          NW_CSS_ValueType_Token,
                                          &dirValue);
    NW_THROW_ON(status, KBrsrOutOfMemory);

	//If the status is KBrsrNotFound, it does not necessary mean that the text direction should be from LEFT to RIGHT. 
	//It only means that the text direction property is not set in the <html> tag. The actual direction still
	//can be from RIGHT to LEFT. For example, if the language is arabic, it has to be RIGHT to LEFT.
	//We need to try one more time. That is to find the information from the text itself.
	if(status == KBrsrNotFound)
	{
		 NW_Text_Length_t length;
		 TBool freeNeeded = NW_FALSE;
		 TText16* string = NW_Text_GetUCS2Buffer (displayText,
                             NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy, 
                             &length, 
                             (unsigned char*) &freeNeeded);
		TPtrC ptrC(string);
		const TText* text = ptrC.Ptr();
		TChar::TBdCategory category = TChar(*text).GetBdCategory();  //we just test the 1st char
		if(category == TChar::ERightToLeft ||category == TChar::ERightToLeftArabic)
		{
			globalDir = NW_GDI_FlowDirection_RightLeft;
		}
		else
		{
			globalDir = NW_GDI_FlowDirection_LeftRight;
		}
		if (freeNeeded)
		{
			NW_Mem_Free (string);
		}
	}
	else  //found. We use whatever specified
	{
		if (dirValue.token == NW_CSS_PropValue_ltr) 
		{
		globalDir = NW_GDI_FlowDirection_LeftRight;
		}
		else 
		{
		globalDir = NW_GDI_FlowDirection_RightLeft;
		}
	}
    /* Invoke our superclass draw method first. 
     */
    status = NW_LMgr_MediaBox_Class.NW_LMgr_Box.draw (box, deviceContext, hasFocus);

    NW_THROW_ON_ERROR(status);

    /* Get the box properties */
    status = NW_LMgr_AbstractTextBox_GetTextProperties (box, &font, &color, &textDecoration, 
                                                        &compressWS, NULL);
    NW_THROW_ON_ERROR(status);

    /* TODO: if cache is empty, ignore CSS imposed by "a:visited" pseudo-class,
             but for now we just fix the color */

    /* Check for link by looking at parent container. */
    if (NW_Object_IsClass(NW_LMgr_Box_GetParent(box), &NW_LMgr_ActiveContainerBox_Class))
    {
      /* If Cache is empty, force links to be drawn in uncached color (blue). 
         NOTE: GetNumEntries allocates a list of the cached files, so avoid it for non-links. */
      //if (UrlLoader_GetNumCacheEntries() == 0)
      //{
      //  color = NW_GDI_Color_Blue;
      //  if (NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_color, &tempProp) == KBrsrSuccess)
      //  {
      //    color = tempProp.value.integer;
      //  }
      //}
    }

    if (hasFocus)
    {
      NW_LMgr_PropertyValue_t focusBehavior;

      focusBehavior.token = NW_CSS_PropValue_none;
      status = NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_focusBehavior,
                                           NW_CSS_ValueType_Token,
                                           &focusBehavior);

      if (focusBehavior.token != NW_CSS_PropValue_altText) 
        {       
       /* text of the focused anchor is always black */
      color = 0x00000000;

      deviceContext->SetForegroundColor ( (NW_GDI_Color_t)0x00777777);
     }
    }

    /* Prepare the DeviceContext */
    paintMode = NW_GDI_PaintMode_Copy;
    if (hasFocus) 
      {
      paintMode = (NW_GDI_PaintMode_e) (paintMode | NW_GDI_PaintMode_Not);
    }

    if ((textDecoration == NW_CSS_PropValue_blink) || (textDecoration == NW_CSS_PropValue_blinkUnderline))
    {
      /* In case of blink, we draw the TextBox only at every second "round". */
      rootBox = NW_LMgr_Box_GetRootBox (box);
      if (rootBox->blinkState) 
        {
        NW_THROW_SUCCESS(status);
      }
    }

    NW_LMgr_AbstractTextBox_SetDeviceContext ( deviceContext, color, paintMode);

    /* Now draw the text */
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
    point = boxBounds.point;

    if ((textDecoration == NW_CSS_PropValue_underline)  || (textDecoration == NW_CSS_PropValue_blinkUnderline))
      {
      drawDecor = NW_GDI_TextDecoration_Underline;
    }

    if (textDecoration == NW_CSS_PropValue_strikeout)
      {
      drawDecor = NW_GDI_TextDecoration_Strikeout;
    }

    /* In vertical-layout mode, We want to remove the nbsp, if they are within a table cell 
       For e.g. <td> &nbsp; </td> DeviceContext will use the iRemoveNbsp flag while calculating the text
       extent and drawing the text
    */
    parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(textBox);
    if (NW_Object_IsInstanceOf(parentBox, &NW_LMgr_VerticalTableCellBox_Class))
    {
      deviceContext->SetRemoveNbsp (NW_TRUE);
    }
    status = deviceContext->DrawText (&point, 
                                      displayText, 
                                      font,
                                      0, 
                                      drawDecor,
                                      globalDir,
                                      boxBounds.dimension.width);
    if (deviceContext->RemoveNbsp())
    {
      deviceContext->SetRemoveNbsp (NW_FALSE);
    }
    NW_THROW_ON_ERROR(status);

    /* Reset the dev context */
    NW_LMgr_AbstractTextBox_SetDeviceContext ( deviceContext,
                                               oldColor, oldPaintMode);
  }
  NW_CATCH(status) 
    {
  }
  NW_FINALLY 
    {
    NW_Object_Delete(displayText);
    return status;
    } 
  NW_END_TRY;
}

/* ------------------------------------------------------------------------- */
NW_GDI_Metric_t
_NW_LMgr_AbstractTextBox_GetBaseline (NW_LMgr_Box_t* box)
{
  NW_LMgr_Box_t* parentBox = NW_LMgr_BoxOf(box->parent);
  CGDIFont* font;
  TGDIFontInfo fontInfo;

  /* parameter assertion block */
  NW_ASSERT (box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_AbstractTextBox_Class));

  /* we're going to need the DeviceContext to get the FontInfo */

  /* Set the font to match the parent. TODO: this may not be the best policy */
  font = NW_LMgr_Box_GetFont (parentBox);
  if (font != NULL) 
    {
    TBrowserStatusCode status = font->GetFontInfo ( &fontInfo);
    NW_ASSERT (status == KBrsrSuccess);

    if (NW_LMgr_Box_GetFormatBounds(box).dimension.height > 0) 
      {
      return fontInfo.baseline;
    } 
    else 
      {
      return 0;
    }
  }

  /* unable to get the FontInfo we return our height */
  return NW_LMgr_Box_GetFormatBounds(box).dimension.height;
}

/* ------------------------------------------------------------------------- *
 * Split splits the TextBox based on its "white-space" property,
 * and any flags that are passed in from the container doing the layout.
 *
 * The whitespace property specifies the way paragraphs will
 * be formatted.  Its values signify the following:
 * 1) white-space == nowrap:  whitespace in the string is
 *    compressed, and no splitting is done until the end of paragraph
 *    is reached.
 * 2) white-space == pre: whitespace in the string is conserved,
 *    and the only line breaks occur at the CR character (0x0a).
 * 3) white-space == normal:  whitespace is compressed and
 *    the text is word-wrapped.  If the text can't be wrapped within
 *    the constraint, by default the smallest possible split is returned
 *    which preserves the word boundaries.  
 *
 * The allowed flags are the following:
 * 1) NW_LMgr_Box_IsNewline: indicates that this box is at a newline.
 */

TBrowserStatusCode
_NW_LMgr_AbstractTextBox_Split (NW_LMgr_Box_t* box, 
                                NW_GDI_Metric_t space, 
                                NW_LMgr_Box_t** splitBox,
                                NW_Uint8 flags)
{
  NW_LMgr_AbstractTextBox_t* textBox;
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  CGDIFont* gdiFont;
  NW_LMgr_Box_t* parent;
  NW_Text_t* displayText = NULL;
  NW_LMgr_PropertyValue_t value;
  NW_ADT_Token_t whitespace;
  NW_Bool mustSplit;
  NW_Text_Length_t length = 0;
  NW_GDI_FlowDirection_t globalDir;
  NW_LMgr_PropertyValue_t dirValue;

  /* parameter assertion block */
  NW_ASSERT (box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_AbstractTextBox_Class));

  /* for convenience */
  textBox = NW_LMgr_AbstractTextBoxOf (box);

  /* we're going to need the deviceContext to do text extent calculations */
  rootBox = (NW_LMgr_RootBox_t*) NW_LMgr_Box_GetRootBox (box);
  NW_ASSERT (rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  NW_ASSERT (deviceContext != NULL);

  /* Get the font for the box */
  gdiFont = NW_LMgr_Box_GetFont (box);
  NW_ASSERT(gdiFont != NULL);

  NW_TRY(status) {

    /* In vertical layout mode we do not want to have extra space when visibility is hidden */
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
    {
      NW_LMgr_PropertyValue_t visibilityVal;
      visibilityVal.token = NW_CSS_PropValue_visible;
      status = NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                     NW_CSS_ValueType_Token, &visibilityVal);
      if (status  == KBrsrSuccess && visibilityVal.token == NW_CSS_PropValue_hidden) 
      {
        /* Do not split*/
        *splitBox = NULL;
        NW_THROW_SUCCESS(status);
      }
    }

    /* Get the text object */
    displayText = NW_LMgr_AbstractTextBox_GetDisplayText(textBox, NW_FALSE);
    NW_THROW_OOM_ON_NULL(displayText, status);

    /* Get the text direction */
    dirValue.token = NW_CSS_PropValue_ltr;
    status = NW_LMgr_Box_GetPropertyValue(box, 
                                          NW_CSS_Prop_textDirection, 
                                          NW_CSS_ValueType_Token,
                                          &dirValue);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    if (dirValue.token == NW_CSS_PropValue_ltr) 
      {
      globalDir = NW_GDI_FlowDirection_LeftRight;
    }
    else 
      {
      globalDir = NW_GDI_FlowDirection_RightLeft;
    }

    /* Get the "white-space" property */
    value.token = NW_CSS_PropValue_normal;
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_whitespace, NW_CSS_ValueType_Token, &value);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // The nowrap property may come from a tablecell.
    if( status == KBrsrNotFound && !NW_LMgr_RootBox_GetSmallScreenOn( rootBox )) 
      {
        NW_LMgr_Box_t* parentBox = NW_LMgr_BoxOf(box->parent);
        if (NW_Object_IsInstanceOf(parentBox, &NW_LMgr_StaticTableCellBox_Class))
        {
            status = NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_whitespace, NW_CSS_ValueType_Token, &value);
            NW_THROW_ON(status, KBrsrOutOfMemory);
        }
    }    
    
    whitespace = value.token;
    
    if( NW_LMgr_RootBox_GetWrapEnabled( rootBox ) && (whitespace == NW_CSS_PropValue_nowrap)) {

        /* if the current box (or ancestor) is a 'nobr' box then don't override the nowrap property */
        NW_LMgr_Box_t* tempBox = box;
        NW_Bool nobrPresent = NW_FALSE;
        while (tempBox != NULL && !nobrPresent)
        {
            nobrPresent = NW_Object_IsInstanceOf(tempBox, &NW_LMgr_NobrBox_Class);
            tempBox = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent (tempBox);
        }

      if(!nobrPresent) 
        {
          NW_LMgr_Box_t *surroundingBlock = NW_LMgr_Box_FindBoxOfClass(box, &NW_LMgr_FlowBox_Class, NW_TRUE);
        if (!NW_Object_IsInstanceOf(surroundingBlock, &NW_LMgr_MarqueeBox_Class)) 
          {
            whitespace = NW_CSS_PropValue_normal;
          }
        }
    }

    /* Get the flags */
    if ((flags & NW_LMgr_Box_SplitFlags_NoPendingWS) || (flags & NW_LMgr_Box_SplitFlags_AtNewLine)) 
      {
      mustSplit = NW_TRUE;
    }
    else 
      {
      mustSplit = NW_FALSE;
    }

    /* Do the split */
    switch (whitespace) 
      {    
    case NW_CSS_PropValue_pre:
        status = deviceContext->SplitText( displayText, 
                                           gdiFont,
                                           -1, 
                                           0,
                                           globalDir, 
                                           &length, 
                                                NW_GDI_SplitMode_Newline);
      NW_THROW_ON(status, KBrsrOutOfMemory);
      break;

    case NW_CSS_PropValue_nowrap:
      length = NW_Text_GetCharCount(displayText);
      status = KBrsrSuccess;
      /* sets the parents' width to be the same as box width unless the parent is wider*/
      for (parent = NW_LMgr_BoxOf(box->parent); parent != NULL; parent = NW_LMgr_BoxOf(parent->parent))
        {
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
        NW_GDI_Rectangle_t parentBoxBounds = NW_LMgr_Box_GetFormatBounds( parent );
          
        if( NW_Object_IsInstanceOf (parent, &NW_LMgr_MarqueeBox_Class) ||
            parentBoxBounds.dimension.width > boxBounds.dimension.width )
    	   {
	      break;
          } 
        else
          {
              parentBoxBounds.dimension.width = boxBounds.dimension.width; 
              NW_LMgr_Box_SetFormatBounds( parent, parentBoxBounds );
          }
        }
      break;

    case NW_CSS_PropValue_flags_smallScreenPre:
        status = deviceContext->SplitText( displayText, 
                                           gdiFont,
                                           space, 
                                           0,
                                           globalDir, 
                                           &length, 
                                                NW_GDI_SplitMode_Newline);
      NW_THROW_ON(status, KBrsrOutOfMemory);
        if ( status == KBrsrLmgrNoSplit ) 
          {
          status = deviceContext->SplitText( displayText, 
                                             gdiFont,
                                             space, 
                                             0,
                                             globalDir, 
                                             &length, 
                                              NW_GDI_SplitMode_WordWrap);
        NW_THROW_ON(status, KBrsrOutOfMemory);

          if ( status == KBrsrLmgrNoSplit && 
               mustSplit && 
              (flags & NW_LMgr_Box_SplitFlags_AtNewLine) ) 
            {
          /* If wrap is enabled, we must clip */
            status = deviceContext->SplitText( displayText, 
                                               gdiFont,
                                               space, 
                                               0,
                                               globalDir, 
                                               &length, 
                                                NW_GDI_SplitMode_Clip);
          NW_THROW_ON(status, KBrsrOutOfMemory);
        }
      }
      break;

    case NW_CSS_PropValue_normal:
    default:
        status = deviceContext->SplitText( displayText, 
                                           gdiFont,
                                           space, 
                                           0,
                                           globalDir, 
                                           &length, 
                                              NW_GDI_SplitMode_WordWrap);
      NW_THROW_ON(status, KBrsrOutOfMemory);

        if ( status == KBrsrLmgrNoSplit && 
             mustSplit && 
             (flags & NW_LMgr_Box_SplitFlags_AtNewLine) ) 
          {
        /* If wrap is enabled, we must clip */
          status = deviceContext->SplitText( displayText, 
                                             gdiFont,
                                             space, 
                                             0,
                                             globalDir, 
                                             &length, 
                                                NW_GDI_SplitMode_Clip);
        NW_THROW_ON(status, KBrsrOutOfMemory);
      }
      break;

    }

    if (status == KBrsrSuccess) 
      {
      /* Did I eat the whole thing? */
      if(length == NW_Text_GetCharCount(displayText)) 
        {
        *splitBox = NULL;
      }
      else
        {
        /* I only ate some of the text */
        *splitBox = NW_LMgr_AbstractTextBox_SplitAt (textBox, displayText, length);
        NW_THROW_OOM_ON_NULL(*splitBox, status);
      }
    }
    else 
      {
      *splitBox = NULL;
      NW_THROW_STATUS (status, KBrsrLmgrNoSplit);
    }
  }
  NW_CATCH(status) 
    { 
    /* empty */ 
    }
  NW_FINALLY 
    {
    NW_Object_Delete(displayText);
    return status;
    } 
  NW_END_TRY;
}

/* ------------------------------------------------------------------------- *
 * At this point we are not computing margins, padding and borders for text
 * boxes: i.e. text boxes are just containers for raw text. We may want to 
 * revisit this model, but for now we are assuming that these properties are
 * relevant not to individual text items, but to their containers. This makes
 * sense since XML text elements don't have attributes and the relevant
 * properties are not inheritable.
 */

TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetDimensions (NW_LMgr_AbstractTextBox_t* textBox, 
                                       NW_GDI_Dimension2D_t *size)
{
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  CGDIFont *font = NULL;
  NW_Text_t* displayText = NULL;
  NW_LMgr_Box_t* parentBox = NULL;
  NW_LMgr_PropertyValue_t dirValue;

  /* parameter assertion block */
  NW_ASSERT (textBox != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (textBox, &NW_LMgr_AbstractTextBox_Class));

  /* we're going to need the deviceContext to do text extent calculations */
  rootBox = (NW_LMgr_RootBox_t*) NW_LMgr_Box_GetRootBox ((NW_LMgr_Box_t*)textBox);
  NW_ASSERT (rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  NW_ASSERT (deviceContext != NULL);

  NW_TRY(status) 
    {
    /* In vertical layout mode we do not want to have extra space when visibility is hidden */
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
    {
      NW_LMgr_PropertyValue_t visibilityVal;
      visibilityVal.token = NW_CSS_PropValue_visible;
      if ((NW_LMgr_Box_GetPropertyValue ((NW_LMgr_Box_t*)textBox, NW_CSS_Prop_visibility,NW_CSS_ValueType_Token, &visibilityVal)== KBrsrSuccess ) && visibilityVal.token == NW_CSS_PropValue_hidden) 
      {
        size->width = 0 ;
        size->height = 0;
        NW_THROW_SUCCESS(status);
      }
    }

    /* Get the displayable text */
    displayText = NW_LMgr_AbstractTextBox_GetDisplayText(textBox, NW_TRUE);
    NW_THROW_OOM_ON_NULL(displayText, status);

    /* Get the font for the box */
    font = NW_LMgr_Box_GetFont ((NW_LMgr_Box_t*)textBox);
    NW_ASSERT(font != NULL);

    /* calculate the extents */
    /* NOTE:  DIRECTION-SENSITIVE CODE HERE */
   
   dirValue.token = NW_CSS_PropValue_ltr;
    status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(textBox), 
                                          NW_CSS_Prop_textDirection, 
                                          NW_CSS_ValueType_Token,
                                          &dirValue);
    NW_THROW_ON(status, KBrsrOutOfMemory);

   /* In vertical-layout mode, We want to remove the nbsp, if they are within a table cell 
       For e.g. <td> &nbsp; </td> DeviceContext will use the iRemoveNbsp flag while calculating the text
       extent and drawing the text
    */
    parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(textBox);
    if (NW_Object_IsInstanceOf(parentBox, &NW_LMgr_VerticalTableCellBox_Class))
    {
      deviceContext->SetRemoveNbsp (NW_TRUE);
    }

    status = deviceContext->GetTextExtent( displayText, font, 0, NW_GDI_FlowDirection_LeftRight, size);
    
    if (deviceContext->RemoveNbsp())
    {
      deviceContext->SetRemoveNbsp (NW_FALSE);
    }
    NW_THROW_ON_ERROR(status);
  }
  NW_CATCH(status) 
    {
  }
  NW_FINALLY 
    {
    NW_Object_Delete(displayText);

    return status;
  } NW_END_TRY;
}

TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetMinimumSizeByWord(
    NW_LMgr_AbstractTextBox_t* aTextBox, 
    NW_GDI_Dimension2D_t* aMinSize )
  {
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  CGDIFont* font = NULL;
  NW_Text_t* displayText = NULL;
  NW_Text_t* wordText = NULL;
  NW_GDI_Dimension2D_t extent; 
  NW_Ucs2* wordStart;
  NW_Ucs2* wordEnd;
  NW_Ucs2 savedChar;
  NW_Text_Length_t wordLen;

  // Parameter assertion block.
  NW_ASSERT( aTextBox != NULL );
  NW_ASSERT( NW_Object_IsInstanceOf( aTextBox, &NW_LMgr_AbstractTextBox_Class ) );
  NW_ASSERT( aMinSize != NULL );

  // Get deviceContext to do text extent calculations.
  rootBox = (NW_LMgr_RootBox_t*)NW_LMgr_Box_GetRootBox( (NW_LMgr_Box_t*)aTextBox );
  NW_ASSERT (rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  NW_ASSERT (deviceContext != NULL);

  // Get font for the box.
  font = NW_LMgr_Box_GetFont( (NW_LMgr_Box_t*)aTextBox );
  NW_ASSERT( font != NULL );

  NW_TRY( status )
      {
      // Get the displayable text.
      displayText = NW_LMgr_AbstractTextBox_GetDisplayText( aTextBox, NW_TRUE );
      NW_THROW_OOM_ON_NULL( displayText, status );

      // Create an object to hold the word.
      wordText = (NW_Text_t*)NW_Text_UCS2_New( NULL, 0, 0 );
      NW_THROW_OOM_ON_NULL( wordText, status );
	
      // Find the word that has the longest display extent. The longest
      // extent is the minimum size for ths box.
      extent.width = 0;
      aMinSize->width = NW_LMGR_BOX_MIN_WIDTH;
      wordStart = (NW_Ucs2 *)displayText->storage;


      //For Chinese string we take the length of one character as the minimum length
      EVariantFlag variant = AknLayoutUtils::Variant();
      if(variant == EApacVariant)
      {
        wordLen = 1;
        wordEnd = wordStart + 1;
        // Get the extent of the word.
        NW_Text_SetStorage( wordText, wordStart, wordLen, NW_Text_Flags_NullTerminated );
        status = deviceContext->GetTextExtent( wordText, font, 0, NW_GDI_FlowDirection_LeftRight, &extent );
        *aMinSize = extent;
      }
      else
      {
	  while ( *wordStart )
          {
          wordEnd = wordStart;
          wordLen = 0;
          // Find the end of the current word by looking for whitespace or
          // null. Note, this algorithm is simplistic, looking only for
          // whitespace. A more robust implementation would account for
          // hard/soft hyphens and language-specific rules such as the
          // word-wrapping complexities of Thai.
          while ( *wordEnd && !NW_Str_Isspace( *wordEnd ) )
              {
              wordLen++;
              wordEnd++;
              }
          // Temporarily insert a null at the end of the word.
          savedChar = *wordEnd;
          *wordEnd = 0;
          // Get the extent of the word.
          NW_Text_SetStorage( wordText, wordStart, wordLen, NW_Text_Flags_NullTerminated );
          status = deviceContext->GetTextExtent( wordText, font, 0, NW_GDI_FlowDirection_LeftRight, &extent );
          // Replace the temporary null with the saved character.
          *wordEnd = savedChar;
		      NW_THROW_ON_ERROR(status);
          // Track the largest word extent.
          if (extent.width > aMinSize->width)
            {
            *aMinSize = extent;
		        }
          // Skip over any whitespace.
          while ( NW_Str_Isspace( *wordEnd ) )
            {
            wordEnd++;
	          }
          wordStart = wordEnd;
        }  
	  }
    }
  NW_CATCH(status)
    {
    }
  NW_FINALLY
    {
    if ( displayText )
      {
      NW_Object_Delete(displayText);
	    }
    if ( wordText )
      {
        NW_Object_Delete( wordText );
	    }
    return status;
    }
  NW_END_TRY;
  }

/* ------------------------------------------------------------------------- */
NW_Text_t*
NW_LMgr_AbstractTextBox_GetDisplayText (NW_LMgr_AbstractTextBox_t* textBox,
                                        NW_Bool prepareForRendering) 
{
  NW_Text_t *text;
  NW_LMgr_Box_t *box = NW_LMgr_BoxOf(textBox);
  NW_LMgr_PropertyValueToken_t whitespace;
  NW_Ucs2 *storage = NULL;
  NW_Ucs2 *transStorage = NULL;
  NW_Ucs2 *substr = NULL;
  NW_Text_Length_t len;
  NW_Text_Length_t i = 0;
  NW_Text_Length_t j = 0;
  NW_Text_Length_t endIndex;
  NW_Bool freeNeeded;
  NW_Uint16 flags = 0;
  NW_LMgr_PropertyValue_t value;

  NW_TRY(status) {

    /* Get the whole text */
    text = NW_LMgr_AbstractTextBox_GetText(textBox);
    NW_THROW_OOM_ON_NULL(text, status);

    /* Get the "white-space" property */
    value.token = NW_CSS_PropValue_normal;
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_whitespace, NW_CSS_ValueType_Token, &value);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    whitespace = value.token;

    /* We ignore the prepareForRendering flag if white-space == pre */
    if (whitespace == NW_CSS_PropValue_pre) {
      prepareForRendering = NW_FALSE;
    }

    /* Get storage, using appropriate whitespace handling */
    flags = NW_Text_Flags_Aligned | NW_Text_Flags_Copy | NW_Text_Flags_NullTerminated;
    if (whitespace != NW_CSS_PropValue_pre) {
      flags |= NW_Text_Flags_CompressWhitespace;
      if (whitespace == NW_CSS_PropValue_flags_smallScreenPre) {
        flags |= NW_Text_Flags_RetainLineFeeds;
      }
    }
    storage = NW_Text_GetUCS2Buffer (text, flags, &len, &freeNeeded);
    NW_THROW_OOM_ON_NULL(storage, status);

    /* Apply the transform */
    status = NW_LMgr_AbstractTextBox_ApplyTransform (textBox, storage, &transStorage);
    NW_THROW_ON_ERROR(status);

    /* If we got a new string as a result of the transformation, delete it */
    if (storage != transStorage) {
      if (freeNeeded) {
        NW_Str_Delete(storage);
      }
    }

    /* If length is zero, we assume that it is uninitialized */
    if (NW_LMgr_AbstractTextBox_GetLength(textBox) == 0) {
      NW_LMgr_AbstractTextBox_SetLength(textBox, NW_Str_Strlen(transStorage));
    }

    substr = NW_Str_New(NW_LMgr_AbstractTextBox_GetLength(textBox) + 1);
    NW_THROW_OOM_ON_NULL(substr, status);

    i = NW_LMgr_AbstractTextBox_GetStart(textBox);
    endIndex = (NW_Text_Length_t)(i + NW_LMgr_AbstractTextBox_GetLength(textBox));

    if (prepareForRendering) {
      if (NW_LMgr_AbstractTextBox_IsCompressedLeading(textBox) && NW_Str_Isspace(transStorage[i])) {
        i++;
      }
    }

    for (j = 0; 
         i < endIndex; 
         i++, j++) {
      substr[j] = transStorage[i];
    }

    if (prepareForRendering) {
      if ((j > 0) && NW_LMgr_AbstractTextBox_IsCompressedTrailing(textBox) && NW_Str_Isspace(substr[j-1])) {
        j--;
      }
      if ((j > 0) && (substr[j-1] == NW_TEXT_UCS2_SOFT_HYPHEN)) {
        substr[j-1] = NW_TEXT_UCS2_HYPHEN;
      }
    }
 
    substr[j] = 0;
    NW_ASSERT(j <= NW_LMgr_AbstractTextBox_GetLength(textBox));
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {

    NW_Str_Delete(transStorage);
  
    if (substr != NULL) {
      /* Create the new text object */
      return (NW_Text_t*)
        NW_Text_UCS2_New(substr, j, NW_Text_Flags_TakeOwnership);
    }
    else {
      return NULL;
    }

  } NW_END_TRY;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_LMgr_AbstractTextBox_IsBlank (NW_LMgr_AbstractTextBox_t *textBox)
{
  NW_Text_t* text = NULL;
  NW_Text_Length_t textLen, i;
  NW_Bool isBlank = NW_TRUE;

  NW_TRY(status) {
    text = NW_LMgr_AbstractTextBox_GetDisplayText(textBox, NW_FALSE); 
    NW_THROW_OOM_ON_NULL(text, status);

    textLen = NW_Text_GetCharCount(text);

    for (i=0; i<textLen; i++) {
      if (!NW_Str_Isspace((NW_Ucs2)NW_Text_GetAt(text, i))) {
        isBlank = NW_FALSE;
        NW_THROW_SUCCESS(status);
      }
    }
  }
  NW_CATCH(status) {
		(void) status;
  }
  NW_FINALLY {
    NW_Object_Delete(text);
    return isBlank;
  } NW_END_TRY;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetWS (NW_LMgr_AbstractTextBox_t *textBox, 
                               NW_Bool *hasLeading,
                               NW_Bool *hasTrailing,
                               NW_Bool *endsInNewline) 
{
  NW_Text_t* text;
  NW_Text_Length_t textLen;

  *hasLeading = NW_FALSE;
  *hasTrailing = NW_FALSE;
  *endsInNewline = NW_FALSE;

  NW_TRY(status) {
    text = NW_LMgr_AbstractTextBox_GetDisplayText(textBox, NW_FALSE); 
    NW_THROW_OOM_ON_NULL(text, status);

    textLen = NW_Text_GetCharCount(text);
    if (textLen == 0) {
      NW_THROW_SUCCESS(status);
    }
    else if (textLen == 1) {
      if (NW_Str_Isspace((NW_Ucs2)NW_Text_GetAt(text, 0))) {
        *hasTrailing = NW_TRUE;
      }
      if ((NW_Ucs2)NW_Text_GetAt(text, 0) == NW_TEXT_UCS2_LF) {
        *endsInNewline = NW_TRUE;
      }
    }
    else {
      if (NW_Str_Isspace((NW_Ucs2)NW_Text_GetAt(text, 0))) {
        *hasLeading = NW_TRUE;
      }
      if (NW_Str_Isspace((NW_Ucs2)NW_Text_GetAt(text, 
                                                (NW_Text_Length_t)(textLen - 1)))) {
        *hasTrailing = NW_TRUE;
      }
      if ((NW_Ucs2)NW_Text_GetAt(text, (NW_Text_Length_t)(textLen - 1)) 
                      == NW_TEXT_UCS2_LF) {
        *endsInNewline = NW_TRUE;
      }
    }
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    NW_Object_Delete(text);
    return status;
  } NW_END_TRY;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_AbstractTextBox_GetDirection(NW_LMgr_AbstractTextBox_t* thisObj,
                                     NW_GDI_FlowDirection_t* dir)
{
  NW_Text_t *text = NULL;
  NW_LMgr_RootBox_t *rootBox;
  CGDIDeviceContext *deviceContext;
  NW_LMgr_Box_t* box = NW_LMgr_BoxOf(thisObj);
  NW_Text_Length_t textLen, i;
  NW_Bool isBlank = NW_TRUE;
 

  NW_TRY(status) 
    {
    /* Default response */
    *dir = NW_GDI_FlowDirection_Unknown;

    /* we're going to need the deviceContext to do text extent calculations */
    rootBox = (NW_LMgr_RootBox_t*)NW_LMgr_Box_GetRootBox (box);
    NW_ASSERT (rootBox != NULL);
    deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT (deviceContext != NULL);

    /* Get the display text */
    text = NW_LMgr_AbstractTextBox_GetDisplayText(thisObj, NW_FALSE); 
    NW_THROW_OOM_ON_NULL(text, status);

    /* Make sure the text is not all whitespace */
    textLen = NW_Text_GetCharCount(text);

    for (i=0; i<textLen; i++) 
      {
      if (!NW_Str_Isspace((NW_Ucs2)NW_Text_GetAt(text, i))) 
        {
        isBlank = NW_FALSE;
        break;
      }
    }

    if (!isBlank) 
      {
      /* Get the flow direction */
      status = deviceContext->GetFlowDirection(text, dir);
      NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status) 
    {
  }
  NW_FINALLY 
    {
    NW_Object_Delete(text);
    return status;
    }
  NW_END_TRY;
}
