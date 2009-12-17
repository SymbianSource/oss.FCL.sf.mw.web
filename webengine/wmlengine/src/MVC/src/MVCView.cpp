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
* Description:  Handles the display of the box tree
*
*/


// INCLUDE FILES
//lint -save -e578 -e1707 static assumed for operator new, Declaration of symbol hides symbol
#include <e32def.h>  // To avoid NULL redefine warning (no #ifndef NULL)
#include <aknappui.h>
#include <aknmessagequerydialog.h>
#include <apgcli.h>
#include <badesca.h>
#include <eiksbfrm.h>
#include <eikspane.h>
#include <FeatMgr.h>
#include <f32file.h>
#include <flogger.h>
#include <nwx_defs_symbian.h>
#include <stddef.h>

#include <CenRepNotifyHandler.h>
#include <centralrepository.h>
#include <httpcachemanagerinternalcrkeys.h>

#include <stringloader.h>
#include <webkit.rsg>

#include "BoxFormatHandler.h"
#include "BoxRender.h"
#include "BrCtl.h"
#include "BrsrStatusCodes.h"
#include "CBrowserSettings.h"
#include "GDIDeviceContext.h"
#include "LMgrMarkerText.h"
#include "LMgrMarkerImage.h"
#include "LMgrAnonBlock.h"
#include "LMgrObjectBoxOOC.h"
#include "LMgrObjectBox.h"
#include "MVCScroll.h"
#include "MVCShell.h"
#include "MVCView.h"
#include "MVCViewAsyncInit.h"
#include "MVCViewListener.h"
#include "nw_adt_resizablevector.h"
#include "nwx_cleanup_epoc32.h"
#include "nwx_ctx.h"
#include "nw_evt_accesskeyevent.h"
#include "nw_evt_activateevent.h"
#include "nw_evt_clearfieldevent.h"
#include "nw_evt_controlkeyevent.h"
#include "nw_evt_fontsizechangeevent.h"
#include "nw_evt_focusevent.h"
#include "nw_evt_keyevent.h"
#include "nw_evt_openviewerevent.h"
#include "nw_evt_scrollevent.h"
#include "nw_evt_tabevent.h"
#include "nw_evt_unicodekeyevent.h"
#include "nw_fbox_buttonbox.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_formbox.h"
#include "nw_fbox_formboxutils.h"
#include "nw_fbox_checkbox.h"
#include "nw_fbox_WmlFormattedEditor.h"
#include "nw_fbox_radiobox.h"
#include "nw_fbox_epoc32inputskin.h"
#include "nw_fbox_selectbox.h"
#include "nw_fbox_textareabox.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_fbox_fileselectionbox.h"
#include "nw_hed_documentroot.h"
#include "nw_hed_iimageviewer.h"
#include "nw_image_epoc32simpleimage.h"
#include "nw_image_virtualimage.h"
#include "nw_lmgr_imagebox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_box.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_areabox.h"
#include "nw_lmgr_accesskey.h"
#include "nw_lmgr_eventhandler.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_lmgr_simplepropertylist.h"
#include "nw_lmgr_verticaltablebox.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_imagemapbox.h"
#include "nw_lmgr_animatedimagebox.h"
#include "nw_lmgr_areabox.h"
#include "nw_object_object.h"
#include "nw_system_optionlist.h"
#include "nw_xhtml_savetophonebkevent.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_xhtml_attributepropertytable.h"
#include "nwx_http_defs.h"
#include "nwx_logger.h"
#include "nwx_math.h"
#include "nwx_settings.h"

#include "TEncodingMapping.h"
#include "urlloader_urlloaderint.h"
#include <brctlinterface.h>
#include "brctldialogsprovider.h"

#include "HistoryController.h"
#include "WmlInterface.h"
//lint -restore
#include "pagescaler.h"

#include "nw_lmgr_posflowbox.h"

#ifdef __WINS__
#ifdef _DEBUG
//#define _DEBUG_LOG
#ifdef _DEBUG_LOG
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_Box_DumpBoxTree(NW_LMgr_Box_t* box);
#endif /* DEBUG_LOG */
#endif /* DEBUG */
#endif /* WINS */


// ============================= PRIVATE FUNCTIONS ===============================
NW_Bool
CView::ProcessFontSizeChangeEvent ()
{
  iDeviceContext->SetFontSizeLevel();

  return NW_TRUE;
}

NW_GDI_Rectangle_t
CView::GetBoxDisplayBounds(NW_LMgr_Box_t* aBox )
  {
  NW_GDI_Rectangle_t boxBounds;

  if (NW_Object_IsClass( aBox, &NW_LMgr_AreaBox_Class))
    {
    boxBounds = NW_LMgr_AreaBox_GetBounds( aBox );
    }
  else
    {
    boxBounds = NW_LMgr_Box_GetDisplayBounds( aBox );
    }
  return boxBounds;
  }

void
CView::SetBoxFormatBounds( NW_LMgr_Box_t* aBox, NW_GDI_Rectangle_t aBoxBounds )
  {
  if (NW_Object_IsClass( aBox, &NW_LMgr_AreaBox_Class))
    {
    // set area box is not supported
    NW_ASSERT( 0 );
    }
  else
    {
    NW_LMgr_Box_SetFormatBounds( aBox, aBoxBounds );
    }
  }

/*
This is a fix for version 2.8. For 3.0, we have to do more check for the position absolute, otherwise, it will break the 
absolute position.
*/
void CView::GetParentBox(NW_LMgr_Box_t* parentBox, NW_LMgr_Box_t** pReturnedParentBox)
{
	if(parentBox == NULL) {
		return;
	}

	if (NW_Object_IsInstanceOf(parentBox, &NW_LMgr_FlowBox_Class))
	{
		if(*pReturnedParentBox == NULL) {
			*pReturnedParentBox = parentBox;
		}
	}

	if (NW_Object_IsClass(parentBox, &NW_LMgr_StaticTableCellBox_Class) ||
		NW_Object_IsClass(parentBox, &NW_LMgr_VerticalTableCellBox_Class)) 
	{
		*pReturnedParentBox = parentBox;
		return;
	} else {
		//continue to find parent
		parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(parentBox);
		if(parentBox != NULL) {
			GetParentBox(parentBox, pReturnedParentBox);
		}
	}
}
// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::HandleFormattingComplete ()
    {
    TBrowserStatusCode status = KBrsrSuccess;
    iRootBox->calculateExtents = NW_TRUE;
    iCreateTabList = NW_FALSE;

    NW_LMgr_RootBox_InvalidateCache (iRootBox );
    // if the user has not tabbed to any tab item
    if (GetActiveBoxId() == 0)
        {
        if (((iInitialDocPosition.x != 0) && (iInitialDocPosition.x != RIGHT_ORIGIN))
            || (iInitialDocPosition.y !=0))
            {
            SetOrigin (&iInitialDocPosition);
            }

        // initialActiveBoxId is either the activeBoxId from history or is the first tab index
        status = GotoActiveBox (iInitialActiveBoxId);
        if ( iAnchorName != NULL && iInitialActiveBoxId == 0 )
            {
			  NW_LMgr_Box_t* targetBox = NULL;
			  NW_LMgr_Box_t* box = NULL;
			  NW_LMgr_BoxVisitor_t boxVisitor;
			  NW_LMgr_Property_t  prop;

			  /* find the named box */

			  NW_LMgr_BoxVisitor_Initialize (&boxVisitor, (NW_LMgr_Box_t *)iRootBox);
			  (void) NW_LMgr_BoxVisitor_SetCurrentBox (&boxVisitor, (NW_LMgr_Box_t *)iRootBox);


            while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL)
                {
				status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_boxName, &prop);

                if ((status == KBrsrSuccess) && (prop.type == NW_CSS_ValueType_Text))
                    {
                    if (!NW_Text_Compare (iAnchorName, prop.value.object))
                        {
					targetBox = box;
					break;
				  }
				}
			  }

            if (targetBox != NULL)
                {
				(void) JumpToLink (box, NW_FALSE);
			  }

			  (void) Draw(NW_TRUE);
			  NW_Object_Delete(iAnchorName);
			  iAnchorName = NULL;

		  }
        }
    else
        {
        status = Draw (NW_TRUE);
        }
    for( TInt n=0; n<iListeners.Count(); n++ )
        {
        iListeners[n]->FormattingComplete();
        }
    return status;
    }

/* ------------------------------------------------------------------------- */
//lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h))
TBrowserStatusCode CView::GetBoxTabIndex (NW_LMgr_Box_t* box, NW_ADT_Vector_Metric_t* index)
  {
  NW_Int32 i;
  NW_ADT_Vector_Metric_t size;
  NW_LMgr_Box_t* tempBox;
  NW_LMgr_RootBox_t* rootBox = iRootBox;
  NW_GDI_Rectangle_t tempBoxBounds;
  NW_GDI_Rectangle_t boxBounds;
  NW_Bool isPageRTL = NW_LMgr_RootBox_IsPageRTL(rootBox);
  NW_Bool smallScreenOn = NW_LMgr_RootBox_GetSmallScreenOn( iRootBox );

  NW_ASSERT(box != NULL);
  *index = NW_ADT_Vector_AtEnd;

  size = NW_ADT_Vector_GetSize( iTabList );

  // if a box is about to be added to the tab list the second time -so it's already in the list
  // we need to delete it from the list as this time, then box might have different x y which
  // requires different index to return with.
  i = 0;
  while( i < size )
    {
    tempBox = (NW_LMgr_Box_t*)*NW_ADT_Vector_ElementAt( iTabList,
      (NW_ADT_Vector_Metric_t)i);
    // check if the box is duplicated
    if( tempBox == box )
      {
      NW_ADT_DynamicVector_RemoveAt( iTabList, (NW_ADT_Vector_Metric_t)i );
      break;
      }
    // check if this is a image map box 'cause it appends the area boxes into the list
    // instead of the image map box - only in normal mode
    else if( NW_Object_IsClass(box, &NW_LMgr_ImageMapBox_Class) == NW_TRUE &&
      smallScreenOn == NW_FALSE )
      {
      //
      NW_ADT_Vector_Metric_t arrayIndex;
      NW_ADT_Vector_Metric_t elementSize;
      NW_LMgr_Box_t* areaBox = NULL;
      NW_ADT_DynamicVector_t* areaBoxArray =
        NW_LMgr_ImageMapBox_GetAreaBoxList( NW_LMgr_ImageMapBoxOf( box ) );
      if( areaBoxArray )
        {
        // find the index of the node vector
        elementSize = NW_ADT_Vector_GetSize(areaBoxArray);

        // loop through all the area boxes in the array and check if the box has already
        // been added
        for( arrayIndex = 0; arrayIndex < elementSize; arrayIndex++ )
          {
          areaBox = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt( areaBoxArray, arrayIndex );
          if( tempBox == areaBox )
            {
            NW_ADT_DynamicVector_RemoveAt( iTabList, (NW_ADT_Vector_Metric_t)i );
            // set size variable
            size = NW_ADT_Vector_GetSize( iTabList );
            // get the next box with the same index ( the result of the delete )
            i--;
            break;
            }
          }
        }
      }
    i++;
    }

  size = NW_ADT_Vector_GetSize( iTabList );
  CMVCScroll* scrollView = static_cast<CMVCScroll*>(GetScroll());
  NW_LMgr_Box_t* siblingBox = scrollView->GetFirstSiblingBox(box);
  boxBounds = NW_LMgr_Box_GetDisplayBounds(siblingBox);

  for( i = size - 1; i >= 0; i-- )
    {
    tempBox = (NW_LMgr_Box_t*)*NW_ADT_Vector_ElementAt(iTabList,
                                                        (NW_ADT_Vector_Metric_t)i);
    siblingBox = scrollView->GetFirstSiblingBox( NW_LMgr_BoxOf(tempBox) );
    tempBoxBounds = GetBoxDisplayBounds( siblingBox );

    // rule of inserting: first check y so that links on the same line go right after each other
    // and then check x to keep the order within the line

    // crossing rectangles tells us if those two boxes are in the same line
    NW_GDI_Rectangle_t boxRect;
    NW_GDI_Rectangle_t tempBoxRect;

    // x does not matter as we are only interested in y.

    // set fake x and width. we must set the same x, width values on those two boxes though.
    boxRect.point.x = tempBoxRect.point.x = 0;
    boxRect.dimension.width = tempBoxRect.dimension.width = 10;
    // set real y, height values
    boxRect.point.y = boxBounds.point.y;
    boxRect.dimension.height = boxBounds.dimension.height;
    tempBoxRect.point.y = tempBoxBounds.point.y;
    tempBoxRect.dimension.height = tempBoxBounds.dimension.height;

    if( NW_GDI_Rectangle_Cross( &boxRect, &tempBoxRect, NULL ) )
      {
      // same line

      // on right to left pages the x has to be decreasing as you jump
      // from link to link within the line.
      if( !isPageRTL )
        {
        if( boxBounds.point.x > tempBoxBounds.point.x ||
            (boxBounds.point.x == tempBoxBounds.point.x &&
             boxBounds.point.y > tempBoxBounds.point.y) )
          {
          *index = (NW_ADT_Vector_Metric_t)( i + 1 );
          break;
          }
        else if( i == 0 )
          {
          // the candiate is above even the first box
          *index = 0;
          break;
          }
        }
      // rtl
      else
        {
        if( boxBounds.point.x < tempBoxBounds.point.x ||
            (boxBounds.point.x == tempBoxBounds.point.x &&
             boxBounds.point.y > tempBoxBounds.point.y) )
          {
          *index = (NW_ADT_Vector_Metric_t)( i + 1 );
          break;
          }
        else if( i == 0 )
          {
          // the candiate is above even the first box
          *index = 0;
          break;
          }
        }
      }
    else if( boxBounds.point.y > tempBoxBounds.point.y )
      {
      // the box is right below the tempbox.
      *index = (NW_ADT_Vector_Metric_t)( i + 1 );
      break;
      }
    else if( i == 0 )
      {
      // the candiate is above even the first box
      *index = 0;
      break;
      }
    else
      {
      // not on the same line but above, keep going
      }
    }
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBool CView::ValidateAddedIndex(NW_LMgr_Box_t* aBox, NW_LMgr_Box_t* aMasterBox)
    {
    NW_GDI_Rectangle_t boxBounds = GetBoxDisplayBounds(aMasterBox);
    // If master box is not visible, do not add to tab index
    // Break box now has a width of 1. Don't add to tab index if there are only break boxes
    if ( boxBounds.dimension.width == 0 || boxBounds.dimension.height == 0)
        {
        return EFalse;
        }

    // If all the boxes associated with this box are not visible, don't add to tab index
    NW_LMgr_BoxVisitor_t boxVisitor;
    NW_LMgr_Box_t* visitedBox;
    NW_LMgr_BoxVisitor_Initialize (&boxVisitor, aBox);
    while((visitedBox = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL)
        {
        boxBounds = GetBoxDisplayBounds(visitedBox);
        if (NW_Object_IsInstanceOf(visitedBox, &NW_LMgr_ContainerBox_Class))
            {
            continue;
            }

        if (( boxBounds.dimension.width != 0 || boxBounds.dimension.height != 0) &&
            !NW_Object_IsInstanceOf(visitedBox, &NW_LMgr_BreakBox_Class))
            {
            // Found a visible box
            return ETrue;;
            }
        }
    NW_LMgr_BoxVisitor_Initialize (&boxVisitor, aMasterBox);
    while((visitedBox = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL)
        {
        boxBounds = GetBoxDisplayBounds(visitedBox);
        if (NW_Object_IsInstanceOf(visitedBox, &NW_LMgr_ContainerBox_Class))
            {
            continue;
            }
        if ((boxBounds.dimension.width != 0 && boxBounds.dimension.height != 0) &&
            !NW_Object_IsInstanceOf(visitedBox, &NW_LMgr_BreakBox_Class))
            {
            // Found a visible box
            return ETrue;;
            }
        }
    return EFalse;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CView::AppendTabItem (NW_LMgr_Box_t* aBox)
  {
  TBrowserStatusCode status = KBrsrSuccess;
  //lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h))
  NW_ADT_Vector_Metric_t index = 0;
  NW_LMgr_Property_t prop;
  NW_LMgr_Box_t* box = aBox;

  if (box->propList && NW_Object_IsInstanceOf(box->propList, &NW_LMgr_SlavePropertyList_Class))
    {
    // this is a slave, get the master box as we MUST NOT add slave box to the tablist
    // Slave boxes are destroyed when box tree is collaped. Collapse can happen multiple times while
    // the page is being loaded and if we added slave boxes to the tablist, then we would end up having
    // invalid pointers in the tablist after each collapse.
    NW_LMgr_Box_t* masterBox = NW_LMgr_SlavePropertyList_GetMasterBox( box->propList );
    if( !masterBox )
      {
      return KBrsrUnexpectedError;
      }
    box = masterBox;
    }
    // Add the new box only if there is something visible for this active box
    if (!ValidateAddedIndex(aBox, box))
        {
        return KBrsrSuccess;
        }
  // set available accesskey
  status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_accesskey, &prop);

  if (status == KBrsrSuccess)
    {
    (void) NW_LMgr_AccessKey_SetAvailableKey (NW_LMgr_AccessKeyOf (prop.value.object),
      iTabList);
    }
  // ignore duplicated boxes
  if( GetBoxTabIndex (box, &index) == KBrsrSuccess )
    {
    // if the box is an image map box, and we are in normal layout mode, then we want to
    // add the image map's area boxes to the tab list instead of the image map box itself
    // if we are in small screen mode, we will add the image map box to the tab list
    if ((NW_Object_IsClass(box, &NW_LMgr_ImageMapBox_Class) == NW_TRUE) &&
      ( !NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) ))
      {
      NW_ADT_Vector_Metric_t arrayIndex;
      NW_ADT_Vector_Metric_t elementSize;
      NW_LMgr_AreaBox_t* areaBox = NULL;

      NW_LMgr_ImageMapBox_t* imageMapBox = (NW_LMgr_ImageMapBox_t*)box;
      NW_ADT_DynamicVector_t* areaBoxArray = NW_LMgr_ImageMapBox_GetAreaBoxList(imageMapBox);

      // It is possible to have an ImageMapBox with no AreaBoxArray
      // This happens when the usemap id is not present anywhere in the document
      if (areaBoxArray)
        {
        // find the index of the  node vector
        elementSize = NW_ADT_Vector_GetSize(areaBoxArray);

        // loop through all the area boxes in the array and instert them into the tab list
        int offset = 0;
        for (arrayIndex = 0; arrayIndex < elementSize; arrayIndex++)
          {
          areaBox = *(NW_LMgr_AreaBox_t**)NW_ADT_Vector_ElementAt (areaBoxArray, arrayIndex);

          //
          if( index != NW_ADT_Vector_AtEnd )
            {
            offset = arrayIndex;
            }
          if (NW_ADT_DynamicVector_InsertAt (iTabList,
            &areaBox, (NW_ADT_Vector_Metric_t)(index + offset) ) == NULL)
            {
            return KBrsrOutOfMemory;
            }
          }
        }
      }
    else
      {
      if( NW_ADT_DynamicVector_InsertAt (iTabList, &box, index) == NULL )
        {
        return KBrsrOutOfMemory;
        }
      }
    }
  return KBrsrSuccess;
  }

TBrowserStatusCode
CView::SetupTableFormat ( NW_Bool /*relayoutEvent*/ )
  {
    // no need to visit the box tree and format tables in Small Screen Mode
    // since we do not have any tables in this mode
    if( NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) )
      {
      return KBrsrNotFound;
      }

    NW_Bool foundTable = NW_FALSE;
    NW_LMgr_BoxVisitor_t boxVisitor;
    NW_LMgr_Box_t* child = NULL;
    NW_Uint8 skipChildren = NW_FALSE;

    NW_LMgr_BoxVisitor_Initialize(&boxVisitor, NW_LMgr_BoxOf(iRootBox));

    while( ( child = NW_LMgr_BoxVisitor_NextBox( &boxVisitor, &skipChildren ) ) != NULL )
      {
	    if( NW_Object_IsInstanceOf( child, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
        {
        // set maximum/minimum pass on tables
        NW_LMgr_StaticTableBox_SetAutomaticTableWidthPass( NW_LMgr_StaticTableBoxOf( child ), NW_LMgr_StaticTableBox_AutomaticWidth_MaximumPass );
        foundTable = NW_TRUE;
        }
      }

    // first check if the page has table at all as if it doesn't have, then no layout is needed at all
    if( foundTable == NW_TRUE )
      {
      ((CLMgr_BoxFormatHandler*)(iBoxFormatHandler))->SetBackgroundFormat();
      }
    else
      {
      return KBrsrNotFound;
      }

  return KBrsrSuccess;
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::UpdateScrollBars () const
  {
    NW_GDI_Dimension2D_t canvasSize;
    TBrowserStatusCode status = NW_LMgr_RootBox_GetExtents (iRootBox, &canvasSize);
    if (status != KBrsrSuccess)
        {
        return status;
        }
    iWmlControl->WKWmlInterface()->updateScrollbars(canvasSize.height, Rect().Height(), iDeviceContext->Origin()->y, 
        canvasSize.width, Rect().Width(), iDeviceContext->Origin()->x);
  return KBrsrSuccess;
  }

// -----------------------------------------------------------------------------
NW_Bool
CView::ProcessTabEvent (const NW_Evt_TabEvent_t* tabEvent)
{
  TBool noBoxVisible;
  TBool returnVal = EFalse;
  CMVCScroll* scroll;

  /* parameter assertion block */
  NW_ASSERT (tabEvent != NULL);

  scroll = static_cast<CMVCScroll*>(iScroll);

    switch (NW_Evt_TabEvent_GetDirection (tabEvent))
        {
    case NW_Evt_TabEvent_Direction_Up:
    case NW_Evt_TabEvent_Direction_Left:
      returnVal = scroll->TabBackward (&noBoxVisible);
    break;

    case NW_Evt_TabEvent_Direction_Down:
    case NW_Evt_TabEvent_Direction_Right:
      returnVal = scroll->TabForward (NW_FALSE/* not new page */, &noBoxVisible);
    break;
  }

  /* we didn't absorb the event */
  return (NW_Bool)returnVal;
}

// -----------------------------------------------------------------------------
NW_Bool
CView::ProcessScrollEvent (const NW_Evt_ScrollEvent_t* scrollEvent)
{
  NW_GDI_Point2D_t origin;
  NW_Uint16 delta;

  /* parameter assertion block */
  NW_ASSERT (scrollEvent != NULL);

  origin = *(iDeviceContext->Origin());
  delta = NW_Evt_ScrollEvent_GetAmount(scrollEvent);
    switch (NW_Evt_ScrollEvent_GetDirection (scrollEvent))
        {
    case NW_Evt_ScrollEvent_Direction_Left:
      origin.x = (NW_GDI_Metric_t) (origin.x - delta);
      break;

    case NW_Evt_ScrollEvent_Direction_Right:
      origin.x = (NW_GDI_Metric_t) (origin.x + delta);
      break;

    case NW_Evt_ScrollEvent_Direction_Up:
      origin.y = (NW_GDI_Metric_t) (origin.y - delta);
      break;

    case NW_Evt_ScrollEvent_Direction_Down:
      origin.y = (NW_GDI_Metric_t) (origin.y + delta);
      break;

    default:
      /* we didn't absorb the event */
      return NW_FALSE;
  }

  /* */
  TBrowserStatusCode status = SetOrigin (&origin);
  NW_ASSERT (status == KBrsrSuccess);

  /* we absorbed the event */
  return NW_TRUE;
}

// -----------------------------------------------------------------------------
void
CView::DeleteOptionItems ()
{
  //lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h))
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t size;

    if (iOptionItemList == NULL)
        {
    return;
  }
  size = NW_ADT_Vector_GetSize(iOptionItemList);
    if (size == 0 )
        {
    NW_Object_Delete(iOptionItemList);
    iOptionItemList = NULL;
    return;
  }

  for(index =0;index<size; index++)
  {
     NW_Ucs2 *str =  *(NW_Ucs2 **)
        NW_ADT_Vector_ElementAt (iOptionItemList, index);
     NW_Mem_Free(str);
  }

  NW_Object_Delete(iOptionItemList);
  iOptionItemList = NULL;
  return;
}

// ============================= PUBLIC FUNCTIONS ===============================
void
CView::UpdateCBAs () const
{
  NW_AppServices_SoftKeyAPI_t* softKeyAPI;
  NW_HED_AppServices_t* appServices;
  NW_LMgr_RootBox_t* rootBox = iRootBox;

  appServices = NW_LMgr_RootBox_GetAppServices (rootBox);
  softKeyAPI = &(appServices->softKeyApi);

  if (softKeyAPI->setLeft != NULL)
  {
    softKeyAPI->setLeft(NW_SOFTKEY_OK); // NW_SOFTKEY_OK parameter is here just because of API
  }
}

// ----------------------------------------------------------------------------
TBrowserStatusCode CView::InputInvalid(void* aCEpoc32View, TInt aMin)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    TRAPD(err, status = InputInvalidL(aCEpoc32View, aMin));
    if (err != KErrNone)
        return KBrsrFailure;
    return status;
    }
// ----------------------------------------------------------------------------
TBrowserStatusCode CView::InputInvalidL(void* aCEpoc32View, TInt aMin)
    {
    HBufC* message = NULL;

    if (aMin == 1)
        {
        // We only require one char
        message = StringLoader::LoadLC(R_TEXT_INPUT_REQ);
        }
    else
        {
        TBuf<8> aMinStr;
        aMinStr.Format(_L("%U"),aMin);
        // We require a minimum number of chars
        message = StringLoader::LoadLC(R_TEXT_MORE_INPUT_REQ,aMinStr);
        }

    TRAPD (err, ((CView*)aCEpoc32View)->iBrCtl->brCtlDialogsProvider()->DialogNoteL(message->Des()));

    // Clean up the memory
    CleanupStack::PopAndDestroy(1); // message

    return (( err == KErrNone) ? KBrsrSuccess : KBrsrFailure);
    }

// ----------------------------------------------------------------------------
TBrowserStatusCode
CView::InitializeTree (NW_LMgr_RootBox_t *rootBox, TBool isFormatingBoxTree)
  {
  TBrowserStatusCode status = KBrsrSuccess;

  // CView::IBoxTreeListener_FormattingComplete takes care of
  // the following clean-ups in case of background formatting
  if( !((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->BackgroundFormat() )
    {
    /* Clear the rendering cache */
    NW_LMgr_RootBox_InvalidateCache(rootBox);

    /* Clear the float context vectors */
    (void) NW_ADT_DynamicVector_Clear(rootBox->placedFloats);
    (void) NW_ADT_DynamicVector_Clear(rootBox->pendingFloats);
    (void) NW_ADT_DynamicVector_Clear(rootBox->positionedAbsObjects);
    rootBox->iPositionedBoxesPlaced = NW_FALSE;

    /* We'll have to recalculate the extents */
    NW_Mem_memset(&(rootBox->extents), 0, sizeof(NW_GDI_Rectangle_t));
    NW_Mem_memset(&(rootBox->scrollRange), 0, sizeof(NW_GDI_Rectangle_t));
    NW_LMgr_RootBox_ExtendsCalculationNeeded(rootBox);
    NW_LMgr_RootBox_SetBody(rootBox, NULL);

    // set formatting bounds

    // Below four lines should go away once container box sets the height of rootbox
    // to be equal to its height.  Currently the height of rootbox is increased to
    // the amount equal to the initial height + the height of inner container box.
    // Where as it should be increased to be equal to the height of inner container box.
    NW_GDI_Rectangle_t displayBounds;

    displayBounds = *(iDeviceContext->DisplayBounds());
    displayBounds.point.x = 0;
    displayBounds.point.y = 0;
    displayBounds.dimension.height = 0;
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( rootBox ), displayBounds );
    }

  /* Set the font on the rootbox */
  status = NW_LMgr_Box_ApplyFontProps(NW_LMgr_BoxOf(rootBox));

  /* by default, the blink timer is off */
  rootBox->isBlinking = NW_FALSE;

  // Reset the formatbounds when the browser is in NSM or in SSM and it has grid mode tables.
  // The bounds need to be reset in order to correctly layout boxes in static cell boxes.
  if( !NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) || (rootBox->iHasGridModeTables &&
      isFormatingBoxTree))
    {
    NW_LMgr_BoxVisitor_t visitor;
    NW_LMgr_Box_t* child = NULL;

    NW_LMgr_BoxVisitor_Initialize(&visitor, NW_LMgr_BoxOf(rootBox));

    /* Iterate through all children */
    child = NW_LMgr_BoxVisitor_NextBox(&visitor, NULL);
    while ((child = NW_LMgr_BoxVisitor_NextBox(&visitor, NULL)) != NULL)
      {
      /* Initialize the coordinates */
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( child );

      NW_Mem_memset(&(boxBounds), 0, sizeof(NW_GDI_Rectangle_t));
      NW_LMgr_Box_SetFormatBounds( child, boxBounds );


      /* Clean up any flows */
            if (NW_Object_IsInstanceOf(child, &NW_LMgr_BidiFlowBox_Class))
                {
        status = ((CLMgr_BoxFormatHandler*)(iBoxFormatHandler))->InitializeBox(NW_LMgr_BidiFlowBoxOf(child));
        }

      if (NW_Object_IsClass(child, &NW_LMgr_ContainerBox_Class) ||
        NW_Object_IsClass(child, &NW_LMgr_ActiveContainerBox_Class))
        {
        status = NW_LMgr_ContainerBox_Initialize(NW_LMgr_ContainerBoxOf(child));
        }

      if (NW_Object_IsClass(child, &NW_LMgr_MarqueeBox_Class))
        {
        status = NW_LMgr_MarqueeBox_Initialize(NW_LMgr_MarqueeBoxOf(child));
        }
      if (status != KBrsrSuccess)
        break;
      }
    }

  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CView::SetDisplayExtents (NW_GDI_Rectangle_t* displayExtents)
{
  CGDIDeviceContext* deviceContext = GetDeviceContext ();

  if (!deviceContext)
  {
	  return KBrsrFailure;
  }

  deviceContext->SetDisplayBounds( displayExtents);
  return KBrsrSuccess;
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::Collapse( NW_LMgr_FlowBox_t* containingFlow, NW_Bool collapseAnonymous )
  {
  return ((CLMgr_BoxFormatHandler*)(iBoxFormatHandler))->Collapse( containingFlow, collapseAnonymous );
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::FormatBoxTree(NW_Bool createTabList)
  {
  TBrowserStatusCode status = KBrsrSuccess;

  NW_LOG0( NW_LOG_LEVEL2, "CView::FormatBoxTree START" );

    // check to see if we have any children in our box tree, if not, then there is nothing to
    // format and we just want to return success.
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(iRootBox), 0);
    if( !child )
      {
      return KBrsrSuccess;
      }
    // cancel any format in progress
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->Cancel();

    // clear tablist
    if( createTabList )
      {
      (void) NW_ADT_DynamicVector_Clear(iTabList);
      }
    iCreateTabList = createTabList;

    // check if background format is needed
    status = SetupTableFormat (NW_TRUE);

    if( status != KBrsrNotFound )
      {
      // ignore KBrsrNotFound error as it indicates that there in no table in the page
      if (status != KBrsrSuccess)
        {
        return status;
        }
      }
    status = KBrsrSuccess;

    // Initialize the tree
    status = InitializeTree (iRootBox, ETrue);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // start formatting
    TRAPD(err, ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->PartialFormatL( iFormatPriority ));
    if( err != KErrNone )
      {
      return KBrsrFailure;
      }
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->PageLoadCompleted();

#ifdef _DEBUG_LOG
    // dump the box tree
    if (iRootBox)
      {
      NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
      }
#endif // DEBUG

    NW_LOG0( NW_LOG_LEVEL2, "CView::FormatBoxTree END" );
    return status;
  }

// -----------------------------------------------------------------------------
void
CView::DisableAnimation()
  {
  NW_LMgr_Box_t* child = NULL;
  NW_LMgr_BoxVisitor_t visitor;

  NW_LMgr_BoxVisitor_Initialize(&visitor, NW_LMgr_BoxOf(iRootBox));

  while( ( child = NW_LMgr_BoxVisitor_NextBox( &visitor, NULL) ) != NULL )
    {
	  if( NW_Object_IsInstanceOf( child, &NW_LMgr_AnimatedImageBox_Class ) == NW_TRUE )
      {
      NW_LMgr_AnimatedImageBox_Suspend (NW_LMgr_AnimatedImageBoxOf(child));
      }
    }
  }

// -----------------------------------------------------------------------------
void
CView::EnableAnimation()
  {
  NW_LMgr_Box_t* child = NULL;
  NW_LMgr_BoxVisitor_t visitor;

  NW_LMgr_BoxVisitor_Initialize(&visitor, NW_LMgr_BoxOf(iRootBox));

  while( ( child = NW_LMgr_BoxVisitor_NextBox( &visitor, NULL ) ) != NULL )
    {
	  if( NW_Object_IsInstanceOf( child, &NW_LMgr_AnimatedImageBox_Class ) == NW_TRUE )
      {
      NW_LMgr_AnimatedImageBox_Resume (NW_LMgr_AnimatedImageBoxOf(child));
      }
    }
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::SwitchToImageMapView ()
  {
  NW_LMgr_RootBox_t* newRootBox = NULL;
  NW_LMgr_Box_t* newImageMapBox = NULL;
  NW_LMgr_BidiFlowBox_t* newBidiFlowBox = NULL;
  LMgrAnonBlock_t* newAnonBlockBox = NULL;
  NW_AppServices_SoftKeyAPI_t* softKeyAPI = NULL;
  NW_LMgr_Property_t prop;
  TBrowserStatusCode status = KBrsrSuccess;

    // set the image map view flag to true
    iIsImageMapView = NW_TRUE;

    // disable any animation
    DisableAnimation();

    // set the current box as the "focus" box on the root box
    // this helps us save the state for when we switch back to the normal view
    NW_LMgr_RootBox_SetFocusBox(iRootBox, iCurrentBox);

    status = NW_LMgr_RootBox_Copy (iRootBox, &newRootBox);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // add the new root box to the root box array
    TRAPD( ret, iRootBoxArray->AppendL(newRootBox) );
    if( ret == KErrNoMemory )
      {
      return KBrsrOutOfMemory;
      }

    // set the "current" root box to be the new root box
    iRootBox = newRootBox;

    // change root box associated with the box format handler
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->SetRootBox(NW_LMgr_BoxOf(iRootBox));

    // set the box tree listener for the new root box (always returns success)
    status = NW_LMgr_RootBox_SetBoxTreeListener (iRootBox, this);

    // make sure the rootbox has the focus at the beginning
    NW_LMgr_RootBox_GainFocus(iRootBox);

    // create a BidiFlowBox in which to build the box tree
    newBidiFlowBox = NW_LMgr_BidiFlowBox_New (1);
    if (!newBidiFlowBox)
      {
      return KBrsrOutOfMemory;
      }

    // set the display property on the bidi flow box
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_display_block;
    NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf (newBidiFlowBox),
                             NW_CSS_Prop_display, &prop);

    prop.value.token = NW_CSS_PropValue_flags_bodyElement;
    status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf (newBidiFlowBox), NW_CSS_Prop_flags, &prop);

    prop.type = NW_CSS_ValueType_Px;
    prop.value.integer = 2;
    NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf(newBidiFlowBox), NW_CSS_Prop_margin, &prop);

    // create a new anon block that will contain our imagemap box since the image map cannot
    // exist outside a block
    newAnonBlockBox = LMgrAnonBlock_New(0);
    if (!newAnonBlockBox)
      {
      return KBrsrOutOfMemory;
      }

    // get the image map box by copying the current box
    status = NW_LMgr_ImageMapBox_Copy(iCurrentBox, &newImageMapBox);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // Add the image map box to the new anon container box
    status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(newAnonBlockBox),
                                           newImageMapBox);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // Insert the anon block box into the box tree
    status = NW_LMgr_ContainerBox_InsertChildAt(NW_LMgr_ContainerBoxOf(newBidiFlowBox),
                                                NW_LMgr_BoxOf(newAnonBlockBox),
                                                0);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // Set the display property on the anon block box
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_display_block;
    status = NW_LMgr_Box_SetProperty(NW_LMgr_BoxOf(newAnonBlockBox),
                                     NW_CSS_Prop_display, &prop);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // switch to normal screen mode
    NW_Settings_SetInternalVerticalLayoutEnabled( NW_FALSE );

    // set the current box to point to the new image map box
    iCurrentBox = newImageMapBox;

    // set the current box as the "focus" box on the root box
    NW_LMgr_RootBox_SetFocusBox(iRootBox, iCurrentBox);

    // set the box tree on the view
    status = SetBoxTree (NW_LMgr_BoxOf(newBidiFlowBox));
    if (status != KBrsrSuccess)
      {
      return status;
      }

    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->ClearFormattingContext();
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->Cancel();

    // this will actually perform switch to normal screen mode
    status = SwitchVerticalLayout (NW_TRUE, NW_TRUE);
    if (status != KBrsrSuccess)
      {
      return status;
      }

    // ping the softkeys api, it doesn't matter which key and
    // options we are setting, we just want to trigger CommandSetResourceIdL
    // off the BrowserContentView to be called
    // this will trigger the code that switches the changing of the right
    // key to Back when we are in image map mode
    softKeyAPI = &(iRootBox->appServices->softKeyApi);
    NW_ASSERT(softKeyAPI != NULL);

    if (softKeyAPI->setLeft != NULL)
      {
      softKeyAPI->setLeft(NW_SOFTKEY_OK); // this could be anything
      }
    // Inform all observers that returning from image map view
    // This is important because we need to inform Flash plugin to not show itself
    TInt count = iStateChangeObserver.Count();
    for (TInt i = 0; i < count; i++)
      {
      iStateChangeObserver[i]->StateChanged(TBrCtlDefs::EStateImageMapView,ETrue);
      }

  return status;
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::SwitchFromImageMapView ()
  {
  NW_AppServices_SoftKeyAPI_t* softKeyAPI = NULL;
  TBrowserStatusCode status = KBrsrSuccess;

    // set the image map view flag to false
    iIsImageMapView = NW_FALSE;

    // find the size of the root box array
    TInt size = iRootBoxArray->Count();
    NW_ASSERT (size == 2);

    // retrieve the original root box (index position should be 0)
    iRootBox =  iRootBoxArray->At(0);
    NW_ASSERT(iRootBox);

     // change root box associated with the box format handler
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->SetRootBox(NW_LMgr_BoxOf(iRootBox));

    // set the box tree listener for the original root box // always returns success
    status = NW_LMgr_RootBox_SetBoxTreeListener (iRootBox, this);

    // make sure the rootbox has the focus at the beginning
    NW_LMgr_RootBox_GainFocus(iRootBox);

    NW_LMgr_Box_t* boxTree = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(iRootBox), 0);
    NW_ASSERT(boxTree);

    // set the current box to point to the focused box in the box tree
    iCurrentBox = NW_LMgr_RootBox_GetFocusBox(iRootBox);

    // set the focus box to NULL
    NW_LMgr_RootBox_SetFocusBox(iRootBox, NULL);

    // Check to see the status of the external layout switch flag because it
    // is possible that the user changed the layout mode while in the
    // image map view
    if (iExternalLayoutSwitch)
      {
      // if the user changed the layout, call switch vertical layout to reformat the box tree
      // and make sure that we switch tables as well
      status = SwitchVerticalLayout (NW_TRUE, NW_TRUE);
      if (status != KBrsrSuccess)
        {
        return status;
        }
      }
    else
      {
      // if the user hasn't changed the layout, switch back to vertical layout mode
      NW_Settings_SetInternalVerticalLayoutEnabled( NW_TRUE );

      // this will actually perform switch to the vertical layout mode, but since we
      // are passing in NW_FALSE for the third parameter, we will not switch the tables
      status = SwitchVerticalLayout (NW_TRUE, NW_FALSE);
      if (status != KBrsrSuccess)
        {
        return status;
        }
      }

    // remove and delete the image map box tree
    NW_LMgr_RootBox_t* oldRootBox = iRootBoxArray->At( size - 1 );
    if (!oldRootBox)
      {
      return KBrsrFailure;
      }

    NW_LMgr_Box_t* oldBoxTree = NW_LMgr_RootBox_GetFocusBox(oldRootBox);
    if (!oldBoxTree)
      {
      return KBrsrFailure;
      }

    // delete image map box tree
    NW_Object_Delete (oldBoxTree);
    oldBoxTree = NULL;

    // delete the root box
    iRootBoxArray->Delete( size - 1 );
    NW_Object_Delete (oldRootBox);

    // ping the softkeys api, it doesn't matter which key and
    // options we are setting, we just want to trigger CommandSetResourceIdL
    // off the BrowserContentView to be called
    // this will trigger the code that switches the changing of the right
    // key what ever is appropriate when we are in normal mode
    softKeyAPI = &(iRootBox->appServices->softKeyApi);
    NW_ASSERT(softKeyAPI != NULL);

    if (softKeyAPI->setLeft != NULL)
      {
      softKeyAPI->setLeft(NW_SOFTKEY_OK); // this could be anything
      }

    iExternalLayoutSwitch = NW_FALSE;

    // enable any animation
    EnableAnimation();

    // Inform all observers that returning from image map view
    // This is important because we need to inform Flash plugin to show itself again
    TInt count = iStateChangeObserver.Count();
    for (TInt i = 0; i < count; i++)
      {
      iStateChangeObserver[i]->StateChanged(TBrCtlDefs::EStateImageMapView, EFalse);
      }

  return status;
  }

// -----------------------------------------------------------------------------
void
CView::ExternalLayoutSwitch ()
  {
  if (iIsImageMapView)
    {
    iExternalLayoutSwitch = NW_TRUE;
    }
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::SetOrigin (NW_GDI_Point2D_t* origin)
{
  const NW_GDI_Dimension2D_t* displaySize;
  NW_GDI_Dimension2D_t canvasSize;
  NW_GDI_Dimension2D_t scrollRange;
  NW_GDI_Point2D_t oldOrigin;

  // get the display and canvas size
  displaySize =
    &(iDeviceContext->DisplayBounds()->dimension);
  TBrowserStatusCode status = NW_LMgr_RootBox_GetExtents (iRootBox, &canvasSize);
  NW_ASSERT (status == KBrsrSuccess);

  /* make sure that the speicified origin does not extend us beyond the
     canvas size */
  if (origin->x + displaySize->width > canvasSize.width)
  {
    origin->x = (NW_GDI_Metric_t) (canvasSize.width - displaySize->width);
  }


  if (origin->x < 0)
  {
    origin->x = 0;
  }

  NW_LMgr_RootBox_GetScrollRange(NW_LMgr_RootBoxOf(iRootBox), &scrollRange);
  if (((canvasSize.width - origin->x) > scrollRange.width) &&
    NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBoxOf(iRootBox)))
  {
    origin->x = canvasSize.width - scrollRange.width;
  }

  if (origin->y + displaySize->height > canvasSize.height)
  {
    origin->y = (NW_GDI_Metric_t) (canvasSize.height - displaySize->height);
  }

  if (origin->y < 0)
  {
    origin->y = 0;
  }

  oldOrigin = *(iDeviceContext->Origin());

  // set the origin of the DeviceContext
  if ((oldOrigin.x != origin->x) || (oldOrigin.y != origin->y))
  {
    NW_LMgr_RootBox_InvalidateCache(iRootBox);
    iDeviceContext->SetOrigin (origin);
    // update the rootBox about the change in orgin)
    NW_LMgr_RootBox_OriginChanged(iRootBox, (NW_GDI_Point2D_t*)(&oldOrigin));
  }
  return KBrsrSuccess;
}

// ------------------------------------------------------------------------- 
// box is a flow box, go ahead formatting the subtree of the box 
//
// take out CView::Draw function call from ReformatBox function;
// using NW_LMgr_IBoxTreeListener_RedrawDisplay instead, reason:
// imageCH calls ReformatBox for each virtual image; 
// in turn CView::Draw is called for each virtual image if CView::Draw is called inside ReformatBox;
// shell will control this situation if many View_Draw msg sent
// thus CView::Draw called once for a group of virtual images which shares one simple image
// ------------------------------------------------------------------------- 
TBrowserStatusCode
CView::ReformatBox (NW_LMgr_Box_t* box)
    {
    
    NW_LMgr_BoxVisitor_t visitor;
    NW_LMgr_Box_t* child = NULL;
    NW_LMgr_Box_t* parentBox = NULL;
    NW_LMgr_Box_t* flowBox = NULL;
    NW_GDI_Rectangle_t childBounds;
    NW_GDI_Rectangle_t flowBounds;
    NW_GDI_Metric_t oldHeight;
    NW_GDI_Metric_t newHeight;
    NW_ADT_DynamicVector_t* backupPlacedFloats = NULL;
    NW_ADT_DynamicVector_t* backupPendingFloats = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    NW_Bool createTabList;
    CLMgr_BoxFormatHandler* tempBoxFormatHandler= NULL;
    TInt ret = KErrNone;
    
    // We try to find out the containing flow box */
    parentBox = box;

	//if the parent is a bidiflowbox, we continue to check if there is still tabel cell box as a parent, if not, 
	//we still keep this bidiflowbox as parent
	NW_LMgr_Box_t* returnedParentBox = NULL;
	GetParentBox(parentBox, &returnedParentBox);
	if(returnedParentBox != NULL) {
		parentBox = returnedParentBox;
	}

    /* The parent box should be an instance of the flow box */
    flowBox = parentBox;
    NW_ASSERT (NW_Object_IsInstanceOf(flowBox, &NW_LMgr_FlowBox_Class));
    
    /*
    ** iSuppressDupReformats is true when a virtual image with more than
    ** one image observer is notifying its observers that it has been "opened".
    ** When iPendingReformatBox matches this box's parent then it can safely 
    ** skip this reformat because this box will be reformatted when 
    ** iPendingReformatBox is formatted -- either the box has a different parent-
    ** flow-box or the box is associated with the last image observer.
    **
    ** This helps speed up the special case when an image is duplicated many times
    ** within the same document.
    */
    if(iSuppressDupReformats != NW_Image_AbstractImage_SuppressDupReformats_Default)
        {
        if (iPendingReformatBox != flowBox && iPendingReformatBox != box)
            {
            // If flowBox is inside iPendingReformatBox, then forget flowBox and format only iPendingFormatBox
            NW_LMgr_Box_t* parent =(NW_LMgr_Box_t*) NW_LMgr_Box_GetParent(flowBox);
            while (parent != NULL && parent != iPendingReformatBox)
                {
                parent = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(parent);
                }
            if (iPendingReformatBox && parent == iPendingReformatBox)
                {
                iSuppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Stop;
                flowBox = iPendingReformatBox;
                iPendingReformatBox = NULL;
                }
            else
                {
                if (iPendingReformatBox)
                    {
                    NW_Int16 state = iSuppressDupReformats;
                    iSuppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Stop;
                    ReformatBox (iPendingReformatBox);
                    iSuppressDupReformats = state;
                    }
                iPendingReformatBox = flowBox;
                }
            }
        
        
        if (iSuppressDupReformats != NW_Image_AbstractImage_SuppressDupReformats_Stop)
            goto finish;    
        
        iSuppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Default;
        
        }
    
    flowBounds = NW_LMgr_Box_GetFormatBounds(flowBox);
    
    // get the current height of the flow box since after formatting the 
    // height will change
    oldHeight = flowBounds.dimension.height;
    
    // collapse the contents of this flow box, since it will be reformatted
    status = Collapse (NW_LMgr_FlowBoxOf(flowBox), NW_FALSE);
    NW_LOG0(NW_LOG_LEVEL2, "CView::Collapse passed");
    if (status != KBrsrSuccess)
        {
        goto finish;    
        }
    
    /* Initialize the visitor to flowBox */
    NW_LMgr_BoxVisitor_Initialize(&visitor, flowBox);
    
    child = NW_LMgr_BoxVisitor_NextBox(&visitor, 0);
    
    /* Set the font on the rootbox */
    status = NW_LMgr_Box_ApplyFontProps(NW_LMgr_BoxOf(child));
    NW_LOG0(NW_LOG_LEVEL2, "NW_LMgr_Box_ApplyFontProps passed");
    if (status != KBrsrSuccess)
        {
        goto finish;    
        }
    
    // floats needs to be removed from the rootbox while the re-layout is 
    // formatting the subtree as the formatting code cannot cope with
    // the case where only a subtree is being formatted but the 
    // float lists include boxes from the entire tree.
    // This is a quick fix on 2.1 showstopper and it has to be 
    // reviewed and find a better solution (fixing the format code)
    
    // save placed and pending floats
    backupPlacedFloats = iRootBox->placedFloats;
    iRootBox->placedFloats = (NW_ADT_DynamicVector_t*)
        NW_ADT_ResizableVector_New(sizeof (NW_LMgr_Box_t*), 0, 5 );
    if (!iRootBox->placedFloats)
        {
        status = KBrsrOutOfMemory;
        goto finish;
        }
    
    backupPendingFloats = iRootBox->pendingFloats;
    iRootBox->pendingFloats = (NW_ADT_DynamicVector_t*)
        NW_ADT_ResizableVector_New(sizeof (NW_LMgr_Box_t*), 0, 5 );
    if (!iRootBox->pendingFloats)
        {
        status = KBrsrOutOfMemory;    
        goto finish;
        }
    
    while ((child = NW_LMgr_BoxVisitor_NextBox(&visitor,0)) != NULL) 
        {
        if (box != NULL && NW_Object_IsInstanceOf(child, &NW_LMgr_PosFlowBox_Class))
            {
            NW_Bool skip = NW_TRUE;
            child = NW_LMgr_BoxVisitor_NextBox( &visitor, &skip);
            if (child == NULL)
                {
                break;
                }
            }
        /* Initialize the coordinates */
        NW_Mem_memset( &childBounds, 0, sizeof(NW_GDI_Rectangle_t));
        
        SetBoxFormatBounds( child, childBounds );
        NW_LMgr_Box_UpdateDisplayBounds( child, childBounds );
        /* Clean up any flows */
        if (NW_Object_IsInstanceOf(child, &NW_LMgr_BidiFlowBox_Class) ||
            NW_Object_IsClass(child, &NW_LMgr_StaticTableCellBox_Class)) 
            {        
            status = ((CLMgr_BoxFormatHandler*)(iBoxFormatHandler))->InitializeBox(NW_LMgr_BidiFlowBoxOf(child));
            if (status != KBrsrSuccess)
                {
                goto finish;    
                }
            }
        
        if ( !NW_Object_IsInstanceOf(box, &NW_LMgr_PosFlowBox_Class) &&
			 (NW_Object_IsClass(child, &NW_LMgr_ContainerBox_Class) ||
			  NW_Object_IsClass(child, &NW_LMgr_ActiveContainerBox_Class) ) )
            {
            status = NW_LMgr_ContainerBox_Initialize(NW_LMgr_ContainerBoxOf(child));
            if (status != KBrsrSuccess)
                {
                goto finish;    
                }
            }
        }
    NW_LOG0(NW_LOG_LEVEL2, "child = NW_LMgr_BoxVisitor_NextBox passed");
    /* because of KBrsrLmgrFormatOverflow, don't check on status;
    * is this ok for SSL?
    */

    // create a boxFormatHandler and format the flowBox
    TRAP(ret, tempBoxFormatHandler = CLMgr_BoxFormatHandler::NewL(flowBox, ETrue));
    if (!tempBoxFormatHandler)
        {
        status = KBrsrOutOfMemory;    
        goto finish;
        }
    
#ifdef _DEBUG_LOG
    //#pragma message(__LINE__" WARNING: The following code crashes on a sar segment test of 10Kb or greater.\n")
    /* dump the box tree */
    NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  ReformatBox method: before PartialFormatL]");
    NW_LMgr_Box_DumpBoxTree (flowBox);
#endif 
    
    // we do not want the tab list to be appended on selective formatting
    createTabList = iCreateTabList ;
    iCreateTabList  = NW_FALSE;
    TRAPD(ret1,tempBoxFormatHandler->PartialFormatL( iFormatPriority ));
    // set the flag 
    iCreateTabList = createTabList;
    NW_LOG0(NW_LOG_LEVEL2, "PartialFormatL passed");
    if (ret1 != KErrNone)
        {
        status = KBrsrFailure;
        }
    tempBoxFormatHandler->PageLoadCompleted();
    NW_LOG0(NW_LOG_LEVEL2, "PageLoadCompleted passed");
    delete tempBoxFormatHandler;
    
    if (status != KBrsrSuccess)
        {
        goto finish;    
        }
    
#ifdef _DEBUG_LOG
    /* dump the box tree */
    NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  ReformatBox method: after PartialFormatL]");
    NW_LMgr_Box_DumpBoxTree (flowBox);
#endif 
    
    flowBounds = NW_LMgr_Box_GetFormatBounds(flowBox);
    newHeight = flowBounds.dimension.height;
    
    // If we are formatting a positioned box - do not shift the rest of the tree
    // If we are formatting an image inside a positioned box, shift only the sub-tree up to the positioned box
    // If we are formatting an image that is a sibling of a positioned box - skip the positioned box when shifting the box tree
    if (oldHeight != newHeight && (!NW_Object_IsInstanceOf(flowBox, &NW_LMgr_PosFlowBox_Class)))
        {
        // Now we want to adjust heigtht of all parent boxes affected 
        parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(flowBox);
        
        while (parentBox != NULL && !NW_Object_IsInstanceOf(parentBox, &NW_LMgr_PosFlowBox_Class))
            {
            NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( parentBox );
            if (oldHeight < newHeight)
                {
                boxBounds.dimension.height = (NW_GDI_Metric_t) (boxBounds.dimension.height +
                    (newHeight - oldHeight));
                } 
            else
                {
                boxBounds.dimension.height = (NW_GDI_Metric_t)(boxBounds.dimension.height -
                    (oldHeight - newHeight));
                }
            NW_LMgr_Box_SetFormatBounds( parentBox, boxBounds );
            NW_LMgr_Box_UpdateDisplayBounds( parentBox, boxBounds );
            parentBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(NW_LMgr_BoxOf(parentBox));
            } 
        
        // update rootbox 
        iRootBox->calculateExtents = NW_TRUE;
        
        if (!NW_Object_IsInstanceOf(parentBox, &NW_LMgr_PosFlowBox_Class))
            {
            // Now we want to adjust the y parameter of all the lower boxes 
        
            // Initialize the Box Visitor to current FlowBox
            NW_LMgr_BoxVisitor_Initialize(&visitor, NW_LMgr_BoxOf(iRootBox));
            visitor.currentBox = flowBox;
            // Skip children of this Flow Box and go to the next box in the tree
            NW_Bool skipChildren = NW_TRUE;
            child = NW_LMgr_BoxVisitor_NextBox(&visitor,&skipChildren);
            while (child != NULL) 
                {
                // Do not shift nested absolute positioned boxes
                if (NW_Object_IsInstanceOf(child, &NW_LMgr_PosFlowBox_Class))
                    {
                    NW_Bool skip = NW_TRUE;
                    child = NW_LMgr_BoxVisitor_NextBox(&visitor, &skip);
                    continue;
                    }
                childBounds = NW_LMgr_Box_GetFormatBounds(child);
            
                if (oldHeight < newHeight)
                    {
                    childBounds.point.y = (NW_GDI_Metric_t)
                        (childBounds.point.y + (newHeight - oldHeight));
                    }
                else 
                    {
                    childBounds.point.y = (NW_GDI_Metric_t)
                        (childBounds.point.y - (oldHeight - newHeight));
                    }
                SetBoxFormatBounds( child, childBounds );
                NW_LMgr_Box_UpdateDisplayBounds( child, childBounds );
            
                child = NW_LMgr_BoxVisitor_NextBox(&visitor,0);
                }
            }
        else
            {
            // If the vertical position was specified from the bottom, the positioned sub-tree
            // has to be adjusted, to maintain the correct bottom position
            if (NW_LMgr_PosFlowBoxOf(parentBox)->iHasBottom)
                {
                // Initialize the Box Visitor to current FlowBox
                NW_LMgr_BoxVisitor_Initialize(&visitor, NW_LMgr_BoxOf(flowBox));
                // Skip children of this Flow Box and go to the next box in the tree
                NW_Bool skipChildren = NW_FALSE;
                child = NW_LMgr_BoxVisitor_NextBox(&visitor,&skipChildren);
                while (child != NULL) 
                    {
                    // Do not shift nested absolute positioned boxes
                    if (NW_Object_IsInstanceOf(child, &NW_LMgr_PosFlowBox_Class))
                        {
                        NW_Bool skip = NW_TRUE;
                        child = NW_LMgr_BoxVisitor_NextBox(&visitor, &skip);
                        continue;
                        }
                    childBounds = NW_LMgr_Box_GetFormatBounds(child);
            
                    childBounds.point.y = (NW_GDI_Metric_t)
                            (childBounds.point.y - (newHeight - oldHeight));
                    SetBoxFormatBounds( child, childBounds );
                    NW_LMgr_Box_UpdateDisplayBounds( child, childBounds );
                    child = NW_LMgr_BoxVisitor_NextBox(&visitor, &skipChildren);
                    }
                }
            }
        }
    NW_LOG0(NW_LOG_LEVEL2, "height update passed");
    
    // DrawNow msg sent
    (void)RedrawDisplay( NW_FALSE );
    
#ifdef _DEBUG_LOG
    /* dump the box tree */
    NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  End ReformatBox method]");
    NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
#endif 
    
finish:
    // restore both placed and pending floats ( see comments above)    
    if( backupPlacedFloats )
        {
        NW_Object_Delete( iRootBox->placedFloats);
        iRootBox->placedFloats = backupPlacedFloats;
        }
    
    if( backupPendingFloats )
        {
        NW_Object_Delete( iRootBox->pendingFloats);
        iRootBox->pendingFloats = backupPendingFloats;
        }
    
    /* successful completion */
    NW_LOG0(NW_LOG_LEVEL2, "CView::ReformatBox passed");
    return status;    
    }


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CView::FormatPartial ()
  {

  TBrowserStatusCode status =KBrsrSuccess;
  NW_LOG0(NW_LOG_LEVEL2, "CView::FormatPartial START");

  //TODO handle leave
  TRAPD(err, ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->PartialFormatL( iFormatPriority ));
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( NW_LMgr_BoxOf( iRootBox ) );

  iRootBox->extents.dimension.height = boxBounds.dimension.height;
  UpdateScrollBars ();
  NW_LOG0(NW_LOG_LEVEL2, "PartialFormatL passed");
  if(err != KErrNone)
    {
    status = KBrsrFailure;
    }
  NW_LOG0(NW_LOG_LEVEL2, "CView::FormatPartial END");
  return status;
  }

/* ------------------------------------------------------------------------- */
/* return NULL in non-split case
*/
NW_LMgr_Box_t*
CView::GetNextSiblingBox (const NW_LMgr_Box_t* box)
{
  NW_LMgr_Box_t *siblingBox = NULL;
  NW_LMgr_PropertyList_t* propList;
  NW_LMgr_Property_t prop;

  NW_ASSERT(box != NULL);

  propList = NW_LMgr_Box_PropListGet(box);
  if( propList )
  {
      prop.value.object = NULL;
      (void)NW_LMgr_PropertyList_Get(propList, NW_CSS_Prop_sibling, &prop);
      if(prop.value.object != NULL)
	  {
          siblingBox = NW_LMgr_BoxOf(prop.value.object);
            }
        else
      /* If it's null, it means non-split case */
	  {
	    siblingBox = NULL;
	  }
        }
    else
  {
    siblingBox = NULL;
  }

  return siblingBox;
}

/* ------------------------------------------------------------------------- *
   public/protected static methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
NW_LMgr_Box_t*
CView::GetNextEventListener (NW_LMgr_BoxVisitor_t* boxVisitor)
{
  NW_LMgr_Box_t* box = NULL;
  NW_GDI_Rectangle_t boxBounds;

  while ((box = NW_LMgr_BoxVisitor_NextBox (boxVisitor, NULL)) != NULL)
  {
    NW_LMgr_IEventListener_t* eventListener;

    /* we are only looking for boxes implementing the NW_LMgr_IEventListener
       interface */
      eventListener = static_cast<NW_LMgr_IEventListener_t*>(NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class));
        if (eventListener == NULL)
            {
      continue;
    }

    /* If activeContainerBox has zero length or zero width, we skip this
       activeContainerBox. This happens when activeContaninerBox only has
       one breakBox child */
    if (NW_Object_IsInstanceOf (box, &NW_LMgr_ActiveContainerBox_Class))
    {
      boxBounds = NW_LMgr_Box_GetDisplayBounds(box);
      if (boxBounds.dimension.width == 0 || boxBounds.dimension.height == 0)
      {
        continue;
      }
    }
    return (box);
  }
  return (NULL);
}

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */
// -----------------------------------------------------------------------------
// CView::CView
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CView::CView (CBrCtl* aBrCtl) :
  iVScrollPosition(0), iFormatPriority( ((CActive::TPriority)( CActive::EPriorityLow + 2 )) )
{
    iBrCtl = aBrCtl;
    iDrag = EFalse;
}

// -----------------------------------------------------------------------------
// CView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CView::ConstructL (CCoeControl* aParent, TRect& aRect, CWmlControl* aWmlControl )
{
  static const NW_GDI_Metric_t sVScrollAmt = 18;
  static const NW_GDI_Metric_t sHScrollAmt = 18;
  static const NW_GDI_Metric_t sVVisibleAmt = 10;
  static const NW_GDI_Metric_t sHVisibleAmt = 10;
  CGDIDeviceContext* deviceContext = NULL;
  void* graphicsContext = NULL;
  NW_GDI_Rectangle_t displayBounds = { {0,0},{0,0} };
  NW_HED_AppServices_t* appServices = NULL;

  CreateWindowL( aParent );
  SetMopParent( aParent );

  iWmlControl = aWmlControl;

  // create a bitmap to be used offscreen
  CreateOffscreenBitmapL( Rect() );

  // create root box array
  iRootBoxArray = new (ELeave) CArrayPtrFlat<NW_LMgr_RootBox_t>(2);

  iSuppressDupReformats = NW_Image_AbstractImage_SuppressDupReformats_Default;
  // create the rootBox
  iRootBox = NW_LMgr_RootBox_New (appServices);
  User::LeaveIfNull(iRootBox);

  iRootBoxArray->AppendL(iRootBox);
  // observer small screen and text wrap settings
  //CBrowserSettings::Instance()->SetObserver( this,
   // EVerticalLayoutEnabled|EDisableSmallScreenLayout|ETextWrapEnabled );

  iBoxFormatHandler = (void*)CLMgr_BoxFormatHandler::NewL(NW_LMgr_BoxOf(iRootBox ), EFalse);

  (void) NW_LMgr_RootBox_SetBoxTreeListener (iRootBox, this);

  /* Make sure the rootbox has the focus at the beginning */
  NW_LMgr_RootBox_GainFocus(iRootBox);

  iTabList = (NW_ADT_DynamicVector_t*)
                     NW_ADT_ResizableVector_New (sizeof(NW_LMgr_Box_t), 10, 5 );
  iOptionItemList = NULL;

  /* Deleted in destruct */
  iScroll = CMVCScroll::NewL(this);

  iMaxScrollAmt.width = 0;
  iMaxScrollAmt.height = 0;
  iScrollAmt.width = sHScrollAmt;
  iScrollAmt.height = sVScrollAmt;
  iVisibleAmt.width = sHVisibleAmt;
  iVisibleAmt.height = sVVisibleAmt;

  iCreateTabList = NW_FALSE;

  iIsImageMapView = NW_FALSE;
  iExternalLayoutSwitch = NW_FALSE;

  // create the deviceContext
  graphicsContext = iBitmapContext;
  deviceContext = CGDIDeviceContext::NewL (graphicsContext, this);
  // ignore err
  NW_ASSERT ( deviceContext!= NULL);

  /* set bounds */
  deviceContext->SetDisplayBounds( &displayBounds);

  /* set the DeviceContext on ourselves */
  TBrowserStatusCode status = CView::SetDeviceContext (deviceContext);
  NW_ASSERT (status == KBrsrSuccess);

  // create SharedDataClient and ask for a couple of notifications
  iRepository = CRepository::NewL(KCRUidCacheManager);
  iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iRepository, CCenRepNotifyHandler::EIntKey, (TUint32)KCacheManagerHttpCacheEnabled);
  iNotifyHandler->StartListeningL();

  if (FeatureManager::FeatureSupported(KFeatureIdJapanesePicto))
    {
      iPictographInterface = CAknPictographInterface::NewL( *this, *this );
    }
  else
    {
      iPictographInterface = NULL;
    }
  iViewAsyncInit = CViewAsyncInit::NewL();

  InitializePageScalerL();

  SetRect(aRect);

  if (AknLayoutUtils::PenEnabled())
    {
    DrawableWindow()->SetPointerGrab(ETrue);
    EnableDragEvents();
    }
  
  ActivateL();
  }

// -----------------------------------------------------------------------------
// CView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CView* CView::NewL (CCoeControl* aParent,
                             CBrCtl* aBrCtl, TRect& aRect, CWmlControl* aWmlControl )
{
	CView* epocView = new (ELeave) CView(aBrCtl);

	CleanupStack::PushL(epocView);
	epocView->ConstructL(aParent, aRect, aWmlControl );
	CleanupStack::Pop(); // epocView
	return epocView;
}

//-----------------------------------------------------------------------------
// Destructor
CView::~CView()
{
  // remove observer
  //CBrowserSettings::Instance()->RemoveObserver();

  if(iNotifyHandler)
  {
    iNotifyHandler->StopListening();
  }
  delete iNotifyHandler;
  delete iRepository;
  if (iAnchorName != NULL)
        {
		NW_Object_Delete(iAnchorName);
		iAnchorName = NULL;

		}
  if (iScroll != NULL)
  {
    delete iScroll;
    iScroll = NULL;
  }

  if (iPageScaler != NULL)
    {
    delete iPageScaler;
    iPageScaler = NULL;
    }
    
  delete (CLMgr_BoxFormatHandler*)iBoxFormatHandler;

  if (iRootBoxArray != NULL)
    {
    for (TInt i = 0; i < iRootBoxArray->Count(); i++)
      {
      NW_LMgr_RootBox_t* rootBox = iRootBoxArray->At(i);
      NW_ASSERT(rootBox);

      NW_Object_Delete(rootBox);
      }
    iRootBoxArray->Reset();
    delete iRootBoxArray;
    }

  delete iDeviceContext;
  NW_Object_Delete (iTabList);
  DeleteOptionItems ();

  delete iBitmapContext;
  delete iBitmapDevice;
  delete iOffscreenBitmap;
  delete iPictographInterface;
  iViewFocusObserver.Reset();
  iStateChangeObserver.Reset();

  if (iViewAsyncInit)
    {
    iViewAsyncInit->Cancel();
    delete iViewAsyncInit;
    }

  NW_Cleanup();

	CloseSTDLIB();
}

/* ------------------------------------------------------------------------- *
   methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::SetBoxTree (const NW_LMgr_Box_t* boxTree)
  {
  TBrowserStatusCode status = KBrsrSuccess;
  TBool errOccur = ETrue;
  NW_GDI_Point2D_t origin;

    NW_LOG0( NW_LOG_LEVEL2, "CView::SetBoxTree START" );

    /* remove any pre-existing box tree */
    if (boxTree == NULL)
      {
	  iDocumentFinished = EFalse;
	  iPageScaler->DocumentStarted();

      NW_LOG0( NW_LOG_LEVEL2, "CView::CreateTabIndexList boxTree == NULL" );
      /* unset the currentBox */
      iCurrentBox = NULL;
      // we do this only for WML or the first chunk
      if (iOptionItemList != NULL)
        {
        DeleteOptionItems ();
        }
      (void) NW_ADT_DynamicVector_Clear (iTabList);
      iCreateTabList = NW_TRUE;

      /* reset the DeviceContext origin */
      NW_Mem_memset (&origin, 0, sizeof origin);
      iDeviceContext->SetOrigin (&origin);

      /* remove any pre-existing box tree */
      status =
        NW_LMgr_ContainerBox_RemoveChildren (NW_LMgr_ContainerBoxOf (iRootBox));
      NW_ASSERT (status == KBrsrSuccess);
      NW_LMgr_RootBox_SetBody(iRootBox, NULL);
      }

    // get the document box tree and insert it into the RootBox
    if(boxTree != NULL && NW_LMgr_Box_GetParent (boxTree) == NULL)
      {
      status =
        NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf (iRootBox),
        NW_LMgr_BoxOf(boxTree));
      NW_ASSERT (status == KBrsrSuccess);
      }

    NW_LMgr_Box_t* rootBox = NW_LMgr_BoxOf (iRootBox);
    NW_ASSERT(rootBox);
    if (rootBox) {} // fix warning

    NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  Before box tree formatting... in CView::SetBoxTree()]");
#ifdef _DEBUG_LOG

    NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
#endif

    /* different formatting for partial render */
    if ( boxTree == NULL || !iRootBox->partialRender ||
      ( iRootBox->partialRender && iRootBox->renderingState == NW_LMgr_RootBox_RenderFirstChunk ) )
      {
      NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  Start box tree formatting...]");
      // Initialize the tree
      status = InitializeTree (iRootBox, EFalse);
      NW_LOG0( NW_LOG_LEVEL2, "CView::InitializeTree passed" );
      if (status != KBrsrSuccess)
        {
        goto finish;
        }

      // Do the layout
      if( boxTree )
        {
        status = SetupTableFormat (NW_FALSE);
        NW_LOG0( NW_LOG_LEVEL2, "SetupTableFormat passed" );

        if( status != KBrsrNotFound )
          {
          // ignore KBrsrNotFound error as it indicates that there in no table in the page
          if (status != KBrsrSuccess)
            {
            goto finish;
            }
          }

        status = KBrsrSuccess;

        if( iOptionItemList )
          {
          DeleteOptionItems ();
          }
        (void) NW_ADT_DynamicVector_Clear( iTabList );
        iCreateTabList = NW_TRUE;
        // clear any prev formatting context for the first chunk
        ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->ClearFormattingContext();
        TRAPD( err ,((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->PartialFormatL( iFormatPriority ));

        NW_LOG0( NW_LOG_LEVEL2, "PartialFormatL passed" );

        if(err != KErrNone)
          {
          status = KBrsrFailure;
          goto finish;
          }
        }
      NW_ASSERT(status == KBrsrSuccess);
#ifdef _DEBUG_LOG
      /* dump the box tree */
      NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  End box tree formatting... in CView::SetBoxTree()]");
      NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
#endif

      }
    else
      {
      FormatPartial();
      NW_LOG0( NW_LOG_LEVEL2, "CView::FormatPartial passed" );

#ifdef _DEBUG_LOG
      /* dump the box tree */
      NW_LOG0(NW_LOG_LEVEL3, "[LMgr:  End box tree partial formatting...]");
      NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
#endif

      } // end of if ( boxTree == NULL ||
    /* when markupComplete, has to call partial Format again */
    if (iRootBox->renderingState ==  NW_LMgr_RootBox_RenderComplete)
      {
      NW_LMgr_RootBox_InvalidateCache(iRootBox);
      ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->PageLoadCompleted();
      NW_LOG0( NW_LOG_LEVEL2, "PageLoadCompleted passed" );
      }

      errOccur = EFalse;

finish:
      if (errOccur)
        {
        HandleError (status);
        if (status != KBrsrOutOfMemory)
          {
          status = KBrsrUnexpectedError;
          }
        }

      NW_LOG0( NW_LOG_LEVEL2, "CView::SetBoxTree END" );

      return status;
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::Draw (const NW_Bool drawNow)
{
  NW_GDI_Rectangle_t oldClip;
  NW_GDI_Color_t oldBgColor = NW_GDI_Color_White;
  NW_GDI_Color_t bgColor = NW_GDI_Color_White;  /* White is the default background */
  NW_LMgr_Property_t bodyColor;
  NW_LMgr_Box_t *bodyBox = NULL;
  NW_GDI_PaintMode_t oldPaintMode;
  TBrowserStatusCode status = KBrsrSuccess;

  if(NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(iRootBox)) == 0)
  {
    UpdateScrollBars ();
    return KBrsrSuccess;
  }
  NW_LOG0( NW_LOG_LEVEL2, "CView::Draw START" );

    /* Get the body, if there is one */
  bodyBox = NW_LMgr_RootBox_GetBody(iRootBox);

  /* If there is a body element, does it have background-color set? */
  if (bodyBox != NULL)
  {
    bodyColor.type = NW_CSS_ValueType_Token;
    bodyColor.value.token = NW_CSS_PropValue_transparent;
    if (NW_LMgr_Box_GetProperty(bodyBox, NW_CSS_Prop_backgroundColor, &bodyColor) == KBrsrSuccess)
    {
      if (bodyColor.type == NW_CSS_ValueType_Color)
      {
      bgColor = bodyColor.value.integer;
    }
  }
}

  oldClip = iDeviceContext->ClipRect();
  oldBgColor = iDeviceContext->BackgroundColor();
  oldPaintMode = iDeviceContext->PaintMode();

    NW_GDI_Rectangle_t viewBounds;
    NW_GDI_Rectangle_t maxBounds;

    /* Set the clip */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());
    iDeviceContext->SetClipRect(&viewBounds);

    /* This is our initial clip */
    maxBounds.point.x = maxBounds.point.y = 0;
    maxBounds.dimension.width =  0x7fff;
    maxBounds.dimension.height = 0x7fffffff;

    iDeviceContext->SetBackgroundColor(bgColor);
    iDeviceContext->SetPaintMode(NW_GDI_PaintMode_Copy);
    /* clear the view */
    iDeviceContext->PreDraw();

    /* draw the box tree */
    if (iCurrentBox != NULL &&
        IsBoxVisible (iCurrentBox, &viewBounds))
        {
      /* if the current box is visible enough(at least several pixels),
         hilight it */
      status = NW_LMgr_Box_Render((NW_LMgr_Box_t*) iRootBox,
                                   iDeviceContext, &maxBounds,
                                   iCurrentBox, 0, NULL, NULL, iRootBox );
    }
    else
    {
      /* otherwise dehilight it */
      status = NW_LMgr_Box_Render((NW_LMgr_Box_t*) iRootBox,
                                   iDeviceContext, &maxBounds,
                                   NULL, 0, NULL, NULL, iRootBox );
    }
    if (status == KBrsrOutOfMemory)
      {
      goto finish;
      }
    status = DrawPositionedBoxes();
    if (status != KBrsrSuccess)
        {
        goto finish;
        }
    /* update the scroll bars */
    status = UpdateScrollBars ();

    /* invalidate the view */
    iDeviceContext->PostDraw (drawNow);

finish:
  iDeviceContext->SetClipRect(&oldClip);
  iDeviceContext->SetBackgroundColor(oldBgColor);
  iDeviceContext->SetPaintMode(oldPaintMode);

  NW_LOG0( NW_LOG_LEVEL2, "CView::Draw passed" );
  return KBrsrSuccess;
}

// -----------------------------------------------------------------------------
NW_Bool
CView::DispatchEvent (const NW_Evt_Event_t* event)
{
  NW_LMgr_Box_t* box = NULL;
  NW_GDI_Rectangle_t viewBounds;

    // Parameter assertion block
  NW_ASSERT (event != NULL);

    // Initialize the viewBounds
  viewBounds = *(iDeviceContext->DisplayBounds());
  viewBounds.point = *(iDeviceContext->Origin());

    // Starting at our current box, we must traverse the box hierarchy looking
    // for a box implementing the NW_LMgr_IEventListener interface that is
    // willing to absorb the event
    for (box = iCurrentBox; box != NULL; box = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(box))
        {
    NW_LMgr_IEventListener_t* eventListener;

        // Before we dispatch the activateEvent, do a few checks
        if (NW_Object_IsInstanceOf(event, &NW_Evt_ActivateEvent_Class))
            {
            // If the current box is not visible, then we do not dispatch
            // the activate event and simply return false.  This is a good
            // behaviour for small devices which do not have Tab and Enter keys.
            if (!IsBoxVisible(box, &viewBounds))
                {
                return NW_FALSE;
                }

            }

        if (NW_Object_IsInstanceOf(event, &NW_Evt_ActivateEvent_Class) ||
            NW_Object_IsInstanceOf(event, &NW_Evt_UnicodeKeyEvent_Class))
            {
            // If the current box is an InputBox and capabilities is set to
            // ECapabilityDisableInputAndPlugins, then do not dispatch the
            // event and simply return false
            if (NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class) &&
                (iBrCtl->capabilities() & TBrCtlDefs::ECapabilityDisableInputAndPlugins))
                {
      return NW_FALSE;
    }
            }


        // If the box implements IEventListener, allow it to process the event
    eventListener = static_cast<NW_LMgr_IEventListener_t*>(NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class));
        if (eventListener != NULL)
            {
      // The activate event is not forwarded to form boxes if a context switch
      // is in progress. This prevents the form from launching auxiliary
      // dialogs during a context switch.
      if (NW_Object_IsInstanceOf (event, &NW_Evt_ActivateEvent_Class) &&
                NW_Object_IsInstanceOf(box, &NW_FBox_FormBox_Class))
                {
        NW_HED_DocumentRoot_t* docRoot = NW_FBox_FormBoxUtils_GetDocRoot(NW_LMgr_BoxOf(box));
        NW_ASSERT(docRoot != NULL);
                if (NW_HED_DocumentRoot_IsContextSwitchPending(docRoot))
                    {
          return NW_FALSE;
        }
      }
            if (NW_LMgr_IEventListener_ProcessEvent (eventListener, event))
                {
        return NW_TRUE;
      }
    }
        }   // end of for (box = iCurrent...

    // The event was not absorbed by the boxTree
  return NW_FALSE;
}

// -----------------------------------------------------------------------------
// this function comes from old View.cpp
NW_Bool
CView::EpocProcessEvent (const NW_Evt_Event_t* event)
{
  NW_LMgr_ContainerBox_t* containerRootBox = NULL;

  // parameter assertion block
  NW_ASSERT (event != NULL);

  // * TODO:  if the event received causes focus to leave an input box, send
  // * a de-activate event to that input box before propagating the original
  // * event
  // *
  containerRootBox = NW_LMgr_ContainerBoxOf(iRootBox);

  if(NW_LMgr_ContainerBox_GetChildCount(containerRootBox) == 0)
    {
    return NW_FALSE;
    }

  // first try to pawn the event off on the boxTree
  if (DispatchEvent (event))
    {
    return NW_TRUE;
  }

  if (NW_Object_Core_GetClass (event) == &NW_Evt_TabEvent_Class)
    {
    return ProcessTabEvent (NW_Evt_TabEventOf (event));
    }
  else if (NW_Object_Core_GetClass (event) == &NW_Evt_ScrollEvent_Class)
    {
    return ProcessScrollEvent (NW_Evt_ScrollEventOf (event));
  }

  // we didn't absorb the event
  return NW_FALSE;
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::CreateTabIndexList ()
{
  NW_LMgr_BoxVisitor_t boxVisitor;
  NW_LMgr_Box_t* box;

  // we do this only for WML or the first chunk
   if (iOptionItemList != NULL)
   {
   DeleteOptionItems ();
  }

  /* initialize the boxVisitor */
  NW_LMgr_BoxVisitor_Initialize (&boxVisitor, NW_LMgr_BoxOf (iRootBox));
  // start the boxVistor at the last formatted box of the previous chunk

  while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL)
  {
    NW_LMgr_IEventListener_t* eventListener;

    // we are only looking for boxes implementing the NW_LMgr_IEventListener interface
    eventListener = static_cast<NW_LMgr_IEventListener_t*>(NW_Object_QueryInterface (box,
      &NW_LMgr_IEventListener_Class));

    if (!eventListener)
    {
      continue;
    }
    AppendTabItem (box);
  }
  return KBrsrSuccess;
}

// -----------------------------------------------------------------------------
NW_Bool
CView::ScrollAfterCurrentBox (const NW_LMgr_Box_t* box,
                              NW_Bool scrollMaxAmt)
    {
    NW_Int32 scrollDelta = 0;
    NW_Bool scrolled = NW_FALSE;
    NW_Evt_ScrollEvent_t scrollEvent;
    NW_Evt_ScrollEvent_Direction_t scrollDir;

    /* parameter assertion block */
    NW_ASSERT (box != NULL);

    if (GetXScrollAmt (box, &scrollDelta, &scrollDir, scrollMaxAmt))
        {
        if (scrollDelta > 0)
            {
            NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                           scrollDir,
                                           (NW_Uint16)scrollDelta);
            scrolled = ProcessEvent (NW_Evt_EventOf(&scrollEvent));
            }
        }

    if (GetYScrollAmt (box, &scrollDelta, &scrollDir, scrollMaxAmt))
        {
        if (scrollDelta > 0)
            {
            NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                           scrollDir,
                                           (NW_Uint16)scrollDelta);
            scrolled = ProcessScrollEvent (NW_Evt_ScrollEventOf (&scrollEvent));
            }
        }

    return scrolled;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::JumpToLink (const NW_LMgr_Box_t* box,
                   NW_Bool scrollMaxAmt)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_Bool aboveVisible = NW_TRUE;
    NW_Bool aboveCurrent = NW_TRUE;
    NW_GDI_Rectangle_t boxBounds;
    NW_GDI_Rectangle_t activeBoxBounds;
    NW_LMgr_Box_t* prevBox = NULL;
    NW_LMgr_Box_t* activeBox = NULL;
    NW_GDI_Rectangle_t viewBounds;
    NW_LMgr_BoxVisitor_t boxVisitor;
    NW_Int32 scrollDelta = 0;
    NW_Evt_ScrollEvent_t scrollEvent;
    NW_Evt_ScrollEvent_Direction_t scrollDir;

    // parameter assertion block
    NW_ASSERT (box != NULL);
    NW_ASSERT (iRootBox != NULL);

    // Handle intra-page hyperlink.
    // If the target box is within the view, don't scroll.
    // Otherwise, scroll the target box to the top of the view.
    if (GetXScrollAmt (box, &scrollDelta, &scrollDir, scrollMaxAmt))
        {
        if (scrollDelta > 0)
            {
            status = NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                                    scrollDir,
                                                    (NW_Uint16)scrollDelta);
            if (status != KBrsrSuccess)
                {
                return status;
                }

            (void) ProcessEvent ( NW_Evt_EventOf(&scrollEvent) );
            }
        }

    if (GetYScrollAmt (box, &scrollDelta, &scrollDir, scrollMaxAmt))
        {
        if (scrollDelta > 0)
            {
            status = NW_Evt_ScrollEvent_Initialize (&scrollEvent,
                                                    scrollDir,
                                                    (NW_Uint16)scrollDelta);
            if (status != KBrsrSuccess)
                {
                return status;
                }
            (void) ProcessEvent ( NW_Evt_EventOf(&scrollEvent) );
            }
        }

    NW_LMgr_BoxVisitor_Initialize (&boxVisitor, (NW_LMgr_Box_t*) iRootBox);

    // initialize the viewBounds
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    // Find the most appropriate active box in the new view
    // so it can become the new currentBox.

    // If the target box is active, it's automatically the best choice.
    if (NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class) != NULL)
        {
        activeBox = (NW_LMgr_Box_t*) box;
        }
    else
        {
        // If the target box is further down than the current box,
        // the current box is the initial candidate.
        if (scrollDir == NW_Evt_ScrollEvent_Direction_Down)
            {
            // If we jumped here from another page, it's possible that
            // currentBox isn't set. If that's the case, start from the target box.
            if (iCurrentBox == NULL)
                {
                boxBounds = GetBoxDisplayBounds(NW_LMgr_BoxOf(box));
                }
            else
                {
                boxBounds = GetBoxDisplayBounds(iCurrentBox);
                }

            do
                {
                activeBox = GetNextEventListener (&boxVisitor);

                if (!activeBox)
                    {
                    return KBrsrNotFound;
                    }

                activeBoxBounds = GetBoxDisplayBounds(activeBox);

                if (activeBoxBounds.point.y >= boxBounds.point.y)
                    {
                    aboveCurrent = NW_FALSE;
                    }
                }
            while (aboveCurrent || activeBoxBounds.point.x < boxBounds.point.x);
            }
        // If the target box is further up than the current box,
        // the first box on the page is the initial candidate.
        else
            {
            activeBox = GetNextEventListener (&boxVisitor);
            if (!activeBox)
                {
                return KBrsrNotFound;
                }
            }

        prevBox = activeBox;

        NW_ASSERT (activeBox != NULL);

        boxBounds = GetBoxDisplayBounds(NW_LMgr_BoxOf(box));


        // Starting with the initial candidate,
        // find the first active box in the new view.
        do
            {
            activeBoxBounds = GetBoxDisplayBounds(activeBox);

            if (activeBoxBounds.point.y > boxBounds.point.y)
                {
                aboveVisible = NW_FALSE;
                }

            // Keep track of the last active box before the visible area.
            // The focus will move there in the case where
            // there's no active box in the new view itself.
            if (aboveVisible || (prevBox == NULL))
                {
                prevBox = activeBox;
                }

            if (NW_LMgr_Box_GetVisibleBounds(activeBox, NULL) &&
                IsBoxVisible (activeBox, &viewBounds))
                {
                break;
                }

            activeBox = GetNextEventListener (&boxVisitor);
            }
        while (activeBox);
        }

    if (!activeBox)
        {
        activeBox = prevBox;
        }

    // If we jumped here from another page, sometimes currentBox isn't set.
    // It's possible that the active box isn't highlighted.
    // Make sure it gets highlighted.
    if (iCurrentBox == NULL)
        {
        iCurrentBox = activeBox;
        NW_LMgr_Box_SetHasFocus (NW_LMgr_BoxOf(iCurrentBox), NW_TRUE);
        status = NW_LMgr_Box_Refresh(activeBox);
        if (status != KBrsrSuccess)
            {
            return status;
            }
        }
    else
        {
        if (iCurrentBox != activeBox)
            {
            prevBox = iCurrentBox;
            iCurrentBox = activeBox;
            NW_LMgr_Box_SetHasFocus (NW_LMgr_BoxOf(iCurrentBox), NW_TRUE);
            NW_LMgr_Box_SetHasFocus (NW_LMgr_BoxOf(prevBox), NW_FALSE);
            status = NW_LMgr_Box_Refresh(prevBox);
            if (status != KBrsrSuccess)
                {
                return status;
                }
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode CView::SetAnchorName (NW_Text_t *anchorName)
    {

    if (anchorName == NULL)
        return KBrsrFailure;

    TBool state = ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->GetFormattingComplete();

    if (!state)
        {
        if (iAnchorName != NULL)
            {
            NW_Object_Delete(iAnchorName);
            iAnchorName = NULL;

            }

        iAnchorName = NW_Text_New( HTTP_iso_10646_ucs_2, anchorName->storage,
                                   anchorName->characterCount, NW_Text_Flags_Copy );

        if (!iAnchorName)
            {
            return KBrsrOutOfMemory;
            }
        }

    else
        {

        TBrowserStatusCode status;
        NW_LMgr_Box_t* targetBox = NULL;
        NW_LMgr_Box_t* box = NULL;
        NW_LMgr_BoxVisitor_t boxVisitor;
        NW_LMgr_Property_t prop;


        /* find the named box */

        NW_LMgr_BoxVisitor_Initialize (&boxVisitor, (NW_LMgr_Box_t *)iRootBox);
        (void) NW_LMgr_BoxVisitor_SetCurrentBox (&boxVisitor, (NW_LMgr_Box_t *)iRootBox);


        while ((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL)
            {
            status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_boxName, &prop);

            if ((status == KBrsrSuccess) && (prop.type == NW_CSS_ValueType_Text))
                {
                if (!NW_Text_Compare (anchorName, prop.value.object))
                    {
                    targetBox = box;
                    break;
                    }
                }
            }

        if (targetBox != NULL)
            {
            (void) JumpToLink (box, NW_FALSE);
            }

        }

    return KBrsrSuccess;
    }

void CView::ContentSize(TSize& aContentSize)
    {
    if (iRootBox)
        {
        aContentSize.iWidth = iRootBox->extents.dimension.width;
        aContentSize.iHeight = iRootBox->extents.dimension.height;
        }
    }


NW_Bool
CView::IsBoxVisible (const NW_LMgr_Box_t* box,
                     NW_GDI_Rectangle_t* bounds) const
    {
    NW_GDI_Rectangle_t boxBounds;
    NW_GDI_Rectangle_t overlap;
    const NW_LMgr_Box_t* siblingBox;

    /* parameter assertion block */
    NW_ASSERT (box != NULL);

    siblingBox = box;
    /* go through sibling chain to see if any of them visible */
    do
        {
        CView* view = const_cast<CView*>(this);
        boxBounds = view->GetBoxDisplayBounds((NW_LMgr_Box_t*)siblingBox);

        /* below check handles the case where the box is at least has sHVisibleAmt and sVVisibleAmt
        * inside the display bounds
        */
        if (NW_GDI_Rectangle_Cross(&boxBounds, bounds, &overlap))
            {
            if ((overlap.dimension.width >= iVisibleAmt.width) &&
                (overlap.dimension.height >= iVisibleAmt.height))
                {
                return NW_TRUE;
                }
            // the overlap width and height are less than the visible amount
            // in some cases the box might be smaller than the visible amount
            // then we want to check that if the overlap dimension and width
            // are same as the overlap
            else if ((overlap.dimension.width == boxBounds.dimension.width) &&
                     (overlap.dimension.height == boxBounds.dimension.height))
                {
                return NW_TRUE;
                }
            }

        siblingBox = view->GetNextSiblingBox (siblingBox);

        }
    while (siblingBox != NULL && siblingBox != box);

    return NW_FALSE;
    }

// -----------------------------------------------------------------------------
NW_LMgr_Box_t*
CView::GetInFocusBox () const
    {
    NW_GDI_Rectangle_t viewBounds;

    /* initialize the viewBounds */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    if (iCurrentBox != NULL &&
        IsBoxVisible (iCurrentBox, &viewBounds))
        {
        return iCurrentBox;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::ForceCurrentBox (NW_LMgr_Box_t* newBox,
                        NW_Bool isNewPage,
                        NW_Bool isRefreshNeeded )
    {
    NW_Evt_FocusEvent_t focusEvent;
    NW_LMgr_Box_t *prevBox = NULL;
    NW_LMgr_Box_t *tempBox = NULL;
    NW_Uint8 absorbed = !NW_LMgr_EventAbsorbed;
    NW_LMgr_Property_t prop, propNew;
    NW_LMgr_PropertyList_t *propList, *masterPropList;
    TBrowserStatusCode status = KBrsrSuccess;
    if (iCurrentBox != newBox)
        {
        if ( isNewPage )
            {
            iCurrentBox = newBox;
            return status;
            }

        if (iCurrentBox != NULL)
            {
            NW_Evt_FocusEvent_Initialize(&focusEvent, NW_FALSE);
            absorbed = ProcessEvent ( NW_Evt_EventOf(&focusEvent) );
            if (absorbed)
                {
                prevBox = iCurrentBox;
                iCurrentBox = newBox;
                // do not refresh unless it is needed
                if ( isRefreshNeeded )
                    {
                    // the following code deals with split and non-split boxes
                    propList = NULL;
                    prop.value.object = NULL;
                    masterPropList = NW_LMgr_Box_PropListGet( prevBox );

                    // first check if we are on a master or a slave box
                    if ( NW_Object_IsInstanceOf( masterPropList, &NW_LMgr_SlavePropertyList_Class ) )
                        {
                        // this is a slave, get the master's property list
                        tempBox = NW_LMgr_SlavePropertyList_GetMasterBox( masterPropList );
                        (void)NW_LMgr_Box_GetProperty( tempBox, NW_CSS_Prop_sibling, &prop );

                        // refresh master: dehighlight it
                        status = NW_LMgr_Box_Refresh(tempBox);
                        if (status != KBrsrSuccess)
                            {
                            return status;
                            }
                        }
                    else
                        {
                        (void)NW_LMgr_Box_GetProperty( prevBox, NW_CSS_Prop_sibling, &prop );
                        // refresh the non-split box: dehighlight it
                        status = NW_LMgr_Box_Refresh(prevBox);
                        if (status != KBrsrSuccess)
                            {
                            return status;
                            }
                        }

                    // get the next to the master box
                    if ( prop.value.object != NULL )
                        {
                        tempBox = NW_LMgr_BoxOf( prop.value.object );
                        if ( tempBox != NULL )
                            {
                            // this must be a slave property list
                            propList = NW_LMgr_Box_PropListGet( tempBox );
                            }
                        }
                    // split case
                    if ( propList )
                        {
                        // at this point tempbox holds the box next to the master, let's go through the chain
                        do
                            {
                            // refresh slave box: dehighlight it
                            status = NW_LMgr_Box_Refresh(tempBox);
                            if (status != KBrsrSuccess)
                                {
                                return status;
                                }
                            // get the next box
                            tempBox = NW_LMgr_SlavePropertyList_GetSiblingBox( propList );
                            propList = NW_LMgr_Box_PropListGet( tempBox );
                            // NW_LMgr_SimplePropertyList_Class means that we are on the master box again.
                            }
                        while (!NW_Object_IsClass ( propList, &NW_LMgr_SimplePropertyList_Class ) );
                        }
                    } // End of: the following code deals with split and non-split boxes
                }
            else
                {
                return KBrsrFailure;
                }
            } // End of: if(iCurrentBox != NULL)

        if (newBox != NULL)
            {
            iCurrentBox = newBox;

            NW_Evt_FocusEvent_Initialize(&focusEvent, NW_TRUE);
            (void) ProcessEvent ( NW_Evt_EventOf(&focusEvent) );

            // After changing the focus if iCurrentBox has changed
            // set that as the newBox
            if(newBox != iCurrentBox)
              {
              newBox = iCurrentBox;
              }

            // do not refresh unless it is needed
            if ( isRefreshNeeded )
                {
                // the following code deals with split and non-split boxes
                propList = NULL;
                propNew.value.object = NULL;
                masterPropList = NW_LMgr_Box_PropListGet( iCurrentBox );
                // first check if we are on a master or a slave box
                if ( NW_Object_IsInstanceOf( masterPropList, &NW_LMgr_SlavePropertyList_Class ) )
                    {
                    // this is a slave, get the master's property list
                    tempBox = NW_LMgr_SlavePropertyList_GetMasterBox( masterPropList );
                    (void)NW_LMgr_Box_GetProperty( tempBox, NW_CSS_Prop_sibling, &propNew );
                    // refresh the master: highlight it.  Has to set it to be the currentBox.
                    iCurrentBox = tempBox;
                    status = NW_LMgr_Box_Refresh(tempBox);
                    if (status != KBrsrSuccess)
                        {
                        return status;
                        }
                    }
                else
                    {
                    (void)NW_LMgr_Box_GetProperty( iCurrentBox, NW_CSS_Prop_sibling, &propNew );
                    // refresh the non-split box: highlight it
                    status = NW_LMgr_Box_Refresh(iCurrentBox);
                    if (status != KBrsrSuccess)
                        {
                        return status;
                        }
                    }

                // get the next to the master box
                if ( propNew.value.object != NULL )
                    {
                    tempBox = NW_LMgr_BoxOf( propNew.value.object );
                    if ( tempBox != NULL )
                        {
                        // this must be a slave property list
                        propList = NW_LMgr_Box_PropListGet( tempBox );
                        }
                    }
                // split case
                if ( propList )
                    {
                    // at this point tempbox holds the box next to the master, let's go through the chain
                    do
                        {
                        // refresh slave box: highlight it.  Has to set it to be the currentBox.
                        iCurrentBox = tempBox;
                        status = NW_LMgr_Box_Refresh(tempBox);
                        if (status != KBrsrSuccess)
                            {
                            return status;
                            }
                        // get the next box
                        tempBox = NW_LMgr_SlavePropertyList_GetSiblingBox( propList );
                        propList = NW_LMgr_Box_PropListGet( tempBox );
                        // NW_LMgr_SimplePropertyList_Class means that we are on the master box again.
                        }
                    while ( propList != NULL && !NW_Object_IsClass ( propList, &NW_LMgr_SimplePropertyList_Class ) );
                    }
                }
            // set currentBox back to be newBox
            iCurrentBox = newBox;
            } // End of: if (newBox != NULL)
        } // End of: if(iCurrentBox != newBox)

    return status;
    }

// -----------------------------------------------------------------------------
void*
CView::GetScroll () const
    {
    if (iScroll != NULL)
        {
        return iScroll;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::SetListener (MViewListener* viewListener)
    {
    /* install the view listener */
    TInt status = iListeners.Append( viewListener );
    return status == KErrNone ? KBrsrSuccess : KBrsrFailure;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::SetDeviceContext (CGDIDeviceContext* deviceContext)
    {
    NW_GDI_Rectangle_t displayBounds;
    NW_LMgr_Box_t* rootBox = NULL;

    /* parameter assertion block */
    NW_ASSERT (deviceContext != NULL) ;


    iDeviceContext = deviceContext;
    displayBounds = *(iDeviceContext->DisplayBounds());

    rootBox = NW_LMgr_BoxOf (iRootBox);

    (void) NW_Mem_memset (&displayBounds.point, 0, sizeof (displayBounds.dimension));
    NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( rootBox ), displayBounds);
    return KBrsrSuccess;
    }


// -----------------------------------------------------------------------------
NW_Bool
CView::GetXScrollAmt (const NW_LMgr_Box_t* box,
                      NW_Int32* scrollDelta,
                      NW_Evt_ScrollEvent_Direction_t* scrollDir,
                      NW_Bool scrollMaxAmt) const
    {
    NW_GDI_Rectangle_t boxBounds;
    NW_GDI_Rectangle_t viewBounds;

    /* parameter assertion block */
    NW_ASSERT (box != NULL);

    /* initialize the viewBounds */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    *scrollDelta = 0;
    *scrollDir = NW_Evt_ScrollEvent_Direction_Left;

    if (!NW_LMgr_Box_GetVisibleBounds(box, &boxBounds))
        {
        return NW_FALSE;
        }

    if ((boxBounds.point.x + boxBounds.dimension.width) >=
        (viewBounds.point.x + viewBounds.dimension.width))
        {

        *scrollDelta = (boxBounds.point.x + boxBounds.dimension.width)
                       - (viewBounds.dimension.width + viewBounds.point.x) + 1;

        if (boxBounds.dimension.width >= viewBounds.dimension.width)
            *scrollDelta = boxBounds.point.x - viewBounds.point.x + 1;

        *scrollDir = NW_Evt_ScrollEvent_Direction_Right;
        }

    if (boxBounds.point.x < viewBounds.point.x)
        {
        *scrollDelta = viewBounds.point.x - boxBounds.point.x + 1;
        *scrollDir = NW_Evt_ScrollEvent_Direction_Left;
        }

    if (*scrollDelta > 0)
        {
        /* Scroll amount may not exceed specified maximum.
         * If we are jumping to the target box (hyperlink) there is no limit.
         */
        if ((scrollMaxAmt) && (iMaxScrollAmt.width != 0) &&
            ( *scrollDelta > iMaxScrollAmt.width))
            {
            *scrollDelta = iMaxScrollAmt.width;
            }
        return NW_TRUE;
        }

    return NW_FALSE;
    }


// -----------------------------------------------------------------------------
NW_Bool
CView::GetYScrollAmt (const NW_LMgr_Box_t* box,
                      NW_Int32* scrollDelta,
                      NW_Evt_ScrollEvent_Direction_t* scrollDir,
                      NW_Bool scrollMaxAmt) const
    {
    NW_GDI_Rectangle_t boxBounds;
    NW_GDI_Rectangle_t viewBounds;

    /* parameter assertion block */
    NW_ASSERT (box != NULL);

    /* initialize the viewBounds */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    CView* view = const_cast<CView*>(this);
    boxBounds = view->GetBoxDisplayBounds(NW_LMgr_BoxOf(box));

    *scrollDelta = 0;
    *scrollDir = NW_Evt_ScrollEvent_Direction_Down;

    /* Scroll down calculations for scrolling to target box */
    if (scrollMaxAmt)
        {
        if ((boxBounds.point.y + boxBounds.dimension.height) >
            (viewBounds.point.y + viewBounds.dimension.height))
            {

            *scrollDelta = (boxBounds.point.y + boxBounds.dimension.height)
                           - (viewBounds.point.y + viewBounds.dimension.height) + 1;
            if (boxBounds.dimension.height >= viewBounds.dimension.height)
                {
                *scrollDelta = boxBounds.point.y - viewBounds.point.y + 1;
                }
            *scrollDir = NW_Evt_ScrollEvent_Direction_Down;
            }
        }
    /* Jump to target box (hyperlink). If target box is not in the current view,
     * calculate scroll amount that will bring it to the top of the new view.
     */
    else
        {
        *scrollDelta = boxBounds.point.y - viewBounds.point.y;
        }

    /* Scroll up calculations same for scrolling or hyperlink. */
    if (boxBounds.point.y <= viewBounds.point.y)
        {
        *scrollDelta = viewBounds.point.y - boxBounds.point.y + 1;
        *scrollDir = NW_Evt_ScrollEvent_Direction_Up;
        }

    if (*scrollDelta > 0)
        {
        /* Scroll amount may not exceed specified maximum.
         * If we are jumping to the target box (hyperlink) there is no limit.
         */
        if ((scrollMaxAmt) && (iMaxScrollAmt.height != 0) &&
            ( *scrollDelta > iMaxScrollAmt.height))
            {
            *scrollDelta = iMaxScrollAmt.height;
            }
        return NW_TRUE;
        }
    return NW_FALSE;
    }

// -----------------------------------------------------------------------------
NW_Bool
CView::ProcessAccessKey (const NW_Evt_KeyEvent_t* event)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_ADT_Vector_Metric_t numTabItems;
    //lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h))
    NW_ADT_Vector_Metric_t index;
    NW_Uint32 keyVal;

    // according to the xhtml i-mode UI spec, neither # nor *
    // should be accepted as access key in small screen mode
    if ( NW_Object_IsInstanceOf(event, &NW_Evt_UnicodeKeyEvent_Class) )
        {
        keyVal = (NW_Uint32)NW_Evt_UnicodeKeyEvent_GetKeyVal(event);
        }
    else
        {
        keyVal = (NW_Uint32)NW_Evt_ControlKeyEvent_GetKeyVal(event);
        }
    // so do not absorb the event unless we are in normal mode
    if ( NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) &&
         ( keyVal == '#' || keyVal == '*' ) )
        {
        return NW_FALSE;
        }
    if (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(iRootBox)) == 0)
        {
        return NW_FALSE;
        }

    numTabItems = NW_ADT_Vector_GetSize(iTabList);

    for (index = 0; index < numTabItems; index++)
        {
        NW_LMgr_Box_t* box;
        NW_LMgr_Property_t prop;
        NW_LMgr_AccessKey_t* ak;
        NW_Evt_AccessKeyEvent_t accessKeyEvent;
        NW_Bool isControlKey;
        NW_Bool eventAbsorbed = NW_TRUE;
        NW_LMgr_IEventListener_t* eventListener;

        box = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (iTabList, index);

        // We need to process access keys differently for image map boxes, since the
        // access keys apply to the area tags within the image map box
        if (NW_Object_IsClass(box, &NW_LMgr_ImageMapBox_Class))
            {
            NW_ADT_DynamicVector_t* areaBoxArray = NW_LMgr_ImageMapBox_GetAreaBoxList((NW_LMgr_ImageMapBox_t*) box);

            if (areaBoxArray)
                {
                NW_ADT_Vector_Metric_t elementSize = NW_ADT_Vector_GetSize(areaBoxArray);

                NW_LMgr_AreaBox_t* areaBox = NULL;

                for (NW_ADT_Vector_Metric_t arrayIndex = 0; arrayIndex < elementSize; arrayIndex++)
                    {
                    areaBox = *(NW_LMgr_AreaBox_t**)NW_ADT_Vector_ElementAt (areaBoxArray, arrayIndex);

                    status = NW_LMgr_Box_GetPropertyFromList((NW_LMgr_Box_t*)areaBox, NW_CSS_Prop_accesskey, &prop);

                    if (status == KBrsrSuccess)
                        {
                        ak = NW_LMgr_AccessKeyOf(prop.value.object);

                        if (NW_Object_IsInstanceOf(event, &NW_Evt_UnicodeKeyEvent_Class))
                            {
                            isControlKey = NW_FALSE;
                            keyVal = (NW_Uint32)NW_Evt_UnicodeKeyEvent_GetKeyVal(event);
                            }
                        else
                            {
                            isControlKey = NW_TRUE;
                            keyVal = (NW_Uint32)NW_Evt_ControlKeyEvent_GetKeyVal(event);
                            }

                        if (NW_LMgr_AccessKey_MatchKey(ak, keyVal, isControlKey))
                            {
                            // Note that we are jumping to the ImageMapBox
                            (void) JumpToLink (box, NW_TRUE);
                            NW_Evt_AccessKeyEvent_Initialize(&accessKeyEvent);

                            eventAbsorbed = NW_FALSE;
                            eventListener = static_cast<NW_LMgr_IEventListener_t*>(NW_Object_QueryInterface (areaBox, &NW_LMgr_IEventListener_Class));
                            if (eventListener != NULL)
                                {
                                eventAbsorbed =
                                    NW_LMgr_IEventListener_ProcessEvent (eventListener, &accessKeyEvent);
                                }
                            }
                        }
                    }
                }
            }
        else
            {
            status = NW_LMgr_Box_GetPropertyFromList(box, NW_CSS_Prop_accesskey, &prop);
            if (status != KBrsrSuccess)
                {
                continue;
                }

            ak = NW_LMgr_AccessKeyOf(prop.value.object);

            if (NW_Object_IsInstanceOf(event, &NW_Evt_UnicodeKeyEvent_Class))
                {
                isControlKey = NW_FALSE;
                keyVal = (NW_Uint32)
                         NW_Evt_UnicodeKeyEvent_GetKeyVal(event);
                }
            else
                {
                isControlKey = NW_TRUE;
                keyVal = (NW_Uint32)
                         NW_Evt_ControlKeyEvent_GetKeyVal(event);
                }

            if (!NW_LMgr_AccessKey_MatchKey(ak, keyVal, isControlKey))
                {
                continue;
                }

            (void) JumpToLink (box, NW_TRUE);
            NW_Evt_AccessKeyEvent_Initialize(&accessKeyEvent);

            eventAbsorbed = NW_FALSE;
            eventListener = static_cast<NW_LMgr_IEventListener_t*>(NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class));
            if (eventListener != NULL)
                {
                // The AccessKey event is not forwarded to form boxes if a context switch
                // is in progress. This prevents the form from launching auxiliary
                // dialogs during a context switch.
                if (NW_Object_IsInstanceOf(box, &NW_FBox_FormBox_Class))
                    {
                    NW_HED_DocumentRoot_t* docRoot = NW_FBox_FormBoxUtils_GetDocRoot(NW_LMgr_BoxOf(box));
                    NW_ASSERT(docRoot != NULL);
                    if (NW_HED_DocumentRoot_IsContextSwitchPending(docRoot))
                        {
                        return NW_FALSE;
                        }
                    }
                eventAbsorbed =
                    NW_LMgr_IEventListener_ProcessEvent (eventListener, &accessKeyEvent);
                }
            }

        if (eventAbsorbed)
            {
            (void) Draw (NW_TRUE);
            }
        return eventAbsorbed;
        }

    return NW_FALSE;
    }
// -----------------------------------------------------------------------------
NW_Bool
CView::HandleGNavigation (NW_Uint32 tabIndex)
    {
    NW_LMgr_Box_t* box = NULL;
    NW_Evt_AccessKeyEvent_t accessKeyEvent;
    NW_Bool eventAbsorbed;
    NW_LMgr_IEventListener_t* eventListener;

    if ( (NW_ADT_Vector_Metric_t) tabIndex >=
         NW_ADT_Vector_GetSize(iTabList))
        {
        return NW_FALSE;
        }

    box = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (iTabList,
            (NW_ADT_Vector_Metric_t) tabIndex);
    if (box == NULL)
        {
        return NW_FALSE;
        }
    (void) ForceCurrentBox (box, NW_FALSE /* not new page */, NW_TRUE);
    NW_Evt_AccessKeyEvent_Initialize(&accessKeyEvent);

    eventAbsorbed = NW_FALSE;
    eventListener = static_cast<NW_LMgr_IEventListener_t*>(NW_Object_QueryInterface (box, &NW_LMgr_IEventListener_Class));
    if (eventListener != NULL)
        {
        eventAbsorbed =
            NW_LMgr_IEventListener_ProcessEvent (eventListener, &accessKeyEvent);
        }

    if (eventAbsorbed)
        {
        (void) Draw (NW_TRUE);
        }

    return eventAbsorbed;
    }

// -----------------------------------------------------------------------------
void
CView::HandleError (TBrowserStatusCode status)
    {
    for( TInt n=0; n<iListeners.Count(); n++ )
        {
        iListeners[n]->HandleError (status);
        }
    }


/* ------------------------------------------------------------------------- *
   IBoxTreeListener implementation
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
void
CView::BoxTreeDestroyed ()
    {
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->ClearFormattingContext();

    iCurrentBox = NULL;
    NW_LMgr_RootBox_SetBody(iRootBox, NULL);
    (void) NW_ADT_DynamicVector_Clear(iTabList);
    iCreateTabList = NW_TRUE;
    (void) NW_ADT_DynamicVector_Clear(iRootBox->iOutOfViewListeners);
    iRootBox->iYScrollChange = 0;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::BoxTreeListenerAppendTabItem (NW_LMgr_Box_t* box)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    if (iCreateTabList )
        {
        status = AppendTabItem (box);
        }

    return status;
    }

// -----------------------------------------------------------------------------
void
CView::FormattingComplete ()
    {

	TRAP_IGNORE( iPageScaler->DocumentChangedL() );
    iDocumentFinished = ETrue;
    iPageScaler->DocumentCompleted();

    // we need to handle the return status here
    if ( !((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->BackgroundFormat() )
        {
        (void) HandleFormattingComplete ();
        NW_LMgr_RootBox_ExtendsCalculationNeeded( iRootBox );
       	(void) NW_LMgr_Box_UpdateContainerBodyBox (iRootBox );
        }
    else
        {
        // end of background format.
        // Clear the rendering cache
        NW_LMgr_RootBox_InvalidateCache( iRootBox );

        // Clear the float context vectors
        (void) NW_ADT_DynamicVector_Clear(iRootBox->placedFloats);
        (void) NW_ADT_DynamicVector_Clear(iRootBox->pendingFloats);

        // We'll have to recalculate the extents
        NW_Mem_memset(&(iRootBox->extents), 0, sizeof(NW_GDI_Rectangle_t));
        NW_Mem_memset(&(iRootBox->scrollRange), 0, sizeof(NW_GDI_Rectangle_t));
        NW_LMgr_RootBox_ExtendsCalculationNeeded( iRootBox );
        NW_LMgr_RootBox_SetBody( iRootBox, NULL );
        }

#ifdef __WINS__
 #ifdef _DEBUG
    NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
#endif
 #endif

    }

// -----------------------------------------------------------------------------
void
CView::StartCSSApply ()
    {
    // cancel outstanding format and clear the tablist
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->Cancel();

    // clear tablist
    (void) NW_ADT_DynamicVector_Clear( iTabList );
    iCreateTabList = NW_TRUE;
    // css might delete current box
    iCurrentBox = NULL;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::Relayout(NW_Bool createTabList)
    {
    TBrowserStatusCode status = KBrsrSuccess;

    status = Collapse( NULL, NW_FALSE );
    if (status != KBrsrSuccess)
        {
        return status;
        }

    status = FormatBoxTree( createTabList );

    return status;
    }


// -----------------------------------------------------------------------------
NW_ADT_Vector_Metric_t
CView::GetActiveBoxId()
    {
    //lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h))
    NW_ADT_Vector_Metric_t index = 0;
    NW_Uint16 size = 0;

    if (iCurrentBox == NULL)
        {
        return 0;
        }
    size = NW_ADT_Vector_GetSize(iTabList);

    for (index = 0; index < size; index++)
        {
        NW_LMgr_Box_t* box = NULL;

        box = (NW_LMgr_Box_t*) * NW_ADT_Vector_ElementAt(iTabList, index);
        if ((box) && (iCurrentBox == box))
            return index;
        }
    return 0;
    }

NW_LMgr_Box_t*
CView::GetVisibleBox () const
    {
    NW_ADT_Vector_Metric_t i, size;
    NW_LMgr_Box_t* box;
    NW_GDI_Rectangle_t viewBounds;

    size = NW_ADT_Vector_GetSize(iTabList);
    /* initialize the viewBounds */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    for (i = 0; i < size; i++)
        {
        box = (NW_LMgr_Box_t*) * NW_ADT_Vector_ElementAt(iTabList,
                (NW_ADT_Vector_Metric_t)i);
        if ( (IsBoxVisible(box, &viewBounds)) && !(IsZeroBox(box)) )
            return box;
        }
    return NULL;
    }

NW_LMgr_Box_t*
CView::GetVisibleBoxReverse () const
    {
    TInt i, size;
    NW_LMgr_Box_t* box;
    NW_GDI_Rectangle_t viewBounds;

    size = NW_ADT_Vector_GetSize(iTabList);
    /* initialize the viewBounds */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    for (i = size  - 1; i >= 0 ; i--)
        {
        box = (NW_LMgr_Box_t*) * NW_ADT_Vector_ElementAt(iTabList,
                (NW_ADT_Vector_Metric_t)i);
        if ( (IsBoxVisible(box, &viewBounds)) && !(IsZeroBox(box)) )
            return box;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
NW_LMgr_Box_t*
CView::GetLastVisibleBox () const
    {
    NW_ADT_Vector_Metric_t i, size;
    NW_LMgr_Box_t* box = NULL;
    NW_LMgr_Box_t* tmpBox;
    NW_GDI_Rectangle_t viewBounds;

    size = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(iTabList));
    /* initialize the viewBounds */
    viewBounds = *(iDeviceContext->DisplayBounds());
    viewBounds.point = *(iDeviceContext->Origin());

    for (i = 0; i < size; i++)
        {
        tmpBox = (NW_LMgr_Box_t*) * NW_ADT_Vector_ElementAt(iTabList,
                 (NW_ADT_Vector_Metric_t)i);
        if (IsBoxVisible (tmpBox, &viewBounds))
            {
            box = tmpBox;
            }
        }
    return box;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::GotoActiveBox(NW_ADT_Vector_Metric_t activeBoxId)
    {
    NW_LMgr_Box_t* box = NULL;
    TBool noBoxVisible = EFalse;

    void ** element;
    CMVCScroll* scroll = static_cast<CMVCScroll*>(iScroll);

    iCurrentBox = NULL;
    if ((activeBoxId == 0) || (activeBoxId >= NW_ADT_Vector_GetSize(iTabList)))
        {
        scroll->TabForward(NW_TRUE /* new page */, &noBoxVisible);
        }
    else
        {
        element = NW_ADT_Vector_ElementAt(iTabList, activeBoxId);
        if (element != NULL)
            {
            box = (NW_LMgr_Box_t*) * element;
            /* if necessary, scroll the box into the view */
            (void) JumpToLink (box, NW_TRUE);
            }
        else
            {
            scroll->TabForward(NW_TRUE /* new page */, &noBoxVisible);
            }
        }

    if (noBoxVisible)
        {
        /* first box is not visible. try to get one*/
        box = GetVisibleBox ();
        if (box != NULL)
            {
            (void) ForceCurrentBox (box, NW_TRUE, NW_TRUE );
            }
        }

    return Draw (NW_TRUE);
    }


// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::GotoBox(NW_LMgr_Box_t* aBox)
    {
    TBrowserStatusCode status = KBrsrNotFound;
    if (aBox != NULL)
        {
        NW_Bool found = NW_FALSE;
        NW_LMgr_Box_t* box = NULL;
        //lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h))
        NW_ADT_Vector_Metric_t index ;
        NW_ADT_Vector_t* tabListVector = NW_ADT_VectorOf( iTabList );
        NW_Uint16 size = NW_ADT_Vector_GetSize( tabListVector );
        for ( index = 0; index < size; index++ )
            {
            box = (NW_LMgr_Box_t*) * NW_ADT_Vector_ElementAt( tabListVector, index );

            if ( aBox == box )
                {
                found = NW_TRUE;
                break;
                }
            }
        if (found)
            {
            status = KBrsrSuccess;
            (void) GotoActiveBox (index);
            }
        }

    return status;
    }

// this function comes from old View.cpp
TBrowserStatusCode
CView::SwitchVerticalLayout (NW_Bool aFormatBoxTree,
                             NW_Bool aSwitchBox)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_LMgr_BoxVisitor_t boxVisitor;
    NW_LMgr_Box_t* box = NULL;
    NW_GDI_Point2D_t origin;

    // cancel any existing formatting
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->Cancel();

    origin.x = 0;
    origin.y = 0;
    iDeviceContext->SetOrigin (&origin);

    // if the box is an image map box, then we want to highlight the first area box when
    // we switch layout modes
    box = GetCurrentBox ();
    if (box)
        {
        if (NW_Object_IsClass(box, &NW_LMgr_ImageMapBox_Class))
            {
            NW_ADT_DynamicVector_t* areaBoxList = NW_LMgr_ImageMapBox_GetAreaBoxList (NW_LMgr_ImageMapBoxOf(box));

            if (areaBoxList)
                {
                NW_ADT_Vector_Metric_t elementSize = NW_ADT_Vector_GetSize(areaBoxList);

                if ( elementSize > 0 )
                    {
                    NW_LMgr_AreaBox_t* areaBox = *(NW_LMgr_AreaBox_t**)
                                                 NW_ADT_Vector_ElementAt (areaBoxList, 0);

                    if (areaBox)
                        {
                        // not set focus to true on AreaBox
                        // setCurrentBox() will take care of it
                        iCurrentBox = NW_LMgr_BoxOf(areaBox);

                        NW_LMgr_Box_SetHasFocus (NW_LMgr_BoxOf(box), NW_FALSE);
                        }
                    }
                }
            }
        // if the box is an area box, then we want to highlight the parent image map box when
        // we switch layout modes
        else if (NW_Object_IsClass(box, &NW_LMgr_AreaBox_Class))
            {
            NW_LMgr_ImageMapBox_t* imageMapBox = NW_LMgr_AreaBox_GetParentImageMapBox(box);

            if (imageMapBox)
                {
                iCurrentBox = NW_LMgr_BoxOf(imageMapBox);
                NW_LMgr_Box_SetHasFocus (NW_LMgr_BoxOf(imageMapBox), NW_TRUE);

                NW_LMgr_Box_SetHasFocus (NW_LMgr_BoxOf(box), NW_FALSE);
                }
            }
        }

    status = Collapse (NULL, NW_FALSE);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    /* create the Box Visitor object and use it to traverse the box tree */
    status = NW_LMgr_BoxVisitor_Initialize (&boxVisitor, NW_LMgr_BoxOf(iRootBox));
    if (status != KBrsrSuccess)
        {
        return status;
        }

    /* Iterate through all children */
    if (aSwitchBox)
        {
        while ((box = NW_LMgr_BoxVisitor_NextBox(&boxVisitor, 0)) != NULL)
            {
            NW_LMgr_Box_t * newBox = NW_LMgr_VerticalTableBox_SwitchBox(box);
            if (newBox)
                {
                boxVisitor.currentBox = newBox;
	            if (NW_Object_IsInstanceOf(newBox, &NW_LMgr_StaticTableBox_Class))
	              {
	              NW_XHTML_AttributePropertyTable_ApplyGridModeTableStyle(newBox);
	              }
                }
            else
                {
		            // Apply grid mode related styles to static table boxes.
		            if (NW_Object_IsInstanceOf(box, &NW_LMgr_StaticTableBox_Class))
		              {
		              NW_XHTML_AttributePropertyTable_ApplyGridModeTableStyle(box);
		              }
		            }
                }

        }

    if (aFormatBoxTree)
        {
        status = FormatBoxTree (NW_TRUE);
        if (status != KBrsrSuccess)
            {
            return status;
            }
        status = Draw (NW_TRUE);
        if (status != KBrsrSuccess)
            {
            return status;
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::Cancel ()
    {
    ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->Cancel();
    return KBrsrSuccess;
    }

TBrowserStatusCode
CView::UpdateHScrollBar () const
    {
    NW_GDI_Dimension2D_t canvasSize;
    NW_GDI_Dimension2D_t extents;
    NW_GDI_Dimension2D_t size;
    NW_GDI_Metric_t originX;
    TBrowserStatusCode status = KBrsrSuccess;

    status = NW_LMgr_RootBox_GetExtents (iRootBox, &extents);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    originX = (NW_GDI_Metric_t)(iDeviceContext->Origin()->x);
    /* Shift coordinates to 0 scrollRange will always be smaller or identical to the extents. */
    if (NW_LMgr_RootBox_IsPageRTL(NW_LMgr_RootBoxOf(iRootBox)))
        {
        /* get range */
        status = NW_LMgr_RootBox_GetScrollRange (iRootBox, &canvasSize);
        if (status != KBrsrSuccess)
            {
            return status;
            }

        originX = originX + canvasSize.width - extents.width;
        if (originX < 0)
            {
            originX = 0;
            }

        size = canvasSize;
        }
    else
        {
        size = extents;
        }

    // UI should fix it first.
    // if( !NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) )
        {
        CView* view = const_cast<CView*>(this);
		TInt err;
        TRAP (err, view->UpdateScrollBarsL (CEikScrollBar::EHorizontal, originX, size.width));
        }

    /* successful completion */
    return KBrsrSuccess;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CView::UpdateVScrollBar () const
    {
    NW_GDI_Dimension2D_t canvasSize;
    TBrowserStatusCode status = KBrsrSuccess;

    /* get range */
    status = NW_LMgr_RootBox_GetExtents (iRootBox, &canvasSize);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    CView* view = const_cast<CView*>(this);
	TInt err;
    TRAP (err, view->UpdateScrollBarsL (CEikScrollBar::EVertical, iDeviceContext->Origin()->y , canvasSize.height));

    /* successful completion */
    return KBrsrSuccess;
    }

//-----------------------------------------------------------------------------
void
CView::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    CEikBorderedControl::Draw( aRect );

    // put offscreen bitmap onto the screen
    gc.BitBlt( Rect().iTl, iOffscreenBitmap );
    }

//-----------------------------------------------------------------------------
void
CView::DrawHistory( CFbsBitGc& aGc, const TRect& aDocumentAreaToDraw ) const
    {
    // put offscreen bitmap onto the screen
    aGc.BitBlt( aDocumentAreaToDraw.iTl, iOffscreenBitmap );
    }

/* ------------------------------------------------------------------------- */
void
CView::EpocDrawNow (NW_Bool drawNow)
    {
    if (drawNow)
        DrawNow ();
    else
        DrawDeferred ();
    }

/* ------------------------------------------------------------------------- *
   IBoxTreeListener implementation
 * ------------------------------------------------------------------------- */
void
CView::EnableSuppressDupReformats (NW_Int16 state)
    {
    iSuppressDupReformats = state;
    if (state == NW_Image_AbstractImage_SuppressDupReformats_Start)
        iPendingReformatBox = NULL;
    iForceReformatNeeded = NW_FALSE;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
CView::BoxTreeListenerRelayout (NW_Bool createTabList)
    {
    /* get the listener and send a relayout request */
    return iShell->SendRelayoutMsg (createTabList);
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CView::Refresh (NW_LMgr_Box_t* box)
    {
    CGDIDeviceContext* deviceContext;
    NW_LMgr_Box_t *currentBox;
    NW_GDI_Rectangle_t origClip;
    NW_GDI_Color_t origBgColor;
    NW_GDI_Color_t bgColor = NW_GDI_Color_White;
    NW_LMgr_Property_t bodyColor;
    TBrowserStatusCode status = KBrsrSuccess;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_LMgr_Box_Class));

    if (IsImageMapView() && (box != NW_LMgr_RootBox_GetFocusBox (iRootBox)))
        {
        box = NW_LMgr_RootBox_GetFocusBox (iRootBox);
        }

    deviceContext = iDeviceContext;

    /* Save the initial clip and bg color*/
    origClip = deviceContext->ClipRect();
    origBgColor = deviceContext->BackgroundColor();

    NW_GDI_Rectangle_t clipRect;
    NW_GDI_Rectangle_t viewBounds;
    NW_Uint8 flags = 0;

    /* initialize the viewBounds */
    viewBounds = *(deviceContext->DisplayBounds());
    viewBounds.point = (*deviceContext->Origin());

    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds(box);

    if (NW_GDI_Rectangle_Cross(&viewBounds, &boxBounds, &clipRect))
        {
        /* Get the current box */
        currentBox = NW_LMgr_RootBox_GetCurrentBox( iRootBox );
        /* Set the new clip */
        deviceContext->SetClipRect( &clipRect );

        bodyColor.type = NW_CSS_ValueType_Token;
        bodyColor.value.token = NW_CSS_PropValue_transparent;
        (void)NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_backgroundColor, &bodyColor);
        if (bodyColor.type == NW_CSS_ValueType_Color)
            {
            bgColor = bodyColor.value.integer;
            }
        deviceContext->SetBackgroundColor( bgColor );

        /* Clear the area */
        deviceContext->PreDraw ();

        /* Draw the background */
        status = NW_LMgr_Box_DrawBackground (box, box->parent, deviceContext);
        if (status != KBrsrSuccess)
            {
            goto finish;
            }

        if ( NW_Object_IsClass( box, &NW_LMgr_AnimatedImageBox_Class) )
            {
            flags = (NW_Uint8)NW_LMgr_Box_Flags_DrawFloats;
            }
        else if ( NW_Object_IsDerivedFrom( box, &NW_LMgr_ImgContainerBox_Class) )
            {
            if ( NW_LMgr_ImgContainerBox_ImageIsBroken( box ) == NW_FALSE )
                {
                flags = (NW_Uint8)NW_LMgr_Box_Flags_DrawFloats;
                }
            }
        //R->embed
        //else if ( ObjectUtils::IsObjectBox(*box, EFalse) )
        //    {
        //    flags = (NW_Uint8)NW_LMgr_Box_Flags_DrawFloats;
        //    }

        /* Render the box and its descendants */
        /* Adjust crop if parent has it set  */
        NW_LMgr_Box_t* tempBox = NULL;
        NW_GDI_Rectangle_t newClip = viewBounds;

        if ( !NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) )
            {
            tempBox = NW_LMgr_BoxOf( box->parent );
            /* EXPENSIVE OPERATION:Needs to be reevaluated */
            do
                {
                if ( NW_Object_IsInstanceOf( tempBox, &NW_LMgr_StaticTableCellBox_Class ) )
                    {
                    newClip = NW_LMgr_StaticTableCellBoxOf(tempBox)->cellClip;
                    break;
                    }
                tempBox = NW_LMgr_BoxOf(tempBox->parent);
                }
            while ( tempBox != NULL );
            }
        // Find out if the box or the parent is in focus
        NW_Bool hasFocus;
        // check if childbox is the current box
        hasFocus = ( currentBox == box );
        if ( !hasFocus )
            {
            // if the currentBox is a container, then check its childern to see if 'box' is
            // one of them.
            if ( currentBox && NW_Object_IsDerivedFrom( currentBox, &NW_LMgr_ContainerBox_Class) )
                {
                //
                NW_LMgr_BoxVisitor_t boxVisitor;

                NW_LMgr_BoxVisitor_Initialize( &boxVisitor, (NW_LMgr_Box_t*)currentBox );
                while ( ( tempBox = NW_LMgr_BoxVisitor_NextBox( &boxVisitor, NULL ) ) != NULL )
                    {
                    //
                    if ( box == tempBox )
                        {
                        hasFocus = NW_TRUE;
                        break;
                        }
                    }
                }
            }
        flags = (NW_Uint8)NW_LMgr_Box_Flags_DrawFloats;
        status = BoxRender_Render (box, deviceContext, &newClip, currentBox, flags, hasFocus,
                                   iRootBox );
        if (status != KBrsrSuccess)
            {
            goto finish;
            }

        /* Invalidate the box area */
        deviceContext->PostDraw (NW_FALSE);
        }
    if (!NW_Object_IsInstanceOf(box, &NW_LMgr_PosFlowBox_Class))
        {
        DrawPositionedBoxes();
        }

finish:
    /* Reset the clip */
    deviceContext->SetClipRect ( &origClip);
    deviceContext->SetBackgroundColor( origBgColor );

    return status;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CView::RedrawDisplay (NW_Bool drawNow)
    {
    /* get the listener and send a redraw request */
    return iShell->SendRedrawMsg (drawNow);
    }

const NW_GDI_Point2D_t*
CView::GetOrigin ()
    {
    return iDeviceContext->Origin();
    }

//-----------------------------------------------------------------------------
// functions from Epoc32View
//-----------------------------------------------------------------------------
NW_Bool
CView::ProcessEvent (const NW_Evt_Event_t* event)
    {
    CMVCScroll* scroll;

    // parameter assertion block
    NW_ASSERT (event != NULL);

    scroll = (CMVCScroll*)iScroll;

    if ( EpocProcessEvent(event) )
        {
        return NW_TRUE;
        }

    if (NW_Object_IsClass(event, &NW_Evt_FontSizeChangeEvent_Class))
        {
        return ProcessFontSizeChangeEvent ();
        }

    if (NW_Object_IsClass(event, &NW_Evt_UnicodeKeyEvent_Class))
        {
        NW_Bool absorbed = NW_FALSE;

        // long keypress -> access key handling
        absorbed = ProcessAccessKey( NW_Evt_KeyEventOf(event) );
        NW_Evt_ControlKeyType_t controlKey = NW_Evt_ControlKeyType_None;
        // short keypress -> home/end functionality
        if(!absorbed)
            {

            if ( controlKey != NW_Evt_ControlKeyType_None )
                {
                absorbed = (NW_Bool)scroll->ScrollPage( controlKey );
                // refresh if absorbed
                if ( absorbed )
                    {
                    (void)Draw( NW_TRUE );
                    }
                }
            }
        return absorbed;
        }

    // our superclass didn't want to take responsibility of the event so we must
    // try to do something with it
    if (NW_Object_IsInstanceOf (event, &NW_Evt_ControlKeyEvent_Class))
        {
        NW_Evt_ControlKeyType_t key;

        // we really only care about a handful of key events
        key = NW_Evt_ControlKeyEvent_GetKeyVal (event);
        if (key != NW_Evt_ControlKeyType_Left && key != NW_Evt_ControlKeyType_Right &&
            key != NW_Evt_ControlKeyType_Up && key != NW_Evt_ControlKeyType_Down)
            {
            return NW_FALSE;
            }
        if ( !NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) )
            {
            return (NW_Bool)scroll->ProcessControlKey(key);
            }
        else
            {
            // Process keys for vertical layout
            return (NW_Bool)scroll->ProcessControlKeyVL(key);
            }
        }

    return NW_FALSE;
    }

//-----------------------------------------------------------------------------
// functions from CEpoc32View, Epoc32ViewCallbacks
//-----------------------------------------------------------------------------
void
CView::SwitchVerticalLayout()
    {
    /* If the page is WML or XHTML, don't switch mode */
    if (NW_Settings_GetDisableSmallScreenLayout())
        {
        return ;
        }

    if (NW_Object_IsInstanceOf (iShell->GetDocumentRoot()->childNode, &NW_XHTML_ContentHandler_Class))
        {
        ExternalLayoutSwitch ();

        if (IsImageMapView())
            {
            SwitchFromImageMapView();
            }
        else
            {
            SwitchVerticalLayout (NW_TRUE, NW_TRUE);
            }
        }

    /* In case we change from vertical mode to normal mode, we want to load all the images
     that we did not load in vertical mode since they were too small
     */
    if ( !NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) )
        {
        if (NW_Object_IsInstanceOf (iShell->GetDocumentRoot()->childNode, &NW_XHTML_ContentHandler_Class))
            {
            NW_CSS_Processor_t* processor = NW_XHTML_ContentHandler_GetCSSProcessor(
                                                NW_XHTML_ContentHandlerOf (iShell->GetDocumentRoot()->childNode));
            if (processor != NULL)
                {
                NW_CSS_Processor_ProcessPendingLoads(processor);
                }
            }
        if (NW_Settings_GetImagesEnabled())
            {
            NW_HED_DocumentRoot_ShowImages (iShell->GetDocumentRoot());
            }
        }
    }

TKeyResponse
CView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (aType != EEventKey)
        return EKeyWasNotConsumed;

    TKeyResponse retVal = EKeyWasNotConsumed;

    retVal = DoHandleControlKeysL(aKeyEvent);
    if (retVal == EKeyWasConsumed)
        return retVal;

    NW_Bool isLongKeypress = ((aKeyEvent.iRepeats == 1) && (iLastRepeats == 0)); // Long Keypress
    iLastRepeats = aKeyEvent.iRepeats;

    NW_Bool absorbed = NW_FALSE;
    NW_Evt_UnicodeKeyEvent_t keyEvent;

    switch ((NW_Ucs2)aKeyEvent.iCode)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {
            break;
            }
        case '*':  // fall through
        case '#':
            {
            // *(home) and #(end) shortcuts work on both small
            // and normal mode
            // process short *,# key event
            break;
            }
        default:
            {
            //return EKeyWasNotConsumed; for qwerty functionality
            }
        }

    TBrowserStatusCode status = NW_Evt_UnicodeKeyEvent_Initialize(&keyEvent, (NW_Ucs2)aKeyEvent.iCode);
    if (status == KBrsrSuccess)
        {
        if (isLongKeypress)
            {
            NW_Evt_KeyEvent_SetLongPress(&keyEvent);
            }
        absorbed = ProcessEvent ( NW_Evt_EventOf(&keyEvent) );
        }

    return (absorbed == NW_FALSE) ? EKeyWasNotConsumed : EKeyWasConsumed;
    }

//-----------------------------------------------------------------------------
void CView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    switch (aPointerEvent.iType) {
        case TPointerEvent::EButton1Down:
        	iLastPosition = aPointerEvent.iPosition;
            iDrag = EFalse;
			break;
		case TPointerEvent::EDrag:
			TPoint  currPosition;
			TPoint  nextPosition;
			nextPosition = iLastPosition - aPointerEvent.iPosition;
			iLastPosition = aPointerEvent.iPosition;
			
			if(nextPosition.iX || nextPosition.iY)
			{
				currPosition.iX = iDeviceContext->Origin()->x;
				currPosition.iY = iDeviceContext->Origin()->y;
				ScrollTo(currPosition+nextPosition);

                //This is for Drag event
                //Introducing 10 pixel offset. This is introduced to tackle problem that occurs if user clicks
                //link but still causes a very small pixel move. This gives a small difference in previous
                //and next position. As a side effect iDrag will be true and request will not be sent even if
                //there is very small pixel difference.
                //Following logic tries to handle such scenario.
                TInt nOffset = 10;
                TBool bXOffset = ((nextPosition.iX > nOffset) || (nextPosition.iX < -nOffset));
                TBool bYOffset = ((nextPosition.iY > nOffset) || (nextPosition.iY < -nOffset));
                if(bXOffset || bYOffset)
                {
                    iDrag = ETrue;
                }
            } 
            break;
    }
    
    NW_ADT_Vector_Metric_t i;
    NW_ADT_Vector_Metric_t size = NW_ADT_Vector_GetSize( iTabList );
    i = 0;
    TBool activeInputBox = iCurrentBox && NW_Object_IsInstanceOf(iCurrentBox, &NW_FBox_InputBox_Class) && 
        NW_FBox_InputBox_IsActive( iCurrentBox );
    TBool found = EFalse;
    TPoint point = aPointerEvent.iPosition + (TPoint(iDeviceContext->Origin()->x, iDeviceContext->Origin()->y));
    for( i = 0; i < size; i++ )
        {
        NW_LMgr_Box_t* tempBox = (NW_LMgr_Box_t*)*NW_ADT_Vector_ElementAt( iTabList, (NW_ADT_Vector_Metric_t)i);
        NW_GDI_Rectangle_t boxRect = GetBoxDisplayBounds(tempBox);
        TRect rect(TPoint(boxRect.point.x, boxRect.point.y), TSize(boxRect.dimension.width, boxRect.dimension.height));
        if (rect.Contains(point))
            {
            found = ETrue;
            }
        else
            {
            NW_LMgr_Property_t prop;
            NW_LMgr_Box_t* box = tempBox;
            TBool done = EFalse;
            while(!done)
                {
                prop.value.object = NULL;
                if (NW_LMgr_Box_GetProperty( box, NW_CSS_Prop_sibling, &prop ) == KBrsrSuccess)
                    {
                    if ( prop.value.object != NULL && prop.value.object != tempBox )
                        {
                        box = NW_LMgr_BoxOf( prop.value.object );
                        boxRect = GetBoxDisplayBounds(box);
                        rect.SetRect(TPoint(boxRect.point.x, boxRect.point.y), TSize(boxRect.dimension.width, boxRect.dimension.height));
                        if (rect.Contains(point))
                            {
                            found = ETrue;
                            break;
                            }
                        }
                    else
                        {
                        done = ETrue;
                        }
                    }
                else
                    {
                    done = ETrue;
                    }
                }
            }

        if (found)
            {
            if (tempBox != iCurrentBox)
                {
                iShouldActivate = EFalse;
               }
            if (aPointerEvent.iType == TPointerEvent::EButton1Down)
                {
                iShouldActivate = ETrue;
                }
            if (tempBox != iCurrentBox)
                {
                if (activeInputBox)
                    {
                    if (!InputElementEditComplete( NW_TRUE ))
                        {
                        iShouldActivate = EFalse;
                        break;
                        }
                    }
                NW_LMgr_Box_SetHasFocus (iCurrentBox, NW_FALSE);
                NW_LMgr_Box_SetHasFocus (tempBox, NW_TRUE);
                SetCurrentBox(tempBox);
                NW_LMgr_RootBox_SetFocusBox(iRootBox, tempBox);
                Draw (NW_TRUE /*DrawNow*/);
                }
            if ((aPointerEvent.iType == TPointerEvent::EButton1Up) && (iShouldActivate))
                {
                    if((!iDrag))
                    {
                        NW_Evt_ActivateEvent_t actEvent;
                        NW_Evt_ActivateEvent_Initialize (&actEvent);
                        ProcessEvent (NW_Evt_EventOf(&actEvent));
                    }
                }
            break;
            }
        }
    if (!found) // Clicked or dragged on an empty area
        {
        iShouldActivate = EFalse;
        if (activeInputBox)
            {
            InputElementEditComplete( NW_TRUE );
            }
        }
    }

//-----------------------------------------------------------------------------
void
CView::FocusChanged (TDrawNow aDrawNow)
    {
    if (IsFocused())
        {
        iShell->GainFocus();
        }
    else
        // control is about losing the focus
        {
        iShell->LoseFocus();
        }
    // Inform all focus observers that the focus changed.
    // This is important because we need to inform Flash plugin to stop playing
    // when the browser is in the background
    TInt count = iViewFocusObserver.Count();
    for (TInt i = 0; i < count; i++)
        {
        //iViewFocusObserver[i]->ViewFocusChanged(IsFocused());
        }

    CEikBorderedControl::FocusChanged(aDrawNow);
    }

// -------------------------------------------------------------------------
void CView::SetFormattingPriority( CActive::TPriority aPriority )
    {
    iFormatPriority = aPriority;
    }

// -------------------------------------------------------------------------
TInt CView::AppendFocusObserver(MViewFocusObserver* aViewFocusObserver)
    {
    NW_ASSERT(aViewFocusObserver != NULL);
    return iViewFocusObserver.Append(aViewFocusObserver);
    }

// -------------------------------------------------------------------------
void CView::RemoveFocusObserver(MViewFocusObserver* aViewFocusObserver)
    {
    NW_ASSERT(aViewFocusObserver != NULL);
    TInt i = iViewFocusObserver.Find(aViewFocusObserver);
    if (i != KErrNotFound)
        {
        iViewFocusObserver.Remove(i);
        }
    }

// -------------------------------------------------------------------------
TInt CView::AddStateChangeObserver(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver)
    {
    NW_ASSERT(aBrCtlStateChangeObserver != NULL);
    return iStateChangeObserver.Append(aBrCtlStateChangeObserver);
    }

// -------------------------------------------------------------------------
void CView::RemoveStateChangeObserver(MBrCtlStateChangeObserver* aBrCtlStateChangeObserver)
    {
    NW_ASSERT(aBrCtlStateChangeObserver != NULL);
    TInt i = iStateChangeObserver.Find(aBrCtlStateChangeObserver);
    if (i != KErrNotFound)
        {
        iStateChangeObserver.Remove(i);
        }
    }

//-----------------------------------------------------------------------------
TKeyResponse
CView::DoHandleControlKeysL(const TKeyEvent& aKeyEvent)
    {
    TKeyResponse keyConsumed = EKeyWasNotConsumed;
    NW_Evt_ControlKeyEvent_t controlKeyEvent;
    NW_Evt_ControlKeyType_t controlKeyVal;

    controlKeyVal = NW_Evt_ControlKeyType_None;

    switch (aKeyEvent.iCode)
        {
        case EKeyLeftArrow:
            controlKeyVal = NW_Evt_ControlKeyType_Left;
            break;

        case EKeyRightArrow:
            controlKeyVal = NW_Evt_ControlKeyType_Right;
            break;

        case EKeyUpArrow:
            controlKeyVal = NW_Evt_ControlKeyType_Up;
            break;

        case EKeyDownArrow:
            controlKeyVal = NW_Evt_ControlKeyType_Down;
            break;

        case EKeyEnter:
        case EKeyDevice3:
            {
            NW_Evt_ActivateEvent_t actEvent;
            NW_Evt_ActivateEvent_Initialize (&actEvent);

            if ( ProcessEvent (NW_Evt_EventOf(&actEvent)) )
                {
                keyConsumed = EKeyWasConsumed;
                }
            break;
            }

        default:
            return keyConsumed;
        }
    if (controlKeyVal != NW_Evt_ControlKeyType_None)
        {
        TBrowserStatusCode status = KBrsrSuccess;

        status = NW_Evt_ControlKeyEvent_Initialize (&controlKeyEvent, controlKeyVal);
        if (status != KBrsrSuccess)
            {
            return keyConsumed;
            }
        NW_GDI_Rectangle_t viewBoundsOld, viewBoundsNew;
        viewBoundsOld = *iDeviceContext->DisplayBounds();
        viewBoundsOld.point = *iDeviceContext->Origin();

        if ( ProcessEvent (NW_Evt_EventOf(&controlKeyEvent)) )
            {
            viewBoundsNew = *iDeviceContext->DisplayBounds();
            viewBoundsNew.point = *iDeviceContext->Origin();

            /* scroll happened*/
            if (viewBoundsNew.point.y != viewBoundsOld.point.y || viewBoundsNew.point.x != viewBoundsOld.point.x)
                {
                status = Draw (NW_TRUE /*DrawNow*/);
                NW_ASSERT(status == KBrsrSuccess);
                }

            return EKeyWasConsumed;
            }
        }
    return keyConsumed;
    }

//-----------------------------------------------------------------------------
void
CView::SizeChanged()
    {
    const TRect rect(Rect());
    NW_GDI_Rectangle_t rbDisplayBounds;
    NW_LMgr_BoxVisitor_t visitor;
    NW_LMgr_Box_t* child = NULL;

    rbDisplayBounds.point.x = (NW_GDI_Metric_t)rect.iTl.iX;
    rbDisplayBounds.point.y = (NW_GDI_Metric_t)rect.iTl.iY;
    rbDisplayBounds.dimension.height = (NW_GDI_Metric_t)rect.Height();
    rbDisplayBounds.dimension.width = (NW_GDI_Metric_t)rect.Width();

    TRAPD(err, CreateOffscreenBitmapL( Rect() ));
    if (err != KErrNone)
        {
        return ;
        }

    // IKZ - it is not good to set GC directly to OOC class - may be we shall implement a setter
    iDeviceContext->SetGraphicsContext( (void*)iBitmapContext );

    const NW_GDI_Rectangle_t* oldDisplayBounds = iDeviceContext->DisplayBounds();

    if (rbDisplayBounds.point.x != oldDisplayBounds->point.x ||
        rbDisplayBounds.point.y != oldDisplayBounds->point.y ||
        rbDisplayBounds.dimension.height != oldDisplayBounds->dimension.height ||
        rbDisplayBounds.dimension.width != oldDisplayBounds->dimension.width )
        {
        // We need to protect against the case where the StatusPane changed due to an embedded app.
        // When that happens, we compute all the clipRects based on the wrong size.
        NW_LMgr_RootBox_InvalidateCache (iRootBox);
        }

    // if the width of the rect has changed, we want to reformat the box tree
    if (rbDisplayBounds.dimension.width != oldDisplayBounds->dimension.width )
        {
        // cancel any existing formatting
        ((CLMgr_BoxFormatHandler*)iBoxFormatHandler)->Cancel();

        NW_GDI_Point2D_t origin = *(iDeviceContext->Origin());
        origin.x = 0;
        origin.y = 0;
        iDeviceContext->SetOrigin (&origin);

        iDeviceContext->SetDisplayBounds(&rbDisplayBounds);
        // Iterate through all children and initialize the box bounds to 0
        NW_LMgr_BoxVisitor_Initialize(&visitor, NW_LMgr_BoxOf(iRootBox));

        child = NW_LMgr_BoxVisitor_NextBox(&visitor, NULL);
        // Initialize the coordinates
        NW_GDI_Rectangle_t boxBounds; // = NW_LMgr_Box_GetFormatBounds( child );

        NW_Mem_memset(&(boxBounds), 0, sizeof(NW_GDI_Rectangle_t));
        while ((child = NW_LMgr_BoxVisitor_NextBox(&visitor, NULL)) != NULL)
            {
            NW_LMgr_Box_SetFormatBounds( child, boxBounds );
            NW_LMgr_Box_UpdateDisplayBounds( child, boxBounds );
            }

        (void) Collapse (NULL, NW_FALSE);

        (void) FormatBoxTree (NW_TRUE);

        }

    SetDisplayExtents(&rbDisplayBounds);

    (void) Draw (NW_TRUE /*DrawNow*/);
    }

/* ------------------------------------------------------------------------- */
void
CView::SetAppServices (void* appServices)
    {
    iRootBox->appServices = (NW_HED_AppServices_t*) appServices;
    iDeviceContext->SetFontSizeLevel( (TGDIDeviceContextFontSizeLevel)NW_Settings_GetFontSizeLevel() );
    }

//-----------------------------------------------------------------------------
void CView::SetShell(CShell* shell)
    {
    iShell = shell;
    }

//-----------------------------------------------------------------------------
void CView::DoScrollL(const TInt16 /*aScrollDirection*/, const TUint32 /*aScrollDelta*/ )
    {}

//-----------------------------------------------------------------------------
TBrCtlDefs::TBrCtlElementType CView::GetFocusedElementType()
    {
    TBrCtlDefs::TBrCtlElementType retVal = TBrCtlDefs::EElementNone;
    NW_LMgr_Box_t* currentBox = iCurrentBox;
    NW_GDI_Rectangle_t viewBounds;

    viewBounds = *iDeviceContext->DisplayBounds();
    viewBounds.point = *iDeviceContext->Origin();
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetDisplayBounds( currentBox );

    if ( currentBox == NULL || !NW_GDI_Rectangle_Cross(&viewBounds, &boxBounds, NULL))
        {
        /* if there is no current box or it it not visible */
        retVal = TBrCtlDefs::EElementNone;
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_LMgr_ActiveContainerBox_Class) )
        {
        if (NW_LMgr_ActiveContainerBox_GetActionType(currentBox) == NW_LMgr_ActionType_SendMail)
            {
            retVal = TBrCtlDefs::EElementMailtoAnchor;
            }
        else if (NW_LMgr_ActiveContainerBox_GetActionType(currentBox) == NW_LMgr_ActionType_DialNumber)
            {
            retVal = TBrCtlDefs::EElementTelAnchor;
            }
        else
            {
            retVal = TBrCtlDefs::EElementAnchor;
            }
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_RadioBox_Class) )
        {
        NW_Bool state = NW_FBox_CheckBox_GetState(NW_FBox_CheckBoxOf(currentBox));
        if (state == NW_TRUE)
            {
            retVal = TBrCtlDefs::EElementRadioButtonSelected;
            }
        else
            {
            retVal = TBrCtlDefs::EElementRadioButtonUnSelected;
            }
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_CheckBox_Class) )
        {
        NW_Bool state = NW_FBox_CheckBox_GetState(NW_FBox_CheckBoxOf(currentBox));
        if (state == NW_TRUE)
            {
            retVal = TBrCtlDefs::EElementCheckBoxChecked;
            }
        else
            {
            retVal = TBrCtlDefs::EElementCheckBoxUnChecked;
            }
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_ButtonBox_Class) )
        {
        retVal = TBrCtlDefs::EElementButton;
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_InputBox_Class) )
        {
        if ( NW_FBox_InputBox_IsActive( currentBox ) )
            {
            retVal = TBrCtlDefs::EElementActivatedInputBox;
            }
        else
            {
            retVal = TBrCtlDefs::EElementInputBox;
            }
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_SelectBox_Class) )
        {
        retVal = TBrCtlDefs::EElementSelectBox;
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_TextAreaBox_Class) )
        {
        retVal = TBrCtlDefs::EElementTextAreaBox;
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_FBox_FileSelectionBox_Class) )
        {
        if ( NW_FBox_FileSelectionBox_HasContent( NW_FBox_FileSelectionBoxOf( currentBox ) ) )
            {
            retVal = TBrCtlDefs::EElementFileSelectionBoxWithContent;
            }
        else
            {
            retVal = TBrCtlDefs::EElementFileSelectionBoxNoContent;
            }
        }
    else if ( NW_Object_IsInstanceOf(currentBox, &NW_LMgr_AreaBox_Class) &&
              IsImageMapView())
        {
        retVal = TBrCtlDefs::EElementAreaBox;
        }

    return retVal;
    }

//-----------------------------------------------------------------------------
NW_Bool
CView::InputElementEditComplete( TBool aInputConfirmed )
    {
    NW_Evt_ControlKeyEvent_t event;

    if ( aInputConfirmed )
        {
        NW_Evt_ControlKeyEvent_Initialize(&event, NW_Evt_ControlKeyType_SoftA );
        }
    else
        {
        NW_Evt_ControlKeyEvent_Initialize(&event, NW_Evt_ControlKeyType_SoftB );
        }

    return ProcessEvent( NW_Evt_EventOf(&event) );
    }

//-----------------------------------------------------------------------------

void
CView::HandleNotifyString(TUint32 /*aKeyId*/, const TDesC16& /*aValue*/)
    {
    }

//-----------------------------------------------------------------------------
void
CView::DrawPictographsInText(CBitmapContext& aGc,
                             const CFont& aFont,
                             const TDesC& aText,
                             const TPoint& aPosition)
    {
    if (iPictographInterface)
        {
        iPictographInterface->Interface()->DrawPictographsInText(aGc, aFont, aText, aPosition);
        }
    }

//-----------------------------------------------------------------------------
void
CView::DrawPictographArea()
    {
    Draw (NW_TRUE);
    }

//-----------------------------------------------------------------------------

// Any slow browser initialization can happen here
void CView::AsyncInit()
    {
	TInt ret;
    TRAP(ret, AsyncInitL());
    }

// Delete entries in a given directory
void CView::DeleteDirectoryEntriesL(RFs& aRfs, TPtrC16& aFilePath)
    {
    TInt ret = aRfs.MkDirAll(aFilePath);
    // If temp directory was already created, creating it will fail, so go ahead and empty the entries
    if (ret == KErrAlreadyExists)
        {
        CDir* dir;
        ret = aRfs.GetDir(aFilePath, KEntryAttNormal, ESortNone, dir);
        if (ret == KErrNone)
            {
            CleanupStack::PushL(dir);
            TInt entryCount = dir->Count();
            TInt i;
            TTime theTime;
            theTime.HomeTime();
            for (i = entryCount - 1; i >= 0; i--)
                {
                TEntry entry = (*dir)[i];
                if (theTime.DaysFrom(entry.iModified).Int() >= 1)
                    {
                    HBufC* fullFileName = HBufC::NewL(aFilePath.Length() + entry.iName.Length() + 1);
                    CleanupStack::PushL(fullFileName);
                    fullFileName->Des().Copy(aFilePath);
                    fullFileName->Des().Append(entry.iName);
                    aRfs.Delete(*fullFileName);
                    CleanupStack::PopAndDestroy(); // fullFileName
                    }
                }
            CleanupStack::PopAndDestroy(); // dir
            }
        }
    }

// Any slow browser initialization can happen here
void CView::AsyncInitL()
    {
#ifndef FEATURE_MEMORY_PROFILE
    TUint16* filePath = NW_Settings_GetTempFilesPath();
    if (filePath != NULL)
        {
        TPtrC filePathPtr(filePath);
        CleanupStack::PushL(filePath);
        RFs rfs;
        User::LeaveIfError(rfs.Connect());
        CleanupClosePushL(rfs);
        DeleteDirectoryEntriesL(rfs, filePathPtr);
        filePath[0] = NW_Settings_GetMMCDrive();
        DeleteDirectoryEntriesL(rfs, filePathPtr);
        CleanupStack::PopAndDestroy(2); // rfs, filePath
        }
#endif

    }

//-----------------------------------------------------------------------------

void
CView::HandleActivationEvent()
    {
    NW_Evt_ActivateEvent_t activateEvent;

    NW_Evt_ActivateEvent_Initialize(&activateEvent);

    (void) ProcessEvent( NW_Evt_EventOf(&activateEvent) );
    (void) Draw (NW_TRUE /*DrawNow */);
    }

//-----------------------------------------------------------------------------
void
CView::GenerateAndProcessEvent(const TKimonoEventType& aEvent)
    {
    switch (aEvent)
        {
        case EKmEvAddToPhoneBook:
            {
            NW_XHTML_SaveToPhoneBkEvent_t saveToPbEvent;
            NW_XHTML_SaveToPhoneBkEvent_Initialize(&saveToPbEvent);
            (void) ProcessEvent (NW_Evt_EventOf(&saveToPbEvent));
            break;
            }
        case EKmEvMakeCall:
            {
            NW_Evt_ActivateEvent_t actEvent;
            NW_Evt_ActivateEvent_Initialize (&actEvent);
            (void) ProcessEvent (NW_Evt_EventOf(&actEvent));
            break;
            }
        case EKmEvOpenToExternalApp:
            {
            NW_Evt_OpenViewerEvent_t openViewerEvent;
            NW_Evt_OpenViewerEvent_Initialize (&openViewerEvent);
            (void) ProcessEvent (NW_Evt_EventOf(&openViewerEvent));
            break;
            }
        case EKmEvDownloadObject:
            {
            NW_Evt_OpenViewerEvent_t openViewerEvent;
            NW_Evt_OpenViewerEvent_Initialize (&openViewerEvent);
            (void) ProcessEvent (NW_Evt_EventOf(&openViewerEvent));
            break;
            }
        case EKmEvRemoveFileName:
            {
            NW_Evt_ClearFieldEvent_t clearFieldEvent;
            NW_Evt_ClearFieldEvent_Initialize (&clearFieldEvent);
            (void) ProcessEvent (NW_Evt_EventOf(&clearFieldEvent));
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CView::SetBrowserSettingL
// This method takes two parameters, the setting you wish to 'set' and the value
// you wish to change the setting to.  This method will call methods off the
// CBrowserSettings class which will change the cached preferences and if we are
// a stand-alone browser instance, it will persist the setting by writing the new
// value to the browser's ini file.  If any other action needs to take place as
// a result of the setting change, that code should be placed here.
// -----------------------------------------------------------------------------
void CView::SetBrowserSettingL(TUint aSetting, TUint value)
    {
    switch (aSetting)
        {
        case TBrCtlDefs::ESettingsSmallScreen:
            {
            NW_Settings_SetVerticalLayoutEnabled((NW_Bool)value);
            SwitchVerticalLayout();

            break;
            }
        case TBrCtlDefs::ESettingsAutoLoadImages:
            {
            NW_Settings_SetImagesEnabled((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsFontSize:
            {
            NW_Settings_SetFontSizeLevel((NW_Uint8)value);

            NW_Evt_FontSizeChangeEvent_t fontSizeChangeEvent;
            NW_Evt_FontSizeChangeEvent_Initialize (&fontSizeChangeEvent, (NW_Evt_FontSizeChangeEvent_FontLevel_t)value);
            (void) ProcessEvent (NW_Evt_EventOf(&fontSizeChangeEvent));

            break;
            }
        case TBrCtlDefs::ESettingsTextWrapEnabled:
            {
            NW_Settings_SetTextWrapEnabled((NW_Bool)value);

            NW_GDI_Point2D_t origin;
            Relayout (NW_FALSE);
            origin = *(iDeviceContext->Origin());
            SetOrigin (&origin);
            (void) Draw (NW_TRUE /*DrawNow */);

            break;
            }
        case TBrCtlDefs::ESettingsCookiesEnabled:
            {
            NW_Settings_SetCookiesEnabled((NW_Bool)value);

            //UrlLoader_ChangeFilterLoadStatusL(ECookieFilter, (NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsEmbedded:
            {
            NW_Settings_SetIsBrowserEmbedded((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsCSSFetchEnabled:
            {
            NW_Settings_SetCSSFetchEnabled((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsECMAScriptEnabled:
            {
            NW_Settings_SetEcmaScriptEnabled((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsIMEINotifyEnabled:
            {
            NW_Settings_SetIMEINotifyEnabled((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsSendRefererHeader:
            {
            NW_Settings_SetSendReferrerHeader((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsCharacterset:
            {
            // value is the UID of the charset converter. We don't save it in the settings
            iShell->CharacterSetChangedL(value);
            TUint16 internalEncoding = EAutomatic;
            TInt i;
            for (i = 0; supportedCharset[i].uid != 0; i++)
            {
                if (supportedCharset[i].uid == value)
                {
                    internalEncoding = supportedCharset[i].internalEncoding;
                    break;
                }
            }
            NW_Settings_SetEncoding(internalEncoding);
            break;
            }
        case TBrCtlDefs::ESettingsSecurityWarnings:
            {
            NW_Settings_SetHttpSecurityWarnings((NW_Bool)value);

            break;
            }
        case TBrCtlDefs::ESettingsApId:
            {
            NW_Settings_SetIAPId(value);
			//UrlLoader_SetIAPid(value);

            break;
            }
        default:
            {
            // OK - some settings not of interest to us here
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CView::GetBrowserSettingL
// This method takes one parameter and that is the setting you wish to retrieve
// the value of. GetBrowserSettingL will then call methods off the CBrowserSettings
// class to return the current value of the setting.
// -----------------------------------------------------------------------------
TUint CView::GetBrowserSettingL(TUint aSetting) const
    {
    TUint returnValue = 0;

    switch (aSetting)
        {
        case TBrCtlDefs::ESettingsSmallScreen:
            returnValue = (TUint)NW_Settings_GetVerticalLayoutEnabled();
            break;

        case TBrCtlDefs::ESettingsAutoLoadImages:
            returnValue = (TUint)NW_Settings_GetImagesEnabled();
            break;

        case TBrCtlDefs::ESettingsFontSize:
            returnValue = (TUint)NW_Settings_GetFontSizeLevel();
            break;

        case TBrCtlDefs::ESettingsTextWrapEnabled:
            returnValue = (TUint)NW_Settings_GetTextWrapEnabled();
            break;

        case TBrCtlDefs::ESettingsCookiesEnabled:
            returnValue = (TUint)NW_Settings_GetCookiesEnabled();
            break;

        case TBrCtlDefs::ESettingsEmbedded:
            returnValue = (TUint)CBrowserSettings::Instance()->GetIsBrowserEmbedded();
            break;

        case TBrCtlDefs::ESettingsCSSFetchEnabled:
            returnValue = (TUint)NW_Settings_GetCSSFetchEnabled();
            break;

        case TBrCtlDefs::ESettingsECMAScriptEnabled:
            returnValue = (TUint)NW_Settings_GetEcmaScriptEnabled();
            break;

        case TBrCtlDefs::ESettingsIMEINotifyEnabled:
            returnValue = (TUint)NW_Settings_GetIMEINotifyEnabled();
            break;

        case TBrCtlDefs::ESettingsSendRefererHeader:
            returnValue = (TUint)NW_Settings_GetSendReferrerHeader();
            break;

        case TBrCtlDefs::ESettingsSecurityWarnings:
            returnValue = (TUint)NW_Settings_GetHttpSecurityWarnings();
            break;

        case TBrCtlDefs::ESettingsApId:
            returnValue = (TUint)NW_Settings_GetIAPId();
            break;

        default:
            // if the user has not specified one of the above settings, then we
            // need to leave
            User::Leave(KErrArgument);
            break;
        }

    return returnValue;
    }

//-----------------------------------------------------------------------------
void CView::BrowserSettingChanged( enum TBrowserSetting aBrowserSetting )
    {
    // let the rootbox know about it
    if ( iRootBox )
        {
        switch ( aBrowserSetting )
            {
            case EVerticalLayoutEnabled:
            case EDisableSmallScreenLayout:
                {
                NW_LMgr_RootBox_SetSmallScreenOn( iRootBox, NW_Settings_GetVerticalLayoutEnabled() );
                break;
                }
            case ETextWrapEnabled:
                {
                NW_LMgr_RootBox_SetWrapEnabled( iRootBox, NW_Settings_GetTextWrapEnabled() );
                break;
                }
            default:
                {
                // no default implementation
                break;
                }
            }
        }
    }

//-----------------------------------------------------------------------------
CArrayFixFlat<TBrCtlImageCarrier>*
CView::GetPageImagesL()
    {
    NW_ADT_DynamicVector_t* images = (NW_ADT_DynamicVector_t*)
                                     NW_ADT_ResizableVector_New(sizeof(NW_HED_ViewImageList_Entry_t), 10, 5 );

    if (images == NULL)
        {
        User::LeaveNoMemory();
        }

    // NW_HED_ViewImageList_Entry_t
    NW_HED_DocumentRoot_t* root = iShell->GetDocumentRoot();
    NW_ASSERT(root != NULL);

    // Since we're not checking status, status shows up as an unused variable
    // warning.  Removed declaration of status until CCC decides to actually
    // use it.
    if (IsImageMapView())
        {
        GetViewImageList(images);
        }
    else
        {
        NW_HED_DocumentRoot_GetViewImageList(root, images);
        }

    CArrayFixFlat<TBrCtlImageCarrier>* array = new(ELeave) CArrayFixFlat<TBrCtlImageCarrier>(5);
    CleanupStack::PushL(array);

    NW_ADT_Vector_Metric_t i;
    NW_ADT_Vector_Metric_t size = NW_ADT_Vector_GetSize(images);

    for (i = 0; i < size; i++)
        {
        NW_HED_ViewImageList_Entry_t* entry =
            (NW_HED_ViewImageList_Entry_t*)NW_ADT_Vector_ElementAt(images, i);

        TPtrC8 rawData( entry->rawData, entry->rawDataLen );

        TPtrC url( NULL, 0 );
        if (entry->url != NULL)
            {
            url.Set( entry->url );
            }

        TPtrC altText( NULL, 0 );
        if (entry->altText != NULL)
            {
            altText.Set( entry->altText );
            }

        TBrCtlImageType type = EImageTypeAny;

        switch (entry->imageType)
            {
            case ERecognizedImage:
            case EUnsupportedImage:
                type = EImageTypeAny;
                break;
            case EWbmpImage:
                type = EImageTypeWbmp;
                break;
            case EOtaImage:
                type = EImageTypeOta;
                break;
            default:
                break;
            }

		TPtrC contentType( NULL, 0 );
		if(entry->contentType != NULL)
			{
            contentType.Set(entry->contentType);
			}
        TBrCtlImageCarrier carrier (rawData, url, altText, type, contentType);
        array->AppendL(carrier);
        }

    CleanupStack::Pop();    // array
    NW_Object_Delete(images);

    return array;
    }

//-----------------------------------------------------------------------------
// UpdateScrollBarsL
//
// If the BrCtl supports scrollBars (ECapabilityDisplayScrollBar) we display
// and handle the scrollBars within the BrowserEngine and inform the
// BrCtlLayoutObserver listener. If the BrCtrl doesn't support scrollBars, we
// only inform the BrCtlLayoutObserver listener that a scroll event occurred.
//-----------------------------------------------------------------------------
void
CView::UpdateScrollBarsL(CEikScrollBar::TOrientation aOrientation,
                         const TInt aThumbPos,
                         const TInt aScrollSpan )
    {
    TInt document = aScrollSpan; // Span of the web page
    TInt display = 0;    // Span of the display
    TInt displayPos = aThumbPos; // Position of the display within the web page

    // Set layout
    NW_LMgr_RootBox_t* rootBox;
    rootBox = iRootBox;

    // Get the BrCtl scrolling provider, if we have it. Can be NULL, if
    // then the parent app is providing the scrolling, supported through the
    // BrCtlLayoutObserver events.
/*    ScrollingProvider* brCtlScrolling = iBrCtl->scrollingProvider();
*/
    // Page direction
    if ( NW_LMgr_RootBox_IsPageRTL( rootBox ) )
        {
        // Right to left page
/*        if (brCtlScrolling)
            {            
//	     iWmlControl->WKWmlInterface()->WKScrollingProviderNotifyLayoutChange(EOriginTopRight);
            }
*/        iBrCtl->brCtlLayoutObserver()->NotifyLayoutChange( EOriginTopRight );
        }
    else
        {
        // Left to right page
/*        if (brCtlScrolling)
            {          
//            iWmlControl->WKWmlInterface()->WKScrollingProviderNotifyLayoutChange(EOriginTopLeft);
            }
*/         iBrCtl->brCtlLayoutObserver()->NotifyLayoutChange( EOriginTopLeft );
        }

    // Page orientation
    if ( aOrientation == CEikScrollBar::EHorizontal )
        {
        // Horizontal scrollbar
        display = Rect().Width();

        // Small (Narrow) Screen Mode
        if ( NW_LMgr_RootBox_GetSmallScreenOn( iRootBox ) )
            {
            document = display;
            }

/*        if (brCtlScrolling)
            {           
//            iWmlControl->WKWmlInterface()->WKSetScrollingProviderUpdateHScrollBarL(document, display, displayPos);
            }
*/        iBrCtl->brCtlLayoutObserver()->UpdateBrowserHScrollBarL(document, display, displayPos );
        }
    else
        {
        // Vertical scrollbar
        display = Rect().Height();
/*        if (brCtlScrolling)
            {           
//            iWmlControl->WKWmlInterface()->WKSetScrollingProviderUpdateVScrollBarL(document, display, displayPos);
            }
*/        iBrCtl->brCtlLayoutObserver()->UpdateBrowserVScrollBarL(document, display, displayPos );
        }
    }

//-----------------------------------------------------------------------------
TInt
CView::CountComponentControls() const
    {
    TInt componentControls = 0;

    // If there's an active input element on the screen - draw it
    // because of T9 underline
    NW_LMgr_Box_t* currentBox = iCurrentBox;

    if (NW_Object_IsInstanceOf(currentBox, &NW_FBox_InputBox_Class) &&
        NW_FBox_InputBox_IsActive( currentBox ))
        {
        NW_FBox_FormBox_t* formBox = NW_FBox_FormBoxOf(currentBox);

        NW_FBox_Epoc32InputSkin_t* inputSkin = (NW_FBox_Epoc32InputSkin_t*) NW_FBox_FormBox_GetSkin(formBox);

        CCoeControl* inputElement = (CCoeControl*)inputSkin->cppEpoc32InputBox;
        if (inputElement)
            {
            componentControls++;
            }
        }

    return componentControls;
    }

//-----------------------------------------------------------------------------
CCoeControl*
CView::ComponentControl(TInt /*aIndex*/) const
    {
    NW_LMgr_Box_t* currentBox = iCurrentBox;

    if (NW_Object_IsInstanceOf(currentBox, &NW_FBox_InputBox_Class) &&
        NW_FBox_InputBox_IsActive( currentBox ))
        {
        NW_FBox_FormBox_t* formBox = NW_FBox_FormBoxOf(currentBox);

        NW_FBox_Epoc32InputSkin_t* inputSkin = (NW_FBox_Epoc32InputSkin_t*) NW_FBox_FormBox_GetSkin(formBox);

        CCoeControl* inputElement = (CCoeControl*)inputSkin->cppEpoc32InputBox;
        if (inputElement)
            {
            return inputElement;
            }
        }

    return NULL;
    }

//-----------------------------------------------------------------------------
void
CView::CreateOffscreenBitmapL( const TRect& aRect )
    {
    delete iBitmapContext;
    iBitmapContext = NULL;
    delete iBitmapDevice;
    iBitmapDevice = NULL;
    delete iOffscreenBitmap;
    iOffscreenBitmap = NULL;

    // create a bitmap to be used offscreen
    iOffscreenBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iOffscreenBitmap->Create( TSize( aRect.Width(), aRect.Height() ), EColor4K ) );

    // create and offscreen device and context
    iBitmapDevice = CFbsBitmapDevice::NewL( iOffscreenBitmap );
    User::LeaveIfError( iBitmapDevice->CreateBitmapContext( iBitmapContext ) );
    }

//-----------------------------------------------------------------------------
void
CView::HandleResourceChange(TInt aType)
    {
    if (aType == KEikMessageColorSchemeChange)
        {
        (void) Draw (NW_TRUE /*DrawNow */);
        DrawNow();
        }
    }


//-----------------------------------------------------------------------------
// Return the number of active elements in the page
// This is used by the UI to decide if to display CANCEL on the right softkey
// when an object is activated
//-----------------------------------------------------------------------------
TUint
CView::GetActiveElements()
    {
    return iTabList->size;
    }

// -------------------------------------------------------------------------
//    CView::GetViewImageList
// -------------------------------------------------------------------------
void
CView::GetViewImageList(void* aDynamicVector)
    {
    NW_LMgr_ImageMapBox_t* imageMapBox = NULL;
    NW_Image_Epoc32Simple_t* image = NULL;
    NW_Image_Virtual_t* virtualImage = NULL;
    NW_HED_ViewImageList_Entry_t* entry = NULL;
    NW_ADT_DynamicVector_t* dynamicVector = NULL;
    NW_LMgr_RootBox_t* rootBox = NULL;
    NW_LMgr_Box_t* currentBox = NULL;

    dynamicVector = (NW_ADT_DynamicVector_t*)aDynamicVector;
    rootBox = iRootBox;
    NW_ASSERT(rootBox);

    currentBox = rootBox->focusBox;

    // in this case the current box should be an area box
    if (currentBox && NW_Object_IsClass(currentBox, &NW_LMgr_ImageMapBox_Class))
        {
        imageMapBox = NW_LMgr_ImageMapBoxOf(currentBox);
        NW_ASSERT(imageMapBox);

        virtualImage = (NW_Image_Virtual_t*)imageMapBox->super.super.virtualImage;
        NW_ASSERT(virtualImage);

        image = NW_Image_Epoc32SimpleOf (virtualImage->image);

        /* Save the text object in the dynamicVector */
        entry = (NW_HED_ViewImageList_Entry_t*)NW_ADT_DynamicVector_InsertAt(dynamicVector, NULL,
                NW_ADT_Vector_AtEnd);
        if (entry)
            {
            entry->rawData = (NW_Byte*)image->rawData;
            entry->rawDataLen = image->rawDataLength;
            entry->imageType = image->imageType;

            if (imageMapBox->iUrl)
                {
                entry->url = (NW_Ucs2*)imageMapBox->iUrl->storage;
                }
            else
                {
                entry->url = NULL;
                }

            if (imageMapBox->super.super.altText)
                {
                entry->altText = (NW_Ucs2*)imageMapBox->super.super.altText->storage;
                }
            else
                {
                entry->altText = NULL;
                }
            }
        }
    }

NW_Bool

CView::IsZeroBox (const NW_LMgr_Box_t* box) const

    {
    NW_GDI_Rectangle_t boxBounds;

    /* parameter assertion block */
    NW_ASSERT (box != NULL);

    CView* view = const_cast<CView*>(this);
    boxBounds = view->GetBoxDisplayBounds(NW_LMgr_BoxOf(box));

    if ((boxBounds.point.y == 0) &&
        (boxBounds.point.x == 0) &&

        (boxBounds.dimension.height == 0) &&
        (boxBounds.dimension.height == 0))
        {
        NW_LOG0(NW_LOG_LEVEL4, "[CView::IsZeroBox Error! (0.0; 0x0) box in the iTabList]");

        return NW_TRUE;

        }
    return NW_FALSE;
    }

//-------------------------------------------------------------------------------
// DrawDocumentPart ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
void CView::DrawDocumentPart(
    CFbsBitGc& aGc,
    CFbsBitmap* /*aBitmap*/,
    const TRect& aDocumentAreaToDraw )
    {
    TRect drawRect( aDocumentAreaToDraw ) ;
    DrawHistory( aGc, drawRect );
    }

//-------------------------------------------------------------------------------
// DocumentViewport ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
TRect CView::DocumentViewport() 
    {
    return Rect();
    }

//-------------------------------------------------------------------------------
// ScaledPageChanged ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
void CView::ScaledPageChanged(
    const TRect& /*aRect*/,
    TBool aFullScreen,
    TBool aScroll )
    {
    HistoryControllerInterface* historyController = &( iWmlControl->HistoryController() );
    if ( historyController->historyViewEnabled() && iDocumentFinished && !aScroll && aFullScreen)
        {
        // update the history with new bitmap
        CFbsBitmap* scaledPage = iPageScaler->ScaledPage();
        if(scaledPage)
            {
            // Get the browser control rect
	     TRAP_IGNORE( iWmlControl->HistoryController().updateHistoryEntryThumbnailL(scaledPage));
            // ignore err since we will use the default image
            }

        }
    }

//-------------------------------------------------------------------------------
// DocumentSize ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
TSize CView::DocumentSize()
    {
	TSize contentSize;
	ContentSize( contentSize );
    return contentSize;
	}


//-------------------------------------------------------------------------------
// InitializePageScalerL
//
//-------------------------------------------------------------------------------
void CView::InitializePageScalerL()
    {
    TBool lowQuality = !( iBrCtl->capabilities() & TBrCtlDefs::ECapabilityGraphicalHistory );
    iPageScaler = CPageScaler::NewL( *this, EColor64K, lowQuality );
    iPageScaler->SetVisible( EFalse );
    iBrCtl->setWmlPageScaler( *iPageScaler );
    }


//-------------------------------------------------------------------------------
// DrawPositionedBoxes
//
//-------------------------------------------------------------------------------
TBrowserStatusCode CView::DrawPositionedBoxes()
    {
    TBrowserStatusCode status = KBrsrSuccess;
    if (!iRootBox->iPositionedBoxesPlaced)
        return status;
    NW_ADT_DynamicVector_t *positionedObjects = iRootBox->positionedAbsObjects;
    TInt count = NW_ADT_Vector_GetSize(positionedObjects);
    TUint16 index;
    NW_LMgr_Box_t* positionedBox;
    for (index = 0; index < count; index++)
        {
        positionedBox = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (positionedObjects, index);  
        Refresh(positionedBox);
        }
    return status;
    }

NW_Bool CView::FormatPositionedBoxes ()
    {
 
	NW_LMgr_Box_t *positionedBox;
	NW_LMgr_PosFlowBox_t* tempPositionedBox;
    NW_ADT_DynamicVector_t *positionedObjects = iRootBox->positionedAbsObjects;
    // Place the positioned boxes only once
    if (iRootBox->iPositionedBoxesPlaced)
        return NW_FALSE;
    
    //handle positioned boxes
    NW_ADT_Vector_Metric_t posCount, index;
    posCount = NW_ADT_Vector_GetSize(positionedObjects);
    for (index = 0; index < NW_ADT_Vector_GetSize(positionedObjects); index++)
        {
        positionedBox = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (positionedObjects, index);
        if (!NW_Object_IsInstanceOf(positionedBox, &NW_LMgr_PosFlowBox_Class))
            {
            NW_ASSERT(0); // should never happen
            break;
            }
        ReformatBox(positionedBox);
		tempPositionedBox = (NW_LMgr_PosFlowBox_t*)positionedBox;
		if( (tempPositionedBox != NULL) &&
			(NW_Object_IsInstanceOf(tempPositionedBox, &NW_LMgr_PosFlowBox_Class) ) &&
			(tempPositionedBox->iContainingBlock != NULL) && 
				  NW_Object_IsInstanceOf(tempPositionedBox->iContainingBlock, &NW_LMgr_Box_Class) )
			{

              NW_LMgr_PosFlowBox_HandlePostFormat((NW_LMgr_PosFlowBox_t*)positionedBox);
			}
        }
    iRootBox->iPositionedBoxesPlaced = NW_TRUE;
    return (posCount > 0);
	
	//return TRUE;
    }

void CView::ScrollTo(TPoint aPoint)
    {
    NW_GDI_Point2D_t origin;
    origin.x = aPoint.iX;
    origin.y = aPoint.iY;
    if (iCurrentBox && NW_Object_IsInstanceOf(iCurrentBox, &NW_FBox_InputBox_Class) && 
        NW_FBox_InputBox_IsActive( iCurrentBox ))
        {
        if (!InputElementEditComplete( NW_TRUE ))
            {
            return;
            }
        }
    TBool scrollDown = aPoint.iY > iDeviceContext->Origin()->y;
    SetOrigin(&origin);
    GetDeviceContext()->SetOrigin(&origin);
    // Is box still visible?
    TRect rect(0, 0, 0, 0);
    NW_GDI_Rectangle_t displayBounds;
    displayBounds = *(iDeviceContext->DisplayBounds());
    displayBounds.point = *(iDeviceContext->Origin());
    if (!iCurrentBox || !IsBoxVisible(iCurrentBox, &displayBounds))
        {
        NW_LMgr_Box_t* box;
        if (scrollDown)
            {
            box = GetVisibleBox();
            }
        else
            {
            box = GetVisibleBoxReverse();
            }            
        if (box)
            {
            NW_LMgr_Box_SetHasFocus (iCurrentBox, NW_FALSE);
            NW_LMgr_Box_SetHasFocus (box, NW_TRUE);
            SetCurrentBox(box);
            NW_LMgr_RootBox_SetFocusBox(iRootBox, box);
            }
        }    
    Draw(NW_TRUE);
    }


// ============================ MEMBER FUNCTIONS ===============================
