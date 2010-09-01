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


#include "nw_lmgr_lineboxi.h"
#include "nw_adt_resizablevector.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_rootbox.h"
#include "BrsrStatusCodes.h"

/* -------------------------------------------------------------------------
   The static instance of the Format_Box class object 
 ------------------------------------------------------------------------- */
const
NW_LMgr_LineBox_Class_t  NW_LMgr_LineBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_Box_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_LineBox_t),
    /* construct                 */ _NW_LMgr_LineBox_Construct,
    /* destruct                  */ _NW_LMgr_LineBox_Destruct
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
  },
  { /* NW_LMgr_LineBox           */
    /* unused                    */ NW_Object_Unused
  }
};


TBrowserStatusCode
_NW_LMgr_LineBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp)
{
  NW_TRY (status) {
    NW_LMgr_LineBox_t* lineBox;
  
    /* for convenience */
    lineBox = NW_LMgr_LineBoxOf (dynamicObject);
  
    /* invoke our superclass constructor */
    status = _NW_LMgr_Box_Construct (dynamicObject, argp);
    if (status != KBrsrSuccess) {
      return status;
    }
  
    /* initialize the member variables */
    lineBox->line = (NW_ADT_DynamicVector_t*)
      NW_ADT_ResizableVector_New (sizeof (NW_LMgr_Box_t*), 10, 5 );
    NW_THROW_OOM_ON_NULL (lineBox->line, status);

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */

/* Line boxes must NOT delete boxes on the line list since these references
 * are always owned by other objects.
 */

void
_NW_LMgr_LineBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_LineBox_t* lineBox;
  
  /* for convenience */
  lineBox = NW_LMgr_LineBoxOf (dynamicObject);
  
  /* release our resources */
  NW_Object_Delete (lineBox->line);
}


TBrowserStatusCode
NW_LMgr_LineBox_Clear(NW_LMgr_LineBox_t *lineBox){

  NW_ASSERT(lineBox != NULL);
  NW_ASSERT(lineBox->line != NULL);
  
  return NW_ADT_DynamicVector_Clear(lineBox->line);
}


NW_LMgr_Box_t*
NW_LMgr_LineBox_GetBox(NW_LMgr_LineBox_t *lineBox, NW_ADT_Vector_Metric_t index){
  
  NW_ASSERT(lineBox != NULL);
  NW_ASSERT(lineBox->line != NULL);
  
  return *(NW_LMgr_Box_t**)
    NW_ADT_Vector_ElementAt(lineBox->line, (NW_ADT_Vector_Metric_t) index);
}


NW_ADT_Vector_Metric_t
NW_LMgr_LineBox_GetCount(NW_LMgr_LineBox_t *lineBox){

  NW_ASSERT(lineBox != NULL);
  NW_ASSERT(lineBox->line != NULL);
  
  return NW_ADT_Vector_GetSize(lineBox->line);
}


TBrowserStatusCode
NW_LMgr_LineBox_Append(NW_LMgr_LineBox_t *lineBox, 
                       NW_LMgr_Box_t* box,
                       NW_GDI_FlowDirection_t dir){
  
  NW_ASSERT(lineBox != NULL);
  NW_ASSERT(lineBox->line != NULL);
  NW_ASSERT(box != NULL);
  
  if (dir == NW_GDI_FlowDirection_LeftRight) {
    if (NW_ADT_DynamicVector_InsertAt (lineBox->line, &box, 
                                       NW_ADT_Vector_AtEnd) == NULL) {
      return KBrsrOutOfMemory;
    }
  }
  else
  {
    if (NW_ADT_DynamicVector_InsertAt (lineBox->line, &box, 0) == NULL) {
      return KBrsrOutOfMemory;
    }
  }
  return KBrsrSuccess;
}

/* 
 * LineBox_HShift equally shifts all boxes in the line box along the
 * axis in which successive boxes in the linebox are laid out. This is
 * the shift that is done to support CSS text-alignment. This is called
 * "horizontal" here because it is orthogonal to the adjustment of boxes 
 * to support CSS vertical-alignment (See LineBox_VAlign). (For flows
 * that are in fact horizontal, this is actually a horizontal shift.)
 * LineBox_HShift does not change the relative alignment of boxes along 
 * this axis, i.e., it assumes that relative placement of boxes on this axis
 * has already been done.
 */

TBrowserStatusCode
NW_LMgr_LineBox_HAlign(NW_LMgr_LineBox_t *lineBox,
                       NW_LMgr_PropertyValueToken_t type, 
                       NW_GDI_Metric_t space,
                       NW_GDI_FlowDirection_t dir)
  {
  NW_ADT_Vector_Metric_t boxes = 0;
  NW_ADT_Vector_Metric_t i;
  NW_GDI_Point2D_t delta;
  NW_GDI_Metric_t width = 0;
  NW_GDI_Metric_t shift;
  NW_LMgr_Box_t *box;
  NW_Bool ltr;
  NW_LMgr_RootBox_t* rootBox;
  NW_Int32 rangeLimit;
  NW_LMgr_FrameInfo_t margin;

  NW_ASSERT (lineBox != NULL);
  
  /* How many boxes are there on the line */
  boxes = NW_LMgr_LineBox_GetCount(lineBox);

  /* First calculate the cumulative width of the line box */
  for(i = 0; i < boxes; i++)
    {
    box = NW_LMgr_LineBox_GetBox(lineBox, (NW_ADT_Vector_Metric_t) i);
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

    // make sure to calculate in the margins of the box when considering width
    NW_LMgr_Box_GetMargins(box, &margin, NULL, ELMgrFrameLeft | ELMgrFrameRight );

    width = (NW_GDI_Metric_t)(width + boxBounds.dimension.width + margin.left + margin.right);
    }
  
  NW_GDI_Rectangle_t lineBoxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( lineBox ) );
  lineBoxBounds.dimension.width = width;
  NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( lineBox ), lineBoxBounds );


  /* Get the paragraph direction */
  if (dir == NW_GDI_FlowDirection_LeftRight) 
    {
    ltr = NW_TRUE;
    }
  else 
    {
    ltr = NW_FALSE;
    }

  /* Position boxes horizontally according to their alignment props */  
  shift = 0;
  if (ltr) 
    {
    if(type == NW_CSS_PropValue_right)
      {
      shift = (NW_GDI_Metric_t)(space - width);
      }
    else if (type == NW_CSS_PropValue_center)
      {
      shift = (NW_GDI_Metric_t)((space - width)/2);
      }
    }
  else 
    {
    if(type == NW_CSS_PropValue_left)
      {
      shift = (NW_GDI_Metric_t)(-(space - width));
      }
    else if (type == NW_CSS_PropValue_center)
      {
      shift = (NW_GDI_Metric_t)(-(space - width)/2);
      }
    }

  /* This prevents any boxes from going beyond the left margin
     due to alignment. */
  if (shift < 0 && ltr) 
    {
    shift = 0;
    }
  rootBox = NW_LMgr_Box_GetRootBox( lineBox );
  if( rootBox != NULL )
    {
    rangeLimit = NW_LMgr_RootBox_GetRightLimit( rootBox );

    if (shift + width > rangeLimit ) 
      {
      shift = rangeLimit - width;
      }
    }

  /* Shift the boxes */
  delta.y = 0;
  delta.x = shift;
  for(i = 0; i < boxes; i++)
    {
    box = NW_LMgr_LineBox_GetBox(lineBox, (NW_ADT_Vector_Metric_t) i);
    (void)NW_LMgr_Box_Shift(box, &delta);
    }

  return KBrsrSuccess;
  }

/* Set the position of boxes in accordance with CSS vertical alignment
 * This sets boxes on an axis which is orthogonal to the axis along which
 * boxes are being laid out. (For horizontal flows this is vertical
 * positioning.) Unlike LineBox_HShift, this makes no assumption that boxes
 * have already been laid out vertically. Instead of shifting boxes, this
 * method sets their absolute positions relative to the y-location of the
 * line-box, a specified line-box height, and a specified base-line.
 */

TBrowserStatusCode
NW_LMgr_LineBox_VAlign( NW_LMgr_LineBox_t *lineBox, 
                        NW_GDI_Metric_t contextY,
                        NW_GDI_Metric_t *outBase, 
                        NW_GDI_Metric_t *outHeight ) 
    {
    NW_ADT_Vector_Metric_t boxes = 0;
    NW_ADT_Vector_Metric_t i, j, pass;
    NW_GDI_Metric_t lineHeight = 0;
    NW_GDI_Metric_t boxHeight = 0;
    NW_GDI_Metric_t lineBase = 0;
    NW_GDI_Metric_t topIncrement, bottomIncrement;
    NW_GDI_Metric_t lh = 0;
    NW_GDI_Metric_t lineEm = 0, lineEx = 0;
    NW_LMgr_Box_t* memberBox;
    NW_LMgr_FrameInfo_t margin;
    NW_GDI_Rectangle_t bounds;
    NW_GDI_Metric_t boxBase;
    NW_GDI_Metric_t boxTop;
    NW_LMgr_Property_t vaProp, lhProp;
    NW_GDI_Metric_t leading;
    NW_GDI_Metric_t *placed;
    TBrowserStatusCode status = KBrsrSuccess;

    NW_ASSERT (lineBox != NULL);
    NW_ASSERT (lineBox->line != NULL);

    /* Initialize the box height */
    NW_GDI_Rectangle_t lineBoxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( lineBox ) );
    lineBoxBounds.dimension.height = 0;
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( lineBox ), lineBoxBounds );

    /* Get the number of boxes */
    boxes = NW_ADT_Vector_GetSize(lineBox->line);

    /* Allocate the temporary array for the calculated locations */
    placed = (NW_GDI_Metric_t *) NW_Mem_Malloc (sizeof(NW_GDI_Metric_t) * boxes);
    if (placed == NULL) 
        {
        return KBrsrOutOfMemory;
        }
    NW_Mem_memset(placed, 0, sizeof(NW_GDI_Metric_t) * boxes);

    /* Calculate the line em & ex */
    lineEm = NW_LMgr_LineBox_GetEm(lineBox);
    lineEx = (NW_GDI_Metric_t)(lineEm / 2);

    /* In the first pass we calculate the line base and the line height */
    for (pass=0; pass<2; pass++) 
        {
        for(i = 0; i < boxes; i++)
            {
            memberBox = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt(lineBox->line, 
                                                       (NW_ADT_Vector_Metric_t) i);
            bounds = NW_LMgr_Box_GetFormatBounds(memberBox);

            /* Get the verlical-align property */
            vaProp.type = NW_CSS_ValueType_Token;
            vaProp.value.token = NW_CSS_PropValue_baseline;
            status = NW_LMgr_Box_GetProperty(memberBox, NW_CSS_Prop_verticalAlign, &vaProp);
            if (status == KBrsrNotFound) 
                {
                NW_LMgr_Box_t* parent = NW_LMgr_BoxOf( memberBox->parent );
                // vAlign is not inherited! 
                // If parent is container box (inline), try to get vAlign from the parent
                // e.g.  X <em style="vertical-align: super">2</em>
                if( NW_Object_IsClass( parent, &NW_LMgr_ContainerBox_Class ) )
                    {
                    (void)NW_LMgr_Box_GetProperty (parent, NW_CSS_Prop_verticalAlign, &vaProp);
                    }
                }

            if ((pass == 0) &&
            !((vaProp.type == NW_CSS_ValueType_Token) && (vaProp.value.token == NW_CSS_PropValue_baseline))) 
                {
                continue;
                }
            if ((pass == 1) &&
            (vaProp.type == NW_CSS_ValueType_Token) && (vaProp.value.token == NW_CSS_PropValue_baseline)) 
                {
                continue;
                }

            /* Get the line-height property */
            lhProp.type = NW_CSS_ValueType_Token;
            lhProp.value.token = NW_CSS_PropValue_normal;
            NW_LMgr_Box_GetProperty(memberBox, NW_CSS_Prop_lineHeight, &lhProp);

            /* Get the box margins */
            NW_LMgr_Box_GetMargins(memberBox, &margin, NULL, ELMgrFrameTop|ELMgrFrameBottom );

            /* Calculate the leading */
            if (lhProp.type == NW_CSS_ValueType_Px) 
                {
                lh = (NW_GDI_Metric_t)lhProp.value.integer;
                leading = (NW_GDI_Metric_t)(lh - bounds.dimension.height - margin.top - margin.bottom);
                if (leading <= 0)
                    {
                      leading = 0;
                    }
                }
            else if (lhProp.type == NW_CSS_ValueType_Number) 
                {
                NW_LMgr_PropertyValue_t fontSize;
                NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(memberBox->parent), NW_CSS_Prop_fontSize, 
                                         NW_CSS_ValueType_Px, &fontSize);
                lh = (NW_GDI_Metric_t)(fontSize.integer * lhProp.value.decimal);
                leading = (NW_GDI_Metric_t)(lh - bounds.dimension.height - margin.top - margin.bottom);
                if (leading <= 0)
                    {
                    leading = 0;
                    }
                }
            else 
                {
                leading = 0;
                }

            /* Get the box baseline */
            boxBase = NW_LMgr_Box_GetBaseline(memberBox);
            if (boxBase != bounds.dimension.height) 
                {
                boxBase = (NW_GDI_Metric_t)(boxBase + margin.top + leading/2);
                }
            else 
                {
                boxBase = (NW_GDI_Metric_t)(boxBase + margin.top + margin.bottom + leading/2);
                }

            /* Calculate the inline box height */
            boxHeight = (NW_GDI_Metric_t)(bounds.dimension.height 
                             + margin.top + margin.bottom + leading);

            /* Align the boxes */
            if (vaProp.type == NW_CSS_ValueType_Token) 
                {
                switch (vaProp.value.token) 
                    {
                    case NW_CSS_PropValue_top:
                        boxTop = 0;
                        break;  
                    case NW_CSS_PropValue_bottom:
                        boxTop = (NW_GDI_Metric_t)(lineHeight - boxHeight);
                        break;
                    case NW_CSS_PropValue_middle:
                    /* Align the vertical midpoint of the box with the baseline of the parent box plus half the x-height of the parent.*/
                        boxTop = (NW_GDI_Metric_t)(lineBase - lineEx / 2 - (boxHeight+1)/2);
                        break;
                    case NW_CSS_PropValue_sub:
                        boxTop = (NW_GDI_Metric_t)(lineBase + lineEx / 2 - boxBase);        
                        break;
                    case NW_CSS_PropValue_super:
                        boxTop = (NW_GDI_Metric_t)(lineBase - lineEx / 2 - boxBase);
                        break;
                    default:  /* baseline */
                        boxTop = (NW_GDI_Metric_t)(lineBase - boxBase);
                        break;
                    }
                }
            else if (vaProp.type == NW_CSS_ValueType_Px) 
                {    
                boxTop = (NW_GDI_Metric_t)(lineBase - boxBase - vaProp.value.integer);
                }
            else if (vaProp.type == NW_CSS_ValueType_Percentage) 
                {    
                boxTop = (NW_GDI_Metric_t)(lineBase - boxBase - vaProp.value.decimal / 100 * boxHeight);
                }
            else 
                {
                boxTop = 0;
                }

            /* Adjust the height and the baseline */
            topIncrement = bottomIncrement = 0;
            if (boxTop < 0) 
                {
                topIncrement = (NW_GDI_Metric_t)(-boxTop);
                }
            if (boxTop + boxHeight > lineHeight) 
                {
                bottomIncrement = (NW_GDI_Metric_t)(boxTop + boxHeight - lineHeight);
                }
            lineHeight = (NW_GDI_Metric_t)(lineHeight + topIncrement + bottomIncrement);
            lineBase = (NW_GDI_Metric_t)(lineBase + topIncrement);
            placed[i] = (NW_GDI_Metric_t)(boxTop + leading/2 + margin.top);

            /* If we changed the top, we must update the placed array */
            /* Note that the baseline boxes that were already placed need to be
            * updated, too, thus the condition "j<=boxes".  Any other boxes at 
            * j>i will be overwritten as we go along. */
            if (topIncrement > 0) 
                {
                for (j = 0; j < boxes; j ++) 
                    {
                    placed[j] = (NW_GDI_Metric_t)(placed[j] + topIncrement);
                    }
                }
            }
        }

    /* In the second pass we place the boxes */
    for(i = 0; i < boxes; i++)
        {
        /* Get the inline box */
        memberBox = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt(lineBox->line, 
                                                 (NW_ADT_Vector_Metric_t) i);
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( memberBox );
        boxBounds.point.y = (NW_GDI_Metric_t)(contextY + placed[i]);
        NW_LMgr_Box_SetFormatBounds( memberBox, boxBounds );
        }

    /* Set the calculated line height */
    lineBoxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( lineBox ) );
    lineBoxBounds.dimension.height = lineHeight;
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( lineBox ), lineBoxBounds );

    /* Free the temporary array */
    NW_Mem_Free(placed);

    *outHeight = lineHeight;
    *outBase = lineBase;

    // successful completion
    return KBrsrSuccess;
    }

NW_GDI_Metric_t
NW_LMgr_LineBox_GetEm(NW_LMgr_LineBox_t *lineBox){

  NW_ADT_Vector_Metric_t boxes = 0;
  NW_ADT_Vector_Metric_t i;
  NW_LMgr_Box_t *box;
  NW_GDI_Metric_t em = 0;
  NW_GDI_Metric_t lineEm = 0;

  NW_ASSERT (lineBox != NULL);

  boxes = NW_LMgr_LineBox_GetCount(lineBox);
  
  for(i = 0; i < boxes; i++){
    box = NW_LMgr_LineBox_GetBox(lineBox, (NW_ADT_Vector_Metric_t) i);

    em = NW_LMgr_Box_GetEm(box);
    lineEm = (NW_GDI_Metric_t) (lineEm < em ? em : lineEm);
  }

  return lineEm;
}



/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_LineBox_t*
NW_LMgr_LineBox_New ()
{
  return (NW_LMgr_LineBox_t*)
    NW_Object_New (&NW_LMgr_LineBox_Class, 0);
}


