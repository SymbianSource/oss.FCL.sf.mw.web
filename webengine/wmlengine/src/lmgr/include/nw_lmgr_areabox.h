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
* Description:  Box class associated with the <area> tag
*
*/


#ifndef NW_AREABOX_H
#define NW_AREABOX_H

//  INCLUDES
#include "nw_lmgr_activebox.h"
#include "NW_LMgr_IEventListener.h"
#include "nw_lmgr_eventhandler.h"
#include "nw_lmgr_imagemapbox.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// DATA TYPES
typedef enum NW_LMGR_AreaBoxShape_Type_e{
    NW_LMGR_ABS_RECT,
    NW_LMGR_ABS_CIRC,
    NW_LMGR_ABS_POLY
}NW_LMGR_AreaBoxShape_Type_t;

// ------------------------------------------------------------------------- 
//   forward declarations
// ------------------------------------------------------------------------- 
typedef struct NW_LMgr_AreaBox_Class_s NW_LMgr_AreaBox_Class_t;
typedef struct NW_LMgr_AreaBox_s NW_LMgr_AreaBox_t;

// ------------------------------------------------------------------------- 
//   class declaration
// ------------------------------------------------------------------------- 
typedef struct NW_LMgr_AreaBox_ClassPart_s {
  void** unused;
} NW_LMgr_AreaBox_ClassPart_t;
  
struct NW_LMgr_AreaBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_LMgr_AreaBox_ClassPart_t NW_LMgr_AreaBox;
};

// ------------------------------------------------------------------------- 
//   object declaration
// ------------------------------------------------------------------------- 
struct NW_LMgr_AreaBox_s {
  NW_LMgr_ActiveBox_t super;

  // implemented interface
  NW_LMgr_IEventListener_t NW_LMgr_IEventListener;

  // member variables 
  NW_LMgr_EventHandler_t* eventHandler;

  NW_LMgr_ImageMapBox_t* iImageMapBox;

  NW_Text_t* iCoordsText;
  NW_Text_t* iShapeText;
  NW_Text_t* iAltText;
  NW_Text_t* iHRefText;

  NW_Ucs2* iString;
  NW_GDI_Rectangle_t iComboBounds;

  NW_Bool iIsInvalid;

  NW_LMGR_AreaBoxShape_Type_t iShape;
};
    
// ------------------------------------------------------------------------- 
//   convenience macros
// ------------------------------------------------------------------------- 
#define NW_LMgr_AreaBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_AreaBox))

#define NW_LMgr_AreaBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_AreaBox))

// ------------------------------------------------------------------------- 
//   global static data
// ------------------------------------------------------------------------- 
NW_LMGR_EXPORT const NW_LMgr_AreaBox_Class_t NW_LMgr_AreaBox_Class;

// ------------------------------------------------------------------------- 
//   public methods
// ------------------------------------------------------------------------- 

/**
* NW_LMgr_AreaBox_New
* @desc Method to return a new NW_LMgr_AreaBox_t object
* @param NW_ADT_Vector_Metric_t aNumProperties: number of arguments in the parameter list
* @param NW_LMgr_EventHandler_t* aEventHandler: pointer to the eventHandler associated 
*   with this box
* @param NW_LMgr_ImageMapBox_t* aImageMapBox: the parent image map box to which this area 
*   box belongs
* @param NW_Text_t* aShapeText: pointer to the string which holds the shape name
* @param NW_Text_t* aCoordsText: pointer to the string which holds the coordinates of this area
* @param NW_Text_t* aAltText: pointer to the string which holds the alternate text for this area
* @param NW_Text_t* aHRefText: pointer to the href string associated with this area
* @return NW_LMgr_AreaBox_t*: pointer to a NW_LMgr_AreaBox_t object
*/
NW_LMgr_AreaBox_t*
NW_LMgr_AreaBox_New (NW_ADT_Vector_Metric_t aNumProperties,
                     NW_LMgr_EventHandler_t* aEventHandler,
                     NW_LMgr_ImageMapBox_t* aImageMapBox,
                     NW_Text_t* aShapeText,
                     NW_Text_t* aCoordsText,
                     NW_Text_t* aAltText,
                     NW_Text_t* aHRefText);

/**
* NW_LMgr_AreaBox_GetBounds
* @description This method "overrides" the non-virtual method 'GetBounds' off the base  
*   NW_LMgr_Box_t class. We need to override GetBounds because the bounds of the area box are 
*   only relative to it's parent image box bounds.  For all the calculations used in the view, 
*   return the bounds of the area box _plus_ the bounds of the parent image box so that the 
*   we want to correct actions are performed when manipulating the view.  
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to retreive the bounds of
* @return NW_GDI_Rectangle_t*: pointer to a NW_GDI_Rectangle_t which is the addition of the 
*   area box bounds _plus_ the image map box bounds.
*/
NW_GDI_Rectangle_t 
NW_LMgr_AreaBox_GetBounds (NW_LMgr_Box_t* aBox);

/**
* NW_LMgr_AreaBox_SetBounds
* @description This method "overrides" the non-virtual method 'SetBounds' off the base  
*   NW_LMgr_Box_t class. 
* @param NW_LMgr_Box_t* box: pointer to the box you want to set the bounds of
* @param  NW_GDI_Rectangle_t: NW_GDI_Rectangle_t which is the addition of the 
*   area box bounds _plus_ the image map box bounds.
*/
void
NW_LMgr_AreaBox_SetBounds( NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aBoxBounds );

/**
* NW_LMgr_AreaBox_GetRootBox
* @description This method "overrides" the non-virtual method 'GetRootBox' off the base  
*   NW_LMgr_Box_t class. We need to override GetRootBox because the area box is not in the box 
*   tree (only its associated image map box parent is) so it does not have its own root box. 
*   Instead we need to return the root box of the image map box parent. 
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to retreive the root box of
* @return NW_LMgr_RootBox_t*: the root box of our parent image map box because this area box
*   doesn have one of its own.
*/
NW_LMgr_RootBox_t* 
NW_LMgr_AreaBox_GetRootBox (NW_LMgr_Box_t* aBox);

/**
* NW_LMgr_AreaBox_GetParentImageMapBox
* @description This method returns a pointer to this area box's parent image map box
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to retrieve the parent image map
*    box of
* @return NW_LMgr_ImageMapBox_t*: pointer to this area box's parent image map box
*/
NW_LMgr_ImageMapBox_t* 
NW_LMgr_AreaBox_GetParentImageMapBox (NW_LMgr_Box_t* aBox);

/**
* NW_LMgr_AreaBox_SetParentImageMapBox
* @description This method sets this area box's parent image map box
* @param NW_LMgr_AreaBox_t* aAreaBox: pointer to the box you want to set the parent image map
*    box of
* @param NW_LMgr_ImageMapBox_t* aParentImageMapBox: pointer to the image map box parent
* @return void
*/
void
NW_LMgr_AreaBox_SetParentImageMapBox (NW_LMgr_AreaBox_t* aAreaBox, NW_LMgr_ImageMapBox_t* aParentImageMapBox);

/**
* NW_LMgr_AreaBox_IsValidAreaBox
* @description This method returns TRUE if the area box is valid (i.e the coordinates check out)
* @param NW_LMgr_Box_t* aBox: pointer to the box you want to see is valid
* @return NW_Bool: TRUE if the area box is valid, FALSE otherwise
*/
NW_Bool 
NW_LMgr_AreaBox_IsInvalidAreaBox (NW_LMgr_Box_t* aBox);

/**
* NW_LMgr_AreaBox_Copy
* @description Will return a new box which is a copy of the area box passed in as the first
*   parameter. The caller of this method is responsible for deleting the newly created
*   area box. This method is mainly used when making a copy of the area box array 
* @param NW_LMgr_AreaBox_t* aBox: the original box you want to copy
* @param NW_LMgr_AreaBox_t** aNewBox: address of a pointer to a area box object
* @return TBrowserStatusCode
*/
TBrowserStatusCode
NW_LMgr_AreaBox_Copy(NW_LMgr_AreaBox_t* aBox, NW_LMgr_AreaBox_t** aNewBox);

/**
* NW_LMgr_AreaBox_DisplayAltText
* @description This method is called to display alt-text associated with the area box
*   if the image map box image is broken.   
* @param NW_LMgr_AreaBox_t* aBox: the area box which you want to display the alt text for
* @param CGDIDeviceContext* aDeviceContext: pointer to the device context
* @return TBrowserStatusCode
*/
TBrowserStatusCode
NW_LMgr_AreaBox_DisplayAltText(NW_LMgr_AreaBox_t* aAreaBox, 
                               CGDIDeviceContext* aDeviceContext);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_AREABOX_H */
