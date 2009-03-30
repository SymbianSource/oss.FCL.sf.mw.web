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


#ifndef NW_LMGR_ROOTBOX_H
#define NW_LMGR_ROOTBOX_H
  
#include "nw_lmgr_bidiflowbox.h"
#include "LMgrBoxTreeListener.h"
#include "nw_image_cannedimages.h"
#include "nw_system_timer.h"
#include "nw_hed_appservices.h"
#include "BrsrStatusCodes.h"

#include "BoxRender.h"
  
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The origin in a Right to Left document*/
#define RIGHT_ORIGIN (0x3FFF)

    
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_RootBox_Class_s NW_LMgr_RootBox_Class_t;
typedef enum NW_LMgr_RootBox_Rendering_State_e
  {
  NW_LMgr_RootBox_RenderUninitialized,
  NW_LMgr_RootBox_RenderFirstChunk,
  NW_LMgr_RootBox_RenderFurtherChunks,
  NW_LMgr_RootBox_RenderComplete
  } NW_LMgr_RootBox_Rendering_State_t;

/* ------------------------------------------------------------------------- *
   class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_RootBox_ClassPart_s {
  void** unused;
} NW_LMgr_RootBox_ClassPart_t;

struct NW_LMgr_RootBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
  NW_LMgr_BidiFlowBox_ClassPart_t NW_LMgr_BidiFlowBox;
  NW_LMgr_RootBox_ClassPart_t NW_LMgr_RootBox;
};

/* ------------------------------------------------------------------------- *
   object structure
 * ------------------------------------------------------------------------- */
struct NW_LMgr_RootBox_s {
  NW_LMgr_BidiFlowBox_t super;

  /* interfaces implemented */
  MBoxTreeListener* boxTreeListener;

  /* The tree extents (calculated during the first Render) */
  NW_GDI_Rectangle_t extents;
  NW_Bool calculateExtents;

  /* This holds the real width  of the content when it is right aligned */
  NW_GDI_Dimension2D_t scrollRange;
  NW_Bool calculateScrollRange;

  /* We cache the boxes that are visible on-screen.  */
  NW_ADT_DynamicVector_t *renderedBoxes;
  NW_ADT_DynamicVector_t *renderedClips;
  NW_Bool renderFromCache;

  /* The box currently in focus */
  NW_LMgr_Box_t *focusBox;                  

  /* global float vectors used for formatting */
  NW_ADT_DynamicVector_t *placedFloats;      /* The floats we've already placed */
  NW_ADT_DynamicVector_t *pendingFloats;     /* The pending floats */

  NW_ADT_DynamicVector_t *positionedAbsObjects;     /* The position: absolute objects */
  NW_Bool iPositionedBoxesPlaced;

  /* AppServices */
  NW_HED_AppServices_t *appServices;

  /* The blink vars */
  NW_System_Timer_t* blinkTimer;
  NW_Int8 blinkCount;   /* i-mode HTML Elements CUIS 28.0 limits blinks to 16 */
  #define NW_ISA_BLINK_LIMIT 16
  NW_Bool blinkState;
  NW_Bool isBlinking;

  /* Is the box tree currently visible */
  NW_Bool hasFocus;
 
  /* Canned images dictionary */
  NW_Image_CannedImages_t* cannedImages;

  NW_LMgr_Box_t* body;

  NW_Int32 outstandingImageOpenings;
  NW_Bool needsRelayout;

  /* Flag related to partial rendering: 
   * markup* are useful only when partialRender is true 
   */
  NW_Bool partialRender;
  NW_LMgr_RootBox_Rendering_State_t renderingState;

  // cache settings to avoid Tls:: calls
  NW_Bool iSmallScreenOn;

  NW_Bool iHasGridModeTables;

  //
  NW_Bool iWrapEnabled;
  //
  NW_Int8 iIsPageRTL;
  
  // out of view listeners
  NW_ADT_DynamicVector_t* iOutOfViewListeners;
  NW_Int32 iYScrollChange;

};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_RootBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_RootBox))

#define NW_LMgr_RootBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_RootBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_RootBox_Class_t NW_LMgr_RootBox_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_RootBox_GetBoxTreeListener(_rootBox) \
   (NW_LMgr_RootBoxOf (_rootBox)->boxTreeListener)

#define NW_LMgr_RootBox_SetBoxTreeListener(_rootBox, _boxTreeListener) \
  ((NW_LMgr_RootBoxOf (_rootBox)->boxTreeListener = (_boxTreeListener)), \
   KBrsrSuccess)

#define NW_LMgr_RootBox_HasFocus(_rootBox) \
  (_rootBox->hasFocus)

#define NW_LMgr_RootBox_GainFocus(_rootBox) \
  ((_rootBox)->hasFocus = NW_TRUE)

#define NW_LMgr_RootBox_LoseFocus(_rootBox) \
  ((_rootBox)->hasFocus = NW_FALSE)

#define NW_LMgr_RootBox_GetAppServices(_rootBox) \
  ((_rootBox)->appServices)

#define NW_LMgr_RootBox_SetBody(_rootBox, _bodyBox) \
  ((_rootBox)->body = _bodyBox)

#define NW_LMgr_RootBox_GetBody(_rootBox) \
  ((_rootBox)->body)

NW_LMGR_EXPORT
CGDIDeviceContext*
NW_LMgr_RootBox_GetDeviceContext (NW_LMgr_RootBox_t* rootBox);

NW_LMGR_EXPORT
void
NW_LMgr_RootBox_InvalidateCache(NW_LMgr_RootBox_t *rootBox);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_RootBox_GetCurrentBox (NW_LMgr_RootBox_t* rootBox);

NW_LMGR_EXPORT
void
NW_LMgr_RootBox_BoxTreeDestroyed (NW_LMgr_RootBox_t* rootBox);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_RootBox_Refresh(NW_LMgr_RootBox_t* thisObj, NW_LMgr_Box_t* box);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_RootBox_RedrawNeeded(NW_LMgr_RootBox_t* thisObj, NW_LMgr_Box_t* box);


NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_RootBox_GetExtents (NW_LMgr_RootBox_t* rootBox,
                            NW_GDI_Dimension2D_t* extents);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_RootBox_InitializeTree(NW_LMgr_RootBox_t *rootBox);

NW_Bool NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBox_t *rootBox);

TBrowserStatusCode NW_LMgr_RootBox_GetScrollRange(NW_LMgr_RootBox_t *rootBox,
                                           NW_GDI_Dimension2D_t* scrollRange);

NW_Int32 NW_LMgr_RootBox_GetRightLimit(NW_LMgr_RootBox_t* rootBox);

NW_LMGR_EXPORT
void
NW_LMgr_RootBox_ImageOpeningStarted( NW_LMgr_RootBox_t* rootBox );

NW_LMGR_EXPORT
void
NW_LMgr_RootBox_ImageOpened( NW_LMgr_RootBox_t* rootBox );

NW_LMGR_EXPORT
void
NW_LMgr_RootBox_SetNeedsRelayout( NW_LMgr_RootBox_t* rootBox );

NW_LMGR_EXPORT
void
NW_LMgr_RootBox_ExtendsCalculationNeeded (NW_LMgr_RootBox_t* rootBox);

/** 
* NW_LMgr_RootBox_SetFocusBox
* @description Sets the focus box for this root box. Used to keep context of the current
*   box for a particular root box. 
* @param aRootBox: this object
* @param aFocusBox: current active box for this root box
* @return: void
*/
void
NW_LMgr_RootBox_SetFocusBox (NW_LMgr_RootBox_t* aRootBox, NW_LMgr_Box_t* aFocusBox);

/** 
* NW_LMgr_RootBox_GetFocusBox
* @description Called to retrieve the current active box associated with this 
*   root box
* @param aRootBox: this object
* @return: TBrowserStatusCode
*/
NW_LMgr_Box_t*
NW_LMgr_RootBox_GetFocusBox (NW_LMgr_RootBox_t* aRootBox);

/** 
* NW_LMgr_RootBox_Copy
* @description Called to create a copy of a root box given as the first parameter
* @param aOldRootBox: this object you want to create a copy from
* @param aNewRootBox: pointer to a pointer to the newly created root box
* @return: TBrowserStatusCode
*/
TBrowserStatusCode
NW_LMgr_RootBox_Copy (NW_LMgr_RootBox_t* aOldRootBox, NW_LMgr_RootBox_t** aNewRootBox);

/** 
* NW_LMgr_RootBox_SetSmallScreenOn
* @description Called to cache the small screen setting. By caching this settings, 
* we can save a lot on formatting/rendering as we don't call Tls:: functions anymore.
* @param aRootBox: current rootbox
* @param aSmallScreenOn: smallscreen setting
* @return: void
*/
void
NW_LMgr_RootBox_SetSmallScreenOn( NW_LMgr_RootBox_t* aRootBox, const NW_Bool aSmallScreenOn );

/** 
* NW_LMgr_RootBox_SetWrapEnabled
* @description Called to cache the wrap enabled setting. By caching this settings, 
* we can save a lot on formatting/rendering as we don't call Tls:: functions anymore.
* @param aRootBox: current rootbox
* @param aWrapEnabled: wrap enabled setting
* @return: void
*/
void
NW_LMgr_RootBox_SetWrapEnabled( NW_LMgr_RootBox_t* aRootBox, const NW_Bool aWrapEnabled );

/** 
* NW_LMgr_RootBox_GetSmallScreenOn
* @description Called to retrieve the cached small screen setting.
* @param aRootBox: current rootbox
* @return: NW_Bool
*/
NW_Bool
NW_LMgr_RootBox_GetSmallScreenOn( NW_LMgr_RootBox_t* aRootBox );

/** 
* NW_LMgr_RootBox_GetWrapEnabled
* @description Called to retrieve the cached wrap enabled setting.
* @param aRootBox: current rootbox
* @return: NW_Bool
*/
NW_Bool
NW_LMgr_RootBox_GetWrapEnabled( NW_LMgr_RootBox_t* aRootBox );

void NW_LMgr_RootBox_OriginChanged( NW_LMgr_RootBox_t* aRootBox, 
                                   NW_GDI_Point2D_t* aOldOrigin );

TBrowserStatusCode
NW_LMgr_RootBox_AddToOutOfViewList( NW_LMgr_RootBox_t* aRootBox, 
                                    NW_Image_AbstractImage_t* aImage );

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_RootBox_t*
NW_LMgr_RootBox_New (NW_HED_AppServices_t* appServices);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ROOTBOX_H */
