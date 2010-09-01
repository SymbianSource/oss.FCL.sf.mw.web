/*
* Copyright (c) 2002-2204 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of the Browser On-screen scrollbar
*
*/

#ifndef SCROLLBAR_H
#define SCROLLBAR_H

//----------------------------------------------------------------------------
// INCLUDES
//----------------------------------------------------------------------------
#include <coecntrl.h>
#include <e32std.h>

//----------------------------------------------------------------------------
// CONSTANTS
//----------------------------------------------------------------------------
const TInt KTrackBkgdThickness = 1;	// Thickness of the scrollbar track

//----------------------------------------------------------------------------
// FORWARD DECLARATIONS
//----------------------------------------------------------------------------
class CScrollThumb;	

//----------------------------------------------------------------------------
// CLASS DEFINITION
//----------------------------------------------------------------------------

/**
*  Browser On-screen scrollbar
*
*  @since Series60_version 2.5
*/
class CScrollBar : public CCoeControl
    {
    public:
        
        // Used by scrollbars and its components to detemine the orientation of
        // a scrollbar
        enum TOrientation
            {
            EVertical,
            EHorizontal
            };
        
        //------------------------------------------------------------------
        // Constructors and Destructors - Public
        //------------------------------------------------------------------
    public:
        
        /**
        * Two-phased constructor.
        */
        static CScrollBar* NewL(const TOrientation aOrientation, 
                                const CCoeControl& aParent);		
        /**
        * Destructor.
        */
        virtual ~CScrollBar();
        
        //------------------------------------------------------------------
        // Inherited Member Functions - Public
        //------------------------------------------------------------------
    public:
        
        virtual void Draw(const TRect& aRect) const;
        
        //------------------------------------------------------------------
        // Member Functions - Public
        //------------------------------------------------------------------
    public:
        
        /**
        * Sets the rect for the scroll bar and its background
        * @since Series60_version 2.5
        * @param aRect - the rectangle in which the scrollbar is drawn into
        * @return none
        */
        void SetTrackAndBackground(const TRect& aRect);
        
        //------------------------------------------------------------------
        // Member Functions - Public - Get/Set
        //------------------------------------------------------------------
    public:
        
        /**
        * Calls SetRect() on iThumb
        * @since Series60_version 2.5
        * @param aRect - the rectangle for the scrollbar thumb
        * @return none
        */
        void SetThumb(const TRect& aRect);
        
        /**
        * Sets the orientation of the scrollbar
        * @since Series60_version 2.5
        * @param aOrientation - the orientation of the scrollbar
        * @return none
        */
        inline void SetOrientation(const TOrientation aOrientation) 
                                    { iOrientation = aOrientation; }
        
        //------------------------------------------------------------------
        // Constructors and Destructors - Private
        //------------------------------------------------------------------
    private:
        
        /**
        * C++ default constructor.
        */
        CScrollBar(const TOrientation aOrientation);
        
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL(const CCoeControl& aParent);
        
        //------------------------------------------------------------------
        // Inherited Member Functions - Private
        //------------------------------------------------------------------
    private:
        
        /** 
        *	From CCoeControl, CountComponentControls
        */
        TInt CountComponentControls() const;
        
        /**
        *	From CCoeControl, SizeChanged
        */
        virtual void SizeChanged();
        
        /**
        *	From CCoeControl, ComponentControl
        */
        CCoeControl* ComponentControl(TInt aIndex) const;
        
        //------------------------------------------------------------------
        // Member Functions - Private
        //------------------------------------------------------------------
    private:
        
        /**
        * Sets the colours of a scrollbar
        * @since Series60_version 2.5
        * @param aTrackColour - the colour of the track
        * @param aTrackBkgdColour - the colour of the track background
        * @return none
        */
        void SetTrackColours(const TRgb aTrackColour, const TRgb aTrackBkgdColour);
        
        //------------------------------------------------------------------
        // Member Data - Public
        //------------------------------------------------------------------
    public:
        
        CScrollThumb* iThumb;	// Scrollbar Thumb - owned
        
        //------------------------------------------------------------------
        // Member Data - Private
        //------------------------------------------------------------------
    private:
        TOrientation iOrientation;  // Scrollbar orientation
        
        TRgb iTrackColour;		    // track colour (track aka 'shaft') 
        TRgb iTrackBkgdColour;	    // track background colour (track aka 'shaft')
        
        TRect iTrackCentre;         // the rect of the track shown inside the track shadow
};

#endif  // SCROLLBAR_H 
