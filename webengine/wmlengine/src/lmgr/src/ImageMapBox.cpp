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
* Description:  Box class associated with the handling image maps
*
*/


// INCLUDE FILES
#include <e32def.h>  // To avoid NULL redefine warning (no #ifndef NULL)

#include "nw_lmgr_imagemapboxi.h"
#include "nw_image_cannedimages.h"
#include "nw_image_epoc32cannedimage.h"
#include "nw_image_virtualimage.h"
#include "nw_lmgr_rootbox.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_areabox.h"
#include "nwx_settings.h"
#include "GDIDevicecontext.h"
#include "nw_evt_controlkeyevent.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_scrollevent.h"
#include "nw_evt_activateevent.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_lmgr_animatedimageboxi.h"
#include "nw_lmgr_simplepropertylist.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NW_LMgr_ImageMapBox_InitializeBorder
// Method which initializes the border width, color, and style to set values.
//   This method is used by the DrawBorder method.
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
void
NW_LMgr_ImageMapBox_InitializeBorder(CGDIDeviceContext* aDeviceContext,
                                     NW_LMgr_FrameInfo_t& aBorderWidth,
                                     NW_LMgr_FrameInfo_t& aBorderColor,
                                     NW_LMgr_FrameInfo_t& aBorderStyle,
                                     NW_GDI_Color_t& aInitialBgColor,
                                     NW_GDI_Color_t& aColor)
{
  aBorderWidth.top    = (NW_GDI_Metric_t)NW_LMgr_Box_MediumBorderWidth;
  aBorderWidth.bottom = (NW_GDI_Metric_t)NW_LMgr_Box_MediumBorderWidth;
  aBorderWidth.left   = (NW_GDI_Metric_t)NW_LMgr_Box_MediumBorderWidth;
  aBorderWidth.right  = (NW_GDI_Metric_t)NW_LMgr_Box_MediumBorderWidth;

  aBorderColor.bottom = NW_GDI_Color_Blue;
  aBorderColor.left = NW_GDI_Color_Blue;
  aBorderColor.right = NW_GDI_Color_Blue;
  aBorderColor.top = NW_GDI_Color_Blue;

  aBorderStyle.bottom = NW_CSS_PropValue_solid;
  aBorderStyle.left = NW_CSS_PropValue_solid;
  aBorderStyle.right = NW_CSS_PropValue_solid;
  aBorderStyle.top = NW_CSS_PropValue_solid;

  aInitialBgColor = aDeviceContext->BackgroundColor();
  
  aColor = aBorderColor.bottom;
  aDeviceContext->SwitchAgainstBG (&aColor);
  aBorderColor.bottom = aColor;
  aColor = aBorderColor.left;
  aDeviceContext->SwitchAgainstBG (&aColor);
  aBorderColor.left = aColor;
  aColor = aBorderColor.right;
  aDeviceContext->SwitchAgainstBG (&aColor);
  aBorderColor.right = aColor;
  aColor = aBorderColor.top;
  aDeviceContext->SwitchAgainstBG (&aColor);
  aBorderColor.top = aColor;
}

// -----------------------------------------------------------------------------
// NW_LMgr_ImageMapBox_InitializeImageMapIcon
// This method creates a new image map icon if one does not exist already and
//   returns a pointer to the image.
// Returns: NW_Image_AbstractImage_t*
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_ImageMapBox_InitializeImageMapIcon(NW_LMgr_ImageMapBox_t* thisObj)
  {
  NW_ASSERT(thisObj);

  NW_TRY (status)
    {
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( thisObj );
    NW_Image_AbstractImage_t* abstractImageMap = (NW_Image_AbstractImage_t*)NW_Image_CannedImages_GetImage( 
     rootBox->cannedImages, NW_Image_ImageMap );

    NW_ASSERT(abstractImageMap);

    // this will be deleted in the destructor
    thisObj->iImageMapIcon = (NW_Image_AbstractImage_t*)NW_Image_Virtual_New( abstractImageMap );
    NW_THROW_OOM_ON_NULL(thisObj->iImageMapIcon, status);
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

TBrowserStatusCode
NW_LMgr_ImageMapBox_DrawImageMapIcon(NW_LMgr_ImageMapBox_t* thisObj,
                                     CGDIDeviceContext* deviceContext)
  {
  NW_GDI_Point2D_t imagePos;
  NW_GDI_Dimension3D_t imageSize;
  NW_GDI_Rectangle_t innerRect;

  NW_TRY (status)
    {
    // draw the image map icon on top of the image map
    deviceContext->SetPaintMode (NW_GDI_PaintMode_Copy);

    // if we don't have an image map icon, create one
    if (!thisObj->iImageMapIcon)
      {
      status = NW_LMgr_ImageMapBox_InitializeImageMapIcon(thisObj);
      NW_THROW_ON_ERROR(status);
      }

    // get image size 
    status = NW_Image_AbstractImage_GetSize( thisObj->iImageMapIcon, &imageSize );
    NW_THROW_ON_ERROR(status);

    NW_LMgr_Box_GetInnerRectangle( NW_LMgr_BoxOf(thisObj), &innerRect );
    // do not stretch out image map icon
    if( innerRect.dimension.height < imageSize.height )
        {
        imageSize.height = innerRect.dimension.height;
        }
    if( innerRect.dimension.width < imageSize.width )
        {
        imageSize.width = innerRect.dimension.width;
        }

    imagePos.x = innerRect.point.x + NW_LMgr_Box_MediumBorderWidth;
    imagePos.y = innerRect.point.y + NW_LMgr_Box_MediumBorderWidth;

    status = NW_Image_AbstractImage_DrawScaled( thisObj->iImageMapIcon, 
                                                deviceContext,
                                                &imagePos, 
                                                &imageSize );
    NW_THROW_ON_ERROR(status);
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
   static data
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
static
const
NW_LMgr_IEventListener_Class_t _NW_LMgr_ImageMapBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_ImageMapBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_LMgr_ImageMapBox_IEventListener_ProcessEvent
  }
};

// -----------------------------------------------------------------------------
static
const NW_Object_Class_t* const _NW_LMgr_ImageMapBox_InterfaceList[] = {
  &_NW_LMgr_ImageMapBox_IEventListener_Class,
  NULL
};

// ========================== INTERFACE FUNCTIONS ==============================

// -----------------------------------------------------------------------------
NW_Uint8
_NW_LMgr_ImageMapBox_IEventListener_ProcessEvent(NW_LMgr_IEventListener_t* eventListener,
                                                 NW_Evt_Event_t* event)
{
  NW_LMgr_ImageMapBox_t* thisObj;
  NW_Uint8 ret = 0;

  // parameter assertion block
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  // obtain the implementer 
  thisObj = static_cast<NW_LMgr_ImageMapBox_t*>(NW_Object_Interface_GetImplementer (eventListener));

  if (thisObj->eventHandler != NULL) 
  {
    ret = NW_LMgr_EventHandler_ProcessEvent (thisObj->eventHandler,
                                             NW_LMgr_BoxOf (thisObj), 
                                             event);
  }

  return ret;
}

// -----------------------------------------------------------------------------
const
NW_LMgr_ImageMapBox_Class_t  NW_LMgr_ImageMapBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_AnimatedImageBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ _NW_LMgr_ImageMapBox_InterfaceList
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_ImageMapBox_t),
    /* construct                 */ _NW_LMgr_ImageMapBox_Construct,
    /* destruct                  */ _NW_LMgr_ImageMapBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_ImgContainerBox_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_ImageMapBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_AnimatedImageBox_Draw,
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
  },
  { /* NW_LMgr_AnimatedImageBox  */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_ImageMapBox       */
    /* unused                    */ NW_Object_Unused
  }
};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// _NW_LMgr_ImageMapBox_Construct
// Description:  The constructor.  
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
_NW_LMgr_ImageMapBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp)
{
  NW_TRY (status) 
  {
    NW_LMgr_ImageMapBox_t* imageBox;

    NW_ASSERT(dynamicObject != NULL);

    // invoke our superclass constructor
    status = _NW_LMgr_AnimatedImageBox_Construct (dynamicObject, argp);
    NW_THROW_ON_ERROR(status);

    // for convenience
    imageBox = NW_LMgr_ImageMapBoxOf( dynamicObject );
   
    // initialize the member variables
    imageBox->eventHandler = va_arg (*argp, NW_LMgr_EventHandler_t*);
    
    imageBox->iUrl = va_arg (*argp, NW_Text_t*);

    imageBox->iAreaBoxArrayList = NULL;

    imageBox->iImageMapIcon = NULL;
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
// _NW_LMgr_ImageMapBox_Destruct
// Description:  The destructor.  
// Returns: void
// -----------------------------------------------------------------------------
//
void
_NW_LMgr_ImageMapBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
  {
  NW_LMgr_ImageMapBox_t* thisObj;

  // for convenience 
  thisObj = NW_LMgr_ImageMapBoxOf (dynamicObject);
  // destroy members
  if (thisObj->iAreaBoxArrayList != NULL)
    {
    //
    NW_ADT_Vector_Metric_t arrayIndex;
    NW_ADT_Vector_Metric_t elementSize = NW_ADT_Vector_GetSize( thisObj->iAreaBoxArrayList );

    for( arrayIndex = 0; arrayIndex < elementSize; arrayIndex++ ) 
      {
      NW_LMgr_AreaBox_t* areaBox = *(NW_LMgr_AreaBox_t**)NW_ADT_Vector_ElementAt( thisObj->iAreaBoxArrayList, arrayIndex );
      NW_Object_Delete( areaBox );
      }

    NW_Object_Delete (thisObj->iAreaBoxArrayList);
    thisObj->iAreaBoxArrayList = NULL;
    }

  if (thisObj->iImageMapIcon != NULL)
    {
    NW_Object_Delete( thisObj->iImageMapIcon );
    thisObj->iImageMapIcon = NULL;
    }
  
  if (thisObj->iUrl != NULL)
    {
    NW_Object_Delete( thisObj->iUrl );
    thisObj->iUrl = NULL;
    }
    
  if (thisObj->eventHandler != NULL)
    {
  	NW_Object_Delete( thisObj->eventHandler );
  	thisObj->eventHandler = NULL;
    }
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_ImageMapBox_GetMinimumContentSize( NW_LMgr_Box_t* box,
                                            NW_GDI_Dimension2D_t* size )
  {
  NW_TRY (status) 
    {
    NW_GDI_Dimension3D_t imageSize;
    NW_LMgr_ImageMapBox_t* thisObj = NW_LMgr_ImageMapBoxOf( box );
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

    // call the parent's class implementation to get the size of the image map box
    status = _NW_LMgr_ImgContainerBox_GetMinimumContentSize (box, size);
    NW_THROW_ON_ERROR( status );
    
    // we only want to recalculate the size of the image map box if we are
    // in small screen mode
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
      {
      // now we need to get the size of the icon we are using
      // if we don't have an image map icon, create one
      if (!thisObj->iImageMapIcon)
        {
        status = NW_LMgr_ImageMapBox_InitializeImageMapIcon(thisObj);
        NW_THROW_ON_ERROR(status);
        }

      status = NW_Image_AbstractImage_GetSize( thisObj->iImageMapIcon, &imageSize );
      NW_THROW_ON_ERROR(status);
  
      // we want to see if the image map box size is less that the icon size plus
      // the size of the borders (upper/lower)
      if (size->height < (imageSize.height + (NW_LMgr_Box_MediumBorderWidth * 2)))
        {
        size->height = (imageSize.height + (NW_LMgr_Box_MediumBorderWidth * 2));
        }
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
NW_LMgr_ImageMapBox_t*
NW_LMgr_ImageMapBox_New (NW_ADT_Vector_Metric_t aNumProperties, 
                         NW_Image_AbstractImage_t* aVirtualImage, 
                         NW_Text_t* aAltText, 
                         NW_Bool aBrokenImage,
                         NW_LMgr_EventHandler_t* aEventHandler,
                         NW_Text_t* aUrl)
{
  return (NW_LMgr_ImageMapBox_t*)NW_Object_New (&NW_LMgr_ImageMapBox_Class, 
    aNumProperties, aVirtualImage, aAltText, aBrokenImage, aEventHandler, aUrl );
}

// -----------------------------------------------------------------------------
void
NW_LMgr_ImageMapBox_SetAreaBoxList (NW_LMgr_ImageMapBox_t* thisObj, 
                                    NW_ADT_DynamicVector_t* aAreaBoxArrayList)
{
  thisObj->iAreaBoxArrayList = aAreaBoxArrayList;
}

// -----------------------------------------------------------------------------
NW_ADT_DynamicVector_t*
NW_LMgr_ImageMapBox_GetAreaBoxList (NW_LMgr_ImageMapBox_t* thisObj)
{
  return thisObj->iAreaBoxArrayList;
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_LMgr_ImageMapBox_DrawBorder(NW_LMgr_Box_t* box,
                               CGDIDeviceContext* deviceContext)
  {
  NW_LMgr_FrameInfo_t borderWidth;
  NW_LMgr_FrameInfo_t borderStyle;
  NW_LMgr_FrameInfo_t borderColor;
  NW_GDI_Color_t initialBgColor = 0;
  NW_GDI_Color_t color = 0;
  NW_LMgr_ImageMapBox_t* thisObj = NULL; 
  NW_ADT_Vector_Metric_t arrayIndex;
  NW_ADT_Vector_Metric_t elementSize;
  NW_LMgr_AreaBox_t* areaBox = NULL;

  NW_TRY(status)
    {  
    NW_ASSERT(box != NULL);
    thisObj = NW_LMgr_ImageMapBoxOf( box );

    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(box);
    NW_ASSERT(rootBox);

    NW_HED_AppServices_t* appServices = rootBox->appServices;
    NW_ASSERT(appServices);

    NW_LMgr_ImageMapBox_InitializeBorder(deviceContext,
                                         borderWidth,
                                         borderColor,
                                         borderStyle,
                                         initialBgColor,
                                         color);

    // if we are in vertical layout mode (small screen) then we want to "highlight" the
    // image box and display the image map icon if it has focus, otherwise we are in 
    // normal layout mode and we will want to highlight the area box that has focus
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
      {
      if( NW_LMgr_Box_HasFocus ( NW_LMgr_BoxOf( thisObj ) ) )
        {
        // draw the border around the image map box
        status = NW_LMgr_Box_DrawBorder(NW_LMgr_Box_GetFormatBounds(box), 
                                         deviceContext, 
                                         &borderWidth, 
                                         &borderStyle, 
                                         &borderColor);
        NW_THROW_ON_ERROR(status);

        // draw the image map icon on the top left corner of the image
        status = NW_LMgr_ImageMapBox_DrawImageMapIcon(thisObj,
                                                      deviceContext);

        NW_THROW_ON_ERROR(status);
        }
      }
    else
      {
      // loop through area boxes and see if any are in focus
      NW_ADT_DynamicVector_t* areaBoxArray = NW_LMgr_ImageMapBox_GetAreaBoxList(thisObj);

      // It is possible to have an ImageMapBox with no AreaBoxArray
      // This happens when the usemap id is not present anywhere in the document
      if (areaBoxArray != NULL)
        {
        // find the index of the  node vector
        elementSize = NW_ADT_Vector_GetSize(areaBoxArray);
    
        for (arrayIndex = 0; arrayIndex < elementSize; arrayIndex++) 
          {
          areaBox = *(NW_LMgr_AreaBox_t**)NW_ADT_Vector_ElementAt (areaBoxArray, arrayIndex);
 
          if( NW_LMgr_Box_HasFocus( NW_LMgr_BoxOf( areaBox ) ) ) 
            {
            // draw the border around the area box
            status = NW_LMgr_Box_DrawBorder (NW_LMgr_AreaBox_GetBounds(NW_LMgr_BoxOf(areaBox)), 
                                             deviceContext, 
                                             &borderWidth, 
                                             &borderStyle, 
                                             &borderColor);

            NW_THROW_ON_ERROR(status);

            // if the image map box is broken, then we will need to display the alt text
            // of the area boxes if alt text is provided
            if( appServices->browserAppApi.IsImageMapView() && (NW_LMgr_ImgContainerBox_ImageIsBroken(box))) 
              {
              status = NW_LMgr_AreaBox_DisplayAltText( areaBox, deviceContext );
              NW_THROW_ON_ERROR (status);
              }

            break;
            }
          }
        }
      }
    }
  NW_CATCH (status)
    {
    }
  NW_FINALLY
    {
    deviceContext->SetBackgroundColor( initialBgColor);
    return status;
    }
  NW_END_TRY
  }

// -----------------------------------------------------------------------------
// Function: NW_LMgr_ImageMapBox_Copy
// Description: Will return a new box which is a clone of the image map box to be 
//  used in the image map box view. The caller is responsible for deletion of the 
//  new box
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_ImageMapBox_Copy(NW_LMgr_Box_t* aBox, 
                         NW_LMgr_Box_t** aNewBox)
  {
  NW_LMgr_ImageMapBox_t* oldImageMapBox = (NW_LMgr_ImageMapBox_t*)aBox;
  NW_LMgr_ImageMapBox_t* newImageMapBox = NULL;
  NW_ADT_Vector_Metric_t arrayIndex;
  NW_ADT_Vector_Metric_t elementSize;
  NW_LMgr_AreaBox_t* newAreaBox = NULL;
  NW_LMgr_AreaBox_t* oldAreaBox = NULL;
  NW_Image_Virtual_t* newVirtualImage = NULL;
  NW_HED_EventHandler_t* eventHandler = NULL;
  NW_Text_t* altText = NULL;
  NW_Text_t* url = NULL;
  NW_ADT_DynamicVector_t* areaList = NULL;
  NW_GDI_Rectangle_t bounds;
  NW_LMgr_SimplePropertyList_t* oldPropertyList = NULL;
  NW_LMgr_SimplePropertyList_t* newPropertyList = NULL;
  void** ptr = NULL;

  NW_ASSERT(oldImageMapBox);

  NW_TRY( status ) 
    {
    // copy the virtual image (each image map box needs it's own.)
    // it will be deleted in the destructor
    NW_Image_Virtual_t* virtualImage = (NW_Image_Virtual_t*)oldImageMapBox->super.super.virtualImage;
    newVirtualImage = NW_Image_Virtual_New( virtualImage->image );
    NW_THROW_OOM_ON_NULL( newVirtualImage, status );

    // copy the alt text
    // the alt text will be deleted in the destructor
    if (oldImageMapBox->super.super.altText)
      {
      altText = NW_Text_Copy(oldImageMapBox->super.super.altText, NW_TRUE);
      NW_THROW_OOM_ON_NULL( altText, status );
      }

    if (oldImageMapBox->iUrl)
      {
      url = NW_Text_Copy(oldImageMapBox->iUrl, NW_TRUE);
      NW_THROW_OOM_ON_NULL( url, status );
      }

    // copy the event handler
    eventHandler = NW_HED_EventHandler_New(NW_HED_EventHandlerOf(oldImageMapBox->eventHandler)->documentNode, NW_HED_EventHandlerOf(oldImageMapBox->eventHandler)->context );
    NW_THROW_OOM_ON_NULL( eventHandler, status );

    // the caller of this method is responsible for deleting the new image map box
    newImageMapBox = NW_LMgr_ImageMapBox_New(5, 
                                             (NW_Image_AbstractImage_t*)newVirtualImage, 
                                             altText,
                                             NW_LMgr_ImgContainerBox_ImageIsBroken((NW_LMgr_Box_t*)oldImageMapBox),
                                             NW_LMgr_EventHandlerOf(eventHandler),
                                             url);
    NW_THROW_OOM_ON_NULL( newImageMapBox, status );

    // now that the newImageMapBox has taken ownership of the image, alttext, and eventhandlers, set them to NULL
    newVirtualImage = NULL;
    altText = NULL;
    eventHandler = NULL;
    url = NULL;

    // create the new image map box's area list 
    areaList = (NW_ADT_DynamicVector_t*) NW_ADT_ResizableVector_New (sizeof(NW_LMgr_AreaBox_t), 10, 5 );
    NW_THROW_OOM_ON_NULL (areaList, status);
      
    // set the area list on the image map box
    NW_LMgr_ImageMapBox_SetAreaBoxList(newImageMapBox, areaList);

    // copy the old image map box's area array to the new image map box's area array
    NW_ADT_DynamicVector_t* oldAreaList = NW_LMgr_ImageMapBox_GetAreaBoxList(oldImageMapBox);

    if (oldAreaList != NULL)
      {
      // find the index of the  node vector
      elementSize = NW_ADT_Vector_GetSize(oldAreaList);
  
      for (arrayIndex = 0; arrayIndex < elementSize; arrayIndex++) 
        {
        oldAreaBox = *(NW_LMgr_AreaBox_t**)NW_ADT_Vector_ElementAt (oldAreaList, arrayIndex);
         
        status = NW_LMgr_AreaBox_Copy(oldAreaBox, &newAreaBox);
        NW_THROW_OOM_ON_NULL (newAreaBox, status);

        NW_LMgr_AreaBox_SetParentImageMapBox (newAreaBox, newImageMapBox);
        
        ptr = NW_ADT_DynamicVector_InsertAt (areaList, &newAreaBox, NW_ADT_Vector_AtEnd);
        NW_THROW_OOM_ON_NULL(ptr, status);
        }
      }

    // now that the area list has been set on the image map box, set the pointer to NULL
    areaList = NULL;

    // set the old image map box bounds as the bounds for the new image map box
    bounds = NW_LMgr_Box_GetFormatBounds(NW_LMgr_BoxOf(oldImageMapBox));
    NW_LMgr_Box_SetFormatBounds(NW_LMgr_BoxOf(newImageMapBox), bounds);
    
    // in order to have good picture quality once we are in image map mode, we need to delete
    // the bitmap that was associated with the image map box in small screen mode and redecode 
    // it.  This will create a new bitmap that will be better for the larger display
    // of course we only want to do this if we are not a broken image
    if (!NW_LMgr_ImgContainerBox_ImageIsBroken(aBox))
      {
      NW_Image_Epoc32Simple_t* image = NW_Image_Epoc32SimpleOf (virtualImage->image);
      
      // delete the image bitmap that was used in small screen mode
      if(( image->imageDecoding == NW_FALSE ) && (image->bitmap))
        {
        // delete the image bitmap
        CFbsBitmap* oldBitmap = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)image->bitmap);
        delete oldBitmap;
        image->bitmap = NULL;
        CFbsBitmap* oldMask = CONST_CAST(CFbsBitmap*, (const CFbsBitmap*)image->mask);
        delete oldMask;
        image->mask = NULL;
        image->srcHeight = 0;
        image->srcWidth = 0;
        }
      
      // force a decode on the image    
      (void)NW_Image_AbstractImage_ForceImageDecode( NW_Image_AbstractImageOf(virtualImage ));
      }

    // copy the old image map box property list to the new image map box property list
    oldPropertyList = (NW_LMgr_SimplePropertyList_t*)NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(oldImageMapBox));
    status = NW_LMgr_SimplePropertyList_CopyEntireList(oldPropertyList, &newPropertyList);
    NW_THROW_ON_ERROR (status);

    // Delete the default property list that was allocated when newImageMapBox was created,
    // otherwise we leak
    NW_Object_Delete( (NW_LMgr_SimplePropertyList_t*)NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(newImageMapBox)) );

    // set the property list on the new image map box
    status = NW_LMgr_Box_SetPropList(NW_LMgr_BoxOf(newImageMapBox), (NW_LMgr_PropertyList_t*)newPropertyList);
    NW_THROW_ON_ERROR (status);

    *aNewBox = (NW_LMgr_Box_t*)newImageMapBox;
    }
  NW_CATCH( status ) 
    {
    if (newVirtualImage)
      {
      NW_Object_Delete(newVirtualImage);
      }

    if (altText)
      {
      NW_Object_Delete(altText);
      }

    if (url)
      {
      NW_Object_Delete(url);
      }

    if (eventHandler)
      {
      NW_Object_Delete(eventHandler);
      }

    if (newImageMapBox)
      {
      NW_Object_Delete(newImageMapBox);
      }

    if (areaList)
      {
      NW_Object_Delete(areaList);
      }

    if (newPropertyList)
      {
      NW_Object_Delete(newPropertyList);
      }
    } 
  NW_FINALLY 
    {
    return status;  
    } 
  NW_END_TRY
  }
