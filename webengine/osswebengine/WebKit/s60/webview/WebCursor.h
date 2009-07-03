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
* Description:   Implemetation of WebCursor
*
*/

#ifndef __WEBKITCURSOR_H__
#define __WEBKITCURSOR_H__

#include <e32base.h>
#include "WebCoreWidget.h"
#include "WebCannedImages.h"
#include "WebSprite.h"
#include <w32std.h>

namespace WebCore {
    class IntRect;
    class IntPoint;
}
#define KInitialOffset 5

class WebView;
class WebFrame;
class CWindowGc;
class WebFrame;
class CFbsBitmap;
class CWebSprite;

class WebCursor : public CBase
    {
    public: // constructors and destructors

        static WebCursor* NewL();
        virtual ~WebCursor();

    public:

        const TPoint& position() const { return m_pos; }
        const TRect& nodeRect() const { return m_nodeRect; }
        void setPosition(const TPoint& pt) { m_pos = pt;}
        void updatePositionAndElemType(const TPoint& pt);
        void setCurrentView(WebView& view);
        void offsetCursor(const TPoint& offset);
        bool isVisible() const { return m_visible; }
        void setCursorVisible(bool visible) { m_visible = visible; }
        void setCursor(CursorTypes);
        void cursorUpdate(bool visible);
        void moveCursorAdjusted(int lr,int tb,int scrllrange);
        void reset();
        void setWaitCursor( bool waiton );
        bool waitCursor() { return m_waiton; }
        void setTransparent(bool transparent);
        bool transparent() const { return m_transparent; }
        void setOpaqueUntil(int microsecs);
        void increaseTransparencyMoveCount();
        void resetTransparency();
        void scrollAndMoveCursor(int dir, int scrollRange, bool auto);
        WebFrame* getFrameUnderCursor();
        bool navigableNodeUnderCursor(WebFrame& webFrame, TPoint& aPoint, TBrCtlDefs::TBrCtlElementType& aElType, TRect& aFocusRect) const;
        WebFrame* getFrameAtPoint(const TPoint& viewPos_);
        

  private:
        void moveCursor(int lr,int tb, int scrollRange);
        bool determineCursorPosition(WebFrame& webFrame, TBrCtlDefs::TBrCtlElementType& aElType, TRect& aFocusRect, TRect& aSearchRect, 
            TPoint& aCursorPosition, bool aInitialize);
        void increaseSearchRect(int lr,int tb,TRect& aRect);
        bool decideCursorPosition(WebFrame& webFrame, const WebCore::IntRect& searchRect, WebCore::IntPoint& cursorPosition);
        WebFrame* calculateScrollableFrameView(TPoint& pos, TPoint& aPoint, TRect& fRect, bool autoscroll);
        void innerRect(TRect& rect);
        bool frameHasContentToScroll(WebFrame* frame, TPoint& delta);
        WebFrame*  determineScrollingFrame(int border1, int border2, int pos, 
        		                           WebFrame* fr1, WebFrame* fr2, TPoint& delta);
    private:

        WebCursor();
        void ConstructL();
        TRect calcSearchRect(int lr, int tb, int scrollRange);
        void constructSpriteL();
        

        TPoint                          m_pos;
        int                             m_previousLr;
        int                             m_previousTb;
        TRect                           m_nodeRect;
        TCannedImageData                m_hand;
        TCannedImageData                m_arrow;
        TCannedImageData                m_ibeam;        
        TCannedImageData                m_wait;
        TCannedImageData                m_selectMulti;

        //TCannedImageData                iSmartLinkPhoneImage;
        //TCannedImageData                iSmartLinkEmailImage;

        WebView*                        m_view;
        bool                            m_visible;
        
        CWebSprite*                     m_sprite;
        bool                            m_waiton;
        int                             m_flipcounter;
        bool                            m_transparent;
        CFbsBitmap*                     m_transarrowmask;
        CPeriodic*                      m_transtimer;
        int                             m_transcount;
        CursorTypes                     m_type;
        int                             m_flipCounter;
        TPoint                          m_incrLimit;
    };

#endif
