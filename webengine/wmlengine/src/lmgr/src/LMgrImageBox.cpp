/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_imageboxi.h"

#include "nw_image_epoc32cannedimage.h"
#include "nw_image_virtualimage.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_settings.h"
#include "GDIDeviceContext.h"
#include "nw_lmgr_activecontainerbox.h"
#include "BrsrStatusCodes.h"

/* This method implements the image handling rules of vertical layout algorithm 
   1. scale the image maintaining the aspect ratio if width is greater than display width
   2. scale the image maintaining the aspect ratio if height is greater than display height
   3. If image is too small we do not want it to participate in the layout 

   Note: if box hasn't been added to the box tree yet, must pass in a valid rootbox.
   */
/* --------------------------------------------------------------------------*/
void
NW_LMgr_ImageBox_HandleVerticalLayout(NW_LMgr_Box_t *box, NW_GDI_Dimension2D_t *size, NW_LMgr_RootBox_t* rootBox)
{
  CGDIDeviceContext* deviceContext;
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
    if (NW_LMgr_Box_FindBoxOfClass (box, &NW_LMgr_ActiveContainerBox_Class, NW_FALSE) == NULL)
    {
      size->width = 0;
      size->height = 0;    
    }
  }
  else
  {
    if (!rootBox)
      rootBox = NW_LMgr_Box_GetRootBox(box);
    NW_ASSERT (rootBox);
    deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    rectangle = deviceContext->DisplayBounds();
    NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );
    maxWidth = (NW_GDI_Metric_t)(rectangle->dimension.width - padding.left - padding.right - borderWidth.right - borderWidth.left - 6);
    maxHeight = (NW_GDI_Metric_t)(rectangle->dimension.width - padding.top - padding.bottom - borderWidth.top - borderWidth.bottom);

    /* scale the image maintaining the aspect ratio if width is greater than display width */
    if (size->width > maxWidth)
    {
      NW_LMgr_Box_t* parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(box);
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( parentBox );
      if((parentBox != NULL) && (boxBounds.dimension.width != 0))
      {
        maxWidth = boxBounds.dimension.width;
      }

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

/* ------------------------------------------------------------------------- */
/* The static instance of the Image_Box class object */


const
NW_LMgr_ImageBox_Class_t  NW_LMgr_ImageBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_MediaBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_ImageBox_t),
    /* construct                 */ _NW_LMgr_ImageBox_Construct,
    /* destruct                  */ _NW_LMgr_ImageBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_ImageBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_ImageBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_ImageBox          */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_ImageBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  NW_LMgr_ImageBox_t* imageBox;
  TBrowserStatusCode status;

  /* for convenience */
  imageBox = NW_LMgr_ImageBoxOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_LMgr_Box_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize our member */
  imageBox->image = va_arg (*argp, NW_Image_AbstractImage_t*);
  if (imageBox->image != NULL) {
    NW_GDI_Rectangle_t bounds;
    NW_GDI_Dimension3D_t imageSize;

    bounds.point.x = 0;
    bounds.point.y = 0;
    status = NW_Image_AbstractImage_GetSize (imageBox->image, &imageSize);
    NW_ASSERT (status == KBrsrSuccess);
    bounds.dimension.width = imageSize.width;
    bounds.dimension.height = imageSize.height;
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( imageBox ), bounds);
  }
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_ImageBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_ImageBox_t* imageBox;

  /* for convenience */
  imageBox = NW_LMgr_ImageBoxOf (dynamicObject);

  /* deallocate the image */
  if (imageBox->image != NULL &&
      NW_Object_Core_GetClass(imageBox->image) != &NW_Image_Epoc32Canned_Class) {

    NW_Object_Delete (imageBox->image);
  }
}

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_LMgr_ImageBox_GetMinimumContentSize(NW_LMgr_Box_t *box,
                                        NW_GDI_Dimension2D_t *size)
  {
  NW_LMgr_ImageBox_t* imageBox;
  NW_LMgr_RootBox_t* rootBox;
  NW_GDI_Dimension3D_t imageSize;
  TBrowserStatusCode status;
  NW_LMgr_PropertyValue_t sizeVal;
  
  /* For convenience */
  imageBox = NW_LMgr_ImageBoxOf (box);

  NW_ASSERT(size != NULL);

  /* Get image size */

  status = NW_Image_AbstractImage_GetSize (imageBox->image, &imageSize);
  if(status != KBrsrSuccess){
    return status;
  }

  /* If the box has its Width/Height properties set, use that for sizing;
   * otherwise return image size.
   */
  size->height = size->width = 0;

  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal);
  if (status == KBrsrSuccess){
    size->height = (NW_GDI_Metric_t)sizeVal.integer;
  }
  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
  if (status == KBrsrSuccess){
    size->width = (NW_GDI_Metric_t)sizeVal.integer;
  }
  if ((size->height !=0 ) && (size->width == 0) && (imageSize.height != 0)){
    size->width = (NW_GDI_Metric_t)((size->height*imageSize.width)/imageSize.height);
  }
  if ((size->width != 0) && (size->height == 0) && (imageSize.width != 0)){
    size->height = (NW_GDI_Metric_t)((size->width*imageSize.height)/imageSize.width);
  }
  if (size->width == 0){
    size->width = imageSize.width;
  }
  if (size->height == 0){
    size->height = imageSize.height;
  }

  /* In case of vertical layout is enabled implement the image handling algorithm */
  rootBox = NW_LMgr_Box_GetRootBox( box );

  if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
  {
    NW_LMgr_ImageBox_HandleVerticalLayout(box, size, NULL);    
  }

  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
_NW_LMgr_ImageBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus)
{
  NW_LMgr_ImageBox_t* imageBox;
  NW_GDI_Dimension3D_t dimension;
  NW_GDI_Rectangle_t innerRect;
  NW_LMgr_FrameInfo_t borderWidth;
  NW_LMgr_FrameInfo_t borderStyle;
  NW_LMgr_FrameInfo_t borderColor;
  NW_LMgr_PropertyValue_t focusBehavior;

  /* for convenience */
  imageBox = NW_LMgr_ImageBoxOf (box);

  NW_TRY (status) {

    /* height or width == 0 implies that there is no need to draw the image
       as the box does not occupy any space on display */
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
    if (( boxBounds.dimension.width == 0) || ( boxBounds.dimension.height == 0))
      NW_THROW_SUCCESS(status);

    /* invoke our superclass draw method first */
    status = _NW_LMgr_Box_Draw (box, deviceContext, hasFocus);
    _NW_THROW_ON_ERROR (status);
 
    /* Get inner rectangle */
    NW_LMgr_Box_GetInnerRectangle(box, &innerRect);

    /* make our image draw itself at the specified location */
    deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);

    dimension.width = innerRect.dimension.width;
    dimension.height = innerRect.dimension.height;
    dimension.depth = 0; 

    status = NW_Image_AbstractImage_DrawScaled (imageBox->image, 
                                                deviceContext,
                                                &innerRect.point, 
                                                &dimension);

    NW_LMgr_Box_GetBorderWidth (box, &borderWidth, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderColor (box, &borderColor);
    NW_LMgr_Box_GetBorderStyle (box, &borderStyle, ELMgrFrameAll );
 
    /* Modify the look of the box if we're in focus */
    if (hasFocus){
      focusBehavior.token = NW_CSS_PropValue_none;
      (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_focusBehavior,
                                           NW_CSS_ValueType_Token,
                                           &focusBehavior);

      if (focusBehavior.token == NW_CSS_PropValue_growBorder) {
        borderWidth.top = (NW_GDI_Metric_t)(borderWidth.top + 1);
        borderWidth.bottom = (NW_GDI_Metric_t)(borderWidth.bottom + 1);
        borderWidth.left = (NW_GDI_Metric_t)(borderWidth.left + 1);
        borderWidth.right = (NW_GDI_Metric_t)(borderWidth.right + 1);
        if ((borderStyle.top == NW_CSS_PropValue_none) || (borderStyle.top == NW_CSS_PropValue_hidden)) {
          borderStyle.top = NW_CSS_PropValue_solid;
        }
        if ((borderStyle.bottom == NW_CSS_PropValue_none) || (borderStyle.bottom == NW_CSS_PropValue_hidden)) {
          borderStyle.bottom = NW_CSS_PropValue_solid;
        }
        if ((borderStyle.left == NW_CSS_PropValue_none) || (borderStyle.left == NW_CSS_PropValue_hidden)) {
          borderStyle.left = NW_CSS_PropValue_solid;
        }
        if ((borderStyle.right == NW_CSS_PropValue_none) || (borderStyle.right == NW_CSS_PropValue_hidden)) {
          borderStyle.right = NW_CSS_PropValue_solid;
        }
      }
    }

    /* draw the border */
    /* TODO add support for other border types */
    status = NW_LMgr_Box_DrawBorder ( NW_LMgr_Box_GetDisplayBounds(box), deviceContext, &borderWidth, &borderStyle, &borderColor);
    _NW_THROW_ON_ERROR(status);
  
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_ImageBox_t*
NW_LMgr_ImageBox_New (NW_ADT_Vector_Metric_t numProperties,
                      NW_Image_AbstractImage_t* image)
{
  return (NW_LMgr_ImageBox_t*)
    NW_Object_New (&NW_LMgr_ImageBox_Class, numProperties, image);
}



