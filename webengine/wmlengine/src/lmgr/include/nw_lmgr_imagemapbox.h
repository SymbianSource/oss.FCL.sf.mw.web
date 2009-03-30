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


#ifndef NW_IMAGEMAPBOX_H
#define NW_IMAGEMAPBOX_H

//  INCLUDES
#include <e32def.h>
#include <e32base.h>
#include <bitdev.h>
#include "nw_lmgr_animatedimagebox.h"
#include "nw_object_dynamic.h"
#include "nw_adt_resizablevector.h"
#include "nw_image_abstractimage.h"
#include "NW_LMgr_IEventListener.h"
#include "nw_lmgr_eventhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// -------------------------------------------------------------------------
//   forward declarations
// ------------------------------------------------------------------------- 
typedef struct NW_LMgr_ImageMapBox_Class_s NW_LMgr_ImageMapBox_Class_t;
typedef struct NW_LMgr_ImageMapBox_s NW_LMgr_ImageMapBox_t;

// ------------------------------------------------------------------------- 
//   class declaration
// ------------------------------------------------------------------------- 
typedef struct NW_LMgr_ImageMapBox_ClassPart_s {
  void** unused;
} NW_LMgr_ImageMapBox_ClassPart_t;

struct NW_LMgr_ImageMapBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_ImgContainerBox_ClassPart_t NW_LMgr_ImgContainerBox;
  NW_LMgr_AnimatedImageBox_ClassPart_t NW_LMgr_AnimatedImageBox;
  NW_LMgr_ImageMapBox_ClassPart_t NW_LMgr_ImageMapBox;
};

// ------------------------------------------------------------------------- 
//   object declaration
// ------------------------------------------------------------------------- 
struct NW_LMgr_ImageMapBox_s {
  NW_LMgr_AnimatedImageBox_t super;

  // implemented interfaces
  NW_LMgr_IEventListener_t NW_LMgr_IEventListener;
  NW_LMgr_EventHandler_t* eventHandler;

  // member variables 
  NW_ADT_DynamicVector_t* iAreaBoxArrayList;

  NW_Image_AbstractImage_t* iImageMapIcon;
  NW_Text_t* iUrl;
};

// ------------------------------------------------------------------------- 
//   convenience macros
// ------------------------------------------------------------------------- 
#define NW_LMgr_ImageMapBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ImageMapBox))

#define NW_LMgr_ImageMapBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ImageMapBox))

// ------------------------------------------------------------------------- 
//   global static data
// ------------------------------------------------------------------------- 
NW_LMGR_EXPORT const NW_LMgr_ImageMapBox_Class_t NW_LMgr_ImageMapBox_Class;

// ------------------------------------------------------------------------- 
//   public methods
// ------------------------------------------------------------------------- 

/**
* NW_LMgr_ImageMapBox_New
* @desc Method to return a new NW_LMgr_ImageMapBox_t object
* @param NW_ADT_Vector_Metric_t aNumProperties: number of arguments in the 
*   parameter list
* @param NW_Image_AbstractImage_t* aVirtualImage: the virtual image 
*   associated with this image box (passed up to the base class)
* @param NW_Text_t* aAltText: pointer to the string which holds the 
*   alternate text for this image map box (passed up to the base class)
* @param NW_Bool aBrokenImage: a boolean value to determine whether or not 
*   this image is a broken image (passed up to the base class)
* @param NW_LMgr_EventHandler_t* aEventHandler: pointer to the eventHandler 
*   associated with this box
* @param NW_Text_t* aUrl: pointer to the string which holds the 
*   url associated with the image
* @return NW_LMgr_AreaBox_t*: pointer to a NW_LMgr_ImageMapBox_t object
*/
NW_LMgr_ImageMapBox_t*
NW_LMgr_ImageMapBox_New (NW_ADT_Vector_Metric_t aNumProperties,  
                         NW_Image_AbstractImage_t* aVirtualImage, 
                         NW_Text_t* aAltText, 
                         NW_Bool aBrokenImage,
                         NW_LMgr_EventHandler_t* aEventHandler,
                         NW_Text_t* aUrl);

/**
* NW_LMgr_ImageMapBox_GetAreaBoxList
* @description This method returns a pointer to the image map box's area 
*   box list
* @param NW_LMgr_ImageMapBox_t*: pointer to the image map box for which 
*   you want to retrieve the area box list
* @return NW_ADT_DynamicVector_t*: pointer to a NW_ADT_DynamicVector_t 
*   (area box list)
*/
NW_ADT_DynamicVector_t*
NW_LMgr_ImageMapBox_GetAreaBoxList (NW_LMgr_ImageMapBox_t* aImageMapBox);

/**
* NW_LMgr_ImageMapBox_SetAreaBoxList
* @description This method sets the area box list on the image box 
*   (because it is created elsewhere, but the image map box should own it.) 
* @param NW_LMgr_Box_t* box: pointer to the the image map box you want to 
*   set the area box list on
* @return void
*/
void
NW_LMgr_ImageMapBox_SetAreaBoxList (NW_LMgr_ImageMapBox_t* aImageMapBox, 
                                    NW_ADT_DynamicVector_t* aAreaBoxArrayList);

/**
* NW_LMgr_ImageMapBox_DrawBorder
* @description This method is called to draw the border for the image map box.
*   In the case of small screen mode, a border is drawn around the in focus
*   scaled image. In the case of normal layout mode, a border is drawn around 
*   the in focus area box.
* @param NW_LMgr_Box_t* aBox: the box for which the border is to be drawn
* @param CGDIDeviceContext* aDeviceContext: the device context to be used
* @return TBrowserStatusCode
*/
TBrowserStatusCode
NW_LMgr_ImageMapBox_DrawBorder(NW_LMgr_Box_t* aBox,
                               CGDIDeviceContext* aDeviceContext);

/**
* NW_LMgr_ImageMapBox_Copy
* @description Will return a new box which is a copy of the image map box passed in as
*   the first parameter. The caller of this method is responsible for deleting the newly
*   created image map box. This method is primarily used for creating
*   a box tree node for use in the image map view.
* @param NW_LMgr_Box_t* aBox: the original box you want to copy
* @param NW_LMgr_Box_t** aNewBox: address of a pointer to a image map box object
* @return TBrowserStatusCode
*/
TBrowserStatusCode
NW_LMgr_ImageMapBox_Copy(NW_LMgr_Box_t* aBox, NW_LMgr_Box_t** aNewBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_IMAGEMAPBOX_H */
