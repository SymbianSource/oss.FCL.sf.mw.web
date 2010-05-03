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


#include "nw_fbox_epoc32imagebuttonboxskini.h"

#include "nw_fbox_imagebuttonbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_fbox_epoc32formimages.h"
#include "nw_fbox_formboxutils.h"
#include "nw_image_epoc32cannedimage.h"
#include "GDIDevicecontext.h"
#include "nw_lmgr_imgcontainerboxi.h"
#include "nw_image_cannedimages.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32ImageButtonBoxSkin_Class_t  NW_FBox_Epoc32ImageButtonBoxSkin_Class = {
  { /* NW_Object_Core           */
    /* super                    */ &NW_FBox_ImageButtonBoxSkin_Class,
    /* queryInterface           */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base           */
    /* interfaceList            */ NULL
  },
  { /* NW_Object_Dynamic        */
    /* instanceSize             */ sizeof (NW_FBox_Epoc32ImageButtonBoxSkin_t),
    /* construct                */ _NW_FBox_Skin_Construct,
    /* destruct                 */ NULL
  },
  { /* NW_FBox_Skin             */
    /* draw                     */ _NW_FBox_ImageButtonBoxSkin_Draw,
    /* getSize                  */ _NW_FBox_ImageButtonBoxSkin_GetSize
  },
  { /* NW_FBox_ButtonBoxSkin    */
    /* drawContent              */ _NW_FBox_Epoc32ImageButtonBoxSkin_DrawContent
  },
  { /* NW_FBox_Epoc32ButtonBoxSkin */
    /* unused                   */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_Epoc32ImageButtonBoxSkin_DrawContent(NW_FBox_ImageButtonBoxSkin_t* skin,
                                   NW_LMgr_Box_t* box,
                                   CGDIDeviceContext* deviceContext,
                                   NW_Uint8 hasFocus)
{ 
  NW_TRY(status)
  {
    NW_FBox_ImageButtonBox_t* imageButtonBox;
    NW_GDI_Point2D_t imageStart;
    NW_GDI_Rectangle_t innerRect;
    NW_Text_t* value;
    NW_GDI_Dimension3D_t dimension;

    NW_REQUIRED_PARAM(skin);
    NW_REQUIRED_PARAM(hasFocus);

    /* for convenience */
    imageButtonBox = NW_FBox_ImageButtonBoxOf (box);

    /* this is the label */
    value = NW_FBox_ImageButtonBox_GetValue(imageButtonBox);

    /* Calculate the inner rectangle */
    NW_LMgr_Box_GetInnerRectangle(box, &innerRect);
 
    /* Get the start coordinates */
    imageStart = innerRect.point;
  
    dimension.width = innerRect.dimension.width;
    dimension.height = innerRect.dimension.height;
    dimension.depth = 0; 

    /* Draw the valid image, if there is one */  
    if ((imageButtonBox->image)  && 
        !NW_Object_IsInstanceOf(imageButtonBox->image, &NW_Image_Epoc32Canned_Class))
    {
      /* we want to be able to scale image buttons if necessary, so we invoke DrawScaled method */
      status = NW_Image_AbstractImage_DrawScaled (imageButtonBox->image, deviceContext, &imageStart, &dimension);
      NW_THROW(status);
    }
  
    /* now we know that we do not have a valid image */
    /* Make button label draw itself at the specified location */
    NW_Image_AbstractImage_t* image = imageButtonBox->image;
    if( image == NULL)
      {
      // get canned image if there is no image available
      NW_LMgr_RootBox_t *rootBox = NULL;
      
      rootBox = NW_LMgr_Box_GetRootBox( box );
      NW_THROW_OOM_ON_NULL( rootBox, status );

      image = (NW_Image_AbstractImage_t*)
          NW_Image_CannedImages_GetImage( rootBox->cannedImages, NW_Image_Missing );
      NW_THROW_OOM_ON_NULL( image, status );      
      }
    status = NW_LMgr_ImgBox_DrawContent( box, deviceContext, image, NW_TRUE, value, hasFocus );
    NW_THROW_ON_ERROR(status);

  } NW_CATCH (status) {
  } NW_FINALLY {

    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_ImageButtonBoxSkin_t*
NW_FBox_ImageButtonBoxSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_ImageButtonBoxSkin_t*)
    NW_Object_New (&NW_FBox_Epoc32ImageButtonBoxSkin_Class, formBox);
}
