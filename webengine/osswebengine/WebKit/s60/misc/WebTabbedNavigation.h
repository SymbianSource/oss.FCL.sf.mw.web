/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implemetation of CWebKitView
*
*/

#ifndef __WEBTABBEDNAVIGATION_H__
#define __WEBTABBEDNAVIGATION_H__

#include <e32std.h>

class WebView;

class WebTabbedNavigation {
    public:

        WebTabbedNavigation(WebView* webView);
        ~WebTabbedNavigation();
        bool navigate(int horizontalDir, int verticalDir);
        void clear();
        void initializeForPage();

    private:
        bool selectNode(int horizontalDir, int verticalDir, TRect& selectedRect, TRect& newNodeRect, TPoint& selectedPoint, TPoint& newFocusPoint);
        TPoint potentialFocusPoint(int horizontalDir, int verticalDir, TRect& newNodeRect);
        int distanceFunction(int horizontalDir, int verticalDir, TRect& rect, TPoint& point);

    private:
        TRect m_selectedElementRect;
        WebView* m_webView; // not owned
        bool m_initializedForPage;
        bool m_firstNavigationOnPage;
        TPoint m_focusPosition;
        void* m_node;
};

#endif //__WEBTABBEDNAVIGATION_H__

// END OF FILE

