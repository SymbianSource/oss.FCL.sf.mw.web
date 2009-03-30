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


#include "nw_lmgr_ruleboxi.h"
#include "nw_lmgr_cssproperties.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

/*#define NW_HR_WIDTH_DEFAULT  (NW_GDI_Metric_t)100*/
#define NW_HR_SIZE_DEFAULT   (NW_GDI_Metric_t)2
#define NW_HR_MAX_HEIGHT	   (NW_GDI_Metric_t)10


const
NW_LMgr_RuleBox_Class_t  NW_LMgr_RuleBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_MediaBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_RuleBox_t),
    /* construct                 */ _NW_LMgr_RuleBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_RuleBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_RuleBox_Constrain,
    /* draw                      */ _NW_LMgr_RuleBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ 0
  },
  { /* NW_LMgr_RuleBox           */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_RuleBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  TBrowserStatusCode status;

  /* invoke our superclass constructor */
  status = _NW_LMgr_Box_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
static 
TBrowserStatusCode
NW_LMgr_RuleBox_GetRuleRect(NW_LMgr_RuleBox_t *ruleBox,
                            NW_GDI_Rectangle_t *ruleRect) 
{

  NW_LMgr_Box_t *box = NW_LMgr_BoxOf(ruleBox);
  NW_GDI_Rectangle_t bounds;
  NW_GDI_Rectangle_t rect;
  NW_LMgr_PropertyValue_t widthProp, heightProp, alignProp;
  NW_LMgr_FrameInfo_t margin;

  // Get our bounds
  bounds = NW_LMgr_Box_GetDisplayBounds(box);

  // Get the width for the rule 
  widthProp.integer = bounds.dimension.width;
  (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &widthProp);
  if (widthProp.integer == 0) 
    {
      return KBrsrFailure;
    }
  rect.dimension.width = (NW_GDI_Metric_t)widthProp.integer;
  if (rect.dimension.width > bounds.dimension.width) 
    {
    rect.dimension.width = bounds.dimension.width; 
    } 

  // Check for margins and set the width of the rule accordingly
  NW_LMgr_Box_GetMargins(box, &margin, NULL, ELMgrFrameLeft | ELMgrFrameRight );

  rect.dimension.width = rect.dimension.width - (margin.left + margin.right);

  // Get the height
  heightProp.integer = NW_HR_SIZE_DEFAULT;
  (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &heightProp);
  if (heightProp.integer > NW_HR_MAX_HEIGHT)
  {
    heightProp.integer = NW_HR_MAX_HEIGHT;
  }
  rect.dimension.height = (NW_GDI_Metric_t)heightProp.integer;

  // Get the alignment
  alignProp.token = NW_CSS_PropValue_left;
  (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_textAlign, NW_CSS_ValueType_Token, &alignProp);

  // Now place the rule in the box 
  // TODO: also use CSS align, width, height properties 
  switch (alignProp.token) 
  {
  case NW_CSS_PropValue_center:
    rect.point.x = (NW_GDI_Metric_t)(bounds.point.x +
        ((bounds.dimension.width - rect.dimension.width)/2));
    break;
  case NW_CSS_PropValue_right:
    rect.point.x = (NW_GDI_Metric_t)(bounds.point.x + 
                       bounds.dimension.width - rect.dimension.width);
    break;
  case NW_CSS_PropValue_left:
  default:
    rect.point.x = bounds.point.x;
    break;
  }
  rect.point.y = bounds.point.y;

  *ruleRect = rect;

  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
static 
TBrowserStatusCode
NW_LMgr_RuleBox_GetForegroundColors(NW_LMgr_RuleBox_t *ruleBox,
                                     NW_GDI_Color_t *fgColorTopLeftPtr,
                                     NW_GDI_Color_t *fgColorBottomRightPtr) {
  NW_LMgr_Box_t *box = NW_LMgr_BoxOf(ruleBox);
  TBrowserStatusCode status;
  NW_LMgr_PropertyValue_t ruleProp;

  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_color, 
                                        NW_CSS_ValueType_Color, &ruleProp);
  if (status == KBrsrNotFound) {
    *fgColorTopLeftPtr = NW_LMGR_RULEBOX_COLOR_DGRAY;
    *fgColorBottomRightPtr = NW_LMGR_RULEBOX_COLOR_LGRAY;
  } else if (status == KBrsrSuccess) {
    *fgColorTopLeftPtr = (NW_GDI_Color_t)ruleProp.integer;
    *fgColorBottomRightPtr = (NW_GDI_Color_t)ruleProp.integer;
  } else {
    return status;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static 
TBrowserStatusCode
NW_LMgr_RuleBox_GetBackgroundColor(NW_LMgr_RuleBox_t *ruleBox,
                                     NW_GDI_Color_t *bgColorPtr) {

  NW_LMgr_Box_t *box = NW_LMgr_BoxOf(ruleBox);
  TBrowserStatusCode status;
  NW_LMgr_PropertyValue_t ruleProp;

  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_backgroundColor, 
                                        NW_CSS_ValueType_Color, &ruleProp);
  if (status == KBrsrNotFound) {
    *bgColorPtr = NW_LMGR_RULEBOX_COLOR_BLACK;
  } else if (status == KBrsrSuccess) {
    *bgColorPtr = (NW_GDI_Color_t)ruleProp.integer;
  } else {
    return status;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static 
TBrowserStatusCode
NW_LMgr_RuleBox_DrawRule(NW_LMgr_RuleBox_t *ruleBox,
                          CGDIDeviceContext* deviceContext,
                          NW_GDI_Color_t fgColorTopLeft,
                          NW_GDI_Color_t fgColorBottomRight,
                          NW_GDI_Color_t bgColor) {
  TBrowserStatusCode status;
  NW_GDI_Rectangle_t ruleRect;
  NW_GDI_Point2D_t points[3];
  NW_GDI_Color_t initialFg;
  NW_GDI_Color_t initialBg;
  NW_GDI_PaintMode_t initialPm;
  NW_LMgr_Property_t prop;
  NW_LMgr_Box_t *box = (NW_LMgr_Box_t*)ruleBox;

  // If the box dimensions (ht/width) are zero, we do not want to draw it 
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
  if ((boxBounds.dimension.height == 0) && (boxBounds.dimension.width == 0))
  {
    return KBrsrSuccess;
  }

  // Get the DeviceContext state 
  initialFg = deviceContext->ForegroundColor();
  initialBg = deviceContext->BackgroundColor();
  initialPm = deviceContext->PaintMode();

  status = NW_LMgr_RuleBox_GetRuleRect(ruleBox, &ruleRect);
  if (status != KBrsrSuccess) 
  {
    return status;
  }
  
  // Set the paint mode 
  deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);

  // If the noshade attribute is set, draw a filled rect;
  //   otherwise, draw an empty one 

  (void)NW_LMgr_Box_GetProperty (NW_LMgr_BoxOf(ruleBox), NW_CSS_Prop_noshade, &prop);
  if (prop.value.token == NW_CSS_PropValue_true) 
  {
	(void) bgColor;
    deviceContext->SetForegroundColor( fgColorTopLeft);
    deviceContext->SetBackgroundColor( fgColorTopLeft);
    deviceContext->FillRectangle( &ruleRect);
  } 
  else 
  {
    deviceContext->SetForegroundColor( fgColorBottomRight);
    points[0].x = (NW_GDI_Metric_t)(ruleRect.point.x);
    points[0].y = (NW_GDI_Metric_t)(ruleRect.point.y + ruleRect.dimension.height - 1);
    points[1].x = (NW_GDI_Metric_t)(ruleRect.point.x + ruleRect.dimension.width - 1);
    points[1].y = (NW_GDI_Metric_t)(ruleRect.point.y + ruleRect.dimension.height - 1);
    points[2].x = (NW_GDI_Metric_t)(ruleRect.point.x + ruleRect.dimension.width - 1);
    points[2].y = (NW_GDI_Metric_t)(ruleRect.point.y);
    (void)deviceContext->DrawPolyline( 3, points, NW_FALSE);
    deviceContext->SetForegroundColor( fgColorTopLeft);
    points[1].x = (NW_GDI_Metric_t)(ruleRect.point.x);
    points[1].y = (NW_GDI_Metric_t)(ruleRect.point.y);
    (void)deviceContext->DrawPolyline( 3, points, NW_FALSE);
  }

  // Reset the DeviceContext 
  deviceContext->SetForegroundColor( initialFg);
  deviceContext->SetBackgroundColor( initialBg);
  deviceContext->SetPaintMode ( initialPm);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_RuleBox_Draw (NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus)
{
  NW_GDI_Color_t fgColorTopLeft = 0;
  NW_GDI_Color_t fgColorBottomRight =0;
  NW_GDI_Color_t bgColor=0;
  NW_LMgr_RuleBox_t *ruleBox = NW_LMgr_RuleBoxOf(box);
  TBrowserStatusCode status;

  /* Invoke our superclass draw method first */
  status = NW_LMgr_MediaBox_Class.NW_LMgr_Box.draw (box, deviceContext, hasFocus);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* Get the colors */
  status = NW_LMgr_RuleBox_GetForegroundColors (ruleBox, &fgColorTopLeft, &fgColorBottomRight);
  if (status != KBrsrSuccess) {
    return status;
  }
  status = NW_LMgr_RuleBox_GetBackgroundColor (ruleBox, &bgColor);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* Now draw the rule */
  status = NW_LMgr_RuleBox_DrawRule (ruleBox, deviceContext, 
                                      fgColorTopLeft, fgColorBottomRight, bgColor);
  if (status != KBrsrSuccess) {
    return status;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_RuleBox_Constrain(NW_LMgr_Box_t* box,
                           NW_GDI_Metric_t constraint) 
{
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  boxBounds.dimension.width = constraint;
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_LMgr_RuleBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context) {

  NW_LMgr_PropertyValue_t heightProp;
  NW_REQUIRED_PARAM(context);
  /* Get the height */
  heightProp.integer = NW_HR_SIZE_DEFAULT;
  (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &heightProp);
  if (heightProp.integer > NW_HR_MAX_HEIGHT)
  {
    heightProp.integer = NW_HR_MAX_HEIGHT;
  }
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  boxBounds.dimension.height = (NW_GDI_Metric_t)heightProp.integer;
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

NW_LMgr_RuleBox_t*
NW_LMgr_RuleBox_New ()
{

  return (NW_LMgr_RuleBox_t*)
    NW_Object_New (&NW_LMgr_RuleBox_Class, 0);
}

