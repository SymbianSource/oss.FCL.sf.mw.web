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


#include "nw_lmgr_bidiflowboxi.h"
#include "nw_lmgr_formatcontext.h"
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
#include "nwx_settings.h"
#include "nw_gdi_utils.h"

/* --------------------------------------------------------------------------*
 *                          FORWARD DECLARATIONS                               *
 * --------------------------------------------------------------------------*/






const
NW_LMgr_BidiFlowBox_Class_t  NW_LMgr_BidiFlowBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_FlowBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_BidiFlowBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_FlowBox_Resize,
    /* postResize                */ _NW_LMgr_FlowBox_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_FormatBox_Constrain,
    /* draw                      */ _NW_LMgr_Box_Draw,
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
  { /* NW_LMgr_BidiFlowBox       */
    /* unused                    */ NW_Object_Unused
  }
};






/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_BidiFlowBox_t*
NW_LMgr_BidiFlowBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_BidiFlowBox_t*)
    NW_Object_New (&NW_LMgr_BidiFlowBox_Class, numProperties);
}
