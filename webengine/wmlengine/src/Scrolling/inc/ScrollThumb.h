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
* Description:  Definition of the Browser On-screen scrollbar thumb
*
*/

#ifndef _SCROLLTHUMB_H
#define _SCROLLTHUMB_H

//-----------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------
#include "ScrollBar.h"

#include <coecntrl.h>
#include <e32std.h>

//-----------------------------------------------------------------------
// CONSTANTS
//-----------------------------------------------------------------------
const TInt KShadowThickness = 1; // thickness of the shadow of the thumb

//-----------------------------------------------------------------------
// CLASS DEFINITION
//-----------------------------------------------------------------------
//
/**
*  Browser On-screen scrollbar thumb
*
*  @since Series60_version 2.5
*/
class CScrollThumb : public CCoeControl
    {
    //------------------------------------------------------------------
    // Constructors and Destructors - Public
    //------------------------------------------------------------------
    public:
        
        /**
        * Two-phased constructor.
        */
        static CScrollThumb* NewL(CScrollBar::TOrientation aOrientation);
        
        /**
        * Destructor.
        */
        virtual ~CScrollThumb();
        
        //------------------------------------------------------------------
        // Member Functions - Public
        //------------------------------------------------------------------
    public:
        
        void Draw(const TRect& aRect) const;
        
        /**
        * Sets the rect of the thumb and its shadow
        * @since Series60_version 2.5
        * @param aRect - rectangle of the thumb
        * @return none
        */
        void SetThumbAndShadow(const TRect& aRect);
        
        //------------------------------------------------------------------
        // Constructors and Destructors - Private
        //------------------------------------------------------------------
    private:
        
        /**
        * C++ default constructor.
        */
        CScrollThumb(CScrollBar::TOrientation aOrientation);
        
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();
        
        //------------------------------------------------------------------
        // Inherited Member Functions - Private
        //------------------------------------------------------------------
    private:
        
        /**
        *	From CCoeControl, SizeChanged
        */
        virtual void SizeChanged();
        
        //------------------------------------------------------------------
        // Member Functions - Private
        //------------------------------------------------------------------
    private:
        
        /**
        * Sets the colours of a thumb
        * @since Series60_version 2.5
        * @param aThumbColour - the colour of the thumb
        * @param aThumbShadowColour - the colour of the thumb shadow
        * @return none
        */
        void SetThumbColours(TRgb aThumbColour, TRgb aThumbShadowColour);		
        
        //------------------------------------------------------------------
        // Member Data - Private
        //------------------------------------------------------------------
    private:
        CScrollBar::TOrientation iOrientation;	// Orientation of the thumb
        
        TRect iThumbShadow;         // The outline of the thumb
        TRect iThumbCentre;		    // the rect of the thumb shown inside the thumb shadow
        
        TRgb iThumbColour;		    // the colour of the thumb
        TRgb iThumbShadowColour;	// the colour of the thumb shadow
    };

#endif  // _SCROLLTHUMB_H
