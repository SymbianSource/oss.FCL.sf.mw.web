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


#include "nw_lmgr_formatboxi.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_rootbox.h"
#include "BrsrStatusCodes.h"

/* -------------------------------------------------------------------------
   The static instance of the Format_Box class object 
 ------------------------------------------------------------------------- */
const
NW_LMgr_FormatBox_Class_t  NW_LMgr_FormatBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_ContainerBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_FormatBox_t),
    /* construct                 */ _NW_LMgr_ContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_FormatBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_FormatBox_Constrain,
    /* draw                      */ _NW_LMgr_Box_Draw,
    /* render                    */ _NW_LMgr_ContainerBox_Render,
    /* getBaseline               */ _NW_LMgr_ContainerBox_GetBaseline,
    /* shift                     */ _NW_LMgr_ContainerBox_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ContainerBox      */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_FormatBox         */
    /* applyFormatProps          */ 0
  }
};


/*
 * Format boxes are resized by laying out their children with one
 * dimension constrained.  They grow to whatever size is needed in the
 * other dimension. Before the child layout takes place, the
 * constraint must be set, so constrain must be called before calling
 * this or the results of resize will be undetermined.
 */

TBrowserStatusCode
_NW_LMgr_FormatBox_Resize(NW_LMgr_Box_t* box, NW_LMgr_FormatContext_t* context)
{
  NW_LMgr_FormatContext_t *newContext = NULL;

  NW_TRY (status) {
    TBrowserStatusCode savedStatus = KBrsrSuccess;
    NW_GDI_Metric_t minWidth;

    /* When this method is entered, some size constraints must have been set.
     * Resize then lays the children out to get
     * the final size. In general, child layout keeps the size set in
     * one dimension but causes the box to grow in the other dimension.  
     */
  
    /* First allocate a new context for formatting children */
  
    newContext = NW_LMgr_FormatContext_New();
    NW_THROW_OOM_ON_NULL (newContext, status);
  
    /* Set formatting margins and the point where formatting starts */
  
    status = NW_LMgr_FormatBox_ApplyFormatProps((NW_LMgr_FormatBox_t *) box, newContext);
    if(status == KBrsrLmgrFormatOverflow) {
      savedStatus = status;
    }

    /* we don't enforce minimum width in the fixed width case to ensure table span display properly */
	if (!NW_LMgr_Box_HasFixedWidth (box)) {
      /* Make sure the box is big enough to include a reasonable space for formatting */
      minWidth = NW_LMGR_BOX_MIN_WIDTH;
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
      if( boxBounds.dimension.width < minWidth)
        {
        boxBounds.dimension.width = minWidth;
        NW_LMgr_Box_SetFormatBounds( box, boxBounds );
        savedStatus = KBrsrLmgrFormatOverflow;
	  }
	}
    context->formatChildren= NW_TRUE;
    context->formatBox = box;
    context->newFormatContext = newContext;
    newContext->referenceCount = 0;

    // return the format result
    status = savedStatus;

  } NW_CATCH (status) {  
  } NW_FINALLY {
    /* Clean up the new context we allocated */

    // jay  will be cleaned up in post format
    //NW_Object_Delete(newContext);
    return status;
  } NW_END_TRY
}

/* Format box constrain does the work of setting a preliminary size so
 * that resize can take place by formatting children within
 * constraints.  This must be called before resize.  Constrain sets
 * size in one dimension using either the width size property or the
 * supplied constraint.  
 */
 

TBrowserStatusCode
_NW_LMgr_FormatBox_Constrain(NW_LMgr_Box_t* box,
                             NW_GDI_Metric_t constraint)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_GDI_Dimension2D_t minSize;
  NW_GDI_Dimension2D_t setSize;

  NW_ASSERT(box != NULL);

  /* Get minimum size for the box. */
  (void) NW_LMgr_Box_GetMinimumSize(box, &minSize);

  /* See if any size properties are set, Currently we ignore return
   * status and use the values of setSize members to see if size
   * properties were set 
   */
  (void) NW_LMgr_Box_GetSizeProperties(box, &setSize);
  
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  /* If a constraint was supplied, apply it */
  if(constraint > 0)
    {
    /* If no horizontal size property, set according to constraint */
    if(setSize.width <= 0)
      {
      boxBounds.dimension.width = constraint;
      NW_LMgr_Box_SetFormatBounds( box, boxBounds );
      }
    /* Else, set to minimum width.  Any size properties are already included
     * in this value. 
     */
    else 
      {
      boxBounds.dimension.width = minSize.width;
      NW_LMgr_Box_SetFormatBounds( box, boxBounds );
      if(constraint < minSize.width)
        {
        /* constraint too small */
        status = KBrsrLmgrConstraintTooSmall;
      }
    } 
  }

  /* If no constraint was provided, set to minimum width.  We cannot do
   * a layout without a constraint. */
  else 
    {
    boxBounds.dimension.width = minSize.width;
    NW_LMgr_Box_SetFormatBounds( box, boxBounds );
  }

  return status;
}














