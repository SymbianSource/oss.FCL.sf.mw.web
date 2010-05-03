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


#include "nw_lmgr_posflowboxi.h"
#include "nw_lmgr_formatcontext.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_cssproperties.h"
#include "NW_LMgr_IEventListener.h"
#include "GDIDevicecontext.h"
#include "nw_lmgr_verticaltablecellboxi.h"
#include "nw_lmgr_abstracttextboxi.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_breakbox.h"

/* --------------------------------------------------------------------------*
 *                          FORWARD DECLARATIONS                               *
 * --------------------------------------------------------------------------*/
static void NW_LMgr_PosFlowBox_GetMinSize(NW_LMgr_Box_t* aBox,
                                          NW_GDI_Dimension2D_t& aMinSize);


static NW_LMgr_Box_t* NW_LMgr_PosFlowBox_GetPrev(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_ASSERT(aPosFlowBox != NULL);
    NW_LMgr_Box_t* box = NW_LMgr_BoxOf(aPosFlowBox);
    NW_LMgr_ContainerBox_t* parent = NW_LMgr_Box_GetParent(box);
    NW_Int16 index = 0;
    NW_LMgr_Box_t* prevBox = NULL;
    do
        {
        index = NW_LMgr_ContainerBox_GetChildIndex(parent, box);
        if (index > 0)
            {
            break;
            }
        box = NW_LMgr_BoxOf(parent);
        parent = NW_LMgr_Box_GetParent(box);
        }
    while (parent != NULL);
    if (index > 0)
        {
        prevBox = NW_LMgr_ContainerBox_GetChild(parent, (NW_Uint16)(index - 1));
        }
    return prevBox;
    }

// NW_LMgr_PosFlowBox_CalculatePosition must be called prior to calling this function
static NW_GDI_Metric_t NW_LMgr_PosFlowBox_GetWidth(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_LMgr_Box_t* box = (NW_LMgr_Box_t*) aPosFlowBox;
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), (NW_ADT_Vector_Metric_t)0);
    NW_ASSERT(child != NULL); // This child is the box with position:absolue property
    NW_ASSERT(NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box)) == 1);
    
    NW_LMgr_RootBox_t *rootBox = NW_LMgr_Box_GetRootBox(box);
    CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT(deviceContext != NULL);
    NW_GDI_Metric_t width = deviceContext->DisplayBounds()->dimension.width;
    if (aPosFlowBox->iContainingBlock != NULL)
        {
        width = aPosFlowBox->iContainingBlock->iFormatBounds.dimension.width;
        }

    NW_LMgr_Property_t property;
    property.type = NW_CSS_ValueType_Token;
    TBrowserStatusCode status = NW_LMgr_Box_GetProperty(child, NW_CSS_Prop_width, &property);
    TBool hasWidth = EFalse;

    // Get the width is it is specified
    if (status == KBrsrSuccess)
        {
        switch (property.type)
            {
            case NW_CSS_ValueType_Px:
                width = property.value.integer;
                hasWidth = ETrue;
                box->iFormatBounds.dimension.width = width;
                child->iFormatBounds.dimension.width = width;
                break;
            case NW_CSS_ValueType_Percentage:
                {
                NW_Float32 percent = 0;
                percent = (property.value.decimal * width) / 100;
                width = (NW_GDI_Metric_t)percent; 
                hasWidth = ETrue;
                box->iFormatBounds.dimension.width = width;
                child->iFormatBounds.dimension.width = width;
                break;
                }
            default:
                break;
            }
        }
    if (!hasWidth)
        {
        // Both left and right are specified, calculate the width from that
        if (aPosFlowBox->iHasLeft && aPosFlowBox->iHasRight)
            {
            width -= (aPosFlowBox->iLeft + aPosFlowBox->iRight);
            }
        else if (aPosFlowBox->iLeft > 0)
            {
            if (aPosFlowBox->iLeft < width)
                {
                width -= aPosFlowBox->iLeft;
                }
            }
        else if (aPosFlowBox->iRight > 0)
            {
            if (aPosFlowBox->iRight < width)
                {
                width -= aPosFlowBox->iRight;
                }
            }
        }
    if (width < NW_LMGR_BOX_MIN_WIDTH)
        {
        width = NW_LMGR_BOX_MIN_WIDTH;
        }
    return width;
    }

// NW_LMgr_PosFlowBox_CalculatePosition must be called prior to calling this function
static NW_GDI_Metric_t NW_LMgr_PosFlowBox_GetHeight(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_LMgr_Box_t* box = (NW_LMgr_Box_t*) aPosFlowBox;
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), (NW_ADT_Vector_Metric_t)0);
    NW_ASSERT(child != NULL); // This child is the box with position:absolue property
    NW_ASSERT(NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box)) == 1);
    
    NW_LMgr_RootBox_t *rootBox = NW_LMgr_Box_GetRootBox(box);
    CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT(deviceContext != NULL);
    NW_GDI_Metric_t height = deviceContext->DisplayBounds()->dimension.height;
    if (aPosFlowBox->iContainingBlock != NULL)
        {
        height = aPosFlowBox->iContainingBlock->iFormatBounds.dimension.height;
        }

    NW_LMgr_Property_t property;
    property.type = NW_CSS_ValueType_Token;
    TBrowserStatusCode status = NW_LMgr_Box_GetProperty(child, NW_CSS_Prop_height, &property);
    TBool hasHeight = EFalse;

    // Get the width is it is specified
    if (status == KBrsrSuccess)
        {
        switch (property.type)
            {
            case NW_CSS_ValueType_Px:
                height = property.value.integer;
                hasHeight = ETrue;
                box->iFormatBounds.dimension.height = height;
                child->iFormatBounds.dimension.height = height;
                break;
            case NW_CSS_ValueType_Percentage:
                {
                NW_Float32 percent = 0;
                percent = (property.value.decimal * height) / 100;
                height = (NW_GDI_Metric_t)percent; 
                hasHeight = ETrue;
                box->iFormatBounds.dimension.height = height;
                child->iFormatBounds.dimension.height = height;
                break;
                }
            default:
                break;
            }
        }
    if (!hasHeight)
        {
        // Both top and bottom are specified, calculate the height from that,
        // and set it on the box
        if (aPosFlowBox->iHasTop && aPosFlowBox->iHasBottom)
            {
            height -= (aPosFlowBox->iTop + aPosFlowBox->iBottom);
            NW_LMgr_Property_t prop;
            prop.type = NW_CSS_ValueType_Px;
            prop.value.integer = height;
            NW_LMgr_Box_SetProperty (child, NW_CSS_Prop_height, &prop);
            }
        else if (aPosFlowBox->iTop > 0)
            {
            height -= aPosFlowBox->iTop;
            }
        else if (aPosFlowBox->iBottom > 0)
            {
            height -= aPosFlowBox->iBottom;
            }
        }
    if (height < NW_LMGR_BOX_MIN_HEIGHT)
        {
        height = NW_LMGR_BOX_MIN_HEIGHT;
        }
    return height;
    }

#if 0
static void NW_LMgr_PosFlowBox_SetProperty(NW_LMgr_PosFlowBox_t* aPosFlowBox, NW_LMgr_Property_t* aProp,
                                      NW_LMgr_PropertyName_t aPropName)
    {
    NW_LMgr_Property_t prop;
    prop.value.object = NULL;
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(aPosFlowBox),
        (NW_ADT_Vector_Metric_t)0);
    NW_LMgr_Box_SetProperty(child, aPropName, aProp);
    if (NW_Object_IsInstanceOf(child, &NW_LMgr_ContainerBox_Class))
        {
        (void)NW_LMgr_Box_GetProperty(child, NW_CSS_Prop_sibling, &prop);
        NW_LMgr_ContainerBox_t* siblingBox;
        while(prop.value.object && (NW_LMgr_BoxOf(prop.value.object) != child))
            {
            siblingBox = NW_LMgr_ContainerBoxOf(prop.value.object);
            NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(siblingBox), aPropName, aProp);
            prop.value.object = NULL;
            (void)NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(siblingBox), NW_CSS_Prop_sibling, &prop);
            }
        }
    }
#endif

static void NW_LMgr_PosFlowBox_GetMinContainerSize(NW_LMgr_Box_t* aBox,
                                                   NW_GDI_Dimension2D_t& aMinSize)
    {
    NW_ADT_Vector_Metric_t count = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aBox));
    NW_ADT_Vector_Metric_t index;
    NW_GDI_Dimension2D_t size;
    NW_GDI_Dimension2D_t prevSize;
    size.width = prevSize.width = 0;

    for (index = 0; index < count; index++)
        {
        NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(aBox), index);
        NW_LMgr_PosFlowBox_GetMinSize(child, size);
        if (size.width > prevSize.width)
            {
            prevSize.width = size.width;
            }
        }
    aMinSize.width = prevSize.width;
    }

static void NW_LMgr_PosFlowBox_GetMinSize(NW_LMgr_Box_t* aBox,
                                          NW_GDI_Dimension2D_t& aMinSize)
    {
    if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_AbstractTextBox_Class))
        {
        (void)NW_LMgr_AbstractTextBox_GetMinimumSizeByWord(NW_LMgr_AbstractTextBoxOf(aBox), &aMinSize);
        }
    else if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_StaticTableBox_Class))
        {
        NW_LMgr_PosFlowBox_GetMinContainerSize(aBox, aMinSize);
        }
    else if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_PosFlowBox_Class))
        {
        aMinSize.width = 0;;
        }
    else if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_StaticTableRowBox_Class))
        {
        NW_ADT_Vector_Metric_t count = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aBox));
        NW_GDI_Dimension2D_t size;
        NW_ADT_Vector_Metric_t index;
        aMinSize.width = 0;

        for (index = 0; index < count; index++)
            {
            NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(aBox), index);
            NW_LMgr_PosFlowBox_GetMinSize(child, size);
            aMinSize.width += size.width;
            }
        }

    else if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_ContainerBox_Class))
        {
        NW_LMgr_PosFlowBox_GetMinContainerSize(aBox, aMinSize);
        }
    else 
        {
        aMinSize.width = aBox->iFormatBounds.dimension.width;
        }
    }


// Adjusting the size is needed if the positioned box initial width was not a good choice
// That could happen if the "left" property is bigger than the width of the containing block,
// or if the "right" property was specified, and the positioned box is wider than the content
// If the width property exists, then we just use that, and no need to 
static TBool NW_LMgr_PosFlowBox_AdjustSizeNeeded(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    const TInt extraSpace = 10;
    NW_LMgr_Box_t* box = (NW_LMgr_Box_t*) aPosFlowBox;
    NW_LMgr_Box_t* firstChild = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), (NW_ADT_Vector_Metric_t)0);
    NW_ASSERT(firstChild != NULL); // must have at least one child
    NW_LMgr_Property_t widthProp;
    TBrowserStatusCode status = NW_LMgr_Box_GetProperty(firstChild, NW_CSS_Prop_width, &widthProp);
    // The width was not specified
    if (status == KBrsrNotFound)
        {
        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(aPosFlowBox);
        NW_ASSERT(rootBox != NULL);
        CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
        NW_ASSERT(deviceContext != NULL);

        NW_GDI_Rectangle_t displayBounds;
        if (aPosFlowBox->iContainingBlock == NULL)
            {
            displayBounds = (*deviceContext->DisplayBounds());
            }
        else
            {
			 if( (aPosFlowBox->iContainingBlock != NULL) && 
				  NW_Object_IsInstanceOf(aPosFlowBox->iContainingBlock, &NW_LMgr_Box_Class) )
			 {
              displayBounds = NW_LMgr_Box_GetFormatBounds(NW_LMgr_BoxOf(aPosFlowBox->iContainingBlock));
			 }
			 else
			 {
			  displayBounds = (*deviceContext->DisplayBounds());
			 }
            }

        NW_GDI_Dimension2D_t size;
        size.width = box->iFormatBounds.dimension.width;

        // If the default width was too large, just adjust it
        if ((NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aPosFlowBox)) == 1) &&
            (firstChild->iFormatBounds.point.x == box->iFormatBounds.point.x) &&
            (firstChild->iFormatBounds.dimension.width < box->iFormatBounds.dimension.width - extraSpace))
            {
            box->iFormatBounds.dimension.width = firstChild->iFormatBounds.dimension.width + 5;
            }
        // If we have a positioned <div> element whose content is not as wide as the screen
        // By default <div> gets the width of the screen. This is a problem only if the <div> has a 
        // "right" attribute
        else if (aPosFlowBox->iHasRight)
            {
            if (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aPosFlowBox)) == 1)
                {
                if (NW_Object_IsInstanceOf(firstChild, &NW_LMgr_ContainerBox_Class))
                    {
                    NW_LMgr_Box_t* theChild = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(firstChild),
                        (NW_ADT_Vector_Metric_t)0);
                    if (theChild->iFormatBounds.dimension.width < firstChild->iFormatBounds.dimension.width + extraSpace)
                        {
                        size.width = theChild->iFormatBounds.dimension.width + 5;
                        }
                    }
                }
            }
        else if (aPosFlowBox->iHasLeft && (aPosFlowBox->iLeft > displayBounds.dimension.width))
            {
            NW_LMgr_PosFlowBox_GetMinContainerSize(box, size);
            // Our algorithm is not accurate and "chops off" the last letter
            size.width += 5;
            }

        if (size.width < box->iFormatBounds.dimension.width - extraSpace)
            {
            NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(aPosFlowBox);
            MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
            boxTreeListener->Collapse(NW_LMgr_FlowBoxOf(aPosFlowBox), NW_TRUE);

            NW_LMgr_BoxVisitor_t visitor;
            NW_LMgr_Box_t* nextBox = NULL;
            NW_LMgr_BoxVisitor_Initialize(&visitor, box);
            NW_GDI_Rectangle_t boxBounds;
            NW_Bool skip = NW_FALSE;

            //nextBox = NW_LMgr_BoxVisitor_NextBox(&visitor, &skip);
            NW_Mem_memset(&(boxBounds), 0, sizeof(NW_GDI_Rectangle_t));
            while ((nextBox = NW_LMgr_BoxVisitor_NextBox(&visitor, &skip)) != NULL)
                {
                NW_LMgr_Box_SetFormatBounds( nextBox, boxBounds );
                NW_LMgr_Box_UpdateDisplayBounds( nextBox, boxBounds );
                }
            NW_LMgr_Property_t property;
            property.type = NW_CSS_ValueType_Px;
            property.value.integer = size.width;
            firstChild = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), (NW_ADT_Vector_Metric_t)0);
            NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(firstChild), NW_CSS_Prop_width, &property);
            return ETrue;
            }
        }
    return EFalse;
    }


const
NW_LMgr_PosFlowBox_Class_t  NW_LMgr_PosFlowBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_FlowBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_PosFlowBox_t),
    /* construct                 */ _NW_LMgr_PosFlowBox_Construct,
    /* destruct                  */ _NW_LMgr_PosFlowBox_Destruct
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_PosFlowBox_Split,
    /* resize                    */ _NW_LMgr_PosFlowBox_Resize,
    /* postResize                */ _NW_LMgr_PosFlowBox_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_PosFlowBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_PosFlowBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_PosFlowBox_Constrain,
    /* draw                      */ _NW_LMgr_PosFlowBox_Draw,
    /* render                    */ _NW_LMgr_PosFlowBox_Render,
    /* getBaseline               */ _NW_LMgr_PosFlowBox_GetBaseline,
    /* shift                     */ _NW_LMgr_PosFlowBox_Shift,
    /* clone                     */ _NW_LMgr_PosFlowBox_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox         */
    /* applyFormatProps          */ _NW_LMgr_PosFlowBox_ApplyFormatProps
  },
  { /* NW_LMgr_FlowBox           */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_BidiFlowBox       */
    /* unused                    */ NW_Object_Unused
  }
};






/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_PosFlowBox_t*
NW_LMgr_PosFlowBox_New (NW_ADT_Vector_Metric_t numProperties)
    {
    NW_LMgr_PosFlowBox_t* box = (NW_LMgr_PosFlowBox_t*)
        NW_Object_New (&NW_LMgr_PosFlowBox_Class, numProperties);
    return box;
    }

// -----------------------------------------------------------------------------
// _NW_LMgr_PosFlowBox_Destruct
// Description:  The destructor.  
// Returns:      void
// -----------------------------------------------------------------------------
//
void
_NW_LMgr_PosFlowBox_Destruct( NW_Object_Dynamic_t* /*dynamicObject */)
    {
    }


// Helper function that copies the position to class members
TBrowserStatusCode NW_LMgr_PosFlowBox_AssignPos(NW_LMgr_PropertyName_t aPropertyName,
                                                NW_Int32 &aValue, NW_LMgr_Box_t* aBox)
    {
    NW_LMgr_Property_t property;
    property.type = NW_CSS_ValueType_Token;
    TBrowserStatusCode status = NW_LMgr_Box_GetProperty(aBox, aPropertyName, &property);
    if (status == KBrsrSuccess)
        {
        switch (property.type)
            {
            case NW_CSS_ValueType_Px:
                aValue = property.value.integer;
                break;
            case NW_CSS_ValueType_Percentage:
                {
                NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(aBox);
                NW_ASSERT(rootBox != NULL);
                CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
                NW_ASSERT(deviceContext != NULL);
                const NW_GDI_Rectangle_t* displayBounds = deviceContext->DisplayBounds();
                NW_ASSERT(displayBounds != NULL);
                NW_Float32 percent = 0;
                if (aPropertyName == NW_CSS_Prop_left || aPropertyName == NW_CSS_Prop_right)
                    {
                    percent = (property.value.decimal * displayBounds->dimension.width) / 100;
                    }
                else if (aPropertyName == NW_CSS_Prop_top || aPropertyName == NW_CSS_Prop_bottom)
                    {
                    percent = (property.value.decimal * displayBounds->dimension.height) / 100;
                    }

                aValue = (NW_Int32)percent;            
                break;
                }
            default:
                break;
            }
        }
    return status;
    }

void NW_LMgr_PosFlowBox_CalculatePosition(NW_LMgr_PosFlowBox_t* aPosFlowBox, NW_LMgr_Box_t* aBox)
    {
    NW_ASSERT(aPosFlowBox != NULL);
    NW_ASSERT(aBox != NULL);
    NW_LMgr_PropertyValue_t positionVal;
    positionVal.token = NW_CSS_PropValue_none;

    (void)NW_LMgr_Box_GetPropertyValue(aBox, NW_CSS_Prop_position,
                                       NW_CSS_ValueType_Token, &positionVal);
    NW_ASSERT(positionVal.token == NW_CSS_PropValue_position_absolute);

    if (NW_LMgr_PosFlowBox_AssignPos(NW_CSS_Prop_left, aPosFlowBox->iLeft, aBox) == KBrsrSuccess)
        {
        aPosFlowBox->iHasLeft = NW_TRUE;
        }
    if (NW_LMgr_PosFlowBox_AssignPos(NW_CSS_Prop_right, aPosFlowBox->iRight, aBox) == KBrsrSuccess)
        {
        aPosFlowBox->iHasRight = NW_TRUE;
        }
    if (NW_LMgr_PosFlowBox_AssignPos(NW_CSS_Prop_top, aPosFlowBox->iTop, aBox) == KBrsrSuccess)
        {
        aPosFlowBox->iHasTop= NW_TRUE;
        }
    if (NW_LMgr_PosFlowBox_AssignPos(NW_CSS_Prop_bottom, aPosFlowBox->iBottom, aBox) == KBrsrSuccess)
        {
        aPosFlowBox->iHasBottom = NW_TRUE;
        }
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp)
    {
    TBrowserStatusCode status = _NW_LMgr_ContainerBox_Construct(dynamicObject, argp);
    NW_LMgr_PosFlowBox_t* box = (NW_LMgr_PosFlowBox_t*) dynamicObject;
    if (status == KBrsrSuccess)
        {
        box->iLeft = 0;
        box->iRight = 0;
        box->iTop = 0;
        box->iBottom = 0;
        box->iHasLeft = NW_FALSE;
        box->iHasRight = NW_FALSE;
        box->iHasTop = NW_FALSE;
        box->iHasBottom = NW_FALSE;
        box->iContainingBlock = NULL;
        }
    return status;
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Split(NW_LMgr_Box_t* box, 
                           NW_GDI_Metric_t space, 
                           NW_LMgr_Box_t** splitBox,
                           NW_Uint8 flags)
    {
    return _NW_LMgr_Box_Split(box, space, splitBox, flags);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context)
    {
    return _NW_LMgr_FlowBox_Resize(box, context);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_PostResize(NW_LMgr_Box_t* box)
    {
    return _NW_LMgr_FlowBox_PostResize(box);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_GetMinimumContentSize(NW_LMgr_Box_t* box,
                                          NW_GDI_Dimension2D_t *size)
    {
    return _NW_LMgr_Box_GetMinimumContentSize(box, size);

    }

NW_Bool
_NW_LMgr_PosFlowBox_HasFixedContentSize(NW_LMgr_Box_t *box)
    {
    return _NW_LMgr_Box_HasFixedContentSize(box);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Constrain(NW_LMgr_Box_t* box, NW_GDI_Metric_t constraint)
    {
    return _NW_LMgr_FormatBox_Constrain(box, constraint);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Draw (NW_LMgr_Box_t* box,
                           CGDIDeviceContext* deviceContext,
                           NW_Uint8 hasFocus)
    {
    return _NW_LMgr_Box_Draw (box, deviceContext, hasFocus);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Render (NW_LMgr_Box_t* box,
                             CGDIDeviceContext* deviceContext,
                             NW_GDI_Rectangle_t* clipRect,
                             NW_LMgr_Box_t *currentBox,
                             NW_Uint8 flags,
                             NW_Bool* hasFocus,
                             NW_Bool* skipChildren,
                             NW_LMgr_RootBox_t* rootBox )
    {
    return _NW_LMgr_ContainerBox_Render (box, deviceContext, clipRect, currentBox,
        flags, hasFocus, skipChildren, rootBox );
    }

NW_GDI_Metric_t
_NW_LMgr_PosFlowBox_GetBaseline(NW_LMgr_Box_t* box)
    {
    return _NW_LMgr_FlowBox_GetBaseline(box);
    }

TBrowserStatusCode
_NW_LMgr_PosFlowBox_Shift (NW_LMgr_Box_t* box, 
                           NW_GDI_Point2D_t *delta)
    {
    return _NW_LMgr_ContainerBox_Shift (box, delta);
    }

NW_LMgr_Box_t*
_NW_LMgr_PosFlowBox_Clone (NW_LMgr_Box_t* box)
    {
    return _NW_LMgr_Box_Clone (box);
    }

// This function is a copy of _NW_LMgr_ContainerBox_ApplyFormatProps
// with the exception that the minimun size is the size of the root box
TBrowserStatusCode
_NW_LMgr_PosFlowBox_ApplyFormatProps(NW_LMgr_FormatBox_t* format, 
                                    NW_LMgr_FormatContext_t* context)
    {

    NW_LMgr_PosFlowBox_t* posBox = (NW_LMgr_PosFlowBox_t*) format;
    NW_LMgr_Box_t* box = (NW_LMgr_Box_t*) format;
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), (NW_ADT_Vector_Metric_t)0);
    NW_ASSERT(child != NULL); // This child is the box with position:absolue property
    NW_ASSERT(NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box)) == 1);
    NW_LMgr_PosFlowBox_CalculatePosition(posBox, child);
    
    NW_LMgr_RootBox_t *rootBox = NW_LMgr_Box_GetRootBox(box);
    NW_GDI_Metric_t minWidth;
    NW_LMgr_FrameInfo_t paddingInfo;
    NW_LMgr_FrameInfo_t borderWidthInfo;
    TBrowserStatusCode status = KBrsrSuccess;
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( format ) );
    NW_LMgr_Property_t dirProp;
    
    NW_LMgr_Box_GetPadding(NW_LMgr_BoxOf(format), &paddingInfo, ELMgrFrameAll );
    NW_LMgr_Box_GetBorderWidth(NW_LMgr_BoxOf(format), &borderWidthInfo, ELMgrFrameAll );
    
    minWidth = NW_LMgr_PosFlowBox_GetWidth(posBox);

    context->startMargin = 
        (NW_GDI_Metric_t)(boxBounds.point.x + paddingInfo.left
        + borderWidthInfo.left);
    context->endMargin = (NW_GDI_Metric_t)(boxBounds.point.x
        + boxBounds.dimension.width
//        + minWidth
        - paddingInfo.right
        - borderWidthInfo.right);
    
    /* Set initial vertical position */
    context->y = (NW_GDI_Metric_t) (boxBounds.point.y + paddingInfo.top
        + borderWidthInfo.top);
    
    if (context->endMargin < context->startMargin + minWidth) 
        {
        context->endMargin = (NW_GDI_Metric_t)(context->startMargin + minWidth);
        boxBounds.dimension.width =  (NW_GDI_Metric_t)(minWidth + paddingInfo.left + 
            borderWidthInfo.left + paddingInfo.right + borderWidthInfo.right);
        
        NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( format ), boxBounds );
        // 
        status = KBrsrLmgrFormatOverflow;   
        }
    boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( format ) );
    /* Now reset initial height. This will change as children are laid out */  
    boxBounds.dimension.height =  (NW_GDI_Metric_t)( paddingInfo.top + paddingInfo.bottom +
        borderWidthInfo.top + borderWidthInfo.bottom);
    
    NW_LMgr_PosFlowBox_GetHeight(posBox);
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( format ), boxBounds );
    
    if (!NW_Object_IsClass(format, &NW_LMgr_RootBox_Class)) {
        rootBox = NW_LMgr_Box_GetRootBox (format);
        NW_ASSERT (rootBox != NULL);
        }
    else {
        rootBox = NW_LMgr_RootBoxOf(format);
        }
    
    /* Now set the flow-specific context variables */
    context->lineStart = NW_LMgr_FlowBox_GetLineStart(NW_LMgr_FlowBoxOf(format),
        context, context->y);
    context->lineEnd = NW_LMgr_FlowBox_GetLineEnd(NW_LMgr_FlowBoxOf(format),
        context, context->y);
    
    /* Get the flow direction */
    dirProp.type = NW_CSS_ValueType_Token;
    dirProp.value.token = NW_CSS_PropValue_ltr;
    NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(format), NW_CSS_Prop_textDirection, &dirProp);
    if (dirProp.value.token == NW_CSS_PropValue_ltr) {
        context->direction = NW_GDI_FlowDirection_LeftRight;
        }
    else 
        {
        context->direction = NW_GDI_FlowDirection_RightLeft;
        }
    
    /* Set the initial x position */
    if (dirProp.value.token == NW_CSS_PropValue_ltr) {
        context->x = (NW_GDI_Metric_t)(context->lineStart + 
            NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBoxOf(format),
            context));
        }
    else {
        context->x = (NW_GDI_Metric_t)(context->lineEnd - 
            NW_LMgr_FlowBox_GetIndentation(NW_LMgr_FlowBoxOf(format),
            context));
        }
    return status;
    }




void NW_LMgr_PosFlowBox_AdjustPosition(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_ASSERT(aPosFlowBox != NULL);
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(aPosFlowBox), (NW_ADT_Vector_Metric_t)0);
    NW_ASSERT(child != NULL);

    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(aPosFlowBox);
    NW_ASSERT(rootBox != NULL);
    CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT(deviceContext != NULL);

    NW_GDI_Rectangle_t displayBounds;
    NW_LMgr_Box_t* containingBlock = NW_LMgr_BoxOf(aPosFlowBox->iContainingBlock);
    if (aPosFlowBox->iContainingBlock == NULL)
        {
        displayBounds = (*deviceContext->DisplayBounds());
        }
    else
        {
        displayBounds = NW_LMgr_Box_GetDisplayBounds(containingBlock);
        }
    NW_GDI_Point2D_t delta;

    NW_LMgr_FrameInfo_t margins;
    NW_LMgr_FrameInfo_t isAuto;
    NW_Int8 aFrameSide = ELMgrFrameAll;
    
    NW_LMgr_Box_GetMargins(child, &margins, &isAuto, aFrameSide);

    delta.x = delta.y = 0;
    if (aPosFlowBox->iHasLeft)
        {
        delta.x = aPosFlowBox->iLeft + displayBounds.point.x - child->iFormatBounds.point.x;
        if (!isAuto.left)
            {
            delta.x += margins.left;
            }
        }
    else if (aPosFlowBox->iHasRight)
        {
        delta.x = displayBounds.dimension.width + displayBounds.point.x - child->iFormatBounds.point.x -
            aPosFlowBox->iRight - child->iFormatBounds.dimension.width;
        if (!isAuto.right)
            {
            delta.x -= margins.right;
            }
        }
    else
        {
        NW_LMgr_Box_t* prevBox = NW_LMgr_PosFlowBox_GetPrev(aPosFlowBox);
        if (prevBox != NULL)
            {
            delta.x = prevBox->iFormatBounds.point.x + 
                prevBox->iFormatBounds.dimension.width - child->iFormatBounds.point.x;

            //if delta.x is too big, very close to the maximum, we need to add a new line.
            //we put 8 for temporary, more investigation needs to be done for a reasonable number
            //but first check if there already an breakbox as positionedbox sibling, if yes, don't
            //insert any breakbox
            if (!NW_Object_IsClass (prevBox, &NW_LMgr_BreakBox_Class) && 
                delta.x - displayBounds.dimension.width < 8)
                {
                  //add an empty box to force the line to go the next line
                   TBrowserStatusCode status = KBrsrSuccess;
                      /* Create the break box */
           
                    NW_LMgr_Box_t* breakBox = NULL;
                    breakBox = (NW_LMgr_Box_t*)NW_LMgr_BreakBox_New ((NW_ADT_Vector_Metric_t)1);
                    if (breakBox == NULL)
                        return;

                    NW_LMgr_ContainerBox_t* parent = NW_LMgr_Box_GetParent(aPosFlowBox);
                    status = NW_LMgr_ContainerBox_InsertChild(parent, 
                                                                NW_LMgr_BoxOf(breakBox), 
                                                                (NW_LMgr_Box_t*)aPosFlowBox);
                    if (status != KBrsrSuccess)
                        return;
                    
                    MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
                    boxTreeListener->ReformatBox(NW_LMgr_BoxOf(breakBox));

                    prevBox = NW_LMgr_PosFlowBox_GetPrev(aPosFlowBox);
                    if (prevBox != NULL)
                    {
                        delta.x = prevBox->iFormatBounds.point.x + 
                        prevBox->iFormatBounds.dimension.width - child->iFormatBounds.point.x;
                    }
                    
                }
            }
        }

    if (aPosFlowBox->iHasTop)
        {
        delta.y = aPosFlowBox->iTop + displayBounds.point.y - child->iFormatBounds.point.y;
        if (!isAuto.top)
            {
            delta.y += margins.top;
            }
        }
    else if (aPosFlowBox->iHasBottom)
        {
        delta.y = displayBounds.dimension.height + displayBounds.point.y - 
            child->iFormatBounds.point.y - 
            aPosFlowBox->iBottom - (NW_LMgr_BoxOf(aPosFlowBox))->iFormatBounds.dimension.height;
        if (!isAuto.bottom)
            {
            delta.y -= margins.bottom;
            }
        }
    else
        {
        NW_LMgr_Box_t* prevBox = NW_LMgr_PosFlowBox_GetPrev(aPosFlowBox);
        if (prevBox != NULL)
            {
            delta.y = prevBox->iFormatBounds.point.y - child->iFormatBounds.point.y;
            }
        }

    NW_LMgr_BoxVisitor_t visitor;
    NW_Bool skipChildren = NW_FALSE;

    NW_LMgr_BoxVisitor_Initialize (&visitor, NW_LMgr_BoxOf (aPosFlowBox)); 
    while ((child = NW_LMgr_BoxVisitor_NextBox(&visitor, &skipChildren)) != NULL)
        {
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( child );
        boxBounds.point.x = (NW_GDI_Metric_t)(boxBounds.point.x + delta.x);
        boxBounds.point.y = (NW_GDI_Metric_t)(boxBounds.point.y + delta.y);
        NW_LMgr_Box_SetFormatBounds(child, boxBounds);
        NW_LMgr_Box_UpdateDisplayBounds(child, boxBounds);
        }
    }

TBrowserStatusCode NW_LMgr_PosFlowBox_AddTabItem(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_ASSERT(aPosFlowBox != NULL);
    NW_LMgr_BoxVisitor_t boxVisitor;
    TBrowserStatusCode status = KBrsrSuccess;
    NW_LMgr_Box_t* child = NULL;
    NW_LMgr_Box_t* box = NW_LMgr_BoxOf(aPosFlowBox);
    NW_Uint8 skipChildren = NW_FALSE;

    (void)NW_LMgr_BoxVisitor_Initialize(&boxVisitor, box);

    // Add tabitems for the subtree
    while ((child = NW_LMgr_BoxVisitor_NextBox(&boxVisitor, &skipChildren)) != NULL)
        {
        // we are only looking for boxes implementing the NW_LMgr_IEventListener interface
        NW_LMgr_IEventListener_t* eventListener = 
            (NW_LMgr_IEventListener_t*)(NW_Object_QueryInterface (child, &NW_LMgr_IEventListener_Class));

        if (eventListener)
            {
            NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(aPosFlowBox);
            MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
            status = boxTreeListener->BoxTreeListenerAppendTabItem (child);
            if (status == KBrsrOutOfMemory)
                {
                return status;
                }
            }
        }
    return status;
    }

TBrowserStatusCode NW_LMgr_PosFlowBox_HandlePostFormat(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_ASSERT(aPosFlowBox != NULL);
    if (NW_LMgr_PosFlowBox_AdjustSizeNeeded(aPosFlowBox))
        {
        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(aPosFlowBox);
        MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
        boxTreeListener->ReformatBox(NW_LMgr_BoxOf(aPosFlowBox));
        }
    NW_LMgr_PosFlowBox_AdjustPosition(aPosFlowBox);
    return NW_LMgr_PosFlowBox_AddTabItem(aPosFlowBox);
    }

// This function adds a positioned box to the array of positioned boxes.
// If the positioned box is not nested, it is added to the end of the array.
// If the box is nested, it is added after all other boxes that have the same containing block
void NW_LMgr_PosFlowBox_AddPositionedBox(NW_LMgr_PosFlowBox_t* aPosFlowBox)
    {
    NW_ASSERT(aPosFlowBox != NULL);
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( NW_LMgr_BoxOf(aPosFlowBox) );
    if (aPosFlowBox->iContainingBlock == NULL)
        {
        NW_ADT_DynamicVector_InsertAt(rootBox->positionedAbsObjects, &aPosFlowBox, NW_ADT_Vector_AtEnd);
        }
    else
        {
        NW_ADT_Vector_Metric_t posCount, index;
        posCount = NW_ADT_Vector_GetSize(rootBox->positionedAbsObjects);
        // First find the containing block
        for (index = 0; index < posCount; index++)
            {
            if (*(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (rootBox->positionedAbsObjects, index) == aPosFlowBox->iContainingBlock)
                {
                break;
                }            
            }
        // Skip the containing block, and all boxes that have the same containing block
        if (*(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (rootBox->positionedAbsObjects, index) == aPosFlowBox->iContainingBlock)
            {
            for (index++ ; index < posCount; index++)
                {
                if ((*(NW_LMgr_PosFlowBox_t**) NW_ADT_Vector_ElementAt (rootBox->positionedAbsObjects, index))->iContainingBlock != aPosFlowBox->iContainingBlock)
                    {
                    break;
                    }
                }
            }
        // Insert the box 
        NW_ADT_DynamicVector_InsertAt(rootBox->positionedAbsObjects, &aPosFlowBox, index);
        }
    }

