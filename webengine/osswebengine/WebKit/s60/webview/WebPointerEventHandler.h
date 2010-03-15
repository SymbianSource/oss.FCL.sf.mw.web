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
* Description:   Implemetation of WebPointerEventHandler
*
*/

#ifndef __WEBPOINTEREVENTHANDLER_H__
#define __WEBPOINTEREVENTHANDLER_H__

#include <e32base.h>
#include <w32std.h>
#include <stmgestureinterface.h>
#include "Timer.h"


namespace WebCore
{
    class Node;
    class Element;
};

class WebView;
class CActiveSchedulerWait;
class WebGestureInterface;

class WebPointerEventHandler : public CBase
{
public:
    static WebPointerEventHandler* NewL(WebView* view);     
    void ConstructL();
    virtual ~WebPointerEventHandler();

public:
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void HandleHighlightChange(const TPoint &aPoint);
    void HandleGestureEventL(const TStmGestureEvent& aGesture);
    
private:    
    bool checkForEventListener(WebCore::Node* node);
    bool canDehighlight(const TPoint &aPoint);
    void dehighlight(const TPoint &aPoint = TPoint(-1,-1));

    bool isHighlitableElement(TBrCtlDefs::TBrCtlElementType& elType);
    TBrCtlDefs::TBrCtlElementType highlitableElement();
    void buttonDownTimerCB(WebCore::Timer<WebPointerEventHandler>* t);
    void handleTouchDownL(const TStmGestureEvent& aGesture);
    void handleTouchUp(const TStmGestureEvent& aGesture);
    void handleTapL(const TStmGestureEvent& aGesture);
    void handleDoubleTap(const TStmGestureEvent& aGesture);
    void handleMove(const TStmGestureEvent& aGesture);
    void doTapL();
    void updateCursor(const TPoint& pos);
    void setFocusRing();
    void handlePinchZoomL(const TStmGestureEvent& aGesture);
private:
    WebPointerEventHandler(WebView* view);            
    WebView* m_webview;

               
    TPointerEvent m_buttonDownEvent;
    TPointerEvent m_lastTapEvent;
    
    // Support for link highlight/dehighlight
    TBool m_isHighlighted;
    TInt m_offset;
    TPoint m_highlightPos;
    WebCore::Node* m_highlightedNode;
    TPointerEvent m_currentEvent;
    WebCore::Timer<WebPointerEventHandler> m_buttonDownTimer; 
    
    TPointerEvent m_lastPointerEvent;
    bool   m_ignoreTap; 
    CActiveSchedulerWait*    m_waiter; 
    WebGestureInterface*  m_gestureInterface; 
    
};



#endif
