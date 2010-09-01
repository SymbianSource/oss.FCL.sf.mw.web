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


#ifndef BOX_FORMATTER_H
#define BOX_FORMATTER_H

#include <e32def.h>  // To avoid NULL redefine warning (no #ifndef NULL)


//#include "nw_lmgr_posflowbox.h"
#include "nw_lmgr_flowbox.h"
#include "nw_lmgr_formatcontext.h"
#include "nw_fbox_inputbox.h"
#include "nw_lmgr_splittextbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "nw_lmgr_marqueebox.h"
#include "nwx_settings.h"
#include "BrsrStatusCodes.h"
#include "nw_lmgr_emptybox.h"
#include "nw_lmgr_rulebox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_listbullets.h"
#include "nw_image_virtualimage.h"
#include "nw_image_cannedimages.h"
#include "nw_fbox_formbox.h"
#include "nwx_string.h"
#include "nw_lmgr_imgcontainerbox.h"
#include "LMgrObjectBoxOOC.h"
#include "nwx_settings.h"

#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


void PartialFormatL(NW_LMgr_Box_t* aFormatBox, TBool aDocumentComplete);
void FormatL(NW_LMgr_Box_t* aFormatBox,
       NW_LMgr_ContainerBox_t *containerBox,
       NW_LMgr_FormatContext_t* aContext);


class TLMgr_BoxFormatter
    {
    public:
        // check if they have to be virtual functions

        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_PostFormat(NW_LMgr_Box_t* flow,
                                       NW_LMgr_ContainerBox_t *whose,
                                       NW_LMgr_FormatContext_t* parentContext);

        virtual TBrowserStatusCode
        NW_LMgr_FlowBox_PostFormatBox(NW_LMgr_Box_t* parentFormatBox,
                                       NW_LMgr_Box_t* box,
                                       NW_LMgr_FormatContext_t* parentContext);

        virtual TBrowserStatusCode NW_LMgr_FlowBox_FormatBox(NW_LMgr_Box_t* parentFormatBox,
                           NW_LMgr_Box_t* box,
                           NW_LMgr_FormatContext_t* parentContext);
        // virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_FlowInline(NW_LMgr_Box_t* flow,
                                        NW_LMgr_Box_t* box,
                                        NW_LMgr_FormatContext_t* parentContext) ;
        // virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_PostFlowInline(NW_LMgr_Box_t* flow,
                                           NW_LMgr_Box_t* box,
                                           NW_LMgr_FormatContext_t* parentContext);
        // virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_FlowBlock(NW_LMgr_Box_t* flow,
                                       NW_LMgr_Box_t* box,
                                       NW_LMgr_FormatContext_t* parentContext);

        //virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_PostFlowBlock(NW_LMgr_Box_t* flow,
                                           NW_LMgr_Box_t* box,
                                           NW_LMgr_FormatContext_t* parentContext);

        // virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_PostFlowFloat(NW_LMgr_Box_t* flow,
                                           NW_LMgr_Box_t* box,
                                           NW_LMgr_FormatContext_t* parentContext);

        // virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_Float(NW_LMgr_Box_t* flow,
                                   NW_LMgr_Box_t* box,
                                   NW_LMgr_FormatContext_t* parentContext);

        // virtual function
        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_NewLine(NW_LMgr_Box_t* flow,
                                     NW_LMgr_FormatContext_t* context,
                                     NW_GDI_Metric_t delta,
                                     NW_Bool indentNewLine);

        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_ListItem(NW_LMgr_Box_t* flow,
                              NW_LMgr_Box_t* item,
                              NW_LMgr_FormatContext_t* context);

        virtual TBrowserStatusCode
        NW_LMgr_BidiFlowBox_DirectKey(NW_LMgr_Box_t* flow,
                              NW_LMgr_Box_t* item,
                              NW_LMgr_FormatContext_t* context);

        // public function
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_Flush(NW_LMgr_Box_t* flow,
                                  NW_LMgr_FormatContext_t* parentContext, NW_Bool addLineSpacing);
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_AdvanceToClear(NW_LMgr_Box_t* flow,
                                            NW_LMgr_FormatContext_t* parentContext,
                                            NW_LMgr_PropertyValueToken_t clearToken);

		TBrowserStatusCode
        NW_LMgr_BidiFlowBox_AdvanceToClearReformat(NW_LMgr_Box_t* flow,
                                            NW_LMgr_FormatContext_t* parentContext,
                                            NW_LMgr_PropertyValueToken_t clearToken);

        NW_GDI_Metric_t
        NW_LMgr_FlowBox_GetIndentation(NW_LMgr_Box_t* flow,
                                       NW_LMgr_FormatContext_t* parentContext);
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_FlushInlineContainers(NW_LMgr_Box_t* flow,
                NW_LMgr_FormatContext_t* parentContext);

        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_ReformatLine(NW_LMgr_Box_t* flow,
                                          NW_LMgr_FormatContext_t* parentContext);

        NW_LMgr_Box_t*
        NW_LMgr_BidiFlowBox_GetFirstBlock(NW_LMgr_Box_t* flow);

        NW_LMgr_Box_t*
        NW_LMgr_BidiFlowBox_GetLastBlock(NW_LMgr_Box_t* flow);

        NW_Bool IsClosedTop(NW_LMgr_Box_t *box);

        NW_Bool IsClosedBottom(NW_LMgr_Box_t *box);

        TBrowserStatusCode NW_LMgr_BidiFlowBox_CollapseMargins(NW_LMgr_Box_t* flow,
                NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* parentContext, NW_ADT_Vector_Metric_t index);

        NW_GDI_Metric_t NW_LMgr_FlowBox_GetLineStart(NW_LMgr_Box_t* flow,
                NW_LMgr_FormatContext_t* parentContext,
                NW_GDI_Metric_t where);

        NW_GDI_Metric_t NW_LMgr_FlowBox_GetLineEnd(NW_LMgr_Box_t* flow,
                NW_LMgr_FormatContext_t* parentContext,
                NW_GDI_Metric_t where);

        NW_Bool NW_LMgr_BidiFlowBox_AtNewLine(NW_LMgr_Box_t* flow,
                                               NW_LMgr_FormatContext_t* parentContext);

        void NW_LMgr_BidiFlowBox_GetConstraint(NW_LMgr_Box_t *flow,
                                                NW_LMgr_FormatContext_t *parentContext,
                                                NW_GDI_Metric_t *constraint);

        void
        NW_LMgr_BidiFlowBox_GetBlockMargins (NW_LMgr_Box_t* flow,
                                              NW_LMgr_Box_t* box,
                                              NW_LMgr_FormatContext_t *parentContext,
                                              NW_LMgr_FrameInfo_t *margin);

        void
        NW_LMgr_BidiFlowBox_PlaceChild(NW_LMgr_Box_t* flow,
                                        NW_LMgr_Box_t* box,
                                        NW_LMgr_FormatContext_t* parentContext);
        void
        NW_LMgr_BidiFlowBox_PlaceChildAt(NW_LMgr_Box_t* flow,
                                          NW_LMgr_Box_t* box,
                                          NW_GDI_Metric_t x,
                                          NW_GDI_Metric_t y);

        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_HandleInline(NW_LMgr_Box_t* flow,
                                          NW_LMgr_Box_t* box,
                                          NW_LMgr_FormatContext_t* parentContext);
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_HandleInlineContainer(NW_LMgr_Box_t* flow,
                NW_LMgr_ContainerBox_t* container,
                NW_LMgr_FormatContext_t* parentContext);
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_HandleInlineContainerPost(NW_LMgr_Box_t* flow,
                NW_LMgr_ContainerBox_t* container,
                NW_LMgr_FormatContext_t* parentContext);
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_HandleBlock (NW_LMgr_Box_t* flow,
                                          NW_LMgr_Box_t* box,
                                          NW_LMgr_FormatContext_t* parentContext);

        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_HandleBlockPost (NW_LMgr_Box_t* flow,
                                              NW_LMgr_Box_t* box,
                                              NW_LMgr_FormatContext_t* parentContext);
        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_PlaceFloat(NW_LMgr_Box_t* flow,
                                        NW_LMgr_Box_t *box,
                                        NW_LMgr_FormatContext_t* parentContext);

        TBrowserStatusCode NW_LMgr_BidiFlowBox_HandleFloat(NW_LMgr_Box_t* flow,
                                 NW_LMgr_Box_t* box,
                                 NW_LMgr_FormatContext_t* parentContext);

        TBrowserStatusCode
        NW_LMgr_BidiFlowBox_HandleBreak(NW_LMgr_Box_t* flow,
                                         NW_LMgr_Box_t* box,
                                         NW_LMgr_FormatContext_t* parentContext);

        NW_LMgr_Box_t* NW_LMgr_BidiFlowBox_GetMarker(NW_LMgr_Box_t* flowBox,
                              NW_LMgr_Box_t* item,
                              NW_LMgr_FormatContext_t* context,
                              NW_LMgr_PropertyValueToken_t* markerVerticalAlign,
                              NW_Bool* style);
        NW_LMgr_Box_t* NW_LMgr_BidiFlowBox_GetDirectKeyMarker(NW_LMgr_Box_t* item);
        NW_Image_AbstractImage_t*  NW_LMgr_BidiFlowBox_GetListStyleImage(NW_LMgr_Box_t* flowBox,
                                      NW_LMgr_PropertyValueToken_t token);

        TBrowserStatusCode NW_LMgr_BidiFlowBox_Initialize(NW_LMgr_BidiFlowBox_t *flow);
        TBrowserStatusCode NW_LMgr_BidiFlowBox_InitializeSSL(NW_LMgr_BidiFlowBox_t *flow);
        TBrowserStatusCode NW_LMgr_BidiFlowBox_GetListMargin(NW_LMgr_FlowBox_t* flowBox);
		TBrowserStatusCode NW_LMgr_ResizeToFloat(NW_LMgr_Box_t* flow, NW_LMgr_FormatContext_t* parentContext);
    };


class TLMgr_Marquee_Box_Formatter:public TLMgr_BoxFormatter
  {
public:
      // virtual function
      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_FlowInline(NW_LMgr_Box_t* flow,
                                      NW_LMgr_Box_t* box,
                                      NW_LMgr_FormatContext_t* parentContext) ;
      // virtual function
      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_PostFlowInline(NW_LMgr_Box_t* flow,
                                         NW_LMgr_Box_t* box,
                                         NW_LMgr_FormatContext_t* parentContext);
      // virtual function
      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_FlowBlock(NW_LMgr_Box_t* flow,
                                     NW_LMgr_Box_t* box,
                                     NW_LMgr_FormatContext_t* parentContext);

      //virtual function
      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_PostFlowBlock(NW_LMgr_Box_t* flow,
                                         NW_LMgr_Box_t* box,
                                         NW_LMgr_FormatContext_t* parentContext);

      // virtual function
      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_Float(NW_LMgr_Box_t* flow,
                                 NW_LMgr_Box_t* box,
                                 NW_LMgr_FormatContext_t* parentContext);

      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_ListItem(NW_LMgr_Box_t* flow,
                            NW_LMgr_Box_t* item,
                            NW_LMgr_FormatContext_t* context);

      virtual TBrowserStatusCode
      NW_LMgr_BidiFlowBox_DirectKey(NW_LMgr_Box_t* flow,
                              NW_LMgr_Box_t* item,
                              NW_LMgr_FormatContext_t* context);

  };


class TLMgr_StaticCell_Box_Formatter:public TLMgr_BoxFormatter
  {
  public:

    virtual TBrowserStatusCode
    NW_LMgr_FlowBox_FormatBox(NW_LMgr_Box_t* parentFormatBox,
                              NW_LMgr_Box_t* box,
                              NW_LMgr_FormatContext_t* parentContext);


    virtual TBrowserStatusCode
    NW_LMgr_BidiFlowBox_Float(NW_LMgr_Box_t* flow,
                              NW_LMgr_Box_t* box,
                              NW_LMgr_FormatContext_t* parentContext);

    virtual TBrowserStatusCode
    NW_LMgr_BidiFlowBox_NewLine(NW_LMgr_Box_t* flow,
                                NW_LMgr_FormatContext_t* context,
                                NW_GDI_Metric_t delta,
                                NW_Bool indentNewLine);
    virtual TBrowserStatusCode
    NW_LMgr_FlowBox_PostFormatBox(NW_LMgr_Box_t* parentFormatBox,
                             NW_LMgr_Box_t* box,
                             NW_LMgr_FormatContext_t* parentContext);


  };

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif

