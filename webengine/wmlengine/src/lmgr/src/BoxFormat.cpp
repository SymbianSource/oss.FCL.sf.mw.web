/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Description : The class handles formatting of the bidiflowbox
*
*/



#include "cstack.h"
#include "nwx_logger.h"
#include "nw_lmgr_linebox.h"
#include "nw_lmgr_nobrbox.h"
#include "LMgrObjectBoxOOC.h"
#include "nw_gdi_utils.h"
#include "BoxFormat.h"
#include "nw_lmgr_posflowbox.h"
#include "LMgrMarkerText.h"
#include "LMgrMarkerImage.h"
#include "LMgrAnonBlock.h"
#include "GDIDevicecontext.h"
#include "nw_lmgr_accesskey.h"
#include "nw_lmgr_verticaltablecellbox.h"

#define NW_LMgr_maxMarkerLen (NW_Uint8)2

const TInt KFlowBoxLineSpacing = 4;

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_FlowBox_FormatBox(NW_LMgr_Box_t* parentFormatBox,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_PropertyValue_t positionVal;
    NW_LMgr_PropertyValue_t floatVal;
    NW_LMgr_PropertyValue_t propertyVal;
    NW_LMgr_Box_t *flow;
    TBrowserStatusCode status = KBrsrSuccess;

    /* for convenience */
    flow = parentFormatBox;

    positionVal.token = NW_CSS_PropValue_none;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_position,
                                       NW_CSS_ValueType_Token, &positionVal);
    if(positionVal.token == NW_CSS_PropValue_position_absolute &&
		NW_Settings_GetDisableSmallScreenLayout())
        {
//        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );
//        NW_ASSERT(rootBox != NULL);
        // We have to insert a new box into the tree, to keep track of the positioned subtree
        NW_LMgr_ContainerBox_t* parent = NW_LMgr_Box_GetParent(box);
        
		if (!NW_Object_IsInstanceOf(parent, &NW_LMgr_PosFlowBox_Class))
            {
            /*
            NW_LMgr_PosFlowBox_t* newParent = NW_LMgr_PosFlowBox_New(5);
            if (newParent == NULL)
                {
                return KBrsrOutOfMemory;
                }
            TInt childIndex = NW_LMgr_ContainerBox_GetChildIndex (parent, box);
            NW_ASSERT (childIndex >= 0);
            NW_LMgr_ContainerBox_RemoveChild (parent, box);
            NW_LMgr_ContainerBox_InsertChildAt (parent, NW_LMgr_BoxOf(newParent), (NW_ADT_Vector_Metric_t)childIndex);
            NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(newParent), box);

            NW_LMgr_PropertyValue_t visibilityVal;
            visibilityVal.token = NW_CSS_PropValue_visible;
            (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_visibility,
                                                  NW_CSS_ValueType_Token, &visibilityVal);
            NW_LMgr_PropertyValue_t displayVal;
            displayVal.token = NW_CSS_PropValue_display_inline;
            (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_display,
                                                  NW_CSS_ValueType_Token, &displayVal);
           
            if ((visibilityVal.token != NW_CSS_PropValue_hidden) && (displayVal.token != NW_CSS_PropValue_none))
                {
                NW_LMgr_PosFlowBox_AddPositionedBox(newParent);
                //NW_ADT_DynamicVector_InsertAt(rootBox->positionedAbsObjects, &newParent, NW_ADT_Vector_AtEnd);
                }

            */

            // Per CSS2 spec, remove float property, and set display to block:
            // 2. Otherwise, ’position’ has the value ’absolute’ or ’fixed’, ’display’ is set to
            // ’block’ and ’float’ is set to ’none’. The position of the box will be determined
            // by the ’top’, ’right’, ’bottom’ and ’left’ properties and the box’s containing
            // block.
            NW_LMgr_Property_t prop;
            prop.type = NW_CSS_ValueType_Token;
            prop.value.token = NW_CSS_PropValue_none;
            NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_float, &prop);
            return KBrsrContinue;
            }
        }

    /* inline is the default display property value in CSS */
    propertyVal.token = NW_CSS_PropValue_display_inline;
    (void)NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_display,
                                        NW_CSS_ValueType_Token, &propertyVal);

    floatVal.token = NW_CSS_PropValue_none;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_float,
                                       NW_CSS_ValueType_Token, &floatVal);

    //Floats do not work within tables. The Titan workaround was to
    //disable them. The same is done here. Note that this condition is
    //tested by looking at the parentFormatBox.  (e.g. flow).
    if ((floatVal.token != NW_CSS_PropValue_none) &&
        !(NW_Object_IsClass (flow, &NW_LMgr_StaticTableCellBox_Class)) &&
        !(NW_Object_IsClass (flow, &NW_LMgr_VerticalTableCellBox_Class)) )
        {
        status = NW_LMgr_BidiFlowBox_Float(flow, box, parentContext);
        if (status != KBrsrFailure)
        {
            return status;
            }
        }

    switch (propertyVal.token)
        {
        case NW_CSS_PropValue_display_block:
        /* This is handle special case Epoc32 boxes */
        if ( ( NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class) ) ||
             ( NW_Object_IsInstanceOf(box, &NW_LMgr_ActiveContainerBox_Class) ) ||
             ( NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class) &&
               NW_LMgr_ImgContainerBox_ImageIsBroken( box ) == NW_TRUE ) )
            {
            status = NW_LMgr_BidiFlowBox_FlowInline(flow, box, parentContext);
            }
        else
            {
              if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class) ||
                  NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class) ||
                  NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class) ||
                  NW_Object_IsInstanceOf (box, &NW_LMgr_FormatBox_Class))
                  {
                  status = NW_LMgr_BidiFlowBox_HandleBlock(flow, box, parentContext);
                  }
              //status = NWLMgr_BidiFlowBox_HandleBlock(flow, box, parentContext);
            }
        break;
        case NW_CSS_PropValue_display_inline:
          status = NW_LMgr_BidiFlowBox_FlowInline(flow, box, parentContext);
          break;
        case NW_CSS_PropValue_display_listItem:
        if (NW_Object_IsInstanceOf(box, &NW_FBox_FormBox_Class))
            {
            status = NW_LMgr_BidiFlowBox_FlowInline(flow, box, parentContext);
            }
        else if (NW_Object_IsInstanceOf(box, &NW_LMgr_BidiFlowBox_Class))
            {
            status = NW_LMgr_BidiFlowBox_ListItem(flow, box, parentContext);
            }
        else if  (NW_Object_IsInstanceOf(box, &NW_LMgr_FormatBox_Class))
            {
            status = NW_LMgr_BidiFlowBox_HandleBlock(flow, box, parentContext);
            }
        break;

        case NW_CSS_PropValue_display_directKey:
            status = NW_LMgr_BidiFlowBox_DirectKey(flow, box, parentContext);
            status = NW_LMgr_BidiFlowBox_FlowInline(flow, box, parentContext);
            break;

        case NW_CSS_PropValue_none:
        /* Flow boxes can't handle other display types */
        default:
          break;
        }

    return status;
    }


TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PostFormat(NW_LMgr_Box_t* flow,
        NW_LMgr_ContainerBox_t *whose,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_TRY(status)
        {

        NW_LMgr_Box_t *flowBox = NW_LMgr_BoxOf(flow);


        /* If we are formatting someone else's children, carry on, otherwise .. */

        if (NW_LMgr_ContainerBoxOf(flow) == whose)
            {
            /* Flush any leftover inline content */
            status = NW_LMgr_BidiFlowBox_Flush(flow, parentContext, NW_TRUE);
            _NW_THROW_ON_ERROR(status);

            /* If we are the rootbox, flush all pending floats */
            if (NW_Object_IsClass(flow, &NW_LMgr_RootBox_Class))
                {
                status = NW_LMgr_BidiFlowBox_AdvanceToClear(flow,
                         parentContext,
                         NW_CSS_PropValue_both);
                _NW_THROW_ON_ERROR(status);
                }
            /* Remove the extra pixels of space from the last line */
            if (parentContext->lineNumber > 0 )
                {
                NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( flowBox );
                boxBounds.dimension.height =
                    (NW_GDI_Metric_t)(boxBounds.dimension.height - KFlowBoxLineSpacing);
                NW_LMgr_Box_SetFormatBounds( flowBox, boxBounds );
                }
            /* Flush the pending vertical margin, if there is one */
            if ((NW_Object_IsClass(flowBox, &NW_LMgr_RootBox_Class))
                || IsClosedBottom(flowBox))
                {
                NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( flowBox );
                boxBounds.dimension.height =
                    (NW_GDI_Metric_t)(boxBounds.dimension.height +
                                      parentContext->pendingMargin);
                NW_LMgr_Box_SetFormatBounds( flowBox, boxBounds );
                }

            }
        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }




TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_FlowBox_PostFormatBox(NW_LMgr_Box_t* parentFormatBox,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_PropertyValue_t floatVal;
    NW_LMgr_PropertyValue_t propertyVal;
    NW_LMgr_Box_t *flow;
    TBrowserStatusCode status = KBrsrSuccess;

    /* for convenience */
    flow = parentFormatBox;

    floatVal.token = NW_CSS_PropValue_none;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_float,
                                       NW_CSS_ValueType_Token, &floatVal);

    //Floats do not work within tables in Small Screen OFF mode. The
    //Titan workaround was to disable them. The same is done here. Note
    //that this condition is tested by looking at the parentFormatBox
    //(e.g. flow).
    //  if (floatVal.token != NW_CSS_PropValue_none)
    if ((floatVal.token != NW_CSS_PropValue_none) &&
        !(NW_Object_IsClass (flow, &NW_LMgr_StaticTableCellBox_Class)))
        {
           return NW_LMgr_BidiFlowBox_PostFlowFloat(flow, box, parentContext);
        }

    /* inline is the default display property value in CSS */
    propertyVal.token = NW_CSS_PropValue_display_inline;
    (void)NW_LMgr_Box_GetPropertyValue (box, NW_CSS_Prop_display,
                                        NW_CSS_ValueType_Token, &propertyVal);

    switch (propertyVal.token)
        {
        case NW_CSS_PropValue_display_block:
          /* This is handle special case Epoc32 boxes */
          if ( ( NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class) ) ||
               ( NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class) &&
                 NW_LMgr_ImgContainerBox_ImageIsBroken( box ) == NW_TRUE ) )
            {
              status = NW_LMgr_BidiFlowBox_PostFlowInline(flow, box, parentContext);
            }
          else
            {
              status = NW_LMgr_BidiFlowBox_PostFlowBlock(flow, box, parentContext);
            }
          break;
        case NW_CSS_PropValue_display_inline:
        case NW_CSS_PropValue_display_directKey:
          status = NW_LMgr_BidiFlowBox_PostFlowInline(flow, box, parentContext);
          break;
        case NW_CSS_PropValue_display_listItem:
          if (NW_Object_IsInstanceOf(box, &NW_FBox_FormBox_Class))
            {
              status = NW_LMgr_BidiFlowBox_PostFlowInline(flow, box, parentContext);
            }
          else
            {
              status = NW_LMgr_BidiFlowBox_PostFlowBlock(flow, box, parentContext);
            }
          break;
        case NW_CSS_PropValue_none:
          /* Flow boxes can't handle other display types */
        default:
          break;
        }

    return status;
    }


/* ----------------------------------------------------------------------------*/
/* Add an inline box to the flow.
 * The typechecking here is done to enforce a box-model schema that
 * defines which kinds of boxes can be flowed in each formatting context.
 */


TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_FlowInline(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_GDI_Metric_t displayWidth;
    CGDIDeviceContext* deviceContext;
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );
    NW_Bool smallScreenOn = NW_LMgr_RootBox_GetSmallScreenOn( rootBox );

    TBrowserStatusCode status = KBrsrSuccess;

    /* Inline a format */
    if (NW_Object_IsInstanceOf(box, &NW_LMgr_FormatBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, parentContext);
        }

    /* Inline a table */
    else if (NW_Object_IsClass(box, &NW_LMgr_StaticTableBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, parentContext);
        }

    /* Inline a container of some other kind */
    else if (NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInlineContainer(flow, NW_LMgr_ContainerBoxOf(box), parentContext);
        }

    /* If the box is a split box, we have already formatted it */
    else if (NW_Object_IsInstanceOf (box, &NW_LMgr_SplitTextBox_Class))
        {
        status = KBrsrSuccess;
        }

    /* Inline a media box */
    else if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, parentContext);
        }

    /* Inline a form box */
    else if (NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, parentContext);

    if( smallScreenOn )
      {
       deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
       displayWidth = deviceContext->DisplayBounds()->dimension.width;
       if (NW_LMgr_RootBox_IsPageRTL(rootBox))
         {
         displayWidth += RIGHT_ORIGIN;
         }


       NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

       if(( boxBounds.point.x + boxBounds.dimension.width) >= displayWidth)
       {
           boxBounds.dimension.width = displayWidth - boxBounds.point.x - 4;
           NW_LMgr_Box_SetFormatBounds( box, boxBounds );
       }
     }
   }

    /* Inline a break box */
    else if (NW_Object_IsClass (box, &NW_LMgr_BreakBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleBreak(flow, box, parentContext);
        }

    /* Inline an empty box */
    else if (NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, parentContext);
        }
    /* Inline a object box */ //R
    //else if (ObjectUtils::IsObjectBox (*box, EFalse))
    //  {
    //  status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, parentContext);
    //  }
    /* Otherwise its a schema violation */
    else
        {
        NW_ASSERT(0);
        }
     return status;

    }


TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PostFlowInline(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {

    TBrowserStatusCode status = KBrsrSuccess;

    /* Inline a container of some other kind */
    if (NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleInlineContainerPost(flow, NW_LMgr_ContainerBoxOf(box), parentContext);
        }

    return status;
    }

/* ----------------------------------------------------------------------------*/
/* Add a block formatted box to the flow */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_FlowBlock(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class) ||
        NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class) ||
        NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class) ||
        NW_Object_IsInstanceOf (box, &NW_LMgr_FormatBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleBlock(flow, box, parentContext);
        }

    /* Otherwise its a schema violation */
    else
        {
        NW_ASSERT(0);
        }

    return status;
    }

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PostFlowBlock(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class) ||
        NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class) ||
        NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class) ||
        NW_Object_IsInstanceOf (box, &NW_LMgr_FormatBox_Class))
        {
        status = NW_LMgr_BidiFlowBox_HandleBlockPost(flow, box, parentContext);
        }

    return status;
    }

// This method handles the postformatting for the floated object.
TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PostFlowFloat(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class) ||
        NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class) ||
        NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class) ||
        NW_Object_IsInstanceOf (box, &NW_LMgr_FormatBox_Class))
        {
        NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;

        if (placedFloats == NULL)
            {
            return KBrsrLmgrNotClear;
            }

        // Add the float to the vector
        void *floatPtr = NW_ADT_DynamicVector_InsertAt(placedFloats, &box, NW_ADT_Vector_AtEnd);
        if (floatPtr == NULL)
            {
            status = KBrsrOutOfMemory;
            }
        else
            {
            // Reformat any boxes that are already on the line
            status = NW_LMgr_BidiFlowBox_ReformatLine(flow, parentContext);
            }
        }
    /* Otherwise its a schema violation */
    else
        {
        NW_ASSERT(0);
        }

    return status;
    }


/* --------------------------------------------------------------------------*/
/* Add a float box to the flow */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_Float(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_ADT_DynamicVector_t *pendingFloats = parentContext->pendingFloats;
    NW_ADT_DynamicVector_t *placedFloats = parentContext->pendingFloats;

    NW_TRY(status)
        {
        /* If the float parentContext is not set, remove the box */
        /* This will be the case if a particular flow does not support
         * floats (e.g., table cells) */
        if (pendingFloats == NULL || placedFloats == NULL)
            {
            NW_THROW_STATUS(status, KBrsrFailure);
            }

        /* Float the box */
        if (NW_Object_IsInstanceOf (box, &NW_LMgr_FormatBox_Class) ||
            NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class) ||
            NW_Object_IsClass (box, &NW_LMgr_StaticTableBox_Class) ||
            NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class))
            {

            status = NW_LMgr_BidiFlowBox_HandleFloat(flow, box, parentContext);
            if (status == KBrsrLmgrNotClear)
                {
                NW_THROW_OOM_ON_NULL
                (NW_ADT_DynamicVector_InsertAt(pendingFloats, &box,
                                               NW_ADT_Vector_AtEnd),
                 status);
                }
            }
        else
            { /* Otherwise its a schema violation */
            NW_LMgr_Box_RemoveProperty(box, NW_CSS_Prop_float);
            NW_THROW_STATUS(status, KBrsrFailure);
            }
        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }

/* Update the flow to a new line. This updates the formatting context and takes
 * care of any floaters that are either waiting to be placed or are going out
 * of scope.
 *
 * returns: KBrsrSuccess, KBrsrOutOfMemory, KBrsrUnexpectedError
 */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_NewLine(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* context,
        NW_GDI_Metric_t delta,
        NW_Bool indentNewLine)
    {
    NW_ADT_Vector_Metric_t floatCount, index;
    NW_LMgr_Box_t *floatBox;
    NW_ADT_DynamicVector_t *pendingFloats = context->pendingFloats;

    NW_TRY(status)
        {
        /* Add the delta */
        context->y = (NW_GDI_Metric_t)(context->y + delta);

        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( flow );

        boxBounds.dimension.height = (NW_GDI_Metric_t)
                boxBounds.dimension.height + delta;
        NW_LMgr_Box_SetFormatBounds( flow, boxBounds );

        /* Reset the whitespace flag */
        context->pendingWhitespace = NW_FALSE;

        /* Update the context */
        context->lineStart = NW_LMgr_FlowBox_GetLineStart(flow, context,
                             context->y);
        context->lineEnd = NW_LMgr_FlowBox_GetLineEnd(flow, context, context->y);
        if (context->direction == NW_GDI_FlowDirection_LeftRight)
            {
            if (indentNewLine)
                {
                context->x = (NW_GDI_Metric_t)(context->lineStart
                                               + NW_LMgr_FlowBox_GetIndentation(flow,
                                                                                context));
                }
            else
                {
                context->x = context->lineStart;
                }
            }
        else
            {
            context->x = (NW_GDI_Metric_t)(context->lineEnd
                                           - NW_LMgr_FlowBox_GetIndentation(flow,
                                                                            context));
            }

        /* Flush the current line */
        status = NW_LMgr_BidiFlowBox_Flush(flow, context, NW_TRUE);
        _NW_THROW_ON_ERROR(status);

        /* If the float context is missing, nothing to do */
        if (pendingFloats == NULL)
            {
            NW_THROW_SUCCESS(status);
            }

        /* Look for any pending floats */
        floatCount = NW_ADT_Vector_GetSize(pendingFloats);
        for (index = 0; index < floatCount;)
            {
            floatBox =
                *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (pendingFloats, index);
            NW_ASSERT(floatBox != NULL);

            status = NW_LMgr_BidiFlowBox_HandleFloat(flow, floatBox, context);
            if (status != KBrsrLmgrNotClear)
                {
                (void)NW_ADT_DynamicVector_RemoveAt(pendingFloats, index);
                floatCount--;
                }
            else
                {
                index++;
                }
            }
        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }

NW_Image_AbstractImage_t*
TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetListStyleImage(NW_LMgr_Box_t* flowBox,
        NW_LMgr_PropertyValueToken_t token)
    {
    NW_Image_AbstractImage_t *image;
    NW_LMgr_RootBox_t* rootBox;
    NW_Int8 imageId;

    if (token == 0)
        {
        return NULL;
        }

    /* get the root box */
    rootBox = NW_LMgr_Box_GetRootBox(flowBox);
    if (rootBox == NULL)
        {
        return NULL;
        }

    imageId = NW_Image_Invalid;
    switch (token)
        {
        case NW_CSS_PropValue_square:
        imageId = NW_Image_Square;
        break;
        case NW_CSS_PropValue_disc:
        imageId = NW_Image_Disc;
        break;
        case NW_CSS_PropValue_circle:
        imageId = NW_Image_Circle;
        break;
        default:
        return NULL;
        }

    /* get the image from the canned images dictionary */
    NW_ASSERT(rootBox->cannedImages != NULL);
    image = CONST_CAST(NW_Image_AbstractImage_t*, NW_Image_CannedImages_GetImage( rootBox->cannedImages,
                       imageId ));

    return image;
    }

NW_LMgr_Box_t* TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetMarker(NW_LMgr_Box_t* flowBox,
        NW_LMgr_Box_t* item,
        NW_LMgr_FormatContext_t* context,
        NW_LMgr_PropertyValueToken_t* markerVerticalAlign,
        NW_Bool* style)
    {
    NW_LMgr_Property_t listProp;
    NW_LMgr_Property_t dirProp;
    NW_LMgr_PropertyValue_t listImage;
    NW_Text_t *bulletText = NULL;
    NW_LMgr_Box_t* marker = NULL;
    NW_Ucs2 *bulletString = NULL;
    NW_Image_AbstractImage_t *image = NULL;
    NW_Image_Virtual_t *virtualImage = NULL;

    NW_TRY(status)
        {
        /* By default Left to Right */
        dirProp.value.token = NW_CSS_PropValue_ltr;
        /* Get flow direction */
        dirProp.type = NW_CSS_ValueType_Token;
        status = NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(flowBox), NW_CSS_Prop_textDirection, &dirProp);

        /* Set the marker default */
        *markerVerticalAlign = NW_CSS_PropValue_baseline;

        *style = NW_FALSE;

        /* Get the list-style-type property */
        listProp.value.token = NW_CSS_PropValue_disc;
        status = NW_LMgr_PropertyList_Get(item->propList,
                                          NW_CSS_Prop_listStyleType,
                                          &listProp);
        if ((status == KBrsrNotFound) || (listProp.type & NW_CSS_ValueType_DefaultStyle))
            {
            status = NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(item->parent),
                                             NW_CSS_Prop_listStyleType,
                                             &listProp);
            }

        /* Get the list-style-image property */
        listImage.object = NULL;
        (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(item),
                                           NW_CSS_Prop_listStyleImage,
                                           NW_CSS_ValueType_Image, &listImage);

        if (listImage.object != NULL)
            {
            image = (NW_Image_AbstractImage_t*)listImage.object;
            }
        else
            {
            /* Look for a canned image */
            image = (NW_Image_AbstractImage_t*)
                    NW_LMgr_BidiFlowBox_GetListStyleImage(flowBox, listProp.value.token);

            /* Canned images are the only ones that are middle aligned */
            if (image != NULL)
                {
                /* center the image */
                *markerVerticalAlign = NW_CSS_PropValue_middle;
                }
            }

        if (image != NULL)
            {
            NW_GDI_Dimension3D_t size;

            /* Get the virtual image */
            virtualImage = NW_Image_Virtual_New(image);
            NW_THROW_OOM_ON_NULL(virtualImage, status);

            /* Get the width of the image, if it is greater than the default left margin
            replace the image by the default disc this is to fix bug SQ02061441511 - Vishy 06/19/2002 */
            (void) NW_Image_AbstractImage_GetSize (image, &size);

            //
            NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( flowBox );
            //
            if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) && ( size.width < NW_VerticalLayout_MinImageWidth1 ||
                size.height < NW_VerticalLayout_MinImageHeight1 ||
                (size.width < NW_VerticalLayout_MinImageWidth2 && size.height > NW_VerticalLayout_MinImageHeight2)))
                {
                /* ignore the image */
                NW_Object_Delete(virtualImage);
                virtualImage = NULL;

                image = (NW_Image_AbstractImage_t*)
                        NW_LMgr_BidiFlowBox_GetListStyleImage(flowBox, listProp.value.token);

                if (image == NULL)
                    {
                    image = (NW_Image_AbstractImage_t*)
                            NW_LMgr_BidiFlowBox_GetListStyleImage(flowBox, NW_CSS_PropValue_disc);
                    }

                /* Canned images are the only ones that are middle aligned */
                if (image != NULL)
                    {
                    /* center the image */
                    *markerVerticalAlign = NW_CSS_PropValue_middle;
                    /* Get the virtual image */
                    virtualImage = NW_Image_Virtual_New(image);
                    NW_THROW_OOM_ON_NULL(virtualImage, status);
                    }
                }
        
            /* Associate the image with the box */
            if (virtualImage != NULL)
                {
                marker = (NW_LMgr_Box_t*)
                         LMgrMarkerImage_New(1, (NW_Image_AbstractImage_t*)virtualImage, NULL, NW_FALSE );
                NW_THROW_OOM_ON_NULL(marker, status);
                // image container takes image ownership
                virtualImage = NULL;
                }
            }
        else if (listProp.value.token != NW_CSS_PropValue_none)
            {
            *style = NW_TRUE;
            switch (listProp.value.token)
                {
                case NW_CSS_PropValue_lowerAlpha:
                bulletString = NW_LMgr_ListBullets_StringFromUInt('a',
                               context->listCount);
                break;
                case NW_CSS_PropValue_upperAlpha:
                bulletString = NW_LMgr_ListBullets_StringFromUInt('A',
                               context->listCount);
                break;
                case NW_CSS_PropValue_lowerRoman:
                bulletString = NW_LMgr_ListBullets_StringFromUInt('r',
                               context->listCount);
                break;
                case NW_CSS_PropValue_upperRoman:
                bulletString = NW_LMgr_ListBullets_StringFromUInt('R',
                               context->listCount);
                break;
                case NW_CSS_PropValue_decimal:
                default:
                bulletString = NW_LMgr_ListBullets_StringFromUInt('d',
                               context->listCount);
                *style = NW_FALSE;
                break;
                }
            if (bulletString)
                {
                bulletText = (NW_Text_t*)NW_Text_UCS2_New(bulletString,
                             NW_Str_Strlen(bulletString),
                             NW_Text_Flags_TakeOwnership);
                bulletString = NULL;
                NW_THROW_OOM_ON_NULL(bulletText, status);

                marker = (NW_LMgr_Box_t*)LMgrMarkerText_New(1, bulletText);
                NW_THROW_OOM_ON_NULL(marker, status);
                }
            else
                {
                NW_THROW_STATUS(status, KBrsrOutOfMemory);
                }
            }
        }
    NW_CATCH(status)
        {
        NW_Mem_Free(bulletString);
        NW_Object_Delete(bulletText);
        NW_Object_Delete(marker);
        NW_Object_Delete(virtualImage);
        marker = NULL;
        }
    NW_FINALLY {
        /* To fix TI compiler warning */
        (void) status;
        return marker;
    }
    NW_END_TRY
    }

NW_LMgr_Box_t* TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetDirectKeyMarker(NW_LMgr_Box_t* item)
    {
    NW_LMgr_AccessKey_t* ak;
    NW_Uint32 val;
    NW_Bool result;
    NW_Ucs2 *string = NULL;
    NW_Text_t *text = NULL;
    NW_LMgr_Box_t* marker = NULL;
    NW_LMgr_PropertyValue_t propValue;

    NW_TRY(status)
        {
        /* Set the marker default */
        status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(item),
                                                  NW_CSS_Prop_accesskey,
                                                  NW_CSS_ValueType_Object,
                                                  &propValue);

        if (status == KBrsrSuccess)
            {
            ak = (NW_LMgr_AccessKey_t*)(propValue.object);
            result = NW_LMgr_AccessKey_GetDirectKey (ak, &val);
            if (result == NW_TRUE)
                {
                string = NW_Str_New(NW_LMgr_maxMarkerLen);
                NW_THROW_OOM_ON_NULL(string, status);
                NW_Str_Strcat(string, (const NW_Ucs2 *) &val);
                NW_Str_StrcatConst(string, ".");
                }
            }

        if (string)
          {
          text = (NW_Text_t*)NW_Text_UCS2_New(string,
                 NW_Str_Strlen(string),
                 NW_Text_Flags_TakeOwnership);
          string = NULL;
          NW_THROW_OOM_ON_NULL(text, status);

          marker = (NW_LMgr_Box_t*)LMgrMarkerText_New(1, text);
          NW_THROW_OOM_ON_NULL(marker, status);
          }
       else
          {
          NW_THROW_STATUS(status, KBrsrOutOfMemory);
          }
        }
    NW_CATCH(status)
        {
        NW_Mem_Free(string);
        NW_Object_Delete(text);
        NW_Object_Delete(marker);
        marker = NULL;
        }
    NW_FINALLY
        {
        /* To fix TI compiler warning */
        (void) status;
        return marker;
        }
    NW_END_TRY
    }

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_ListItem(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* item,
        NW_LMgr_FormatContext_t* context)
    {
    NW_LMgr_ContainerBox_t* markerContainer = NULL;
    NW_LMgr_Box_t* marker = NULL;
    NW_Bool insidePosition;
    NW_LMgr_Property_t prop;
    NW_LMgr_Property_t dirProp;
    NW_LMgr_PropertyValue_t propValue;
    NW_LMgr_PropertyValueToken_t markerVerticalAlign;
    NW_GDI_Dimension2D_t extent;
    NW_GDI_Metric_t markerOffset = 2;
    NW_LMgr_Box_t *currentFlow = NULL;
    NW_Bool ownsMarker = NW_TRUE;
    NW_Bool styleMarker = NW_FALSE;
    NW_GDI_Rectangle_t boundsFlow;
    NW_GDI_Metric_t marginFlow, borderFlow, paddingFlow;

    /* Check that the box type is legal */
    if (!NW_Object_IsClass(item, &NW_LMgr_BidiFlowBox_Class))
        {
        return KBrsrSuccess;
        }

    NW_TRY(status)
        {

        // Find the first block element.
        // This is where we're going to put the marker.
        currentFlow = item;
        markerContainer = NW_LMgr_ContainerBoxOf(currentFlow);
        currentFlow = (NW_LMgr_Box_t*)NW_LMgr_BidiFlowBox_GetFirstBlock(currentFlow);
        if (currentFlow && NW_Object_IsInstanceOf(currentFlow, &NW_LMgr_ContainerBox_Class))
            {
            markerContainer = NW_LMgr_ContainerBoxOf(currentFlow);
            }
 
        /* Get list-style-position */
        insidePosition = NW_FALSE;
        propValue.token = NW_CSS_PropValue_outside;
        (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(item),
                                           NW_CSS_Prop_listStylePosition,
                                           NW_CSS_ValueType_Token, &propValue);
        if (propValue.token == NW_CSS_PropValue_inside)
            {
            insidePosition = NW_TRUE;
            }

        /* If this is the first item, check for any start values */
        if (context->itemsHandled == 0)
            {
            status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow),
                                                  NW_CSS_Prop_listValue,
                                                  NW_CSS_ValueType_Integer,
                                                  &propValue);
            if (status == KBrsrSuccess)
                {
                /* Reset the list count */
                context->listCount = propValue.integer - 1;
                }
            }

        /* Do we have an explicit value set on the item */
        status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(item),
                                              NW_CSS_Prop_listValue,
                                              NW_CSS_ValueType_Integer,
                                              &propValue);
        if (status == KBrsrSuccess)
            {
            /* Reset the list count */
            context->listCount = propValue.integer - 1;
            }

        /* Get the appropriate marker */
        marker = NW_LMgr_BidiFlowBox_GetMarker(flow, item, context,
                                              &markerVerticalAlign, &styleMarker);

        /* Increment the list count */
        context->listCount = (NW_Uint32)(context->listCount + 1);

        /* Increment the itemsHandled counter */
        context->itemsHandled ++;

        if (marker)
            {
            // Add the marker to the new cell
            status = NW_LMgr_ContainerBox_InsertChildAt(markerContainer, marker, 0);
            NW_THROW_ON_ERROR(status);

            // Give up ownership of marker
            ownsMarker = NW_FALSE;

            /* Set vertical alignment */
            prop.type = NW_CSS_ValueType_Token;
            prop.value.token = markerVerticalAlign;
            status = NW_LMgr_Box_SetProperty(marker,
                                             NW_CSS_Prop_verticalAlign, &prop);
            NW_THROW_ON_ERROR(status);

            /* Size the marker */
            if (NW_Object_IsInstanceOf(marker, &LMgrMarkerText_Class))
                {
                status = NW_LMgr_AbstractTextBox_GetDimensions(NW_LMgr_AbstractTextBoxOf(marker), &extent);
                }
            else
                {
                status = NW_LMgr_Box_GetMinimumContentSize(marker, &extent);
                }
            NW_THROW_ON_ERROR(status);

            boundsFlow = NW_LMgr_Box_GetFormatBounds( flow );
            marginFlow = (NW_GDI_Metric_t)(NW_LMgr_Box_GetModelProp(flow, NW_CSS_Prop_leftMargin));
            borderFlow = (NW_GDI_Metric_t)(NW_LMgr_Box_GetModelProp(flow, NW_CSS_Prop_leftBorderWidth));
            paddingFlow = (NW_GDI_Metric_t)(NW_LMgr_Box_GetModelProp(flow, NW_CSS_Prop_leftPadding));

            /* By default Left to Right */
            dirProp.value.token = NW_CSS_PropValue_ltr;
            /* Get flow direction */
            dirProp.type = NW_CSS_ValueType_Token;
            status = NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(item), NW_CSS_Prop_textDirection, &dirProp);

            if (dirProp.value.token == NW_CSS_PropValue_ltr)
                {
                if (!insidePosition)
                    {
                    // If the item has a position of "outside":
                    // according to CSS spec, marker is placed outside the list item;
                    // adjust startMargin to push list item rightward;
                    // make the margin of marker negative, so it'll be on the left of the list item
                    prop.type = NW_CSS_ValueType_Px;
                    prop.value.integer = (NW_GDI_Metric_t)( - extent.width );
                    status = NW_LMgr_Box_SetProperty(marker,
                                                     NW_CSS_Prop_leftMargin, &prop);
                    NW_THROW_ON_ERROR(status);

                    // startMargin = start of content block (boundsFlow.point.x+marginFlow+borderFlow+paddingFlow)
                    //               +
                    //               marker width (extent.width)
                    context->startMargin = (NW_GDI_Metric_t)( boundsFlow.point.x + 
                                                              marginFlow + borderFlow + paddingFlow + 
                                                              extent.width );
                    }
                else
                    {
                    /* Offset the marker */
                    prop.type = NW_CSS_ValueType_Px;
                    prop.value.integer = markerOffset;
                    status = NW_LMgr_Box_SetProperty (marker,
                                                      NW_CSS_Prop_rightMargin, &prop);
                    NW_THROW_ON_ERROR(status);
                    }
                }
            else
                { /* dirProp.value.token == NW_CSS_PropValue_rtl */
                if (!insidePosition)
                    {
                    /* If the item is outside, display it on the left of the list item */
                    prop.type = NW_CSS_ValueType_Px;
                    prop.value.integer = (NW_GDI_Metric_t)( - extent.width );
                    status = NW_LMgr_Box_SetProperty(marker,
                                                     NW_CSS_Prop_rightMargin, &prop);
                    NW_THROW_ON_ERROR(status);

                    context->startMargin = (NW_GDI_Metric_t)( boundsFlow.point.x + 
                                                              marginFlow + borderFlow + paddingFlow + 
                                                              extent.width );
                    }
                else
                    {
                    /* Offset the marker */
                    prop.type = NW_CSS_ValueType_Px;
                    prop.value.integer = markerOffset;
                    status = NW_LMgr_Box_SetProperty (marker,
                                                      NW_CSS_Prop_leftMargin, &prop);
                    NW_THROW_ON_ERROR(status);
                    }
                }
            }

        /* Finally handle the item as a regular block */
        status = NW_LMgr_BidiFlowBox_HandleBlock(flow, item, context);
        NW_THROW_ON_ERROR(status);
        }
    NW_CATCH(status)
        {
        if (ownsMarker)
            {
            NW_Object_Delete(marker);
            }
        }
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_DirectKey(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* item,
        NW_LMgr_FormatContext_t* context)
{
    NW_LMgr_Property_t  prop;
    NW_LMgr_Box_t* marker = NULL;
    NW_LMgr_Box_t *currentFlow = NULL;
    NW_GDI_Dimension2D_t extent;
    NW_Bool ownsMarker = NW_TRUE;

    /* Check that the box type is legal */
    if (!NW_Object_IsClass(item, &NW_LMgr_ActiveContainerBox_Class))
        {
        return KBrsrSuccess;
        }

    NW_TRY(status)
        {

        // Find the first container element.
        // This is where we're going to put the marker.
        currentFlow = item;

        /* Get the appropriate marker */
        marker = NW_LMgr_BidiFlowBox_GetDirectKeyMarker(item);

        if (marker)
            {
            // Add the marker to the new cell
            status = NW_LMgr_ContainerBox_InsertChild (NW_LMgr_ContainerBoxOf(flow), marker, currentFlow);
            NW_THROW_ON_ERROR(status);

            // Give up ownership of marker
            ownsMarker = NW_FALSE;

            /* Set vertical alignment */
            prop.type = NW_CSS_ValueType_Token;
            prop.value.token = NW_CSS_PropValue_baseline;;
            status = NW_LMgr_Box_SetProperty(marker,
                                             NW_CSS_Prop_verticalAlign, &prop);
            NW_THROW_ON_ERROR(status);
            /* Size the marker */
            status = NW_LMgr_Box_GetMinimumContentSize(marker, &extent);
            NW_THROW_ON_ERROR(status);
            }
        status = NW_LMgr_BidiFlowBox_HandleInline(flow, marker, context);
        }
    NW_CATCH(status)
        {
       if ((marker != NULL) && ownsMarker)
            {
            NW_Object_Delete(marker);
            }
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }

/* method to adjust the current line of elements according to
 * their alignment. This returns the line height resulting from vertical
 * alignment adjustment.
 *
 * returns: KBrsrSuccess, KBrsrOutOfMemory, KBrsrUnexpectedError
 */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_Flush(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext, NW_Bool addLineSpacing)
    {
    NW_GDI_Metric_t lineHeight = 0;
    NW_GDI_Metric_t lineBase = 0;
    NW_LMgr_PropertyValue_t tAlign;
    NW_LMgr_Property_t baselineProp;
    NW_GDI_Metric_t space;
    NW_LMgr_Box_t* lastBox;
    NW_ADT_Vector_Metric_t numBoxes;
    NW_GDI_Metric_t flowBoxLineSpacing;

    NW_REQUIRED_PARAM(flow);
    NW_ASSERT(flow != NULL);
    NW_ASSERT(parentContext != NULL);

    numBoxes = NW_LMgr_LineBox_GetCount(parentContext->currentLine);
    if (numBoxes == 0)
        {
        return KBrsrSuccess;  /* Nothing to flush */
        }

    NW_TRY(status)
        {
        /* We should ignore trailing space at EOL */
        lastBox = NW_LMgr_LineBox_GetBox(parentContext->currentLine, (NW_ADT_Vector_Metric_t)(numBoxes - 1));
        if (lastBox != NULL)
            {
            if (NW_Object_IsInstanceOf(lastBox, &NW_LMgr_AbstractTextBox_Class))
                {
                NW_LMgr_AbstractTextBox_CompressTrailing(NW_LMgr_AbstractTextBoxOf(lastBox));
                }
            }

        /* Get the available space for the line */
        space = ((NW_GDI_Metric_t)
                 (parentContext->lineEnd - parentContext->lineStart));

        /* Get the alignment property, or default to left align if not specified */
        if (parentContext->direction == NW_GDI_FlowDirection_LeftRight)
            {
            tAlign.token = NW_CSS_PropValue_left;
            }
        else
            {
            tAlign.token = NW_CSS_PropValue_right;
            }

            (void) NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow),
                                           NW_CSS_Prop_textAlign,
                                           NW_CSS_ValueType_Token, &tAlign);

        /* Shift horizontally */
        (void)NW_LMgr_LineBox_HAlign(parentContext->currentLine, tAlign.token, space,
                                     parentContext->direction);

        /* Position boxes vertically according to their alignment props */
        status = NW_LMgr_LineBox_VAlign(parentContext->currentLine, parentContext->y,
                                        &lineBase, &lineHeight);
        _NW_THROW_ON_ERROR(status);

        /* Flush any inline containers */
        status = NW_LMgr_BidiFlowBox_FlushInlineContainers(flow, parentContext);
        _NW_THROW_ON_ERROR(status);

        /* Clear the line box, we're done */
        status = NW_LMgr_LineBox_Clear(parentContext->currentLine);
        _NW_THROW_ON_ERROR(status);

        /* Update the parentContext */
        flowBoxLineSpacing = (addLineSpacing == NW_TRUE ? KFlowBoxLineSpacing : 0);

        parentContext->y = (NW_GDI_Metric_t)(parentContext->y + lineHeight + flowBoxLineSpacing);

        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( flow );
        boxBounds.dimension.height =
            (NW_GDI_Metric_t)( boxBounds.dimension.height + lineHeight + flowBoxLineSpacing);
        NW_LMgr_Box_SetFormatBounds( flow, boxBounds );

        /* Set the baseline on the flow box */
        if (parentContext->lineNumber == 0)
            {
            baselineProp.type = NW_CSS_ValueType_Px;
            baselineProp.value.integer = lineBase;
            status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(flow),
                                             NW_CSS_Prop_flowBaseline,
                                             &baselineProp);
            _NW_THROW_ON_ERROR(status);
            }
        parentContext->lineNumber = (NW_Uint16)(parentContext->lineNumber + 1);
        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }


/* --------------------------------------------------------------------------*/
/* A method to find the next clear position. */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_AdvanceToClear(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext,
        NW_LMgr_PropertyValueToken_t clearToken)
    {
    NW_ADT_Vector_Metric_t index, floatCount;
    NW_LMgr_PropertyValue_t floatValue;
    NW_GDI_Metric_t clearPosition, floatBottom;
    NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;
    NW_LMgr_FrameInfo_t margin;
    NW_LMgr_Box_t *floatBox;

    NW_REQUIRED_PARAM(flow);

    NW_TRY(status)
        {
        if (placedFloats == NULL)
            {
            NW_THROW_SUCCESS(status);
            }
        if (clearToken == NW_CSS_PropValue_none)
            {
            NW_THROW_SUCCESS(status);
            }
        do
            {
            /* Look for the floats */
            floatCount = NW_ADT_Vector_GetSize(placedFloats);
            clearPosition = parentContext->y;

            for (index = 0; index < floatCount; index++)
                {

                floatBox =
                    *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
                NW_ASSERT(floatBox != NULL);

                floatValue.token = NW_CSS_PropValue_none;
                NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                                             NW_CSS_ValueType_Token, &floatValue);

                NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameBottom );

                if (((floatValue.token == NW_CSS_PropValue_left)
                     && (clearToken == NW_CSS_PropValue_left))
                    || ((floatValue.token == NW_CSS_PropValue_right)
                        && (clearToken == NW_CSS_PropValue_right))
                    || (clearToken == NW_CSS_PropValue_both))
                    {
                    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );
                    floatBottom = (NW_GDI_Metric_t)(boxBounds.point.y
                                                    + boxBounds.dimension.height
                                                    + margin.bottom);
                    if (floatBottom > clearPosition)
                        {
                        clearPosition = floatBottom;
                        }
                    }
                }

            if (clearPosition > parentContext->y)
                {
                status = NW_LMgr_BidiFlowBox_NewLine(flow, parentContext,
                         (NW_GDI_Metric_t)(clearPosition
                                           - parentContext->y), NW_TRUE);
                _NW_THROW_ON_ERROR(status);
                }

            /* The newline method might have emitted a pending float.  If that was
               the case, we should check that we're still clear */
            }
        while (floatCount
               != NW_ADT_Vector_GetSize(placedFloats));
        }
    NW_CATCH(status)
    {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }

/* --------------------------------------------------------------------------*/
/* A method to find the next clear position during reformatting a line. */
TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_AdvanceToClearReformat(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext,
        NW_LMgr_PropertyValueToken_t clearToken)
    {
    NW_ADT_Vector_Metric_t index, floatCount;
    NW_LMgr_PropertyValue_t floatValue;
    NW_GDI_Metric_t clearPosition, floatBottom;
    NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;
    NW_LMgr_FrameInfo_t margin;
    NW_LMgr_Box_t *floatBox;
	NW_GDI_Metric_t delta;

    NW_REQUIRED_PARAM(flow);

    NW_TRY(status)
        {
        if (placedFloats == NULL)
            {
            NW_THROW_SUCCESS(status);
            }
        if (clearToken == NW_CSS_PropValue_none)
            {
            NW_THROW_SUCCESS(status);
            }
        do
            {
            /* Look for the floats */
            floatCount = NW_ADT_Vector_GetSize(placedFloats);
            clearPosition = parentContext->y;

            for (index = 0; index < floatCount; index++)
                {

                floatBox =
                    *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
                NW_ASSERT(floatBox != NULL);

                floatValue.token = NW_CSS_PropValue_none;
                NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                                             NW_CSS_ValueType_Token, &floatValue);

                NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameBottom );

                if (((floatValue.token == NW_CSS_PropValue_left)
                     && (clearToken == NW_CSS_PropValue_left))
                    || ((floatValue.token == NW_CSS_PropValue_right)
                        && (clearToken == NW_CSS_PropValue_right))
                    || (clearToken == NW_CSS_PropValue_both))
                    {
                    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );
                    floatBottom = (NW_GDI_Metric_t)(boxBounds.point.y
                                                    + boxBounds.dimension.height
                                                    + margin.bottom);
                    if (floatBottom > clearPosition)
                        {
                        clearPosition = floatBottom;
                        }
                    }
                }

            if (clearPosition > parentContext->y)
                {
                /*status = NW_LMgr_BidiFlowBox_NewLine(flow, parentContext,
                         (NW_GDI_Metric_t)(clearPosition
                                           - parentContext->y), NW_TRUE);
                _NW_THROW_ON_ERROR(status);*/

				 /* Add the delta */

				delta = clearPosition - parentContext->y;

				parentContext->y = (NW_GDI_Metric_t)(parentContext->y + delta);

				NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( flow );

				boxBounds.dimension.height = (NW_GDI_Metric_t)
                boxBounds.dimension.height + delta;
				NW_LMgr_Box_SetFormatBounds( flow, boxBounds );

				/* Reset the whitespace flag */
				parentContext->pendingWhitespace = NW_FALSE;

				/* Update the context */
				parentContext->lineStart = NW_LMgr_FlowBox_GetLineStart(flow, parentContext,
									 parentContext->y);
				parentContext->lineEnd = NW_LMgr_FlowBox_GetLineEnd(flow, parentContext, parentContext->y);
				if (parentContext->direction == NW_GDI_FlowDirection_LeftRight)
					{
						parentContext->x = (NW_GDI_Metric_t)(parentContext->lineStart
													   + NW_LMgr_FlowBox_GetIndentation(flow,
																						parentContext));

					}
				else
					{
					parentContext->x = (NW_GDI_Metric_t)(parentContext->lineEnd
												   - NW_LMgr_FlowBox_GetIndentation(flow,
																					parentContext));
					}

				/* Flush the current line */
				status = NW_LMgr_BidiFlowBox_Flush(flow, parentContext, NW_TRUE);
				_NW_THROW_ON_ERROR(status);
                }


            /* The newline method might have emitted a pending float.  If that was
               the case, we should check that we're still clear */
            }
        while (floatCount
               != NW_ADT_Vector_GetSize(placedFloats));
        }
    NW_CATCH(status)
    {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }
NW_GDI_Metric_t TLMgr_BoxFormatter::NW_LMgr_FlowBox_GetIndentation(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_GDI_Metric_t indent = 0;
    NW_LMgr_PropertyValue_t tIndent;
    NW_LMgr_PropertyValue_t tAlign;

    if (parentContext->lineNumber == 0)
        {
        /* Get the text alignment */
        tAlign.token = NW_CSS_PropValue_left;
        NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow), NW_CSS_Prop_textAlign,
                                     NW_CSS_ValueType_Token, &tAlign);

        if (tAlign.token == NW_CSS_PropValue_left)
            {
            /* Get the text indent */
            tIndent.integer = 0;
            NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow), NW_CSS_Prop_textIndent,
                                         NW_CSS_ValueType_Px, &tIndent);

            indent = (NW_GDI_Metric_t)tIndent.integer;
            }
        }
    return indent;
    }

/* A method that takes care of any inline containers whose children
 * were formatted by the flow and are now in the line box.
 * The method does two things: (1) if a container did not fit
 * entirely on the line, this container is split;  (2) all
 * containers whose children are in the line box are resized.
 */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_FlushInlineContainers(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_LineBox_t *lineBox = parentContext->currentLine;
    NW_LMgr_PropertyValue_t propValue;
    NW_LMgr_Box_t *child = NULL;
    NW_LMgr_Box_t *nextSibling = NULL;
    NW_LMgr_Box_t *splitBox;
    NW_LMgr_ContainerBox_t *parent = NULL;
    NW_LMgr_ContainerBox_t *parentTemp;
    NW_ADT_Vector_Metric_t childCount;
    NW_ADT_Vector_Metric_t childIndex;
    NW_ADT_Vector_Metric_t i, boxesOnLine;
    NW_ADT_Vector_Metric_t index;
    NW_Bool ltr;

    NW_TRY(status)
        {
        boxesOnLine = NW_LMgr_LineBox_GetCount(parentContext->currentLine);

        // Return if no boxes in the line.
        if (boxesOnLine == 0)
            {
            NW_THROW_SUCCESS(status);
            }

        // Determine the direction -- LTR or RTL.
        ltr = (parentContext->direction == NW_GDI_FlowDirection_LeftRight)
            ? NW_TRUE : NW_FALSE;

        // First, split any containers that did not fit on the line.

        // Get the last box on the line. The index of which depends on
        // direction. If LTR, it truely is the last box.  If RTL, it's
        // reversed and is 0.
        index = (NW_ADT_Vector_Metric_t)((ltr) ? (boxesOnLine - 1) : 0);
        child = NW_LMgr_LineBox_GetBox(lineBox, index);
        

        // Traverse tree up to containing flow and split any inline containers
        // found on the way.
        parent = NW_LMgr_Box_GetParent(child);

        while (parent && parent != NW_LMgr_ContainerBoxOf(flow))
            {

            // Get the display property.
            propValue.token = NW_CSS_PropValue_display_inline;
            (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(parent),
                                               NW_CSS_Prop_display,
                                               NW_CSS_ValueType_Token, &propValue);

            // Check that the container is inline.
            if (propValue.token == NW_CSS_PropValue_display_inline || propValue.token == NW_CSS_PropValue_display_directKey)
                {

                childCount = NW_LMgr_ContainerBox_GetChildCount(parent);
                childIndex = NW_LMgr_ContainerBox_GetChildIndex(parent, child);

                //If <br/> tag in <a> and there is empty text before closing 
                //tags then don't create text box of zero dimension as it will 
                //not hightlight the anchor text in <a> tag.

                 if( (parent && NW_Object_IsInstanceOf( parent, &NW_LMgr_ActiveContainerBox_Class ) )
                     && ( child && NW_Object_IsInstanceOf(child, &NW_LMgr_BreakBox_Class) ) )
                 {
                  if (childIndex < childCount - 1)
                  {
                   nextSibling = 
                       NW_LMgr_ContainerBox_GetChild (parent,(NW_ADT_Vector_Metric_t) ( childIndex + 1));
                   if(nextSibling && NW_Object_IsClass( nextSibling, &NW_LMgr_TextBox_Class ) )
                   {
                    NW_Text_t* aTextData = NW_LMgr_TextBox_GetText(NW_LMgr_TextBoxOf(nextSibling) );
                    NW_Text_Length_t len = NW_Text_GetCharCount(aTextData);
                    NW_Uint32 i =0;
                    NW_Bool onlySplChar = NW_TRUE;
                 

                    //If there is only special character then don't create split box.

                     if(aTextData)
                     {
                      NW_Ucs2* storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (aTextData, 0, NULL, NULL);

                      for(i=0; i < len ; i++)
                       {
                        
                        if(storage[i] <=  ( (NW_Ucs2) 0x0020) )
                        {
                            continue;
                        }
                        else
                        {
                            onlySplChar = NW_FALSE;
                            break;
                        }
                       }//end for(..)
                     }//end if(aTextData) 

                    if(onlySplChar)
                    {
					 //Remove this box from the box tree list, since <br/> is already a whilespace
						status = NW_LMgr_ContainerBox_RemoveChild(parent,nextSibling);
						_NW_THROW_ON_ERROR(status);
						childCount = NW_LMgr_ContainerBox_GetChildCount(parent);
						childIndex = NW_LMgr_ContainerBox_GetChildIndex(parent, child);
                    }//endof onlySplChar
                   }//endof nextSibling
                  }//endof childIndex
                 }

                // If current box is not the last box in the container,
                // split the container.
                if (childIndex < childCount - 1)
                    {

                    splitBox =
                        NW_LMgr_ContainerBox_SplitAtIndex(parent,
                                                          ((NW_ADT_Vector_Metric_t)
                                                           (childIndex + 1)));
                    NW_THROW_OOM_ON_NULL(splitBox, status);

                    // Insert the new box into the parent.
                    parentTemp = NW_LMgr_Box_GetParent((NW_LMgr_Box_t*)parent);
                    status = NW_LMgr_ContainerBox_InsertChild(parentTemp,
                             splitBox,
                             (NW_LMgr_Box_t*)parent);
                    _NW_THROW_ON_ERROR(status);

                    // Make the split box current.
                    parent = NW_LMgr_ContainerBoxOf(splitBox);
                    }
				else
					{
					if (NW_Object_IsClass (child, &NW_LMgr_BreakBox_Class))
						//If the current box is the last box in the container, 
						//don't split the container nor its parent.
						break; 
					}
                }//if (childIndex < childCount - 1)

            // Move one level up.
            child = NW_LMgr_BoxOf(parent);
            parent = NW_LMgr_Box_GetParent(child);
            }//endof while

        // Second, resize all inline container boxes.

        // Iterate through the children, starting from the 'last' one so as to
        // properly handle any embedded containers. The actual last one depends
        // on the direction -- LTR or RTL.
        for (i = 0; i < boxesOnLine; i++)
            {
            // Determine actual index based on the direction.
            index = (NW_ADT_Vector_Metric_t)((ltr) ? ((boxesOnLine - i) - 1) : i);
            NW_ASSERT(index < 0x7FFF);
            // Get the child.
            child = NW_LMgr_LineBox_GetBox(lineBox, index);
            // Resize all containers that start with this child.
            parent = NW_LMgr_Box_GetParent(child);

            while (parent && parent != NW_LMgr_ContainerBoxOf(flow))
                {
                if (NW_Object_IsClass(parent, &NW_LMgr_ContainerBox_Class) ||
                    NW_Object_IsClass(parent, &NW_LMgr_ActiveContainerBox_Class) ||
                    NW_Object_IsClass(parent, &NW_LMgr_NobrBox_Class))
                    {
                    // Only resize if this is the first child.
                    childIndex = NW_LMgr_ContainerBox_GetChildIndex(parent, child);
                    if (childIndex == 0)
                        {
                        status = NW_LMgr_Box_Resize((NW_LMgr_Box_t*)NW_LMgr_BoxOf(parent), parentContext);
                        _NW_THROW_ON_ERROR(status);
                        }
                    else
                        {
                        // The current container was not resized; therefore,
                        // no point in resizing any parent containers. Break
                        // so as to not continue climbing up the hierarchy.
                        break;
                        }
                    }

                // Move one level up.
                child = NW_LMgr_BoxOf(parent);
                parent = NW_LMgr_Box_GetParent(child);
                }
            }
        }
    NW_CATCH(status)
        {}
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }

/* ----------------------------------------------------------------------------*/
/* method to reformat the line box after a float has been inserted.
 */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_ReformatLine(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_LineBox_t *lineBox = parentContext->currentLine;
    NW_ADT_Vector_Metric_t index, boxesOnLine;
    NW_LMgr_Box_t *childBox;
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( NW_LMgr_BoxOf( flow ) );

    // Reset the parentContext
    parentContext->x =
        (NW_GDI_Metric_t)(parentContext->lineStart
                          + NW_LMgr_FlowBox_GetIndentation(flow, parentContext));

    if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
        {
        CGDIDeviceContext* deviceContext;
        const NW_GDI_Rectangle_t* rectangle;

        deviceContext = NW_LMgr_RootBox_GetDeviceContext( rootBox );
        rectangle = deviceContext->DisplayBounds();

        // Reformat Line is created after placing floated objects. If we have already
        // occupied more than 2/3rd of display width, we want to push next objects on the next line
       if ((parentContext->lineEnd - parentContext->lineStart) < (rectangle->dimension.width) / 3)
            {
			NW_LMgr_PropertyValueToken_t clearToken =
				(parentContext->x > (2*rectangle->dimension.width) / 3)?
				NW_CSS_PropValue_left: NW_CSS_PropValue_right;
            NW_LMgr_BidiFlowBox_AdvanceToClearReformat(flow, parentContext, clearToken);
            }
        }

    boxesOnLine = NW_LMgr_LineBox_GetCount(parentContext->currentLine);

    /* If there are no boxes in the line, then no work to do */
    if (boxesOnLine == 0)
        {
        return KBrsrSuccess;
        }

    NW_TRY(status)
        {
        parentContext->currentLine = NW_LMgr_LineBox_New();
        NW_THROW_OOM_ON_NULL(parentContext->currentLine, status);

        /* Reformat each box */
        for (index = 0; index < boxesOnLine; index++)
            {
            childBox = NW_LMgr_LineBox_GetBox(lineBox, index);
            status = NW_LMgr_BidiFlowBox_HandleInline(flow, childBox, parentContext);
            _NW_THROW_ON_ERROR(status)
            }

        /* Lose the old linebox */
        NW_Object_Delete(lineBox);
        }
    NW_CATCH(status)
        {
        if (parentContext->currentLine != NULL)
            {
            NW_Object_Delete(parentContext->currentLine);
            parentContext->currentLine = lineBox;
            }
        }
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }


NW_LMgr_Box_t* TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetFirstBlock(NW_LMgr_Box_t* flow)
    {
    NW_LMgr_ContainerBox_t *flowContainer = NW_LMgr_ContainerBoxOf(flow);
    NW_LMgr_Box_t *child;
    NW_LMgr_Box_t *firstBlock = NULL;
    NW_LMgr_PropertyValue_t propValue, floatVal;
    NW_Uint16 childIndex, childCount;

    childCount = NW_LMgr_ContainerBox_GetChildCount(flowContainer);
    for (childIndex = 0; childIndex < childCount; childIndex++)
        {
        child = NW_LMgr_ContainerBox_GetChild(flowContainer, childIndex );

        floatVal.token = NW_CSS_PropValue_none;
        (void)NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatVal);
        if (floatVal.token == NW_CSS_PropValue_none)
            {
            firstBlock = child;
            break;
            }
        }

    if (firstBlock == NULL)
        {
        return NULL;
        }
    else
        {
        propValue.token = NW_CSS_PropValue_display_inline;
        (void)NW_LMgr_Box_GetPropertyValue(firstBlock, NW_CSS_Prop_display,
                                           NW_CSS_ValueType_Token, &propValue);

        if ((propValue.token == NW_CSS_PropValue_display_block) ||
            (propValue.token == NW_CSS_PropValue_display_listItem))
            {
            return firstBlock;
            }
        else
            {
            return NULL;
            }
        }
    }



NW_LMgr_Box_t* TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetLastBlock(NW_LMgr_Box_t* flow)
    {
    NW_LMgr_ContainerBox_t *flowContainer = NW_LMgr_ContainerBoxOf(flow);
    NW_LMgr_Box_t *lastBlock = NULL;
    NW_Int16 childCount;
    NW_Int16 childIndex;
    NW_LMgr_PropertyValue_t propValue;
    NW_LMgr_PropertyValue_t floatVal;
    NW_LMgr_Box_t *child;

    childCount = NW_LMgr_ContainerBox_GetChildCount(flowContainer);

    if (childCount == 0)
        {
        return NULL;
        }
    else
        {
        for (childIndex = (NW_Int16)(childCount - 1); childIndex >= 0; childIndex --)
            {
            child = NW_LMgr_ContainerBox_GetChild(flowContainer, (NW_ADT_Vector_Metric_t)childIndex);

            floatVal.token = NW_CSS_PropValue_none;
            (void)NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatVal);
            if (floatVal.token == NW_CSS_PropValue_none)
                {
                lastBlock = child;
                }
            }
        }

    if (lastBlock == NULL)
        {
        return NULL;
        }
    else
        {
        propValue.token = NW_CSS_PropValue_display_inline;
        (void)NW_LMgr_Box_GetPropertyValue(lastBlock, NW_CSS_Prop_display,
                                           NW_CSS_ValueType_Token, &propValue);

        if ((propValue.token == NW_CSS_PropValue_display_block) ||
            (propValue.token == NW_CSS_PropValue_display_listItem))
            {
            return lastBlock;
            }
        else
            {
            return NULL;
            }
        }
    }

NW_Bool TLMgr_BoxFormatter::IsClosedTop(NW_LMgr_Box_t *box)
    {
    return (NW_Bool)
           ((NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topPadding) > 0) ||
            ((NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topBorderWidth) > 0) &&
             (NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topBorderStyle) != NW_CSS_PropValue_none) &&
             (NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_topBorderStyle) != NW_CSS_PropValue_hidden)));
    }

NW_Bool TLMgr_BoxFormatter::IsClosedBottom(NW_LMgr_Box_t *box)
    {
    return (NW_Bool)
           ((NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomPadding) > 0) ||
            ((NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomBorderWidth) > 0) &&
             (NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomBorderStyle) != NW_CSS_PropValue_none) &&
             (NW_LMgr_Box_GetModelProp(box, NW_CSS_Prop_bottomBorderStyle) != NW_CSS_PropValue_hidden)));
    }


/* ----------------------------------------------------------------------------*/
/* A method to account for any collapsing margins.  We look at
 * any block boxes adjacent to the current box and updated the
 * format context accordingly.
 */



TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_CollapseMargins(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext,
        NW_ADT_Vector_Metric_t index)
    {
    NW_LMgr_Box_t *current;
    NW_GDI_Metric_t topMargin = 0, bottomMargin = 0;
    NW_GDI_Metric_t finalPositive = 0, finalNegative = 0, finalMargin = 0;

    /* All boxes sharing a horizontal edge will have their margins collapsed. */
    NW_TRY(status)
        {
        if ((index == 0)
            && (NW_Object_IsInstanceOf(flow, &NW_LMgr_RootBox_Class)
                || IsClosedTop(NW_LMgr_BoxOf(flow))))
            {

            finalPositive = finalNegative = finalMargin = 0;

            current = box;
            do
                {
                topMargin =
                    (NW_GDI_Metric_t)NW_LMgr_Box_GetModelProp(current,
                            NW_CSS_Prop_topMargin);

                if (topMargin < 0)
                    {
                    finalNegative =
                        (NW_GDI_Metric_t)((finalNegative < -topMargin)
                                          ? -topMargin : finalNegative);
                    }
                else
                    {
                    finalPositive =
                        (NW_GDI_Metric_t)((finalPositive < topMargin)
                                          ? topMargin : finalPositive);
                    }

                if (IsClosedTop(current) ||
                    (!NW_Object_IsInstanceOf(current, &NW_LMgr_FlowBox_Class)))
                    {
                    break;
                    }
                }
            while ((current =
                        NW_LMgr_BidiFlowBox_GetFirstBlock(current))
                   != NULL);

            finalMargin = (NW_GDI_Metric_t)(finalPositive - finalNegative);

            /* Advance the current position */
            status = NW_LMgr_BidiFlowBox_NewLine(flow, parentContext, finalMargin, NW_TRUE);
            _NW_THROW_ON_ERROR(status);
            }
        else if (index != 0)
            {

            if (parentContext->pendingMargin < 0)
                {
                finalPositive = 0;
                finalNegative = (NW_GDI_Metric_t)( -(parentContext->pendingMargin));
                }
            else
                {
                finalPositive = parentContext->pendingMargin;
                finalNegative = 0;
                }

            current = box;
            do
                {
                topMargin =
                    (NW_GDI_Metric_t)NW_LMgr_Box_GetModelProp(current,
                            NW_CSS_Prop_topMargin);
                if (topMargin < 0)
                    {
                    finalNegative = (NW_GDI_Metric_t)((finalNegative < -topMargin)
                                                      ? -topMargin : finalNegative);
                    }
                else
                    {
                    finalPositive = (NW_GDI_Metric_t)((finalPositive < topMargin)
                                                      ? topMargin : finalPositive);
                    }
                if (IsClosedTop(current) ||
                    (!NW_Object_IsInstanceOf(current, &NW_LMgr_FlowBox_Class)))
                    {
                    break;
                    }
                }
            while ((current =
                        NW_LMgr_BidiFlowBox_GetFirstBlock(current))
                   != NULL);

            /* Calculate the collapsed margin */
            finalMargin = (NW_GDI_Metric_t)(finalPositive - finalNegative);

            /* Advance the current position & reset any pending margin */
            status = NW_LMgr_BidiFlowBox_NewLine (flow, parentContext, finalMargin, NW_TRUE);
            _NW_THROW_ON_ERROR(status);
            parentContext->pendingMargin = 0;
            }

        /* Calculate the bottom margin */
        current = box;
        do
            {
            bottomMargin =
                (NW_GDI_Metric_t)NW_LMgr_Box_GetModelProp(current,
                        NW_CSS_Prop_bottomMargin);

            if (current == box)
                {
                if (bottomMargin < 0)
                    {
                    finalPositive = 0;
                    finalNegative = (NW_GDI_Metric_t)( -bottomMargin);
                    }
                else
                    {
                    finalPositive = bottomMargin;
                    finalNegative = 0;
                    }
                }
            else
                {
                if (bottomMargin < 0)
                    {
                    finalNegative = (NW_GDI_Metric_t)((finalNegative < -bottomMargin)
                                                      ? -bottomMargin : finalNegative);
                    }
                else
                    {
                    finalPositive = (NW_GDI_Metric_t)((finalPositive < bottomMargin)
                                                      ? bottomMargin : finalPositive);
                    }
                }
            if (IsClosedBottom(current)
                || (!NW_Object_IsInstanceOf(current, &NW_LMgr_FlowBox_Class)))
                {
                break;
                }
            }
        while ((current =
                    NW_LMgr_BidiFlowBox_GetLastBlock(current))
               != NULL);

        /* Calculate the collapsed margin */
        finalMargin = (NW_GDI_Metric_t)(finalPositive - finalNegative);

        /* Set the parentContext variable */
        parentContext->pendingMargin = finalMargin;
        }
    NW_CATCH(status)
    {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }



/* --------------------------------------------------------------------------*/
/* Private method to calculate the left margin of the flow at position y.
 */

NW_GDI_Metric_t TLMgr_BoxFormatter::NW_LMgr_FlowBox_GetLineStart(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext,
        NW_GDI_Metric_t where)
    {
    NW_ADT_Vector_Metric_t floatCount, index;
    NW_GDI_Metric_t leftMargin;
    NW_GDI_Metric_t floatRight, floatTop, floatBottom;
    NW_LMgr_Box_t *floatBox;
    NW_LMgr_PropertyValue_t floatValue;
    NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;
    NW_LMgr_FrameInfo_t margin;

    NW_REQUIRED_PARAM(flow);

    leftMargin = parentContext->startMargin;

    if (placedFloats == NULL)
        {
        return leftMargin;
        }

    floatCount = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(placedFloats));

    for (index = 0; index < floatCount; index++)
        {

        floatBox =
            *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
        if( floatBox != flow )
          {
          NW_ASSERT(floatBox != NULL);

          floatValue.integer = NW_CSS_PropValue_none;
          NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                                       NW_CSS_ValueType_Token, &floatValue);

          if (floatValue.token == NW_CSS_PropValue_left)
              {

              NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameTop|ELMgrFrameBottom|ELMgrFrameRight );

              NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );
              floatTop = (NW_GDI_Metric_t)(boxBounds.point.y - margin.top);
              floatBottom = (NW_GDI_Metric_t)(boxBounds.point.y
                                              + boxBounds.dimension.height
                                              + margin.bottom);
              floatRight = (NW_GDI_Metric_t)(boxBounds.point.x
                                             + boxBounds.dimension.width
                                             + margin.right);

              if ((where >= floatTop)
                  && (where < floatBottom)
                  && (leftMargin < floatRight))
                  {
                  leftMargin = floatRight;
                  }
              }
          }
        }
    return leftMargin;
    }


/* --------------------------------------------------------------------------*/
/* Private method to calculate the left margin of the flow at position y.
 */

NW_GDI_Metric_t TLMgr_BoxFormatter::NW_LMgr_FlowBox_GetLineEnd(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext,
        NW_GDI_Metric_t where)
    {
    NW_ADT_Vector_Metric_t floatCount, index;
    NW_GDI_Metric_t rightMargin;
    NW_GDI_Metric_t floatLeft, floatTop, floatBottom;
    NW_LMgr_Box_t *floatBox;
    NW_LMgr_PropertyValue_t floatValue;
    NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;
    NW_LMgr_FrameInfo_t margin;

    NW_REQUIRED_PARAM(flow);

    rightMargin = parentContext->endMargin;

    if (placedFloats == NULL)
        {
        return rightMargin;
        }

    floatCount = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(placedFloats));

    for (index = 0; index < floatCount; index++)
        {

        floatBox =
            *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
        NW_ASSERT(floatBox != NULL);

        if( floatBox != flow )
          {
          floatValue.integer = NW_CSS_PropValue_none;
          NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                                       NW_CSS_ValueType_Token, &floatValue);

          if (floatValue.token == NW_CSS_PropValue_right)
              {

              NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameTop|ELMgrFrameBottom|ELMgrFrameLeft );

              NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );
              floatTop = (NW_GDI_Metric_t)(boxBounds.point.y - margin.top);
              floatBottom = (NW_GDI_Metric_t)(boxBounds.point.y
                                              + boxBounds.dimension.height
                                              + margin.bottom);
              floatLeft = (NW_GDI_Metric_t)(boxBounds.point.x
                                            - margin.left - 1);

              if ((where >= floatTop)
                  && (where < floatBottom)
                  && (rightMargin > floatLeft))
                  {
                  rightMargin = floatLeft;
                  }
              }
          }
        }
    return rightMargin;
    }
/* --------------------------------------------------------------------------*/
/* A method to test whether the flow is at the beginning of a line */


NW_Bool TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_AtNewLine(NW_LMgr_Box_t* flow,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_REQUIRED_PARAM(flow);

    if (parentContext->direction == NW_GDI_FlowDirection_LeftRight)
        {
        if (parentContext->x ==
            parentContext->lineStart + NW_LMgr_FlowBox_GetIndentation(flow, parentContext))
            {
            return NW_TRUE;
            }
        else
            {
            return NW_FALSE;
            }
        }
    else
        {
        if (parentContext->x ==
            parentContext->lineEnd - NW_LMgr_FlowBox_GetIndentation(flow, parentContext))
            {
            return NW_TRUE;
            }
        else
            {
            return NW_FALSE;
            }
        }

    }



/* --------------------------------------------------------------------------*/
/* A method to get the current formatting constraint */

void TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetConstraint(NW_LMgr_Box_t *flow,
        NW_LMgr_FormatContext_t *parentContext,
        NW_GDI_Metric_t *constraint)
    {
    NW_REQUIRED_PARAM(flow);

    /* Allow the remaining space on the current line for constraint width */
    if (parentContext->direction == NW_GDI_FlowDirection_LeftRight)
        {
        *constraint = (NW_GDI_Metric_t)(parentContext->lineEnd - parentContext->x);
        }
    else
        {
        *constraint = (NW_GDI_Metric_t)(parentContext->x - parentContext->lineStart);
        }

    if (*constraint < 0)
        {
        *constraint = 0;
        }
    }

/* ----------------------------------------------------------------------------*/
/* TODO: Add support for "auto" margins.
 */
void TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetBlockMargins (NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t *parentContext,
        NW_LMgr_FrameInfo_t *margin)
    {

    NW_GDI_Dimension2D_t boxSize;
    NW_LMgr_FrameInfo_t boxMargin, isAuto;

    NW_REQUIRED_PARAM(flow);

    /* Rule boxes get special handling */
    if (NW_Object_IsClass(box, &NW_LMgr_RuleBox_Class))
        {
        margin->left = margin->right = 0;
        return ;
        }

    NW_LMgr_Box_GetMargins(box, &boxMargin, &isAuto, ELMgrFrameLeft|ELMgrFrameRight );
    NW_LMgr_Box_GetSizeProperties(box, &boxSize);

    if (boxSize.width <= 0)
        {
        margin->left = boxMargin.left;
        margin->right = boxMargin.right;
        }
    else
        {
        if (isAuto.right && isAuto.left)
            {
            margin->left = (NW_GDI_Metric_t)((parentContext->endMargin - parentContext->startMargin - boxSize.width) / 2);
            margin->right = margin->left;
            }
        else if (isAuto.left)
            {
            margin->left = (NW_GDI_Metric_t)(parentContext->endMargin - parentContext->startMargin - boxSize.width - boxMargin.right);
            margin->right = boxMargin.right;
            }
        else
            {
            margin->left = boxMargin.left;
            margin->right = (NW_GDI_Metric_t)(parentContext->endMargin - parentContext->startMargin - boxSize.width - boxMargin.left);
            }
        }
    }

/* ----------------------------------------------------------------------------*/
/* Private method to place a child box at the current location in the flow.
 * Because a child can also be a container, we call the virtual shift method
 * to do the placing.  It will automatically shift all the children as well.
 */

void TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PlaceChild(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_REQUIRED_PARAM(flow);
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

    boxBounds.point.x = parentContext->x;
    boxBounds.point.y = parentContext->y;

    NW_LMgr_Box_SetFormatBounds( box, boxBounds );

    // This allow it to skip drawing images until they are "placed".
    if (NW_Object_IsInstanceOf(box, &NW_LMgr_ImgContainerBox_Class))
        {
        NW_LMgr_ImgContainerBox_ImageIsPlaced(box);
        }
    }

/* ----------------------------------------------------------------------------*/
/* Private method to place a child box at a specified location in the flow.
 * Because a child can also be a container, we call the virtual shift method
 * to do the placing.  It will automatically shift all the children as well.
 */

void TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PlaceChildAt(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_GDI_Metric_t x,
        NW_GDI_Metric_t y)
    {
    NW_REQUIRED_PARAM(flow);

    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
    boxBounds.point.x = x;
    boxBounds.point.y = y;
    NW_LMgr_Box_SetFormatBounds( box, boxBounds );

    // This allow it to skip drawing images until they are "placed".
    if (NW_Object_IsInstanceOf(box, &NW_LMgr_ImgContainerBox_Class))
        {
        NW_LMgr_ImgContainerBox_ImageIsPlaced(box);
        }
    }

/* --------------------------------------------------------------------------*/
/* A protected method to handle formatting of replaceable boxes */
TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleInline(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_PropertyValue_t whitespaceVal;
    NW_LMgr_PropertyValueToken_t whitespace;
    NW_LMgr_ContainerBox_t *parent;
    NW_GDI_Metric_t constraint;
    NW_LMgr_Box_t* splitBox;
    NW_LMgr_RootBox_t* rootBox;
    NW_LMgr_FrameInfo_t margin;
    NW_Bool finished = NW_FALSE;
    NW_Bool hasLeadingWS = NW_FALSE;
    NW_Bool hasTrailingWS = NW_FALSE;
    NW_Bool endsInNewline = NW_FALSE;
    NW_Bool atNewline = NW_FALSE;
    NW_Uint8 splitFlags;
    NW_Bool wrap = NW_FALSE;
    NW_LMgr_PropertyValue_t displayVal;
    NW_Bool ltr;
    NW_Bool wrapOn;

    NW_TRY(status)
        {
        // NW_LOG5(NW_LOG_LEVEL5, "HANDLE INLINE ::: flow %x, box %x , context %x, parentcontext y: %d, ht %d", flow, box,
        //                        parentContext, parentContext->y, box->bounds.dimension.height );

        /* Get the parent of the box */
        parent = NW_LMgr_Box_GetParent(box);
        rootBox = NW_LMgr_Box_GetRootBox( box );
        wrapOn = NW_LMgr_RootBox_GetWrapEnabled( rootBox );

        /* Get the whitespace property */
        whitespaceVal.token = NW_CSS_PropValue_normal;
        status = NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(flow),
                                              NW_CSS_Prop_whitespace,
                                              NW_CSS_ValueType_Token, &whitespaceVal);
        NW_THROW_ON(status, KBrsrOutOfMemory);
        whitespace = whitespaceVal.token;

        /* This var tells us how to treat whitespace */
        if (whitespace == NW_CSS_PropValue_normal)
            {
            wrap = NW_TRUE;
            }
        if(  wrapOn && (whitespace == NW_CSS_PropValue_nowrap))
            {
            NW_LMgr_Box_t *surroundingBlock = NW_LMgr_Box_FindBoxOfClass(box, &NW_LMgr_FlowBox_Class, NW_TRUE);
            if (!NW_Object_IsInstanceOf(surroundingBlock, &NW_LMgr_MarqueeBox_Class))
                {
                wrap = NW_TRUE;
                }
            }
        if( wrapOn && (whitespace == NW_CSS_PropValue_pre))
            {
            wrap = NW_TRUE;
            }
		NW_LMgr_Box_t *surroundingBlock = NW_LMgr_Box_FindBoxOfClass(box, &NW_LMgr_FlowBox_Class, NW_TRUE);

        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( NW_LMgr_BoxOf( flow ) );

		if ( wrapOn && !NW_Object_IsInstanceOf(surroundingBlock, &NW_LMgr_MarqueeBox_Class) &&
			NW_LMgr_RootBox_GetSmallScreenOn( rootBox ))
			{
            wrap = NW_TRUE;
            }

        if (parentContext->direction == NW_GDI_FlowDirection_LeftRight)
            {
            ltr = NW_TRUE;
            }
        else
            {
            ltr = NW_FALSE;
            }


        /* Check whether we are at newline before adding the margin */
        atNewline = NW_LMgr_BidiFlowBox_AtNewLine(flow, parentContext);

        /* Get the box margins */
        NW_LMgr_Box_GetMargins(box, &margin, NULL, ELMgrFrameLeft|ELMgrFrameRight);

        /* Add any pending inline margin */
        if (ltr)
            {
            parentContext->x = (NW_GDI_Metric_t)(parentContext->x +
                                                 margin.left +
                                                 parentContext->pendingInlineMargin);
            }
        else
            {
            parentContext->x = (NW_GDI_Metric_t)(parentContext->x -
                                                 margin.right -
                                                 parentContext->pendingInlineMargin);
            }

        if (NW_Object_IsInstanceOf(box, &NW_LMgr_AbstractTextBox_Class))
            {
            status = NW_LMgr_AbstractTextBox_GetWS(NW_LMgr_AbstractTextBoxOf(box),
                                                   &hasLeadingWS,
                                                   &hasTrailingWS,
                                                   &endsInNewline);
            NW_THROW_ON_ERROR(status);

            if (whitespace != NW_CSS_PropValue_pre)
                {
                if (atNewline)
                    {
                    NW_LMgr_AbstractTextBox_CompressLeading(NW_LMgr_AbstractTextBoxOf(box));
                    }
                else if (parentContext->pendingWhitespace)
                    {
                    NW_LMgr_AbstractTextBox_CompressLeading(NW_LMgr_AbstractTextBoxOf(box));
                    }
                }
            }

        /* Get the display property */
        displayVal.token = NW_CSS_PropValue_display_inline;
        status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_display,
                                              NW_CSS_ValueType_Token, &displayVal);
        NW_THROW_ON(status, KBrsrOutOfMemory);
        /* This code deals with the cases for input and textarea boxes with block property */
        if ((displayVal.token == NW_CSS_PropValue_display_block) &&
            (!atNewline))
            {
            (void)NW_LMgr_BidiFlowBox_NewLine(flow, parentContext, 0, NW_TRUE);
            }


        while (!finished)
            {

            /* Set the initial constraint used for splitting and sizing boxes */
            NW_LMgr_BidiFlowBox_GetConstraint(flow, parentContext, &constraint);

            /* Get the box to size itself */
            status = NW_LMgr_Box_Resize(box, parentContext);
            NW_THROW_ON_ERROR(status);

            splitFlags = 0;
            if (atNewline)
                {
                splitFlags |= NW_LMgr_Box_SplitFlags_AtNewLine;
                }
            if (!parentContext->pendingWhitespace &&
                !hasLeadingWS)
                {
                splitFlags |= NW_LMgr_Box_SplitFlags_NoPendingWS;
                }
            if (!wrap)
                {
                splitFlags |= NW_LMgr_Box_SplitFlags_Nowrap;
                }

            /* Do the split */
            status = NW_LMgr_Box_Split(box, constraint, &splitBox, splitFlags);
            NW_THROW_ON(status, KBrsrOutOfMemory);

            if (status == KBrsrSuccess || (status == KBrsrLmgrNoSplit && (!wrap)) )
                {

                /* If split box is different than NULL, a new split box has been
                       created; before proceeding we need to insert the new split box
                       into the flow */
                if (splitBox != NULL)
                    {

                    /* Insert the split box into parent's child list just after the original */
                    status = NW_LMgr_ContainerBox_InsertChildAft (parent, splitBox, box);
                    NW_THROW_ON_ERROR(status);

                    /* If the box was split, it needs to resize */
                    status = NW_LMgr_Box_Resize (box, parentContext);
                    NW_THROW_ON(status, KBrsrOutOfMemory);
                    }

                /* For RTL we must modify parentContext beforehand */
                if (!ltr)
                    {
                    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
                    parentContext->x = (NW_GDI_Metric_t) (parentContext->x - boxBounds.dimension.width);
                    }

                /* The y placement that is set here will get readjusted when
                   the line is flushed */
                NW_LMgr_BidiFlowBox_PlaceChild(flow, box, parentContext);


                /* Add split box to current line */
                status = NW_LMgr_LineBox_Append(parentContext->currentLine, box, parentContext->direction);
                NW_THROW_ON_ERROR(status);

                /* Update the parentContext to reflect the addition of the new box */
                if (ltr)
                    {
                    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
                    parentContext->x = (NW_GDI_Metric_t) (parentContext->x + boxBounds.dimension.width);
                    }

                parentContext->pendingInlineMargin = 0;
                parentContext->pendingWhitespace = hasTrailingWS;


                if (splitBox == NULL)
                    {
                    /* Now we can add the remaining margin */
                    if (ltr)
                        {
                        parentContext->x = (NW_GDI_Metric_t) (parentContext->x + margin.right);
                        }
                    else
                        {
                        parentContext->x = (NW_GDI_Metric_t) (parentContext->x - margin.left);
                        }

                    /* If we are in a pre paragraph, we have to make sure that the text
                       box did not end in a newline.  If it did, we must jump to
                       the next line. */
                    if (endsInNewline)
                        {
                        (void)NW_LMgr_BidiFlowBox_NewLine(flow, parentContext, 0, NW_TRUE);
                        }

                    /* We're done */
                    finished = NW_TRUE;
                    }
                else
                    {
                    /* If the box was split, we must jump to the new line */
                    /* pass NW_FALSE because we don't indent the new line */
                    status = NW_LMgr_BidiFlowBox_NewLine(flow, parentContext, 0, NW_FALSE);
                    NW_THROW_ON_ERROR(status);
                    atNewline = NW_TRUE;

                    /* Now handle the split box */
                    box = splitBox;

                    /* Reset the WS values */
                    if (NW_Object_IsInstanceOf(box, &NW_LMgr_AbstractTextBox_Class))
                        {
                        status = NW_LMgr_AbstractTextBox_GetWS(NW_LMgr_AbstractTextBoxOf(box),
                                                               &hasLeadingWS,
                                                               &hasTrailingWS,
                                                               &endsInNewline);
                        NW_THROW_ON_ERROR(status);

                        NW_LMgr_AbstractTextBox_CompressLeading(NW_LMgr_AbstractTextBoxOf(box));
                        }
                    }
                }
            else
                {
                /* Jump to newline */
                (void)NW_LMgr_BidiFlowBox_NewLine(flow, parentContext, 0, NW_TRUE);
                atNewline = NW_TRUE;

                /* We need to add the margins again, because  the split did not succeed.
                 * If a split has already occurred, we will not redisplay the margins.
                 * The algorithm will have set the margin values to zero.  */

                if (ltr)
                    {
                    parentContext->x = (NW_GDI_Metric_t) (parentContext->x +
                                                          parentContext->pendingInlineMargin +
                                                          margin.left);
                    }
                else
                    {
                    parentContext->x = (NW_GDI_Metric_t) (parentContext->x -
                                                          parentContext->pendingInlineMargin -
                                                          margin.right);
                    }

                /* Reinit the WS parentContext */
                parentContext->pendingWhitespace = NW_FALSE;
                if (NW_Object_IsInstanceOf(box, &NW_LMgr_AbstractTextBox_Class))
                    {
                    NW_LMgr_AbstractTextBox_CompressLeading(NW_LMgr_AbstractTextBoxOf(box));
                    }
                }
            }
        if (displayVal.token == NW_CSS_PropValue_display_block)
            {
            (void)NW_LMgr_BidiFlowBox_NewLine(flow, parentContext, 0, NW_TRUE);
            }

        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    } NW_END_TRY;
    }



/* --------------------------------------------------------------------------*/
/*
 * A special case to handle containers inline: containers are
 * sometimes used as invisible boxes that link together a number of
 * children such as the contents of an HTML <a> element. This method
 * handles this case: the container children are flowed within
 * the parent flow parentContext--hence the box has no visible result in the
 * enclosing flow--but the children are maintained in the box tree as
 * children of the invisible container. This allows them to be treated
 * as a group for such purposes as handling an action (such as the <a>
 * element link handler).  */

/* TODO: We'd really like to get rid of this method so that no special
 * casing of containers (requiring runtime type checking) is
 * needed. This is actually the "real" container box "resize"
 * method. Unfortunately, this requires that the parent parentContext be
 * available (since containers don't have their own formatting
 * contexts). Currently there is no way to get this since contexts
 * aren't attached to objects that use them, so we can't walk up the
 * ancestor tree looking for one.
 */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleInlineContainer(NW_LMgr_Box_t* flow,
        NW_LMgr_ContainerBox_t* container,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_EmptyBox_t *emptyBox;
    NW_LMgr_Property_t prop;
    NW_LMgr_FrameInfo_t margin, padding, borderWidth;
    NW_LMgr_PropertyValue_t clearValue;

    NW_TRY(status)
        {

        clearValue.token = NW_CSS_PropValue_none;
        (void)NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(container), NW_CSS_Prop_clear,
                                           NW_CSS_ValueType_Token, &clearValue);

        /* Advance to the next clear position, if necessary */
        status = NW_LMgr_BidiFlowBox_AdvanceToClear(flow, parentContext, clearValue.token);
        _NW_THROW_ON_ERROR(status);

        /* If the inline container is empty, we'll add an empty box to it
           as a placeholder */
        if (NW_LMgr_ContainerBox_GetChildCount(container) == 0)
            {
            emptyBox = NW_LMgr_EmptyBox_New(1);
            NW_THROW_OOM_ON_NULL(emptyBox, status);

            status = NW_LMgr_ContainerBox_AddChild(container,
                                                   NW_LMgr_BoxOf(emptyBox));
            _NW_THROW_ON_ERROR(status);

            /* We top-align the box (this is useful for empty anchors that point to
             * fragments */
            prop.type = NW_CSS_ValueType_Token;
            prop.value.token = NW_CSS_PropValue_top;
            status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(emptyBox),
                                             NW_CSS_Prop_verticalAlign, &prop);
            _NW_THROW_ON_ERROR(status);
            }

        /* Add our left margin to the pending left margin in the parentContext */
        NW_LMgr_Box_GetMargins(NW_LMgr_BoxOf(container), &margin, NULL, ELMgrFrameLeft );
        NW_LMgr_Box_GetPadding(NW_LMgr_BoxOf(container), &padding, ELMgrFrameLeft );
        NW_LMgr_Box_GetBorderWidth(NW_LMgr_BoxOf(container), &borderWidth, ELMgrFrameLeft );
        parentContext->pendingInlineMargin = (NW_GDI_Metric_t)
                                             (parentContext->pendingInlineMargin + margin.left + padding.left
                                              + borderWidth.left);

        /*
         * Flow the container children within the enclosing flow.
         * Note that the first argument to FlowChildren is the
         * enclosing flow and the second argument is the container
         * whose children will be flowed within the enclosing flow.
         */

          parentContext->formatChildren = NW_TRUE;
          parentContext->formatBox = flow;
          parentContext->newFormatContext = parentContext;
          parentContext->referenceCount++;
        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleInlineContainerPost(NW_LMgr_Box_t* /*flow*/,
        NW_LMgr_ContainerBox_t* container,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_FrameInfo_t margin, padding, borderWidth;

    /* If the inline container is empty, we'll add an empty box to it
       as a placeholder */
    /* Add our left margin to the pending left margin in the parentContext */
    NW_LMgr_Box_GetMargins(NW_LMgr_BoxOf(container), &margin, NULL, ELMgrFrameRight );
    NW_LMgr_Box_GetPadding(NW_LMgr_BoxOf(container), &padding, ELMgrFrameRight );
    NW_LMgr_Box_GetBorderWidth(NW_LMgr_BoxOf(container), &borderWidth, ELMgrFrameRight );

    /*
     * Flow the container children within the enclosing flow.
     * Note that the first argument to FlowChildren is the
     * enclosing flow and the second argument is the container
     * whose children will be flowed within the enclosing flow.
     */

    /* If we had no displayable children, our margins should be ignored */
    if (parentContext->pendingInlineMargin != 0)
        {
        parentContext->pendingInlineMargin = 0;
        }
    else
        {
        /* Otherwise, we add the right margin */
        parentContext->x = (NW_GDI_Metric_t) (parentContext->x + margin.right
                                              + padding.right + borderWidth.right);
        }

    return KBrsrSuccess;
    }

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleBlock (NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_GDI_Metric_t constraint;
    NW_LMgr_PropertyValue_t clearValue;
    NW_LMgr_FrameInfo_t margin;
    NW_Bool lineConstraint = NW_TRUE;
    NW_LMgr_ContainerBox_t* containerBox;
    NW_Uint16 childCount;
    NW_Uint16 childIndex;
    NW_LMgr_Box_t* childBox;
    NW_ADT_Vector_Metric_t index;
    NW_LMgr_EmptyBox_t *emptyBox;

    NW_TRY(status)
        {
       /* Get the "clear" property */
        clearValue.token = NW_CSS_PropValue_none;
        (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_clear,
                                           NW_CSS_ValueType_Token, &clearValue);

        /* This determines what kind of constraint we need */
        if (NW_Object_IsInstanceOf (box, &NW_LMgr_FlowBox_Class))
            {

            lineConstraint = NW_FALSE;

            containerBox = NW_LMgr_ContainerBoxOf (box);
            childCount = NW_LMgr_ContainerBox_GetChildCount (containerBox);
            
            //if this flow box doesn't have any child and also it has attribute height, then 
            //we need to create an empty textbox as its child. The reason is to fix the bug as:
            //<div height=xx color=red></div>
            //text here
            //if we don't create an empty box, there will be overlap between div section and text.
            if (childCount == 0)
            {
                NW_LMgr_Property_t property;
                status = NW_LMgr_Box_GetProperty (box, NW_CSS_Prop_height, &property);
                if (status == KBrsrSuccess)
                {
                    //creat an empty text box
                    emptyBox = NW_LMgr_EmptyBox_New(1);
                    NW_THROW_OOM_ON_NULL(emptyBox, status);

                    //add it as the flowbox child
                    status = NW_LMgr_ContainerBox_AddChild(containerBox, NW_LMgr_BoxOf(emptyBox));
                    _NW_THROW_ON_ERROR(status);
                }
            }

            for (childIndex = 0; childIndex < childCount; childIndex++)
                {

                childBox = NW_LMgr_ContainerBox_GetChild (containerBox, childIndex);
                if (NW_Object_IsClass (childBox, &NW_LMgr_StaticTableBox_Class))
                    {
                    lineConstraint = NW_TRUE;
                    }
                }
            }

        /* Move to new line */
        status = NW_LMgr_BidiFlowBox_NewLine (flow, parentContext, 0, NW_TRUE);
        _NW_THROW_ON_ERROR(status);

        /* Get the position of the current block in the flow */
        index = NW_LMgr_ContainerBox_GetChildIndex(NW_LMgr_ContainerBoxOf(flow), box);
//        NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( NW_LMgr_BoxOf( flow ) );

//        if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
//            {
            // Many times the flow is not the direct parent of the box
            // For e.g. <p><span>Foo
            // while formatting Foo, flowbox belongs to p and not to span, so
            // the box will not be a child of Flow. We want to avoid such cases.
            if ((index > 0) && (NW_LMgr_BoxOf(flow) == NW_LMgr_BoxOf(box->parent)))
                {
                // get the previous sibling
                NW_LMgr_Box_t* child =
                    NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(flow), (NW_ADT_Vector_Metric_t)(index-1));
                NW_ASSERT(child);

                // if previous sibling of this flow box is not of type flow, remove extra line spacing
                // that was added in HandleInline of prev sibling. If the tree was normalized, i.e if the prevsibling
                // was surrounded by anonymous FlowBox, this subtraction would have occurred in PostFormat
                // For e.g <p><img/><div>Bar</div>
                // Here PostFormat of the anonymous FlowBox around <img> that was created in Initialize, would
                // have removed the extra spacing. But since we do not have anonymous FlowBox, we will have to
                // remove it in HandleBlock of div
                if (child && !NW_Object_IsInstanceOf(child, &NW_LMgr_FlowBox_Class))
                    {
                    TBool decreaseLineSpacing = ETrue;

                    // For e.g. <p>
                    //             <span><div>Foo</div>
                    //             <div>Bar</div>
                    //          </p>
                    // Here we do not have to remove the extra spacing, even though span is a container box,
                    // since PostFormat of first div will remove the extra spacing. SO, we look for the
                    // first child
                    if (NW_Object_IsInstanceOf(child, &NW_LMgr_ContainerBox_Class))
                        {
                        // if child is a containerbox - check if the last child is a FlowBox, if yes
                        // don't fo the following, because FlowBox PostFormat will take care of it
                        childCount = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(child));
                        if (childCount > 0)
                            {
                            NW_LMgr_Box_t* lastChild =
                                NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(child), (NW_ADT_Vector_Metric_t)(childCount-1));
                            if (lastChild && NW_Object_IsInstanceOf(lastChild, &NW_LMgr_FlowBox_Class))
                                {
                                // PostFormat of this FlowBox will take care of this
                                decreaseLineSpacing = EFalse;
                                }
                            }
                        }
                    if (decreaseLineSpacing && (parentContext->y >= KFlowBoxLineSpacing))
                        {
                        parentContext->y = parentContext->y-KFlowBoxLineSpacing;
                        }
                    }
                }
//            }

        /* Get the horizontal margins for the box */
        NW_LMgr_BidiFlowBox_GetBlockMargins(flow, box, parentContext, &margin);

        /* Advance to the next clear position, if necessary */
        status = NW_LMgr_BidiFlowBox_AdvanceToClear(flow, parentContext, clearValue.token);
        _NW_THROW_ON_ERROR(status);

        /* Set the size constraint for boxes that need to be constrained in order
           to size themselves */
        if (!lineConstraint)
            {
            constraint = (NW_GDI_Metric_t)
                         (parentContext->endMargin - parentContext->startMargin - margin.left - margin.right);
            }
        else
            {
            constraint = (NW_GDI_Metric_t)
                         (parentContext->lineEnd - parentContext->lineStart - margin.left - margin.right);
            }

        status = NW_LMgr_Box_Constrain(box, constraint);
        NW_THROW_ON(status, KBrsrOutOfMemory);

        /* Collapse any margins */
        status = NW_LMgr_BidiFlowBox_CollapseMargins(flow, box, parentContext, index);
        _NW_THROW_ON_ERROR(status);

        // NW_LOG2(NW_LOG_LEVEL5, "Handle block::: after collapse margins : parentcontext y: %d, ht %d", parentContext->y, box->bounds.dimension.height);
        /* Set the (x,y) coordinates of the block */
        if (!lineConstraint)
            {
            NW_LMgr_BidiFlowBox_PlaceChildAt(flow, box,
                                                   (NW_GDI_Metric_t)(parentContext->startMargin + margin.left),
                                                   parentContext->y);
            }
        else
            {
            NW_LMgr_BidiFlowBox_PlaceChildAt(flow, box,
                                                   (NW_GDI_Metric_t)(parentContext->lineStart + margin.left),
                                                   parentContext->y);
            }

        /* Now have the box set its own idea of size */
        status = NW_LMgr_Box_Resize(box, parentContext);
        _NW_THROW_ON_ERROR(status);

        // Deepika: There should not be special handling. PostFormat should be called on leaf
        // node s as well - To be done later
        if (NW_Object_IsInstanceOf(box, &NW_LMgr_MediaBox_Class) ||
            NW_Object_IsInstanceOf(box, &NW_FBox_FormBox_Class) )
            {
            // In case of boxes which have fixed content size, e.g. RuleBox, there will not
            // be any children to be formatted. They will be leaf nodes and there won't be
            // any postformat performed on them. That's why we need to add new line here
            status =  NW_LMgr_BidiFlowBox_HandleBlockPost(flow, box, parentContext);
            _NW_THROW_ON_ERROR(status);
            }
        }

    NW_CATCH(status)
        { /* empty */
        }

    NW_FINALLY {
        return status;
    } NW_END_TRY;
    }

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleBlockPost (NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    /* We're assuming that block boxes are always followed by a newline too.
       If someone decides the CSS spec should be read differently, we can just
       remove newline call. */
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
    TBrowserStatusCode status =  NW_LMgr_BidiFlowBox_NewLine(flow, parentContext,
            boxBounds.dimension.height, NW_TRUE);
    return status;

    }


/* --------------------------------------------------------------------------*/
/* returns: KBrsrLmgrNotClear, KBrsrSuccess */
TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_PlaceFloat(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_PropertyValue_t floatValue;
    NW_LMgr_FrameInfo_t margin;

    floatValue.token = NW_CSS_PropValue_none;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_float,
                                       NW_CSS_ValueType_Token, &floatValue);

    NW_LMgr_Box_GetMargins(box, &margin, NULL, ELMgrFrameLeft|ELMgrFrameRight|ELMgrFrameTop);
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
    /* Check that there is enough room for the float */
    if (parentContext->lineEnd - parentContext->lineStart <=
        boxBounds.dimension.width + margin.left + margin.right)
        {
        if ( NW_Object_IsDerivedFrom(box, &NW_LMgr_ImgContainerBox_Class) &&
             NW_LMgr_ImgContainerBox_ImageIsBroken( box ) == NW_TRUE &&
             (parentContext->lineStart == 4))
            {
            boxBounds.dimension.width = (NW_GDI_Metric_t)(parentContext->lineEnd - parentContext->lineStart);
            NW_LMgr_Box_SetFormatBounds( box, boxBounds );
            }
        else
            {
            NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

            if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
              {
              if(NW_LMgr_BidiFlowBox_AtNewLine(flow, parentContext))
                {
                boxBounds.dimension.width = (NW_GDI_Metric_t)(parentContext->lineEnd - parentContext->lineStart);
                NW_LMgr_Box_SetFormatBounds( box, boxBounds );
                }
              else
                {
                return KBrsrLmgrNotClear;
                }
              }
          }
      }

    /* Place the float in the flow */
    if (floatValue.token == NW_CSS_PropValue_left)
        {

        NW_LMgr_BidiFlowBox_PlaceChildAt(flow, box,
                                               ((NW_GDI_Metric_t)
                                                (parentContext->lineStart + margin.left)),
                                               ((NW_GDI_Metric_t)
                                                (parentContext->y + margin.top)));

        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
        parentContext->lineStart =
            ((NW_GDI_Metric_t)
             (parentContext->lineStart + boxBounds.dimension.width
              + margin.left + margin.right));
        }
    else
        {
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
        NW_LMgr_BidiFlowBox_PlaceChildAt(flow, box,
                                               ((NW_GDI_Metric_t)
                                                (parentContext->lineEnd - margin.right
                                                 - boxBounds.dimension.width)),
                                               ((NW_GDI_Metric_t)
                                                (parentContext->y + margin.top)));
        parentContext->lineEnd =
            ((NW_GDI_Metric_t)
             (parentContext->lineEnd - boxBounds.dimension.width
              - margin.left - margin.right));
        }
    return KBrsrSuccess;
    }

/* --------------------------------------------------------------------------*/
/* A method to handle float boxes.
 * returns: KBrsrLmgrNotClear, KBrsrOutOfMemory,
 *          KBrsrUnexpectedError
 */
TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleFloat(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_PropertyValue_t clearValue;
    NW_Bool isClearLeft, isClearRight;
    NW_LMgr_PropertyValueToken_t clearToken;
    NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;

    if (placedFloats == NULL)
        {
        return KBrsrLmgrNotClear;
        }

    clearValue.token = NW_CSS_PropValue_none;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_clear,
                                       NW_CSS_ValueType_Token, &clearValue);
    clearToken = clearValue.token;

    /* Get our parentContext info */
    isClearLeft = (NW_Bool)(parentContext->startMargin == parentContext->lineStart);
    isClearRight = (NW_Bool)(parentContext->endMargin == parentContext->lineEnd);

    /* If we are not clear, add the box to the list of pending floats */
    if (!(((clearToken == NW_CSS_PropValue_left) && isClearLeft)
          || ((clearToken == NW_CSS_PropValue_right) && isClearRight)
          || ((clearToken == NW_CSS_PropValue_both) && isClearLeft && isClearRight)
          || (clearToken == NW_CSS_PropValue_none))
       )
        {
        return KBrsrLmgrNotClear;
        }

    NW_TRY(status)
        {
        /* Format the float */
        if (NW_LMgr_Box_HasFixedContentSize(box))
            {
            // Resize the float using its intrinsic width/height
            status = NW_LMgr_Box_Resize(box, parentContext);
            _NW_THROW_ON_ERROR(status);

            // Place the float
            status = NW_LMgr_BidiFlowBox_PlaceFloat(flow, box, parentContext);
            _NW_THROW_ON_ERROR(status);

            // In case of boxes which have fixed content size, e.g. image, there will not
            // be any children to be formatted. They will be leaf nodes and there won't be
            // any postformat performed on them. That's why we need to add them to
            // placedFloat and reformat line boxes in this method, rather than in
            // PostFormatFloat

            // Add the float to the vector
            void *floatPtr = NW_ADT_DynamicVector_InsertAt(placedFloats, &box, NW_ADT_Vector_AtEnd);
            NW_THROW_OOM_ON_NULL(floatPtr, status);

            // Reformat any boxes that are already on the line
            status = NW_LMgr_BidiFlowBox_ReformatLine(flow, parentContext);
            _NW_THROW_ON_ERROR(status);

			// if floating fixed size box was inside an <a> tag currently ActiveContainerBox
			// will not get resized. So, we explicitly resize it here
			if (NW_Object_IsInstanceOf(box->parent, &NW_LMgr_ActiveContainerBox_Class))
				{
				status = NW_LMgr_ContainerBox_ResizeToChildren(box->parent);
				_NW_THROW_ON_ERROR(status);

				}
            }
        else
            {
            NW_LMgr_Property_t wprop;

            wprop.value.token = NW_CSS_PropValue_auto;
            wprop.type = NW_CSS_ValueType_Token;
            (void)NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_width, &wprop);

            if ((wprop.type == NW_CSS_ValueType_Token)
                && (wprop.value.token == NW_CSS_PropValue_auto))
                {
                /* Set the auto width to 25% -- this is completely arbitrary,
                 but makes better sense than not showing the box altogether */
                wprop.type = NW_CSS_ValueType_Percentage;
                wprop.value.decimal = 25;
                status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_width, &wprop);
                _NW_THROW_ON_ERROR(status);
                /* set the flag so that we know that this width was set by float */
                wprop.type = NW_CSS_ValueType_Token;
                wprop.value.token = NW_CSS_PropValue_flags_floatWidth;
                status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_flags, &wprop);
                _NW_THROW_ON_ERROR(status);
                }
            /* Constrain using the width property */
            status = NW_LMgr_Box_Constrain(box, 0);
            if (status != KBrsrLmgrConstraintTooSmall)
                {
                /* just ignore too small */
                _NW_THROW_ON_ERROR(status);
                }

            /* Place the float */
            status = NW_LMgr_BidiFlowBox_PlaceFloat(flow, box, parentContext);
            _NW_THROW_ON_ERROR(status);

            /* Resize the float */
            status = NW_LMgr_Box_Resize(box, parentContext);
            _NW_THROW_ON_ERROR(status);
            }

        }
    NW_CATCH(status)
        {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }



/* ----------------------------------------------------------------------------*/
/* A private method to handle break boxes. */
TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_HandleBreak(NW_LMgr_Box_t* flow,
        NW_LMgr_Box_t* box,
        NW_LMgr_FormatContext_t* parentContext)
    {
    NW_LMgr_PropertyValue_t clearValue;

    NW_TRY(status)
        {
        /* Resize */

        status = NW_LMgr_Box_Resize(box, parentContext);
        _NW_THROW_ON_ERROR(status);

        /* Add the break box to the line */
        status = NW_LMgr_LineBox_Append(parentContext->currentLine, box, parentContext->direction);
        _NW_THROW_ON_ERROR(status);

        /* Set the (x,y) coordinates for the box */
        NW_LMgr_BidiFlowBox_PlaceChild(flow, box, parentContext);

        /* If the box dimensions (ht/width) are zero, we do not want this box to participate in
           the layout and create a new line (for e.g. in the vertical layout mode */
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
        if ((boxBounds.dimension.height == 0) && (boxBounds.dimension.width == 0))
            NW_THROW_SUCCESS(status);

        /* Jump to the next line */
        status = NW_LMgr_BidiFlowBox_NewLine (flow, parentContext, 0, NW_TRUE);
        _NW_THROW_ON_ERROR(status);

        /* Get the "clear" property */
        clearValue.token = NW_CSS_PropValue_none;
        (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_clear,
                                           NW_CSS_ValueType_Token, &clearValue);

        /* Advance to the next clear position, if necessary */
        status = NW_LMgr_BidiFlowBox_AdvanceToClear(flow, parentContext, clearValue.token);
        _NW_THROW_ON_ERROR(status);
        }
    NW_CATCH(status)
    {}
    NW_FINALLY {
        return status;
    }
    NW_END_TRY
    }



/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_BidiFlowBox_Initialize
 * A public method to add any anonymous block container boxes to
 * a block container that contains both block and inline content.
 * This assures that all content in a block box participates
 * in the same format context (either block or inline).  This becomes
 * essential when margins are collapsed.  CSS2 p98.
 */

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_Initialize(NW_LMgr_BidiFlowBox_t *flow)
    {
    NW_ADT_Vector_Metric_t startInlineContent = 0;
    NW_ADT_Vector_Metric_t endInlineContent = 0;
    NW_ADT_Vector_Metric_t childIndex;
    NW_ADT_Vector_Metric_t childCount;
    NW_ADT_Vector_Metric_t i;
    NW_LMgr_Box_t *childBox;
    NW_LMgr_ContainerBox_t *container = NW_LMgr_ContainerBoxOf(flow);
    NW_LMgr_Box_t *inlineChild;
    NW_LMgr_PropertyValue_t dispValue;
    NW_LMgr_PropertyValue_t floatValue;
    NW_LMgr_Property_t prop;
    NW_Bool inlineStarted = NW_FALSE, inlineEnded = NW_FALSE;
    NW_Bool containsBlocks = NW_FALSE;
    NW_Bool childIsBlock = NW_FALSE;
    NW_LMgr_Box_t *anonBlock = NULL;
    NW_Bool canCollapse = NW_TRUE;
    
    NW_TRY(status) 
        {
        childIndex = 0;
        while (childIndex < NW_LMgr_ContainerBox_GetChildCount(container))
            {
            // Get the box at position i
            childBox = NW_LMgr_ContainerBox_GetChild(container, childIndex);
            
            if (NW_Object_IsInstanceOf(childBox, &NW_LMgr_SplitTextBox_Class))
                {
                NW_Object_Delete(childBox);
                childBox = NULL;
                continue;
                }
            
            childIndex++;
            }
        
        childCount = NW_LMgr_ContainerBox_GetChildCount(container);
        
        for (childIndex = 0; childIndex < childCount; childIndex++) 
            {
            // Get the box at position i
            childBox = NW_LMgr_ContainerBox_GetChild(container, childIndex);
            
            NW_LMgr_PropertyValue_t positionVal;
            
            positionVal.token = NW_CSS_PropValue_none;
            (void)NW_LMgr_Box_GetPropertyValue(childBox, NW_CSS_Prop_position,
                NW_CSS_ValueType_Token, &positionVal);
            if(positionVal.token == NW_CSS_PropValue_position_absolute)
                {
                // absolute positioned boxes should not cause a new format context to be created
                continue;
                }
            // If any of the children are block-level, set the flag
            dispValue.token = NW_CSS_PropValue_display_inline;
            (void)NW_LMgr_Box_GetPropertyValue(childBox, NW_CSS_Prop_display, NW_CSS_ValueType_Token, &dispValue);
            floatValue.token = NW_CSS_PropValue_none;
            (void)NW_LMgr_Box_GetPropertyValue(childBox, NW_CSS_Prop_float, NW_CSS_ValueType_Token, &floatValue);
            if (((dispValue.token == NW_CSS_PropValue_display_block) ||
                (dispValue.token == NW_CSS_PropValue_display_listItem)) &&
                (floatValue.token == NW_CSS_PropValue_none))
                {
                containsBlocks = NW_TRUE;
                childIsBlock = NW_TRUE;
                }
            
            /* We have found some inline content */
            else 
                {
                childIsBlock = NW_FALSE;
                
                if (!inlineStarted) 
                    {
                    startInlineContent = childIndex;
                    inlineStarted = NW_TRUE;
                    }
                }
            
                /* If we have encountered any inline content previously, and now we
                encounter a block following it, or we are the end of the container,
            we must enclose any inline elements with an anonymous block box. */
            if (childIsBlock && inlineStarted) 
                {
                endInlineContent = (NW_ADT_Vector_Metric_t)(childIndex - 1);
                inlineEnded = NW_TRUE;
                }
            
            else if (inlineStarted && (childIndex == childCount-1) && containsBlocks)
                {
                endInlineContent = childIndex;
                inlineEnded = NW_TRUE;
                }
            
            if (inlineEnded)
                {
                // Create the anonymous container */
                anonBlock = (NW_LMgr_Box_t*)LMgrAnonBlock_New(0);
                NW_THROW_OOM_ON_NULL(anonBlock, status);
                
                // Move the inline children to the new block box
                for (i = startInlineContent; i <= endInlineContent; i++)
                    {
                    inlineChild = NW_LMgr_ContainerBox_GetChild(container,
                        startInlineContent);
                    
                    // Get the minimum size
                    if (canCollapse)
                        {
                        if (NW_Object_IsInstanceOf(inlineChild, &NW_LMgr_AbstractTextBox_Class))
                            {
                            canCollapse = NW_LMgr_AbstractTextBox_IsBlank(NW_LMgr_AbstractTextBoxOf(inlineChild));
                            _NW_THROW_ON_ERROR(status);
                            }
                        else
                            {
                            canCollapse = NW_FALSE;
                            }
                        }
                    
                    // Add the child to the new flow
                    status =
                        NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(anonBlock),
                        inlineChild);
                    _NW_THROW_ON_ERROR(status);
                    
                    childCount--;
                    childIndex--;
                    }
                
                // If the inline content appears to be empty,
                //   do not add it to the flow
                if (!canCollapse)
                    {
                    // Insert the new container
                    status = NW_LMgr_ContainerBox_InsertChildAt(container, anonBlock,
                        startInlineContent);
                    _NW_THROW_ON_ERROR(status);
                    
                    // Set the display property on the container
                    prop.type = NW_CSS_ValueType_Token;
                    prop.value.token = NW_CSS_PropValue_display_block;
                    status = NW_LMgr_Box_SetProperty(anonBlock,
                        NW_CSS_Prop_display, &prop);
                    _NW_THROW_ON_ERROR(status);
                    
                    // Update the variables
                    childCount ++;
                    childIndex ++;
                    }
                else
                    {
                    NW_Object_Delete(anonBlock);
                    }
                
                // Reset the variables
                anonBlock = NULL;
                inlineStarted = NW_FALSE;
                inlineEnded = NW_FALSE;
                }
            }
            
        }
        NW_CATCH(status) 
            {
            NW_Object_Delete(anonBlock);
            }
        NW_FINALLY 
            {
            return status;
            }
        NW_END_TRY

    }
      

/* ------------------------------------------------------------------------- *
 * Function:     NW_LMgr_BidiFlowBox_InitializeSSL
 * A public method to remove any empty text child box in small screen mode
 */

TBrowserStatusCode 
TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_InitializeSSL(NW_LMgr_BidiFlowBox_t *flow)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_ADT_Vector_Metric_t childIndex;
    NW_ADT_Vector_Metric_t childCount;
    NW_LMgr_Box_t *childBox;
    NW_LMgr_ContainerBox_t *container = NW_LMgr_ContainerBoxOf(flow);

    // remove empty text box
    childCount = NW_LMgr_ContainerBox_GetChildCount(container);

    for (childIndex = 0; childIndex < childCount; childIndex++) 
        {
        // Get the box at position i
        childBox = NW_LMgr_ContainerBox_GetChild(container, childIndex);

        if( NW_Object_IsInstanceOf(childBox, &NW_LMgr_AbstractTextBox_Class) &&
            NW_LMgr_AbstractTextBox_IsBlank(NW_LMgr_AbstractTextBoxOf(childBox)) )
            {
            NW_Object_Delete(childBox);
            childBox = NULL;

            childCount--;
            childIndex--;
            }
        }

    return status;
    }


TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_BidiFlowBox_GetListMargin(NW_LMgr_FlowBox_t* flowBox)
{
  TBrowserStatusCode status;
  NW_LMgr_Property_t prop;
  NW_LMgr_Property_t propMargin;
  NW_LMgr_PropertyName_t propName;
  NW_LMgr_ContainerBox_t* parent;

  NW_ASSERT(flowBox != NULL);
  parent = NW_LMgr_Box_GetParent(flowBox);

  prop.type = NW_CSS_ValueType_Token;
  prop.value.token = NW_CSS_PropValue_ltr;
  status = NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(flowBox), NW_CSS_Prop_textDirection, &prop);

  if (prop.value.token == NW_CSS_PropValue_ltr) {
    propName = NW_CSS_Prop_leftMargin;
  }
  else {
    propName = NW_CSS_Prop_rightMargin;
  }
  propMargin.type = NW_CSS_ValueType_Px;
  status = NW_LMgr_Box_GetPropertyFromList(NW_LMgr_BoxOf(parent), NW_CSS_Prop_listMargin, &propMargin);
  /* We set the value, it must be there */
  if (status == KBrsrSuccess)
  {
  status = NW_LMgr_Box_GetPropertyFromList(NW_LMgr_BoxOf(flowBox), propName, &propMargin);
  if (status != KBrsrSuccess) {
    /* The margin was not set yet */
    status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(flowBox), propName, &propMargin);
  }
  }
  return status;
}

TBrowserStatusCode TLMgr_BoxFormatter::NW_LMgr_ResizeToFloat(NW_LMgr_Box_t* flow, NW_LMgr_FormatContext_t* parentContext)
    {
    NW_GDI_Rectangle_t flowBounds = NW_LMgr_Box_GetFormatBounds(flow);
    NW_ADT_Vector_Metric_t floatCount, index;
    NW_LMgr_Box_t *floatBox;
    NW_LMgr_PropertyValue_t floatValue;
    NW_GDI_Metric_t floatBottom;
    NW_LMgr_FrameInfo_t margin;
    
    NW_ADT_DynamicVector_t *placedFloats = parentContext->placedFloats;
    if (placedFloats)
        {
        floatCount = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(placedFloats));
        }
    else
        {
        floatCount = 0;
        }
    
    for (index = 0; index < floatCount; index++)
        {
        
        floatBox =
            *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (placedFloats, index);
        if( floatBox != flow && flow == (NW_LMgr_Box_t *)NW_LMgr_Box_GetParent(floatBox))
            {
            NW_ASSERT(floatBox != NULL);
            
            floatValue.integer = NW_CSS_PropValue_none;
            NW_LMgr_Box_GetPropertyValue(floatBox, NW_CSS_Prop_float,
                NW_CSS_ValueType_Token, &floatValue);
            
            
            NW_LMgr_Box_GetMargins(floatBox, &margin, NULL, ELMgrFrameBottom );
            
            NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( floatBox );
            floatBottom = (NW_GDI_Metric_t)( boxBounds.point.y
                + boxBounds.dimension.height
                + margin.bottom);
            
            if( flowBounds.dimension.height + flowBounds.point.y < floatBottom) {
                flowBounds.dimension.height = floatBottom - flowBounds.point.y  ;
                NW_LMgr_Box_SetFormatBounds(flow, flowBounds);
                }
            }
        }
    
    return KBrsrSuccess;
    }


