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


#include "nw_lmgr_marqueeboxi.h"
#include "nw_lmgr_formatcontext.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_mediabox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_animatedimageaggregatei.h"
#include "nw_lmgr_emptybox.h"
#include "nw_gdi_utils.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_textbox.h"
#include "BrsrStatusCodes.h"

// CONSTANTS

// Max marquee loop count. The value was chosen arbitrarily and limits looping
// to minimize battery drain.
const NW_Uint16 KMarqueeMaxLoopCount = 16;
// Infinite loop count value is the max.
const NW_Uint16 KMarqueeInfiniteLoopCount = KMarqueeMaxLoopCount;


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */

static
TBrowserStatusCode
NW_LMgr_MarqueeBox_TimerCallback (void *timerData);

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_MarqueeBox_CreateTimer (NW_LMgr_MarqueeBox_t* thisObj)
{
  NW_TRY (status) {
    if (thisObj->timer == NULL) {
      thisObj->timer =
        NW_System_Timer_New (NW_LMgr_MarqueeBox_TimerCallback, thisObj,
                             thisObj->marqueeSpeed, NW_TRUE);
      NW_THROW_ON_NULL (thisObj->timer, status, KBrsrOutOfMemory);
    }
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static
void
NW_LMgr_MarqueeBox_DestroyTimer (NW_LMgr_MarqueeBox_t* thisObj) 
{
  if (thisObj->timer != NULL) {
    NW_Object_Delete (thisObj->timer);
    thisObj->timer = NULL;
  }
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_MarqueeBox_StartPosition (NW_LMgr_MarqueeBox_t* thisObj) 
  {
  NW_LMgr_Box_t *marqueeBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_Box_t *parentBox = NW_LMgr_BoxOf(marqueeBox->parent);
  NW_GDI_Point2D_t offset = {0,0};
  NW_GDI_Metric_t marqueeX, parentX;
  NW_GDI_Metric_t marqueeWidth, parentWidth;

  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( parentBox );
  parentX = boxBounds.point.x;
  parentWidth = boxBounds.dimension.width;
  
  boxBounds = NW_LMgr_Box_GetFormatBounds( marqueeBox );
  marqueeX = boxBounds.point.x;
  marqueeWidth = boxBounds.dimension.width;

  NW_TRY(status) 
    {
    if (thisObj->marqueeStyle == NW_CSS_PropValue_alternate) 
      {
      if (thisObj->marqueeDir == NW_CSS_PropValue_ltr) 
        {
        if (marqueeWidth <= parentWidth) 
          {
          offset.x = (NW_GDI_Metric_t)(0 - (marqueeX - parentX));
          } 
        else 
          {
          offset.x = (NW_GDI_Metric_t)((parentWidth - marqueeWidth)
                                       - (marqueeX - parentX));
          }
        } 
      else 
        {
        if (marqueeWidth <= parentWidth) 
          {
          offset.x = (NW_GDI_Metric_t)((parentWidth - marqueeWidth)
                                       - (marqueeX - parentX));
          } 
        else 
          {
          offset.x = (NW_GDI_Metric_t)(0 - (marqueeX - parentX));
          }
        }
      } 
    else 
      { /* NW_CSS_PropValue_slide, NW_CSS_PropValue_scroll, default */

      if (thisObj->marqueeDir == NW_CSS_PropValue_ltr) 
        {
        offset.x = (NW_GDI_Metric_t)(- marqueeWidth - (marqueeX - parentX));
        } 
      else 
        {
        offset.x = (NW_GDI_Metric_t)(parentWidth - (marqueeX - parentX));
        }
      }

    if (offset.x != 0) 
      {
      status = NW_LMgr_Box_Shift(marqueeBox, &offset);
      
      _NW_THROW_ON_ERROR(status);
      }
    }
  NW_CATCH(status) 
    {
    }
  NW_FINALLY 
    {
    return status;
    }
  NW_END_TRY
  }

static void 
NW_LMgr_MarqueeBox_DestroyImageTimers(NW_LMgr_Box_t *marqueeBox)
{
    NW_LMgr_ContainerBox_t* container = NW_LMgr_ContainerBoxOf(marqueeBox);
    NW_ADT_Vector_Metric_t numChildren;
    NW_ADT_Vector_Metric_t index;
    const NW_ADT_Vector_t *children;
    NW_LMgr_Box_t* child;

    children = NW_LMgr_ContainerBox_GetChildren(container);
    numChildren = NW_ADT_Vector_GetSize(children);
   
    for (index = 0; index < numChildren; index++) 
    {
        child = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, index);
        if(NW_Object_IsClass(child, &NW_LMgr_AnimatedImageBox_Class))
        {
            NW_LMgr_AnimatedImageAggregate_t* animatedImageAggregate = NULL;
            animatedImageAggregate = (NW_LMgr_AnimatedImageAggregate_t *)
                    NW_Object_QuerySecondary (NW_LMgr_AnimatedImageBoxOf (child), 
                    &NW_LMgr_AnimatedImageAggregate_Class);    
            if(animatedImageAggregate != NULL)
            {
                NW_LMgr_AnimatedImageAggregate_DestroyTimer (animatedImageAggregate);
            }
        }
    }
}
/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_MarqueeBox_Animate (NW_LMgr_MarqueeBox_t* thisObj) 
  {
  NW_LMgr_Box_t *marqueeBox = NW_LMgr_BoxOf(thisObj);
  NW_LMgr_Box_t *parentBox = NW_LMgr_BoxOf(marqueeBox->parent);
  NW_GDI_Metric_t marqueeX, parentX;
  NW_GDI_Metric_t marqueeWidth, parentWidth;
  NW_GDI_Metric_t marqueeRight, parentRight;
  NW_GDI_Metric_t scrollAmount;
  NW_GDI_Point2D_t offset;
  
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( parentBox );
  parentX = boxBounds.point.x;
  parentWidth = boxBounds.dimension.width;
  
  boxBounds = NW_LMgr_Box_GetDisplayBounds( marqueeBox );
  marqueeX = boxBounds.point.x;
  marqueeWidth = boxBounds.dimension.width;
  
  parentRight = (NW_GDI_Metric_t)(parentX + parentWidth);
  marqueeRight = (NW_GDI_Metric_t)(marqueeX + marqueeWidth);
  
  NW_TRY(status) 
    {    
    if (thisObj->loopFinished) 
      {
      thisObj->loopCount --;

      if (thisObj->marqueeStyle == NW_CSS_PropValue_alternate) 
        {
        if (thisObj->marqueeDir == NW_CSS_PropValue_ltr) 
          {
          thisObj->marqueeDir = NW_CSS_PropValue_rtl;
          }
        else 
          {
          thisObj->marqueeDir = NW_CSS_PropValue_ltr;
          }
        }
      
      if (thisObj->loopCount == 0) 
        {
        NW_LMgr_MarqueeBox_DestroyTimer(thisObj);
        if (thisObj->marqueeStyle == NW_CSS_PropValue_scroll)
          {
          NW_LMgr_MarqueeBox_DestroyImageTimers(marqueeBox);
          }
        }
      else 
        {
        NW_LMgr_MarqueeBox_StartPosition(thisObj);
        boxBounds = NW_LMgr_Box_GetFormatBounds( marqueeBox );
        NW_LMgr_Box_UpdateDisplayBounds( marqueeBox, boxBounds );
        NW_LMgr_Box_Refresh(parentBox);
        }
      
      thisObj->loopFinished = NW_FALSE;
      
      NW_THROW_SUCCESS(status);
      }
    
    if (((thisObj->marqueeStyle) == NW_CSS_PropValue_slide)
      || ((thisObj->marqueeStyle) == NW_CSS_PropValue_alternate)) 
      {
      
      if (thisObj->marqueeDir == NW_CSS_PropValue_ltr) 
        {
        scrollAmount = (NW_GDI_Metric_t)(thisObj->marqueeAmount);
        if (marqueeWidth <= parentWidth) 
          {
          if (marqueeRight + scrollAmount >= parentRight) 
            {
            scrollAmount = (NW_GDI_Metric_t)(parentRight - marqueeRight);
            thisObj->loopFinished = NW_TRUE;
            }
          } 
        else 
          {
          if (marqueeX + scrollAmount >= parentX) 
            {
            scrollAmount = (NW_GDI_Metric_t)(parentX - marqueeX);
            thisObj->loopFinished = NW_TRUE;
            }
          }
        } 
      else 
        {
        scrollAmount = (NW_GDI_Metric_t)(-thisObj->marqueeAmount);
        if (marqueeWidth <= parentWidth) {
          if (marqueeX + scrollAmount <= parentX) 
            {
            scrollAmount = (NW_GDI_Metric_t)(parentX - marqueeX);
            thisObj->loopFinished = NW_TRUE;
            }
          } 
        else 
          {
          if (marqueeRight + scrollAmount <= parentRight) 
            {
            scrollAmount = (NW_GDI_Metric_t)(parentRight - marqueeRight);
            thisObj->loopFinished = NW_TRUE;
            }
          }
        }
      } 
    else 
      { /* NW_CSS_PropValue_scroll, default */
      
      if (thisObj->marqueeDir == NW_CSS_PropValue_ltr) 
        {
        scrollAmount = thisObj->marqueeAmount;
        if (marqueeX + scrollAmount >= parentX + parentWidth) 
          {
          scrollAmount = (NW_GDI_Metric_t)(parentX + parentWidth - marqueeX);
          thisObj->loopFinished = NW_TRUE;
          }
        } 
      else 
        {
        scrollAmount = (NW_GDI_Metric_t)(-thisObj->marqueeAmount);
        if (marqueeX + scrollAmount < parentX - marqueeWidth) 
          {
          scrollAmount = (NW_GDI_Metric_t)(parentX - marqueeWidth - marqueeX);
          thisObj->loopFinished = NW_TRUE;
          }
        }
      }
    
    offset.x = scrollAmount;
    offset.y = 0;
    status = NW_LMgr_Box_Shift(marqueeBox, &offset);
    _NW_THROW_ON_ERROR(status);
    
    // update display bounds
    NW_LMgr_BoxVisitor_t* visitor;
    NW_LMgr_Box_t* child;
    visitor = NW_LMgr_ContainerBox_GetBoxVisitor( marqueeBox );
    NW_THROW_OOM_ON_NULL( visitor, status );
    
    // Invoke update display bound on this box and all children 
    NW_GDI_Rectangle_t bounds;
    while( ( child = NW_LMgr_BoxVisitor_NextBox( visitor, NULL ) ) != NULL ) 
      {    
      bounds = NW_LMgr_Box_GetFormatBounds( child );
      NW_LMgr_Box_UpdateDisplayBounds( child, bounds );
      }
    bounds = NW_LMgr_Box_GetFormatBounds( marqueeBox );
    NW_LMgr_Box_UpdateDisplayBounds( marqueeBox, bounds );
    //
    NW_Object_Delete( visitor );
    // and refresh it
    status = NW_LMgr_Box_Refresh(parentBox);
    _NW_THROW_ON_ERROR(status);
    
    if (thisObj->marqueeStyle == NW_CSS_PropValue_slide) 
      {
      if (thisObj->loopFinished) 
        {
        thisObj->pause = (NW_Uint16)(1000 / thisObj->marqueeSpeed);
        }
      }
    }
  NW_CATCH(status) 
    {
    }
  NW_FINALLY 
    {
    return status;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_LMgr_MarqueeBox_TimerCallback (void *timerData)
{
  NW_LMgr_MarqueeBox_t *thisObj;
  NW_LMgr_RootBox_t *rootBox;

  NW_TRY(status) {

  /* cast timerData to a box */
  thisObj = NW_LMgr_MarqueeBoxOf (timerData);
  NW_ASSERT (thisObj);

  /* make sure it is a MarqueeBox */
  NW_ASSERT (NW_Object_IsClass (thisObj, &NW_LMgr_MarqueeBox_Class));

  /* if the box tree is not visible, we do not draw */
  rootBox = NW_LMgr_Box_GetRootBox (thisObj);
  if (rootBox == NULL) {
      NW_THROW_SUCCESS(status);
  }

  if (!NW_LMgr_RootBox_HasFocus(rootBox)) {
      NW_THROW_SUCCESS(status);
  }

  if (thisObj->pause) {
    thisObj->pause--;
  }
  else {
      status = NW_LMgr_MarqueeBox_Animate(thisObj);
      _NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}


/* ------------------------------------------------------------------------- */
const
NW_LMgr_MarqueeBox_Class_t  NW_LMgr_MarqueeBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_FlowBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_MarqueeBox_t),
    /* construct                 */ _NW_LMgr_MarqueeBox_Construct,
    /* destruct                  */ _NW_LMgr_MarqueeBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_FormatBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_FormatBox_Constrain,
    /* draw                      */ _NW_LMgr_MarqueeBox_Draw,
    /* render                    */ _NW_LMgr_ContainerBox_Render,
    /* getBaseline               */ _NW_LMgr_FlowBox_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox         */
    /* applyFormatProps          */ _NW_LMgr_FlowBox_ApplyFormatProps
  },
  { /* NW_LMgr_FlowBox           */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_BidiFlowBox         */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_MarqueeBox*/
    /* unused                    */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_MarqueeBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp)
{
  /* invoke our superclass constructor */
  return NW_LMgr_BidiFlowBox_Class.NW_Object_Dynamic.construct (dynamicObject, argp);
}

/* ------------------------------------------------------------------------- */
void
_NW_LMgr_MarqueeBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_MarqueeBox_t* thisObj = NW_LMgr_MarqueeBoxOf(dynamicObject);

  /* destroy the timer */
  if (thisObj->timer != NULL) { 
    NW_LMgr_MarqueeBox_DestroyTimer(thisObj);
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_MarqueeBox_Draw(NW_LMgr_Box_t *box,
                         CGDIDeviceContext *deviceContext,
                         NW_Bool hasFocus)
{
  NW_TRY (status) {
  NW_LMgr_MarqueeBox_t* thisObj = NW_LMgr_MarqueeBoxOf(box);
  NW_LMgr_Box_t *parentBox = NW_LMgr_BoxOf(box->parent);
  NW_LMgr_Property_t marqueeLoop;
  NW_LMgr_PropertyValue_t marqueeStyle;
  NW_LMgr_PropertyValue_t marqueeAmount;
  NW_LMgr_PropertyValue_t marqueeDir;
  NW_LMgr_PropertyValue_t textDir;
  NW_LMgr_Property_t marqueeSpeed;

  if (!thisObj->started) {

    /* Get the marquee styles and store them for easy access */
    marqueeStyle.token = NW_CSS_PropValue_scroll;
    NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_marqueeStyle, NW_CSS_ValueType_Token, &marqueeStyle);
    thisObj->marqueeStyle = marqueeStyle.token;

    /* Get text direction Get the direction from the parent's properties. 
       If it is not found, find the direction and set it to the partnt's properties.
       The scrolling direction will be opposite to the text direction
     */
    textDir.token = NW_CSS_PropValue_ltr;
    if (NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(parentBox), 
                                     NW_CSS_Prop_textDirection,
                                     NW_CSS_ValueType_Token, 
                                     &textDir) != KBrsrSuccess) {
    }

    marqueeDir.token = (textDir.token == NW_CSS_PropValue_ltr) ? NW_CSS_PropValue_rtl : NW_CSS_PropValue_ltr;

    NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_marqueeDir,  NW_CSS_ValueType_Token, &marqueeDir);
    thisObj->marqueeDir = marqueeDir.token;

    // Default looping is infinite, which is same behavior as Microsoft I.E.
    // Default is used if loop="0", loop value is blank, invalid or loop
    // attribute is not specified at all.
    thisObj->loopCount = KMarqueeInfiniteLoopCount;
    status = NW_LMgr_Box_GetProperty(parentBox, NW_CSS_Prop_marqueeLoop, &marqueeLoop);
    if (status == KBrsrSuccess) {
      if ((marqueeLoop.type == NW_CSS_ValueType_Integer) 
           && (marqueeLoop.value.integer >= 1)) {
        thisObj->loopCount = (NW_Uint16)marqueeLoop.value.integer;
        if (thisObj->loopCount > KMarqueeMaxLoopCount) {
          thisObj->loopCount = KMarqueeMaxLoopCount;
        }
      }
    }

    marqueeSpeed.type = NW_CSS_ValueType_Token;
    marqueeSpeed.value.token = NW_CSS_PropValue_normal;
    NW_LMgr_Box_GetProperty(parentBox, NW_CSS_Prop_marqueeSpeed, &marqueeSpeed);
    if (marqueeSpeed.type == NW_CSS_ValueType_Token) {
      switch (marqueeSpeed.value.token) {
      case NW_CSS_PropValue_slow:
        thisObj->marqueeSpeed = 400;
        break;
      case NW_CSS_PropValue_normal:
        thisObj->marqueeSpeed = 150;
        break;
      case NW_CSS_PropValue_fast:
        thisObj->marqueeSpeed = 85;
        break;
      }
    }
    else {
      if (marqueeSpeed.value.integer > 0) {
        thisObj->marqueeSpeed = (NW_Uint16)(marqueeSpeed.value.integer);
      		if(thisObj->marqueeSpeed < 85)
      		{
      			thisObj->marqueeSpeed = 85;
      		}
      }
      else {
        thisObj->marqueeSpeed = 200;
      }
    }

    marqueeAmount.integer = 4;
    NW_LMgr_Box_GetPropertyValue(parentBox, NW_CSS_Prop_marqueeAmount,  NW_CSS_ValueType_Px, &marqueeAmount);
    thisObj->marqueeAmount = (NW_GDI_Metric_t)marqueeAmount.integer;

    status = NW_LMgr_ContainerBox_ResizeToChildren(NW_LMgr_ContainerBoxOf(box));
      _NW_THROW_ON_ERROR(status);

    /* We only start the scrolling if loop > 0 */
    if (thisObj->loopCount != 0) {
      /* Place the box in its start position */
      NW_LMgr_MarqueeBox_StartPosition(thisObj);
      
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
      NW_LMgr_Box_UpdateDisplayBounds( box, boxBounds );

      status = NW_LMgr_MarqueeBox_CreateTimer(thisObj);
        _NW_THROW_ON_ERROR(status);
    }

    thisObj->started = NW_TRUE;
  }

  status = NW_LMgr_Box_Class.NW_LMgr_Box.draw(box, deviceContext, hasFocus);

  } NW_CATCH (status) {
  } NW_FINALLY {
  return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_MarqueeBox_Float(NW_LMgr_FlowBox_t* flow, 
                          NW_LMgr_Box_t* box,
                          NW_LMgr_FormatContext_t* context){

  NW_ADT_DynamicVector_t *pendingFloats = context->pendingFloats;
  NW_ADT_DynamicVector_t *placedFloats = context->pendingFloats;

  NW_REQUIRED_PARAM(flow);

  /* If the float context is not set, remove the box */
  if (pendingFloats == NULL || placedFloats == NULL) {
    return KBrsrFailure;
  }

  if (NW_ADT_DynamicVector_InsertAt(pendingFloats, &box, NW_ADT_Vector_AtEnd) == NULL) {
    return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;
}


/* ------------------------------------------------------------------------- */
/*                   PUBLIC METHODS                                          */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_MarqueeBox_Initialize
 */
TBrowserStatusCode
NW_LMgr_MarqueeBox_Initialize(NW_LMgr_MarqueeBox_t *marquee)
{
  NW_TRY (status) {
    NW_LMgr_ContainerBox_t* container = NW_LMgr_ContainerBoxOf(marquee);
    NW_ADT_Vector_Metric_t numChildren;
    NW_ADT_Vector_Metric_t index;
    const NW_ADT_Vector_t *children;
    NW_LMgr_Box_t* child;
    NW_LMgr_ContainerBox_t *newContainer;
    NW_LMgr_PropertyValue_t floatVal;

    marquee->started = NW_FALSE;
    children = NW_LMgr_ContainerBox_GetChildren(container);

    numChildren = NW_ADT_Vector_GetSize(children);
   
    for (index = 0; index < numChildren; index++) {

      child = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, index);

      if (NW_Object_IsInstanceOf(child, &NW_LMgr_FlowBox_Class) ||
          NW_Object_IsInstanceOf(child, &NW_LMgr_StaticTableBox_Class) ||
          NW_Object_IsInstanceOf(child, &NW_LMgr_StaticTableRowBox_Class)) {
      
        floatVal.token = NW_CSS_PropValue_none;
        status = NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatVal);
        NW_THROW_ON (status, KBrsrOutOfMemory);

        if (floatVal.token == NW_CSS_PropValue_none) {

          newContainer = NW_LMgr_ContainerBox_New(0);
          NW_THROW_OOM_ON_NULL (newContainer, status);

          status = NW_LMgr_ContainerBox_AppendChildrenOf(newContainer, NW_LMgr_ContainerBoxOf(child));
          _NW_THROW_ON_ERROR(status);

          status = NW_LMgr_ContainerBox_ReplaceChild(container, child, NW_LMgr_BoxOf(newContainer));
          _NW_THROW_ON_ERROR(status);

          (void) NW_LMgr_Box_SetPropList(NW_LMgr_BoxOf(newContainer), NW_LMgr_Box_PropListGet(child));
          (void) NW_LMgr_Box_SetPropList(child, NULL);
          NW_LMgr_Box_RemoveProperty(NW_LMgr_BoxOf(newContainer), NW_CSS_Prop_display);
          NW_LMgr_Box_RemoveProperty(NW_LMgr_BoxOf(newContainer), NW_CSS_Prop_float);

          NW_Object_Delete(child);
          child = NULL;
        }
      }
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    return KBrsrSuccess;
  } NW_END_TRY
}

NW_LMgr_MarqueeBox_t*
NW_LMgr_MarqueeBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_MarqueeBox_t*)
    NW_Object_New (&NW_LMgr_MarqueeBox_Class, numProperties);
}
