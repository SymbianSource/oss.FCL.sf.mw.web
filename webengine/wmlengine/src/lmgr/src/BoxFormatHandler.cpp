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
* Description:  Description : The class handles formatting of the boxes , refreshes the boxe on the postformatting,
*                adds the active boxes  to the tab list
*
*/

// INCLUDE FILES
#include "BoxFormatHandler.h"
#include "nwx_logger.h"
#include "nwx_settings.h"
#include "GDIDeviceContext.h"
#include "LMgrMarkerText.h"
#include "LMgrMarkerImage.h"
#include "LMgrAnonBlock.h"
#include "nw_lmgr_imagemapbox.h"
#include "nw_lmgr_posflowbox.h"

#include <f32file.h>
#include <flogger.h>

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint KRootBoxInitialHeight = 2;

// MACROS

// LOCAL CONSTANTS AND MACROS
//lint --e{752} local declarator not referenced
#ifdef _DEBUG_LOG
_LIT( KWmlBrowserLogDir, "browser" );
_LIT( KLogFileName, "boxFormat.txt" );
#endif

// MODULE DATA STRUCTURES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

//
// returns last formatted box
// @return - last formatted box of the box tree
//
NW_LMgr_Box_t* CLMgr_BoxFormatHandler::GetLastBox()
    {
    NW_ADT_Vector_Metric_t numChildren = 0;

    iLastBoxToFormat = iRootBox;
    do
        {
        const NW_ADT_Vector_t* children = NW_LMgr_ContainerBox_GetChildren ((NW_LMgr_ContainerBox_t*)iLastBoxToFormat);
        numChildren = NW_ADT_Vector_GetSize (children );
        if (numChildren == 0 )
            break;
        numChildren --;
        iLastBoxToFormat = *(NW_LMgr_Box_t**) NW_ADT_Vector_ElementAt (children, numChildren );
        }
    while (NW_Object_IsInstanceOf (iLastBoxToFormat ,
                                   &NW_LMgr_ContainerBox_Class));
    NW_ASSERT (iLastBoxToFormat);
    return iLastBoxToFormat;
    }

//
// The method clears all the items on the format stack and sets the current box to
// formatted to NULL
//
void CLMgr_BoxFormatHandler::ClearFormatStack()
    {
    while (!iFormatStack->IsEmpty())
        {
        CFormatStackContext* formatStackContext = iFormatStack->Pop();
        delete formatStackContext ;
        }
    }

void CLMgr_BoxFormatHandler::CFormatStackContext::CleanupStackContext(TAny* aFormatStackContext)
    {
    CFormatStackContext* formatStackContext = (CFormatStackContext*)aFormatStackContext;
    delete formatStackContext;
    }

// deletes the format context if the reference count is zero else if > 0 decrements the reference count
CLMgr_BoxFormatHandler::CFormatStackContext::~CFormatStackContext()
    {
    if (iParentContext->newFormatContext)
        {
        if (iParentContext->newFormatContext != iParentContext &&
	        iParentContext->newFormatContext->referenceCount == 0)
            {
#ifdef _DEBUG
            NW_LOG2(NW_LOG_LEVEL5, "Delete FormatContext :::::Pop box %x , context %x", iBox, iParentContext->newFormatContext);
#endif
            iParentContext->newFormatContext = NULL;
            }
        else
            {
            iParentContext->newFormatContext->referenceCount--;
            }
        }
    }

// default contstructor
CLMgr_BoxFormatHandler::CFormatStackContext::CFormatStackContext(CLMgr_BoxFormatHandler* aBoxFormatHandler) 
    {
    iBoxFormatHandler = aBoxFormatHandler;
    }

// copy constructor
CLMgr_BoxFormatHandler::CFormatStackContext::CFormatStackContext(const CFormatStackContext&
        aFormatStackContext)
    {
    iBox = aFormatStackContext.iBox;
    iParentFormatBox = aFormatStackContext.iParentFormatBox;
    iParentContext = aFormatStackContext.iParentContext;
    iBoxFormatHandler = aFormatStackContext.iBoxFormatHandler ;
    // iParentContext->newFormatContext can be NULL if the box is a container  box
    if(iParentContext->newFormatContext)
      {
      iParentContext->newFormatContext->referenceCount++;
      }
    }


//
// The method handles post formatting on a box
// @param aFormatStackContext- Formatting context instance to be post formatted
//
void CLMgr_BoxFormatHandler::PostFormatL(CFormatStackContext& aFormatStackContext)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    TLMgr_BoxFormatter& formatter = GetBoxFormatter(aFormatStackContext.iBox);

    if ((NW_Object_IsInstanceOf (aFormatStackContext.iBox, &NW_LMgr_MarqueeBox_Class) ||
         NW_Object_IsInstanceOf (aFormatStackContext.iBox, &NW_LMgr_BidiFlowBox_Class)) &&
      NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aFormatStackContext.iBox )) > 0)
        {
        status = formatter.NW_LMgr_BidiFlowBox_PostFormat(aFormatStackContext.iParentContext->formatBox, NW_LMgr_ContainerBoxOf(aFormatStackContext.iBox ), aFormatStackContext.iParentContext->newFormatContext);
        }
    if (status == KBrsrSuccess)
        {
        status = NW_LMgr_Box_PostResize(aFormatStackContext.iBox);
        if (status == KBrsrSuccess)
            {
            status = formatter.NW_LMgr_FlowBox_PostFormatBox(aFormatStackContext.iParentFormatBox, aFormatStackContext.iBox , aFormatStackContext.iParentContext );
            }
        }
    // interested in OOM , other errors are ignored
    if (status == KBrsrOutOfMemory)
        {
        User::Leave(KErrNoMemory);
        }

    // update display
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( iRootBox );

    if( NW_LMgr_RootBox_GetSmallScreenOn( NW_LMgr_RootBoxOf( rootBox ) ) )
      {
      NW_LMgr_Property_t prop;

      if (NW_LMgr_Box_GetProperty(aFormatStackContext.iBox, NW_CSS_Prop_gridModeApplied,
          &prop) == KBrsrNotFound)
        {
        // update and refresh display
        UpdateDisplayBounds( aFormatStackContext.iBox );
        RefreshBoxL( aFormatStackContext.iBox );

        // Add it to the tab list
        AddTabItemL( aFormatStackContext.iBox );
        }
      }
    else
      {
      NW_GDI_Rectangle_t bounds = NW_LMgr_Box_GetFormatBounds( aFormatStackContext.iBox );
      // if the box is in a cell, then the x any y coordinates do not get updated until
      // the cellbox has not been resized so
      // do not update until we get the real coordinates
      if( bounds.point.x != 0 && bounds.point.y != 0)
        {
        UpdateDisplayBounds( aFormatStackContext.iBox );
        RefreshBoxL( aFormatStackContext.iBox );
        }      
      }
    }

void CLMgr_BoxFormatHandler::UpdateDisplayBounds( NW_LMgr_Box_t* aBox )
  {
  NW_GDI_Rectangle_t bounds;
  NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( iRootBox );
  NW_Bool smallscreeOn = NW_LMgr_RootBox_GetSmallScreenOn( rootBox );

  if( !iBackgroundFormat )
    {
    if( NW_Object_IsInstanceOf( aBox, &NW_LMgr_ContainerBox_Class ) == NW_TRUE )
      {
      NW_LMgr_ContainerBox_t* container = NW_LMgr_ContainerBoxOf( aBox );
      NW_ADT_Vector_Metric_t numChildren = NW_LMgr_ContainerBox_GetChildCount( container );

      for( NW_ADT_Vector_Metric_t index = 0; index < numChildren; index++ )
        {
        NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild( container, index );

        if( NW_Object_IsInstanceOf( child, &NW_LMgr_ContainerBox_Class ) == NW_TRUE )
          {
            // If you have the following markup: <a><div>Text</div></a>, The parentFlow is the div 
            // box - that gets formatted, but not the container for <a>. We need to add extra code to 
            // Resize the container. According to CSS, an inline container cannot have a block level
            // box and InitializeTree normalizes the tree. So, we wouldn't be in this situation in normal mode.
            // But in small screen mode, initialize tree is not run - so we need to handle that.
          if( smallscreeOn )
              {
              // that means that the child is either Container or ActiveContainer
              if (!NW_Object_IsInstanceOf(child, &NW_LMgr_FormatBox_Class))
                  {
                  bounds = NW_LMgr_Box_GetFormatBounds( child );
                  if ((bounds.dimension.width == 0) || (bounds.dimension.height == 0))
                      {
                      (void)NW_LMgr_ContainerBox_ResizeToChildren(NW_LMgr_ContainerBoxOf(child));
                      }
                  }
              }
          UpdateDisplayBounds( child );
          
          // Add table cellbox to the table list while in normal screen mode or 
          // when the cellbox is in a grid mode table.
          NW_LMgr_Property_t prop;

          if( !smallscreeOn || (NW_LMgr_Box_GetProperty(child, NW_CSS_Prop_gridModeApplied,
              &prop) == KBrsrSuccess))
            {
			//if (!NW_Object_IsInstanceOf(child, &NW_LMgr_FormatBox_Class))
				if (NW_Object_IsInstanceOf(child, &NW_LMgr_ActiveContainerBox_Class))
				{
					NW_LMgr_Box_t* tmpParent = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(child);
					NW_LMgr_Box_t* tmpGrandParent = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(tmpParent);
					if (NW_Object_IsInstanceOf(tmpParent, &NW_LMgr_BidiFlowBox_Class) &&
						NW_Object_IsInstanceOf(tmpGrandParent, &NW_LMgr_RootBox_Class))
					{
						// we need to add active boxes contained within the ContainerBox.
						// Active boxes within the FormatBox were added when PostFormat
						// was called
						AddSubTreeTabItemsL(child);// and add it to the tab list
					}
				}
				else
				{
					// and add it to the tab list
					AddTabItemL( child );
				}
			}
          }
        else if (NW_Object_IsClass (child->parent, &NW_LMgr_MarqueeBox_Class) )
        {
          bounds = NW_LMgr_Box_GetFormatBounds( child );
          NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( child->parent ), bounds );
        }
        else
          {
          bounds = NW_LMgr_Box_GetFormatBounds( child );
          NW_LMgr_Box_UpdateDisplayBounds( child, bounds );
          }
        }
      }
    bounds = NW_LMgr_Box_GetFormatBounds( aBox );
    NW_LMgr_Box_UpdateDisplayBounds( aBox, bounds );
    }
  }

//
// A two phase constructor that constructs  and initializes CLMgr_BoxFormatHandler class
// leaves KErrNoMemory if out of memory
// @param aRootBox - flowBox to be formatted
// @param aSelectiveFormatting is ETrue if only a flowBox  and its children needs to be formatted
// @return CLMgr_BoxFormatHandler* - pointer to the instance of  class CLMgr_BoxFormatHandler
//
CLMgr_BoxFormatHandler* CLMgr_BoxFormatHandler::NewL(NW_LMgr_Box_t* aRootBox, TBool aSelectiveFormatting)
    {
    CLMgr_BoxFormatHandler* self = new (ELeave)CLMgr_BoxFormatHandler;
    CleanupStack::PushL(self );
    self->ConstructL(aRootBox, aSelectiveFormatting);
    CleanupStack::Pop();
    return self;
    }

// default contructor
CLMgr_BoxFormatHandler::CLMgr_BoxFormatHandler(): CActive( EPriorityLow + 2 )
    {
    iFormattingPriority = ((CActive::TPriority)( CActive::EPriorityLow + 2 ));
    iBackgroundFormat = EFalse;  
	iFormattingCompleted = EFalse;
    }

// CLMgr_BoxFormatHandler initializer , creates iFormatStack and adds the instance
// to CActiveScheduler
void CLMgr_BoxFormatHandler::ConstructL(NW_LMgr_Box_t* aRootBox, TBool aSelectiveFormatting)
    {
    iFormatStack = new (ELeave)CStack<CFormatStackContext, ETrue>;
    iFormatContextRef = new (ELeave)CArrayPtrFlat<NW_LMgr_FormatContext_t>(2);
    iRootBox = aRootBox;
    iFormatBox = NULL;
    iSelectiveFormatting = aSelectiveFormatting;
    CActiveScheduler::Add(this);
    }

// initialize the flowbox and its children
TBrowserStatusCode CLMgr_BoxFormatHandler::InitializeBox(NW_LMgr_BidiFlowBox_t *flow)
    {
    return iTLMgr_BoxFormatter.NW_LMgr_BidiFlowBox_Initialize(flow);
    }

// initialize the flowbox and its children in small screen mode
TBrowserStatusCode CLMgr_BoxFormatHandler::InitializeBoxSSL(NW_LMgr_BidiFlowBox_t*)
    {
    return KBrsrSuccess;
    }

// destructor - deletes iFormatStack and removes the instance from CActiveScheduler
CLMgr_BoxFormatHandler::~CLMgr_BoxFormatHandler()
    {
    Cancel();
    if(iFormatContextRef)
        {
        for(TInt index = 0 ; index < iFormatContextRef->Count(); index++)
            {
            NW_Object_Delete(iFormatContextRef->At(index));
            }
        delete iFormatContextRef;
        }
    delete iFormatStack;
    }

void CLMgr_BoxFormatHandler::AppendFormatContextL(NW_LMgr_FormatContext_t* aLMgr_FormatContext_t)
    {
    TBool found = EFalse;
    for(TInt index = 0 ; index < iFormatContextRef->Count(); index++)
        {
        if(iFormatContextRef->At(index) == aLMgr_FormatContext_t)
            {
            found = ETrue;
            break;
            }
        }

    if(!found)
        {
        iFormatContextRef->AppendL(aLMgr_FormatContext_t);    
        }
    }

//
// Finds the format context of the next box of aFormatBox to be formatted
// If the aFormatBox is NULL , then a new FormatContext is created on rootbox and
// the child of the rootbox becomes the next box to be formattec
// Leaves with KErrNoMemory if OOM
//
void CLMgr_BoxFormatHandler::FindFormatContextL(NW_LMgr_Box_t* aFormatBox)
    {
    CFormatStackContext* formatStackContext = NULL;
    iFormatBox = NULL;
    // if the iLastBoxToFormat is NULL , then it is the first request, start formatting
    // from the rootBox
    if (aFormatBox == NULL)
        {
        // removing any existing items from the stack
        ClearFormatStack();
        // get the first child from the box tree
        iFormatBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(iRootBox), 0);
        // rootbox is the current flowbox for its child
        iCurrentFlowBox = iRootBox;
        iCurrentFormatContext = NW_LMgr_FormatContext_New();
        User::LeaveIfNull(iCurrentFormatContext );
        (void) NW_LMgr_FormatBox_ApplyFormatProps(NW_LMgr_FormatBoxOf (iCurrentFlowBox ), iCurrentFormatContext);
        // format the rootbox children to TRUE
        // set the formatBox and formatContext to format the rootbox grandchildren
        // the formatbox and formatcontext may be reset on formatting the first child of the
        // rootbox
        iCurrentFormatContext->formatBox = iCurrentFlowBox ;
        iCurrentFormatContext->newFormatContext = iCurrentFormatContext ;

        if( !iSelectiveFormatting )
            {
            (void) NW_LMgr_FormatBox_ApplyFormatProps(NW_LMgr_FormatBoxOf (iCurrentFlowBox ), iCurrentFormatContext);
            iCurrentFormatContext->newFormatContext = iCurrentFormatContext ;
            }
        else
            {
			//do a while loop to find the real parent, there must be a real parent
            NW_LMgr_Box_t* parBox =(NW_LMgr_Box_t*) NW_LMgr_Box_GetParent(iRootBox);
            while (!NW_Object_IsInstanceOf(parBox, &NW_LMgr_FormatBox_Class))
                {
					parBox = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(parBox);
                }
            (void) NW_LMgr_FormatBox_ApplyFormatProps(NW_LMgr_FormatBoxOf (parBox ), iCurrentFormatContext);

            // for the formatting after image was opened
            // we need to create newContext to format containing boxes of iRootBox
            // the following block is similar to LMgrFormatBox.cpp Resize(...)
            NW_LMgr_FormatContext_t* newContext = NW_LMgr_FormatContext_New();
            (void) NW_LMgr_FormatBox_ApplyFormatProps(NW_LMgr_FormatBoxOf (iCurrentFlowBox ), newContext);
            iCurrentFormatContext->newFormatContext = newContext ;
            newContext->formatBox = iCurrentFlowBox ;
            newContext->referenceCount = 0;

            // push the parentContext on to the stack
            formatStackContext = new (ELeave)CFormatStackContext(this);
			formatStackContext->iParentFormatBox = parBox;
            formatStackContext->iBox = iRootBox;
            formatStackContext->iParentContext = iCurrentFormatContext;
            iFormatStack->PushL(formatStackContext);

            iCurrentFormatContext = newContext ;
            }
        }
    else
        {
        // find format context from the formatstack
        NW_LMgr_Box_t* box = aFormatBox;

#ifdef _DEBUG_LOG

        for (int index1 = iFormatStack->Count() - 1 ; index1 >= 0; index1--)
            {
            NW_LOG2(NW_LOG_LEVEL5, "Index: %d, box %x ", index1 , iFormatStack->At(index1)->iBox);
            }
#endif
#ifdef _DEBUG_LOG
        // integrity check on format stack
        NW_LMgr_Box_t* tempBox = aFormatBox;
        for (int index = iFormatStack->Count() - 1 ; index >= 0 && NW_LMgr_ContainerBox_IsLastChild(tempBox); index--)
            {
            NW_LOG2(NW_LOG_LEVEL5, "Index: %d, box %x ", index , iFormatStack->At(index)->iBox);
            formatStackContext = iFormatStack->At(index);

            // if the box is an instance of container box then popped item from the stack is the box itseld , the next to check
            // is from the top of the stack  else the next to be check would be the popped item from the stack
            if (tempBox == formatStackContext->iBox)
                {
                tempBox = iFormatStack->Head()->iBox;
                }
            else
                {
                tempBox = formatStackContext->iBox;
                }
            }
        // check the stack, the format stack is not correct
        if (index == -1 && !iContainerSplit )
            {
            NW_LMgr_Box_DumpBoxTree (NW_LMgr_BoxOf(iRootBox));
            }
#endif

        // if the last formatted box is on top of the stack , reformat the box
        // the case is true of the lastbox in the previous chunk was a container box
        if (aFormatBox == iFormatStack->Head()->iBox)
            {
            formatStackContext = iFormatStack->Pop();
            iFormatBox = formatStackContext->iBox;
            iCurrentFlowBox = formatStackContext->iParentFormatBox;
            formatStackContext->iParentContext->newFormatContext->referenceCount ++;
            iCurrentFormatContext = formatStackContext->iParentContext;
            delete formatStackContext;
            }
        // if the container box is split , the split container is on top of the stack
        // and all of its children have been formatted. Complete post formatting of the
        // container from top of the stack and next item on the stack is the containerbox that
        // is split , the children of the container that was split are not formatted ,
        // get the first child from the stack and format the box
        else if (iContainerSplit )
            {
            iContainerSplit = EFalse;
            do
                {
                NW_ASSERT(!iFormatStack->IsEmpty());
                formatStackContext = iFormatStack->Pop();
                CleanupStack::PushL(TCleanupItem(CFormatStackContext::CleanupStackContext,formatStackContext));
                PostFormatL(*formatStackContext);
                CleanupStack::PopAndDestroy(); //formatStackContext
                }
            while ((NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(iFormatStack->Head()->iBox)) == 0));

            // set the correct context from the item on stack
            formatStackContext = iFormatStack->Head();
            if ((NW_Object_IsInstanceOf (formatStackContext->iBox, &NW_LMgr_BidiFlowBox_Class) ||
                 NW_Object_IsInstanceOf (formatStackContext->iBox, &NW_LMgr_MarqueeBox_Class))&&
                formatStackContext->iParentContext->newFormatContext)
                {
                iCurrentFlowBox = formatStackContext->iParentContext->formatBox;
                iCurrentFormatContext = formatStackContext->iParentContext->newFormatContext;
                }
            else
                {
                iCurrentFlowBox = formatStackContext->iParentFormatBox;
                iCurrentFormatContext = formatStackContext->iParentContext;
                }
            iFormatBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(formatStackContext->iBox), 0);
            }
        else
            {
            // first find the next box to be formatted
            // Pop from the stack until the boxes are the last child of its parent
            while (NW_LMgr_ContainerBox_IsLastChild(box) && !iFormatStack->IsEmpty())
                {
                // pop the item from the stack and complete the post formatting
                formatStackContext = iFormatStack->Pop();

#ifdef _DEBUG_LOG

                NW_LOG1(NW_LOG_LEVEL5, "Pop box %x ", formatStackContext->iBox);
#endif

                CleanupStack::PushL(TCleanupItem(CFormatStackContext::CleanupStackContext,formatStackContext));
                PostFormatL(*formatStackContext);
                // if the box is an instance of container box then popped item from the stack is the box itseld , the next to check
                // is from the top of the stack  else the next to be check would be the popped item from the stack
                if (box == formatStackContext->iBox)
                    {
                    box = iFormatStack->Head()->iBox;
                    }
                else
                    {
                    box = formatStackContext->iBox;
                    }
                CleanupStack::PopAndDestroy(); //formatStackContext
                }
            if (!iFormatStack->IsEmpty())
                {
                // get the next sibling of the box that is on top of the stack
                NW_ADT_Vector_Metric_t childIndex = NW_LMgr_ContainerBox_GetChildIndex(NW_LMgr_Box_GetParent(box), box);
                iFormatBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_Box_GetParent(box),
                             (NW_ADT_Vector_Metric_t)(childIndex + 1));

                // pop all the items from the stack until we find the formatbox parent
                while (NW_LMgr_BoxOf(NW_LMgr_Box_GetParent(iFormatBox) ) != iFormatStack->Head()->iBox)
                    {
                    NW_ASSERT(!iFormatStack->IsEmpty());
                    formatStackContext = iFormatStack->Pop();
                    CleanupStack::PushL(TCleanupItem(CFormatStackContext::CleanupStackContext,formatStackContext));
                    PostFormatL(*formatStackContext );
                    CleanupStack::PopAndDestroy(); //formatStackContext
                    }

                NW_ASSERT(NW_LMgr_BoxOf(NW_LMgr_Box_GetParent(iFormatBox)) == iFormatStack->Head()->iBox);
                // from the top of the stack , set the contexts
                formatStackContext = iFormatStack->Head();

                if ((NW_Object_IsInstanceOf (formatStackContext->iBox, &NW_LMgr_BidiFlowBox_Class) ||
                     NW_Object_IsInstanceOf (formatStackContext->iBox, &NW_LMgr_MarqueeBox_Class)  ) &&
                    formatStackContext->iParentContext->newFormatContext)
                    {
                    iCurrentFormatContext = formatStackContext->iParentContext->newFormatContext;
                    iCurrentFlowBox = formatStackContext->iParentContext->formatBox;
                    }
                else
                    {
                    iCurrentFormatContext = formatStackContext->iParentContext;
                    iCurrentFlowBox = formatStackContext->iParentFormatBox;
                    }
                }
            }
        }
    }

//
// The method returns the BoxFormatter instance based on the box type
// @param aBox the box to find the formatter .
// @return TLMgr_BoxFormatter instance that can handle formatting of aBox
//
TLMgr_BoxFormatter& CLMgr_BoxFormatHandler::GetBoxFormatter(NW_LMgr_Box_t* aBox)
    {
    TLMgr_BoxFormatter* formatter = &iTLMgr_BoxFormatter;

    
    if (NW_Object_IsInstanceOf (aBox, &NW_LMgr_StaticTableCellBox_Class) ||
        NW_Object_IsInstanceOf (aBox, &NW_LMgr_StaticTableRowBox_Class))
        {
        formatter = &iTLMgr_StaticCell_Box_Formatter;
        }

#if 0
// Marquee Box formatter doesn't do the job properly.
    else if (NW_Object_IsInstanceOf (aBox, &NW_LMgr_MarqueeBox_Class))
       {
       formatter = &iTLMgr_Marquee_Box_Formatter ;
       }
#endif

    return *formatter;
    }

// the function clears of all the formatting stack and resets the
// last formatFormattedbox to NULL
void CLMgr_BoxFormatHandler::ClearFormattingContext()
    {
    ClearFormatStack();
    iContainerSplit = EFalse;
    iFormatBox = NULL;
    }

// the  function is called when the document load is complete.
// the function completes the post formatting on all the items on the format stack
void CLMgr_BoxFormatHandler::PageLoadCompleted()
    {
    iPageLoadCompleted = ETrue;
    if (!IsActive())
        {
        HandlePageLoadCompletedL();
        }
    }

TBrowserStatusCode
CLMgr_BoxFormatHandler::Collapse( NW_LMgr_FlowBox_t* aContainingFlow, NW_Bool aCollapseAnonymous )
  {
  NW_LMgr_BoxVisitor_t* visitor;
  NW_LMgr_Box_t* child = NULL;
  NW_LMgr_Box_t* prevChild = NULL;
  NW_LMgr_RootBox_t* rootBox = NW_LMgr_RootBoxOf( iRootBox );
  NW_LMgr_FlowBox_t* boxToCollapse = aContainingFlow;

  // Positioned boxes will be collapsed only if the whole tree is collapsed
  if (aContainingFlow == NULL)
      {
      rootBox->iPositionedBoxesPlaced = EFalse;
      }

  NW_TRY( status ) 
    {   
    // cancel any outstanding format first
    // css may be coming while we are formatting the page
    Cancel();
    ClearFormattingContext();

    //
    if( !aContainingFlow )
      {
      // Clear the float context vectors. If containing flow is not NULL
      // removing floating boxes should be taken care of by the calling method
      // in this case ReformatBox
      (void) NW_ADT_DynamicVector_Clear( rootBox->placedFloats );
      (void) NW_ADT_DynamicVector_Clear( rootBox->pendingFloats );
      (void) NW_ADT_DynamicVector_Clear( rootBox->positionedAbsObjects);
      rootBox->iPositionedBoxesPlaced = NW_FALSE;
      aContainingFlow = NW_LMgr_FlowBoxOf( rootBox );
      }
    NW_ASSERT( NW_Object_IsInstanceOf( aContainingFlow, &NW_LMgr_FlowBox_Class ) );
    
    visitor = NW_LMgr_ContainerBox_GetBoxVisitor( aContainingFlow );
    prevChild = NW_LMgr_BoxOf( aContainingFlow );
    NW_THROW_OOM_ON_NULL( visitor, status );
    
    // collapse the tree 
    
    // Iterate through all children 
    child = NW_LMgr_BoxVisitor_NextBox( visitor,0 );
    while( ( child = NW_LMgr_BoxVisitor_NextBox( visitor,0 ) ) != NULL ) 
      {
      // positioned boxes are not part of the normal flow and do not need to be collapsed,
      // unless the whole tree is collapsed. e.g. when switching from small screen to normal screen
      if (NW_Object_IsInstanceOf(child, &NW_LMgr_PosFlowBox_Class))
          {
          NW_LMgr_Box_t* box = child;
          NW_Bool shouldCollapsePositioned = (boxToCollapse == NULL);
          if (!shouldCollapsePositioned && NW_Object_IsInstanceOf(boxToCollapse, &NW_LMgr_PosFlowBox_Class))
              {
              NW_LMgr_Box_t* theBox = box;
              NW_LMgr_ContainerBox_t* theParent = NW_LMgr_Box_GetParent(theBox);
              while (theParent != NULL && NW_LMgr_BoxOf(theParent) != NW_LMgr_BoxOf(boxToCollapse))
                  {
                  theBox = NW_LMgr_BoxOf(theParent);
                  theParent = NW_LMgr_Box_GetParent(theBox);
                  }
              if (NW_LMgr_BoxOf(theParent) == NW_LMgr_BoxOf(boxToCollapse))
                  {
                  shouldCollapsePositioned = NW_TRUE;
                  }
              }

          if (shouldCollapsePositioned)
              {
              // Collapse the positioned box and then remove it.
              Collapse(NW_LMgr_FlowBoxOf(box), aCollapseAnonymous);
              NW_LMgr_Box_t* posChild = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(box), 0);
              NW_LMgr_ContainerBox_t* parent = NW_LMgr_Box_GetParent(box);
              NW_ASSERT(posChild != NULL);
              NW_Bool skip = NW_TRUE;
              child = NW_LMgr_BoxVisitor_NextBox( visitor, &skip);
              NW_Int16 index = NW_LMgr_ContainerBox_GetChildIndex(parent, box);
              (void)NW_LMgr_ContainerBox_RemoveChild(parent, box);
              NW_LMgr_ContainerBox_InsertChildAt(parent, posChild, index);
              NW_ADT_DynamicVector_RemoveElement(rootBox->positionedAbsObjects, &box);
              NW_Object_Delete(box);
              if (child == NULL)
                  {
                  break;
                  }
              }
          else
              {
              NW_LMgr_Box_t* posChild = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(box), 0);
              NW_ASSERT(posChild != NULL);
              NW_Bool skip = NW_TRUE;
              child = NW_LMgr_BoxVisitor_NextBox( visitor, &skip);
              if (child == NULL)
                  {
                  break;
                  }
              }
          }
      if( NW_Object_IsClass( child, &NW_LMgr_ContainerBox_Class ) ||
          NW_Object_IsClass( child, &NW_LMgr_ActiveContainerBox_Class ) )
        {
        status = NW_LMgr_ContainerBox_Collapse( NW_LMgr_ContainerBoxOf( child ) );
        prevChild = child;
        }
      else if( NW_Object_IsClass( child, &NW_LMgr_TextBox_Class ) )
        {
        status = NW_LMgr_TextBox_Collapse( NW_LMgr_TextBoxOf( child ) );
        prevChild = child;
        }
      // marker was generated in formatting, so we need to delete it
      else if( NW_Object_IsClass( child, &LMgrMarkerText_Class ) || 
               NW_Object_IsClass( child, &LMgrMarkerImage_Class ) )
        {
        visitor->currentBox = prevChild;
        NW_Object_Delete( child );
        }
      // we do IsClass because we want to delete the anonymous boxes created
      // during formatting and not the ones which were created during CreateBoxTree
      // CreateBoxTree creates LMgrAnonTableBox for wrapping tables and caption
      // We do not want to delete it
      else if( aCollapseAnonymous && NW_Object_IsClass( child, &LMgrAnonBlock_Class ) )
        {
        status = LMgrAnonBlock_Collapse( LMgrAnonBlockOf( child ));
        visitor->currentBox = prevChild;
        }
      else
        {
        prevChild = child;
        }
      NW_ASSERT( status == KBrsrSuccess );
      _NW_THROW_ON_ERROR( status );
      }    
    // successful completion 
    status = KBrsrSuccess;
    NW_LMgr_RootBox_InvalidateCache( rootBox );
    
    }
  NW_CATCH( status ) 
    {      
    } 
  NW_FINALLY 
    {
    NW_Object_Delete( visitor );
    return status;
    } 
  NW_END_TRY
  }

//
// The method is called after the formatting all the chunks and no more
// chunks are available for the page. The method does post formatting of all
// the boxes on the stack and calls the formatting completed on IBoxTreeListener
//
void CLMgr_BoxFormatHandler::HandlePageLoadCompletedL()
    {
    TBrowserStatusCode status = KBrsrSuccess; 
    NW_ADT_Vector_Metric_t index;
#ifdef _DEBUG_LOG
    RFileLogger::Write( KWmlBrowserLogDir, KLogFileName , EFileLoggingModeAppend, _L(" Formatting:Page LoadCompleted " ));
#endif
    while (!iFormatStack->IsEmpty())
        {
        CFormatStackContext* formatStackContext = iFormatStack->Pop();
        CleanupStack::PushL(formatStackContext);
        if (iSelectiveFormatting && iFormatStack->Count()==0)           
            {
            // new information exists in newFormatContext
			status = iTLMgr_BoxFormatter.NW_LMgr_BidiFlowBox_PostFormat(iRootBox, NW_LMgr_ContainerBoxOf(iRootBox), formatStackContext->iParentContext->newFormatContext);
			status = iTLMgr_BoxFormatter.NW_LMgr_ResizeToFloat(iRootBox,formatStackContext->iParentContext->newFormatContext);
			}
        else
            {
            PostFormatL(*formatStackContext);
            if (NW_Object_IsInstanceOf (formatStackContext->iBox->parent, &NW_LMgr_RootBox_Class))
                {
                NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( iRootBox );
                boxBounds.dimension.height = KRootBoxInitialHeight;
                NW_LMgr_Box_SetFormatBounds( iRootBox, boxBounds );
                status = iTLMgr_BoxFormatter.NW_LMgr_BidiFlowBox_PostFormat(formatStackContext->iParentFormatBox, NW_LMgr_ContainerBoxOf(formatStackContext->iBox->parent ), formatStackContext->iParentContext);
                status = NW_LMgr_Box_PostResize(NW_LMgr_BoxOf(formatStackContext->iBox->parent));
                }
            }
        // leave if OOM
        if (status == KBrsrOutOfMemory)
            {
            User::Leave(KErrNoMemory);
            }

        UpdateDisplayBounds( formatStackContext->iBox );

        CleanupStack::PopAndDestroy(); //formatStackContext
        }
      if(!iSelectiveFormatting)
        {
        NW_LMgr_RootBox_t* rootBox = NW_LMgr_RootBoxOf(iRootBox);
        MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
        boxTreeListener->FormatPositionedBoxes ();
        boxTreeListener->FormattingComplete ();
        }
    
        for(index = 0 ; index < iFormatContextRef->Count(); index++)
            {
            NW_Object_Delete(iFormatContextRef->At(index));
            }
        iFormatContextRef->Reset();
        ClearFormattingContext();

    // end of background format
    if( iBackgroundFormat )
      {
      iBackgroundFormat = EFalse;
      NW_LMgr_BoxVisitor_t boxVisitor;
      NW_LMgr_Box_t* child = NULL;
      NW_Uint8 skipChildren = NW_FALSE;
      NW_LMgr_BoxVisitor_Initialize( &boxVisitor, iRootBox );
      // set  final pass on tables
      while( (child = NW_LMgr_BoxVisitor_NextBox( &boxVisitor, &skipChildren ) ) != NULL )
        {
		    if( NW_Object_IsInstanceOf( child, &NW_LMgr_StaticTableBox_Class ) == NW_TRUE )
          {
          NW_LMgr_StaticTableBox_SetAutomaticTableWidthPass( NW_LMgr_StaticTableBoxOf( child ), NW_LMgr_StaticTableBox_AutomaticWidth_FinalPass );
          }
        }      
      // collapse tree
      Collapse( NULL, NW_TRUE );
      // format again      
      PartialFormatL( (CActive::TPriority)Priority() );
      //
      PageLoadCompleted();
	  
      }
	iFormattingCompleted = ETrue;
    }

//
// the method is called from view to format box tree
//
void CLMgr_BoxFormatHandler::PartialFormatL( CActive::TPriority aPriority )
    {
    iFormattingPriority = aPriority;
    //
    if( !IsActive() && iFormattingPriority != Priority() )
      {
      // set the priority
      SetPriority( iFormattingPriority );
      }
    //
	iFormattingCompleted = EFalse;
    // We need to set the bodybox and initialize the origin
    if(!iSelectiveFormatting)
        {
        InitRootBox();
        }

    // if the iLastBoxToFormat to format has changed and if the
    // active object is not active then set it to active
    iLastBoxToFormat = GetLastBox();
    if (iLastBoxToFormat != iFormatBox )
        {
        if (!iFormatBox)
            {
            // find the first box to be formatted
            iPageLoadCompleted = EFalse;
            }
        if (iSelectiveFormatting || !IsActive())
            {
            FindFormatContextL(iFormatBox);
            HandlePartialFormatL();
            }
        }
    if(!iSelectiveFormatting)
      {
    int index = iFormatStack->Count() - 1 ;
    for (; index >= 0; index--)
      {
      NW_GDI_Dimension2D_t tempDimensions = CalculateBoxTempBounds(iFormatStack->At(index)->iBox);
      iFormatStack->At(index)->iTempDimensions  = tempDimensions ;
      NW_LMgr_Box_t* box = iFormatStack->At(index)->iBox ;
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
      boxBounds.dimension.height += tempDimensions.height;
      NW_LMgr_Box_SetFormatBounds( box, boxBounds );
      }
    NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(iRootBox), 0);
    NW_GDI_Rectangle_t rootBoxBounds = NW_LMgr_Box_GetFormatBounds( iRootBox );
    NW_GDI_Rectangle_t childBoxBounds = NW_LMgr_Box_GetFormatBounds( child );
    rootBoxBounds.dimension.height = childBoxBounds.dimension.height;
    NW_LMgr_Box_SetFormatBounds( iRootBox, rootBoxBounds );
    for ( index = iFormatStack->Count() - 1 ; index >= 0; index--)
      {
      NW_GDI_Dimension2D_t tempDimensions  = iFormatStack->At(index)->iTempDimensions;
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( iFormatStack->At(index)->iBox );
      boxBounds.dimension.height -= tempDimensions.height;
      NW_LMgr_Box_SetFormatBounds( iFormatStack->At(index)->iBox, boxBounds );
      }
    }
  }


//
// Formats iFormatBox (the current box to be formatted ) and all the boxes to
// the left subtree iFormatBox
// Leaves with KErrNoMemory if OOM
//
void CLMgr_BoxFormatHandler::HandlePartialFormatL()
    {
    TBrowserStatusCode status = KBrsrSuccess;
#ifdef _DEBUG_LOG
    RFileLogger::Write( KWmlBrowserLogDir, KLogFileName , EFileLoggingModeAppend, _L(" Start formatting" ));
#endif

    CFormatStackContext* formatStackContext = NULL;
    TBool formatChildren = EFalse;
    // find the next box to be formatted and also the
    // formatcontext and flowbox used to  format the box
    while (iFormatBox)
        {
        // Skip positioned boxes. They will be formatted later separately
        if (iFormatBox != iRootBox && NW_Object_IsInstanceOf(iFormatBox, &NW_LMgr_PosFlowBox_Class))
            {
            FindFormatContextL(NW_LMgr_BoxOf(iFormatBox));
            continue;
            }
        formatChildren = EFalse;
        // find the instance of the formatter class
        TLMgr_BoxFormatter& formatter = GetBoxFormatter(iFormatBox);


        NW_LMgr_ContainerBox_t* prevParentBox = NW_LMgr_Box_GetParent(iFormatBox);
#ifdef _DEBUG_LOG

        NW_LOG2(NW_LOG_LEVEL5, "FORMAT: box %x :::formatBox %x ", iFormatBox, iCurrentFlowBox );
#endif
        // initialization code - this used to be in RootBox_InitializeTree
        InitBoxL(iFormatBox);

        // format the box
        status = formatter.NW_LMgr_FlowBox_FormatBox(iCurrentFlowBox, iFormatBox, iCurrentFormatContext);
        // When a box has absolute positioning, it should be formatted separately from the rest of the box tree
        if (status == KBrsrContinue)
            {
            NW_LMgr_PosFlowBox_t* newParent = NW_LMgr_PosFlowBox_New(5);
            if (newParent != NULL)
                {
                NW_LMgr_ContainerBox_t* parent = NW_LMgr_Box_GetParent(iFormatBox);
                TInt childIndex = NW_LMgr_ContainerBox_GetChildIndex (parent, iFormatBox);
                NW_ASSERT (childIndex >= 0);
                NW_LMgr_ContainerBox_RemoveChild (parent, iFormatBox);
                NW_LMgr_ContainerBox_InsertChildAt (parent, NW_LMgr_BoxOf(newParent), (NW_ADT_Vector_Metric_t)childIndex);
                NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBoxOf(newParent), iFormatBox);

                NW_LMgr_PropertyValue_t visibilityVal;
                visibilityVal.token = NW_CSS_PropValue_visible;
                (void)NW_LMgr_Box_GetPropertyValue(iFormatBox, NW_CSS_Prop_visibility,
                                                      NW_CSS_ValueType_Token, &visibilityVal);
                NW_LMgr_PropertyValue_t displayVal;
                displayVal.token = NW_CSS_PropValue_display_inline;
                (void)NW_LMgr_Box_GetPropertyValue(iFormatBox, NW_CSS_Prop_display,
                                                      NW_CSS_ValueType_Token, &displayVal);
           

                if (iRootBox != NULL && NW_Object_IsInstanceOf(iRootBox, &NW_LMgr_PosFlowBox_Class))
                    {
                    newParent->iContainingBlock = iRootBox;
                    }

                if ((visibilityVal.token != NW_CSS_PropValue_hidden) && (displayVal.token != NW_CSS_PropValue_none))
                    {
                    NW_LMgr_PosFlowBox_AddPositionedBox(newParent);
                    }

                FindFormatContextL(NW_LMgr_BoxOf(NW_LMgr_Box_GetParent(iFormatBox)));
                if (iPageLoadCompleted && iFormatBox == NULL)
                    {
                    HandlePageLoadCompletedL();
                    break;
                    }
                else
                    {
                    continue;
                    }
                }
            }
        AppendFormatContextL (iCurrentFormatContext);
        if(iCurrentFormatContext->newFormatContext)
            {
            AppendFormatContextL (iCurrentFormatContext->newFormatContext);
            }
        // if the parent has changed . update the format stack of the new parent
        if ( NW_LMgr_Box_GetParent(iFormatBox) != prevParentBox )
            {
            NW_ASSERT(iFormatStack->Head()->iBox == NW_LMgr_BoxOf(prevParentBox));
            NW_LOG2(NW_LOG_LEVEL5, "REPLACED box %x with box %x ", prevParentBox, iFormatBox->parent);
            // do the post formatting on the boxes from top of the stack
            formatStackContext = new (ELeave)CFormatStackContext(*iFormatStack->Head());
            formatStackContext->iBox = NW_LMgr_BoxOf(iFormatBox->parent);
            iFormatStack->PushL(formatStackContext);
#ifdef _DEBUG_LOG

            NW_LOG1(NW_LOG_LEVEL5, "Pushbox %x ", formatStackContext->iBox);
#endif

            iContainerSplit = ETrue;
            }

        // if the children need to be formatted push the current context on to the stack
        // and set the new context
        if (iCurrentFormatContext->formatChildren )
            {
            // reset formatChildren flag as the new context can be used to format all of its children
            iCurrentFormatContext->formatChildren = NW_FALSE;
            // push the parentContext on to the stack
            formatStackContext = new (ELeave)CFormatStackContext(this);
            formatStackContext->iParentFormatBox = iCurrentFlowBox;
            formatStackContext->iBox = iFormatBox;
            formatStackContext->iParentContext = iCurrentFormatContext;

            // if the formatted box is the last box then push the box on to the stack  and return
            if (iLastBoxToFormat == iFormatBox)
                {
                iFormatStack->PushL(formatStackContext);
                break;
                }
            // set the new parentContext for the children to be formatted
            iCurrentFlowBox = iCurrentFormatContext->formatBox;
            iCurrentFormatContext = iCurrentFormatContext->newFormatContext;

            if ( NW_Object_IsInstanceOf(iFormatBox, &NW_LMgr_ContainerBox_Class) &&
                (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(iFormatBox)) > 0))
                {
                formatChildren = ETrue;
                iFormatBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(iFormatBox), 0);
#ifdef _DEBUG_LOG

                NW_LOG1(NW_LOG_LEVEL5, "Pushbox %x ", formatStackContext->iBox);
#endif
                iFormatStack->PushL(formatStackContext);
                }
            else
                {
                formatChildren = EFalse;
                delete formatStackContext;
                }
            }

        if (!formatChildren)
            {
            // if the box is not NULL
            // then do the post formatting on the box
            // else set the box from the top of the stack
            if (iFormatBox)
                {
                (void) NW_LMgr_Box_PostResize(iFormatBox);
                }
            // is a leaf box do the post formatting on the box
            // if it is last box then return ;
            if (iLastBoxToFormat == iFormatBox)
                {
                break;
                }
            // check if the box is the last child of the parent and do the post formatting
            // and find the next box to be formatted
            if ( NW_LMgr_ContainerBox_IsLastChild(iFormatBox))
                {
                // if it is normal layout or part of the boxtree is to be formatted 
                // then formatting is synchronous
                if ( iSelectiveFormatting)
                    {
                    FindFormatContextL(iFormatBox);
                    }
                else
                    {
                    // Call SetActive() so that this active object gets notified of completed requests.
                    SetActive();
                    // Complete the status
                    TRequestStatus* status = &iStatus;
                    User::RequestComplete(status, KErrNone);                    
                    break;
                    }
                }
            else
                {
                // get the next sibling
                NW_ADT_Vector_Metric_t childIndex = NW_LMgr_ContainerBox_GetChildIndex(NW_LMgr_Box_GetParent(iFormatBox), iFormatBox);
                iFormatBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_Box_GetParent(iFormatBox),
                             (NW_ADT_Vector_Metric_t)(childIndex + 1));
                CFormatStackContext* headFmtStackContext = iFormatStack->Head();
                if ( (NW_Object_IsInstanceOf (headFmtStackContext->iBox, &NW_LMgr_BidiFlowBox_Class)
                    ||NW_Object_IsInstanceOf (headFmtStackContext->iBox, &NW_LMgr_MarqueeBox_Class) )&&
                    headFmtStackContext->iParentContext->newFormatContext)
                    {
                    iCurrentFormatContext = headFmtStackContext->iParentContext->newFormatContext;
                    iCurrentFlowBox = headFmtStackContext->iParentContext->formatBox;
                    }
                else
                    {
                    iCurrentFormatContext = headFmtStackContext->iParentContext;
                    iCurrentFlowBox = headFmtStackContext->iParentFormatBox;
                    }
                }
            }
        }

#ifdef _DEBUG_LOG
    RFileLogger::Write( KWmlBrowserLogDir, KLogFileName , EFileLoggingModeAppend, _L(" End formatting" ));
#endif

    }

//
//
// overriddes Active virtual function . The method formats  a flowbox (  including its children)
// or a leaf box
// leaves if OOM
//
void CLMgr_BoxFormatHandler::RunL()
    {
    // adjust priority
    if( iFormattingPriority != Priority() )
        {
        SetPriority( iFormattingPriority );
        }
    // if the page is not loaded  and if there are boxes to format
    // find the next box to be formatted and also the
    // formatcontext and flowbox used to  format the box
    if (!iPageLoadCompleted || iFormatBox != iLastBoxToFormat)
        {
        iFormattingCompleted = EFalse;
        FindFormatContextL(iFormatBox);
        HandlePartialFormatL();
        }
    if (iPageLoadCompleted && iFormatBox == iLastBoxToFormat)
        {
        HandlePageLoadCompletedL();
        }
    }

//
// overriddes Active virtual function
// Clears the formatting context, if the formatting is cancelled
//
void CLMgr_BoxFormatHandler::DoCancel()
    {
    iPageLoadCompleted = ETrue;
    ClearFormattingContext();
    iBackgroundFormat = NW_FALSE;
    }

//
// overriddes Active virtual function
// Clears the formatting context, incase if any error
//
TInt CLMgr_BoxFormatHandler::RunError(TInt aError)
    {
    ClearFormattingContext();
    NW_ASSERT (NW_Object_IsClass(iRootBox, &NW_LMgr_RootBox_Class));
    MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(iRootBox);
    TBrowserStatusCode status = KBrsrFailure;
    if (aError == KErrNoMemory)
        {
        status = KBrsrOutOfMemory;
        }
    boxTreeListener->HandleError( status );
    return KErrNone;
    }

//
// sets the root box on the format handler (generally used if the
// root box changes, but the format handler is the same)
//
void CLMgr_BoxFormatHandler::SetRootBox(NW_LMgr_Box_t* aRootBox)
  {
  iRootBox = aRootBox;
  }

//
// The method returns dimensions of aBox by the aBox childrens height and width
// @return NW_GDI_Dimension2D_t - dimensions of the aBox
//
NW_GDI_Dimension2D_t CLMgr_BoxFormatHandler::CalculateBoxTempBounds(NW_LMgr_Box_t* aBox )
  {
  NW_LMgr_ContainerBox_t* box = (NW_LMgr_ContainerBox_t*)aBox;
  NW_GDI_Dimension2D_t tempDimensions;
  tempDimensions.height= tempDimensions.width = 0;
  NW_ADT_Vector_Metric_t numberOfChildren = NW_LMgr_ContainerBox_GetChildCount(box );
  if(numberOfChildren >0)
    {
    for(TInt index = numberOfChildren -1 ; index >= 0 ; index -- )
      {
      NW_LMgr_Box_t* childBox = NW_LMgr_ContainerBox_GetChild(box  , 
          (NW_ADT_Vector_Metric_t)index );
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( childBox );
      tempDimensions.height += boxBounds.dimension.height;
      if(tempDimensions.width < boxBounds.dimension.width)
        {
        tempDimensions.width = boxBounds.dimension.width;
        }
      }
    }
  return tempDimensions;
  }
  

//
//  The method normalizes the box on the box tree
//  @param box - box to be normalized
//  @return KBrsrSuccess if successful , KBrsrOutOfMemory if OOM
//
void CLMgr_BoxFormatHandler::InitBoxL(NW_LMgr_Box_t *box)
    {

    TBrowserStatusCode status = KBrsrSuccess;

    if (NW_Object_IsInstanceOf(box, &NW_LMgr_TextBox_Class)) 
    {
      // Initialize 
      NW_LMgr_AbstractTextBoxOf(box)->length = 0;
    }

    if (!NW_Object_IsInstanceOf(box, &NW_LMgr_ContainerBox_Class))        
    {
        // In small screen layout, we want to keep only 2 consequent <br/>
        // We keep the first <br/> and the last <br/> in the sequence.
        // That's because the last <br/> is the master box in case of <a><br/><br/><br/></a>
        // If we remove the last <br/>, that link will not be selectable.
        // The following code deals with both patterns of 
        // <br/><br/><br/><br/><br/> and <a><br/><br/><br/><br/><br/></a>
        
      if (NW_Object_IsClass(box, &NW_LMgr_BreakBox_Class))
      {
            if (iFirstBr)
        {
                NW_LMgr_ContainerBox_t* parentBox = NW_LMgr_Box_GetParent(box);

                if (parentBox != NULL)
                    {
                    TInt count = NW_LMgr_ContainerBox_GetChildCount(parentBox);
                    NW_ADT_Vector_Metric_t idx = NW_LMgr_ContainerBox_GetChildIndex(parentBox, box);

                    TBool lastBreak = (idx == count - 1);
                    if (!lastBreak)
                        {
                        NW_LMgr_Box_t* nextBox = NW_LMgr_ContainerBox_GetChild(
                            parentBox,(NW_ADT_Vector_Metric_t)(idx + 1));
                        lastBreak = !NW_Object_IsInstanceOf(nextBox, &NW_LMgr_BreakBox_Class);
                        }

                    if (lastBreak)
                        {
                        iFirstBr = EFalse;
                        }
                    }
        }
        if (iFirstBr)
        {
                NW_LMgr_Property_t prop;
                // Set flag. This <br/> will not be resized and will be ingnored
                prop.type = NW_CSS_ValueType_Token;
                prop.value.token = NW_CSS_PropValue_flags_thirdBr;
                status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_flags, &prop);
        }
        else
        {
          iFirstBr = ETrue;
        }
      }
      else
      {
        iFirstBr = EFalse;
      }
    }

    if (status == KBrsrSuccess)
        {
        // Apply font properties 
        status = NW_LMgr_Box_ApplyFontProps(box);
        }

    if (!iSelectiveFormatting)
        {
        NW_LMgr_RootBox_t* rootBox = NW_LMgr_RootBoxOf(iRootBox);
        if (!rootBox->isBlinking)
            {
            NW_LMgr_PropertyValue_t textDecoration;

            /* Does the box blink? */
            textDecoration.token = NW_CSS_PropValue_none;
            NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_textDecoration, 
                                         NW_CSS_ValueType_Token, &textDecoration);
            if ((textDecoration.token == NW_CSS_PropValue_blink) ||
                (textDecoration.token == NW_CSS_PropValue_blinkUnderline))
                {
                rootBox->isBlinking = NW_TRUE;
                }
            }
        } 
    // leave if OOM
    if (status == KBrsrOutOfMemory)
        {
        User::Leave(KErrNoMemory);
        }

    NW_LMgr_PropertyValue_t dispValue;
    // Get own display property 
    dispValue.token = NW_CSS_PropValue_display_inline;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_display, NW_CSS_ValueType_Token, &dispValue);

    // Now determine the flow direction
    // We set the direction property only if the box does not contain blocks, or it is a table box 
    if (dispValue.token == NW_CSS_PropValue_display_listItem) 
        {
        if ((NW_Object_IsInstanceOf(box, &NW_LMgr_BidiFlowBox_Class) ||
             NW_Object_IsInstanceOf(box, &NW_LMgr_MarqueeBox_Class)) &&
            NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box)))
            {
            iTLMgr_BoxFormatter.NW_LMgr_BidiFlowBox_GetListMargin(NW_LMgr_FlowBoxOf(box));
            }
        }
    }

//
// The method refreshes the box after the box is post formatted
// and adds the children boxes which are instances of active boxes
// to the tab list
// @param aBox - box to be refresed
// @return void
//
void CLMgr_BoxFormatHandler::RefreshBoxL(NW_LMgr_Box_t *aBox )
    {
    TBrowserStatusCode status=KBrsrSuccess;
    TBool refreshed = EFalse;
    // At the moment we want to refresh during formatting only in vertical layout mode
    // Should be revisited later: Deepika
    if( !iBackgroundFormat )
        {
        // we want to refresh only a FormatBox, because PostFormat on format box
        // determines x,y,ht, width on descendant boxes
        if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_FormatBox_Class))
            {
            // if the parent of format box is on stack we will not refresh, since refresh
            // on parent will cause the draw - this is to avoid flickering
            if(iFormatStack->IsEmpty() || 
              iFormatStack->Head()->iBox != (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(aBox ))
                {
                status = NW_LMgr_Box_Refresh(aBox);
                // denotes that the box has been refreshed
                refreshed = ETrue;
                }

            // if refresh on the box was not called, we refresh the containers and leaf nodes
            // This is to let the content be displayed as soon as possible.
            // We go through the children of the format box and add tab items 
            NW_ADT_Vector_Metric_t numChildren = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aBox));
            for (NW_ADT_Vector_Metric_t index=0; index < numChildren; index++)
                {
                NW_LMgr_Box_t* child =
                    NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(aBox), index);

                // we want to avoid calling refresh on Format boxes, since refresh was already called
                // on them when they were postformatted
                if (!NW_Object_IsInstanceOf(child, &NW_LMgr_FormatBox_Class))
                    {
                    // if the box was already refreshed don't call refresh again
                    if (!refreshed)
                        {
                        status = NW_LMgr_Box_Refresh(child);
                        if (status == KBrsrOutOfMemory)
                            {
                            User::Leave(KErrNoMemory);
                            }
                        }
                    }
                }// end for
            }
        }// if verticallayoutenabled
    }

//
void CLMgr_BoxFormatHandler::AddTabItemL( NW_LMgr_Box_t *aBox )
  {
  if( !iBackgroundFormat )
    {
    // we want to refresh only a FormatBox, because PostFormat on format box
    // determines x,y,ht, width on descendant boxes
    if (NW_Object_IsInstanceOf(aBox, &NW_LMgr_FormatBox_Class))
      {
      NW_ADT_Vector_Metric_t numChildren = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(aBox));
      for (NW_ADT_Vector_Metric_t index=0; index < numChildren; index++)
        {
        NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(aBox), index);
        
        if (!NW_Object_IsInstanceOf(child, &NW_LMgr_FormatBox_Class))
          {
          // we need to add active boxes contained within the ContainerBox.
          // Active boxes within the FormatBox were added when PostFormat
          // was called
          AddSubTreeTabItemsL(child);
          }
        }// end for
      }
    }// if verticallayoutenabled
  }

// This method is called from RefreshBox to add tabitems containeed in container box subtree
void CLMgr_BoxFormatHandler::AddSubTreeTabItemsL(NW_LMgr_Box_t *aBox)
    {
    NW_LMgr_BoxVisitor_t boxVisitor;
    TBrowserStatusCode status = KBrsrSuccess;
    NW_LMgr_Box_t* child = NULL;
    NW_Uint8 skipChildren = NW_FALSE;
    (void)NW_LMgr_BoxVisitor_Initialize(&boxVisitor, aBox);

    // Add tabitems for the subtree
    while ((child = NW_LMgr_BoxVisitor_NextBox(&boxVisitor, &skipChildren)) != NULL)
        {
        // we are only looking for boxes implementing the NW_LMgr_IEventListener interface
        NW_LMgr_IEventListener_t* eventListener = 
            (NW_LMgr_IEventListener_t*)(NW_Object_QueryInterface (child, &NW_LMgr_IEventListener_Class));

        if (eventListener)
            {
            if (NW_Object_IsClass(iRootBox, &NW_LMgr_RootBox_Class))
                {
                NW_LMgr_RootBox_t* rootBox = NW_LMgr_RootBoxOf(iRootBox);
                MBoxTreeListener* boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
                status = boxTreeListener->BoxTreeListenerAppendTabItem (child);
              //  NW_LOG1( NW_LOG_LEVEL5, "NW_LMgr_IBoxTreeListener_AppendTabItem, %x", child );
                if (status == KBrsrOutOfMemory)
                    {
                    User::Leave(KErrNoMemory);
                    }
                }
            }
        skipChildren = NW_FALSE;
        // skip traversing FormatBox children since they were added when PostFormat was called on 
        // format box
        if (NW_Object_IsInstanceOf(child, &NW_LMgr_FormatBox_Class))
            {
            skipChildren = NW_TRUE;
            }
        }
    }

// This method initializes the rootbox - sets body and displaybounds
void CLMgr_BoxFormatHandler::InitRootBox()
    {
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_RootBoxOf(iRootBox);
    if (!rootBox->body)
        {
        // find body box
        NW_LMgr_BoxVisitor_t boxVisitor;
        NW_LMgr_Box_t* child = NULL;

        // set the body box
        (void)NW_LMgr_BoxVisitor_Initialize(&boxVisitor, iRootBox);
        while ((child = NW_LMgr_BoxVisitor_NextBox(&boxVisitor, NULL)) != NULL)
            {
            NW_LMgr_PropertyValue_t bodyFlag;
            bodyFlag.token = NW_CSS_PropValue_none;
            (void)NW_LMgr_Box_GetPropertyValue(child, NW_CSS_Prop_flags, NW_CSS_ValueType_Token, &bodyFlag);
            if (bodyFlag.token == NW_CSS_PropValue_flags_bodyElement) 
                {
                NW_LMgr_RootBox_SetBody(rootBox, child);
                break;
                }   
            }
        }
    // initialize display bounds
    if (rootBox->body)
        {            
        CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext(rootBox);
        if (NW_LMgr_RootBox_IsPageRTL(rootBox)) 
            {
            NW_GDI_Point2D_t origin;
            NW_GDI_Rectangle_t displayBounds;

            displayBounds = *deviceContext->DisplayBounds ();
            displayBounds.point.x = RIGHT_ORIGIN;
            NW_LMgr_Box_SetFormatBounds( NW_LMgr_BoxOf( rootBox ), displayBounds);
            origin.x = RIGHT_ORIGIN;
            origin.y = 0;
            deviceContext->SetOrigin ( &origin);
            }
        } 
    }

