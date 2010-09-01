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
* Description:  Description : The class handles formatting of the table cells and rows
*
*/


#include "BoxFormat.h"

TBrowserStatusCode
TLMgr_StaticCell_Box_Formatter::NW_LMgr_BidiFlowBox_Float(NW_LMgr_Box_t* flow,
                                                           NW_LMgr_Box_t* box,
                                                           NW_LMgr_FormatContext_t* context)
{
  NW_ASSERT(box);
  NW_ASSERT(context);
  NW_REQUIRED_PARAM(flow);

  NW_ADT_DynamicVector_t *pendingFloats = context->pendingFloats;
  NW_ADT_DynamicVector_t *placedFloats = context->pendingFloats;


  /* If the float context is not set, remove the box */
  if (pendingFloats == NULL || placedFloats == NULL) {
    return KBrsrFailure;
  }

  if (NW_ADT_DynamicVector_InsertAt(pendingFloats, &box, NW_ADT_Vector_AtEnd) == NULL) {
    return KBrsrOutOfMemory;
  }
  // we do not handle floating in cell.
  return KBrsrFailure;

}

TBrowserStatusCode
TLMgr_StaticCell_Box_Formatter::NW_LMgr_BidiFlowBox_NewLine(NW_LMgr_Box_t* flow,
                                                            NW_LMgr_FormatContext_t* context,
                                                            NW_GDI_Metric_t delta,
                                                            NW_Bool indentNewLine)
{
  NW_ASSERT(flow);
  NW_ASSERT(context);
  NW_REQUIRED_PARAM(indentNewLine);

  NW_TRY(status) {

    /* Flush the current line */
    status = NW_LMgr_BidiFlowBox_Flush(flow, context, NW_FALSE);
    _NW_THROW_ON_ERROR(status);

    /* Add the delta */
    context->y = (NW_GDI_Metric_t)(context->y + delta);
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf(flow) ); 
    boxBounds.dimension.height = (NW_GDI_Metric_t)
      (boxBounds.dimension.height + delta);
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( flow ), boxBounds ); 

    /* Reset the whitespace flag */
    context->pendingWhitespace = NW_FALSE;
  
    /* Update the context */
    context->lineStart = NW_LMgr_FlowBox_GetLineStart(flow, context,
                                                      context->y);
    context->lineEnd = NW_LMgr_FlowBox_GetLineEnd(flow, context, context->y);
    if (context->direction == NW_GDI_FlowDirection_LeftRight) {
      context->x = (NW_GDI_Metric_t)(context->lineStart
                                     + NW_LMgr_FlowBox_GetIndentation(flow,
                                                                      context));
  }
    else {
      context->x = (NW_GDI_Metric_t)(context->lineEnd
                                     - NW_LMgr_FlowBox_GetIndentation(flow,
                                                                      context));
    }

  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

TBrowserStatusCode 
TLMgr_StaticCell_Box_Formatter::NW_LMgr_FlowBox_FormatBox(NW_LMgr_Box_t* /*parentFormatBox*/,
                                                          NW_LMgr_Box_t* box,
                                                          NW_LMgr_FormatContext_t* parentContext)
  {
  NW_ASSERT(box);
  NW_ASSERT(parentContext);

  return NW_LMgr_Box_Resize(box, parentContext);
  }


TBrowserStatusCode
TLMgr_StaticCell_Box_Formatter::NW_LMgr_FlowBox_PostFormatBox(NW_LMgr_Box_t* /*parentFormatBox*/,
                                                             NW_LMgr_Box_t* /*box*/,
                                                             NW_LMgr_FormatContext_t* /*parentContext*/)
  {
  return KBrsrSuccess;
  }
