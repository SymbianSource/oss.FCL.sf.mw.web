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
* Description:  Handles scrolling operations of the view
*
*/

 
// INCLUDE FILES
#include <e32def.h>  // To avoid NULL redefine warning (no #ifndef NULL)
#include <nwx_settings.h>

#include "MVCScroll.h"

#include "MVCView.h"
#include "nw_evt_tabevent.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_gdi_utils.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_lmgr_simplepropertylist.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_logger.h"
#include "GDIDeviceContext.h"
#include "nw_lmgr_areabox.h"
#include "nw_evt_scrollevent.h"

// CONSTANTS
#define SMALL_SCROLL 16
#define NORMAL_SCROLL 18
#define LARGE_SCROLL 21

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMVCScroll::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMVCScroll* 
CMVCScroll::NewL(CView* view)
{
	CMVCScroll* scroll = new (ELeave) CMVCScroll(view);

	CleanupStack::PushL(scroll);
	CleanupStack::Pop(); // scroll

	return scroll;
}

// -----------------------------------------------------------------------------
// CMVCScroll::CMVCScroll
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CMVCScroll::CMVCScroll(CView* view) : iView(view)
{
}

// -----------------------------------------------------------------------------
// CMVCScroll::~CMVCScroll
// Destructor.
// -----------------------------------------------------------------------------
//
CMVCScroll::~CMVCScroll()
{
}

// -----------------------------------------------------------------------------
// IsBoxVisibleInEither
// Description: Helper function to test if box is visible in either rectangles             
// Returns: TBool : NW_TRUE if the box is visible within the two bounds
//                  NW_FALSE if the box is NOT visible within the bounds
// -----------------------------------------------------------------------------
//
TBool
CMVCScroll::IsBoxVisibleInEither (const NW_LMgr_Box_t* box,
                                  NW_GDI_Rectangle_t* bounds1,
                                  NW_GDI_Rectangle_t* bounds2)
{
  NW_ASSERT(iView != NULL);
  NW_ASSERT(box != NULL);
  NW_ASSERT(bounds1 != NULL);
  NW_ASSERT(bounds2 != NULL);

  if ( iView->IsBoxVisible(box, bounds1) ||
       iView->IsBoxVisible(box, bounds2) ) 
  {
    return NW_TRUE;
  }
  else 
  {
    return NW_FALSE;
  }
}

// -----------------------------------------------------------------------------
// This only get range of the box, not considering sibling or master box's range
//
void 
CMVCScroll::GetBoxRangeRestrict (NW_LMgr_Box_t* box, 
                                 NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                                 NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 )
{
  NW_ASSERT(box != NULL);

  if (NW_Object_IsClass(box, &NW_LMgr_AreaBox_Class)) 
  {
    NW_GDI_Rectangle_t areaBoxBounds = NW_LMgr_AreaBox_GetBounds(box);
    
    *referenceY = areaBoxBounds.point.y;
    *refY2 = (NW_GDI_Metric_t)(areaBoxBounds.point.y + areaBoxBounds.dimension.height);
    *referenceX = areaBoxBounds.point.x;
    *refX2 = (NW_GDI_Metric_t)(areaBoxBounds.point.x + areaBoxBounds.dimension.width);
  }
  else
  {
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( box );   

    *referenceY = boxBounds.point.y;
    *refY2 = (NW_GDI_Metric_t)(boxBounds.point.y + boxBounds.dimension.height);
    *referenceX = boxBounds.point.x;
    *refX2 = (NW_GDI_Metric_t)(boxBounds.point.x + boxBounds.dimension.width);
  }

  return;
}

// -----------------------------------------------------------------------------
// In case of non-split, simply box's range will be returned
//
void 
CMVCScroll::GetMasterBoxRange (NW_LMgr_Box_t* box, 
                               NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                               NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 )
{
  NW_LMgr_Box_t *masterBox = NULL;
  NW_LMgr_PropertyList_t* propList;

  NW_ASSERT(box != NULL);

  propList = NW_LMgr_Box_PropListGet(box);
  if (NW_Object_IsInstanceOf(propList, &NW_LMgr_SlavePropertyList_Class))
  {
    masterBox = 
      NW_LMgr_SlavePropertyList_GetMasterBox(NW_LMgr_SlavePropertyListOf(propList));
  }
  else
  {
    masterBox = box;
  }

  NW_ASSERT(masterBox != NULL);

  GetBoxRangeRestrict( masterBox, referenceY, refY2, referenceX, refX2 );

  return;
}

// -----------------------------------------------------------------------------
// This method doesn't return the first sibling box with width and height as 0;
// instead, the sibling whose width and height are not 0 and is right next to the 
// first sibling box or the master box if no other sibling exists will be returned.
// In case of non-split, simply the box will be returned.
// Does not return NULL.
//
NW_LMgr_Box_t*
CMVCScroll::GetFirstSiblingBox (NW_LMgr_Box_t* box)
{
  NW_LMgr_Box_t *tempBox = NULL;
  NW_LMgr_Box_t *firstBox = NULL;
  NW_LMgr_PropertyList_t* propList;
  NW_LMgr_Property_t prop;

  NW_ASSERT(box != NULL);

  propList = NW_LMgr_Box_PropListGet(box);
  /* ensure box is a master box */
  if (NW_Object_IsInstanceOf(propList, &NW_LMgr_SlavePropertyList_Class))
  {
    box = 
      NW_LMgr_SlavePropertyList_GetMasterBox(NW_LMgr_SlavePropertyListOf(propList));
    propList = NW_LMgr_Box_PropListGet(box);
  }

  /* Get first box */
  do
  {
    prop.value.object = NULL;
    if( propList )
    {
      (void)NW_LMgr_PropertyList_Get(propList, NW_CSS_Prop_sibling, &prop);
      /* If we come back in a loop to the master box, then stop */
      if (prop.value.object == box)
      {
        /* This occurs when only one slave box exist and its width and height is 0 */
        if (firstBox == NULL)
        {
          firstBox = box;
        }
        break;
      }

      if(prop.value.object != NULL)
      {
        tempBox = NW_LMgr_BoxOf(prop.value.object);
        propList = NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(tempBox));
        NW_ASSERT (NW_Object_IsInstanceOf (propList, &NW_LMgr_PropertyList_Class));
        /* ignore the sibling box with width and height as 0 */
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( tempBox );   

        if( boxBounds.dimension.height != 0 && boxBounds.dimension.width != 0 )
        {
          firstBox = tempBox;
        }
      } else
      /* If it's null, it means non-split case */
      {
        firstBox = box;  // break out because prop.value.object == NULL
      }
    }
  }while(prop.value.object != NULL);

  if(firstBox == NULL)
  {
    firstBox = box;
  }

  return firstBox;
}


// -----------------------------------------------------------------------------
// In case of non-split, simply box's range will be returned;
//
void 
CMVCScroll::GetFirstSiblingBoxRange (NW_LMgr_Box_t* box, 
                                     NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                                     NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 )
{
  NW_LMgr_Box_t *firstBox = NULL;

  NW_ASSERT(box != NULL);

  firstBox = GetFirstSiblingBox(box);
  GetBoxRangeRestrict( firstBox, referenceY, refY2, referenceX, refX2 );

  return;
}

// -----------------------------------------------------------------------------
NW_Bool
CMVCScroll::HowBigIsBoxVisible ( const NW_LMgr_Box_t* box,
                                 NW_GDI_Rectangle_t* bounds,
                                 NW_GDI_Rectangle_t* overlap )
{
  NW_GDI_Rectangle_t boxBounds;

  /* parameter assertion block */
  NW_ASSERT (iView != NULL);
  NW_ASSERT (box != NULL);
  
  if (NW_Object_IsClass(box, &NW_LMgr_AreaBox_Class)) 
  {
    boxBounds = NW_LMgr_AreaBox_GetBounds( NW_LMgr_BoxOf( box ) );
  }
  else
  {
    boxBounds = NW_LMgr_Box_GetDisplayBounds( NW_LMgr_BoxOf( box ) );   
  }

  /* below check handles the case where the box is at least has sHVisibleAmt and sVVisibleAmt 
   * inside the display bounds
   */
  if (NW_GDI_Rectangle_Cross(&boxBounds, bounds, overlap)) 
  {
    if ((overlap->dimension.width >= iView->GetVisibleAmt().width) &&
        (overlap->dimension.height >= iView->GetVisibleAmt().height)) 
    {
      return NW_TRUE;
    }
    // If the box is smaller than visibleAmt, then we should go to that link
    else if ((overlap->dimension.width == boxBounds.dimension.width) &&
      (overlap->dimension.height == boxBounds.dimension.height))
    {
      return NW_TRUE;
    }
  }

  return NW_FALSE;
}

// -----------------------------------------------------------------------------
NW_Bool
CMVCScroll::GetScrollDownBounds ( CView* view,
                                  NW_GDI_Rectangle_t* scrolledBounds )
{
  const NW_GDI_Dimension2D_t* displaySize;
  NW_GDI_Dimension2D_t canvasSize;
  NW_Bool scroll;

  // get the display and cavas size
  displaySize =
    &(view->GetDeviceContext()->DisplayBounds()->dimension);
  TBrowserStatusCode status = NW_LMgr_RootBox_GetExtents (view->GetRootBox(), &canvasSize);
  NW_ASSERT (status == KBrsrSuccess);

  // can scroll 
  if (scrolledBounds->point.y + view->GetScrollAmt().height + displaySize->height 
      <= canvasSize.height) 
  {
    scrolledBounds->point.y = 
        (NW_GDI_Metric_t)(scrolledBounds->point.y + view->GetScrollAmt().height);
    scroll = NW_TRUE;
  } 
  else if (scrolledBounds->point.y + displaySize->height < canvasSize.height)
  {
    scrolledBounds->point.y = 
        (NW_GDI_Metric_t)(canvasSize.height - displaySize->height);
    scroll = NW_TRUE;
  } 
  else
  {
    scroll = NW_FALSE;
  }

  return scroll;
}

// -------------------------------------------------------------------------
NW_Bool
CMVCScroll::GetScrollUpBounds ( CView* view,
                                NW_GDI_Rectangle_t* scrolledBounds )
{
  NW_Bool scroll;

  // can scroll
  if (scrolledBounds->point.y - view->GetScrollAmt().height >= 0) 
  {
    scrolledBounds->point.y = 
        (NW_GDI_Metric_t)(scrolledBounds->point.y - view->GetScrollAmt().height);
    scroll = NW_TRUE;
  } 
  else if (scrolledBounds->point.y > 0) 
  {
    scrolledBounds->point.y = 0;
    scroll = NW_TRUE;
  } 
  else
  {
    scroll = NW_FALSE;
  }

  return scroll;
}

// -------------------------------------------------------------------------
NW_LMgr_Box_t*
CMVCScroll::GetTabIndex( const NW_LMgr_Box_t* oldBox, 
                         NW_Bool nextIndex, 
                         NW_Int32* currentTabIndex )
{
  NW_Bool found = NW_FALSE;
  NW_LMgr_Box_t* box = NULL;
  NW_ADT_Vector_t* tabListVector = NW_ADT_VectorOf( iView->GetTabList() );
  NW_Uint16 size = NW_ADT_Vector_GetSize( tabListVector );
  NW_ADT_Vector_Metric_t i;
  
  // if oldBox is NULL then return the first item in the list.
  if( oldBox == NULL )
  {
    i = 0;
    found = NW_TRUE;
  }
  else
  {
    // if the currentTabIndex is provided then get the next item in the list
    if( *currentTabIndex != -1 )
    {
      i = (NW_ADT_Vector_Metric_t)*currentTabIndex;
      // skip to the previous/next box
      i = (NW_ADT_Vector_Metric_t) (( nextIndex == NW_TRUE ) ? ( i + 1 ) : ( i - 1 ));
      found = NW_TRUE;
    }
    else
    {
      for( i = 0; i < size; i++ ) 
      {
        box = (NW_LMgr_Box_t*)*NW_ADT_Vector_ElementAt( tabListVector, i );

        if( oldBox == box ) 
        {
          // skip to the previous/next box
          i = (NW_ADT_Vector_Metric_t) (( nextIndex == NW_TRUE ) ? ( i + 1 ) : ( i - 1 ));
          found = NW_TRUE;
          break;
        }
      }
    }
  }
  // if the box was found and index is valid
  if( found == NW_TRUE && i < size )
  {
    box = (NW_LMgr_Box_t*)*
      NW_ADT_Vector_ElementAt( tabListVector, i );
    *currentTabIndex = i;
  }
  else
  {
    box = NULL;
    *currentTabIndex = -1;
  }
  return box;
}

// -------------------------------------------------------------------------
NW_LMgr_Box_t*
CMVCScroll::GetNextEventListener (NW_LMgr_BoxVisitor_t* boxVisitor)
{
    NW_LMgr_Box_t* box = NULL;
    NW_GDI_Rectangle_t boxBounds;

    while ((box = NW_LMgr_BoxVisitor_NextBox (boxVisitor, NULL)) != NULL) {
      NW_LMgr_IEventListener_t* eventListener;

      /* we are only looking for boxes implementing the NW_LMgr_IEventListener
         interface */
      eventListener = (NW_LMgr_IEventListener_t*)NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class);
      if (eventListener == NULL) {
        continue;
      }

      /* If activeContainerBox has zero length or zero width, we skip this
         activeContainerBox. This happens when activeContaninerBox only has
         one breakBox child */
      if (NW_Object_IsInstanceOf (box, &NW_LMgr_ActiveContainerBox_Class)) {
          boxBounds = NW_LMgr_Box_GetDisplayBounds(box);
          if (boxBounds.dimension.width == 0 || boxBounds.dimension.height == 0) {
              continue;
          }
      }
      return (box);
    }
    return (NULL);
}

// ------------------------------------------------------------------------- 
void
CMVCScroll::GetFirstVisible(NW_LMgr_Box_t* box, 
                         NW_LMgr_Box_t** firstVisible )
{
  NW_LMgr_PropertyList_t* propList;
  NW_LMgr_PropertyList_t* masterPropList;
  NW_LMgr_Box_t* masterBox = NULL;
  NW_LMgr_Box_t* tempBox = NULL;
  NW_LMgr_Property_t prop;
  NW_Bool found = NW_FALSE;
  NW_GDI_Rectangle_t viewBounds;

  NW_ASSERT( box != NULL );
  NW_ASSERT( iView != NULL );
  
  viewBounds = *(iView->GetDeviceContext()->DisplayBounds());
  viewBounds.point = *(iView->GetDeviceContext()->Origin());

  // let's check first if the box is visible
  if( iView->IsBoxVisible( box, &viewBounds ) )
  {
    // we've got the box
    tempBox = box;
    found = NW_TRUE;
  }
  else
  {
    propList = NW_LMgr_Box_PropListGet( box );
    if( propList != NULL )
    {
      // first check if we are on a master or a slave box
      if( NW_Object_IsInstanceOf( propList, &NW_LMgr_SlavePropertyList_Class ) )
      {
        // this is a slave
        // get the master's property list
        masterBox = NW_LMgr_SlavePropertyList_GetMasterBox( propList );
      }
      else
      {
        masterBox = box;
      }
      // we need the master property list to check if we've got a chain here
      masterPropList = NW_LMgr_Box_PropListGet( masterBox );
      propList = NULL;
      if( NW_LMgr_PropertyList_Get( masterPropList, NW_CSS_Prop_sibling, &prop ) == KBrsrSuccess )
      {
        // get the next to the master box
        if( prop.value.object != NULL )
        {
          tempBox = NW_LMgr_BoxOf( prop.value.object );
          if( tempBox != NULL )
          {
            // this must be a slave property list
            propList = NW_LMgr_Box_PropListGet( NW_LMgr_BoxOf( tempBox ) );
          }
        }
      }
      else
      {
        // this is not a chain
      }
      // let's go through the chain
      if( propList )
      {
        // at this point tempbox holds the box next to the master
        do
        {
          // check if this box is visible
          if( iView->IsBoxVisible( tempBox, &viewBounds ) )
          {
            // we've got the box
            found = NW_TRUE;
            break;
          }
          // get the next box
          tempBox = NW_LMgr_SlavePropertyList_GetSiblingBox( propList );
          propList = NW_LMgr_Box_PropListGet( tempBox );
        } 
        // NW_LMgr_SimplePropertyList_Class means that we are on the master box again. 
        while( !NW_Object_IsClass ( propList, &NW_LMgr_SimplePropertyList_Class ) );
      }
      // since the "while" leaves out the masterbox, check it out here
      if( found == NW_FALSE && tempBox )
      {
        // check if this box is visible
        if( iView->IsBoxVisible( tempBox, &viewBounds ) )
        {
          // we've got the box
          found = NW_TRUE;
        }
      }
    }
  }
  *firstVisible = ( found == NW_TRUE ? tempBox : NULL );
}

// -----------------------------------------------------------------------------
void 
CMVCScroll::GetBoxRange(NW_LMgr_Box_t* box, NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                     NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 )
{
  NW_LMgr_Box_t *masterBox = NULL, *firstBox = NULL;
  NW_LMgr_PropertyList_t* propList;
  NW_LMgr_Property_t prop;

  NW_ASSERT(box != NULL);

  propList = NW_LMgr_Box_PropListGet(box);
  // We are inserting only the master boxes in the tree, so the box
  // has to be master box
  masterBox = box;

  // Get first box 
  do
  {
    prop.value.object = NULL;
    if( propList )
    {
      (void)NW_LMgr_PropertyList_Get(propList, NW_CSS_Prop_sibling, &prop);
      // If we come back in a loop to the master box, then stop 
      if (prop.value.object == box)
      {
        break;
      }

      if(prop.value.object != NULL)
      {
        firstBox = NW_LMgr_BoxOf(prop.value.object);
        propList = NW_LMgr_Box_PropListGet(NW_LMgr_BoxOf(firstBox));
        NW_ASSERT (NW_Object_IsInstanceOf (propList, &NW_LMgr_PropertyList_Class));
      }
    }
  } while(prop.value.object != NULL);

  if(firstBox == NULL)
  {
    firstBox = box;
  }
  
  /* ignore the sibling box with width and height as 0 */
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( firstBox );   

  if( boxBounds.dimension.height==0 && boxBounds.dimension.width==0 )
  {
    firstBox = masterBox;
  }

  if (NW_Object_IsClass(firstBox, &NW_LMgr_AreaBox_Class) != NW_FALSE) 
  {
    NW_GDI_Rectangle_t areaBoxBounds = NW_LMgr_AreaBox_GetBounds(firstBox);

    *referenceY = areaBoxBounds.point.y;
    *referenceX = areaBoxBounds.point.x;
  }
  else
  {
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds(firstBox);
    *referenceY = boxBounds.point.y;
    *referenceX = boxBounds.point.x;
  }

  if (NW_Object_IsClass(masterBox, &NW_LMgr_AreaBox_Class) != NW_FALSE) 
  {
    NW_GDI_Rectangle_t areaBoxBounds = NW_LMgr_AreaBox_GetBounds( masterBox );

    *refY2 = (NW_GDI_Metric_t)(areaBoxBounds.point.y + areaBoxBounds.dimension.height);
    *refX2 = (NW_GDI_Metric_t)(areaBoxBounds.point.x + areaBoxBounds.dimension.width);
  }
  else
  {
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds(masterBox);
    *refY2 = (NW_GDI_Metric_t)(boxBounds.point.y + boxBounds.dimension.height);
    *refX2 = (NW_GDI_Metric_t)(boxBounds.point.x + boxBounds.dimension.width);
  }

  return;
}

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::TabForward(TBool isNewPage,
                      TBool* noBoxVisible)
{
  NW_Evt_TabEvent_t tabEvent;
  TBrowserStatusCode status;
  NW_LMgr_Box_t* selectionCandidateBox = NULL;
  NW_LMgr_Box_t* tempBox = NULL;
  NW_GDI_Metric_t projectionX1 = 0;
  NW_GDI_Metric_t projectionX2 = 0;
  NW_GDI_Metric_t projectionS = 0;
  NW_GDI_Metric_t projectionT = 0;
  NW_GDI_Metric_t currentTopY = 0, currentBottomY = 0;
  NW_GDI_Metric_t currentLeftX = 0, currentRightX = 0;
  NW_GDI_Metric_t selectionCandidateTopY = 0, selectionCandidateBottomY = 0;
  NW_GDI_Metric_t selectionCandidateLeftX = 0, selectionCandidateRightX = 0;
  NW_GDI_Metric_t tempTopY = 0, tempBottomY = 0;
  NW_GDI_Metric_t tempLeftX = 0, tempRightX = 0;
  NW_LMgr_EventHandler_t *eventHandler = NULL;
  NW_GDI_Rectangle_t viewBounds;
  NW_GDI_Rectangle_t scrolledBounds;
  NW_GDI_Rectangle_t overlapS, overlapT;
  NW_Bool update = NW_FALSE;
  NW_Bool scrollCanHappen = NW_FALSE;
  NW_Bool isPageRTL = NW_FALSE;
  NW_Bool smallScreenOn;
  NW_Int32 currentIndex = -1;
  NW_GDI_Rectangle_t overlap;
  NW_GDI_Rectangle_t boxBounds;

  // parameter assertion block 
  NW_ASSERT (iView != NULL);
  
  status = NW_Evt_TabEvent_Initialize (&tabEvent, NW_Evt_TabEvent_Direction_Down);

  if (status != KBrsrSuccess)
  {
    return NW_FALSE;
  }

  smallScreenOn = NW_LMgr_RootBox_GetSmallScreenOn( iView->GetRootBox() );
  isPageRTL = NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBoxOf(iView->GetRootBox()));

  if (isPageRTL) 
  {
    projectionX1 = projectionX2 = RIGHT_ORIGIN + iView->GetRootBox()->extents.dimension.width;
    currentLeftX = currentRightX = RIGHT_ORIGIN + iView->GetRootBox()->extents.dimension.width;
  }
  
  viewBounds = *(iView->GetDeviceContext()->DisplayBounds());
  viewBounds.point = *(iView->GetDeviceContext()->Origin());

  // master box is the lowest box in case of split; we need it in case of tab down
  if (iView->GetCurrentBox() != NULL)
  {
    GetMasterBoxRange(iView->GetCurrentBox(), &currentTopY, &currentBottomY,
      &currentLeftX, &currentRightX );
  }
  
  // Special handling for anchors 
  selectionCandidateBox = iView->GetCurrentBox();
  
  if ((selectionCandidateBox != NULL) &&
       NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_ActiveContainerBox_Class)) 
  {
    eventHandler = NW_LMgr_ActiveContainerBoxOf(selectionCandidateBox)->eventHandler;
  }

  do 
  {
    selectionCandidateBox = GetNextTabIndex(selectionCandidateBox, &currentIndex);
    if (eventHandler != NULL) 
    {
      // for active container box, it has to be visible to be selection candidate 
      if ((selectionCandidateBox == NULL) ||
          !NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_ActiveContainerBox_Class) ||
          NW_LMgr_ActiveContainerBoxOf(selectionCandidateBox)->eventHandler != eventHandler)
      {
        eventHandler = NULL;
      }
    }      
  } while (eventHandler != NULL);

  // for SSL, always go to the next tab box 
  // for non-SSL, we should start from the selection box on the next line 
  if (selectionCandidateBox != NULL && !smallScreenOn )
  {
    while(selectionCandidateBox != NULL)
    {
      GetFirstSiblingBoxRange(selectionCandidateBox,
                          &selectionCandidateTopY, &selectionCandidateBottomY, 
                          &selectionCandidateLeftX, &selectionCandidateRightX);
 
      NW_LMgr_Box_t* tempSiblingBox = GetFirstSiblingBox (selectionCandidateBox);
      boxBounds = NW_LMgr_Box_GetDisplayBounds( tempSiblingBox );  


      // area boxes can over lap, so if the selection top Y is bigger than the current top Y,
      // this will be our candidate selection box
      if (NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_AreaBox_Class))
        {
        if ((selectionCandidateTopY > currentTopY) &&
            (NW_GDI_Rectangle_Cross(&boxBounds, &viewBounds, &overlap)))
          {
          break;
          }
        }
      else
        {
        // get the first box that has a lower "upper corner" than the current "lower corner" is
        // or there's some overlap on the Y axis 
        // but left edge of the box is overlapping or to the left of the left edge of the current box
        if( ( NW_GDI_Rectangle_Cross(&boxBounds, &viewBounds, &overlap) ) &&
            ( selectionCandidateTopY >= currentBottomY ||
              (!isPageRTL && selectionCandidateTopY < currentBottomY && selectionCandidateLeftX <= currentLeftX)  ||
              (isPageRTL && selectionCandidateTopY < currentBottomY && selectionCandidateRightX >= currentRightX)) )

          {
          break;
          }
        }
      selectionCandidateBox = GetNextTabIndex(selectionCandidateBox, &currentIndex);
    }
  }

  // for non-SSL, we should find the selection box having visible projection with
  // the current box 
  if (selectionCandidateBox != NULL && !smallScreenOn )
  {
    NW_LMgr_Box_t* siblingBox = GetFirstSiblingBox( selectionCandidateBox ); 
    if( HowBigIsBoxVisible( siblingBox, 
                            &viewBounds, &overlapS) )
    {
      projectionX1 = overlapS.point.x > currentLeftX ? overlapS.point.x : currentLeftX; // max
      projectionX2 = overlapS.point.x + overlapS.dimension.width < currentRightX ? 
                     overlapS.point.x + overlapS.dimension.width : currentRightX; // min
      projectionS = projectionX2 - projectionX1;
    }

    NW_LMgr_Box_t* masterBox = selectionCandidateBox;

    // check if we've got more of them in the line immediate below and if so pick the left most 
    // one with projection
    tempBox = selectionCandidateBox;
    while( tempBox )
    {
    tempBox = GetNextTabIndex(masterBox, &currentIndex);
      if( tempBox )
      {
        NW_LMgr_Box_t* siblingBox = GetFirstSiblingBox( tempBox ); 
        if( siblingBox != tempBox )
          {
          // 
          masterBox = tempBox;
          tempBox = siblingBox;          
          }
        else
          {
          masterBox = tempBox;
          }
        //
        GetFirstSiblingBoxRange( tempBox, &tempTopY, &tempBottomY,
                             &tempLeftX, &tempRightX );
        /* check if this box is in the same line as the selected one or
         */
        if( tempTopY >= selectionCandidateBottomY || 
            tempBottomY <= selectionCandidateTopY )
        {
          break;  // out of the line. 
        } 
        else 
        {
          /* get left most box which has projection to the current box */
          /* 
            selectBox_Visible   tempBox_Visible   projectionT>0   projectionS>0   update
            --------------------------------------------------------------------------
            No                  Yes               N/A             N/A             TRUE
            --------------------------------------------------------------------------
            No                  No                N/A             N/A             FALSE
            --------------------------------------------------------------------------
            Yes                 No                N/A             N/A             FALSE
            --------------------------------------------------------------------------
            Yes                 Yes               Yes             Yes             FALSE
                                                  Yes             No              TRUE
                                                  No              Yes             FALSE
                                                  No              No              FALSE
          */
          NW_LMgr_Box_t* selectionCandidateSiblingBox = GetFirstSiblingBox( selectionCandidateBox );
          if( !HowBigIsBoxVisible( selectionCandidateSiblingBox, 
                                   &viewBounds, &overlapS ) )
            {
            if( HowBigIsBoxVisible( masterBox, &viewBounds, &overlapT ) )
              {
              HowBigIsBoxVisible( masterBox, &viewBounds, &overlapT );
              projectionX1 = overlapT.point.x > currentLeftX ? overlapT.point.x : currentLeftX; // max
              projectionX2 = overlapT.point.x + overlapT.dimension.width < currentRightX ? 
                             overlapT.point.x + overlapT.dimension.width : currentRightX; // min
              projectionT = projectionX2 - projectionX1;
              update = NW_TRUE;
              }
            } 
          else 
            {
            if( HowBigIsBoxVisible( tempBox, &viewBounds, &overlapT ) )
              {
              projectionX1 = overlapT.point.x > currentLeftX ? overlapT.point.x : currentLeftX; // max
              projectionX2 = overlapT.point.x + overlapT.dimension.width < currentRightX ? 
                             overlapT.point.x + overlapT.dimension.width : currentRightX; // min
              projectionT = projectionX2 - projectionX1;
              
              // area boxes can over lap, so if the temp projection is bigger, set update to true
              if (NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_AreaBox_Class) && 
                  NW_Object_IsClass(tempBox, &NW_LMgr_AreaBox_Class))
                {
                if ( projectionT > projectionS )
                  {     
                  update = NW_TRUE;
                  }
                }
              else
                {
                if ( projectionT > 0 && projectionT > projectionS )
                  {                 
                  update = NW_TRUE;
                  }
                }
              }
            }

          if (update == NW_TRUE)
            {
            selectionCandidateBox = masterBox;
            selectionCandidateTopY = tempTopY;
            selectionCandidateBottomY = tempBottomY;
            projectionS = projectionT;

            update = NW_FALSE;
            }        
          }
        }
      } // end of while()
    }

    if( projectionS == 0 )
    {
    // scroll instead
    selectionCandidateBox = NULL;
    }

  // never pass invisible selection candidate to setcurrentbox();
  // let scroll happen only when scrollCanHappen or neither selection nor current box visible;
  // scroll mean return NW_TRUE; others return NW_FALSE
  /* 
     selBox != NULL &
     selBoxVis   selBoxVis   curBoxVis   scrlCanHappen   action
     InView      InScrl      InScrl 
     -----------------------------------------------------------------------------------------
     Yes         N/A         N/A                         no scroll; invoke SetCurrentBox()
     -----------------------------------------------------------------------------------------
     No          Yes&&
                 !isNewPage  N/A                         scroll; currentBox = selectionBox
                 Yes&&
                 isNewPage   N/A                         no scroll; do nothing
     -----------------------------------------------------------------------------------------
     No          No          N/A          No||isNewPage  invoke GetVisibleBox(),SetCurrentBox()
     -----------------------------------------------------------------------------------------
                             Null or No   Yes            scroll; noBoxVisible = true
     -----------------------------------------------------------------------------------------
                             Yes          Yes            scroll


     selBox == NULL &
     curBoxVis    scroCanHappen   action
     InScrl      
     -----------------------------------------------------------------------------------------
     N/A          No||isNewPage   invoke GetVisibleBox(),SetCurrentBox()
     -----------------------------------------------------------------------------------------
     Null or No   Yes             scroll; noBoxVisible = true
     -----------------------------------------------------------------------------------------
     Yes          Yes             scroll
   */
  scrolledBounds = viewBounds;
  scrollCanHappen = GetScrollDownBounds( iView, &scrolledBounds );
  if( selectionCandidateBox ) 
  {
    /* selection candidate will be forced to be new current box in SetCurrentBox() */
    if(iView->IsBoxVisible(selectionCandidateBox, &viewBounds))
    {
      if(KBrsrSuccess == SetCurrentBox(selectionCandidateBox, &tabEvent, 
        isNewPage, noBoxVisible)) 
      {
		    return NW_TRUE;
      }
    } 
    else if (iView->IsBoxVisible(selectionCandidateBox, &scrolledBounds))
    {
      if( !isNewPage )
      {
        if (iView->GetCurrentBox() != NULL)
        {
          NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
        }
        if (selectionCandidateBox != NULL)
        {
          NW_LMgr_Box_SetHasFocus (selectionCandidateBox, NW_TRUE);
        }

        iView->SetCurrentBox (selectionCandidateBox);
        
        return NW_FALSE;
      } 
      else
      {
        return NW_TRUE;
      }
    } 
    else 
    {
      if (!scrollCanHappen || isNewPage) 
      {
        if (isNewPage)
        {
          selectionCandidateBox = iView->GetVisibleBox();
          SetCurrentBox( selectionCandidateBox, &tabEvent, 
                         isNewPage, noBoxVisible );  
        } 
        else
        {
          selectionCandidateBox = iView->GetLastVisibleBox();
          NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( selectionCandidateBox );   

          if( selectionCandidateBox != NULL &&
              boxBounds.point.y >= currentBottomY )
          {
            SetCurrentBox( selectionCandidateBox, &tabEvent, isNewPage, noBoxVisible );  
          } // else, keep current box as focused one
        }
        return NW_TRUE;
      } 
      else
      {
        if (iView->GetCurrentBox() == NULL ||
            !iView->IsBoxVisible( iView->GetCurrentBox(), &scrolledBounds))
        {
          *noBoxVisible = NW_TRUE;
        }  
        return NW_FALSE;
      }
    }
  } 
  else if (!scrollCanHappen || isNewPage) // end of if( selectionCandidateBox )
  {
    if (isNewPage)
    {
      selectionCandidateBox = iView->GetVisibleBox();

      if (!selectionCandidateBox)
        {
        currentIndex = -1;
        selectionCandidateBox = GetNextTabIndex(selectionCandidateBox, &currentIndex);
        }

      SetCurrentBox( selectionCandidateBox, &tabEvent, 
                     isNewPage, noBoxVisible );
    } 
    else
    {
      selectionCandidateBox = iView->GetLastVisibleBox();
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( selectionCandidateBox );
      if( selectionCandidateBox != NULL &&
          boxBounds.point.y >= currentBottomY )
      {
        SetCurrentBox( selectionCandidateBox, &tabEvent, 
                       isNewPage, noBoxVisible );  
      } // else, keep current box as focused one
    }
    return NW_TRUE;
  } 
  else
  {
    if (iView->GetCurrentBox() == NULL ||
        !iView->IsBoxVisible( iView->GetCurrentBox(), &scrolledBounds))
    {
      *noBoxVisible = NW_TRUE;
    }
    return NW_FALSE;
  }
    
  return NW_FALSE;
}

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::TabBackward(TBool* noBoxVisible)
{
  NW_Evt_TabEvent_t tabEvent;
  TBrowserStatusCode status;
  NW_LMgr_Box_t* selectionCandidateBox = NULL;
  NW_LMgr_Box_t* tempBox = NULL;
  NW_GDI_Metric_t projectionX1 = 0;
  NW_GDI_Metric_t projectionX2 = 0;
  NW_GDI_Metric_t projectionS = 0;
  NW_GDI_Metric_t projectionT = 0;
  NW_GDI_Metric_t currentTopY = 0, currentBottomY = 0;
  NW_GDI_Metric_t currentLeftX = 0, currentRightX = 0;
  NW_GDI_Metric_t selectionCandidateTopY = 0, selectionCandidateBottomY = 0;
  NW_GDI_Metric_t selectionCandidateLeftX = 0, selectionCandidateRightX = 0;
  NW_GDI_Metric_t tempTopY = 0, tempBottomY = 0;
  NW_GDI_Metric_t tempLeftX = 0, tempRightX = 0;
  NW_LMgr_EventHandler_t *eventHandler = NULL;
  NW_LMgr_RootBox_t* rootBox;
  NW_GDI_Rectangle_t viewBounds;
  NW_GDI_Rectangle_t scrolledBounds;
  NW_GDI_Rectangle_t overlapS, overlapT;
  NW_Bool update = NW_FALSE;
  NW_Bool smallScreenOn;
  NW_Bool scrollCanHappen = NW_FALSE;
  NW_Int32 currentIndex = -1;
  NW_GDI_Rectangle_t overlap;
  NW_GDI_Rectangle_t boxBounds;


  // parameter assertion block 
  NW_ASSERT (iView != NULL);

  status = NW_Evt_TabEvent_Initialize (&tabEvent, NW_Evt_TabEvent_Direction_Up);
  if (status != KBrsrSuccess)
  {
    return NW_FALSE;
  }

  rootBox = NW_LMgr_RootBoxOf(iView->GetRootBox());
  smallScreenOn = NW_LMgr_RootBox_GetSmallScreenOn( rootBox );

  viewBounds = *(iView->GetDeviceContext()->DisplayBounds());
  viewBounds.point = *(iView->GetDeviceContext()->Origin());

  /* first sibling box is the highest box in case of split; we need it in case of tab up */

  if (iView->GetCurrentBox() != NULL)
  {
    GetFirstSiblingBoxRange(iView->GetCurrentBox(), &currentTopY, &currentBottomY,
                            &currentLeftX, &currentRightX );
  }

  /* Find the first selection box that's not sibling of the current box;
     Special handling for anchors */
  selectionCandidateBox = iView->GetCurrentBox();
  if ((selectionCandidateBox != NULL) &&
      NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_ActiveContainerBox_Class)) 
  {
    eventHandler = NW_LMgr_ActiveContainerBoxOf(selectionCandidateBox)->eventHandler;
  }
  do
  {
    selectionCandidateBox = GetPreviousTabIndex( selectionCandidateBox,
                                                 &currentIndex);
    if (eventHandler != NULL) 
    {
      /* for active container box, it has to be visible to be selection candidate */
      if (selectionCandidateBox == NULL ||
          !NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_ActiveContainerBox_Class) ||
          NW_LMgr_ActiveContainerBoxOf(selectionCandidateBox)->eventHandler != eventHandler) 
      {
        eventHandler = NULL;
      }
    }      
  }
  while (eventHandler != NULL);

  /* for SSL, always go to the next tab box */
  /* for non-SSL, we should start from the selection box on the previous line */
  if (selectionCandidateBox != NULL && !smallScreenOn )
  {
    while(selectionCandidateBox != NULL)
    {
      GetFirstSiblingBoxRange( selectionCandidateBox, 
                           &selectionCandidateTopY, &selectionCandidateBottomY, 
                           &selectionCandidateLeftX, &selectionCandidateRightX );

      NW_LMgr_Box_t* tempSiblingBox = GetFirstSiblingBox (selectionCandidateBox);
      boxBounds = NW_LMgr_Box_GetDisplayBounds( tempSiblingBox );  

      // area boxes can overlap so if the selection bottom Y is less than the current bottom Y,
      // set this is the box we want as our candidate selection box
      if (NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_AreaBox_Class))
        {
        if (selectionCandidateBottomY < currentBottomY)
          {
          break;
          }
        }
      else
        {
        /* get the fist box that has a lower "upper corner" then the current "lower corner" is */
      if( ( NW_GDI_Rectangle_Cross(&boxBounds, &viewBounds, &overlap) ) &&
            ( selectionCandidateBottomY <= currentTopY ||
              ((selectionCandidateBottomY > currentTopY) && 
               ((selectionCandidateBottomY - currentTopY) < (currentBottomY - currentTopY)/2) &&
			   ((selectionCandidateBottomY - currentTopY) < (selectionCandidateBottomY - selectionCandidateTopY)/2))))
          {
          break;
          }
        }
      selectionCandidateBox =  GetPreviousTabIndex( selectionCandidateBox,
                                                    &currentIndex );
    }
  }

  /* for non-SSL, we should find the selection box having projection to the current box*/
  if (selectionCandidateBox != NULL && !smallScreenOn )
  {
    NW_LMgr_Box_t* siblingBox = GetFirstSiblingBox( selectionCandidateBox ); 
    if( HowBigIsBoxVisible( siblingBox, 
                            &viewBounds, &overlapS) )
    {
      projectionX1 = overlapS.point.x > currentLeftX ? overlapS.point.x : currentLeftX; // max
      projectionX2 = overlapS.point.x + overlapS.dimension.width < currentRightX ? 
                     overlapS.point.x + overlapS.dimension.width : currentRightX; // min
      projectionS = projectionX2 - projectionX1;
    }

    /* check if we've got more of them in the line immediate below and if so pick the left most 
       one with projection
    */
    NW_LMgr_Box_t* masterBox = selectionCandidateBox;
    tempBox = selectionCandidateBox;
    while( tempBox )
    {
      tempBox = GetPreviousTabIndex(masterBox, &currentIndex);
      // get the first sibling
      if( tempBox )
      {
        NW_LMgr_Box_t* siblingBox = GetFirstSiblingBox( tempBox ); 
        if( siblingBox != tempBox )
          {
          // 
          masterBox = tempBox;
          tempBox = siblingBox;          
          }
        else
          {
          masterBox = tempBox;
          }
        //
        GetFirstSiblingBoxRange( tempBox, &tempTopY, &tempBottomY,
                             &tempLeftX, &tempRightX );
        /* check if this box is in the same line as the selected one or
         */
        if( tempTopY >= selectionCandidateBottomY || 
            tempBottomY <= selectionCandidateTopY )
        {
          break;  // out of the line. 
        } 
        else 
        {
          /* get left most(ltr)/right most(rtl) box which has projection to the current box */
          /* 
            selectBox_Visible   tempBox_Visible   projectionT>0   projectionS>0   update
            --------------------------------------------------------------------------
            No                  Yes               N/A             N/A             TRUE
            --------------------------------------------------------------------------
            No                  No                N/A             N/A             TRUE
            --------------------------------------------------------------------------
            Yes                 No                N/A             N/A             FALSE
            --------------------------------------------------------------------------
            Yes                 Yes               Yes             N/A             TRUE
                                                  No              Yes             FALSE
                                                  No              No              TRUE
          */
          if( !HowBigIsBoxVisible(selectionCandidateBox, 
                                  &viewBounds, &overlapS) )
            {
            if( HowBigIsBoxVisible( masterBox, &viewBounds, &overlapT ) )
              {
              HowBigIsBoxVisible( masterBox, &viewBounds, &overlapT );
              projectionX1 = overlapT.point.x > currentLeftX ? overlapT.point.x : currentLeftX; // max
              projectionX2 = overlapT.point.x + overlapT.dimension.width < currentRightX ? 
                             overlapT.point.x + overlapT.dimension.width : currentRightX; // min
              projectionT = projectionX2 - projectionX1;
              update = NW_TRUE;
              }
            } 
          else 
            {
            if( HowBigIsBoxVisible( masterBox, &viewBounds, &overlapT ) )
              {
              projectionX1 = overlapT.point.x > currentLeftX ? overlapT.point.x : currentLeftX; // max
              projectionX2 = overlapT.point.x + overlapT.dimension.width < currentRightX ? 
                             overlapT.point.x + overlapT.dimension.width : currentRightX; // min
              projectionT = projectionX2 - projectionX1;

              // area boxes can over lap, so if the temp projection is bigger, set update to true
              if (NW_Object_IsClass(selectionCandidateBox, &NW_LMgr_AreaBox_Class) && 
                  NW_Object_IsClass(tempBox, &NW_LMgr_AreaBox_Class))
                {
                if ( projectionT > projectionS )
                  {
                  update = NW_TRUE;
                  }
                }
              else
                {
 //               if ( projectionT > 0 || (projectionS <= 0 && projectionT <= 0) )
                  if ( projectionT > 0 && projectionT > projectionS )
                  {
                  update = NW_TRUE;
                  }
                }
              }
            }

          if (update == NW_TRUE)
            {
            selectionCandidateBox = masterBox;
            selectionCandidateTopY = tempTopY;
            selectionCandidateBottomY = tempBottomY;
            projectionS = projectionT;

            update = NW_FALSE;
            }
          }        
        }
      }
    }

  /* never pass invisible selection candidate to setcurrentbox();
     let scroll happen only when scrollCanHappen or neither selection nor current box visible;
     scroll mean return NW_TRUE; others return NW_FALSE
  */
  /* 
     selBox != NULL &
     selBoxVis   selBoxVis   curBoxVis   scrlCanHappen   action
     InView      InScrl      InScrl 
     -----------------------------------------------------------------------------------------
     Yes         N/A         N/A                         no scroll; invoke SetCurrentBox()
     -----------------------------------------------------------------------------------------
     No          Yes         N/A                         scroll; currentBox = selectionBox
     -----------------------------------------------------------------------------------------
     No          No          Null or No   Yes            scroll; noBoxVisible = true
     -----------------------------------------------------------------------------------------
                             Yes          Yes            scroll
     -----------------------------------------------------------------------------------------
                             N/A          No             invoke GetVisibleBox(),SetCurrentBox()


     selBox == NULL &
     curBoxVisInScrl   scroCanHappen   action
     -----------------------------------------------------------------------------------------
     Null or No        Yes             scroll; noBoxVisible = true
     -----------------------------------------------------------------------------------------
     Yes               Yes             scroll
     -----------------------------------------------------------------------------------------
     N/A               No              invoke GetVisibleBox(),SetCurrentBox()
  */

  if( projectionS == 0 )
    {
    // scroll instead
    selectionCandidateBox = NULL;
    }

  scrolledBounds = viewBounds;
  scrollCanHappen = GetScrollUpBounds( iView, &scrolledBounds );
  if (selectionCandidateBox) 
  {
    /* selection candidate will be forced to be new current box in SetCurrentBox() */
    if(iView->IsBoxVisible( selectionCandidateBox, &viewBounds))
    {
      if(KBrsrSuccess == SetCurrentBox(selectionCandidateBox, &tabEvent, 
                                       NW_FALSE, noBoxVisible)) 
      {
        return NW_TRUE;
      }
    } 
    else if (iView->IsBoxVisible( selectionCandidateBox, &scrolledBounds))
    {
      if (iView->GetCurrentBox() != NULL)
      {
        NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
      }
      if (selectionCandidateBox != NULL)
      {
        NW_LMgr_Box_SetHasFocus (selectionCandidateBox, NW_TRUE);
      }
      iView->SetCurrentBox (selectionCandidateBox);
    } 
    else 
    {
      if (scrollCanHappen)
      {
        if (iView->GetCurrentBox() == NULL ||
            !iView->IsBoxVisible( iView->GetCurrentBox(), &scrolledBounds))
        {
          *noBoxVisible = NW_TRUE;
        }
        return NW_FALSE;
      } 
      else
      {
        selectionCandidateBox = iView->GetVisibleBox();
        NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( selectionCandidateBox );

        if( selectionCandidateBox != NULL &&
            boxBounds.point.y + boxBounds.dimension.height <= currentTopY )
        {
          SetCurrentBox( selectionCandidateBox, &tabEvent, 
                         NW_FALSE, noBoxVisible );  
        }
        return NW_TRUE;
      }
    }
  } 
  else if (scrollCanHappen)  // end of if( selectionCandidateBox )
  {
    if (iView->GetCurrentBox() == NULL ||
        !iView->IsBoxVisible( iView->GetCurrentBox(), &scrolledBounds))
    {
      *noBoxVisible = NW_TRUE;
    }
    return NW_FALSE;
  } 
  else
  {
    selectionCandidateBox = iView->GetVisibleBox();
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( selectionCandidateBox );

    if( selectionCandidateBox != NULL &&
        boxBounds.point.y + boxBounds.dimension.height <= currentTopY )
    {
      SetCurrentBox( selectionCandidateBox, &tabEvent, 
                     NW_FALSE, noBoxVisible);  
    }
    return NW_TRUE;
  }

  return NW_FALSE;
}

// -----------------------------------------------------------------------------
// SetCurrentBox
// Description: The algorithm for below method assumes that we ignore any new 
//   active box that is above, below, to the right or left that is not inside the 
//   display bounds.  On certain platforms it may be necessary to change the below
//   algorithm so that the new active box can be anywhere in the page and on 
//   calling the below function it is going to return true                   
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
TBrowserStatusCode
CMVCScroll::SetCurrentBox (NW_LMgr_Box_t* newBox, 
                           NW_Evt_TabEvent_t* tabEvent, 
                           TBool isNewPage,
                           TBool* noBoxVisible)
{
  //lint --e{774}, newBox is always non-null after the null check...

  NW_GDI_Rectangle_t viewBounds, scrolledBounds;
  NW_GDI_Rectangle_t boxBounds;
  NW_GDI_Rectangle_t newBoxBounds;
  TBrowserStatusCode status = KBrsrFailure;
  NW_Bool scrollingToLinkOnSameLine = NW_FALSE;
  NW_Bool smallScreenOn;

  // parameter assertion block 
  NW_ASSERT (iView != NULL);

  if (newBox == NULL) 
  {
    return status;
  }

  if (isNewPage) 
  {
    iView->SetCurrentBox (NULL);
  }

  smallScreenOn = NW_LMgr_RootBox_GetSmallScreenOn( iView->GetRootBox() );
  // initialize the viewBounds 
  viewBounds = *(iView->GetDeviceContext()->DisplayBounds());
  viewBounds.point = *(iView->GetDeviceContext()->Origin());
  //
  boxBounds = NW_LMgr_Box_GetDisplayBounds(iView->GetCurrentBox());
  if (iView->GetCurrentBox() != NULL) 
  {
    if (NW_Object_IsClass(iView->GetCurrentBox(), &NW_LMgr_AreaBox_Class) != NW_FALSE) 
    {
      boxBounds = NW_LMgr_AreaBox_GetBounds(iView->GetCurrentBox());
    }
    else
    {
      boxBounds = NW_LMgr_Box_GetDisplayBounds(iView->GetCurrentBox());
    }
  }
  //
  newBoxBounds = NW_LMgr_Box_GetDisplayBounds(newBox);
  if (newBox != NULL) 
  {
    if (NW_Object_IsClass(newBox, &NW_LMgr_AreaBox_Class) != NW_FALSE) 
    {
      newBoxBounds = NW_LMgr_AreaBox_GetBounds(newBox);
    }
    else
    {
      newBoxBounds = NW_LMgr_Box_GetDisplayBounds(newBox);
    }
  }

  // if the currentBox is invisible and scrolling make it more visible 
  // keep it as the currentBox 
  if (iView->GetCurrentBox() != NULL &&
      !iView->IsBoxVisible( iView->GetCurrentBox(), &viewBounds)) 
  {
    switch (NW_Evt_TabEvent_GetDirection (tabEvent)) 
    {
      case NW_Evt_TabEvent_Direction_Down:
        {
        if( smallScreenOn )
        {
          if (viewBounds.point.y + viewBounds.dimension.height <= boxBounds.point.y + iView->GetVisibleAmt().height) 
          {
            return status;
          }
        }
        break;
        }
      case NW_Evt_TabEvent_Direction_Right:
        {
        if (viewBounds.point.x + viewBounds.dimension.width <= boxBounds.point.x + iView->GetVisibleAmt().width) 
        {
          return status;
        }
        break;
        }
      case NW_Evt_TabEvent_Direction_Up:
        {
        if( smallScreenOn )
        {
          if (boxBounds.point.y + boxBounds.dimension.height <= viewBounds.point.y + iView->GetVisibleAmt().height) 
          {
            return status;
          }
        }
        break;
        }        
      case NW_Evt_TabEvent_Direction_Left:
        {
        if (boxBounds.point.x + boxBounds.dimension.width <= viewBounds.point.x + iView->GetVisibleAmt().width) 
        {
          return status;
        }
        break;
        }
      default:
        {
        break;
        }
    }
  }

    // ignore all boxes lying above, to the left or to the right of the current
    // view window
    // if this is a new page loaded, we should not consider ScrollAmt, since it won't
    // do scrollAfterCurrentBox 
    if ((isNewPage) && (!iView->IsBoxVisible( newBox, &viewBounds))) 
    {
      if (iView->GetCurrentBox() != NULL)
      {
        NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
      }
      if (newBox != NULL)
      {
        NW_LMgr_Box_SetHasFocus (newBox, NW_TRUE);
        iView->SetCurrentBox (newBox);
      }  
      *noBoxVisible = NW_TRUE;
      return status;
    }

    // otherwise we need test if it's visible after the futher scrolling, in addition to  
    // simple isBoxVislble
    switch (NW_Evt_TabEvent_GetDirection (tabEvent)) 
    {
      case NW_Evt_TabEvent_Direction_Down:
        {
        if( smallScreenOn )
        {
          scrolledBounds = viewBounds;
          scrolledBounds.point.y = 
            (NW_GDI_Metric_t)(scrolledBounds.point.y + iView->GetScrollAmt().height);
		  
        if (iView->IsBoxVisible( newBox, &viewBounds))
          {
            status = KBrsrSuccess;
          }
          else if (iView->IsBoxVisible( newBox, &scrolledBounds))
          {
            // if the box is visible in the scroll bounds, set focus, force the box, then 
            // return a failure status so that the scrolling happens
            if (iView->GetCurrentBox() != NULL)
            {
              NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
            }
            if (newBox != NULL)
            {
              NW_LMgr_Box_SetHasFocus (newBox, NW_TRUE);
            }
    
            // do not refresh here. The caller will refresh the entire page as we scrolled.
            (void) iView->ForceCurrentBox ( newBox, (NW_Bool)isNewPage, NW_FALSE );
            return status;
          }
        }
        else
        {
          status = KBrsrSuccess;
        }
        break;
        }

      case NW_Evt_TabEvent_Direction_Up:
        {
        if( smallScreenOn )
        {
          scrolledBounds = viewBounds;
          scrolledBounds.point.y = 
            (NW_GDI_Metric_t)(scrolledBounds.point.y - iView->GetScrollAmt().height);  

          if (iView->IsBoxVisible( newBox, &viewBounds))
          {
            status = KBrsrSuccess;
          }
          else if (iView->IsBoxVisible( newBox, &scrolledBounds))
          {            
            // if the box is visible in the scroll bounds, set focus, force the box, then 
            // return a failure status so that the scrolling happens
            if (iView->GetCurrentBox() != NULL)
            {
             NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
            }
            if (newBox != NULL)
            {
              NW_LMgr_Box_SetHasFocus (newBox, NW_TRUE);
            }
    
            // do not refresh here. The caller will refresh the entire page as we scrolled.
            (void) iView->ForceCurrentBox ( newBox, (NW_Bool)isNewPage, NW_FALSE);
            return status;
          }
        }
        else
          {  
          status =  KBrsrSuccess;
          }
        break;
        }
      case NW_Evt_TabEvent_Direction_Right:
        {
        scrolledBounds = viewBounds;
        scrolledBounds.point.x = 
          (NW_GDI_Metric_t)(scrolledBounds.point.x + iView->GetScrollAmt().width);

        if (IsBoxVisibleInEither(newBox, &scrolledBounds, &viewBounds)) 
        {
          status = KBrsrSuccess;
        }
        else if (boxBounds.point.y == newBoxBounds.point.y &&
                 boxBounds.point.x < newBoxBounds.point.x)
        {
          scrollingToLinkOnSameLine = NW_TRUE;
        }
        break;
        }
      case NW_Evt_TabEvent_Direction_Left:
        {
        scrolledBounds = viewBounds;
        scrolledBounds.point.x = 
          (NW_GDI_Metric_t)(scrolledBounds.point.x - iView->GetScrollAmt().width);  
        
        if (IsBoxVisibleInEither(newBox, &scrolledBounds, &viewBounds)) 
        { 
          status = KBrsrSuccess;
        }
        else if (boxBounds.point.y == newBoxBounds.point.y &&
                 boxBounds.point.x > newBoxBounds.point.x)
        {
          scrollingToLinkOnSameLine = NW_TRUE;
        }
        break;
        }
    }// end of switch on direction 
 

  if (status == KBrsrSuccess) 
  {
    if (iView->GetCurrentBox() != NULL)
    {
      NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
    }
    if (newBox != NULL)
    {
      NW_LMgr_Box_SetHasFocus (newBox, NW_TRUE);
    }
    status = iView->ForceCurrentBox ( newBox, (NW_Bool)isNewPage, NW_TRUE);
  }
  else
  { 
    if (iView->GetCurrentBox() != NULL &&
      !iView->IsBoxVisible( iView->GetCurrentBox(), &viewBounds))
    { 
      if (scrollingToLinkOnSameLine == NW_TRUE)
      {
          *noBoxVisible = NW_FALSE;
      }
      else
      {
          *noBoxVisible = NW_TRUE;
      }
    }
  }
  
  return status;
}

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::TabLeft(TBool* noBoxVisible)
{
  NW_LMgr_Box_t* box = NULL;
  NW_LMgr_Box_t* firstVisible = NULL;
  NW_Evt_TabEvent_t tabEvent;
  TBrowserStatusCode status = KBrsrFailure;
  NW_GDI_Metric_t reference = 0, refY2 = 0;
  NW_GDI_Metric_t referenceX = 0, refX2 = 0;
  NW_GDI_Metric_t boxReference = 0, boxY2 = 0;
  NW_GDI_Metric_t boxReferenceX = 0, boxX2 = 0;
  NW_LMgr_RootBox_t* rootBox = iView->GetRootBox();
  NW_Bool isPageRTL = NW_FALSE;
  NW_Int32 currentIndex = -1;

  isPageRTL = NW_LMgr_RootBox_IsPageRTL(rootBox);
  status = NW_Evt_TabEvent_Initialize (&tabEvent, NW_Evt_TabEvent_Direction_Left);
  if (status != KBrsrSuccess)
  {
    return NW_FALSE;
  }

  if (iView->GetCurrentBox() != NULL)
  {
    GetBoxRange(iView->GetCurrentBox(), &reference, &refY2, &referenceX, &refX2 );
  }
  
  if (isPageRTL) 
  {
    box = GetNextTabIndex(iView->GetCurrentBox(), &currentIndex);
  }
  else 
  {
    box = GetPreviousTabIndex(iView->GetCurrentBox(), &currentIndex);
  }      

  if (!box)
  {
    return NW_FALSE;
  }

  GetBoxRange(box, &boxReference, &boxY2, &boxReferenceX, &boxX2 );
  // If upper edge of next box is lower than the lower edge of current box
  // or lower edge of next box is higher than the upper edge of current box
  // we can not move right
  if ((boxReference >= refY2) || (boxY2 <= reference))
  {
    return NW_FALSE;
  }

  // check if the selection candidate box is visible
  GetFirstVisible(box, &firstVisible );
  if( firstVisible == NULL )
  {
    // try with the original one
    firstVisible = box;
  }
  if(KBrsrSuccess == SetCurrentBox(firstVisible, &tabEvent, NW_FALSE, noBoxVisible)) 
  {
    return NW_TRUE;
  }

  return NW_FALSE;
}

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::TabRight(TBool* noBoxVisible)
  {
  NW_LMgr_Box_t* firstVisible = NULL;
  NW_LMgr_Box_t* box = NULL;
  NW_Evt_TabEvent_t tabEvent;
  TBrowserStatusCode status = KBrsrFailure;
  NW_GDI_Metric_t reference = 0, refY2 = 0;
  NW_GDI_Metric_t referenceX = 0, refX2 = 0;
  NW_GDI_Metric_t boxReference = 0, boxY2 = 0;
  NW_GDI_Metric_t boxReferenceX = 0, boxX2 = 0;
  NW_LMgr_RootBox_t* rootBox = iView->GetRootBox();
  NW_Bool isPageRTL = NW_FALSE;
  NW_Int32 currentIndex = -1;

  isPageRTL = NW_LMgr_RootBox_IsPageRTL(rootBox);
  status = NW_Evt_TabEvent_Initialize (&tabEvent, NW_Evt_TabEvent_Direction_Right);
  if (status != KBrsrSuccess)
    {
    return NW_FALSE;
    }

  if (iView->GetCurrentBox() != NULL)
    { 
    GetBoxRange(iView->GetCurrentBox(), &reference, &refY2, &referenceX, &refX2 );
    }

  box = iView->GetCurrentBox();

  if (isPageRTL) 
    {
    box = GetPreviousTabIndex(box, &currentIndex);
    }
  else 
    {
    box = GetNextTabIndex(box, &currentIndex);
    }

  if (!box)
    {
    return NW_FALSE;
    }
  
  GetBoxRange(box, &boxReference, &boxY2, &boxReferenceX, &boxX2 );
  // If upper edge of next box is lower than the lower edge of current box
  // or lower edge of next box is higher than the upper edge of current box
  // we can not move right 
  if ((boxReference >= refY2) || (boxY2 <= reference))
    {
    return NW_FALSE;
    }

  // check if the selection candidate box is visible
  GetFirstVisible(box, &firstVisible);
  if( firstVisible == NULL )
    {
    // try with the original one
    firstVisible = box;
    }
  if(KBrsrSuccess == SetCurrentBox(firstVisible , &tabEvent, NW_FALSE, noBoxVisible)) 
    {
    return NW_TRUE;
    }

  return NW_FALSE;
}

// -----------------------------------------------------------------------------
NW_LMgr_Box_t*
CMVCScroll::GetNextTabIndex(const NW_LMgr_Box_t* oldBox,
                            NW_Int32* currentTabIndex)
      {
  return GetTabIndex( oldBox, NW_TRUE, currentTabIndex );
}
 
// -----------------------------------------------------------------------------
NW_LMgr_Box_t*
CMVCScroll::GetPreviousTabIndex (const NW_LMgr_Box_t* oldBox,
                                 NW_Int32* currentTabIndex)
{
  return GetTabIndex( oldBox, NW_FALSE, currentTabIndex );
  }

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::ScrollLink(NW_Evt_ControlKeyType_t key,
                      TBool* noBoxVisible)
{
  NW_LMgr_Box_t* currentBox = NULL;
  NW_LMgr_Box_t* selectionCandidateBox = NULL;
  NW_Evt_TabEvent_t tabEvent;
  TBrowserStatusCode status;
  NW_LMgr_Box_t* firstVisible = NULL;
  NW_Int32 currentIndex = -1;

  // parameter assertion block 
  NW_ASSERT (iView != NULL);
 
  if (key == NW_Evt_ControlKeyType_Down)
  {
    status = NW_Evt_TabEvent_Initialize (&tabEvent, NW_Evt_TabEvent_Direction_Down);
  }
  else
  {
    status = NW_Evt_TabEvent_Initialize (&tabEvent, NW_Evt_TabEvent_Direction_Up);
  }

  if (status != KBrsrSuccess)
  {
    return NW_FALSE;
  }

  // Special handling for anchors 
  currentBox = iView->GetCurrentBox();
  if (currentBox && currentBox->propList)
  {
    if (NW_Object_IsInstanceOf(currentBox->propList, &NW_LMgr_SlavePropertyList_Class))
    {
      // this is a slave, get the master box
      // if master box does not have size problem
      // we should use the master box as selectionCandidateBox
      NW_LMgr_Box_t* tempBox = 
          NW_LMgr_SlavePropertyList_GetMasterBox(NW_LMgr_SlavePropertyListOf(currentBox->propList));
      NW_ASSERT(tempBox);
      NW_GDI_Rectangle_t boxBounds;
      NW_Bool isVisible = NW_LMgr_Box_GetVisibleBounds (tempBox, &boxBounds);
      if (isVisible && boxBounds.dimension.width != 0 && boxBounds.dimension.height != 0) 
      {
        currentBox = tempBox;
      }
      // else: master box has size problem
      // we should use the slave box as selectionCandidateBox
    }
  }

  if (key == NW_Evt_ControlKeyType_Down)
  {
    selectionCandidateBox = GetNextTabIndex(currentBox, &currentIndex);
  }
  else
  {
    selectionCandidateBox = GetPreviousTabIndex(currentBox, &currentIndex);
  }

  // check if the selection candidate box is visible
  if (selectionCandidateBox)
  {
    GetFirstVisible(selectionCandidateBox, &firstVisible );
  }
  
  if( firstVisible ) 
  {
    if(KBrsrSuccess == SetCurrentBox(selectionCandidateBox, &tabEvent, 
      NW_FALSE, noBoxVisible)) 
    {
		  return NW_TRUE;
    }
  }
  return NW_FALSE;
}

// -----------------------------------------------------------------------------
NW_Uint16
CMVCScroll::GetScrollAmount()
{
  TGDIDeviceContextFontSizeLevel fontSizeLevel = 
      (TGDIDeviceContextFontSizeLevel)NW_Settings_GetFontSizeLevel(); 

  // return calculated for 2 lines
  switch(fontSizeLevel)
  {
    case EGDIDeviceContextFontSizeLevelAllSmall:
    case EGDIDeviceContextFontSizeLevelSmaller:
      return (SMALL_SCROLL * 2);

    case EGDIDeviceContextFontSizeLevelNormal:
      return (NORMAL_SCROLL * 2);

    case EGDIDeviceContextFontSizeLevelLarger:    
    case EGDIDeviceContextFontSizeLevelAllLarge:
      return (LARGE_SCROLL * 2);

    default:
      return (NORMAL_SCROLL * 2);
  }  
}

// -----------------------------------------------------------------------------
NW_Uint16
CMVCScroll::GetPagingAmount()
{
  CGDIDeviceContext* deviceContext;
  const NW_GDI_Rectangle_t* rectangle;
  NW_GDI_Metric_t height = 0;

  deviceContext = NW_LMgr_RootBox_GetDeviceContext (iView->GetRootBox());
       
  rectangle = deviceContext->DisplayBounds();
        
  height = (NW_GDI_Metric_t)(rectangle->dimension.height); 

  TGDIDeviceContextFontSizeLevel fontSizeLevel = 
      (TGDIDeviceContextFontSizeLevel)NW_Settings_GetFontSizeLevel(); 

  switch(fontSizeLevel)
  {
    case EGDIDeviceContextFontSizeLevelAllSmall:
    case EGDIDeviceContextFontSizeLevelSmaller:
      return (NW_Uint16)(height - (SMALL_SCROLL * 2));

    case EGDIDeviceContextFontSizeLevelNormal:
      return (NW_Uint16)(height - (NORMAL_SCROLL * 2));

    case EGDIDeviceContextFontSizeLevelLarger:    
    case EGDIDeviceContextFontSizeLevelAllLarge:
      return (NW_Uint16)(height - (LARGE_SCROLL * 2));

    default:
      return (NW_Uint16)(height - (NORMAL_SCROLL * 2));
  }  
}

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::ProcessControlKey(NW_Evt_ControlKeyType_t key)
{
  TBool noBoxVisible = EFalse;
  NW_Evt_ScrollEvent_t scrollEvent;
  TBool absorbed = EFalse;
  NW_Uint16 scrollAmount = 18;

  // first we try to issue a tab event 

  switch(key)
  {
  case NW_Evt_ControlKeyType_Left:
    
    absorbed = TabLeft(&noBoxVisible);
    break;

  case NW_Evt_ControlKeyType_Right:

    absorbed = TabRight(&noBoxVisible);
    break;

  case NW_Evt_ControlKeyType_Up:
  
    absorbed = TabBackward(&noBoxVisible);
    break;

  case NW_Evt_ControlKeyType_Down:
  
    absorbed = TabForward(NW_FALSE, &noBoxVisible);
    break;

  default:
    break;
  }

  if (absorbed) 
  {
    return NW_TRUE;
  }

  // we were unable to tab - next we try to scroll 
  TBrowserStatusCode status = KBrsrSuccess;
  if (status) {} // fix warning
  switch (key) 
  {
    case NW_Evt_ControlKeyType_Left:
      scrollAmount = (NW_Uint16)iView->GetScrollAmt().width;
      status =
        NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                       NW_Evt_ScrollEvent_Direction_Left, 
                                       scrollAmount);

      break;
    case NW_Evt_ControlKeyType_Right:
      scrollAmount = (NW_Uint16)iView->GetScrollAmt().width;
      status =
        NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                     NW_Evt_ScrollEvent_Direction_Right, 
                                     scrollAmount);
    
      break;
    case NW_Evt_ControlKeyType_Up:
       scrollAmount = (NW_Uint16)iView->GetScrollAmt().height;
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Up, 
                                         scrollAmount);

      break;
    case NW_Evt_ControlKeyType_Down:
       scrollAmount = (NW_Uint16)iView->GetScrollAmt().height;
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Down, 
                                         scrollAmount);

      break;
    default:
      break;
  }

  NW_ASSERT (status == KBrsrSuccess);

  //lint -e{645} Symbol may not have been initialized
  absorbed = iView->EpocProcessEvent ((const NW_Evt_Event_t*) &scrollEvent);

  if(absorbed && noBoxVisible)
  {
    NW_LMgr_Box_t* box = NULL;
    // Scroll event was absorbed and there is no active box visible so,
    // try to see if scrolling changes the situation.
    box = iView->GetVisibleBox();
    
    if(box != NULL)
    {
      // Scroll event was absorbed and there is no active box visible so,
      // try to see if scrolling changes the situation.
	    // scrolling up should focus on the lowest box; 
		  // scrolling down should focus on the highest box
      switch (key) 
	    {
        case NW_Evt_ControlKeyType_Left:
        case NW_Evt_ControlKeyType_Right:
          iView->ForceCurrentBox ( box, NW_FALSE, NW_TRUE);
			    
          break;

        case NW_Evt_ControlKeyType_Down:
          if (box != iView->GetCurrentBox())
            {
            if (iView->GetCurrentBox() != NULL)
              {
              NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
              }
              if (box != NULL)
              {
              NW_LMgr_Box_SetHasFocus (box, NW_TRUE);
              }
    
            // do not refresh here. The caller will refresh the entire page as we scrolled.
            (void) iView->ForceCurrentBox ( box, NW_FALSE, NW_FALSE );
            }
			   
          break;

        case NW_Evt_ControlKeyType_Up:
          box = iView->GetLastVisibleBox();
          if(box != NULL)
			    {
		        iView->SetCurrentBox (box);
			    }
          break;

        default:
          break;
	    } // end of switch
    } // end of if(absorbed && noBoxVisible)
  }
  return (NW_Bool)absorbed;
}

// -----------------------------------------------------------------------------
// CMVCScroll::ProcessControlKeyVL
// Description: This method handles the processing of control keys when in 
//   Vertical Layout mode (hince the VL at the end of the method name)
// Returns: TBool : NW_TRUE if the event was absorbed, NW_FALSE otherwise
// -----------------------------------------------------------------------------
// 
TBool 
CMVCScroll::ProcessControlKeyVL(NW_Evt_ControlKeyType_t key)
{
  TBool noBoxVisible = EFalse;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Evt_ScrollEvent_t scrollEvent;
  TBool absorbed = EFalse;
  NW_Uint16 scrollAmount = 18;

  switch(key)
  {
  case NW_Evt_ControlKeyType_Left:
  case NW_Evt_ControlKeyType_Right:
      {    
    absorbed = ScrollPage(key);
    break;
      }
  case NW_Evt_ControlKeyType_Up:
  case NW_Evt_ControlKeyType_Down:
      {
    absorbed = ScrollLink(key, &noBoxVisible);
    break;
      }
  default:
      {
      absorbed = EFalse;
    break;
  }
    }
   
  if( !absorbed ) 
  {
    // we were unable to tab - next we try to scroll 
    scrollAmount = GetScrollAmount();

    // we were unable to tab - next we try to scroll 
    switch (key) 
    {
      case NW_Evt_ControlKeyType_Up:
      {
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Up, 
                                         scrollAmount);
        break;
      }
      case NW_Evt_ControlKeyType_Down:
      {
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Down, 
                                         scrollAmount);
        break;
      }
      default:
      {
        status = KBrsrFailure;
        break;
      }
    } //end switch

    if( status == KBrsrSuccess )
    {
      absorbed = iView->EpocProcessEvent ((const NW_Evt_Event_t*) &scrollEvent);

      if(absorbed && noBoxVisible)
      {
        NW_LMgr_Box_t* box = NULL;
        // Scroll event was absorbed and there is no active box visible so,
        // try to see if scrolling changes the situation.
        box = iView->GetVisibleBox();
        if(box != NULL)
        {
          (void) iView->ForceCurrentBox ( box, NW_FALSE, NW_TRUE);
        }
      }
    }
  }
  return (NW_Bool)absorbed;
}

// -----------------------------------------------------------------------------
TBool 
CMVCScroll::ScrollPage(NW_Evt_ControlKeyType_t key)
{  
  NW_GDI_Point2D_t origin;
  NW_Uint16 scrollAmount = 18;
  NW_Evt_ScrollEvent_t scrollEvent;  
  TBool absorbed = EFalse;

  scrollAmount = GetPagingAmount();
  origin = *(iView->GetDeviceContext()->Origin());
 
  TBrowserStatusCode status = KBrsrSuccess;
  if (status) {} // fix warning

  switch (key) 
  {
    case NW_Evt_ControlKeyType_Home:
    case NW_Evt_ControlKeyType_End:
      {
      // scroll up to the first page or
      // scroll to the last page
      NW_Int32 endAmount;
      NW_GDI_Dimension2D_t canvasSize;
      NW_GDI_Rectangle_t deviceBounds;

      // change keys in case of right to left page
      if( NW_LMgr_RootBox_IsPageRTL( NW_LMgr_RootBoxOf( iView->GetRootBox() ) ) == NW_TRUE )
        {
        key = ( key == NW_Evt_ControlKeyType_Home ? 
          NW_Evt_ControlKeyType_End : NW_Evt_ControlKeyType_Home );
        }

      // initialize key events
      if( key == NW_Evt_ControlKeyType_Home )
        {
        scrollAmount = (NW_Uint16)origin.y;
        status =
          NW_Evt_ScrollEvent_Initialize( &scrollEvent,
                                       NW_Evt_ScrollEvent_Direction_Up, 
                                       scrollAmount );
        }
      else
      // NW_Evt_ControlKeyType_End 
        {      
        (void)NW_LMgr_RootBox_GetExtents( iView->GetRootBox(), &canvasSize );
        deviceBounds = *(iView->GetDeviceContext()->DisplayBounds());
        // calculate amount of srcoll
        endAmount = canvasSize.height - deviceBounds.dimension.height - origin.y;
        // adjust it
        scrollAmount = (NW_Uint16)( endAmount > 0 ? endAmount : 0 );

        status = 
          NW_Evt_ScrollEvent_Initialize( &scrollEvent,
                                       NW_Evt_ScrollEvent_Direction_Down, 
                                       scrollAmount );
        }
      break;
      }
    case NW_Evt_ControlKeyType_Left:
      if (NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBoxOf(iView->GetRootBox()))) 
      {
        status =
        NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                       NW_Evt_ScrollEvent_Direction_Down, 
                                       scrollAmount);
      }
      else
      {
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Up, 
                                         scrollAmount);
      }
      break;
    case NW_Evt_ControlKeyType_Right:
      if (NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBoxOf(iView->GetRootBox()))) 
      {
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Up, 
                                         scrollAmount);
      }
      else
      {
        status =
          NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                         NW_Evt_ScrollEvent_Direction_Down, 
                                         scrollAmount);
      }
      break;

    default:
      break;
  }
    
  NW_ASSERT (status == KBrsrSuccess);

  // in page up/page down case, 
  // change the current box only if the origin has been changed.
  // which means that we really did do a page up/page down.
  // this is meant to filter cases when the user hits page up on
  // the first page or page down on the last one.

  //lint -e{645} Symbol may not have been initialized
  absorbed = iView->EpocProcessEvent ((const NW_Evt_Event_t*) &scrollEvent);

  if(absorbed)
    {
    // check if the origin has been changed
    NW_GDI_Point2D_t newOrigin;
    newOrigin = *(iView->GetDeviceContext()->Origin());
    if( newOrigin.y != origin.y )
      {
      NW_LMgr_Box_t* box = NULL;

      box = iView->GetVisibleBox ();
      if( box != NULL )
        {
        if( box != iView->GetCurrentBox() )
          {
          if (iView->GetCurrentBox() != NULL)
            {
            NW_LMgr_Box_SetHasFocus (iView->GetCurrentBox(), NW_FALSE);
            }
            if (box != NULL)
            {
            NW_LMgr_Box_SetHasFocus (box, NW_TRUE);
            }
    
          // do not refresh here. The caller will refresh the entire page as we scrolled.
          (void) iView->ForceCurrentBox ( box, NW_FALSE, NW_FALSE );

          (void) iView->ScrollAfterCurrentBox(  iView->GetCurrentBox(), NW_TRUE );
	        }
        }
      }
    }
  return absorbed;
}

