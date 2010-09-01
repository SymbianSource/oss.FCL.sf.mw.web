/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "BoxRender.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_adt_resizablevector.h"
#include "nw_adt_vectoriterator.h"
#include "nw_lmgr_eventhandler.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nwx_logger.h"
#include "nwx_settings.h"

#include "CStack.h"

struct RenderContext
{
  NW_LMgr_ContainerBox_t* node;
  NW_ADT_Vector_Metric_t childCount;
  NW_ADT_Vector_Metric_t childIndex;
  NW_GDI_Rectangle_t clipRect;
  NW_Bool hasFocus;
};


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
BoxRender_Render( NW_LMgr_Box_t* box,
                  CGDIDeviceContext* deviceContext,
                  NW_GDI_Rectangle_t* clipRect,
                  NW_LMgr_Box_t* currentBox,
                  NW_Uint8 flags, 
                  NW_Bool hasFocus,
                  NW_LMgr_RootBox_t* rootBox )
  {
  NW_ADT_Vector_Metric_t childCount;
  NW_LMgr_EventHandler_t* eventHandler = NULL;
  NW_Bool skipChildren = NW_FALSE;
  CStack<RenderContext, ETrue>* renderStack = NULL;
  NW_Bool smallScreenMode;

	// for non-container box, render it and return 
  if( !NW_Object_IsInstanceOf( box, &NW_LMgr_ContainerBox_Class ) )
	  {
    // currentBox: Special handling for active containers 
    if( NW_Object_IsInstanceOf( currentBox, &NW_LMgr_ActiveContainerBox_Class ) ) 
      {
      eventHandler = NW_LMgr_ActiveContainerBoxOf( currentBox )->eventHandler;
      
      if( NW_Object_IsInstanceOf( box, &NW_LMgr_ActiveContainerBox_Class ) ) 
        {
        if (NW_LMgr_ActiveContainerBoxOf(box)->eventHandler == eventHandler) 
          {
          currentBox = box;
          }
        }   
      }     
    return NW_LMgr_Box_Render( box, deviceContext, clipRect, currentBox, 
 		                           flags, &hasFocus, &skipChildren, rootBox );
    } 

  NW_TRY( status ) 
    {
    // skip rootbox by getting its first child
    if( NW_Object_IsClass( box, &NW_LMgr_RootBox_Class ) )
      {
      box = NW_LMgr_ContainerBox_GetChild( NW_LMgr_ContainerBoxOf( box ), 0 );
      if( !box )
        {
        // just return
        NW_THROW_SUCCESS( status );
        }
      }
    renderStack = new CStack<RenderContext, ETrue>;
    NW_THROW_OOM_ON_NULL( renderStack, status );

    // optimize tree rendering only when the format has been completed.
    // smallScreenMode = NW_Settings_GetVerticalLayoutEnabled();
    // ZAL: temp 
    smallScreenMode = NW_FALSE;
    do
      {      
      // currentBox: Special handling for active containers 
      if( NW_Object_IsInstanceOf( currentBox, &NW_LMgr_ActiveContainerBox_Class ) ) 
        {
        eventHandler = NW_LMgr_ActiveContainerBoxOf( currentBox )->eventHandler;
        
        if( NW_Object_IsInstanceOf( box, &NW_LMgr_ActiveContainerBox_Class ) ) 
          {
          if (NW_LMgr_ActiveContainerBoxOf(box)->eventHandler == eventHandler) 
            {
            currentBox = box;
            }
          }   
        }     
      
			// some box will change *clipRect in its render function, such as StaticTableCellBox
      status = NW_LMgr_Box_Render( box, deviceContext, clipRect, currentBox, 
	 	                               flags, &hasFocus, &skipChildren, rootBox );            

      // in small screen mode we do not draw children boxes 
      // if the parent set skipChildren to true
      if( smallScreenMode && skipChildren )
        {
        // container is off the view.
        // no children draw
        childCount = 0;
        }
      else if( NW_Object_IsInstanceOf( box, &NW_LMgr_ContainerBox_Class ) )
        {
        // visible container
        childCount = NW_LMgr_ContainerBox_GetChildCount( NW_LMgr_ContainerBoxOf( box ) );        
        }
      else
        {
        // no children as the box is not a container
        childCount = 0;
        }

      if( childCount > 0 )
        {
        // node

        // push container to the stack
        RenderContext* renderCtx = new RenderContext;
        //lint -e{774} Boolean within 'if' always evaluates to False
        NW_THROW_OOM_ON_NULL( renderCtx, status );
		
        renderCtx->node = NW_LMgr_ContainerBoxOf( box );
		    renderCtx->clipRect = *clipRect;
        renderCtx->hasFocus = hasFocus;
        renderCtx->childCount = childCount;
        renderCtx->childIndex = 0;
        TRAPD( err, renderStack->PushL( renderCtx ) );
        if( err == KErrNoMemory )
          {
          status = KBrsrOutOfMemory;
          }
        else if( err != KErrNone )
          {
          status = KBrsrFailure;
          }
        else
          {
          status = KBrsrSuccess;
          }
        _NW_THROW_ON_ERROR( status );
        // and get the first child
        box = NW_LMgr_ContainerBox_GetChild ( renderCtx->node, 0 );
        }
      else 
        {
        // leaf or node whose children must be skipped

        // if the very first box has no children to render
        // then we need to return right away
        if( renderStack->IsEmpty() )
          {
          NW_THROW_SUCCESS( status );
          }
        // check if the box has a sibling to render.
        RenderContext* parent = renderStack->Head();

        // loop until we get the next sibling. note that if the current box
        // is the last child of the parent, then we jump one level up and
        // start checking the parent's sibling as long as we get a sibling
        while( parent->childIndex == ( parent->childCount - 1 ) )
          {
          // pop off parent until we get the next sibling
          parent = renderStack->Pop();
          // restore clipRect: it's important for using clipRect to draw float in RootBox_Render
          *clipRect = parent->clipRect;
          // do not free the box
		      parent->node = NULL;
          delete parent;
          // get its parent
          // empty stack means that we have no more
          // box to render
          if( renderStack->IsEmpty() )
            {
            NW_THROW_SUCCESS( status );
            }
          // get next parent
          parent = renderStack->Head();
          }

        // update *clipRect.
        *clipRect = parent->clipRect;

        // we have the next sibling
        // update the parent index to be able to
        // keep track of the last drawn child
        parent->childIndex++;
        box = NW_LMgr_ContainerBox_GetChild ( parent->node, parent->childIndex );
        // get context
        hasFocus = parent->hasFocus;
        }      
      }
    while( box );
    }
  NW_CATCH( status ) 
    {
    }
  NW_FINALLY 
    {
    // pop off nodes if the stack is not empty. 
    // in fact, it should always be empty
    // do not use PopAndDestroy as that frees
    // boxes in the stack
    if( renderStack && !renderStack->IsEmpty() )
      {
      RenderContext* parent;
      do
        {
        parent = renderStack->Pop();
        NW_ASSERT( parent != NULL );
        // do not free the box
		    parent->node = NULL;
        delete parent;
        }
      while( !renderStack->IsEmpty() );
      }
    delete renderStack;
    return status;
    }
  NW_END_TRY
  }



