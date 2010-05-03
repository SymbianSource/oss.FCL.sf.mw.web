/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_fbox_epoc32buttonboxskini.h"

#include "nw_lmgr_rootbox.h"
#include "nw_fbox_buttonbox.h"
#include "nw_fbox_epoc32formimages.h"
#include "GDIDevicecontext.h"
#include "nw_fbox_formboxutils.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* Get the dir attribute */
static
void
NW_FBox_Epoc32ButtonBoxSkin_GetDirection(NW_LMgr_Box_t *box,
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

static
TBrowserStatusCode
NW_FBox_Epoc32ButtonBoxSkin_DisplayText(NW_LMgr_Box_t* box,
                                        CGDIDeviceContext* deviceContext,
                                        NW_GDI_Point2D_t textStart,
                                        NW_Text_t* value,
                                        NW_Uint16 width)
  {
  NW_Text_Length_t  displayLength = 0;
  NW_Text_UCS2_t*		displayText;
  NW_Ucs2*					displayStorage;
  NW_Ucs2*					bufferStorage;
  NW_Text_Length_t  totalLength = 0;
  TBrowserStatusCode       status;
  NW_GDI_FlowDirection_t dir = NW_GDI_FlowDirection_LeftRight;
  NW_GDI_Dimension2D_t extent;
  CGDIFont* font;

  bufferStorage = NW_Text_GetUCS2Buffer(value, 0, &totalLength, NULL);
  
  font = NW_LMgr_Box_GetFont(box);

  if (totalLength == 0)
    {
    return KBrsrSuccess;
    }
  
  if(width >= 255)
    {
    width = 254;
    }
  
  status = NW_FBox_FormBoxUtils_GetDisplayLength(deviceContext,
                                                 value,
                                                 font,
                                                 totalLength,
                                                 width,
                                                 &displayLength,
                                                 NW_FALSE
                                                 );
  NW_ASSERT (status == KBrsrSuccess);
  
  if(displayLength != value->characterCount)
    {
    displayStorage = NW_Str_New(displayLength + NW_FBox_FormBoxUtils_Truncate_len);
    if (displayStorage == NULL)
      {      
      return KBrsrOutOfMemory;
      }

    displayText = NW_Text_UCS2_New(NULL, 0, NW_Text_Flags_TakeOwnership);
    if (displayText == NULL)
      {       
      return KBrsrOutOfMemory;
      }

    NW_Mem_memcpy(displayStorage, 
                 (bufferStorage),
                 displayLength*sizeof(NW_Ucs2)
                 );
    
    NW_Mem_memcpy(displayStorage + displayLength, 
                  NW_FBox_FormBoxUtils_Truncate, 
                  NW_FBox_FormBoxUtils_Truncate_len*sizeof(NW_Ucs2));

    displayLength += NW_FBox_FormBoxUtils_Truncate_len;
    status = NW_Text_SetStorage(NW_TextOf(displayText), displayStorage, 
                       displayLength, NW_TRUE);
    NW_ASSERT (status == KBrsrSuccess);

    NW_FBox_Epoc32ButtonBoxSkin_GetDirection(box, &dir);
    status = deviceContext->GetTextExtent( NW_TextOf(displayText),
                                           font,
                                           0, 
                                           dir, 
                                           &extent);

    textStart.x += ((width - extent.width) / 2);
    status = deviceContext->DrawText( &textStart, 
                                      NW_TextOf(displayText), 
                                      font, 
                                      0, 
                                      NW_GDI_TextDecoration_None, 
                                      dir, 
                                      extent.width );
    
    NW_Object_Delete(displayText);
    }
  else
    {
    NW_FBox_Epoc32ButtonBoxSkin_GetDirection(box, &dir);
    status = deviceContext->DrawText( &textStart, value, font, 0, NW_GDI_TextDecoration_None, dir, width);
    }
  return status;
  }

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32ButtonBoxSkin_Class_t  NW_FBox_Epoc32ButtonBoxSkin_Class = {
  { /* NW_Object_Core           */
    /* super                    */ &NW_FBox_ButtonBoxSkin_Class,
    /* queryInterface           */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base           */
    /* interfaceList            */ NULL
  },
  { /* NW_Object_Dynamic        */
    /* instanceSize             */ sizeof (NW_FBox_Epoc32ButtonBoxSkin_t),
    /* construct                */ _NW_FBox_Skin_Construct,
    /* destruct                 */ NULL
  },
  { /* NW_FBox_Skin             */
    /* draw                     */ _NW_FBox_ButtonBoxSkin_Draw,
    /* getSize                  */ _NW_FBox_ButtonBoxSkin_GetSize
  },
  { /* NW_FBox_ButtonBoxSkin    */
    /* drawContent              */ _NW_FBox_Epoc32ButtonBoxSkin_DrawContent
  },
  { /* NW_FBox_Epoc32ButtonBoxSkin */
    /* unused                   */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_Epoc32ButtonBoxSkin_DrawContent(NW_FBox_ButtonBoxSkin_t* skin,
                                   NW_LMgr_Box_t* box,
                                   CGDIDeviceContext* deviceContext,
                                   NW_Uint8 hasFocus)
  { 
  NW_FBox_ButtonBox_t* ButtonBox;
  NW_GDI_Point2D_t textStart;
  NW_GDI_Rectangle_t innerRect;
  NW_Text_t* value;
  NW_Uint16 width;
  NW_GDI_Point2D_t shadow[3];
  NW_GDI_Color_t initialColor = 0;
  NW_GDI_Color_t initialBgColor = 0;
  NW_GDI_FlowDirection_t dir;

  NW_TRY(status)
    {
    NW_REQUIRED_PARAM(skin);
    NW_REQUIRED_PARAM(hasFocus);

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (skin, &NW_FBox_Epoc32ButtonBoxSkin_Class));

    /* for convenience */
    ButtonBox = NW_FBox_ButtonBoxOf (box);

    /* this is the label */
    value = NW_FBox_ButtonBox_GetValue(ButtonBox);

    /* Calculate the inner rectangle */
    NW_LMgr_Box_GetInnerRectangle(box, &innerRect);
 
    /* Get the start coordinates for text*/
    textStart = innerRect.point;
	  width = (NW_Uint16)innerRect.dimension.width;
    
    /* now centralize the text */
    NW_LMgr_PropertyValue_t sizeVal;
    NW_Uint16 box_height = 0;
    NW_Uint16 box_width = 0;
    sizeVal.integer = 0;
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
    if (KBrsrSuccess == status)
      {
      box_width = (NW_Uint16)sizeVal.integer;
      }
    
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal);
    if (KBrsrSuccess == status)
      {
      box_height = (NW_Uint16)sizeVal.integer;
      }

    /* calculate the extents */
    NW_GDI_Dimension2D_t textExtent;

    NW_FBox_Epoc32ButtonBoxSkin_GetDirection(box, &dir);
    status = deviceContext->GetTextExtent ( value,
                                            NW_LMgr_Box_GetFont(box),
                                            0, 
                                            dir, 
                                            &textExtent);
    
    /* if no truncation needed, centralize text with text length */
    if (textExtent.width <= box_width)
      {
      textStart.x += box_width/2 - 1;
      textStart.x -= textExtent.width/2;
      }

    if (box_height != 0)
      {
      textStart.y += box_height/2;
      textStart.y -= textExtent.height/2;
      }
    
    /* draw shadow to create 3D effect */
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
    shadow[0].x = (NW_GDI_Metric_t)(boxBounds.point.x + 1);
    shadow[0].y = (NW_GDI_Metric_t)(boxBounds.point.y + boxBounds.dimension.height);
    shadow[1].x = (NW_GDI_Metric_t)(boxBounds.point.x + boxBounds.dimension.width);
    shadow[1].y = shadow[0].y;
    shadow[2].x = shadow[1].x;
    shadow[2].y = (NW_GDI_Metric_t)(boxBounds.point.y + 1);

    initialColor = deviceContext->ForegroundColor();
    initialBgColor = deviceContext->BackgroundColor();
                                                
    deviceContext->SetForegroundColor( NW_GDI_Color_Black); 
    status = deviceContext->DrawPolyline ( 3, shadow, NW_FALSE);
    NW_THROW_ON_ERROR(status);
    
    NW_LMgr_Property_t backgroundProp;
    backgroundProp.type = NW_CSS_ValueType_Token;
    backgroundProp.value.token = NW_CSS_PropValue_transparent;
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_backgroundColor, &backgroundProp);
    
    NW_GDI_Color_t foregroundColor = 0;
    NW_LMgr_Property_t fgColor;
    fgColor.type = NW_CSS_ValueType_Color;
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_color, &fgColor);
    if (KBrsrSuccess == status)
      {
      foregroundColor = fgColor.value.integer;
      }
    else
      {
      foregroundColor = NW_GDI_Color_Black;
      }
    deviceContext->SetForegroundColor( foregroundColor);
    
    /* Make button label draw itself at the specified location */
    deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);

    if ( value != NULL)
      {
      status = NW_FBox_Epoc32ButtonBoxSkin_DisplayText(box, 
        deviceContext, 
        textStart, 
        value, 
        width); 

      NW_THROW_ON_ERROR(status);
      }

    if (hasFocus)
      {
        NW_LMgr_FrameInfo_t borderWidth;
        NW_LMgr_FrameInfo_t borderStyle;
        NW_LMgr_FrameInfo_t borderColor;

        NW_LMgr_Box_GetBorderColor (box, &borderColor);
        NW_LMgr_Box_GetBorderStyle (box, &borderStyle, ELMgrFrameAll );
        NW_LMgr_Box_GetBorderWidth (box, &borderWidth, ELMgrFrameAll );

        borderWidth.top = (NW_GDI_Metric_t)(borderWidth.top + 1);
        borderWidth.bottom = (NW_GDI_Metric_t)(borderWidth.bottom + 1);
        borderWidth.left = (NW_GDI_Metric_t)(borderWidth.left + 1);
        borderWidth.right = (NW_GDI_Metric_t)(borderWidth.right + 1);

        status = NW_LMgr_Box_DrawBorder ( NW_LMgr_Box_GetDisplayBounds( box ), deviceContext, &borderWidth, &borderStyle, &borderColor);
        _NW_THROW_ON_ERROR(status);
      }

    deviceContext->SetForegroundColor( initialColor);
    deviceContext->SetBackgroundColor( initialBgColor);

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

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_ButtonBoxSkin_t*
NW_FBox_ButtonBoxSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_ButtonBoxSkin_t*)
    NW_Object_New (&NW_FBox_Epoc32ButtonBoxSkin_Class, formBox);
}
