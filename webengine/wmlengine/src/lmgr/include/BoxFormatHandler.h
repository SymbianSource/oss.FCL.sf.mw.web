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
* Description:  The class handles formatting of the boxes , refreshes the boxe on the postformatting,
              adds the active boxes  to the tab list
*
*/

#ifndef BOX_FORMAT_HANDLER_H
#define BOX_FORMAT_HANDLER_H

#include "e32base.h"
#include "cstack.h"
#include "nw_gdi_utils.h"
#include "BoxFormat.h"

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  The class handles formatting of the boxes , refreshes the boxe on the postformatting,
*   adds the active boxes  to the tab list
*
*  @lib lmgr.lib
*/
class CLMgr_BoxFormatHandler: public CActive
    {
    public:
        /**
        * A two phase constructor that constructs  and initializes CLMgr_BoxFormatHandler class
        * leaves KErrNoMemory if out of memory
        * @param aRootBox - flowBox to be formatted
        * @param aSelectiveFormatting is ETrue if only a flowBox  and its children needs to be formatted
        * @return CLMgr_BoxFormatHandler* - pointer to the instance of  class CLMgr_BoxFormatHandler
        */
        static CLMgr_BoxFormatHandler* NewL(NW_LMgr_Box_t* aRootBox,
                                            TBool aSelectiveFormatting);
        /**
        * the method is called from view to format box tree
        */
        void PartialFormatL( CActive::TPriority aPriority );

        /**
        * the method is called from view after the partialFormat is called 
        * for the last chunk 
        */
        void PageLoadCompleted();

        TBrowserStatusCode Collapse( NW_LMgr_FlowBox_t* aContainingFlow, NW_Bool aCollapseAnonymous );
        /**
        * the method clears the formatting context . 
        * The formatting context includes the formatting stack and the current format box
        */
        void ClearFormattingContext();

        /**
        * initializes the flowbox and its children
        * @param - flowbox and its children to  be initialized
        * @return - returns KBrsrSuccess if no error 
        */
        TBrowserStatusCode InitializeBox(NW_LMgr_BidiFlowBox_t *flow);

        /**
        * initializes the flowbox and its children in small screen mode
        * @param - flowbox and its children to  be initialized
        * @return - returns KBrsrSuccess if no error 
        */
        TBrowserStatusCode InitializeBoxSSL(NW_LMgr_BidiFlowBox_t *flow);

        /**
        * set the background format flag to true which means that
        * the next format is a background format
        */
        inline void SetBackgroundFormat() { iBackgroundFormat = ETrue; }

        /**
        * This function returns true if the current format is a background format
        */
        inline TBool BackgroundFormat() { return iBackgroundFormat; }

        /**
        * Destructor- deletes format stack and 
        * removes the instance from CActiveScheduler
        */
        ~CLMgr_BoxFormatHandler();

        /**
        * 
        * adds the box to the tab list 
        * @param box - the box to be added to the tab list 
        * @return - returns KBrsrOutOfMemory if OOM
        */
        TBrowserStatusCode AddTabItem(NW_LMgr_Box_t *box);

        /**
        * 
        * returns last formatted box
        * @return - last formatted box of the box tree
        */
        NW_LMgr_Box_t* GetLastFormattedBox()
            {
            return iFormatBox;
            }
        /**
        * 
        * sets the last box to be formatted from the boxtee
        * @param aLastBoxToFormat - the last box to be formatted from the boxtee
        */
        void SetLastBoxToFormatL(NW_LMgr_Box_t *aLastBoxToFormat)
            {
            iLastBoxToFormat = aLastBoxToFormat;
            }

        /**
        * 
        * overriddes Active virtual function . The method formats  a flowbox (  including its children) 
        * or a leaf box 
        * leaves if OOM
        */
        void RunL();

        /**
        * overriddes Active virtual function 
        * Clears the formatting context, if the formatting is cancelled
        */
        void DoCancel();

        /**
        * overriddes Active virtual function 
        * Clears the formatting context, incase if any error 
        */
        TInt RunError(TInt aError);

        /**
        * sets the root box on the format handler (generally used if the
        * root box changes, but the format handler is the same)
        */
        void SetRootBox(NW_LMgr_Box_t* aRootBox);
		TBool GetFormattingComplete(){return iFormattingCompleted;};

    private:
        // auxiallary class to contain ,enable copying and deleting of FormatContexts

    class CFormatStackContext: public CBase
            {
            public:
                CFormatStackContext(CLMgr_BoxFormatHandler* aBoxFormatHandler);
                static void CleanupStackContext(TAny* aFormatStackContext);
           
                CFormatStackContext(const CFormatStackContext& aFormatStackContext);
                ~CFormatStackContext();
                // formatbox - box to be formatted
                NW_LMgr_Box_t *iBox;
                // parent box of iBox
                NW_LMgr_Box_t *iParentFormatBox;
                // context to format iBox
                NW_LMgr_FormatContext_t* iParentContext;
                // used to calculate rootBox temp height after formatting part of box tree
                NW_GDI_Dimension2D_t iTempDimensions ;
                // is ETrue if page load is completed
                TBool iPageLoadCompleted;
                // reference to the containing class
                CLMgr_BoxFormatHandler* iBoxFormatHandler;// has
            };
    private:
        /**
        * Constructor
        **/
        CLMgr_BoxFormatHandler();
        /**
        * Initializes CLMgr_BoxFormatHandler , creates stack to handle formatting
        * and adds the CLMgr_BoxFormatHandler instance to ActiveScheduler
        * @param aRootBox - flowbox to be formatted
        * @param aSelectiveFormatting- is ETrue if only aRootBox and its children need
                  to be formatted
        **/
        void ConstructL(NW_LMgr_Box_t* aRootBox,
                        TBool aSelectiveFormatting);

        /**
        * Formats iFormatBox (the current box to be formatted ) and all the boxes to
        * the left subtree iFormatBox
        * Leaves with KErrNoMemory if OOM
        **/
        void HandlePartialFormatL();
        /**
        * Finds the format context of the next box of aFormatBox to be formatted
        * If the aFormatBox is NULL , then a new FormatContext is created on rootbox and
        * the child of the rootbox becomes the next box to be formattec
        * Leaves with KErrNoMemory if OOM
        **/
        void FindFormatContextL(NW_LMgr_Box_t* aFormatBox);
        /**
        * The method is called after the formatting all the chunks and no more 
        * chunks are available for the page. The method does post formatting of all 
        * the boxes on the stack and calls the formatting completed on IBoxTreeListener
        **/
        void HandlePageLoadCompletedL();
        /**
        * The method handles post formatting on a box 
        * @param CFormatStackContext- Formatting context instance to be post formatted
        **/
        void PostFormatL(CFormatStackContext& );
        /**
        * The method clears all the items on the format stack and sets the current box to 
        * formatted to NULL
        **/
        void ClearFormatStack();
        /**
        * The method returns the BoxFormatter instance based on the box type 
        * @param aBox the box to find the formatter .
        * @return TLMgr_BoxFormatter instance that can handle formatting of aBox
        **/
        TLMgr_BoxFormatter& GetBoxFormatter(NW_LMgr_Box_t* aBox);

        /**
          * The method returns the last box of the boxtree
          * @return the last box of the boxtree
          **/

        NW_LMgr_Box_t* GetLastBox();

        /**
          * The method returns dimensions of aBox by the aBox childrens height and width
          * @return NW_GDI_Dimension2D_t - dimensions of the aBox 
          **/
        NW_GDI_Dimension2D_t CalculateBoxTempBounds(NW_LMgr_Box_t* aBox );

        /**
          * The method normalizes the box on the box tree
          * @param box - box to be normalized
          * @return void
          **/
        void InitBoxL(NW_LMgr_Box_t *aBox);

        /**
          * The method refreshes the box after the box is post formatted 
          * @return void
          * @param box - box to be refresed
          **/
        void RefreshBoxL(NW_LMgr_Box_t *aBox );

        /**
          * This method adds the children boxes which are instances of active boxes
          * to the tab list 
          * @param box - box to be added
          **/
        void AddTabItemL( NW_LMgr_Box_t *aBox );

        /**
          * This method is called from AddTabItemL to add tabitems 
          * containeed in container box subtree
          **/
        void AddSubTreeTabItemsL(NW_LMgr_Box_t *aBox);

        /**
          * This method is called from PartialFormatL to initialize the rootbox 
          * It sets the body box and initializes the display bounds
          **/
        void InitRootBox();

        /**
          * This method appends the NW_LMgr_FormatContext_t* object to the array of pointers
          * iFormatContextRef.
          **/
        void AppendFormatContextL(NW_LMgr_FormatContext_t* aLMgr_FormatContext_t);

        /**
          * This method updates the display bounds on a formatted subtree
          **/
        void UpdateDisplayBounds( NW_LMgr_Box_t* aBox );

    private:
      // friends
      friend class CFormatStackContext;

    private:
        // members
        // formatted flowboxes  are added to the stack(iFormatStack) to be postformatted after
        // formatting the flowboxes children
        CStack<CFormatStackContext, ETrue>* iFormatStack;
        // default box formatter
        TLMgr_BoxFormatter iTLMgr_BoxFormatter;
        // marquee box formatter
        TLMgr_Marquee_Box_Formatter iTLMgr_Marquee_Box_Formatter;
        // Table cell  or table row box formatter
        TLMgr_StaticCell_Box_Formatter iTLMgr_StaticCell_Box_Formatter;

        // Last box to be formatted from the boxtree
        NW_LMgr_Box_t *iLastBoxToFormat ;
        // RootBox reference
        NW_LMgr_Box_t *iRootBox;

        // The current flowbox used to format iFormatBox
        NW_LMgr_Box_t* iCurrentFlowBox ;
        // The current format context used to format iFormatBox
        NW_LMgr_FormatContext_t* iCurrentFormatContext;
        // The box to be formatted
        NW_LMgr_Box_t* iFormatBox;
        // is ETrue if the box that is formatted is split
        TBool iContainerSplit;
        // is ETrue if part of the tree is to be formatted
        TBool iSelectiveFormatting;
        // is ETrue if PartialFormatL method is called for the last chunk
        TBool iPageLoadCompleted;
    	TBool iFormattingCompleted;
        CArrayPtrFlat<NW_LMgr_FormatContext_t>* iFormatContextRef;
        // the following 2 members are used to normalize the boxtree
        TBool iFirstBr;
        TBool iSecondBr;
        TBool iBackgroundFormat;
        CActive::TPriority iFormattingPriority;
    };

#endif
