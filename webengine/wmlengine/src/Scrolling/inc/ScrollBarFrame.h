/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of the Browser On-screen scrollbar frame
*				 and browser scrollbar corner window
*
*/

#ifndef SCROLLBARFRAME_H
#define SCROLLBARFRAME_H

//----------------------------------------------------------------------
// INCLUDES
//----------------------------------------------------------------------
#include "ScrollBar.h"
#include <e32std.h>
#include <coecntrl.h>

// Browser as a Plugin includes
#include <brctlinterface.h>

//----------------------------------------------------------------------
// CONSTANTS
//----------------------------------------------------------------------
const TInt KBrowserVScrollBarWidth	= 6;	// V Scrollbar Width
const TInt KBrowserHScrollBarHeight	= 6;	// H Scrollbar Height

const TInt KBrowserVScrollBarMaxLen	= 204;	// The maximum height of the vertical scrollbar

const TInt KBrowserMaxDisplayWidth = 176;	// Display Width
const TInt KConvertToNegativeValue = -1;	// used to convert numbers to negative

//----------------------------------------------------------------------
// FORWARD DECLARATIONS
//----------------------------------------------------------------------
class CScrollBar;

//----------------------------------------------------------------------
// CLASS DEFINITION - only used in CScrollBarFrame
//----------------------------------------------------------------------
/**
*  Browser scrollbar corner window
*
*  @since Series60_version 2.5
*/
class CScrollBarCornerWindow : public CCoeControl
    {
    //------------------------------------------------------------------
    // Constructors and Destructors
    //------------------------------------------------------------------
    public:
        static CScrollBarCornerWindow* NewL(const CCoeControl& aParent);
        virtual ~CScrollBarCornerWindow();
        
        // Inherited Functions
        void Draw(const TRect& aRect) const;
    private:
        void ConstructL(const CCoeControl& aParent);

    };


//----------------------------------------------------------------------
// CLASS DEFINITION
//----------------------------------------------------------------------
/**
*  Browser scrollbar frame
*  @since Series60_version 2.5
*/
class CScrollBarFrame : public CBase
    {
    //------------------------------------------------------------------
    // Constructors and Destructors - Public
    //------------------------------------------------------------------
    public:
        
    /**
    * Two-phased constructor.
        */
        static CScrollBarFrame* NewL(const CCoeControl& aParent);
        
        /**
        * Destructor.
        */
        virtual ~CScrollBarFrame();
        
        //------------------------------------------------------------------
        // Member Functions - Get/Set Funtions - Public
        //------------------------------------------------------------------
    public:
        
        /**
        * Sets the layout indicator
        * @since Series60_version 2.5
        * @param aLayout - webpage layout
        * @return none
        */
        inline void SetLayout(const TBrCtlLayout aLayout) { iLayout = aLayout; }
        /**
        * Returns the layout indicator value
        * @since Series60_version 2.5
        * @return MBrCtlLayoutObserver::TBrCtlLayout - layout of the webpage
        */
        inline const TBrCtlLayout Layout() { return iLayout; }

        /**
        * Returns the vertical scrollbar
        * @since Series60_version 2.8
        * @return MBrCtlLayoutObserver::TBrCtlLayout - layout of the webpage
        */
        inline const CScrollBar* VScrollBar() {return iVScrollBar;}

        /**
        * Returns the horizontal scrollbar
        * @since Series60_version 2.8
        * @return MBrCtlLayoutObserver::TBrCtlLayout - layout of the webpage
        */
        inline const CScrollBar* HScrollBar() {return iHScrollBar;}
        
        /**
        * Sets the display height
        * @since Series60_version 2.5
        * @param aHeight - display height
        * @return none
        */
        void SetDisplayHeight(const TInt aHeight);
        
        /**
        * Sets the display width
        * @since Series60_version 2.5
        * @param aWidth - display width
        * @return none
        */
        void SetDisplayWidth(const TInt aWidth);
        
        /**
        * Gets the display rect
        * @since Series60_version 2.5
        * @return TRect& - the display rectangle
        */
        const TRect& Display() const { return iDisplay; }
        
        //------------------------------------------------------------------
        // Member Functions - Public
        //------------------------------------------------------------------
    public:
        
        /**
        * Updates the vertical scrollbar
        * @since Series60_version 2.5
        * @return none
        */
        void UpdateVScrollBarL();
        
        /**
        * Updates the Horizontal scrollbar
        * @since Series60_version 2.5
        * @return none
        */
        void UpdateHScrollBarL();
        
        /**
        * Sets indicator relating to the webpage layout
        * @since Series60_version 2.5
        * @param aLayout - the layout of the webpage
        * @return none
        */
        void NotifyLayoutChange(TBrCtlLayout aLayout);
        
        /**
        * Determines whether a vertical scroll bar will be required
        * and the space required for the scrollbar
        * @since Series60_version 2.5
        * @param aDocument - height of the webpage
        * @param aDisplay - height of the display
        * @param aDisplayPos - vertical position of display in webpage
        * @return none
        */
        TInt VScrollBarNeeded(TInt aDocument, TInt aDisplay, TInt aDisplayPos);
        
        /**
        * Determines whether a horizontal scroll bar will be required
        * and the space required for the scrollbar
        * @since Series60_version 2.5
        * @param aDocument - width of the webpage
        * @param aDisplay - width of the display
        * @param aDisplayPos - horizontal position of display in webpage
        * @param aDisplayHeight - vertical position of display in webpage
        * @return none
        */
        TInt HScrollBarNeeded(TInt aDocument, TInt aDisplay, TInt aDisplayPos,
                              TInt aDisplayHeight);
        
        inline TInt VScrollBarWidth() { return KBrowserVScrollBarWidth; }
        
        //------------------------------------------------------------------
        // Constructors and Destructors - Private
        //------------------------------------------------------------------
    private:
        
        /**
        * C++ default constructor.
        */
        CScrollBarFrame();
        
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL( const CCoeControl& aParent );
        
        //------------------------------------------------------------------
        // Member Functions - Private
        //------------------------------------------------------------------
    private:
        
        /**
        * Calculates dimensions of the scroll bar and forwards to scrollbar
        * @since Series60_version 2.5
        * @param aOrientation - scrollbar orientation
        * @return none
        */
        void SetScrollBar(const CScrollBar::TOrientation aOrientation);
        
        /**
        * Calculates dimensions of the thumb and forwards to scrollbar thumb
        * @since Series60_version 2.5
        * @param aOrientation - scrollbar orientation
        * @return none
        */
        void SetThumb(CScrollBar::TOrientation aOrientation);
        
        //------------------------------------------------------------------
        // Member Data - Public
        //------------------------------------------------------------------
    public:
        
        // Scrollbars
        CScrollBar* iVScrollBar;    // owned
        CScrollBar* iHScrollBar;    // owned
        
        // Corner Window - only used with a scrollbar pair
        CScrollBarCornerWindow* iCornerWindow;  // owned
        
        //------------------------------------------------------------------
        // Member Data - Private
        //------------------------------------------------------------------
    private:
        
        TRect	iDocument;          // web page
        TRect	iDisplay;		    // display
        TPoint	iDisplayPos;	    // position of display within web page
        
        TRect	iScrollBarRect;	    // TRect outside iDisplay where scrollbars are drawn
        TBrCtlLayout	iLayout;    // is scrolling left to right OR right to left
};

#endif  // SCROLLBARFRAME_H
