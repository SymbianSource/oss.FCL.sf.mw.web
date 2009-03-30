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


#include "nw_fbox_imagebuttonboxskini.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_image_epoc32cannedimage.h"
#include "nwx_settings.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

const
NW_FBox_ImageButtonBoxSkin_Class_t  NW_FBox_ImageButtonBoxSkin_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_Skin_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_ImageButtonBoxSkin_t),
    /* construct                   */ _NW_FBox_Skin_Construct,
    /* destruct                    */ NULL
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ _NW_FBox_ImageButtonBoxSkin_Draw,
    /* NW_FBox_Skin_GetSize_t      */ _NW_FBox_ImageButtonBoxSkin_GetSize
  },
  { /* NW_FBox_ButtonBoxSkin */
    /* drawContent                 */ NULL
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_ImageButtonBoxSkin_Draw(NW_FBox_Skin_t* skin,
                        NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus)
{
  NW_FBox_ImageButtonBoxSkin_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_ImageButtonBoxSkinOf(skin);

  /* Let the base class draw the box */
  (void) NW_LMgr_Box_Class.NW_LMgr_Box.draw(box, deviceContext, hasFocus);

  return NW_FBox_ImageButtonBoxSkin_DrawContent(thisObj, box, deviceContext, hasFocus);
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_ImageButtonBoxSkin_GetSize(NW_FBox_Skin_t* skin,
                                     NW_LMgr_Box_t* box,
                                     NW_GDI_Dimension3D_t* size)
{
  NW_FBox_ImageButtonBox_t* imageButtonBox;
  NW_Text_t* value;
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  CGDIFont* font;
  TBrowserStatusCode status;
  NW_GDI_Dimension2D_t textExtent;
  NW_GDI_Dimension3D_t imgSize;
  NW_GDI_Dimension3D_t textSize;
  NW_LMgr_Property_t heightVal;
  NW_LMgr_Property_t widthVal;
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_ImageButtonBox_Class));

  /* for convenience */
  imageButtonBox = NW_FBox_ImageButtonBoxOf (box);
  (void)skin;

  /* Get the value */
  value = NW_FBox_ImageButtonBox_GetValue(imageButtonBox);

  /* we're going to need the deviceContext to do text extent calculations */
  rootBox = NW_LMgr_Box_GetRootBox (box);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);

  /* Get the font */
  font = NW_LMgr_Box_GetFont (box);
  NW_ASSERT(font != NULL);

  /* calculate the extents */
  status = deviceContext->GetTextExtent ( value,
                                          font,
                                          0,
                                          NW_GDI_FlowDirection_LeftRight,
                                          &textExtent);
  if (status == KBrsrOutOfMemory) 
    {
    return status;
  }

  if ((value != NULL) && 
      !(NW_Object_IsInstanceOf(imageButtonBox->image, &NW_Image_Epoc32Canned_Class)))
    {
    textExtent.height = 0;
    textExtent.width = 0;
  }

  textSize.height = (NW_GDI_Metric_t)(textExtent.height);
  textSize.width =  (NW_GDI_Metric_t)(textExtent.width);

  if(imageButtonBox->image)
  {
    (void) NW_Image_AbstractImage_GetSize(imageButtonBox->image, &imgSize);
    size->depth = imgSize.depth;
    size->width = imgSize.width;
    size->height = imgSize.height;
    
    if (textSize.width != 0) 
    {
      size->width = (NW_GDI_Metric_t)(size->width + textSize.width); 
      if(imgSize.height > textSize.height) 
        {
        size->height = imgSize.height;
      } 
      else 
        {
        size->height = textSize.height;
      }
    }

	/* If the image button has height and width properties, use those instead */
    heightVal.type = widthVal.type = 0;
    heightVal.value.decimal = widthVal.value.decimal = 0;

	status = NW_LMgr_PropertyList_Get(box->propList, NW_CSS_Prop_width, &widthVal);
    if (status == KBrsrSuccess)
	{
      size->width = widthVal.value.integer;
	}

    status = NW_LMgr_PropertyList_Get(box->propList, NW_CSS_Prop_height, &heightVal);
    if (status == KBrsrSuccess)
	{
      size->height = heightVal.value.integer;
	}

    /* In vertical layout mode, we do not want the size of the image to be bigger than the display,
       so we scale it */
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
    {
      const NW_GDI_Rectangle_t* rectangle;
      NW_GDI_Metric_t maxWidth = 0;
      NW_GDI_Metric_t maxHeight = 0;
      NW_LMgr_FrameInfo_t borderWidth;

      rectangle = deviceContext->DisplayBounds();
      NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );

      maxWidth = (NW_GDI_Metric_t)(rectangle->dimension.width - padding.left - padding.right - borderWidth.right - borderWidth.left - 6);
      maxHeight = (NW_GDI_Metric_t)(rectangle->dimension.width - padding.top - padding.bottom - borderWidth.top - borderWidth.bottom);

	  if (size->width > maxWidth)
	  {
        size->height = (NW_GDI_Metric_t)((maxWidth*size->height)/size->width);
        size->width = maxWidth;
	  }

	  if (size->height > maxHeight)
	  {
        size->width = (NW_GDI_Metric_t)((maxHeight*size->width)/size->height);
        size->height = maxHeight;
	  }
    }
  }
  else 
    {
    size->height = textSize.height;
    size->width = textSize.width;
  }
  return KBrsrSuccess;
}

