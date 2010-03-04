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
* Description:  Object for containing scrolling specific methods.
*
*/


#ifndef __MVCSCROLL_H
#define __MVCSCROLL_H

//  INCLUDES
#include <e32base.h>
#include "MVCView.h"
#include "nw_evt_controlkeyevent.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
NONSHARABLE_CLASS(CMVCScroll) : public CBase
{
public:
    /**
    * Two-phased constructor.
    * @param view A pointer to the view object associated with this scroll object
    * @return CMVCScroll*
	  */
    static CMVCScroll* NewL(CView* view);

    /**
    * Destructor.
    */
    ~CMVCScroll();

    /**
    * Navigates downward through the page
    * @since 2.X
    * @param isNewPage A flag passed to SetCurrentBox
    * @param noBoxVisible A flag used for deciding whether or not to call ForceCurrentBox
    * @return TBool
	  */
    TBool TabForward(TBool isNewPage,
                     TBool* noBoxVisible);
        
    /**
    * Navigates upward through the page
    * @since 2.X
    * @param noBoxVisible A flag used for deciding whether or not to call ForceCurrentBox
    * @return TBool
	  */
    TBool TabBackward(TBool* noBoxVisible);

    /**
    * Navigates left horizontally through the page
    * @since 2.X
    * @param noBoxVisible A flag used for deciding whether or not to call ForceCurrentBox
    * @return TBool
	  */
    TBool TabLeft(TBool* noBoxVisible);

    /**
    * Navigates right horizontally through the page
    * @since 2.X
    * @param noBoxVisible A flag used for deciding whether or not to call ForceCurrentBox
    * @return TBool
	  */
    TBool TabRight(TBool* noBoxVisible);

    /**
    * Returns a NW_LMgr_Box_t pointer to the the previous box in the index
    * @since 2.X
    * @param oldBox A pointer to the (current)box used to calculate the previous box
    * @return NW_LMgr_Box_t*
	  */
    NW_LMgr_Box_t* GetPreviousTabIndex(const NW_LMgr_Box_t* oldBox,
                                       NW_Int32* currentTabIndex);
 
    /**
    * Returns a NW_LMgr_Box_t pointer to the the next box in the index
    * @since 2.X
    * @param oldBox A pointer to the (current)box used to calculate the previous box
    * @return NW_LMgr_Box_t*
	  */
    NW_LMgr_Box_t* GetNextTabIndex(const NW_LMgr_Box_t* oldBox,
                                   NW_Int32* currentTabIndex);

    /**
    * Processes the control key for normal layout
    * @since 2.X
    * @param key The control key that was pressed, up, down, left, right
    * @return TBool
	  */
    TBool ProcessControlKey(NW_Evt_ControlKeyType_t key);

    /**
    * Processes the control key for vertical layout
    * @since 2.X
    * @param key The control key that was pressed, up, down, left, right
    * @return TBool
	  */
    TBool ProcessControlKeyVL(NW_Evt_ControlKeyType_t key);

    /**
    * Method used to calculate the amount to scroll the page up or down
    * @since 2.X
    * @param void
    * @return NW_Uint16
    */
    NW_Uint16 GetScrollAmount(void);

    /**
    * Method used in vertical layout to page up or down by one full
    * page size minus 2 lines
    */
    TBool ScrollPage(NW_Evt_ControlKeyType_t key);

    /**
    * Method returns the first sibling box whose width and height are not zero
    */
    NW_LMgr_Box_t* GetFirstSiblingBox (NW_LMgr_Box_t* box);  

private: // Private methods and member variables
    /**
    * C++ default constructor.
    */
    CMVCScroll(CView* view);
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Method used in vertical layout to tab to the appropriate link
    */
    TBool ScrollLink(NW_Evt_ControlKeyType_t key,
                     TBool* noBoxVisible);
 
    /**
    * Method used to calculate the next box to tab to 
    */
    void GetBoxRange(NW_LMgr_Box_t* box, 
                     NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                     NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2);

    /**
    * Method to set focus to the current box
    */
    TBrowserStatusCode SetCurrentBox(NW_LMgr_Box_t* box, 
                              NW_Evt_TabEvent_t* tabEvent,
                              TBool isNewPage,
                              TBool* noBoxVisible);
    /**
    * Method returns true if the box is visible in both rectangles
    */
    TBool IsBoxVisibleInEither(const NW_LMgr_Box_t* box,
                               NW_GDI_Rectangle_t* bounds1,
                               NW_GDI_Rectangle_t* bounds2);
    /**
    * Method used to get range of the box, not considering sibling or master box's range
    */
    void GetBoxRangeRestrict (NW_LMgr_Box_t* box, 
                              NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                              NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 );
    /**
    * Method used to get master range of the box
    */
    void GetMasterBoxRange (NW_LMgr_Box_t* box, 
                            NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                            NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 );
   
    /**
    * Method used to get the first sibling box range
    */
    void GetFirstSiblingBoxRange (NW_LMgr_Box_t* box, 
                                  NW_GDI_Metric_t* referenceY, NW_GDI_Metric_t* refY2,
                                  NW_GDI_Metric_t* referenceX, NW_GDI_Metric_t* refX2 );
   /**
    * Method used to get the size of the visible box
    */
    NW_Bool HowBigIsBoxVisible ( const NW_LMgr_Box_t* box,
                                 NW_GDI_Rectangle_t* bounds,
                                 NW_GDI_Rectangle_t* overlap );
   /**
    * Method used to get the scroll down bounds
    */
    NW_Bool GetScrollDownBounds ( CView* view,
                                  NW_GDI_Rectangle_t* scrolledBounds );
   /**
    * Method used to get the scroll up bounds
    */
    NW_Bool GetScrollUpBounds ( CView* view,
                                NW_GDI_Rectangle_t* scrolledBounds );
   /**
    * Method used to get the tab index
    */
    NW_LMgr_Box_t* GetTabIndex( const NW_LMgr_Box_t* oldBox, 
                                NW_Bool nextIndex, 
                                NW_Int32* currentTabIndex );
   /**
    * Method used to get the next event listener
    */
    NW_LMgr_Box_t* GetNextEventListener (NW_LMgr_BoxVisitor_t* boxVisitor);
   
    /**
    * Method used to find the first visible box on a page
    */
    void GetFirstVisible(NW_LMgr_Box_t* box, 
                         NW_LMgr_Box_t** firstVisible);
    /**
    * Method used to calculate the amount to page up or down
    */
    NW_Uint16 GetPagingAmount(void);

    // Pointer to the scroll objects associated view object
    CView* iView;
};

#endif //__MVCSCROLL_H
