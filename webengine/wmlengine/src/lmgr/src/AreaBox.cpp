/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Box class associated with the <area> tag
*
*/



// INCLUDE FILES
#include <e32def.h>  // To avoid NULL redefine warning (no #ifndef NULL)
#include "nw_lmgr_areaboxi.h"
#include "nwx_string.h"
#include "nw_gdi_utils.h"
#include "nw_hed_hedeventhandler.h"

// LOCAL FUNCTION PROTOTYPES
static TBrowserStatusCode
NW_LMgr_AreaBox_InterpretShape(NW_LMgr_AreaBox_t* thisObj);

static TBrowserStatusCode
NW_LMgr_AreaBox_InterpretCoords(NW_LMgr_AreaBox_t* thisObj);

static NW_Ucs2*
NW_LMgr_AreaBox_ParseString (NW_LMgr_AreaBox_t* box, NW_Ucs2* string);

// ============================= INTERFACE CLASS ===============================

// -----------------------------------------------------------------------------
static
const
NW_LMgr_IEventListener_Class_t _NW_LMgr_AreaBox_IEventListener_Class = {
  { /* NW_Object_Core         */
    /* super                  */ &NW_LMgr_IEventListener_Class,
    /* queryInterface         */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface    */
    /* offset                 */ offsetof (NW_LMgr_AreaBox_t, NW_LMgr_IEventListener)
  },
  { /* NW_LMgr_IEventListener */
    /* processEvent           */ _NW_LMgr_AreaBox_IEventListener_ProcessEvent
  }
};

// -----------------------------------------------------------------------------
static
const NW_Object_Class_t* const _NW_LMgr_AreaBox_InterfaceList[] = {
  &_NW_LMgr_AreaBox_IEventListener_Class,
  NULL
};

// ========================= INTERFACE FUNCTIONS ===============================

// -----------------------------------------------------------------------------
NW_Uint8
_NW_LMgr_AreaBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                              NW_Evt_Event_t* event)
{
  NW_LMgr_AreaBox_t* thisObj;
  NW_Uint8 ret = 0;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (eventListener,
                                     &NW_LMgr_IEventListener_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (event, &NW_Evt_Event_Class));

  // obtain the implementer 
  thisObj = static_cast<NW_LMgr_AreaBox_t*>(NW_Object_Interface_GetImplementer (eventListener));
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_LMgr_AreaBox_Class));
  
  NW_LMgr_Box_Refresh(NW_LMgr_BoxOf(thisObj->iImageMapBox));

  /* notify the event handler (if there is one) */
  if (thisObj->eventHandler != NULL) 
  {
    ret =  NW_LMgr_EventHandler_ProcessEvent (thisObj->eventHandler,
                                              NW_LMgr_BoxOf (thisObj), event);
  }
  return ret; 
}

// ========================= CLASS DECLARATION =================================

// -----------------------------------------------------------------------------
const
NW_LMgr_AreaBox_Class_t NW_LMgr_AreaBox_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_LMgr_Box_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ _NW_LMgr_AreaBox_InterfaceList
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_LMgr_AreaBox_t),
    /* construct                  */ _NW_LMgr_AreaBox_Construct,
    /* destruct                   */ _NW_LMgr_AreaBox_Destruct
  },
  { /* NW_LMgr_Box                */
    /* split                      */ _NW_LMgr_Box_Split,
    /* resize                     */ _NW_LMgr_Box_Resize,
    /* postResize                */  _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize      */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize        */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                  */ _NW_LMgr_Box_Constrain,
    /* draw                       */ _NW_LMgr_Box_Draw,
    /* render                     */ _NW_LMgr_Box_Render,
    /* getBaseline                */ _NW_LMgr_Box_GetBaseline,
    /* shift                      */ _NW_LMgr_Box_Shift,
    /* clone                      */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_ActiveBox          */
    /* unused                     */ NW_Object_Unused
  },
  { /* NW_LMgr_AreaBox            */
    /* unused                     */ NW_Object_Unused
  }
};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_LMgr_AreaBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp)
{
  NW_LMgr_AreaBox_t* thisObj = NW_LMgr_AreaBoxOf (dynamicObject);

  // invoke our superclass constructor
  TBrowserStatusCode status = _NW_LMgr_Box_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) 
  {
    return status;
  }

  // initialize the member variables
  thisObj->eventHandler = va_arg (*argp, NW_LMgr_EventHandler_t*);

  thisObj->iImageMapBox = va_arg (*argp, NW_LMgr_ImageMapBox_t*);

  thisObj->iShapeText = va_arg (*argp, NW_Text_t*);
  thisObj->iCoordsText = va_arg (*argp, NW_Text_t*);
  thisObj->iAltText = va_arg (*argp, NW_Text_t*);
  thisObj->iHRefText = va_arg (*argp, NW_Text_t*);

  thisObj->iShape = NW_LMGR_ABS_RECT;

  thisObj->iIsInvalid = NW_FALSE;

  // InterpretShape and InterpretCoords take the text attribute arguements and 
  // convert them to the appropriate data types
  NW_LMgr_AreaBox_InterpretShape(thisObj);

  if (NW_LMgr_AreaBox_InterpretCoords(thisObj) != KBrsrSuccess)
  {
    thisObj->iIsInvalid = NW_TRUE;
  }

  // successful completion
  return status;
}

// -----------------------------------------------------------------------------
void
_NW_LMgr_AreaBox_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_AreaBox_t* thisObj = NW_LMgr_AreaBoxOf (dynamicObject);

  // destroy the EventHandler 
  NW_Object_Delete (thisObj->eventHandler);
  
  // destroy the text member variables
  if (thisObj->iCoordsText != NULL)
  {
    NW_Object_Delete (thisObj->iCoordsText);
    thisObj->iCoordsText = NULL;
  }

  if (thisObj->iShapeText != NULL)
  {
    NW_Object_Delete (thisObj->iShapeText);
    thisObj->iShapeText = NULL;
  }

  if (thisObj->iAltText != NULL)
  {
    NW_Object_Delete (thisObj->iAltText);
    thisObj->iAltText = NULL;
  }

  if (thisObj->iHRefText != NULL)
  {
    NW_Object_Delete (thisObj->iHRefText);
    thisObj->iHRefText = NULL;
  }
}

// -----------------------------------------------------------------------------
NW_LMgr_AreaBox_t*
NW_LMgr_AreaBox_New (NW_ADT_Vector_Metric_t aNumProperties,
                     NW_LMgr_EventHandler_t* aEventHandler,
                     NW_LMgr_ImageMapBox_t* aImageMapBox,
                     NW_Text_t* aShapeText,
                     NW_Text_t* aCoordsText,
                     NW_Text_t* aAltText,
                     NW_Text_t* aHRefText)
{
  return (NW_LMgr_AreaBox_t*) NW_Object_New (&NW_LMgr_AreaBox_Class, 
    aNumProperties, aEventHandler, aImageMapBox, aShapeText, aCoordsText, 
    aAltText, aHRefText);
}

// -----------------------------------------------------------------------------
NW_GDI_Rectangle_t
NW_LMgr_AreaBox_GetBounds (NW_LMgr_Box_t* box)
{
  NW_LMgr_AreaBox_t* thisObj;
  NW_GDI_Rectangle_t overlap;

  // for convenience 
  thisObj = NW_LMgr_AreaBoxOf (box);
  
  NW_GDI_Rectangle_t areaBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( thisObj ) );

  NW_GDI_Rectangle_t imageBounds = NW_LMgr_Box_GetDisplayBounds( NW_LMgr_BoxOf( thisObj->iImageMapBox ) );

  // Calculate the iComboBounds (which is the bounds of the area box relative 
  // to the screen instead of the image)
  thisObj->iComboBounds.point.x = areaBounds.point.x + imageBounds.point.x;    
  thisObj->iComboBounds.point.y = areaBounds.point.y + imageBounds.point.y;

  thisObj->iComboBounds.dimension = areaBounds.dimension;

  // Now that we know where the area box is on the screen, lets check to make
  // sure that the area box overlaps the image box.  In the most general case,
  // the area box should be contained within the image map box.  But in some edge
  // cases, flaky web developers might specify bounds for an area that lie outside
  // the image map box bounds. So there are three cases to consider
  // 1. The area box lies completely outside the image map box.
  //    In this case we will not display the area box (ignore it)
  // 2. The area lies partially in the image map box.  
  //    We will take the resulting overlap as the bounds for our area box
  // 3. The area box lies withing the image map box.
  //    In which case the overlap bounds will equal the area box bounds and all is
  //    well with the world:-)
  // For more information on how NW_GDI_Rectangle_Cross behaves, see the header
  // file.
  if (NW_GDI_Rectangle_Cross(&(thisObj->iComboBounds), &imageBounds, &overlap)) 
  {
    thisObj->iComboBounds.point.x = overlap.point.x;    
    thisObj->iComboBounds.point.y = overlap.point.y;

    thisObj->iComboBounds.dimension = overlap.dimension;
  }
  else
  {
    thisObj->iComboBounds.point.x = 0;    
    thisObj->iComboBounds.point.y = 0;

    thisObj->iComboBounds.dimension.height = 0;
    thisObj->iComboBounds.dimension.width = 0;
  }

  (void) NW_LMgr_Box_UpdateDisplayBounds( NW_LMgr_BoxOf(thisObj), thisObj->iComboBounds );
  return thisObj->iComboBounds;
}

// -----------------------------------------------------------------------------
void
NW_LMgr_AreaBox_SetBounds( NW_LMgr_Box_t* /*aBox*/, NW_GDI_Rectangle_t /*aBoxBounds*/ )
  {
  // not supported yet
  }

// -----------------------------------------------------------------------------
NW_LMgr_RootBox_t*
NW_LMgr_AreaBox_GetRootBox (NW_LMgr_Box_t* box)
{
  NW_LMgr_AreaBox_t* thisObj;

  // for convenience 
  thisObj = NW_LMgr_AreaBoxOf (box);
  
  return NW_LMgr_Box_GetRootBox(thisObj->iImageMapBox);
}

// -----------------------------------------------------------------------------
NW_LMgr_ImageMapBox_t* 
NW_LMgr_AreaBox_GetParentImageMapBox (NW_LMgr_Box_t* box)
{
  NW_LMgr_AreaBox_t* thisObj = NW_LMgr_AreaBoxOf (box);

  return thisObj->iImageMapBox;
}

// -----------------------------------------------------------------------------
// Function: NW_LMgr_AreaBox_SetParentImageMapBox
// Description: This method sets this area box's parent image map box
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
void
NW_LMgr_AreaBox_SetParentImageMapBox (NW_LMgr_AreaBox_t* aAreaBox, 
                                      NW_LMgr_ImageMapBox_t* aImageMapBox)
  {
  NW_ASSERT(aAreaBox);
  NW_ASSERT(aImageMapBox);

  aAreaBox->iImageMapBox = aImageMapBox;
  }

// -----------------------------------------------------------------------------
NW_Bool 
NW_LMgr_AreaBox_IsInvalidAreaBox (NW_LMgr_Box_t* aBox)
{
  NW_LMgr_AreaBox_t* thisObj = NW_LMgr_AreaBoxOf (aBox);

  return thisObj->iIsInvalid;
}

// -----------------------------------------------------------------------------
// Function: NW_LMgr_AreaBox_Copy
// Description: Will return a new box which is a copy of the area box
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_AreaBox_Copy(NW_LMgr_AreaBox_t* aBox, NW_LMgr_AreaBox_t** aNewBox)
  {
  NW_LMgr_AreaBox_t* oldAreaBox = aBox;
  NW_LMgr_AreaBox_t* newAreaBox = NULL;
  NW_HED_EventHandler_t* eventHandler = NULL;

  NW_ASSERT( oldAreaBox != NULL );

  NW_TRY( status ) 
    {
    // copy the href and alt text values since each area box has their own copy
    // and are responsible for deletion
    NW_Text_t* hrefVal = NW_Text_Copy(oldAreaBox->iHRefText, NW_TRUE);
    NW_Text_t* altText = NW_Text_Copy(oldAreaBox->iAltText, NW_TRUE);

    // create a new event handler for the new area box
    eventHandler = NW_HED_EventHandler_New(NW_HED_EventHandlerOf(oldAreaBox->eventHandler)->documentNode, NW_HED_EventHandlerOf(oldAreaBox->eventHandler)->context );
    NW_THROW_OOM_ON_NULL( eventHandler, status );

    // create the new area box
    newAreaBox = NW_LMgr_AreaBox_New(6, 
                                  NW_LMgr_EventHandlerOf(eventHandler),
                                  oldAreaBox->iImageMapBox, 
                                  NULL, 
                                  NULL,
                                  altText,
                                  hrefVal);
    NW_THROW_OOM_ON_NULL( newAreaBox, status );

    // because we don't have to parse coordinate and shape text values, we can just copy
    // the shape and bounds values from the old area box
    newAreaBox->iShape = oldAreaBox->iShape;
    NW_GDI_Rectangle_t bounds = NW_LMgr_Box_GetFormatBounds(NW_LMgr_BoxOf(oldAreaBox));
    NW_LMgr_Box_SetFormatBounds(NW_LMgr_BoxOf(newAreaBox), bounds);
    (void) NW_LMgr_Box_UpdateDisplayBounds( NW_LMgr_BoxOf( newAreaBox ), bounds );

    // set the pointer to a pointer to point to our new area box
    *aNewBox = (NW_LMgr_AreaBox_t*)newAreaBox;
    }
  NW_CATCH( status ) 
    {
    if (newAreaBox)
      {
      NW_Object_Delete(newAreaBox);
      }
    *aNewBox = NULL;
    } 
  NW_FINALLY 
    {
    return status;  
    } 
  NW_END_TRY
  }

// -----------------------------------------------------------------------------
// NW_LMgr_AreaBox_InterpretShape
// Description: This method will take the textual shape and convert it to one
//   of the NW_LMGR_AreaBoxShape_Type_t types.
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_AreaBox_InterpretShape(NW_LMgr_AreaBox_t* thisObj)
{
  NW_Ucs2* storage = NULL;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded = 0;

  NW_TRY(status)
  {
    if (thisObj->iShapeText != NULL)
    {
      // convert NW_Text_t shape name to a NW_Ucs2 in order to use Usc2 string methods
      storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (thisObj->iShapeText, 0, &length, &freeNeeded);
      NW_THROW_OOM_ON_NULL( storage, status );
 
      // compare storage string with various string names to determine type
      if ((NW_Str_StricmpConst(storage, "rect") == 0) ||
          (NW_Str_StricmpConst(storage, "rectangle") == 0))
      {
        thisObj->iShape = NW_LMGR_ABS_RECT;
      }
      else if ((NW_Str_StricmpConst(storage, "circ") == 0) ||
               (NW_Str_StricmpConst(storage, "circle") == 0))
      {
        thisObj->iShape = NW_LMGR_ABS_CIRC;
      }
      else if ((NW_Str_StricmpConst(storage, "poly") == 0) ||
               (NW_Str_StricmpConst(storage, "polygon") == 0))
      {
        thisObj->iShape = NW_LMGR_ABS_POLY;
      }
      else
      {
        thisObj->iShape = NW_LMGR_ABS_RECT;
      }
    }
  }
  NW_CATCH (status) 
  {
  } 
  NW_FINALLY 
  {
    if (freeNeeded) 
    {
      NW_Mem_Free ((NW_Ucs2*)storage);
    }

    return status;
  } NW_END_TRY
}

// -----------------------------------------------------------------------------
// NW_LMgr_AreaBox_InterpretCoords
// Description: This method will take the coords associated with the area box and
//   set the height and width dimensions based on a bounding box (i.e. a bounding 
//   box which incompasses the "shape" of the area)
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_LMgr_AreaBox_InterpretCoords(NW_LMgr_AreaBox_t* thisObj)
{
  NW_Ucs2* coordStr = NULL;
  NW_Text_Length_t length;
  NW_Uint8 freeNeeded = NW_FALSE;
  NW_GDI_Rectangle_t bounds;

  NW_TRY(status)
  {
    bounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( thisObj->iImageMapBox ) );
    
    if (thisObj->iCoordsText != NULL)
    {
      // convert NW_Text_t shape name to a NW_Ucs2 in order to use Usc2 string methods
      coordStr = (NW_Ucs2*)NW_Text_GetUCS2Buffer(thisObj->iCoordsText, NW_Text_Flags_Aligned | NW_Text_Flags_NullTerminated, &length, &freeNeeded);
      NW_THROW_OOM_ON_NULL(coordStr, status);

      switch (thisObj->iShape)
      {
      case NW_LMGR_ABS_CIRC:
        {
        NW_Int32 x = 0, y = 0, r = 0;
        NW_Ucs2* points[3];

        // parse the coord values "x,y,r"
        for (NW_Uint16 i = 0; i < 3; i++)
        {
          points[i] = NW_LMgr_AreaBox_ParseString (thisObj, coordStr);
          coordStr = NULL;
        }

        // convert to numeric values
        if (points[0])
        {
          x = NW_Str_Atoi(points[0]);
          if (x < 0)
          {
            x = 0;
          }
          // clean up memory
          NW_Mem_Free (points[0]);
        }
        else
        {
          NW_THROW_STATUS(status, KBrsrFailure);
        }

        if (points[1])
        {
          y = NW_Str_Atoi(points[1]);

          if (y < 0)
          {
            y = 0;
          }
          // clean up memory
          NW_Mem_Free (points[1]);
        }
        else
        {
          NW_THROW_STATUS(status, KBrsrFailure);
        }

        if (points[2])
        {
          r = NW_Str_Atoi(points[2]);
          
          if (r < 0)
          {
            r = 0;
          }
          // clean up memory
          NW_Mem_Free (points[2]);
        }
        else
        {
          NW_THROW_STATUS(status, KBrsrFailure);
        }

        if ((x - r) < 0)
        {
          bounds.point.x = 0;
         
          // the width of the circle if it draws outside the image will be x + r
          bounds.dimension.width = x + r;
        }
        else
        {
          bounds.point.x = x - r;    

          // calculate the width dimension by (r * 2)
          bounds.dimension.width = (r * 2);
        }

        if ((y - r) < 0)
        {
          bounds.point.y = 0;
                  
          // the height of the circle if it draws outside the image will be y + r
          bounds.dimension.height = y + r;
        }
        else
        {
          bounds.point.y = y - r;

          // calculate the height dimension by (r * 2)
          bounds.dimension.height = (r * 2);
        }

        }
        break;

      case NW_LMGR_ABS_POLY:
      case NW_LMGR_ABS_RECT:
      default:
        {
        NW_Ucs2* point = NULL;
        NW_GDI_Metric_t xH = 0, yH = 0;   // variables to hold the high values
        NW_GDI_Metric_t xL = 0, yL = 0;   // variables to hold the low values
        NW_GDI_Metric_t temp = 0;
        NW_Bool xValue = NW_TRUE;
        NW_Bool haveSecondSet = NW_FALSE; // variable used to insure that we have 
                                          // at least 2 sets of coordinates 

        // parse the coord values "x1,y1,x2,y2,x3,y3,..."
        // start with retrieving the first pair of coords
        for (NW_Uint16 i = 0; i < 2; i++)
        {
          if ((point = NW_LMgr_AreaBox_ParseString (thisObj, coordStr)) != NULL)
          {
            if (xValue)
            {
              temp = (NW_GDI_Metric_t) NW_Str_Atoi(point); 
                
              // IE and Opera set negative values to 0
              if (temp < 0)
              {
                temp = 0;
              }

              xH = temp;
              xL = temp;
              xValue = NW_FALSE;
            }
            else
            {
              temp = (NW_GDI_Metric_t) NW_Str_Atoi(point); 
              
              // IE and Opera set negative values to 0
              if (temp < 0)
              {
                temp = 0;
              }

              yH = temp;
              yL = temp;
              xValue = NW_TRUE;
            }
            // clean up memory
            NW_Mem_Free ((NW_Ucs2*)point);
          }
          else
          {
            NW_THROW_STATUS(status, KBrsrFailure); 
          }
          coordStr = NULL;
        }

        // retrieve the remaining coord pairs and compare x and y values to find the
        // heighest and lowest x, y values
        while ((point = NW_LMgr_AreaBox_ParseString (thisObj, coordStr)) != NULL)
        {
          haveSecondSet = NW_TRUE;
          if (xValue)
          {
            temp = (NW_GDI_Metric_t) NW_Str_Atoi(point);

            if (temp < 0)
            {
              temp = 0;
            }

            if (temp > xH)
            {
              xH = temp;
            }
            else if (temp < xL)
            {
              xL = temp;
            }
          
            xValue = NW_FALSE;
          }
          else
          {
            temp = (NW_GDI_Metric_t) NW_Str_Atoi(point);

            if (temp < 0)
            {
              temp = 0;
            }

            if (temp > yH)
            {
              yH = temp;
            }
            else if (temp < yL)
            {
              yL = temp;
            }

            xValue = NW_TRUE;
          }

          // clean up memory
          NW_Mem_Free ((NW_Ucs2*)point);
        }

        // Check to see if we have exited the while loop with a mismatched coordinate
        // (i.e xValue without a yValue). If so, assign the missing yValue to 0.
        if (!xValue)
        {
          temp = 0; 

          if (temp > yH)
          {
            yH = temp;
          }
          else if (temp < yL)
          {
            yL = temp;
          }  
        }

        // if there was only one set of coordinates, throw an error
        if (!haveSecondSet)
        {
          NW_THROW_STATUS(status, KBrsrFailure); 
        }
          
        // largest x, y values are the bound points
        bounds.point.x = xL;
        bounds.point.y = yL; 

        // calculate the width dimension by finding the largest x value and subtracting the smallest x value
        bounds.dimension.width  = xH - xL;
        // calculate the height dimension by finding the largest y value and subtracting the smallest y value
        bounds.dimension.height = yH - yL;
        }
        break;
      } // end switch
      (void) NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( thisObj ), bounds );
      (void) NW_LMgr_Box_UpdateDisplayBounds( NW_LMgr_BoxOf( thisObj ), bounds );
    }
    else
    {
      NW_THROW_STATUS(status, KBrsrFailure); 
    }
  }
  NW_CATCH (status) 
  {
    if (freeNeeded) 
    {
      NW_Mem_Free ((NW_Ucs2*)coordStr);
    }
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}

// -----------------------------------------------------------------------------
// NW_LMgr_AreaBox_ParseString
// Description: This method parses a string looking for numbers(series of digits). 
//   This method returns a pointer to the number substring. Subsequent calls with 
//   'string' set to NULL return the next number series in the string. If there
//   is no number found, the method will return NULL.
// -----------------------------------------------------------------------------
//
NW_Ucs2*
NW_LMgr_AreaBox_ParseString(NW_LMgr_AreaBox_t* box, NW_Ucs2* string)
{
  NW_Ucs2* pStart = NULL;
  NW_Ucs2* pEnd   = NULL;
  NW_Ucs2* returnString = NULL;
  NW_Uint32 strlen = 0;
  NW_Uint32 sublen = 0;
  NW_Bool firstTimeThrough = NW_FALSE;

  if (string != NULL)
  {
    box->iString = string;
    firstTimeThrough = NW_TRUE;
  }

  if (box->iString != NULL)
  {
    // We are trying to find number series within a string
    // Positive and negative numbers are acceptable

    // Loop until we either find a digit or a '-', this is the start of the string
    NW_Ucs2* tmpString = box->iString;

    while (*tmpString != WAE_ASCII_NULL)
    {
      // Check to see if we have a '-' character. This character must be followed
      // by a digit
      if (*tmpString == '-') 
      {
        if (NW_Str_Isdigit(*(tmpString + 1)))
        {
          pStart = tmpString;
          pEnd = tmpString;
          tmpString++;

          break;
        }
      }
      else if (NW_Str_Isdigit(*tmpString))
      {
        pStart = tmpString;
        pEnd = tmpString;
        tmpString++;

        break;
      }
      tmpString++;
    }

    // Loop until we find the end of the number
    if (pStart != NULL)
    {
      while (*tmpString != WAE_ASCII_NULL)
      {
        if (NW_Str_Isdigit(*tmpString))
        {
          pEnd = tmpString;
          tmpString++;
        }     
        else
        {
          break;
        }
      }
      /*lint -e{794} Conceivable use of null pointer*/
      pEnd++;
    }

    if (pEnd != NULL) 
    {
      sublen = (NW_Uint32)(pEnd - pStart);

      returnString = NW_Str_Substr(pStart, 0, sublen);
 
      strlen = NW_Str_Strlen(pStart);

      tmpString = NW_Str_Substr(pStart, sublen, strlen - sublen);
      
      if (!firstTimeThrough)
      {
        NW_Mem_Free((NW_Ucs2*)box->iString);
      }

      box->iString = tmpString;
    }
    else
    {
      if (!firstTimeThrough)
      {
        NW_Mem_Free((NW_Ucs2*)box->iString);
      }

      box->iString = NULL;
    }
  }
  return returnString;
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_LMgr_AreaBox_DisplayAltText(NW_LMgr_AreaBox_t* aAreaBox, 
                               CGDIDeviceContext* aDeviceContext) 
  {                               
  NW_TRY(status)
    {
    CGDIFont* font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(aAreaBox->iImageMapBox));
    NW_GDI_Metric_t space = aAreaBox->iComboBounds.dimension.width;
    NW_GDI_Point2D_t textPos;

    textPos.x = aAreaBox->iComboBounds.point.x;
    textPos.y = aAreaBox->iComboBounds.point.y;
     
    status = NW_LMgr_ImgContainerBox_DisplayAltText(NW_LMgr_BoxOf(aAreaBox),
                                                    aAreaBox->iAltText,
                                                    aDeviceContext,
                                                    space,
                                                    &textPos,
                                                    font);
    NW_THROW_ON_ERROR(status);
    }
  NW_CATCH (status)
    {
    }
  NW_FINALLY
    {
    return status;
    }
  NW_END_TRY
  }
