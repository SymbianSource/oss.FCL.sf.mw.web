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


#include "nw_lmgr_rootboxi.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_splittextbox.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_adt_resizablevector.h"
#include "GDIDeviceContext.h"
#include "nw_lmgr_breakbox.h"
#include "nwx_settings.h"
#include "nw_lmgr_simplepropertylist.h"

#include "nw_image_epoc32simpleimage.h"
#include "nw_image_virtualimage.h"
#include "nw_lmgr_imgcontainerbox.h"

#include <stddef.h>
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
/*                    PRIVATE METHODS                                        */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/*                    BLINK IMPLEMENTATION                                   */
/* ------------------------------------------------------------------------- */

static
TBrowserStatusCode
NW_LMgr_RootBox_Blink(NW_LMgr_RootBox_t* thisObj) 
{
  NW_LMgr_BoxVisitor_t* visitor = NULL;

  NW_TRY (status) {
  NW_LMgr_PropertyValue_t textDecoration;
  NW_LMgr_Box_t *child;

  /* Change the state */
  if (thisObj->blinkState) {
    thisObj->blinkState = NW_FALSE;
  }
  else {
    thisObj->blinkState = NW_TRUE;
  }

  /* Refresh the tree */
    visitor =
      NW_LMgr_ContainerBox_GetBoxVisitor(thisObj);
    NW_THROW_OOM_ON_NULL (visitor, status);
 
  /* Iterate through all children */
  child = NW_LMgr_BoxVisitor_NextBox(visitor, NULL);
  while ((child = NW_LMgr_BoxVisitor_NextBox(visitor, NULL)) != NULL) {

    /* Refresh all text boxes */
    if (NW_Object_IsInstanceOf(child, &NW_LMgr_AbstractTextBox_Class)) {

      /* Does the box blink? */
      textDecoration.token = NW_CSS_PropValue_none;
      NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_textDecoration, 
                                   NW_CSS_ValueType_Token, &textDecoration);

      /* Refresh */
      if ((textDecoration.token == NW_CSS_PropValue_blink) || 
          (textDecoration.token == NW_CSS_PropValue_blinkUnderline))
      {
        status = NW_LMgr_Box_Refresh(child);
	      _NW_THROW_ON_ERROR(status);
      }
    }
  }

  } NW_CATCH (status) {
  } NW_FINALLY {
  NW_Object_Delete(visitor);
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_RootBox_BlinkCallback (void *timerData)
{
  NW_TRY (status) {
  NW_LMgr_RootBox_t *thisObj;

  /* cast timerData to a box */
  thisObj = NW_LMgr_RootBoxOf (timerData);
  NW_ASSERT (thisObj);

  /* make sure it is a RootBox */
  NW_ASSERT (NW_Object_IsClass (thisObj, &NW_LMgr_RootBox_Class));

  /* If the display is suspended, do nothing */
  if (!thisObj->hasFocus) {
    return KBrsrSuccess;
  }

  /* i-mode HTML Elements CUIS 28.0 limits blinks to 16 */
  if (thisObj->blinkState == NW_FALSE) {
    thisObj->blinkCount--;
    if (thisObj->blinkCount == 0) {
      NW_Object_Delete (thisObj->blinkTimer);
      thisObj->blinkTimer = NULL;
      thisObj->isBlinking = NW_FALSE;
      return KBrsrSuccess;
    }
  }

  status = NW_LMgr_RootBox_Blink(thisObj);
    _NW_THROW_ON_ERROR (status);

} NW_CATCH (status) {
  } NW_FINALLY {
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

const
NW_LMgr_RootBox_Class_t  NW_LMgr_RootBox_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_LMgr_BidiFlowBox_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_LMgr_RootBox_t),
    /* construct               */ _NW_LMgr_RootBox_Construct,
    /* destruct                */ _NW_LMgr_RootBox_Destruct
  },
  { /* NW_LMgr_Box             */
    /* split                   */ _NW_LMgr_Box_Split,
    /* resize                  */ _NW_LMgr_Box_Resize,
  /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize   */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize     */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain               */ _NW_LMgr_Box_Constrain,
    /* draw                    */ _NW_LMgr_Box_Draw,
    /* render                  */ _NW_LMgr_RootBox_Render,
    /* getBaseline             */ _NW_LMgr_ContainerBox_GetBaseline,
    /* shift                   */ _NW_LMgr_ContainerBox_Shift,
    /* clone                   */ _NW_LMgr_ContainerBox_Clone
  },
  { /* NW_LMgr_ContainerBox    */
    /* unused                  */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox       */
    /* applyFormatProps        */ _NW_LMgr_FlowBox_ApplyFormatProps
  },
  { /* NW_LMgr_FlowBox         */
    /* unused                  */ NW_Object_Unused
  },
  { /* NW_LMgr_BidiFlowBox       */
    /* unused                  */ NW_Object_Unused
  },
  { /* NW_LMgr_RootBox         */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_LMgr_RootBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp)
{
  NW_LMgr_RootBox_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_RootBoxOf (dynamicObject);
  
  NW_TRY (status) {
    NW_HED_AppServices_t* appServices;

    /* invoke our superclass constructor */
    status = _NW_LMgr_ContainerBox_Construct (dynamicObject, argp);
      if (status != KBrsrSuccess) {
        return status;
      }

    appServices = va_arg (*argp, NW_HED_AppServices_t*);
    thisObj->appServices = appServices;

    /* We must generate the cache later */
    thisObj->renderFromCache = NW_FALSE;

    thisObj->renderingState = NW_LMgr_RootBox_RenderUninitialized;

    /* Allocate the float vectors */
    thisObj->placedFloats = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New(sizeof (NW_LMgr_Box_t*), 10, 5 );
    NW_THROW_OOM_ON_NULL (thisObj->placedFloats, status);

    thisObj->pendingFloats = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New(sizeof (NW_LMgr_Box_t*), 10, 5 );
    NW_THROW_OOM_ON_NULL (thisObj->pendingFloats, status);

    thisObj->positionedAbsObjects = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New(sizeof (NW_LMgr_Box_t*), 10, 5 );
    NW_THROW_OOM_ON_NULL (thisObj->positionedAbsObjects, status);
    thisObj->iPositionedBoxesPlaced = NW_FALSE;

    // allocate the vector
    thisObj->iOutOfViewListeners = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New(sizeof (NW_Image_Epoc32Simple_t*), 5, 2 );
    NW_THROW_OOM_ON_NULL (thisObj->iOutOfViewListeners, status);

    /* create canned images */
    thisObj->cannedImages = NW_Image_CannedImages_New();

    // 
    thisObj->outstandingImageOpenings = 0;

    thisObj->needsRelayout = NW_FALSE;

    thisObj->focusBox = NULL;

    // initiate cached settings
    thisObj->iSmallScreenOn = NW_Settings_GetVerticalLayoutEnabled();

    thisObj->iWrapEnabled = NW_Settings_GetTextWrapEnabled();
    // unset. initialize during the first IsPageRTL() call
    thisObj->iIsPageRTL = -1;

    thisObj->iYScrollChange = 0;

  } NW_CATCH (status) {
    NW_Object_Delete(thisObj->placedFloats);
    NW_Object_Delete(thisObj->pendingFloats);
    NW_Object_Delete(thisObj->positionedAbsObjects);
    NW_Object_Delete(thisObj->iOutOfViewListeners);
  } NW_FINALLY {
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_RootBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_RootBox_t* thisObj;
  
  /* for convenience */
  thisObj = NW_LMgr_RootBoxOf (dynamicObject);

  /* Delete the float vectors */
  (void) NW_ADT_DynamicVector_Clear(thisObj->placedFloats);
  (void) NW_ADT_DynamicVector_Clear(thisObj->pendingFloats);
  (void) NW_ADT_DynamicVector_Clear(thisObj->positionedAbsObjects);
  (void) NW_ADT_DynamicVector_Clear(thisObj->iOutOfViewListeners);
  NW_Object_Delete(thisObj->placedFloats);
  NW_Object_Delete(thisObj->pendingFloats);
  NW_Object_Delete(thisObj->positionedAbsObjects);
  NW_Object_Delete(thisObj->iOutOfViewListeners);

  /* Just in case:  this will delete the cache vectors  */
  NW_LMgr_RootBox_InvalidateCache(thisObj);
  
  /* Destroy the blink timer */
  if (thisObj->blinkTimer != NULL) {
    NW_Object_Delete (thisObj->blinkTimer);
  }

  /* destroy canned images */
  if (thisObj->cannedImages) {
    NW_Object_Delete(thisObj->cannedImages);
  }
    
}  

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_RootBox_Render (NW_LMgr_Box_t* box,
                         CGDIDeviceContext* deviceContext,
                         NW_GDI_Rectangle_t* clipRect,
                         NW_LMgr_Box_t* currentBox,
                         NW_Uint8 flags, 
                         NW_Bool* hasFocus,
                         NW_Bool* skipChildren,
                         NW_LMgr_RootBox_t* rootBox )
{
  NW_LMgr_RootBox_t *thisObj = NW_LMgr_RootBoxOf(box);
  NW_ADT_Vector_Metric_t floatCount, index;
  NW_LMgr_Box_t *floatBox;
  NW_GDI_Rectangle_t oldClip;
  NW_Bool tempHasFocus;

  NW_REQUIRED_PARAM( hasFocus );
  NW_REQUIRED_PARAM( rootBox );
  NW_REQUIRED_PARAM( skipChildren );

  // Save the old clip rect 
  oldClip = deviceContext->ClipRect();

  NW_TRY (status) 
    {

    NW_REQUIRED_PARAM(flags);

    if( thisObj->renderFromCache ) 
      {
      NW_LMgr_Box_t *childBox = NULL;
      NW_GDI_Rectangle_t childClipRect;
      NW_ADT_Vector_Metric_t boxCount, boxIndex;
      NW_LMgr_EventHandler_t *eventHandler = NULL;

      // Special handling for active containers 
      if (NW_Object_IsInstanceOf(currentBox, &NW_LMgr_ActiveContainerBox_Class)) 
        {
        eventHandler = NW_LMgr_ActiveContainerBoxOf(currentBox)->eventHandler;
        }

      // Go through the cached boxes 
      boxCount = NW_ADT_Vector_GetSize(thisObj->renderedBoxes);
      NW_ADT_Vector_Metric_t clipsCount = NW_ADT_Vector_GetSize(thisObj->renderedClips);
      for (boxIndex = 0; boxIndex < boxCount; boxIndex++) 
        {
        NW_ASSERT(boxIndex < clipsCount);
        childBox =
          *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (thisObj->renderedBoxes, boxIndex);

          childClipRect =
            *(NW_GDI_Rectangle_t*) NW_ADT_Vector_ElementAt (thisObj->renderedClips, boxIndex);

        /* Special handling for active containers */
        if (NW_Object_IsInstanceOf(childBox, &NW_LMgr_ActiveContainerBox_Class)) 
          {
          if (NW_LMgr_ActiveContainerBoxOf(childBox)->eventHandler
              == eventHandler) 
            {
            currentBox = childBox;
            }
          }

        tempHasFocus = NW_FALSE;
        /* Are we in focus */
        // check if childbox is the current box
        tempHasFocus = ( childBox == currentBox );
        if( !tempHasFocus )
          {
          // Find out if the box or the parent is in focus 
          // if the currentBox is a container, then check its childern to see if 'childBox' is
          // one of them.
          if( currentBox && NW_Object_IsDerivedFrom( currentBox, &NW_LMgr_ContainerBox_Class) )
            {
            //
            NW_LMgr_BoxVisitor_t boxVisitor;
            NW_LMgr_Box_t* tempBox = NULL;

			      NW_LMgr_BoxVisitor_Initialize( &boxVisitor, (NW_LMgr_Box_t*)currentBox );
			      while( ( tempBox = NW_LMgr_BoxVisitor_NextBox( &boxVisitor, NULL ) ) != NULL ) 
              {
              //
              if( childBox == tempBox ) 
                {
                tempHasFocus = NW_TRUE;
                break;
                }
				      }
            }
          }
        // Now set the clip and draw 
        deviceContext->SetClipRect( &childClipRect);

        status = NW_LMgr_Box_Draw(childBox, deviceContext, tempHasFocus );
        NW_THROW_ON_ERROR(status);
        }
      }
    else 
      {
      // In the case of partial render first chunk, following render immediately 
      // gotoActiveBox will call render again and we should take advantage of 
      // render cache; after partial render complete, all the scrolling will call
      // render and should render from the cache
      if ( !thisObj->partialRender || 
         (thisObj->partialRender && thisObj->renderingState == NW_LMgr_RootBox_RenderFirstChunk ) )
        {
        // Allocate the render cache 
      thisObj->renderedBoxes = (NW_ADT_DynamicVector_t*)
        NW_ADT_ResizableVector_New(sizeof (NW_LMgr_Box_t*), 10, 5 );
      NW_THROW_OOM_ON_NULL (thisObj->renderedBoxes, status);
      thisObj->renderedClips = (NW_ADT_DynamicVector_t*)
        NW_ADT_ResizableVector_New(sizeof (NW_GDI_Rectangle_t), 10, 5 );
      NW_THROW_OOM_ON_NULL (thisObj->renderedClips, status);

        // We will generate the cache  
        thisObj->renderFromCache = NW_TRUE;
        } 
      else
        {
        thisObj->renderedBoxes = NULL;
        thisObj->renderedClips = NULL;
        
        thisObj->renderFromCache = NW_FALSE;
        }
      // Invoke the superclass to render the tree 
      // rootbox never has focus.
      status =
        BoxRender_Render (box, deviceContext, clipRect, currentBox, 0, NW_FALSE, thisObj);
      _NW_THROW_ON_ERROR (status);

      // Draw any floats 
      floatCount = NW_ADT_Vector_GetSize(thisObj->placedFloats);
      for (index = 0; index < floatCount; index++) 
        {  
        floatBox = *(NW_LMgr_Box_t**)
		    NW_ADT_Vector_ElementAt (thisObj->placedFloats, index);
        NW_ASSERT(floatBox != NULL);

        status = BoxRender_Render (floatBox, deviceContext, clipRect, currentBox,
                                     NW_LMgr_Box_Flags_DrawFloats, NW_FALSE, thisObj);
        _NW_THROW_ON_ERROR (status);
        }

      
      // Start the blink timer 
      if ((thisObj->blinkTimer == NULL) && thisObj->isBlinking) 
        {
          thisObj->blinkTimer = NW_System_Timer_New (NW_LMgr_RootBox_BlinkCallback,
                                                     thisObj, 500, NW_TRUE);
          NW_THROW_OOM_ON_NULL (thisObj->blinkTimer, status);
          thisObj->blinkCount = NW_ISA_BLINK_LIMIT;
        }
      }
    } 
  NW_CATCH (status) 
    {
    } 
  NW_FINALLY 
    {    
    // ReSet the new clip rect 
    deviceContext->SetClipRect( &oldClip);

    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   public/protected final methods
 * ------------------------------------------------------------------------- */

CGDIDeviceContext*
NW_LMgr_RootBox_GetDeviceContext (NW_LMgr_RootBox_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_RootBox_Class));

  if (thisObj->boxTreeListener != NULL) {
    return thisObj->boxTreeListener->GetDeviceContext ();
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t*
NW_LMgr_RootBox_GetCurrentBox (NW_LMgr_RootBox_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_RootBox_Class));

  if (thisObj->boxTreeListener != NULL) {
    return thisObj->boxTreeListener->GetCurrentBox ();
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
void
NW_LMgr_RootBox_BoxTreeDestroyed (NW_LMgr_RootBox_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_RootBox_Class));

  /* Stop any blinking */
  thisObj->isBlinking = NW_FALSE;
  /* Destroy the blink timer */
  if (thisObj->blinkTimer != NULL) {
    NW_Object_Delete (thisObj->blinkTimer);
    thisObj->blinkTimer = NULL;
  }

  if (thisObj->boxTreeListener != NULL) {
    thisObj->boxTreeListener->BoxTreeDestroyed ();
  }

  thisObj->outstandingImageOpenings = 0;

  thisObj->needsRelayout = NW_FALSE;

  thisObj->renderingState = NW_LMgr_RootBox_RenderUninitialized;
  // clear RTL
  thisObj->iIsPageRTL = -1;

  thisObj->iHasGridModeTables = NW_FALSE;

  NW_LMgr_RootBox_InvalidateCache(thisObj);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_RootBox_Refresh(NW_LMgr_RootBox_t* thisObj, NW_LMgr_Box_t* box)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_RootBox_Class));

  if (thisObj->boxTreeListener != NULL) {
    return thisObj->boxTreeListener->Refresh (box);
  }
  return KBrsrSuccess; /* TODO: a better status */
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_RootBox_GetExtents (NW_LMgr_RootBox_t* thisObj,
                            NW_GDI_Dimension2D_t* extents)
{
  NW_LMgr_BoxVisitor_t boxVisitor;
  NW_LMgr_Box_t* box;

  if (!thisObj->calculateExtents) {
    *extents = thisObj->extents.dimension;
  }
  /* In certain cases, this function will be called before the box tree */
  /* is rendered and therefore calculatedExtents will be TRUE.  In these */
  /* cases the extents of the rootbox need to be calculated by traversing the tree */
  else {
    /* create the BoxVisitor on the stack */
    TBrowserStatusCode status =
      NW_LMgr_BoxVisitor_Initialize (&boxVisitor, NW_LMgr_BoxOf (thisObj));
    NW_ASSERT (status == KBrsrSuccess);

    /* initialize the extents */
    (void) NW_Mem_memset (extents, 0, sizeof (*extents));

    /* traverse the box tree and determine the maximum size */
    while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL) 
      {
      NW_GDI_Rectangle_t boxBounds;
      
      // if parentbox or box is Marquee then don't calculate extents
      NW_LMgr_ContainerBox_t* parent = NW_LMgr_Box_GetParent(box);
                  
            // check condition for first box in tree 
      if ( ((parent == NULL) && (!(NW_Object_IsClass (box, &NW_LMgr_MarqueeBox_Class))))
            // check condition for non-first box in tree
        || (parent && !(NW_Object_IsClass (parent, &NW_LMgr_MarqueeBox_Class))
              && !(NW_Object_IsClass (box, &NW_LMgr_MarqueeBox_Class))) )
      {
     	
	      boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
	      if (boxBounds.point.x + boxBounds.dimension.width > extents->width) {
	        extents->width =
	          (NW_GDI_Metric_t) (boxBounds.point.x + boxBounds.dimension.width);
	      }
	      if (boxBounds.point.y + boxBounds.dimension.height > extents->height) {
	        extents->height =
	          (NW_GDI_Metric_t) (boxBounds.point.y + boxBounds.dimension.height);
	      }
      }
    }
    thisObj->extents.dimension = *extents;
    thisObj->calculateExtents = NW_FALSE;
  }

  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */
void
NW_LMgr_RootBox_InvalidateCache(NW_LMgr_RootBox_t *rootBox)
{
  /* Clear the cache vectors */
  if (rootBox->renderedBoxes != NULL) {
    (void) NW_ADT_DynamicVector_Clear(rootBox->renderedBoxes);
    NW_Object_Delete(rootBox->renderedBoxes);
    rootBox->renderedBoxes = NULL;
  }

  if (rootBox->renderedClips != NULL) {
    (void) NW_ADT_DynamicVector_Clear(rootBox->renderedClips);
    NW_Object_Delete(rootBox->renderedClips);
    rootBox->renderedClips = NULL;
  }

  /* Reset the flag */
  rootBox->renderFromCache = NW_FALSE;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBox_t* rootBox)
  {
  NW_Bool isPageRTL = NW_FALSE;

  NW_ASSERT(rootBox != NULL);
  
  // get property from the list only if cache value(rootBox->iIsPageRTL)has not been set.
  if( rootBox->iIsPageRTL == -1 )
    {
    NW_LMgr_Property_t bodyTextDirection;
    bodyTextDirection.value.token = NW_CSS_PropValue_ltr;
    
    if( rootBox->body != NULL ) 
      {
      // Get the text direction. Body box will inherit from <HTML> tag, if it does not have its own setting 
      bodyTextDirection.type = NW_CSS_ValueType_Token;
      (void)NW_LMgr_Box_GetProperty(rootBox->body, NW_CSS_Prop_textDirection, &bodyTextDirection);
      isPageRTL = (NW_Bool)(bodyTextDirection.value.token == NW_CSS_PropValue_rtl);
      
      // set cache
      rootBox->iIsPageRTL = (NW_Int8)isPageRTL;
      }
    }
  else
    {
    isPageRTL = (NW_Bool)rootBox->iIsPageRTL;
    }
  return isPageRTL;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_LMgr_RootBox_GetScrollRange(NW_LMgr_RootBox_t *rootBox,
                                           NW_GDI_Dimension2D_t* scrollRange)
{
  NW_LMgr_BoxVisitor_t* visitor = NULL;
  
  NW_TRY (status) {
  NW_LMgr_Box_t *child = NULL;
  NW_Int32 xLeft;
  NW_Int32 xRight = 0;

  NW_ASSERT(rootBox != NULL);
  xLeft = NW_LMgr_RootBox_GetRightLimit(rootBox);

  if (!rootBox->calculateScrollRange) {
    *scrollRange = rootBox->scrollRange;
  }
  else {
    status = NW_LMgr_RootBox_GetExtents(rootBox, scrollRange);
    NW_THROW_ON_ERROR(status);
    if (NW_LMgr_RootBox_IsPageRTL(rootBox)) {
      visitor = NW_LMgr_ContainerBox_GetBoxVisitor(rootBox);
      NW_THROW_OOM_ON_NULL (visitor, status);
 
      /* Iterate through all children */
      while ((child = NW_LMgr_BoxVisitor_NextBox(visitor, NULL)) != NULL) 
        {
        NW_GDI_Rectangle_t childBoxBounds = NW_LMgr_Box_GetDisplayBounds( child );
        if (childBoxBounds.dimension.width < rootBox->extents.dimension.width) 
          {
          // In some boxes the size is not initialized and they are set to 0.
          // We don't want to use them for calculating the scroll range
          xLeft = ((childBoxBounds.point.x == 0) || (xLeft < childBoxBounds.point.x)) ? xLeft : childBoxBounds.point.x;
          xRight = (xRight > childBoxBounds.point.x + childBoxBounds.dimension.width) ? xRight : childBoxBounds.point.x + childBoxBounds.dimension.width;
        }
      }

      if (xRight > xLeft) {
        scrollRange->width = xRight - xLeft;
      }
    }
    rootBox->calculateScrollRange = NW_FALSE;
    rootBox->scrollRange = *scrollRange;
  }


  } NW_CATCH (status) {
  } NW_FINALLY {
  if (visitor != NULL) {
    NW_Object_Delete(visitor);
  }
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_Int32 NW_LMgr_RootBox_GetRightLimit(NW_LMgr_RootBox_t* rootBox)
{
  NW_Int32 x = 0;
  CGDIDeviceContext* deviceContext;
  NW_ASSERT(rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  if ( deviceContext != NULL) {
    x = (deviceContext->ClipRect()).dimension.width;
  }
  x += RIGHT_ORIGIN;
  return x;
}



/* ------------------------------------------------------------------------- */
/* NW_LMgr_RootBox_ImageOpeningStarted
 *
 * Called by image observers when the 'open' request has been made to the media server
 * The number of outanding requests is kept track
 * of. When the open operations are completed, a relayout or redraw is
 * done in NW_LMgr_RootBox_ImageOpened.
 *
 * Parameters:
 *  image:     this object
 */
void
NW_LMgr_RootBox_ImageOpeningStarted( NW_LMgr_RootBox_t* rootBox )
  {
  NW_ASSERT( rootBox != NULL );

  rootBox->outstandingImageOpenings++;

  }

/* ------------------------------------------------------------------------- */
/* NW_LMgr_RootBox_ImageOpened
 *
 * Called by image observers when the 'open' request to the media server has
 * finished. This object keeps track of
 * how many outstanding requests there are. When all the outstanding 
 * requests are completed, then a relayout or a redraw is done.
 *
 * Parameters:
 *  image:     this object
 */
void
NW_LMgr_RootBox_ImageOpened( NW_LMgr_RootBox_t* rootBox )
  {

  NW_ASSERT( rootBox != NULL );
  NW_ASSERT( rootBox->boxTreeListener != NULL ); 

  rootBox->outstandingImageOpenings--;

  if( rootBox->outstandingImageOpenings == 0 && rootBox->needsRelayout )
    {
    (void) rootBox->boxTreeListener->BoxTreeListenerRelayout( NW_TRUE );
    rootBox->needsRelayout = NW_FALSE;
    }
  }


/* ------------------------------------------------------------------------- */
/* NW_LMgr_RootBox_SetNeedsRelayout
 *
 * Called by image observers when an image box needs relayout
 *
 * Parameters:
 *  image:     this object
 */
void
NW_LMgr_RootBox_SetNeedsRelayout( NW_LMgr_RootBox_t* rootBox )
  {
  NW_ASSERT( rootBox != NULL );
  NW_ASSERT( rootBox->outstandingImageOpenings > 0);

  rootBox->needsRelayout = NW_TRUE;
  }

/* ------------------------------------------------------------------------- */
void
NW_LMgr_RootBox_ExtendsCalculationNeeded( NW_LMgr_RootBox_t* rootBox )
  {
  NW_ASSERT( rootBox != NULL );

  rootBox->calculateExtents = NW_TRUE;
  rootBox->calculateScrollRange = NW_TRUE;
  }

/* ------------------------------------------------------------------------- */
/* NW_LMgr_RootBox_SetFocusBox
 *
 * Sets the focus box for this root box. Used to keep context of the current
 * box for a particular root box. 
 *
 * Parameters:
 *  rootBox:     this object
 *  focusBox:    current active box for this root box
 */
void
NW_LMgr_RootBox_SetFocusBox(NW_LMgr_RootBox_t* rootBox, NW_LMgr_Box_t* focusBox)
  {
  NW_ASSERT(rootBox != NULL);

  rootBox->focusBox = focusBox;
  }

/* ------------------------------------------------------------------------- */
/* NW_LMgr_RootBox_GetFocusBox
 *
 * Called to retrieve the current active box associated with this root box
 *
 * Parameters:
 *  rootBox:     this object
 */
NW_LMgr_Box_t*
NW_LMgr_RootBox_GetFocusBox (NW_LMgr_RootBox_t* rootBox)
  {
  return rootBox->focusBox;
  }

/* ------------------------------------------------------------------------- */
/* NW_LMgr_RootBox_Copy
 *
 * Called to create a copy of a root box given as the first parameter. 
 * Currently this method does a partial copy of the root box, so do
 * not use if you need a full root box copy.
 *
 * Parameters:
 *  aOldRootBox:     the box you want to copy
 *  aNewRootBox:     the partial copy of the first box
 */
TBrowserStatusCode
NW_LMgr_RootBox_Copy (NW_LMgr_RootBox_t* aOldRootBox, 
                      NW_LMgr_RootBox_t** aNewRootBox)
  {
  NW_LMgr_RootBox_t* oldRootBox = aOldRootBox;
  NW_LMgr_RootBox_t* newRootBox = NULL;
//  NW_LMgr_SimplePropertyList_t* oldPropertyList = NULL;
//  NW_LMgr_SimplePropertyList_t* newPropertyList = NULL;

  NW_TRY( status ) 
    {
    // retrieve the current appservices object from the current root box to
    // be used when creating our new root box
    NW_HED_AppServices_t* appServices = NW_LMgr_RootBox_GetAppServices(oldRootBox);

    // create a new root box
    newRootBox = NW_LMgr_RootBox_New (appServices);
    NW_THROW_OOM_ON_NULL (newRootBox, status);
    
    // copy the old image map box property list to the new image map box property list
    //oldPropertyList = (NW_LMgr_SimplePropertyList_t*)NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(oldRootBox));

    //status = NW_LMgr_SimplePropertyList_CopyEntireList(oldPropertyList, &newPropertyList);
    //NW_THROW_ON_ERROR (status);

    //(void)NW_LMgr_Box_SetPropList(NW_LMgr_BoxOf(newRootBox), (NW_LMgr_PropertyList_t*)newPropertyList);
    
    *aNewRootBox = newRootBox;
    }
  NW_CATCH( status ) 
    {
    } 
  NW_FINALLY 
    {
    return status;  
    } 
  NW_END_TRY
  }

void
NW_LMgr_RootBox_SetSmallScreenOn( NW_LMgr_RootBox_t* aRootBox, const NW_Bool aSmallScreenOn )
  {
  NW_ASSERT( aRootBox != NULL );
  //
  aRootBox->iSmallScreenOn = aSmallScreenOn;
  }

void
NW_LMgr_RootBox_SetWrapEnabled( NW_LMgr_RootBox_t* aRootBox, const NW_Bool aWrapEnabled )
  {
  NW_ASSERT( aRootBox != NULL );
  //
  aRootBox->iWrapEnabled = aWrapEnabled;
  }

NW_Bool
NW_LMgr_RootBox_GetSmallScreenOn( NW_LMgr_RootBox_t* aRootBox )
  {
  // make sure it works even if rootbox is null.
  return ( aRootBox ? aRootBox->iSmallScreenOn : NW_Settings_GetVerticalLayoutEnabled() );
  }

NW_Bool
NW_LMgr_RootBox_GetWrapEnabled( NW_LMgr_RootBox_t* aRootBox )
  {
  // make sure it works even if rootbox is null.
  return ( aRootBox ? aRootBox->iWrapEnabled : NW_Settings_GetTextWrapEnabled() );
  }

// If the origin change along y coordinate is more than half the display width
// and if we are not in image map view, we check to see which images are out 
// of display and if they are we we delete the bitmap to save memory
void NW_LMgr_RootBox_OriginChanged( NW_LMgr_RootBox_t* aRootBox, 
                                   NW_GDI_Point2D_t* aOldOrigin )
    {
    NW_HED_AppServices_t* appServices = aRootBox->appServices;

    if (!appServices->browserAppApi.IsImageMapView())
        {
        CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (aRootBox);
        const NW_GDI_Dimension2D_t* displaySize = &(deviceContext->DisplayBounds()->dimension);
        const NW_GDI_Point2D_t* origin = deviceContext->Origin();

        aRootBox->iYScrollChange = (NW_Int32)(aRootBox->iYScrollChange + origin->y - aOldOrigin->y);

        // We try to delete the bitmaps which have gone out of view only if origin changes 
        // more than half the display
        if (aRootBox->iYScrollChange > displaySize->height)
            {
            aRootBox->iYScrollChange = 0;

            // go through the images 
            NW_Uint16 index = 0;
            NW_Uint16 size = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(aRootBox->iOutOfViewListeners));

            for (index = 0; index < size; index++)
                {
                NW_Image_Epoc32Simple_t* simpleImage = *(NW_Image_Epoc32Simple_t**) 
                    NW_ADT_Vector_ElementAt (aRootBox->iOutOfViewListeners, index);

                NW_Image_Epoc32Simple_OutOfView(simpleImage);
                }
            }
        }
    }

TBrowserStatusCode
NW_LMgr_RootBox_AddToOutOfViewList( NW_LMgr_RootBox_t* aRootBox, 
                                   NW_Image_AbstractImage_t* aImage )
    {
    if (NW_ADT_DynamicVector_InsertAt(aRootBox->iOutOfViewListeners,
                                      &aImage, NW_ADT_Vector_AtEnd) == NULL)
        {
        return KBrsrOutOfMemory;
        }
    return KBrsrSuccess;
    }

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_RootBox_t*
NW_LMgr_RootBox_New (NW_HED_AppServices_t* appServices)
{
  return (NW_LMgr_RootBox_t*)
    NW_Object_New (&NW_LMgr_RootBox_Class, 1, appServices);
}
