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


#include "nw_fbox_inlinecheckboxskini.h"

#include "nw_fbox_checkbox.h"
#include "nw_fbox_epoc32formimages.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
const
NW_FBox_InlineCheckBoxSkin_Class_t  NW_FBox_InlineCheckBoxSkin_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_CheckBoxSkin_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_InlineCheckBoxSkin_t),
    /* construct                   */ _NW_FBox_Skin_Construct,
    /* destruct                    */ NULL
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ _NW_FBox_InlineCheckBoxSkin_Draw,
    /* NW_FBox_Skin_GetSize_t      */ _NW_FBox_InlineCheckBoxSkin_GetSize
  },
  { /* NW_FBox_CheckBoxSkin */
    /* unused                      */ NW_Object_Unused
  },
  { /* NW_FBox_InlineCheckBoxSkin */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InlineCheckBoxSkin_Draw(NW_FBox_Skin_t* skin,
                                 NW_LMgr_Box_t* box,
                                 CGDIDeviceContext* deviceContext,
                                 NW_Uint8 hasFocus)
{
  NW_FBox_CheckBox_t* checkBox;
  NW_Image_AbstractImage_t* image = 0;
  NW_FBox_FormImages_t* formImages;
  NW_GDI_Rectangle_t innerRect;
  NW_LMgr_Property_t backgroundProp;
  NW_GDI_Color_t backgroundColor = 0x00;
  NW_LMgr_Box_t* parentBox = NULL;
  NW_GDI_Color_t initialBgColor = 0;

  NW_TRY(status)
  {
    /* for convenience */
    checkBox = NW_FBox_CheckBoxOf (box);
    (void)skin;

    /* Let the base class draw draw the box */
    status = NW_LMgr_Box_Class.NW_LMgr_Box.draw(box, deviceContext, hasFocus);
    NW_THROW_ON_ERROR(status);

    /* Calculate the inner rectangle */
    NW_LMgr_Box_GetInnerRectangle(box, &innerRect);
    formImages = (NW_FBox_FormImages_t*)&NW_FBox_FormImages;
    image = (NW_Image_AbstractImage_t*) 
            NW_FBox_FormImages_GetImage(formImages, hasFocus, 
                                        NW_FBox_CheckBox_GetState(checkBox), 
                                        NW_FBox_FormBoxOf(checkBox));
    NW_THROW_OOM_ON_NULL(image, status);
    

    /* get background color info */
    initialBgColor = deviceContext->BackgroundColor();
    backgroundProp.type = NW_CSS_ValueType_Token;
    backgroundProp.value.token = NW_CSS_PropValue_transparent;
    parentBox = NW_LMgr_BoxOf(box->parent);
    while(parentBox != NULL)
    {
      status = NW_LMgr_Box_GetProperty(parentBox, NW_CSS_Prop_backgroundColor, &backgroundProp);
      if (status == KBrsrSuccess){
        break;
      }
      parentBox = NW_LMgr_BoxOf(parentBox->parent);
    }

    if (backgroundProp.type == NW_CSS_ValueType_Color) {
      backgroundColor = backgroundProp.value.integer;
      deviceContext->SetBackgroundColor( backgroundColor);
    }

    /* make our image draw itself at the specified location */
    deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);
    status = NW_Image_AbstractImage_Draw (image, deviceContext, &(innerRect.point));
    NW_THROW_ON_ERROR(status);

    /* draw the focused border */
    if (hasFocus)
    {
      NW_LMgr_FrameInfo_t borderWidth;
      NW_LMgr_FrameInfo_t borderStyle;
      NW_LMgr_FrameInfo_t borderColor;

      NW_LMgr_Box_GetBorderWidth (box, &borderWidth, ELMgrFrameAll );
      NW_LMgr_Box_GetBorderColor (box, &borderColor);
      NW_LMgr_Box_GetBorderStyle (box, &borderStyle, ELMgrFrameAll );

      status = NW_LMgr_Box_DrawBorder( NW_LMgr_Box_GetDisplayBounds(box), deviceContext, &borderWidth, &borderStyle, &borderColor);
      _NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status)
  {
  }
  NW_FINALLY{
    deviceContext->SetBackgroundColor( initialBgColor);
    NW_Object_Delete ((NW_Image_AbstractImage_t*)image);
    return status;
  }NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InlineCheckBoxSkin_GetSize(NW_FBox_Skin_t* skin,
                                     NW_LMgr_Box_t* box,
                                     NW_GDI_Dimension3D_t* size)
{
  NW_FBox_CheckBox_t* checkBox;
  const NW_Image_AbstractImage_t* image;
  NW_FBox_FormImages_t* formImages;
  NW_GDI_Dimension3D_t imageSize;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_CheckBox_Class));

  /* for convenience */
  checkBox = NW_FBox_CheckBoxOf (box);
  (void)skin;
  formImages = (NW_FBox_FormImages_t*)&NW_FBox_FormImages;

  image = (NW_Image_AbstractImage_t*)NW_FBox_FormImages_GetImage(formImages, NW_FALSE, 
                                      NW_FBox_CheckBox_GetState(checkBox), 
                                      NW_FBox_FormBoxOf(checkBox));
  if (image == NULL)
  {  
    return KBrsrOutOfMemory;
  }
  /* Get the size of the image itself */
  (void) NW_Image_AbstractImage_GetSize (image, &imageSize);

  size->height = imageSize.height;
  size->width = imageSize.width;

  NW_Object_Delete ((NW_Image_AbstractImage_t*)image);

  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_CheckBoxSkin_t*
NW_FBox_CheckBoxSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_CheckBoxSkin_t*)
    NW_Object_New (&NW_FBox_InlineCheckBoxSkin_Class, formBox);
}
