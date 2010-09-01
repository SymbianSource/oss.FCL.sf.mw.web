/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements pinch functionality
*
*/



#ifndef WEBPAGEPINCHZOOMHANDLER_H
#define WEBPAGEPINCHZOOMHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>
#include <stmgestureinterface.h>

// MACROS

// FORWARD DECLARATIONS
class WebView;


// CLASS DECLARATION
class WebPagePinchZoomHandler: public CBase {
public:  // Constructor and destructor

    /**
     * Two-phased constructor.
     **/
    static WebPagePinchZoomHandler* NewL(WebView* webView);

    /**
     * Destructor.
     **/
    virtual ~WebPagePinchZoomHandler();

private:  // Constructors

    /**
     * C++ default constructor.
     **/
    WebPagePinchZoomHandler(WebView* webView);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void constructL();
  
public: // New functions
    
    /**
     * Handler for Pinch event
     **/
    void handlePinchGestureEventL(const TStmGestureEvent& aGesture);
    
    /**
     * Handler for PinchEnter event
     **/
    void handlePinchGestureL(const TStmGestureEvent& aGesture);
    
    /**
     * Handler for PinchExit event
     **/
    void handlePinchGestureExitL(const TStmGestureEvent& aGesture);

    /**
     * to set the zoom level for bitmap zooming
     **/
    void setZoomLevelL(TInt zoomLevel);

    /**
     * to update the bitmap
     **/
    void updateBitmap();
    
    /**
     * to get the pinch step size corresponding to a pinch gesture
     **/
    TInt getPinchZoomStepSize();
    
    /**
     * to query whether pinch is active: 
     * pinch is active once GestureEnter for pinch is received. And it is active until the pinch expiry timer gets expired 
     **/
    TBool isPinchActive();
    
    /**
     * handler for pinch exit timer expiry
     **/
    void handlepinchExitWaitTimer();
    
    /**
     * To get the pinch center
     **/
    TPoint pinchCenter() {return m_pinchCenter;};

private:   

    WebView*         m_webView;            // <<not owned>>
    CPeriodic*       m_bitmapUpdateTimer;  // <<owned>> 
    CPeriodic*       m_pinchExitWaitTimer; // <<owned>> 
    TInt             m_pinchFactor;
    int              m_zoomOutBaseLevel;
    bool             m_pinchCenterSet;
    TInt             m_zoomStepSize;
    bool             m_pinchActive;
    TPoint           m_pinchCenter;

};

#endif      //WEBPAGEPINCHZOOMHANDLER_H

// End of File
