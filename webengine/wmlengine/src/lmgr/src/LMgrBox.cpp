/*
* Copyright (c) 2000 - 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_boxi.h" 
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_marqueebox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_simplepropertylist.h"
#include "nw_text_ucs2.h"
#include "nwx_string.h"
#include "nwx_logger.h"
#include "nwx_math.h"
#include "nw_gdi_utils.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_lmgr_posflowbox.h"
#include "nw_lmgr_verticaltablebox.h"
#include "nw_lmgr_verticaltablerowbox.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "urlloader_urlloaderint.h"
#include "nwx_settings.h"
#include "GDIDeviceContext.h"

#include "nw_lmgr_imgcontainerbox.h"
#include "LMgrMarkerImage.h"
#include "LMgrMarkerText.h"
#include "BrsrStatusCodes.h"
#include "LMgrAnonBlock.h"
#include "nw_css_tokentoint.h"
#include "bitdev.h"
#include "aknutils.h"
#include "LMgrObjectBoxOOC.h"

/* ------------------------------------------------------------------------- *
   private final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_ResolveAbsoluteLength
 * Description:  Resolves a relative length (Cm, Mm, In, Pt, Pc).
 * Returns:      KBrsrSuccess.
 */

static
TBrowserStatusCode
NW_LMgr_Box_ResolveAbsoluteLength(NW_LMgr_Box_t* box, 
                                  NW_LMgr_PropertyName_t propertyName,
                                  NW_LMgr_Property_t *property)
    {
    NW_Float32 val = property->value.decimal;

    NW_REQUIRED_PARAM(box);
    NW_REQUIRED_PARAM(propertyName);

    switch(property->type & NW_CSS_ValueType_Mask)
        {
        case NW_CSS_ValueType_Pc:
            // A pica is equivalent to 12 points
            val = val * 12;

            break;

        case NW_CSS_ValueType_Cm:
            // A centimeter is 28.34 points
            val = val * (NW_Float32)28.34;

            break;

        case NW_CSS_ValueType_Mm:
            // A millimeter is 2.83 points
            val = val * (NW_Float32)2.83;

            break;

        case NW_CSS_ValueType_In:
            // A inch is 72 points
            val = val * 72;

            break;

        case NW_CSS_ValueType_Pt:
        default:
            break;
        }
  
    property->type = NW_CSS_ValueType_Pt;
    property->value.decimal = val;

    return KBrsrSuccess;
    }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_ResolveRelativeLength
 * Description:  Resolves a relative length (Em, Ex).
 * Returns:      KBrsrSuccess.
 */

static
TBrowserStatusCode
NW_LMgr_Box_ResolveRelativeLength(NW_LMgr_Box_t* box, 
                                  NW_LMgr_PropertyName_t propertyName,
                                  NW_LMgr_Property_t *property)
    {
    NW_LMgr_Box_t *baseBox;
    NW_Int32 val;
  
    (void) propertyName;

    // Fontsize uses the parent's em
    if (propertyName == NW_CSS_Prop_fontSize) 
        {
        baseBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(box);

        // This assert is only going to be triggered in the case when
        // there are percentage properties applied to the RootBox.
        // That is an error condition because the RootBox should have
        // no properties.
        NW_ASSERT(baseBox != NULL);
        }
    else 
        {
        baseBox = box;
        }

    // set the absolute font-size
    if (property->type == NW_CSS_ValueType_Em) 
        {
        val = (NW_Int32)(property->value.decimal * NW_LMgr_Box_GetEm(baseBox));
        }
    else 
        {
        val = (NW_Int32)(property->value.decimal * NW_LMgr_Box_GetEx(baseBox));
        }

    if (propertyName == NW_CSS_Prop_fontSize) 
        {
        // we can't use pixels here because in NW_LMgr_Box_ApplyFontProps(),
        // pixel will be adjusted to bigger value 
        CEikonEnv* eikEnv = CEikonEnv::Static();
        CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();
        NW_Float32 valPt = FontUtils::PointsFromTwips(screenDev.VerticalPixelsToTwips(val));

        property->type = NW_CSS_ValueType_Pt;
        property->value.decimal = valPt;
        }
    else
        {
        property->type = NW_CSS_ValueType_Px;
        property->value.integer = val;
        }

    return KBrsrSuccess;
    }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_ResolvePercent
 * Description:  Resolves a relative property (percentage).
 * Returns:      KBrsrSuccess.
 */

static
TBrowserStatusCode
NW_LMgr_Box_ResolvePercent(NW_LMgr_Box_t* box, 
                           NW_LMgr_PropertyName_t propertyName,
						   NW_LMgr_Property_t *property)


    {
    NW_LMgr_Box_t* parentBox;
    NW_GDI_Rectangle_t bounds;
    NW_Float32 val = 0;
    NW_Int32 px = 0;
    NW_LMgr_Property_t parentProp;
    NW_GDI_Dimension2D_t size;
    NW_GDI_Dimension3D_t imgSize;
    TBrowserStatusCode status = KBrsrSuccess;
    NW_Image_AbstractImage_t *image = NULL;

    NW_Int32 fontSizeI = 0;
    NW_Float32 fontSizeD = 0.0;

    NW_LMgr_PropertyValue_t backgroundPropValue;

    val = property->value.decimal;

    switch(propertyName)
        {
        // This property calculate its percentage with respect to the
        // value of the same property of their parent box 
        case NW_CSS_Prop_fontSize:
            {
            // First we need to see if the box is a body box, if so, we need
            // to find what the default CSS font size is.
            NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

            // check for body box
            if( NW_LMgr_RootBox_GetBody( rootBox ) == box)
            {
                // TODO query the CSS table for the default font size
                // We know it is 6.5 points
                fontSizeD = (NW_Float32)(val * 6) / 100;

                property->type = NW_CSS_ValueType_Pt;
                property->value.decimal = fontSizeD;

                (void) NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_fontSize, property);
            }
            else
            {
                // calculate the percentage of the parent box's  font size
                parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(box);

                NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf (parentBox), 
                    NW_CSS_Prop_fontSize, &parentProp);

                switch(parentProp.type)
                    {
                    case NW_CSS_ValueType_Em:
                    case NW_CSS_ValueType_Ex:
                    case NW_CSS_ValueType_In:
                    case NW_CSS_ValueType_Cm:
                    case NW_CSS_ValueType_Mm:
                    case NW_CSS_ValueType_Pt:
                    case NW_CSS_ValueType_Pc:
                        fontSizeD = (val * parentProp.value.decimal) / 100;

                        property->type = parentProp.type;
                        property->value.decimal = fontSizeD;

                        (void) NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_fontSize, property);
                        break;

                    case NW_CSS_ValueType_Px: 
                    default:
                        fontSizeI = (NW_Int32) ((val * parentProp.value.integer) / 100);

                        property->type = parentProp.type;
                        property->value.integer = fontSizeI;

                        (void) NW_LMgr_Box_SetProperty (box, NW_CSS_Prop_fontSize, property);
                        break;
                    }
                }
            return status;
            }

        /* These properties calculate their percentage with repect to the
           value of the same property of the surrounding flow */
	    case NW_CSS_Prop_backgroundPosition_x:
	    case NW_CSS_Prop_backgroundPosition_y:
		    {
			    parentBox = box;
			    bounds = NW_LMgr_Box_GetFormatBounds(parentBox);
			    /* check if background-image is defined */
			     backgroundPropValue.object = NULL;
			     NW_LMgr_FrameInfo_t borderWidthInfo;
			     NW_LMgr_Box_GetBorderWidth(parentBox, &borderWidthInfo, ELMgrFrameAll );
			    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_backgroundImage, NW_CSS_ValueType_Image, &backgroundPropValue);
			    if (backgroundPropValue.object == NULL )
			        {
				    return KBrsrSuccess;
			        }

			    /* check if image is opened */
			    image = (NW_Image_AbstractImage_t*)backgroundPropValue.object;
			    (void) NW_Image_AbstractImage_GetSize(image, &imgSize);
			    if (imgSize.width == 0 || imgSize.height == 0)
			        {
				    return KBrsrSuccess;
			        }
 
			    if(propertyName == NW_CSS_Prop_backgroundPosition_x)
			        {
				    px = (NW_Int32)(val * (bounds.dimension.width - imgSize.width- borderWidthInfo.right
				     - borderWidthInfo.left) / 100);
			        }
			    else //propertyName == NW_CSS_Prop_backgroundPosition_y)
			        {
				    px = (NW_Int32)(val * (bounds.dimension.height - imgSize.height

				     - borderWidthInfo.top - borderWidthInfo.bottom	) / 100);
			        }
			    break;
		        }
        case NW_CSS_Prop_width:
        case NW_CSS_Prop_height:
        case NW_CSS_Prop_padding:
        case NW_CSS_Prop_leftPadding:
        case NW_CSS_Prop_rightPadding:
        case NW_CSS_Prop_topPadding:
        case NW_CSS_Prop_bottomPadding:
        case NW_CSS_Prop_margin:
        case NW_CSS_Prop_leftMargin:
        case NW_CSS_Prop_rightMargin:
        case NW_CSS_Prop_topMargin:
        case NW_CSS_Prop_bottomMargin:
        case NW_CSS_Prop_border:
        case NW_CSS_Prop_leftBorderWidth:
        case NW_CSS_Prop_rightBorderWidth:
        case NW_CSS_Prop_topBorderWidth:
        case NW_CSS_Prop_bottomBorderWidth:
        case NW_CSS_Prop_textIndent:
        case NW_CSS_Prop_marqueeAmount:
            /* First find the enclosing flow */
            do 
                {  
                parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(box);
                
                /* This assert is only going to be triggered in the case when
                * there are percentage properties applied to the RootBox.
                * That is an error condition because the RootBox should have
                * no properties.
                */
                NW_ASSERT(parentBox != NULL);
                
                /* Move one level up */
                box = parentBox;
                
                } while (!NW_Object_IsInstanceOf(parentBox, &NW_LMgr_FlowBox_Class));
            if ((propertyName == NW_CSS_Prop_height || propertyName == NW_CSS_Prop_width) && 
                NW_Object_IsInstanceOf(parentBox, &NW_LMgr_PosFlowBox_Class))
                {
                NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(box);
                CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
                // The width of a positioned box depends on the containing block
                // If the positioned box is nested, the containing box is
                // the parent positioned box, otherwise it is the screen size
                NW_LMgr_PosFlowBox_t* posParent = (NW_LMgr_PosFlowBox_t*)parentBox;
                if (posParent->iContainingBlock == NULL)
                    {
                    bounds = (*deviceContext->DisplayBounds());
                    }
                else
                    {
                    bounds = NW_LMgr_Box_GetFormatBounds(posParent->iContainingBlock);
                    }
                if (propertyName == NW_CSS_Prop_width)
                    {	    
                    NW_LMgr_FrameInfo_t paddingInfo;
                    NW_LMgr_FrameInfo_t borderWidthInfo;
                    
                    NW_LMgr_Box_GetPadding(parentBox, &paddingInfo, ELMgrFrameAll );
                    NW_LMgr_Box_GetBorderWidth(parentBox, &borderWidthInfo, ELMgrFrameAll );
                    NW_GDI_Metric_t containingWidth = (NW_GDI_Metric_t)
                        (bounds.dimension.width - 
                        paddingInfo.left - paddingInfo.right -
                        borderWidthInfo.left - borderWidthInfo.right);
                    
                    px = (NW_Int32)((val * containingWidth) / 100);
                    }
                else // propertyName == NW_CSS_Prop_height
                    { 
                
                    px = (NW_Int32)((val * bounds.dimension.height) / 100);

                    }
                }
            else
                {
                if (propertyName != NW_CSS_Prop_height) 
                    {
                    bounds = NW_LMgr_Box_GetFormatBounds(parentBox); //  width 
                    
                    
                    // S60 bug fix <PHEN-64EEW3>
                    // If the box needs to have x% of parent box's width,
                    // deduct border, padding width from parent box's width first
                    // then calculate the x% of the remaining width
                    if (propertyName == NW_CSS_Prop_width) 
                        {
                        NW_LMgr_FrameInfo_t paddingInfo;
                        NW_LMgr_FrameInfo_t borderWidthInfo;
                        
                        NW_LMgr_Box_GetPadding(parentBox, &paddingInfo, ELMgrFrameAll );
                        NW_LMgr_Box_GetBorderWidth(parentBox, &borderWidthInfo, ELMgrFrameAll );
                        NW_GDI_Metric_t containingWidth = (NW_GDI_Metric_t)
                            (bounds.dimension.width - 
                            paddingInfo.left - paddingInfo.right -
                            borderWidthInfo.left - borderWidthInfo.right);
                        
                        px = (NW_Int32)((val * containingWidth) / 100);
                        }
                    else 
                        {
                        px = (NW_Int32)((val * bounds.dimension.width) / 100);
                        }
                    }
                else // propertyName == NW_CSS_Prop_height

                    { 
                    if (!NW_Object_IsClass(parentBox, &NW_LMgr_StaticTableCellBox_Class) &&
		                !NW_Object_IsClass(parentBox, &NW_LMgr_VerticalTableCellBox_Class)) 
                    {
                        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(box);
                        CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
                        bounds = (*deviceContext->DisplayBounds());
                        NW_LMgr_FrameInfo_t paddingInfo;
                        NW_LMgr_FrameInfo_t borderWidthInfo;
                    
                        NW_LMgr_Box_GetPadding(parentBox, &paddingInfo, ELMgrFrameAll );
                        NW_LMgr_Box_GetBorderWidth(parentBox, &borderWidthInfo, ELMgrFrameAll );
                        NW_GDI_Metric_t containingHeight = (NW_GDI_Metric_t)
                            (bounds.dimension.height - 
                        paddingInfo.top - paddingInfo.bottom -
                        borderWidthInfo.top - borderWidthInfo.bottom);
                        px = (NW_Int32)((val * containingHeight) / 100);

                    }
                    else
                    {


                   status = NW_LMgr_Box_GetSizeProperties(parentBox, &size);
                    if (status != KBrsrSuccess) 
                        {
                        return status;
                        }
                    
                    if (size.height != -1) 
                        {
                        px = (NW_Int32) ((val * size.height) / 100);
                        }
                    else 
                        {
                        return KBrsrNotFound;
                        } 
                    }
                        
                    }
                }
            break;

        default:
          /* In all other cases return the percentage as is (this useful for v-align) */
          return KBrsrSuccess;
      }

      property->type = NW_CSS_ValueType_Px;
      property->value.integer = px;

      return status;
    }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_ResolveRelativeProperty
 * Description:  Resolves a relative property (percentage, em, ex,).
 * Returns:      KBrsrSuccess.
 */

static
TBrowserStatusCode
 NW_LMgr_Box_ResolveRelativeProperty(NW_LMgr_Box_t* box, 
                                    NW_LMgr_PropertyName_t propertyName,
                                    NW_LMgr_Property_t *property)  
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (box);
  NW_ASSERT (property);

  // Remove bit flag so it works for default/variable properties as well
  // as normal ones.
  switch (property->type & NW_CSS_ValueType_Mask)
  {
    case NW_CSS_ValueType_Percentage:
      status = NW_LMgr_Box_ResolvePercent (box, propertyName, property); 
      break;

    case NW_CSS_ValueType_Em:
    case NW_CSS_ValueType_Ex:
      status = NW_LMgr_Box_ResolveRelativeLength (box, propertyName, property);
      break;
    
    case NW_CSS_ValueType_In:
    case NW_CSS_ValueType_Cm:
    case NW_CSS_ValueType_Mm:
    case NW_CSS_ValueType_Pt:
    case NW_CSS_ValueType_Pc:
      status = NW_LMgr_Box_ResolveAbsoluteLength (box, propertyName, property);
      break;
  
    default:
      break;
  }
  
  return status;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_ConvertBorderStyle
 * Description:  Converts border style to GDIContext value
 * Returns:      Converted border style
 */

static
NW_GDI_Pattern_t
NW_LMgr_Box_ConvertBorderStyle(NW_LMgr_PropertyValueToken_t style)
{
  NW_GDI_Pattern_t gdiStyle;
  switch (style)
  {
    case NW_CSS_PropValue_solid:
      gdiStyle = NW_GDI_Pattern_Solid;
      break;

    case NW_CSS_PropValue_dashed:
      gdiStyle = NW_GDI_Pattern_Dashed;
      break;

    case NW_CSS_PropValue_dotted:
      gdiStyle = NW_GDI_Pattern_Dotted;
      break;
  
    default:
      gdiStyle = NW_GDI_Pattern_None;
      break;
  }
  return gdiStyle;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

const
NW_LMgr_Box_Class_t  NW_LMgr_Box_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Dynamic_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_Box_t),
    /* construct                 */ _NW_LMgr_Box_Construct,
    /* destruct                  */ _NW_LMgr_Box_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_Box_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Construct
 * Description:  The constructor.  
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory.
 */

TBrowserStatusCode
_NW_LMgr_Box_Construct (NW_Object_Dynamic_t* dynamicObject,
                        va_list* argp)
{
  NW_TRY (status) 
  {
    NW_LMgr_Box_t* box;
    NW_ADT_Vector_Metric_t initPropCount;

    NW_ASSERT(dynamicObject != NULL);

    /* for convenience */
    box = NW_LMgr_BoxOf (dynamicObject);
   
    /* initialize the member variables */
    initPropCount = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);
    if (initPropCount != 0) 
    {
      box->propList = (NW_LMgr_PropertyList_t*)
        NW_LMgr_SimplePropertyList_New (initPropCount, 2);
      NW_THROW_OOM_ON_NULL (box->propList, status);
    } 
    else 
    {
      box->propList = NULL;
    }

    box->hasFocus = NW_FALSE;
    box->rootBox = NULL;
  } 
  NW_CATCH (status) 
  {
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Destruct
 * Description:  The destructor.  
 * Returns:      void
 */

void
_NW_LMgr_Box_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_Box_t* box;

  NW_ASSERT(dynamicObject != NULL);

  /* for convenience */
  box = NW_LMgr_BoxOf (dynamicObject);

  /* before deleting the object, we must make sure that it is removed from the
     parent */
  TBrowserStatusCode status = NW_LMgr_Box_Detach(box);
  NW_ASSERT(status == KBrsrSuccess);
	/* To fix TI compiler warning */
	(void) status; 
  
  /* delete the properties */
  if (box->propList != NULL) {
    NW_Object_Delete (box->propList);
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Split
 * Description:  Splits the box, storing the split box in splitBox.  
 *               By default, boxes can't split.  If there is enough space for the box,
 *               or if we're at the new line, we return success.  Otherwise 
 *               split fails. 
 * Returns:      KBrsrLmgrSplitOk, KBrsrLmgrNoSplit
 */

TBrowserStatusCode
_NW_LMgr_Box_Split(NW_LMgr_Box_t* box, 
                   NW_GDI_Metric_t space, 
                   NW_LMgr_Box_t* *splitBox,
                   NW_Uint8 flags)
{
  NW_ASSERT(box != NULL);

  if (flags & NW_LMgr_Box_SplitFlags_Nowrap) {
    *splitBox = NULL;
    return KBrsrSuccess;
  }

  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

  if((boxBounds.dimension.width <= space) 
     || (flags & NW_LMgr_Box_SplitFlags_AtNewLine)) { 
    *splitBox = NULL;
    return KBrsrSuccess;
  }
  else {
    *splitBox = NULL;
    return KBrsrLmgrNoSplit;
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Resize
 * Description:  Resizes the box.  By default, boxes resize themselves to match 
                 any CSS size properties. Otherwise they get the default minimum size.
 * Returns:      void
 */

TBrowserStatusCode
_NW_LMgr_Box_Resize (NW_LMgr_Box_t* box,  NW_LMgr_FormatContext_t* context)
{
  NW_REQUIRED_PARAM(context);
  NW_TRY (status) {
    NW_GDI_Dimension2D_t size;

    NW_ASSERT(box != NULL);

    status = NW_LMgr_Box_GetMinimumSize (box, &size);
    _NW_THROW_ON_ERROR (status);

    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

    boxBounds.dimension.width = size.width;  
    boxBounds.dimension.height = size.height;
    NW_LMgr_Box_SetFormatBounds( box, boxBounds );

} NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

TBrowserStatusCode
_NW_LMgr_Box_PostResize (NW_LMgr_Box_t* box)
{
  NW_REQUIRED_PARAM(box);
 
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
 * Function:   NW_LMgr_Box_Constrain
 * Description This method sets an initial size for a box depending on an external
 *             constraint. This is different from resize which knows about box
 *             content size, box size properties, and box minimum size. For
 *             container boxes, resize involves laying out children with size
 *             constrained in one dimension: this method is called before resizing
 *             the box to set the constraint. For other box types, this method
 *             does nothing and returns KBrsrLmgrNotConstrained, indicating
 *             that the box has a fixed size and cannot be arbitrarily
 *             constrained. This method can be called to discover whether a box
 *             obeys constraint arguments or has a fixed size. 
 * Returns:    KBrsrLmgrNotConstrained
 */

TBrowserStatusCode
_NW_LMgr_Box_Constrain(NW_LMgr_Box_t *box,
                       NW_GDI_Metric_t constraint) {
  
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(constraint);

  return KBrsrLmgrNotConstrained;
}


/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetMinimumContentSize
 * Description: This virtual method returns the minimum content size for the
 *              box.  If the WIDTH property is set on the box, this is assumed 
 *              to be the minimum size for the box.  Otherwise, the box returns 
 *              the default values (8,8).
 * Returns:     KBrsrSuccess
 */
 
TBrowserStatusCode
_NW_LMgr_Box_GetMinimumContentSize(NW_LMgr_Box_t *box,
                                   NW_GDI_Dimension2D_t *size){
  NW_LMgr_PropertyValue_t sizeVal;

  NW_REQUIRED_PARAM(box);
  NW_ASSERT(size != NULL);

  /* If the box has its Width/Height properties set, use that for sizing;
   * otherwise return default values (8 by 8).
   */
  NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

  if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
  {
    NW_LMgr_PropertyValue_t visibilityVal;
    TBrowserStatusCode status ;
    visibilityVal.token = NW_CSS_PropValue_visible;
    status = NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                   NW_CSS_ValueType_Token, &visibilityVal);
    if (status  == KBrsrSuccess && visibilityVal.token == NW_CSS_PropValue_hidden) 
    {
      size->width = 0 ;
      size->height = 0;
       return KBrsrSuccess;
    }
  }

  if(NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal) == KBrsrSuccess){
    size->width = (NW_GDI_Metric_t)sizeVal.integer;
  }
  else {
    size->width = NW_LMGR_BOX_MIN_WIDTH;
  }

  if(NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal) == KBrsrSuccess){
    size->height = (NW_GDI_Metric_t)sizeVal.integer;
  }
  else {
    size->height = NW_LMGR_BOX_MIN_HEIGHT;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_HasFixedContentSize
 * Description: Determines whether the content of the box has fixed size or not.
 *              By default, boxes don't have a fixed content size: i.e. they can
 *              be arbitrarily resized by applying a constraint.
 * Returns:     NW_FALSE
 */
 
NW_Bool
_NW_LMgr_Box_HasFixedContentSize(NW_LMgr_Box_t *box){
  NW_REQUIRED_PARAM(box);
  return NW_FALSE;
}

/**
 * Function:     NW_LMgr_Box_Predraw
 * Description:  draw the background for the box, including background color and
 *               background image. 
 *               The latter one includes background-image, background-repeat,
 *               background-attachment and background-position.
 * Parameters:   box --     The box for which the background is to be handled.
 *               deviceContext -- The device context object.
 *               hasFocus -- has the focus or not.
 *
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory
 */

TBrowserStatusCode
NW_LMgr_Box_Predraw (NW_LMgr_Box_t* box,
                     CGDIDeviceContext* deviceContext, 
                     NW_Uint8 hasFocus)
{
  NW_GDI_Color_t initialColor;
  NW_GDI_Color_t initialBackgroundColor;
  NW_GDI_Color_t highlightColor;


  /* parameter assertion block */
  NW_ASSERT (box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

  initialColor = deviceContext->ForegroundColor();
  initialBackgroundColor = deviceContext->BackgroundColor();

  NW_TRY (status) 
    {
    NW_LMgr_Property_t backgroundProp;
    NW_LMgr_Property_t backgroundPropX, backgroundPropY;
    NW_LMgr_PropertyValue_t backgroundPropValue;
    NW_Image_AbstractImage_t *image = NULL; 
    NW_GDI_Rectangle_t rect;                  /* box rectangle */
    NW_GDI_Rectangle_t viewBounds;            /* view rectangle */
    NW_GDI_Point2D_t startDraw;                /* start point for drawing the background image */
    NW_GDI_Point2D_t viewOffset;              /* view offset caused by scrolling */
    NW_GDI_Dimension3D_t size;                /* size of image */
    NW_Bool isFixed = NW_FALSE;               /* if the background-attachment is set to fixed */
    NW_LMgr_RootBox_t* rootBox;
        
    rect = NW_LMgr_Box_GetDisplayBounds(box);

    /* draw the background color */

    backgroundProp.type = NW_CSS_ValueType_Token;
    backgroundProp.value.token = NW_CSS_PropValue_transparent;

    if (NW_Object_IsClass(box, &NW_LMgr_StaticTableCellBox_Class))
      {         
      NW_LMgr_StaticTableCellBox_t *cell = NW_LMgr_StaticTableCellBoxOf(box);
      NW_LMgr_StaticTableCellBox_GetBackground(cell, &backgroundProp);
      } 
    else
      {
      status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_backgroundColor, &backgroundProp);
      }

    if (hasFocus && NW_Object_IsInstanceOf(box, &NW_LMgr_AbstractTextBox_Class))
      {
      /* we want to change the background */
      backgroundProp.type = NW_CSS_ValueType_Color;

      deviceContext->GetHighlightColor( &highlightColor);
      backgroundProp.value.integer = highlightColor;
      }
      
    if (backgroundProp.type == NW_CSS_ValueType_Color) 
      {  
      deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);
      deviceContext->SetBackgroundColor( backgroundProp.value.integer);
      deviceContext->FillRectangle( &rect);
      }
    
    /* draw the background image */

    /* check if background-image is defined */
    backgroundPropValue.object = NULL;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_backgroundImage, NW_CSS_ValueType_Image, &backgroundPropValue);
    if (backgroundPropValue.object == NULL )
      {
      NW_THROW (KBrsrSuccess);
      }

    /* check if image is opened */
    image = (NW_Image_AbstractImage_t*)backgroundPropValue.object;
    (void) NW_Image_AbstractImage_GetSize(image, &size);
    if (size.width == 0 || size.height == 0)
      {
      NW_THROW (KBrsrSuccess);
      }

    /* initialized some drawing coordinates */
    viewOffset = deviceContext->ClipRect().point;
    viewBounds = *(deviceContext->DisplayBounds());
    startDraw = rect.point;
    
    rootBox = NW_LMgr_Box_GetRootBox( box );
    
    /* check for body box */
    if( NW_LMgr_RootBox_GetBody( rootBox ) == box)
      {
      /* to make sure the image can be drawn on the entire view */
      startDraw = viewBounds.point;
      rect.dimension.width = rect.dimension.width + rect.point.x;
      rect.dimension.height =rect.dimension.height + rect.point.y;
      rect.point.x = 0;
      rect.point.y = 0;

      // Note: following is a temporary fix for bug RLAU-5W9LA8
      // the cause of the bug was body box's width not being correctly set by format functions, 
      // instead it's always set to roughly view bounds width
      // A better fix should be in post format functions
      if (rect.dimension.width < viewBounds.dimension.width + viewOffset.x)
        rect.dimension.width = viewBounds.dimension.width + viewOffset.x;
     }
      /* check for background-attachment property */
      backgroundPropValue.token = NW_CSS_PropValue_scroll;
      NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_backgroundAttachment, NW_CSS_ValueType_Token, &backgroundPropValue);
      
      if (backgroundPropValue.token == NW_CSS_PropValue_background_fixed)
        {
        isFixed = NW_TRUE;
        }
 

    /* check and apply background-position property */
    backgroundPropX.type = NW_CSS_ValueType_Token;
    backgroundPropX.value.token = NW_CSS_PropValue_none;
    backgroundPropY.type = NW_CSS_ValueType_Token;
    backgroundPropY.value.token = NW_CSS_PropValue_none;
    NW_LMgr_FrameInfo_t borderWidthInfo; 
    NW_LMgr_Box_GetBorderWidth(box, &borderWidthInfo, ELMgrFrameAll );

    NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_backgroundPosition_x, &backgroundPropX); 
    NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_backgroundPosition_y, &backgroundPropY); 
	if(backgroundPropX.type == NW_CSS_ValueType_Token)
	{
    if((backgroundPropX.value.token == NW_CSS_PropValue_none) && (backgroundPropY.value.token != NW_CSS_PropValue_none))
      {
      backgroundPropX.value.token = NW_CSS_PropValue_center;
      }
	}
	if(backgroundPropY.type == NW_CSS_ValueType_Token)
	{
    if((backgroundPropX.value.token != NW_CSS_PropValue_none) && (backgroundPropY.value.token == NW_CSS_PropValue_none))
      {
      backgroundPropY.value.token = NW_CSS_PropValue_center;
      }
	}

    if(backgroundPropX.type == NW_CSS_ValueType_Token)
      {
      /* if background image needs to be shown as fixed, set the position relative to view bounds instead of box bounds */
      if ((backgroundPropX.value.token == NW_CSS_PropValue_center )
          && size.width <= rect.dimension.width)
        {
        if (isFixed == NW_TRUE)
          startDraw.x += (viewBounds.dimension.width - size.width)/2;
        else 
          startDraw.x += (rect.dimension.width - size.width)/2;
        }
      else if (backgroundPropX.value.token == NW_CSS_PropValue_right)
        {
        if (isFixed == NW_TRUE)
          startDraw.x += (viewBounds.dimension.width - size.width);
        else 
          startDraw.x += (rect.dimension.width - size.width);
        }
      }
    else if ( backgroundPropX.type == NW_CSS_ValueType_Number)
      {
      startDraw.x += (TInt32)backgroundPropX.value.decimal + borderWidthInfo.left;
      }
	else
	{
		startDraw.x += backgroundPropX.value.integer + borderWidthInfo.left;
	}

    if(backgroundPropY.type == NW_CSS_ValueType_Token)
      {
      if ((backgroundPropY.value.token == NW_CSS_PropValue_center) &&
          (size.height <= rect.dimension.height))
        {
        if (isFixed == NW_TRUE)
          startDraw.y += (viewBounds.dimension.height - size.height)/2;
        else
          startDraw.y += (rect.dimension.height - size.height)/2;
        }
      else if (backgroundPropY.value.token == NW_CSS_PropValue_bottom)
        {
        if (isFixed == NW_TRUE)
          startDraw.y += (viewBounds.dimension.height - size.height);
        else
          startDraw.y += (rect.dimension.height - size.height);
        }
      }
    else if ( backgroundPropY.type == NW_CSS_ValueType_Number)
      {
		startDraw.y += (TInt32)backgroundPropY.value.decimal + borderWidthInfo.top;
      }
	else
	{
		startDraw.y += backgroundPropY.value.integer + borderWidthInfo.top;
	}

    /* Apply background-attachment feature only to the body element */
    if ( isFixed == NW_TRUE)
      {
      startDraw.x += viewOffset.x;
      startDraw.y += viewOffset.y;
      }

    /* check and apply background-repeat property */
    backgroundPropValue.token = NW_CSS_PropValue_background_repeat;
    NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_backgroundRepeat, NW_CSS_ValueType_Token, &backgroundPropValue);

    // Draw the image once at startDraw.
    if (backgroundPropValue.token == NW_CSS_PropValue_background_norepeat) 
      {
      /* do no repeat, just draw the image once*/
      status = NW_Image_AbstractImage_DrawInRect (image, deviceContext, &startDraw, NULL, &rect );
      } 

    // If repeat-x, draw a row of images at startDraw.y
    else if (backgroundPropValue.token == NW_CSS_PropValue_background_repeat_x) 
      {
      /* do horizonta repeat */
      /* in case the image is moved by background-position property, restore the start to left */
      if (isFixed == NW_FALSE)
        {
        startDraw.x = rect.point.x;
        }
      else
        {
        startDraw.x = viewBounds.point.x + viewOffset.x;
        }
      /*startDraw.x += deviceContext->ClipRect().point.x;*/

      for (; startDraw.x < rect.point.x + rect.dimension.width; startDraw.x += size.width)
        {
        if (startDraw.x < viewBounds.point.x + viewOffset.x - size.width)
          continue;
        if (startDraw.x > viewBounds.point.x + viewOffset.x + viewBounds.dimension.width)
          break;

        status = NW_Image_AbstractImage_DrawInRect (image, deviceContext, &startDraw, NULL, &rect);
        if (status != KBrsrSuccess)
          break;
        }
      }

    // If repeat-y, draw a column of images at startDraw.x.
    else if (backgroundPropValue.token == NW_CSS_PropValue_background_repeat_y) 
      {
      /* do verticall repeat */
      /* in case the image is  moved by background-position property, restore the start to top */
      if (isFixed == NW_FALSE)
        {
        startDraw.y = rect.point.y;
        } 
      else
        {
        startDraw.y = viewBounds.point.y + viewOffset.y;
        }

      for (; startDraw.y < rect.point.y + rect.dimension.height; startDraw.y += size.height)
        {
        /* check to make sure only draw the images in the view */
        if (startDraw.y < viewBounds.point.y + viewOffset.y - size.height)
          continue;
        if (startDraw.y > viewBounds.point.y + viewOffset.y + viewBounds.dimension.height)
          break;

        status = NW_Image_AbstractImage_DrawInRect (image, deviceContext, &startDraw, NULL, &rect);
        if (status != KBrsrSuccess)
          break;
        }
      }

    // If repeating in both x and y directions and the image is a "dot"
    // stretch the image to fill the box.  This speeds up rendering considerably.
    else if ((size.width == 1) && (size.height == 1))
      {
      NW_GDI_Rectangle_t    realViewBounds;
      NW_GDI_Rectangle_t    visibleArea;
      NW_GDI_Dimension3D_t  scaledSize;

      // Set the view rectangle
      realViewBounds = viewBounds;
      realViewBounds.point = viewOffset;

      // Get the visible part of the box on which the image is stretched.
      if( !NW_GDI_Rectangle_Cross( &realViewBounds, &rect, &visibleArea ) )
        {
        // the box is not in the view
        NW_THROW_SUCCESS( status );
        }

      scaledSize.width = visibleArea.dimension.width;
      scaledSize.height = visibleArea.dimension.height;
      scaledSize.depth = 0; 

      // Draw the image to fill scaledSize.
      status = NW_Image_AbstractImage_DrawScaled( image, deviceContext, &startDraw, &scaledSize );
      }

    // Otherwise draw a grid of iamges.
    else  
      {
      NW_GDI_Point2D_t  boxBegin;
      NW_GDI_Point2D_t  boxEnd;
      NW_GDI_Point2D_t  point;
      NW_Int32          x;
      NW_Int32          y;

      // Normalize the box's rectangle in the view.
      boxBegin.x = rect.point.x;
      boxBegin.y = rect.point.y;

      boxEnd.x = boxBegin.x + rect.dimension.width;
      boxEnd.y = boxBegin.y + rect.dimension.height;

      // IsFixed is only true when "background-attachment: fixed" is applied to 
      // a "body" box.  In this case place the first image at the top-left corner
      // of the view.
      if (isFixed)
        {
        startDraw.x = viewOffset.x;
        startDraw.y = viewOffset.y;
        }
      
      // Otherwise place the first image at the top-left corner of the box.
      else
        {
        startDraw.x = boxBegin.x;
        startDraw.y = boxBegin.y;
        }

      // Draw columns that overlap the box's rect.
      for (x = startDraw.x; x < boxEnd.x; x += size.width)
        {
        // Skip columns to the left of the box's rectangle.
        if ((x + size.width) < boxBegin.x)
          {
          continue;
          }

        // Draw cells that overlap the box's box.
        for (y = startDraw.y; y < boxEnd.y; y += size.height)
          {
          // Skip cells above box's rectangle.
          if ((y + size.height) < boxBegin.y)
            {
            continue;
            }

          // Draw the cell.
          point.x = x; 
          point.y = y;

          status = NW_Image_AbstractImage_DrawInRect(image, deviceContext, &point, NULL, &rect);
          _NW_THROW_ON_ERROR(status);
          }
        }
      }
    }
      
  NW_CATCH (status) 
    {
    }
     
  NW_FINALLY 
    {
    deviceContext->SetForegroundColor( initialColor);
    deviceContext->SetBackgroundColor( initialBackgroundColor);
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_Draw
 * Description: The base class draw only draws the borders and the background.
 * Returns:     KBrsrSuccess, KBrsrOutOfMemory.
 */
 
TBrowserStatusCode
_NW_LMgr_Box_Draw (NW_LMgr_Box_t* box,
                   CGDIDeviceContext* deviceContext,
                   NW_Uint8 hasFocus)
  {
  NW_GDI_PaintMode_t oldPaintMode;
  NW_Bool paintModeChanged = NW_FALSE;
  NW_TRY (status) 
    {
    NW_LMgr_FrameInfo_t borderWidth;
    NW_LMgr_FrameInfo_t borderStyle;
    NW_LMgr_FrameInfo_t borderColor;
    NW_LMgr_PropertyValue_t displayVal;
    NW_LMgr_PropertyValue_t focusBehavior;
    NW_Bool getBorder = NW_FALSE;
    NW_LMgr_FrameInfo_t origBorderColor;

    /* parameter assertion block */
    NW_ASSERT (box != NULL);
    NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));
    NW_ASSERT (deviceContext != NULL);

    NW_LMgr_Box_Predraw(box, deviceContext, hasFocus);
    /* Check if display is actually a displayable one */
    displayVal.token = NW_CSS_PropValue_display_inline; 
    (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_display,
                                         NW_CSS_ValueType_Token, &displayVal);
    if (displayVal.token == NW_CSS_PropValue_none) 
      {
      return KBrsrSuccess; 
    }

    // save paint mode
    oldPaintMode = deviceContext->PaintMode();
    deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);
    paintModeChanged = NW_TRUE;

    NW_LMgr_Box_GetBorderStyle (box, &borderStyle, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderColor (box, &origBorderColor);

    // do not check color and width unless border is present.
    if( !( borderStyle.top == NW_CSS_PropValue_none || borderStyle.top == NW_CSS_PropValue_hidden || origBorderColor.top == NW_GDI_Color_Transparent ) ||
        !( borderStyle.bottom == NW_CSS_PropValue_none || borderStyle.bottom == NW_CSS_PropValue_hidden || origBorderColor.bottom == NW_GDI_Color_Transparent) ||
        !( borderStyle.left == NW_CSS_PropValue_none || borderStyle.left == NW_CSS_PropValue_hidden || origBorderColor.left == NW_GDI_Color_Transparent ) ||
        !( borderStyle.right == NW_CSS_PropValue_none || borderStyle.right == NW_CSS_PropValue_hidden || origBorderColor.right == NW_GDI_Color_Transparent ) )
      {
      getBorder = NW_TRUE;
      NW_LMgr_Box_GetBorderColor (box, &borderColor);
      NW_LMgr_Box_GetBorderWidth (box, &borderWidth, ELMgrFrameAll );
      }
    else
      {
      // set border color to white
      borderColor.bottom = borderColor.left = 
        borderColor.right = borderColor.top = NW_GDI_Color_White;
      // and the style to none
      borderWidth.bottom = borderWidth.left = 
        borderWidth.right = borderWidth.top = 0;
      }

    /* Modify the look of the box if we're in focus */
    if (hasFocus)
      {
      focusBehavior.token = NW_CSS_PropValue_none;
      (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_focusBehavior,
                                           NW_CSS_ValueType_Token,
                                           &focusBehavior);

      if (focusBehavior.token == NW_CSS_PropValue_growBorder) 
        {        
        // get the border color and style unless we already have them.
        if( getBorder == NW_FALSE )
          {
          NW_LMgr_Box_GetBorderColor (box, &borderColor);
          NW_LMgr_Box_GetBorderWidth (box, &borderWidth, ELMgrFrameAll );
          }
        borderWidth.top = (NW_GDI_Metric_t)(borderWidth.top + 1);
        borderWidth.bottom = (NW_GDI_Metric_t)(borderWidth.bottom + 1);
        borderWidth.left = (NW_GDI_Metric_t)(borderWidth.left + 1);
        borderWidth.right = (NW_GDI_Metric_t)(borderWidth.right + 1);

        if (origBorderColor.top == NW_GDI_Color_Transparent) 
          {
          borderStyle.top = NW_CSS_PropValue_solid;
          borderColor.top = deviceContext->ForegroundColor();
          }
        if (origBorderColor.bottom == NW_GDI_Color_Transparent) 
          {
          borderStyle.bottom = NW_CSS_PropValue_solid;
          borderColor.bottom = deviceContext->ForegroundColor();
          }
        if (origBorderColor.left == NW_GDI_Color_Transparent) 
          {
          borderStyle.left = NW_CSS_PropValue_solid;
          borderColor.left = deviceContext->ForegroundColor();
          }
        if (origBorderColor.right == NW_GDI_Color_Transparent) 
          {
          borderStyle.right = NW_CSS_PropValue_solid;
          borderColor.right = deviceContext->ForegroundColor();
          }
        if ((borderStyle.top == NW_CSS_PropValue_none) || (borderStyle.top == NW_CSS_PropValue_hidden)) 
          {
          borderStyle.top = NW_CSS_PropValue_solid;
          }
        if ((borderStyle.bottom == NW_CSS_PropValue_none) || (borderStyle.bottom == NW_CSS_PropValue_hidden)) 
          {
          borderStyle.bottom = NW_CSS_PropValue_solid;
          }
        if ((borderStyle.left == NW_CSS_PropValue_none) || (borderStyle.left == NW_CSS_PropValue_hidden)) 
          {
          borderStyle.left = NW_CSS_PropValue_solid;
          }
        if ((borderStyle.right == NW_CSS_PropValue_none) || (borderStyle.right == NW_CSS_PropValue_hidden)) 
          {
          borderStyle.right = NW_CSS_PropValue_solid;
          }
        }
      }

    /* TODO: if cache is empty, ignore CSS imposed by "a:visited" pseudo-class,
             but for now we just fix the color */

    /* Check for link by looking at parent container. */
    if (NW_LMgr_Box_GetParent(box) != NULL && NW_Object_IsClass(NW_LMgr_Box_GetParent(box), &NW_LMgr_ActiveContainerBox_Class))
     {
      /* If Cache is empty, force border to be drawn in uncached color (blue).
         NOTE: GetNumEntries allocates a list of the cached files, so avoid it for non-links. */
      //if (UrlLoader_GetNumCacheEntries() == 0)
      //  {
      //  borderColor.top    = NW_GDI_Color_Blue;
      //  borderColor.bottom = NW_GDI_Color_Blue;
      //  borderColor.left   = NW_GDI_Color_Blue;
      //  borderColor.right  = NW_GDI_Color_Blue;
      //  }
      }
    /* draw the border */
    /* TODO add support for other border types */
    status = NW_LMgr_Box_DrawBorder( NW_LMgr_Box_GetDisplayBounds(box), deviceContext, &borderWidth, &borderStyle, &borderColor);
    _NW_THROW_ON_ERROR(status);
    }
  NW_CATCH (status) 
    {
    } 
  NW_FINALLY 
    {
    if( paintModeChanged == NW_TRUE )
      {
      // reset paint mode
      deviceContext->SetPaintMode(  oldPaintMode );
      }
    return status;
    } 
  NW_END_TRY
  }

/* -------------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Update_RenderCache
 * Description:  Updates the render cache  
 * Returns:      NW_TRUE, NW_FALSE
 */

NW_Bool NW_LMgr_Box_Update_RenderCache (NW_LMgr_RootBox_t* rootBox, 
                                     NW_LMgr_Box_t* box,
                                     NW_GDI_Rectangle_t* realClip,
                                     void ***ptr)
{
    NW_Bool cachePresent = NW_FALSE; 
    if (rootBox->renderedBoxes && rootBox->renderedClips )
        {
        cachePresent = NW_TRUE;
        NW_ADT_Vector_Metric_t index =  NW_ADT_Vector_GetElementIndex(rootBox->renderedBoxes, &box);
        if (index == NW_ADT_Vector_AtEnd)
            {
            *ptr = NW_ADT_DynamicVector_InsertAt(rootBox->renderedBoxes, &box, NW_ADT_Vector_AtEnd);
            if (*ptr == NULL)
                return cachePresent;
            *ptr = NW_ADT_DynamicVector_InsertAt(rootBox->renderedClips, realClip, NW_ADT_Vector_AtEnd);
            if (*ptr == NULL)
                return cachePresent;
            }
        else
            {
            *ptr = NW_ADT_DynamicVector_ReplaceAt(rootBox->renderedBoxes, &box, index);
            if (*ptr == NULL)
                return cachePresent;

            *ptr = NW_ADT_DynamicVector_ReplaceAt(rootBox->renderedClips, realClip, index);
            if (*ptr == NULL)
                return cachePresent;
            }
        }
    return cachePresent;
}

/* -------------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Render
 * Description:  Renders the box.  
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory
 */

TBrowserStatusCode
_NW_LMgr_Box_Render (NW_LMgr_Box_t* box,
                     CGDIDeviceContext* deviceContext,
                     NW_GDI_Rectangle_t* clipRect,
                     NW_LMgr_Box_t* currentBox,
                     NW_Uint8 flags, 
                     NW_Bool* hasFocus,
                     NW_Bool* skipChildren,
                     NW_LMgr_RootBox_t* rootBox )
{
  NW_GDI_Rectangle_t visibleArea;
  NW_LMgr_PropertyValue_t floatVal;
  NW_LMgr_PropertyValue_t visibilityVal;
  NW_GDI_Rectangle_t viewBounds;  
  NW_GDI_Rectangle_t oldClip = {{0, 0}, {0, 0}};  
  NW_GDI_Rectangle_t realClip;  
  NW_Bool clipRectChanged = NW_FALSE;
  void** ptr = NULL;

  NW_TRY (status) 
    {    
    /* Should we draw floats? */
    if (!(flags & NW_LMgr_Box_Flags_DrawFloats)) 
      {
      floatVal.token = NW_CSS_PropValue_none;
      (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_float,
                                           NW_CSS_ValueType_Token, &floatVal);
      if (floatVal.token != NW_CSS_PropValue_none) 
        {
        NW_THROW_SUCCESS (status);
        }
      }
    // check if the this box has focus
    // if hasFocus is already set to true by any parent
    // then it should not be overuled.
    if( !(*hasFocus) && currentBox == box )
      {
      *hasFocus = NW_TRUE;
      }

    /* Calculate the view bounds */
    viewBounds = *(deviceContext->DisplayBounds());
    viewBounds.point = *(deviceContext->Origin());
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );

    /* Calculate tree extents first */
    if (!NW_GDI_Rectangle_Cross(clipRect, &boxBounds, &visibleArea)) 
      {
      NW_THROW_SUCCESS (status);
      }
    /* Is the box visible on screen?  If not, we don't draw */
    if (!NW_GDI_Rectangle_Cross(&viewBounds, &visibleArea, NULL)) 
      {
      NW_THROW_SUCCESS (status);
      }
    /* Calculate the part of the clip that is visible on screen;
       if the clip is off-screen, don't draw */
    if (!NW_GDI_Rectangle_Cross(clipRect, &viewBounds, &realClip)) 
      {
      NW_THROW_SUCCESS (status);
      }

    /* Save the old clip rect */
    oldClip = deviceContext->ClipRect();
    /* Set the new clip rect */
    deviceContext->SetClipRect(  &realClip );
    clipRectChanged = NW_TRUE;

    /* Check if visibility val is not hidden */
    visibilityVal.token = NW_CSS_PropValue_visible;
    (void) NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                         NW_CSS_ValueType_Token, &visibilityVal);
    if ((visibilityVal.token == NW_CSS_PropValue_hidden) ||
        (visibilityVal.token == NW_CSS_PropValue_collapse)) 
      {
      NW_THROW_SUCCESS(status);
      }

    /* make the box draw itself */
    status = NW_LMgr_Box_Draw (box, deviceContext, *hasFocus);
    _NW_THROW_ON_ERROR (status);

    /* Add the box and the clip to the cache */
    NW_Bool cachePresent = NW_LMgr_Box_Update_RenderCache (rootBox, box, &realClip, &ptr);
    if (cachePresent)
        {
      NW_THROW_OOM_ON_NULL(ptr, status);
      }
    } 
  NW_CATCH (status) 
    {
    } 
  NW_FINALLY 
    {
    // box has no child
    *skipChildren = NW_FALSE;

    if( clipRectChanged == NW_TRUE )
      {
      /* Reset the clip rect */
      deviceContext->SetClipRect( &oldClip);
      }
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetBaseline
 * Description: Returns the baseline of a box.  The default implementation
 *              returns the bottom of the box content area as the baseline.
 * Returns:     KBrsrSuccess, KBrsrOutOfMemory.
 */
 
NW_GDI_Metric_t
_NW_LMgr_Box_GetBaseline(NW_LMgr_Box_t* box){

  NW_ASSERT(box != NULL);

  return (NW_GDI_Metric_t)(NW_LMgr_Box_GetFormatBounds(box).dimension.height);
}

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_Shift
 * Description: Shifts the contents of the box by a given offset.  The default
 *              implementation simply modifies the box boundaries.
 * Returns:     KBrsrSuccess
 */
 
TBrowserStatusCode
_NW_LMgr_Box_Shift (NW_LMgr_Box_t* box, NW_GDI_Point2D_t *delta){

  NW_ASSERT(box != NULL);

  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  boxBounds.point.x = (NW_GDI_Metric_t)(boxBounds.point.x + delta->x);
  boxBounds.point.y = (NW_GDI_Metric_t)(boxBounds.point.y + delta->y);
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Clone
 * Description:  Clone returns a "deep" copy of the box, where the actual definition of
 *               "deep" is up to the individual box type.  The default implementation
 *               fails.  Only the ContainerBox and its descendants have
 *               meaningful implementations at present, because they are the only
 *               ones that need it.  TODO:  Fix this.
 * Returns:      NULL.
 */

NW_LMgr_Box_t*
_NW_LMgr_Box_Clone (NW_LMgr_Box_t* box){

  NW_REQUIRED_PARAM(box);
  NW_ASSERT(0);

  return NULL;
}

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetRawProperty
 * Description: Tries to get a property from the box's own property list.
 *              Property inheritance is ignored.
 * Returns:     KBrsrSuccess, KBrsrNotFound.
 */
 
TBrowserStatusCode
NW_LMgr_Box_GetRawProperty(NW_LMgr_Box_t *box,
                           NW_LMgr_PropertyName_t name,
                           NW_LMgr_Property_t* property)
{
  TBrowserStatusCode status;
  NW_LMgr_Property_t origProperty;

  NW_ASSERT(box != NULL);

  if(box->propList != NULL){
    status = NW_LMgr_PropertyList_Get(box->propList, name, &origProperty);
    if (status != KBrsrSuccess) {
      return KBrsrNotFound;
    }

    property->type = (NW_LMgr_PropertyValueType_t)(origProperty.type & NW_CSS_ValueType_Mask);
    property->value = origProperty.value;

    status = KBrsrSuccess;
  }
  else {
    status = KBrsrNotFound;
  }

  return status;
}

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetPropertyFromList
 * Description: Tries to get a property from the box's own property list.
 *              Property inheritance is ignored.
 * Returns:     KBrsrSuccess, KBrsrNotFound.
 */
 
TBrowserStatusCode
NW_LMgr_Box_GetPropertyFromList(NW_LMgr_Box_t* box,
                                NW_LMgr_PropertyName_t name,
                                NW_LMgr_Property_t* property) 
  {
  TBrowserStatusCode status;
  NW_LMgr_Property_t origProperty;
  NW_Bool specialProperty, isMargin = NW_FALSE, isMarker = NW_FALSE;

  NW_ASSERT(box != NULL);

  if( box->propList )
    {
    // In Vertical Layout Mode, table, tr, td, th need to ignore margin, padding,
    // border, whitespace. This special handling is done in 
    // NW_LMgr_VerticalTableBox_GetPropertyFromList.
    // For rest of the properties (specialProperty flag is false) we handle them
    // the normal way
    if (NW_Object_IsClass(box, &NW_LMgr_VerticalTableBox_Class) ||
        NW_Object_IsClass(box, &NW_LMgr_VerticalTableCellBox_Class) ||
        NW_Object_IsClass(box, &NW_LMgr_VerticalTableRowBox_Class))
      {
      status = NW_LMgr_VerticalTableBox_GetPropertyFromList(box, name, property, &specialProperty);
      if (specialProperty)
        {
        return status;
        }
      }
    
    if(NW_Object_IsInstanceOf(box, &LMgrMarkerImage_Class) || NW_Object_IsInstanceOf(box, &LMgrMarkerText_Class))
      {
      isMarker = NW_TRUE;
      }
    if( NW_LMgr_RootBox_GetSmallScreenOn( NW_LMgr_Box_GetRootBox( box ) ) )
      {
      if((name == NW_CSS_Prop_margin) || (name == NW_CSS_Prop_leftMargin) ||
         (name == NW_CSS_Prop_rightMargin) || (name == NW_CSS_Prop_topMargin) ||
         (name == NW_CSS_Prop_bottomMargin) )
        {
        isMargin = NW_TRUE;
        if( NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class) && !isMarker)
          {
          return KBrsrNotFound;
          }
        }
      }
    
    status = NW_LMgr_PropertyList_Get(box->propList, name, &origProperty);
    if (status != KBrsrSuccess) 
      {
      return KBrsrNotFound;
      }
    
    if(isMargin && !isMarker)
      {
      if(origProperty.value.integer < 0)
        {
        property->value.integer = 0;
        return KBrsrSuccess;
        }
      }
    
    *property = origProperty;
    status = NW_LMgr_Box_ResolveRelativeProperty (box, name, property);
    if (status != KBrsrSuccess) 
      {
      return KBrsrNotFound;
      }
    
    property->type = (NW_LMgr_PropertyValueType_t)(property->type & NW_CSS_ValueType_Mask);
    
    status = KBrsrSuccess;
    }
  else 
    {
    status = KBrsrNotFound;
    }

  return status;
  }

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetProperty
 * Description: Tries to get a property from the box's own property list.
 *              If the property is not found, and the property is inherited,
 *              it queries the ancestors for it.
 * Returns:     KBrsrSuccess, KBrsrNotFound.
 */
 
TBrowserStatusCode
NW_LMgr_Box_GetProperty (NW_LMgr_Box_t* box, 
                         NW_LMgr_PropertyName_t name, 
                         NW_LMgr_Property_t* property)
  {
  NW_LMgr_Box_t* childBox = box;
  NW_Bool lookingForInheritProp = NW_FALSE;
  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

  do 
    {
    TBrowserStatusCode status;
    NW_LMgr_Property_t p;

    status = NW_LMgr_Box_GetPropertyFromList(box, name, &p);
    if( status == KBrsrSuccess ) 
      {
      if( p.type != NW_CSS_ValueType_Token ||
          p.value.token != NW_CSS_PropValue_inherit) 
        {
        // if we are looking for inherit property, then
        // first make sure that the property value did not get resolved
        // while calling NW_LMgr_Box_GetPropertyFromList on the parent box.
        // if it was modified, then we need to take a look at the 
        // original type to check if resolving the value is appropriate here.
        // for examp: parent has 50% as height which gets resolved to 100px,
        // so then we get the inherited value as 100px instead of the correct 
        // 50% (50px) 
        if( lookingForInheritProp == NW_TRUE && box->propList )
          {
          NW_LMgr_Property_t origProperty;
          status = NW_LMgr_PropertyList_Get( box->propList, name, &origProperty );
          if( status == KBrsrSuccess && origProperty.type != p.type )
            {
            // resolve the original value on the child now
            status = NW_LMgr_Box_ResolveRelativeProperty( childBox, name, &origProperty);
            if( status == KBrsrSuccess )
              {
              *property = origProperty;
              property->type =
                  (NW_LMgr_PropertyValueType_t) (property->type & NW_CSS_ValueType_Mask);
              return KBrsrSuccess;
              }
            }
          }
        *property = p;
        property->type =
            (NW_LMgr_PropertyValueType_t) (property->type & NW_CSS_ValueType_Mask);
        return KBrsrSuccess;
        }
      } 
    else 
      {
      if (!(name & NW_CSS_PropInherit)) 
        {
        // drop out and return "not found" 
        break; 
        }
      }
    // The prop is inherit 
    lookingForInheritProp = NW_TRUE;
    box = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent (box);
    if( !box ) 
      {
      break;
      }
    // note: Allow reading direction property from anonymous box 
    if (name != NW_CSS_Prop_textDirection) 
      {
      // Check if the parent is an anonymous box 
      if (NW_Object_IsInstanceOf(box, &LMgrAnonBlock_Class)) 
        {
          // this parent is an anonymous box, we should go up to see if grandpa has 
          // the prop we are interested. 
          box = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent( box );
        }
      }
    } while( box );
    
  // the property was not found 
  return KBrsrNotFound;
  }

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetPropertyValue
 * Description: Same as GetProperty, only the type of the property is not returned.
 * Returns:     KBrsrSuccess, KBrsrNotFound.
 */

/* TODO: This method should go away.  We should always query for the property type.
 */

TBrowserStatusCode
NW_LMgr_Box_GetPropertyValue (NW_LMgr_Box_t *box, 
                              NW_LMgr_PropertyName_t name, 
                              NW_LMgr_PropertyValueType_t type,
                              NW_LMgr_PropertyValue_t *value)
{
  NW_LMgr_Property_t property;
  TBrowserStatusCode status;
  NW_LMgr_RootBox_t* rootBox;
  NW_Bool flag;
  NW_Bool inMarquee = NW_FALSE;

  NW_ASSERT(box != NULL);

  status = NW_LMgr_Box_GetProperty (box, name, &property);
  if (status != KBrsrSuccess) {
    return status;
  }

  if (type == (property.type & NW_CSS_ValueType_Mask)) 
    {
    *value = property.value;
    rootBox = NW_LMgr_Box_GetRootBox( box );
    flag = NW_LMgr_RootBox_GetSmallScreenOn( rootBox );

    if (NW_Object_IsInstanceOf(box, &NW_LMgr_BidiFlowBox_Class))
    {
      /* we want to limit the width of the FlowBoxes to be the display width 
         in vertical layout mode
      */
      if ((name == NW_CSS_Prop_width) && flag)
      {
        CGDIDeviceContext* deviceContext;
        const NW_GDI_Rectangle_t* rectangle;
        
        deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
        rectangle = deviceContext->DisplayBounds();

        if (value->integer > rectangle->dimension.width)
        {
          value->integer = (NW_Uint32)(rectangle->dimension.width-4);
        }
      }
    }
    /* In vertical layout mode, we want to ignore the whitespace property */
    if ((name == NW_CSS_Prop_whitespace) && flag)
    {
    while( box != NULL )
      {
      if( NW_Object_IsClass( box, &NW_LMgr_MarqueeBox_Class ) == NW_TRUE )
        {
        inMarquee = NW_TRUE; 
        break;
        }
      if (!box->parent)
        {
        break;
        }
      box = NW_LMgr_BoxOf( box->parent );
      }
    if (!inMarquee) 
      {
      if (value->token == NW_CSS_PropValue_pre) 
        {
        value->token = NW_CSS_PropValue_flags_smallScreenPre;
        } else {
        value->token = NW_CSS_PropValue_normal;
          }
      }
    }
    return KBrsrSuccess;
  } else {
    return KBrsrNotFound;
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_SetProperty
 * Description:  Sets a property on a box.
 * Returns:      KBrsrSuccess if successful; KBrsrOutOfMemory if
 *               out of memory.
 */

TBrowserStatusCode
NW_LMgr_Box_SetProperty (NW_LMgr_Box_t* box,
                         NW_LMgr_PropertyName_t name,
                         NW_LMgr_Property_t* property)
{
  NW_TRY (status) {
    NW_ASSERT(box != NULL);

    if (box->propList == NULL) {
      box->propList =
        (NW_LMgr_PropertyList_t*) NW_LMgr_SimplePropertyList_New( 2, 10 );
      NW_THROW_OOM_ON_NULL (box->propList, status);
    }
  
    status = NW_LMgr_PropertyList_Set (box->propList, name, property);
    _NW_THROW_ON_ERROR (status);

} NW_CATCH (status) {
  } NW_FINALLY {
   return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_RemoveProperty
 * Description:  Removes a property from a box.
 * Returns:      KBrsrSuccess if successful; KBrsrOutOfMemory if
 *               out of memory.
 */

TBrowserStatusCode
NW_LMgr_Box_RemoveProperty (NW_LMgr_Box_t* box,
                            NW_LMgr_PropertyName_t name)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(box != NULL);

  if (box->propList != NULL) {
    status = NW_LMgr_PropertyList_Remove (box->propList, name);
  }

  return status;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_RemovePropertyWithoutDelete
 * Description:  Removes a property from a box; however, does NOT delete the property
 *               Only for simple property list
 * Returns:      KBrsrSuccess if successful; KBrsrOutOfMemory if
 *               out of memory.
 */

TBrowserStatusCode
NW_LMgr_Box_RemovePropertyWithoutDelete (NW_LMgr_Box_t* box,
                                         NW_LMgr_PropertyName_t name)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    NW_ASSERT(box != NULL);
    NW_ASSERT( NW_Object_IsInstanceOf(box->propList, &NW_LMgr_SimplePropertyList_Class) );

    if (box->propList != NULL) 
        {
        status = NW_LMgr_SimplePropertyList_RemoveWithoutDelete (
                                                 (NW_LMgr_SimplePropertyList_t*)(box->propList), 
                                                 name);
        }

    return status;
    }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_ApplyFontProps
 * Description:  Use the CSS properties of a text box to match a GDI font and cache
 *               this as the NW_Prop_GDI_Font property. The allocates a font object
 *               which needs to be freed when the box is destroyed.
 * Returns:     KBrsrSuccess, KBrsrOutOfMemory.
 */
TBrowserStatusCode
NW_LMgr_Box_ApplyFontProps (NW_LMgr_Box_t* box)
    {
    NW_LMgr_Property_t fontProp;

    fontProp.type = NW_CSS_ValueType_Font;
    fontProp.value.object = NULL;

    NW_TRY (status) 
        {
        NW_LMgr_RootBox_t* rootBox;
        CGDIDeviceContext* deviceContext;
        NW_LMgr_Property_t fontFamily;
        NW_LMgr_Property_t size;
        NW_LMgr_Property_t style;
        NW_LMgr_Property_t weight;
        NW_LMgr_Property_t variant;
        NW_Uint8 gdiFontStyle;
        NW_Uint8 gdiFontVariant;
        TBrowserStatusCode s1, s2, s3, s4, s5;

        /* Check whether the properties have already been applied */
        NW_LMgr_Box_GetPropertyFromList (box, NW_CSS_Prop_fontObject, &fontProp);
        if (fontProp.value.object != NULL) 
            {
            return KBrsrSuccess;
            }

        // Get the properties from the box property list
        s1 = NW_LMgr_Box_GetPropertyFromList (box, NW_CSS_Prop_fontFamily, &fontFamily);
        s2 = NW_LMgr_Box_GetPropertyFromList (box, NW_CSS_Prop_fontSize, &size);
        s3 = NW_LMgr_Box_GetPropertyFromList (box, NW_CSS_Prop_fontStyle, &style);
        s4 = NW_LMgr_Box_GetPropertyFromList (box, NW_CSS_Prop_fontWeight, &weight);
        s5 = NW_LMgr_Box_GetPropertyFromList (box, NW_CSS_Prop_fontVariant, &variant);

        // If there are no properties to apply, we return.  If the box is the root,
        // we must apply the properties at all times. 
        if (s1 == KBrsrNotFound && s2 == KBrsrNotFound && s3 == KBrsrNotFound && 
        s4 == KBrsrNotFound && s5 == KBrsrNotFound && box->parent != NULL) 
            {
            return KBrsrSuccess;
            }    

        // Set defaults
        fontFamily.value.object = const_cast<NW_Text_t*>(KGDIFontDefaultFamily);
        size.value.integer = KGDIFontZeroSize;
        style.value.token = NW_CSS_PropValue_normal;
        weight.value.integer = KGDIFontDefaultWeight;
        variant.value.token = NW_CSS_PropValue_normal;

        // Now take into account inheritance
        NW_LMgr_Box_GetProperty (box, NW_CSS_Prop_fontFamily, &fontFamily);
        NW_LMgr_Box_GetProperty (box, NW_CSS_Prop_fontSize, &size);
        NW_LMgr_Box_GetProperty (box, NW_CSS_Prop_fontStyle, &style);
        NW_LMgr_Box_GetProperty (box, NW_CSS_Prop_fontWeight, &weight);
        NW_LMgr_Box_GetProperty (box, NW_CSS_Prop_fontVariant, &variant);

        // For negative value, use default font size
        if ( size.value.integer < 0 )
            {
            size.value.decimal = KGDIFontDefaultSizeInPt;
            size.type = NW_CSS_ValueType_Pt;
            }

        // Check the device resolution in order to resolve the correct font size for the device
        CEikonEnv* eikEnv = CEikonEnv::Static();
        CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();

        // we are using pixels to pass off to the device context, from there they get converted to twips
        TInt px = 0;

        if (size.type == NW_CSS_ValueType_Pt)
            {
            TInt valInt = (TInt)NW_Math_round(size.value.decimal);

            px = screenDev.VerticalTwipsToPixels(FontUtils::TwipsFromPoints(valInt));

            size.type = NW_CSS_ValueType_Px;
            size.value.integer = px;
            }
        else if (size.type == NW_CSS_ValueType_Px)
            {
            TUint dpi = screenDev.VerticalTwipsToPixels(KTwipsPerInch);

            // Since web designers assume a 96 pixel per inch (ppi) display use that as the reference point. 
            // Thus the multiplier should be:
            // ppi/96
            // Below is a list of the density of commercially available LCD displays. 
            // Density Multiplier:
            // 400 ppi	4.2
            // 210 ppi	2.2
            // 200 ppi	2.1
            // 120 ppi	1.3
            // 100 ppi	1.0
            // 96 ppi   1.0       
            if ((200 > dpi) && (dpi > 120))
                {
                px = (TInt)((float)size.value.integer * 1.3);
                }
            else if ((210 > dpi) && (dpi > 200))
                {
                px = (TInt)((float)size.value.integer * 2.1);
                }
            else if ((400 > dpi) && (dpi > 210))
                {
                px = (TInt)((float)size.value.integer * 2.2);
                }
            else if (dpi > 400)
                {
                px = (TInt)((float)size.value.integer * 4.2);
                }

            size.value.integer = px;
            }

        switch (style.value.token)  
            {
            case NW_CSS_PropValue_oblique:
                gdiFontStyle = EGDIFontStyleOblique;
                break;

            case NW_CSS_PropValue_italic:
                gdiFontStyle = EGDIFontStyleItalic;
                break;

            default:
                gdiFontStyle = KGDIFontDefaultStyle;
            }

        switch (variant.value.token) 
            {
            case NW_CSS_PropValue_smallcaps:
                gdiFontVariant = EGDIFontVariantSmallCaps;
                break;

            default:
                gdiFontVariant = KGDIFontDefaultVariant;
            }

        // we're going to need the DeviceContext to create Font
        rootBox = NW_LMgr_Box_GetRootBox (box);
        NW_ASSERT (rootBox != NULL);

        deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
        NW_ASSERT (deviceContext != NULL);

        fontProp.type = NW_CSS_ValueType_Font;
        fontProp.value.object =
        deviceContext->CreateFont ((NW_Text_t*) fontFamily.value.object,
                                 (TGDIFontStyle) gdiFontStyle,
                                 (NW_Uint8) size.value.integer,
                                 (NW_Uint16) weight.value.integer,
                                 (TGDIFontVariant) gdiFontVariant);
        NW_THROW_OOM_ON_NULL (fontProp.value.object, status);

        // Add the computed GDI font to the property list
        status = NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf(box),
                                      NW_CSS_Prop_fontObject, &fontProp);

        _NW_THROW_ON_ERROR (status);

        } 
    NW_CATCH (status) 
        {
        delete (CGDIFont*)fontProp.value.object;
        } 
    NW_FINALLY 
        {
        return status;
        } 
    NW_END_TRY
    }

/* ------------------------------------------------------------------------- *
 * Function:    NW_LMgr_Box_GetFont
 * Description: Get the GDI font handle.
 * Returns:     KBrsrSuccess, KBrsrOutOfMemory.
 */

CGDIFont*
NW_LMgr_Box_GetFont (NW_LMgr_Box_t* box)
{
  TBrowserStatusCode status;
  NW_LMgr_PropertyValue_t font;

  NW_ASSERT(box != NULL);

  font.object = NULL;

  /* If GDI font handle prop is set, return it */
  status = NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_fontObject, NW_CSS_ValueType_Font, &font);
  if (status == KBrsrSuccess) {
    return (CGDIFont*) font.object;
  }
  else {
    return NULL;
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetModelProp
 * Description:  Returns a single box-model property for the box
 *               (as opposed to GetModelProps which returns the values for
 *               all sides of the box).
 * Returns:      Property value.
 */

/* TODO: move all this to GetValueProp */

NW_Int32
NW_LMgr_Box_GetModelProp(NW_LMgr_Box_t *box, NW_LMgr_PropertyName_t propName) 
{
  NW_LMgr_Property_t prop;
  NW_LMgr_PropertyName_t shortcutProp;
  NW_LMgr_PropertyValueType_t defaultPropType;
  NW_Bool doInherit = NW_FALSE;
  TBrowserStatusCode status;

  switch (propName) {
  case NW_CSS_Prop_topBorderWidth:
  case NW_CSS_Prop_bottomBorderWidth:
  case NW_CSS_Prop_leftBorderWidth:
  case NW_CSS_Prop_rightBorderWidth:

    shortcutProp = NW_CSS_Prop_borderWidth;
    defaultPropType = NW_CSS_ValueType_Px;
    break;

  case NW_CSS_Prop_topBorderStyle:
  case NW_CSS_Prop_bottomBorderStyle:
  case NW_CSS_Prop_leftBorderStyle:
  case NW_CSS_Prop_rightBorderStyle:

    shortcutProp = NW_CSS_Prop_borderStyle;
    defaultPropType = NW_CSS_ValueType_Token;
    break;

  case NW_CSS_Prop_topBorderColor:
  case NW_CSS_Prop_bottomBorderColor:
  case NW_CSS_Prop_leftBorderColor:
  case NW_CSS_Prop_rightBorderColor:

    shortcutProp = NW_CSS_Prop_borderColor;
    defaultPropType = NW_CSS_ValueType_Color;
    break;

  case NW_CSS_Prop_topPadding:
  case NW_CSS_Prop_bottomPadding:
  case NW_CSS_Prop_leftPadding:
  case NW_CSS_Prop_rightPadding:

    shortcutProp = NW_CSS_Prop_padding;
    defaultPropType = NW_CSS_ValueType_Px;
    break;

  case NW_CSS_Prop_topMargin:
  case NW_CSS_Prop_bottomMargin:
  case NW_CSS_Prop_leftMargin:
  case NW_CSS_Prop_rightMargin:

    shortcutProp = NW_CSS_Prop_margin;
    defaultPropType = NW_CSS_ValueType_Px;
    break;

  default:
    NW_ASSERT(0);
    return 0;
  }

  status = NW_LMgr_Box_GetPropertyFromList(box, propName, &prop);
  if (status == KBrsrSuccess) {
    if ((prop.type == NW_CSS_ValueType_Token) && (prop.value.token == NW_CSS_PropValue_inherit)) {
      doInherit = NW_TRUE;
    }
    else if (prop.type == defaultPropType) {
      if ((shortcutProp == NW_CSS_Prop_padding) || (shortcutProp == NW_CSS_Prop_borderWidth)) {
        if (prop.value.integer < 0) {
          return 0;
        }
      }
      return prop.value.integer;
    }
    else if (shortcutProp == NW_CSS_Prop_margin) {
      /* Special handling of auto margins */
      if ((prop.type == NW_CSS_ValueType_Token) && (prop.value.token == NW_CSS_PropValue_auto)) {
        return NW_LMgr_FrameInfo_AutoMargin;
      }
    }
  }
  status = NW_LMgr_Box_GetPropertyFromList(box, shortcutProp, &prop);
  if (status == KBrsrSuccess) {
    if ((prop.type == NW_CSS_ValueType_Token) && (prop.value.token == NW_CSS_PropValue_inherit)) {
      doInherit = NW_TRUE;
    }
    else if (prop.type == defaultPropType) {
      if ((shortcutProp == NW_CSS_Prop_padding) || (shortcutProp == NW_CSS_Prop_borderWidth)) {
        if (prop.value.integer < 0) {
          return 0;
        }
      }
      return prop.value.integer;
    }
    else if (shortcutProp == NW_CSS_Prop_margin) {
      /* Special handling of auto margins */
      if ((prop.type == NW_CSS_ValueType_Token) && (prop.value.token == NW_CSS_PropValue_auto)) {
        return NW_LMgr_FrameInfo_AutoMargin;
      }
    }
  }

  if (doInherit && box->parent) 
  {
    if (NW_Object_IsInstanceOf(box->parent, &LMgrAnonBlock_Class)) 
    {
     // this parent is an anonymous box, we should go up to see if grandpa has 
      //  the prop we are interested. 
      box = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent (box);
      if (box->parent == NULL) {
        return 0;
      }
    }
    return NW_LMgr_Box_GetModelProp((NW_LMgr_Box_t*)(box->parent), propName);
  }

  /* Otherwise return defaults */
  if (shortcutProp == NW_CSS_Prop_borderColor) {
    NW_LMgr_PropertyValue_t color;
    color.integer = 0;
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_color, NW_CSS_ValueType_Color, &color);
    return color.integer;
  }
  else if (shortcutProp == NW_CSS_Prop_borderWidth) 
  {
    if (NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class))
    {
      return (NW_LMgr_ImgContainerBox_ImageIsBroken(box) == NW_TRUE) ?
        NW_CSS_PropValue_none : NW_LMgr_Box_MinBorderWidth;
    }
    else
    {
      return NW_LMgr_Box_MediumBorderWidth;
    }
  }
  else if (shortcutProp == NW_CSS_Prop_borderStyle) {
    if (NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class) && 
        NW_LMgr_ImgContainerBox_ImageIsBroken( box ) == NW_TRUE )
    {
      return NW_CSS_PropValue_solid;
    }
    return NW_CSS_PropValue_none;
  }
  else {
    return 0;
  }
}


/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetBorderWidth
 * Description:  Calculates the width of the box border on all four sides
 *               and stores it in borderWidth.  
 * Returns:      void.
 */

void
NW_LMgr_Box_GetBorderWidth(NW_LMgr_Box_t *box,
                           NW_LMgr_FrameInfo_t *borderWidth, 
                           NW_Int8 aFrameSide )
{
  NW_LMgr_FrameInfo_t borderStyle;

  borderWidth->left = 0;
  borderWidth->right = 0;
  borderWidth->top = 0;
  borderWidth->bottom = 0;

  /* set the width to zero if the box style is "none" */
  NW_LMgr_Box_GetBorderStyle( box, &borderStyle, aFrameSide );

  if( ( aFrameSide & ELMgrFrameLeft ) && 
      borderStyle.left != NW_CSS_PropValue_none && borderStyle.left != NW_CSS_PropValue_hidden )
    {
    borderWidth->left = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_leftBorderWidth);
  }
  if( ( aFrameSide & ELMgrFrameBottom ) && 
      borderStyle.bottom != NW_CSS_PropValue_none && borderStyle.bottom != NW_CSS_PropValue_hidden )
    {
    borderWidth->bottom = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomBorderWidth);
  }
  if( ( aFrameSide & ELMgrFrameTop ) && 
      borderStyle.top != NW_CSS_PropValue_none && borderStyle.top != NW_CSS_PropValue_hidden )      
    {
    borderWidth->top = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topBorderWidth);
  }
  if( ( aFrameSide & ELMgrFrameRight ) && 
    borderStyle.right != NW_CSS_PropValue_none && borderStyle.right != NW_CSS_PropValue_hidden )
    {
    borderWidth->right = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_rightBorderWidth);
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetBorderStyle
 * Description:  Gets a set of box border-style properties
 *               (left, right, top, bottom) and stores them in borderStyle.  
 * Returns:      void (in case no property are set, the default value of 0
 *               is returned for the property value).
 */

void
NW_LMgr_Box_GetBorderStyle(NW_LMgr_Box_t *box,
                           NW_LMgr_FrameInfo_t* borderStyle,
                           NW_Int8 aFrameSide )
  {
  // init
  borderStyle->left = 0;
  borderStyle->right = 0;
  borderStyle->top = 0;
  borderStyle->bottom = 0;
  //
  if( aFrameSide & ELMgrFrameLeft )
{
  borderStyle->left = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_leftBorderStyle);
    }
  if( aFrameSide & ELMgrFrameRight )
    {
  borderStyle->right = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_rightBorderStyle);
    }
  if( aFrameSide & ELMgrFrameTop )
    {
  borderStyle->top = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topBorderStyle);
    }
  if( aFrameSide & ELMgrFrameBottom )
    {
  borderStyle->bottom = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomBorderStyle);
    }
  }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetBorderColor
 * Description:  Gets a set of box border-color properties
 *               (left, right, top, bottom) and stores them in borderColor.  
 * Returns:      void (in case no property are set, the default value of 0
 *               is returned for the property value).
 */

void
NW_LMgr_Box_GetBorderColor(NW_LMgr_Box_t *box,
                           NW_LMgr_FrameInfo_t *borderColor)
{
  borderColor->left = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_leftBorderColor);
  borderColor->right = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_rightBorderColor);
  borderColor->top = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topBorderColor);
  borderColor->bottom = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomBorderColor);
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetMargins
 * Description:  Calculates the width of the box margin on all four sides
 *               and stores it in "margins".  
 * Returns:      void.
 */

void
NW_LMgr_Box_GetMargins(NW_LMgr_Box_t *box,
                       NW_LMgr_FrameInfo_t *margins,
                       NW_LMgr_FrameInfo_t* isAuto,
                       NW_Int8 aFrameSide )
{
  NW_LMgr_RootBox_t* rootBox;

  if (isAuto) 
    {
    NW_Mem_memset(isAuto, 0, sizeof(NW_LMgr_FrameInfo_t));
    }

  if( aFrameSide & ELMgrFrameLeft )
    {
    margins->left = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_leftMargin);
    }
  if( aFrameSide & ELMgrFrameRight )
    {
    margins->right = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_rightMargin);
    }
  if( aFrameSide & ELMgrFrameTop )
    {
    margins->top = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topMargin);
    }
  if( aFrameSide & ELMgrFrameBottom )
    {
    margins->bottom = NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomMargin);
    }

  if( ( aFrameSide & ELMgrFrameLeft ) && margins->left == NW_LMgr_FrameInfo_Auto) 
    {
    margins->left = 0;
    if (isAuto) 
      {
      isAuto->left = 1;
      }
    }
  if( ( aFrameSide & ELMgrFrameRight ) && margins->right == NW_LMgr_FrameInfo_Auto) 
    {
    margins->right = 0;
    if (isAuto) 
      {
      isAuto->right = 1;
      }
    }
  if( ( aFrameSide & ELMgrFrameTop ) && margins->top == NW_LMgr_FrameInfo_Auto) 
    {
    margins->top = 0;
    if (isAuto) 
      {
      isAuto->top = 1;
      }
    }
  if( ( aFrameSide & ELMgrFrameBottom ) && margins->bottom == NW_LMgr_FrameInfo_Auto) 
    {
    margins->bottom = 0;
    if (isAuto) 
      {
      isAuto->bottom = 1;
      }
    }

  rootBox = NW_LMgr_Box_GetRootBox(box);
  if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
  {
    CGDIDeviceContext* deviceContext;
    const NW_GDI_Rectangle_t* rectangle;

    /* We want to limit the margin to 15% of display width */
    if ((margins->left != NW_LMgr_FrameInfo_Auto) || (margins->right != NW_LMgr_FrameInfo_Auto))
    {
      NW_GDI_Metric_t maxMargin = 0;
      deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
      rectangle = deviceContext->DisplayBounds();
      maxMargin = (NW_GDI_Metric_t)(rectangle->dimension.width*15/100);

      if( ( aFrameSide & ELMgrFrameLeft ) && margins->left > maxMargin)
      {
        margins->left = maxMargin;
      }
      if( ( aFrameSide & ELMgrFrameRight ) && margins->right > maxMargin)
      {
        margins->right = maxMargin;
      }
    }
  }
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetPadding
 * Description:  Calculates the width of the box padding on all four sides
 *               and stores it in "padding".  
 * Returns:      void
 */

void
NW_LMgr_Box_GetPadding(NW_LMgr_Box_t *box, 
                        NW_LMgr_FrameInfo_t* padding,
                        NW_Int8 aFrameSide )
  {
  // init
  padding->left = 0;
  padding->right = 0;
  padding->top = 0;
  padding->bottom = 0;
  //
  if( aFrameSide & ELMgrFrameLeft )
    {
    padding->left = NW_LMgr_Box_GetModelProp( box, NW_CSS_Prop_leftPadding );
    }
  if( aFrameSide & ELMgrFrameRight )
    {
    padding->right = NW_LMgr_Box_GetModelProp( box, NW_CSS_Prop_rightPadding );
    }
  if( aFrameSide & ELMgrFrameTop )
    {
    padding->top = NW_LMgr_Box_GetModelProp( box, NW_CSS_Prop_topPadding );
    }
  if( aFrameSide & ELMgrFrameBottom )
    {
    padding->bottom = NW_LMgr_Box_GetModelProp( box, NW_CSS_Prop_bottomPadding );
    }
  }


/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_DrawSolidBorder
 * Description:  Draws a solid border around the content of the box,
                 using the current boundaries of the box.  
 * Returns:      KBrsrSuccess, KBrsrFailure (?), KBrsrOutOfMemory.
 */

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_DrawSolidBorder
 * Description:  Draws a solid border around the content of the box,
                 using the current boundaries of the box.  
 * Returns:      KBrsrSuccess, KBrsrFailure (?), KBrsrOutOfMemory.
 */

TBrowserStatusCode
NW_LMgr_Box_DrawBorder (NW_GDI_Rectangle_t bounds, 
                        CGDIDeviceContext*  deviceContext,
                        NW_LMgr_FrameInfo_t* borderWidth, 
                        NW_LMgr_FrameInfo_t* borderStyle, 
                        NW_LMgr_FrameInfo_t* borderColor)
{
  NW_GDI_Color_t oldFg;
  NW_GDI_Pattern_t oldPat;

  oldFg = deviceContext->ForegroundColor();
  oldPat = deviceContext->LinePattern();

  NW_TRY (status) {
    NW_GDI_Metric_t innerWidth, innerHeight;
    NW_Uint8 i;
    NW_GDI_Point2D_t vertices[2];
    NW_Float32 ratioTopLeft=0, ratioBottomLeft=0, ratioTopRight=0, ratioBottomRight=0;
    NW_Float32 ratioLeftTop=0, ratioLeftBottom=0, ratioRightTop=0, ratioRightBottom=0;

    innerWidth = (NW_GDI_Metric_t)(bounds.dimension.width - borderWidth->left - borderWidth->right);
    innerHeight = (NW_GDI_Metric_t)(bounds.dimension.height - borderWidth->top - borderWidth->bottom);

    if (borderWidth->right == 0) {
      ratioTopRight = 0;
    } else {
      ratioTopRight = ((NW_Float32)borderWidth->top)/((NW_Float32)borderWidth->right);
    }
    if (ratioTopRight > 0) {
      ratioRightTop = 1/ratioTopRight;
    }

    if (borderWidth->right == 0) {
      ratioBottomRight = 0;
    } else {
      ratioBottomRight = ((NW_Float32)borderWidth->bottom)/((NW_Float32)borderWidth->right);
    }
    if (ratioBottomRight > 0) {
      ratioRightBottom = 1/ratioBottomRight;
    }

    if (borderWidth->left == 0) {
      ratioTopLeft = 0;
    } else {
      ratioTopLeft = ((NW_Float32)borderWidth->top)/((NW_Float32)borderWidth->left);
    }
    if (ratioTopLeft > 0) {
      ratioLeftTop = 1/ratioTopLeft;
    }

    if (borderWidth->left == 0) {
      ratioBottomLeft = 0;
    } else {
      ratioBottomLeft = ((NW_Float32)borderWidth->bottom)/((NW_Float32)borderWidth->left);
    }
    if (ratioBottomLeft > 0) {
      ratioLeftBottom = 1/ratioBottomLeft;
    }
    
    /* draw top border */
    if (borderWidth->top > 0)
    {
      if ( borderColor->top == NW_GDI_Color_Transparent) {
        borderStyle->top = NW_CSS_PropValue_none;
      } else {
        deviceContext->SetForegroundColor( borderColor->top);
      }
      deviceContext->SetLinePattern( NW_LMgr_Box_ConvertBorderStyle(borderStyle->top));
    
      for (i=0; i<borderWidth->top; i++) {
        if (borderStyle->top == NW_CSS_PropValue_solid ||
            borderStyle->top == NW_CSS_PropValue_dashed ||
            borderStyle->top == NW_CSS_PropValue_dotted) {
          vertices[0].x = (NW_GDI_Metric_t)(bounds.point.x + i*ratioLeftTop);
          vertices[0].y = vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + i);
          vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + bounds.dimension.width - i*ratioRightTop - 1);
        } else if (borderStyle->top == NW_CSS_PropValue_bevelled) {
          if (i >= innerWidth/2) {
            continue;
          }
          vertices[0].x = (NW_GDI_Metric_t)(bounds.point.x + borderWidth->left + i);
          vertices[0].y = vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + borderWidth->top - i - 1);
          vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + bounds.dimension.width - borderWidth->right - i - 1);
        } else {
          // skip any unknown border styles
          continue;
        }
        status = deviceContext->DrawPolyline( 2, vertices, NW_FALSE);
        _NW_THROW_ON_ERROR(status);
      }
    }

    /* draw left border */
    if (borderWidth->left > 0)
    {
      if ( borderColor->left == NW_GDI_Color_Transparent) {
        borderStyle->left = NW_CSS_PropValue_none;
      } else {
        deviceContext->SetForegroundColor( borderColor->left);
      }
      deviceContext->SetLinePattern( NW_LMgr_Box_ConvertBorderStyle(borderStyle->left));

      for (i=0; i<borderWidth->left; i++) {
        if (borderStyle->left == NW_CSS_PropValue_solid ||
            borderStyle->left == NW_CSS_PropValue_dashed ||
            borderStyle->left == NW_CSS_PropValue_dotted) {
          vertices[0].x = vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + i);
          vertices[0].y = (NW_GDI_Metric_t)(bounds.point.y + i*ratioTopLeft);
          vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + bounds.dimension.height - i*ratioBottomLeft - 1);
        } else if (borderStyle->left == NW_CSS_PropValue_bevelled) {
          if (i >= innerHeight/2) {
            continue;
          }
          vertices[0].x = vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + borderWidth->left - i - 1);
          vertices[0].y = (NW_GDI_Metric_t)(bounds.point.y + borderWidth->top + i);
          vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + bounds.dimension.height - borderWidth->bottom - i - 1);
        } else {
          // skip any unknown border styles
          continue;
        }
        status = deviceContext->DrawPolyline( 2, vertices, NW_FALSE);
        _NW_THROW_ON_ERROR(status);
      }
    }

    /* draw bottom border */
    if (borderWidth->bottom > 0)
    {
      if ( borderColor->bottom == NW_GDI_Color_Transparent) {
        borderStyle->bottom = NW_CSS_PropValue_none;
      } else {
        deviceContext->SetForegroundColor( borderColor->bottom);
      }
      deviceContext->SetLinePattern( NW_LMgr_Box_ConvertBorderStyle(borderStyle->bottom)); 
    
      for (i=0; i<borderWidth->bottom; i++) {
        if (borderStyle->bottom == NW_CSS_PropValue_solid ||
            borderStyle->bottom == NW_CSS_PropValue_dashed ||
            borderStyle->bottom == NW_CSS_PropValue_dotted) {
          vertices[0].x = (NW_GDI_Metric_t)(bounds.point.x + i*ratioLeftBottom);
          vertices[0].y = vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + bounds.dimension.height - i - 1);
          vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + bounds.dimension.width - i*ratioRightBottom - 1);
        } else if (borderStyle->bottom == NW_CSS_PropValue_bevelled) {
          if (i >= innerWidth/2) {
            continue;
          }
          vertices[0].x = (NW_GDI_Metric_t)(bounds.point.x + borderWidth->left + i);
          vertices[0].y = vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + bounds.dimension.height - borderWidth->bottom + i);
          vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + bounds.dimension.width - borderWidth->right - i - 1);
        } else {
          // skip any unknown border styles
          continue;
        }
        status = deviceContext->DrawPolyline( 2, vertices, NW_FALSE);
        _NW_THROW_ON_ERROR(status);
      }
    }

    /* draw right border */
    if (borderWidth->right > 0) 
    {
      if ( borderColor->right == NW_GDI_Color_Transparent) {
        borderStyle->right = NW_CSS_PropValue_none;
      } else {
        deviceContext->SetForegroundColor( borderColor->right);
      }
      deviceContext->SetLinePattern( NW_LMgr_Box_ConvertBorderStyle(borderStyle->right));
    
      for (i=0; i<borderWidth->right; i++) {
        if (borderStyle->right == NW_CSS_PropValue_solid ||
            borderStyle->right == NW_CSS_PropValue_dashed ||
            borderStyle->right == NW_CSS_PropValue_dotted) {
          vertices[0].x = vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + bounds.dimension.width - 1 - i);
          vertices[0].y = (NW_GDI_Metric_t)(bounds.point.y + i*ratioTopRight);
          vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + bounds.dimension.height - i*ratioBottomRight - 1);
        } else if (borderStyle->right == NW_CSS_PropValue_bevelled) {
          if (i >= innerHeight/2) {
            continue;
          }
          vertices[0].x = vertices[1].x = (NW_GDI_Metric_t)(bounds.point.x + bounds.dimension.width - borderWidth->right + i);
          vertices[0].y = (NW_GDI_Metric_t)(bounds.point.y + borderWidth->top + i);
          vertices[1].y = (NW_GDI_Metric_t)(bounds.point.y + bounds.dimension.height - borderWidth->bottom - i - 1);
        } else {
          // skip any unknown border styles
          continue;
        }
        status = deviceContext->DrawPolyline( 2, vertices, NW_FALSE);
        _NW_THROW_ON_ERROR(status);
      }
    }

} NW_CATCH (status) {
  } NW_FINALLY {
    deviceContext->SetForegroundColor( oldFg);
    deviceContext->SetLinePattern(oldPat);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetSizeProperties
 * Description:  This method gets any size properties set on box. If the 
 *               property is not set, (-1) is returned.  This is OK because 
 *               negative values are illegal for CSS_width and CSS_height.  
 * Returns:      KBrsrSuccess
 */

TBrowserStatusCode
NW_LMgr_Box_GetSizeProperties(NW_LMgr_Box_t *box,
                              NW_GDI_Dimension2D_t *size){
  
  NW_LMgr_PropertyValue_t sizeVal;

  NW_ASSERT(box != NULL);
  NW_ASSERT(size != NULL);

  size->width = -1;
  size->height = -1;

  if(NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal) == KBrsrSuccess){
    size->width = (NW_GDI_Metric_t)sizeVal.integer;
  }

  if(NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal) == KBrsrSuccess){
    size->height = (NW_GDI_Metric_t)sizeVal.integer;
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetMinimumSize
 * Description:  This method first adds margins, borders, and padding to minimum 
 *               content size.  While this method is final, it invokes a virtual 
 *               method to get the minimum content size.
 * Returns:      KBrsrSuccess, KBrsrOutOfMemory
 */

TBrowserStatusCode
NW_LMgr_Box_GetMinimumSize (NW_LMgr_Box_t* box,
                            NW_GDI_Dimension2D_t* size)
{
  NW_TRY (status) {
    NW_LMgr_FrameInfo_t padding, borderWidth;

    NW_ASSERT(box != NULL);
    NW_ASSERT(size != NULL);
    
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

    // for bgsound support: in normal screen mode if it's object box, still check visibility
    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) ||
        NW_Object_IsInstanceOf (box, &NW_LMgr_ObjectBox_Class) )
    {
      NW_LMgr_PropertyValue_t visibilityVal;
      visibilityVal.token = NW_CSS_PropValue_visible;
      status = NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_visibility,
                                     NW_CSS_ValueType_Token, &visibilityVal);
      if (status  == KBrsrSuccess && visibilityVal.token == NW_CSS_PropValue_hidden) 
      {
        size->width = 0 ;
        size->height = 0;
        NW_THROW_SUCCESS(status);
      }
    }

    /* In case of vertical layout is enabled set the height and width to zero 
        if the visibility attrib is hidden*/
    /* Start with minimum content box */
    status = NW_LMgr_Box_GetMinimumContentSize (box, size);
    _NW_THROW_ON_ERROR (status);

    if ((size->width == 0) && (size->height == 0))
      NW_THROW_SUCCESS(status);

    /* Now add any border, padding */
    NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );
  
    size->width =
      (NW_GDI_Metric_t) (size->width + padding.left + padding.right +
                         borderWidth.left + borderWidth.right);
    size->height = 
      (NW_GDI_Metric_t) (size->height + padding.top + padding.bottom + 
                         borderWidth.top + borderWidth.bottom);

} NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_HasFixedWidth
 * Description:  Determines whether the box has fixed width.
 * Returns:      NW_TRUE, NW_FALSE
 */

NW_Bool
NW_LMgr_Box_HasFixedWidth(NW_LMgr_Box_t* box){

	NW_LMgr_Property_t prop;
  TBrowserStatusCode status;

	NW_ASSERT(box != NULL);

	if(NW_LMgr_Box_HasFixedContentSize(box) == NW_TRUE){
		return NW_TRUE;
	}

  prop.type = NW_CSS_ValueType_Token;
  prop.value.integer = NW_CSS_PropValue_auto;
  status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_width, &prop);
  if (status == KBrsrSuccess && prop.type == NW_CSS_ValueType_Px) {
    return NW_TRUE;
	}

	return NW_FALSE;

}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_HasFixedHeight
 * Description:  Determines whether the box has fixed height.
 * Returns:      NW_TRUE, NW_FALSE
 */

NW_Bool
NW_LMgr_Box_HasFixedHeight(NW_LMgr_Box_t* box){

	NW_LMgr_Property_t prop;
  TBrowserStatusCode status;

	NW_ASSERT(box != NULL);

	if(NW_LMgr_Box_HasFixedContentSize(box) == NW_TRUE){
		return NW_TRUE;
	}

  prop.type = NW_CSS_ValueType_Token;
  prop.value.integer = NW_CSS_PropValue_auto;
  status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_width, &prop);
  if (status == KBrsrSuccess && prop.type == NW_CSS_ValueType_Px) {
    return NW_TRUE;
	}

	return NW_FALSE;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetEm
 * Description:  Calculates the em size for the box.
 * Returns:      The em size in pixels.
 */

/* TODO:  change this interface so that out-of-memory status is propagated up
 */

NW_GDI_Metric_t
NW_LMgr_Box_GetEm (NW_LMgr_Box_t* box)
  {
  NW_GDI_Metric_t em = 0;

  NW_TRY (status) 
    {
    CGDIFont *font;
    TGDIFontInfo fontInfo;

    /* Set the GDI to the box font */
    font = NW_LMgr_Box_GetFont(box);  
    NW_THROW_ON_NULL(font, status, KBrsrUnexpectedError);
    
    /* Get the em value */  
    status = font->GetFontInfo( &fontInfo);
    _NW_THROW_ON_ERROR(status);
    em = fontInfo.height;
    } 
  NW_CATCH (status) 
    {
    em = 0;
    } 
  NW_FINALLY 
    {
    return em;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_GetEx
 * Description:  Calculates the ex size for the box.
 * Returns:      The ex size in pixels.
 */

/* TODO:  Currently ex is set to be 1/2 of em, becuase we have no way
 *        of accessing the actual dimensions of the 'x' character.  This
 *        should be fixed. 
 * TODO:  change this interface so that out-of-memory status is propagated up
 */

NW_GDI_Metric_t
NW_LMgr_Box_GetEx (NW_LMgr_Box_t* box)
{
  return (NW_GDI_Metric_t)(NW_LMgr_Box_GetEm(box)/2);
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_FindBoxOfClass
 * Description:  Tries to find a box of a given class or, if the class is an
 *               interface specification, implementing that interface. If
 *               the derived flag is set, then the parent boxes are queried
 *               for an instance of the given class. 
 * Returns:      The box, if found.  Otherwise, NULL.
 */

NW_LMgr_Box_t*
NW_LMgr_Box_FindBoxOfClass (NW_LMgr_Box_t* box,
                            const NW_Object_Class_t* theClass,
                            NW_Bool derived)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

  /* traverse up the box tree looking for the first box of either derived from
     the specified class or, if the specified class is an interface
     specification, implementing that interface */
  while (box != NULL) {
    NW_Object_t* object;

    object = NW_Object_QueryInterface (box, theClass);
    if (object != NULL && (derived || NW_Object_IsClass (object, theClass))) {
      return box;
    }

    /* try the parent */
    box = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent (box);
  }

  /* return the result */
  return box;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_RootBox_t*
_NW_LMgr_Box_GetRootBox (NW_LMgr_Box_t* box)
{
  // ZAL: temp: change rootBox back to NW_LMgr_RootBox_t*
  // parameter assertion block 
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

  // if the rootBox is not set, then get it from the parent and then set it.
  if( !box->rootBox )
    {    
    if( box->parent )
      {
      box->rootBox = (void*)NW_LMgr_Box_GetRootBox( box->parent );
      }
    // no parent means that we are at the rootbox level
    // However if it is not the rootbox, then the box is corrupt as
    // we cannot get to the rootbox level.
    else if( NW_Object_IsClass( box, &NW_LMgr_RootBox_Class ) )
      {
      // rootbox points back to itself
      box->rootBox = (void*)NW_LMgr_RootBoxOf( box );
      }
    else
      {
      // NW_ASSERT( NW_FALSE )???
      }
  }
  return (NW_LMgr_RootBox_t*)box->rootBox;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Detach
 * Description:  Detaches the box from its parent.
 * Returns:      KBrsrSuccess.
 */

TBrowserStatusCode
NW_LMgr_Box_Detach (NW_LMgr_Box_t* box)
{
  NW_LMgr_ContainerBox_t* parentBox;

  /* parameter assertion block */
  NW_ASSERT(box != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

  /* get the parent box of the box - if it doesn't have one we simply return
     success */
  parentBox = NW_LMgr_Box_GetParent (box);
  if (parentBox == NULL) {
    return KBrsrSuccess;
  }

  /* invoke the ContainerBox's method to remove the child */
  return NW_LMgr_ContainerBox_RemoveChild (parentBox, box);
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_DrawBackground
 * Description:  Redraws the box and its descendants.  Its behavior is only 
 */

TBrowserStatusCode
NW_LMgr_Box_DrawBackground (NW_LMgr_Box_t* box, 
                            NW_LMgr_ContainerBox_t* parent,
                            CGDIDeviceContext* deviceContext)
{
  NW_TRY (status) {
    NW_LMgr_Box_t *parentBox = NW_LMgr_BoxOf(parent);
    NW_LMgr_Property_t prop;

    /* Get the rootbox */
    if (NW_Object_IsClass(parent, &NW_LMgr_RootBox_Class)) {
      return KBrsrSuccess;
    }

    status = NW_LMgr_Box_DrawBackground(box, parentBox->parent, deviceContext);
    _NW_THROW_ON_ERROR (status);

    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_transparent;
    // special handling for getting background color of StaticTableCellBox
    if (NW_Object_IsInstanceOf (parent, &NW_LMgr_StaticTableCellBox_Class))
      {
      NW_LMgr_StaticTableCellBox_t *cell = NW_LMgr_StaticTableCellBoxOf(parent);
      NW_LMgr_StaticTableCellBox_GetBackground(cell, &prop);
      }
    else
      {
      NW_LMgr_Box_GetProperty(parentBox, NW_CSS_Prop_backgroundColor, &prop);
      }

    if (prop.type == NW_CSS_ValueType_Color) 
    {
      NW_LMgr_Box_t tempBox;
      // set the rootbox so that we can get the cached settings
      status = NW_LMgr_Box_Initialize (&tempBox, 1);
      tempBox.rootBox = NW_LMgr_Box_GetRootBox( box ); 
      _NW_THROW_ON_ERROR (status);
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );
      NW_GDI_Rectangle_t parentBoxBounds = NW_LMgr_Box_GetDisplayBounds( parentBox );
      NW_GDI_Rectangle_t tempBoxBounds = NW_LMgr_Box_GetDisplayBounds( &tempBox );

      if (NW_GDI_Rectangle_Cross (&boxBounds, &parentBoxBounds, &tempBoxBounds)) 
        { 
        /* Draw the background */
        NW_LMgr_Box_SetProperty (&tempBox, NW_CSS_Prop_backgroundColor, &prop);
        // set display bounds
        NW_LMgr_Box_UpdateDisplayBounds(&tempBox, tempBoxBounds);
        status = NW_LMgr_Box_Draw (&tempBox, deviceContext, NW_FALSE);
        if (status != KBrsrSuccess) {
          NW_Object_Terminate (&tempBox);
          if (status != KBrsrOutOfMemory) {
            status = KBrsrUnexpectedError;
          }
          return status;
        }
      }

      NW_Object_Terminate (&tempBox);
    }

} NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}
 

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_Box_GetInnerRectangle(NW_LMgr_Box_t* box,
                              NW_GDI_Rectangle_t* innerRectangle)
{
  NW_GDI_Rectangle_t bounds;
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_FrameInfo_t borderWidth;

  /* Get all margin, border and padding settings */
  bounds = (NW_LMgr_Box_GetDisplayBounds(box));
  NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );
  NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );

  innerRectangle->point.x = 
    (NW_GDI_Metric_t)(bounds.point.x + borderWidth.left + padding.left);
  
  innerRectangle->point.y = 
    (NW_GDI_Metric_t)(bounds.point.y + borderWidth.top + padding.top);

  innerRectangle->dimension.height = (NW_GDI_Metric_t)(bounds.dimension.height - 
    (borderWidth.top + borderWidth.bottom + padding.top + padding.bottom));

  innerRectangle->dimension.width = (NW_GDI_Metric_t)(bounds.dimension.width - 
    (borderWidth.left + borderWidth.right + padding.left + padding.right));
  
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_LMgr_Box_GetVisibleBounds(const NW_LMgr_Box_t* box,
                             NW_GDI_Rectangle_t* bounds)
{
  NW_GDI_Rectangle_t rect;
  NW_LMgr_PropertyValue_t overflow;

  if (bounds != NULL) {
    NW_Mem_memset(bounds, 0, sizeof(NW_GDI_Rectangle_t));
  }

  rect = NW_LMgr_Box_GetDisplayBounds((NW_LMgr_Box_t*)box);

  box = (NW_LMgr_Box_t*)box->parent;
  while (box != NULL) {
    if (NW_Object_IsClass(box, &NW_LMgr_BidiFlowBox_Class) ||
        NW_Object_IsClass(box, &NW_LMgr_StaticTableBox_Class) ||
        NW_Object_IsClass(box, &NW_LMgr_StaticTableCellBox_Class)) {
      overflow.token = NW_CSS_PropValue_visible;
      (void)NW_LMgr_Box_GetPropertyValue((NW_LMgr_Box_t*)box, NW_CSS_Prop_overflow,
                                         NW_CSS_ValueType_Token, &overflow);
      if (overflow.token == NW_CSS_PropValue_hidden) {
        NW_GDI_Rectangle_t temp;
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( NW_LMgr_BoxOf( box ) );
        if (!NW_GDI_Rectangle_Cross(&rect, &boxBounds, &temp)) {
          return NW_FALSE;
        }        
        rect = temp;
      }
    }

    box = (NW_LMgr_Box_t*)box->parent;
  }

  if (bounds != NULL) {
    *bounds = rect;
  }
  return NW_TRUE;
}

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_Box_Refresh
 * Description:  Redraws the box and its descendants.  Its behavior is only 
 *               guaranteed to be correct if "overflow: hidden" is set.  Any
 *               already displayed overflows will not be cleared.
 *               The usage of this method should be limited to the cases
 *               where an _isolated_ part of the box tree needs to be updated;
 *               in all other cases, the whole tree should be redrawn.
 *               Refresh CANNOT be called on the root box.
 * Returns:      KBrsrSuccess.
 */

TBrowserStatusCode
NW_LMgr_Box_Refresh (NW_LMgr_Box_t* box)
{
  NW_LMgr_RootBox_t* rootBox;

  NW_ASSERT (!NW_Object_IsClass(box, &NW_LMgr_RootBox_Class));

  /* Get the rootbox */
  rootBox = NW_LMgr_Box_GetRootBox (box);
//  if( !rootBox || !rootBox->renderedBoxes ){
  if( !rootBox ){
    // refresh might be called even if the rendered boxes are null (when the cache is invalidated)
    return KBrsrSuccess;
    }

  return NW_LMgr_RootBox_Refresh(rootBox, box);
}

void
NW_LMgr_Box_SetHasFocus(NW_LMgr_Box_t* box, NW_Bool hasFocus)
{
  NW_LMgr_Box_t* thisObj;

  // for convenience 
  thisObj = NW_LMgr_BoxOf (box);

  thisObj->hasFocus = hasFocus;
}

NW_Bool
NW_LMgr_Box_HasFocus(NW_LMgr_Box_t* box)
{
  NW_LMgr_Box_t* thisObj;

  // for convenience 
  thisObj = NW_LMgr_BoxOf (box);

  return thisObj->hasFocus;
}

NW_GDI_Rectangle_t 
NW_LMgr_Box_GetFormatBounds( NW_LMgr_Box_t* aBox )
  {
  if( aBox )
    {
    return aBox->iFormatBounds;
    }
  else
    {
    NW_ASSERT( 0 );
    NW_GDI_Rectangle_t invalidRect;
    invalidRect.dimension.height = -1;
    invalidRect.dimension.width = -1;
    invalidRect.point.x = -1;
    invalidRect.point.y = -1;
    return invalidRect;
    }
  }

void NW_LMgr_Box_SetFormatBounds( NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aFormatBounds )
  {
  if( aBox )
    {
    aBox->iFormatBounds = aFormatBounds;
    }
  }

NW_GDI_Rectangle_t 
NW_LMgr_Box_GetDisplayBounds( NW_LMgr_Box_t* aBox )
  {
  if( aBox )
    {
    return aBox->iDisplayBounds;
    }
  else
    {
    NW_GDI_Rectangle_t invalidRect;
    invalidRect.dimension.height = -1;
    invalidRect.dimension.width = -1;
    invalidRect.point.x = -1;
    invalidRect.point.y = -1;
    return invalidRect;
    }
  }

void NW_LMgr_Box_UpdateDisplayBounds( NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aDisplayBounds )
  {
  if( aBox )
    {
    aBox->iDisplayBounds = aDisplayBounds;
    }
  }

  /**
* NW_LMgr_Box_UpdateContainerBodyBox
* @description This method updates hight for displayBounds and formatBounds
*  of bodybox container to take into account body bottom margin. 
* it is called only once after page load complete
* @param NW_LMgr_RootBox_t* aRootBox: pointer to the rootBox
*
* @return void
*/
void NW_LMgr_Box_UpdateContainerBodyBox( NW_LMgr_RootBox_t* aRootBox )
{
	NW_GDI_Metric_t bottomMargin = 0;
	NW_LMgr_Box_t* bodyBox = NW_LMgr_RootBox_GetBody(aRootBox);
	if(bodyBox != NULL)
	{
		
		NW_LMgr_ContainerBox_s *parent = bodyBox->parent;
		if(parent != NULL)
		{
			
			bottomMargin =
                (NW_GDI_Metric_t)NW_LMgr_Box_GetModelProp(bodyBox,
                        NW_CSS_Prop_bottomMargin);
            if(bottomMargin > 0)
            {
				NW_GDI_Rectangle_t formatBounds =
				NW_LMgr_Box_GetFormatBounds( (NW_LMgr_Box_t* )parent);
    			formatBounds.dimension.height += bottomMargin;
    			(void) NW_LMgr_Box_SetFormatBounds( (NW_LMgr_Box_t* )parent, formatBounds );
    			NW_GDI_Rectangle_t displayBounds = NW_LMgr_Box_GetDisplayBounds((NW_LMgr_Box_t*) parent);
   				displayBounds.dimension.height += bottomMargin;
 				((NW_LMgr_Box_t* )parent)->iDisplayBounds.dimension.height =
 				displayBounds.dimension.height; 			
			}
		}
	}
 		
}
/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_LMgr_Box_Initialize (NW_LMgr_Box_t* box,
                        NW_ADT_Vector_Metric_t numProperties)
{
  return NW_Object_Initialize (&NW_LMgr_Box_Class, box, numProperties);
}

/* ------------------------------------------------------------------------- */
NW_LMgr_Box_t*
NW_LMgr_Box_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_Box_t*)
    NW_Object_New (&NW_LMgr_Box_Class, (NW_Uint32)numProperties);
}


#ifdef _DEBUG
/*lint -save -esym(960, 87) -e64 Only preprocessor statements and comments before '#include', Type mismatch */

#include "LMgrAnonBlock.h"
#include "LMgrAnonTableBox.h"
#include "LMgrMarkerImage.h"
#include "LMgrMarkerText.h"
#include "LMgrObjectBoxOOC.h"
#include "nw_fbox_buttonbox.h"
#include "nw_fbox_checkbox.h"
#include "nw_fbox_fileselectionbox.h"
#include "nw_fbox_formbox.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_optgrpbox.h"
#include "nw_fbox_optionbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_fbox_hiddenbox.h"
#include "nw_fbox_radiobox.h"
#include "nw_fbox_selectbox.h"
#include "nw_fbox_textareabox.h"
#include "nw_lmgr_activebox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_areabox.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_lmgr_emptybox.h"
#include "nw_lmgr_imagebox.h"
#include "nw_lmgr_flowbox.h"
#include "nw_lmgr_formatbox.h"
#include "nw_lmgr_linebox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_mediabox.h"
#include "nw_lmgr_nobrbox.h"
#include "nw_lmgr_rulebox.h"
#include "nw_lmgr_splittextbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_textbox.h"



#define countof(x) (sizeof(x)/sizeof(x[0]))

typedef struct
    {
    const NW_Object_Class_t* objClass;
    const NW_Int8* className;
    } ClassNameItem_t;

const ClassNameItem_t classNames[] =
    {
        {&NW_FBox_ButtonBox_Class, (NW_Int8*)"FBox_ButtonBox"},
        {&NW_FBox_CheckBox_Class, (NW_Int8*)"FBox_CheckBox"},
        {&NW_FBox_FileSelectionBox_Class, (NW_Int8*)"FBox_FileSelectionBox"},
        {&NW_FBox_ImageButtonBox_Class, (NW_Int8*)"FBox_ImageButtonBox"},
        {&NW_FBox_InputBox_Class, (NW_Int8*)"FBox_InputBox"},
        {&NW_FBox_OptGrpBox_Class, (NW_Int8*)"FBox_OptGrpBox"},
        {&NW_FBox_OptionBox_Class, (NW_Int8*)"FBox_OptionBox"},
        {&NW_FBox_PasswordBox_Class, (NW_Int8*)"FBox_PasswordBox"},
        {&NW_FBox_HiddenBox_Class, (NW_Int8*)"FBox_HiddenBox"},
        {&NW_FBox_RadioBox_Class, (NW_Int8*)"FBox_RadioBox"},
        {&NW_FBox_SelectBox_Class, (NW_Int8*)"FBox_SelectBox"},
        {&NW_FBox_TextAreaBox_Class, (NW_Int8*)"FBox_TextAreaBox"},
        {&NW_LMgr_AbstractTextBox_Class, (NW_Int8*)"AbstractTextBox"},
        {&NW_LMgr_ActiveBox_Class, (NW_Int8*)"ActiveBox"},
        {&NW_LMgr_ActiveContainerBox_Class, (NW_Int8*)"ActiveContainerBox"},
        {&NW_LMgr_AnimatedImageBox_Class, (NW_Int8*)"AnimatedImageBox"},
        {&LMgrAnonBlock_Class, (NW_Int8*)"AnonBlockBox"},
        {&LMgrAnonTableBox_Class, (NW_Int8*)"AnonTableBox"},
        {&NW_LMgr_AreaBox_Class, (NW_Int8*)"AreaBox"},
        {&NW_LMgr_BidiFlowBox_Class, (NW_Int8*)"BidiFlowBox"},
        {&NW_LMgr_BreakBox_Class, (NW_Int8*)"BreakBox"},
        {&NW_LMgr_ContainerBox_Class, (NW_Int8*)"ContainerBox"},
        {&NW_LMgr_EmptyBox_Class, (NW_Int8*)"EmptyBox"},
        {&NW_LMgr_FlowBox_Class, (NW_Int8*)"FlowBox"},
        {&NW_LMgr_FormatBox_Class, (NW_Int8*)"FormatBox"},
        {&NW_LMgr_ImageBox_Class, (NW_Int8*)"ImageBox"},
        {&NW_LMgr_ImgContainerBox_Class, (NW_Int8*)"ImgContainerBox"},
        {&NW_LMgr_ImageMapBox_Class, (NW_Int8*)"ImageMapBox"},
        {&NW_LMgr_LineBox_Class, (NW_Int8*)"LineBox"},
        {&LMgrMarkerImage_Class, (NW_Int8*)"MarkerImageBox"},
        {&LMgrMarkerText_Class, (NW_Int8*)"MarkerTextBox"},
        {&NW_LMgr_MarqueeBox_Class, (NW_Int8*)"MarqueeBox"},
        {&NW_LMgr_MediaBox_Class, (NW_Int8*)"MediaBox"},
        {&NW_LMgr_NobrBox_Class, (NW_Int8*)"NobrBox"},
        {&NW_LMgr_ObjectBox_Class, (NW_Int8*)"ObjectBox"},
        {&NW_LMgr_PosFlowBox_Class, (NW_Int8*)"PositionedBox"},
        {&NW_LMgr_RootBox_Class, (NW_Int8*)"RootBox"},
        {&NW_LMgr_RuleBox_Class, (NW_Int8*)"RuleBox"},
        {&NW_LMgr_SplitTextBox_Class, (NW_Int8*)"SplitTextBox"},
        {&NW_LMgr_StaticTableBox_Class, (NW_Int8*)"StaticTableBox"},
        {&NW_LMgr_StaticTableCellBox_Class, (NW_Int8*)"StaticTableCellBox"},
        {&NW_LMgr_StaticTableRowBox_Class, (NW_Int8*)"StaticTableRowBox"},
        {&NW_LMgr_TextBox_Class, (NW_Int8*)"TextBox"},
        {&NW_LMgr_VerticalTableBox_Class, (NW_Int8*)"VerticalTableBox"},
        {&NW_LMgr_VerticalTableCellBox_Class, (NW_Int8*)"VerticalTableCellBox"},
        {&NW_LMgr_VerticalTableRowBox_Class, (NW_Int8*)"VerticalRowBox"},
    };
#define CLASS_NAMES_COUNT countof(classNames)

const NW_Int8*
NW_LMgr_Box_GetBoxType (NW_LMgr_Box_t *box) {

  for (int i = 0; i < CLASS_NAMES_COUNT; i++) {
      if (classNames[i].objClass == box->super.super.super.objClass) {
          return classNames[i].className;
      }
  }

  return (NW_Int8*)"Unknown box: update NW_LMgr_Box_GetBoxType and/or the classNames table in LMgrBox.cpp.";
}

/* ------------------------------------------------------------------------- */
/* HACK!  HACK!  HACK!  
 * This is here ONLY for debugging purposes.  The proper way to do this 
 * (making GetBoxInfo virtual on box) would clutter the Box interface
 * too much.
 */


static
NW_Uint16
GetBoxLevel (NW_LMgr_Box_t *box, NW_LMgr_Box_t *rootBox) {
  NW_Uint16 level;

  level = 0;
  while (box != rootBox) {
    level ++;
    box = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent (box);
    NW_ASSERT (box != NULL);
  }

  return level;
}

static
void
PrintBoxInfo (NW_LMgr_Box_t *box, NW_Uint16 level) {

  const NW_Int8 *typeStr;
  NW_Ucs2 *textBoxStr = NULL;
  NW_Ucs2 indentBuf[256];
  NW_Ucs2 typeBuf[64];
  NW_Ucs2 tableInfo[64];
  NW_Ucs2 tableInfoFormat[] = { 'S', '%', 'd', ':', 'B', '%', 'd', ':', 
                                'H', '%', 'd', ':', 'V', '%', 'd', '\0' };
  NW_Uint16 i;
  NW_LMgr_StaticTableBox_t *tableBox = NULL;
  NW_Uint16 numBorderInfos = 0;
  NW_Uint16 numSpans = 0;
  NW_Uint16 numHEdges = 0;
  NW_Uint16 numVEdges = 0;
  NW_GDI_Rectangle_t bounds;
  NW_GDI_Rectangle_t dispBounds;
  NW_Text_t *displayText = NULL;

  typeStr = NW_LMgr_Box_GetBoxType(box);

  if (NW_Object_IsInstanceOf (box, &NW_LMgr_AbstractTextBox_Class)) {
    displayText = NW_LMgr_AbstractTextBox_GetDisplayText(NW_LMgr_AbstractTextBoxOf(box), NW_TRUE);
    if( displayText )
    textBoxStr = NW_Text_GetUCS2Buffer (displayText,
                                        NW_Text_Flags_Copy, NULL, NULL);
    if (textBoxStr) {
      NW_Str_Strcharreplace (textBoxStr, '\n', '>');

      // Trunc the text to 900 (NW_Debug_Log doesn't allow a print statement larger than 1024).
      if (displayText->characterCount > 900) {
        textBoxStr[900] = '\0';
      }
    }
  }

  //NW_Str_StrcpyConst(typeBuf, (char*)typeStr);
  /* Get the box bounds */
  bounds = NW_LMgr_Box_GetFormatBounds(box);
  dispBounds = NW_LMgr_Box_GetDisplayBounds (box);
  
  /* Indent the line */
  for (i=0; i<level*2 && i < 254; i++) {
    indentBuf[i] = ' ';
  }
  indentBuf[i] = '\0';

  /* Print the info */
  if (textBoxStr) 
    {
    NW_LOG12(NW_LOG_LEVEL3, "%s%s %X [bounds:(%d,%d)-(%d,%d), dispBounds:(%d,%d)-(%d,%d)\"%s\"]",
                           indentBuf,
                           typeBuf,
                           box,
                           (NW_Uint16)bounds.point.x, 
                           (NW_Uint16)bounds.point.y,
                           (NW_Uint16)bounds.dimension.width, 
                           (NW_Uint16)bounds.dimension.height,
                           (NW_Uint16)dispBounds.point.x, 
                           (NW_Uint16)dispBounds.point.y,
                           (NW_Uint16)dispBounds.dimension.width, 
                           (NW_Uint16)dispBounds.dimension.height,
                           textBoxStr);

    NW_Str_Delete (textBoxStr);
    NW_Object_Delete(displayText);
    }
  else if (tableBox) 
    {
    NW_Str_Sprintf(tableInfo, tableInfoFormat, 
                       (NW_Uint32)numSpans,
                       (NW_Uint32)numBorderInfos,
                       (NW_Uint32)numHEdges,
                       (NW_Uint32)numVEdges);

    NW_LOG12(NW_LOG_LEVEL3, "%s%s %X [bounds:(%d,%d)-(%d,%d), dispBounds:(%d,%d)-(%d,%d) %s]", 
                           indentBuf,
                           typeBuf,
                           box,
                           (NW_Uint16)bounds.point.x, 
                           (NW_Uint16)bounds.point.y,
                           (NW_Uint16)bounds.dimension.width, 
                           (NW_Uint16)bounds.dimension.height,
                           (NW_Uint16)dispBounds.point.x, 
                           (NW_Uint16)dispBounds.point.y,
                           (NW_Uint16)dispBounds.dimension.width, 
                           (NW_Uint16)dispBounds.dimension.height,
                           tableInfo);
    }
  else 
    {
    NW_LOG11(NW_LOG_LEVEL3, "%s%s %X [bounds:(%d,%d)-(%d,%d), dispBounds:(%d,%d)-(%d,%d)]",
                           indentBuf,
                           typeBuf,
                           box,
                           (NW_Uint16)bounds.point.x, 
                           (NW_Uint16)bounds.point.y,
                           (NW_Uint16)bounds.dimension.width, 
                           (NW_Uint16)bounds.dimension.height,
                           (NW_Uint16)dispBounds.point.x, 
                           (NW_Uint16)dispBounds.point.y,
                           (NW_Uint16)dispBounds.dimension.width, 
                           (NW_Uint16)dispBounds.dimension.height);
    if( NW_Object_IsInstanceOf (box, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
      {
      NW_LMgr_StaticTableContext_t *ctx = NW_LMgr_StaticTableBoxOf( box )->ctx;
      if( ctx != NULL )
        {
        NW_LOG4(NW_LOG_LEVEL3, "%s%s     \"    [Table - Min,Max:(%d,%d)]",
                           indentBuf,
                           typeBuf,
                           (NW_Uint16)ctx->tableMinWidth, 
                           (NW_Uint16)ctx->tableMaxWidth );
        }
      }
    else if( NW_Object_IsInstanceOf (box, &NW_LMgr_StaticTableCellBox_Class ) == NW_TRUE )
      {
      NW_LOG4(NW_LOG_LEVEL3, "%s%s     \"    [Cell - Min,Max:(%d,%d)]",
                           indentBuf,
                           typeBuf,
                           NW_LMgr_StaticTableCellBoxOf( box )->minWidth, 
                           NW_LMgr_StaticTableCellBoxOf( box )->maxWidth);
      }
    }
  }


TBrowserStatusCode
NW_LMgr_Box_DumpBoxTree(NW_LMgr_Box_t* box)
{
  NW_LMgr_BoxVisitor_t boxVisitor;
  NW_LMgr_Box_t *current;
  NW_Uint16 level;

  /* Initialize the BoxVisitor */
  NW_LMgr_BoxVisitor_Initialize (&boxVisitor, box);

  /* Dump the subtree */
  NW_LOG0(NW_LOG_LEVEL3, "Printing the box tree...");
  NW_LOG0(NW_LOG_LEVEL3, "N.B.:  The displayed class names could be names of superclasses!"); 
  while((current = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL) {

    level = GetBoxLevel (current, box);

    /* Get the boxInfo for the current box */
    PrintBoxInfo (current, level);
  }

  return KBrsrSuccess;
}

/*lint -restore*/
#endif /* _DEBUG */


