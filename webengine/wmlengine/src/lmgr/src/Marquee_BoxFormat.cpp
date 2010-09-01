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
* Description:  Description : The class handles formatting of the marquee boxes
*
*/


#include "CStack.h"
#include "BoxFormat.h"

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_FlowInline(NW_LMgr_Box_t* flow,
                                                            NW_LMgr_Box_t* box,
                                                            NW_LMgr_FormatContext_t* context) 
  {
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(flow);
  NW_ASSERT(box);
  NW_ASSERT(context);

  /* Inline a container of some other kind */
  if (NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class)){
    status = NW_LMgr_BidiFlowBox_HandleInlineContainer(flow, NW_LMgr_ContainerBoxOf(box), context);
  }

  /* Inline a media box */
  else if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class)){
   status =  NW_LMgr_BidiFlowBox_HandleInline(flow, box, context);
  }

  /* Inline an empty box */
  else if (NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class)){
    status =  NW_LMgr_BidiFlowBox_HandleInline(flow, box, context);
  }

  /* Otherwise its a schema violation */
  else{
    status = KBrsrFailure;
  }
  
  return status;

  }

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_PostFlowInline(NW_LMgr_Box_t* flow,
                                                               NW_LMgr_Box_t* box,
                                                               NW_LMgr_FormatContext_t* context)
  {
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(flow);
  NW_ASSERT(box);
  NW_ASSERT(context);

  // Inline a Marquee
  if (NW_Object_IsInstanceOf(box, &NW_LMgr_MarqueeBox_Class)) {
    status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, context);
  }
 
  /* Inline a container of some other kind */
  if (NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class)){
    status = NW_LMgr_BidiFlowBox_HandleInlineContainerPost(flow, NW_LMgr_ContainerBoxOf(box), context);
  }

  return status;
  }

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_FlowBlock(NW_LMgr_Box_t* flow,
                                                           NW_LMgr_Box_t* box,
                                                           NW_LMgr_FormatContext_t* context)
  {
  TBrowserStatusCode status = KBrsrSuccess;

  NW_REQUIRED_PARAM(flow);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(context);

  /* Inline a container of some other kind */
  if (NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class)){
    status = NW_LMgr_BidiFlowBox_HandleInlineContainer(flow, NW_LMgr_ContainerBoxOf(box), context);
  }

  /* Inline a media box */
  else if (NW_Object_IsInstanceOf (box, &NW_LMgr_MediaBox_Class)){
   status =  NW_LMgr_BidiFlowBox_HandleInline(flow, box, context);
  }

  /* Inline an empty box */
  else if (NW_Object_IsClass (box, &NW_LMgr_EmptyBox_Class)){
    status =  NW_LMgr_BidiFlowBox_HandleInline(flow, box, context);
  }

  /* Otherwise its a schema violation */
  else{
    status = KBrsrFailure;
  }
  
  return status;
  }

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_PostFlowBlock(NW_LMgr_Box_t* flow,
                                                               NW_LMgr_Box_t* box,
                                                               NW_LMgr_FormatContext_t* context)
  {
  TBrowserStatusCode status = KBrsrSuccess;


  NW_REQUIRED_PARAM(flow);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(context);


  // Inline a Marquee
  if (NW_Object_IsInstanceOf(box, &NW_LMgr_MarqueeBox_Class)) {
    status = NW_LMgr_BidiFlowBox_HandleInline(flow, box, context);
  }
 
  /* Inline a container of some other kind */
  if (NW_Object_IsInstanceOf (box, &NW_LMgr_ContainerBox_Class)){
    status = NW_LMgr_BidiFlowBox_HandleInlineContainerPost(flow, NW_LMgr_ContainerBoxOf(box), context);
  }

  return status;
  }

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_Float(NW_LMgr_Box_t* flow,
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

  return KBrsrSuccess;
  }

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_ListItem(NW_LMgr_Box_t* flow, 
                            NW_LMgr_Box_t* box,
                            NW_LMgr_FormatContext_t* context)
  {
  NW_REQUIRED_PARAM(flow);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(context);

  return KBrsrFailure;
  }

TBrowserStatusCode
TLMgr_Marquee_Box_Formatter::NW_LMgr_BidiFlowBox_DirectKey(NW_LMgr_Box_t* flow, 
                            NW_LMgr_Box_t* box,
                            NW_LMgr_FormatContext_t* context)
  {
  NW_REQUIRED_PARAM(flow);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(context);

  return KBrsrFailure;
  }
