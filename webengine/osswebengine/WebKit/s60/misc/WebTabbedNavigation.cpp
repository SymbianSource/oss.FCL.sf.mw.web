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
* Description:  Frame in webkit side
*
*/

#include "config.h"
#include <../bidi.h>
#include <e32math.h>
#include <e32base.h>
#include "WebTabbedNavigation.h"
#include "WebUtil.h"
#include "Document.h"
#include "Frame.h"
#include "HitTestRequest.h"
#include "HitTestResult.h"
#include "HTMLNames.h"
#include "FrameTree.h"
#include "BrCtlDefs.h"
#include "WebCursor.h"
#include "StaticObjectsContainer.h"
#include "Page.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include <wtf/RefPtr.h>
#include "EventHandler.h"
#include "webkitlogger.h"
#include "FocusController.h"
#include "RenderListBox.h"
#include "HTMLSelectElement.h"
#include "HTMLInputElement.h"

using namespace WebCore;
using namespace HTMLNames;

const int KMaxJumpPercent = 90;
const int KScrollWhenNotFound = 5;

WebTabbedNavigation::WebTabbedNavigation(WebView* webView)
{
    m_webView = webView;
    clear();
}

WebTabbedNavigation::~WebTabbedNavigation()
{
}

void WebTabbedNavigation::clear()
{
    // Different initialization for mirrored layouts?
    m_selectedElementRect.SetRect(0, 0, m_webView->Rect().Width(), 0);
    m_initializedForPage = false;
    m_firstNavigationOnPage = true;
    m_focusPosition = TPoint(0,0);
    m_node = NULL;
}

void WebTabbedNavigation::initializeForPage()
{
    if (!m_initializedForPage) {
        if (navigate(0, 1)) {
            m_initializedForPage = true;
            m_firstNavigationOnPage = false;
        }
    }
}

void WebTabbedNavigation::updateCursorPosition(const TPoint& pos)
{
    m_focusPosition = pos;
    WebFrame* frame = StaticObjectsContainer::instance()->webCursor()->getFrameAtPoint(pos);
    TPoint point(frame->frameView()->viewCoordsInFrameCoords(pos));

    Element* node = core(frame)->document()->elementFromPoint(point.iX, point.iY);
    if (node->isFocusable() && !node->hasTagName(iframeTag) && !node->hasTagName(frameTag))
        m_webView->page()->focusController()->setFocusedNode(node, core(frame));
    else
        m_webView->page()->focusController()->setFocusedNode(NULL, core(frame));
    m_selectedElementRect.SetRect(pos.iX, pos.iY, pos.iX + 1, pos.iY + 1);
}

void WebTabbedNavigation::focusedElementChanged(Element* element)
{
    m_initializedForPage = true;
    m_firstNavigationOnPage = false;
    m_node = element;
    m_selectedElementRect = element->getRect().Rect();
    m_focusPosition = StaticObjectsContainer::instance()->webCursor()->position();
}

bool WebTabbedNavigation::navigate(int horizontalDir, int verticalDir)
{
    if (handleSelectElementScrolling(m_webView, verticalDir)) {
        StaticObjectsContainer::instance()->webCursor()->cursorUpdate(true);
        return true;
    }
    bool ret = m_firstNavigationOnPage;
    Frame* focusedFrame = m_webView->page()->focusController()->focusedFrame();
    if (focusedFrame == NULL) focusedFrame = m_webView->page()->mainFrame();
    if (focusedFrame->document()) {
        Node* focusNode = focusedFrame->document()->focusedNode();
        if (focusNode) {
            m_node = focusNode;
            m_selectedElementRect = focusNode->getRect().Rect();
            Frame* frame = focusNode->document()->frame();
            m_selectedElementRect = TRect(kit(frame)->frameView()->frameCoordsInViewCoords(m_selectedElementRect.iTl), 
                kit(frame)->frameView()->frameCoordsInViewCoords(m_selectedElementRect.iBr));
        }
    }
    TPoint oldFocusPoint(m_focusPosition);
    // Move the focus to the edge of the current object
    if (horizontalDir == -1) {
        m_focusPosition.iX = m_selectedElementRect.iTl.iX;
    }
    else if (horizontalDir == 1) {
        m_focusPosition.iX = m_selectedElementRect.iBr.iX;
    }
    if (verticalDir == -1) {
        m_focusPosition.iY = m_selectedElementRect.iTl.iY;
    }
    else if (verticalDir == 1) {
        m_focusPosition.iY = m_selectedElementRect.iBr.iY;
    }
    wkDebug()<<"WebTabbedNavigation::navigate. x = "<<m_focusPosition.iX<<" y = "<<m_focusPosition.iY<<flush;
    wkDebug()<<"x1 = "<<m_selectedElementRect.iTl.iX<<" y1 = "<<m_selectedElementRect.iTl.iY<<" x2 = "<<m_selectedElementRect.iBr.iX<<" y2 = "<<m_selectedElementRect.iBr.iY<<flush;
    
    // Adjust the move
    TPoint br;
    br.iX = m_webView->Rect().iBr.iX * KMaxJumpPercent / m_webView->scalingFactor();
    br.iY = m_webView->Rect().iBr.iY * KMaxJumpPercent / m_webView->scalingFactor();
    TRect view;
    // define the view rect where we are looking for a match
    if (horizontalDir == -1) {
        view.SetRect(m_focusPosition.iX - br.iX, m_focusPosition.iY - br.iY, m_focusPosition.iX, m_focusPosition.iY + br.iY);
    }
    else if (horizontalDir == 1) {
        view.SetRect(m_focusPosition.iX, m_focusPosition.iY - br.iY, m_focusPosition.iX + br.iX, m_focusPosition.iY + br.iY);
    }
    else if (verticalDir == -1) {
        view.SetRect(m_focusPosition.iX - br.iX, m_focusPosition.iY - br.iY, m_focusPosition.iX + br.iX, m_focusPosition.iY);
    }
    else if (verticalDir == 1) {
        view.SetRect(m_focusPosition.iX - br.iX, m_focusPosition.iY, m_focusPosition.iX + br.iX, m_focusPosition.iY + br.iY);
    }
    //wkDebug()<<"view x1 = "<<view.iTl.iX<<" y1 = "<<view.iTl.iY<<" x2 = "<<view.iBr.iX<<" y2 = "<<view.iBr.iY<<flush;
    // walk all focusable nodes
    Frame* f = m_webView->page()->mainFrame();
    TPoint selectedPoint(0, 0);
    TRect selectedRect(0, 0, 0, 0);
    Node* selectedNode = NULL;
    while ( f ) {
        PassRefPtr<HTMLCollection> elements = f->document()->all();   
        TRect frameRect = kit(f)->frameView()->rectInGlobalCoords();
        for (Node* n = elements->firstItem(); n; n = elements->nextItem()) {
            if (n == m_node) continue;
            if (n->isFocusable() && n->isElementNode() && !n->hasTagName(iframeTag) && !n->hasTagName(frameTag)) {
                // Does the node intersect with the view rect?
                TRect nodeRect = n->getRect().Rect();
                //wkDebug()<<"Each node rect x1 = "<<nodeRect.iTl.iX<<" y1 = "<<nodeRect.iTl.iY<<" x2 = "<<nodeRect.iBr.iX<<" y2 = "<<nodeRect.iBr.iY<<flush;
                nodeRect = TRect(kit(f)->frameView()->frameCoordsInViewCoords(nodeRect.iTl), 
                    kit(f)->frameView()->frameCoordsInViewCoords(nodeRect.iBr));
                if (nodeRect.Intersects(view)) {
                    // Compare nodes and select the best fit
                    TPoint newFocusPoint = potentialFocusPoint(horizontalDir, verticalDir, nodeRect);
                    wkDebug()<<"Matching node rect x1 = "<<nodeRect.iTl.iX<<" y1 = "<<nodeRect.iTl.iY<<" x2 = "<<nodeRect.iBr.iX<<" y2 = "<<nodeRect.iBr.iY<<flush;
                    if (selectNode(horizontalDir, verticalDir, selectedRect, nodeRect, selectedPoint, newFocusPoint)) {
                        // found a better fit
                        selectedNode = n;
                        selectedRect.SetRect(nodeRect.iTl, nodeRect.iBr);
                        selectedPoint = newFocusPoint;
                    }
                } // if (nodeRect.Intersects(rect[i])
            } // if (n->isFocusable() && n->isElementNode())
        } // for (Node* n = elements->firstItem(); n; n = elements->nextItem())
        f = f->tree()->traverseNext();
    } // while ( f )
    // Remember new selection
    TPoint contentPos = m_webView->mainFrame()->frameView()->contentPos();
    if (selectedNode) {
        // Found an element to jump to
        m_selectedElementRect = selectedRect;
        m_focusPosition = selectedPoint;
        m_node = selectedNode;
        selectedNode->document()->setFocusedNode(selectedNode);
         m_webView->page()->focusController()->setFocusedFrame(selectedNode->document()->frame());
        // And scroll to the selected element
        RenderLayer *layer = selectedNode->renderer()->enclosingLayer();
        if (layer) {
            layer->scrollRectToVisible(selectedNode->getRect(), RenderLayer::gAlignCenterIfNeeded, RenderLayer::gAlignCenterIfNeeded);
            TRect newRect = TRect(kit(selectedNode->document()->frame())->frameView()->frameCoordsInViewCoords(selectedNode->getRect().Rect().iTl), 
                kit(selectedNode->document()->frame())->frameView()->frameCoordsInViewCoords(selectedNode->getRect().Rect().iBr));
            selectedPoint += (newRect.iTl - selectedRect.iTl);
            m_focusPosition = selectedPoint;
            selectedRect = newRect;
            m_selectedElementRect = selectedRect;

            int x, y;
            selectedNode->renderer()->absolutePosition(x, y);
            Vector<IntRect> rects;
            selectedNode->renderer()->absoluteRects(rects, x, y);
            WebFrameView* fv = kit(selectedNode->document()->frame())->frameView();
            if (rects.size() > 0) {
                 selectedPoint = TPoint(rects[0].x(), rects[0].y());
                selectedPoint = fv->frameCoordsInViewCoords(selectedPoint);
            }
            StaticObjectsContainer::instance()->webCursor()->updatePositionAndElemType(selectedPoint);
            // special handling for Select-Multi
            if (m_webView->focusedElementType() == TBrCtlDefs::EElementSelectMultiBox) {
                Element* e = static_cast<Element*>(m_node);
                if (e->isControl()) {
                    HTMLGenericFormElement* ie = static_cast<HTMLGenericFormElement*>( e );
                    if (ie->type() == "select-multiple") {
                        RenderListBox* render = static_cast<RenderListBox*>(e->renderer());
                        HTMLSelectElement* selectElement = static_cast<HTMLSelectElement*>( e );
                        IntRect itemRect = render->itemRect(0, 0, 0);
                        TPoint cursorPoint(StaticObjectsContainer::instance()->webCursor()->position());
                        int gap = (20 * m_webView->scalingFactor()) / 100;
                        cursorPoint.iX = max(m_focusPosition.iX,  m_selectedElementRect.iTl.iX + gap);
                        cursorPoint.iX = std::min(cursorPoint.iX,  m_selectedElementRect.iBr.iX - gap);
                        if (verticalDir == -1) {
                            cursorPoint.iY -= (itemRect.height() * m_webView->scalingFactor()) / 125;
                        }
                        if (cursorPoint != StaticObjectsContainer::instance()->webCursor()->position()) {
                            StaticObjectsContainer::instance()->webCursor()->setPosition(cursorPoint);
                        }
                    }
                }
            }
            TPointerEvent event;
            event.iPosition = StaticObjectsContainer::instance()->webCursor()->position();
            event.iModifiers = 0;
            event.iType = TPointerEvent::EMove;
            core(m_webView->mainFrame())->eventHandler()->handleMouseMoveEvent(PlatformMouseEvent(event));            
            wkDebug()<<"Focus position x = "<<selectedPoint.iX<<" y = "<<selectedPoint.iY<<flush;
            ret = true;
        }
    }
    else {
        if (!m_firstNavigationOnPage) {
            m_webView->mainFrame()->frameView()->scrollTo(contentPos + TPoint(horizontalDir * m_webView->Rect().Width() / KScrollWhenNotFound, verticalDir * m_webView->Rect().Height() / KScrollWhenNotFound));
            TPoint diff(m_webView->mainFrame()->frameView()->contentPos() - contentPos);
            if (diff.iX || diff.iY) {
                Frame* focusedFrame = m_webView->page()->focusController()->focusedFrame();
                if (focusedFrame == NULL) focusedFrame = m_webView->page()->mainFrame();
                Node* focusNode = focusedFrame->document()->focusedNode();
                if (focusNode) {
                    TRect selectedRect = focusNode->getRect().Rect();
                    selectedRect = TRect(kit(focusedFrame)->frameView()->frameCoordsInViewCoords(selectedRect.iTl), 
                        kit(focusedFrame)->frameView()->frameCoordsInViewCoords(selectedRect.iBr));
                    if (!selectedRect.Intersects(kit(focusedFrame)->frameView()->visibleRect()))
                        m_webView->page()->focusController()->setFocusedNode(NULL,0);
                }
                m_selectedElementRect.Move(diff);
                m_focusPosition = oldFocusPoint + diff;
                m_node = NULL;
                StaticObjectsContainer::instance()->webCursor()->updatePositionAndElemType(m_focusPosition - m_webView->mainFrame()->frameView()->contentPos());
                ret = true;
            }
            else
            {
                m_focusPosition = oldFocusPoint;
            }
        }
    }
    StaticObjectsContainer::instance()->webCursor()->cursorUpdate(true);
    return ret;
}

bool WebTabbedNavigation::selectNode(int horizontalDir, int verticalDir, TRect& selectedRect, TRect& newNodeRect, TPoint& selectedPoint, TPoint& newFocusPoint)
{
    if (selectedPoint == TPoint(0, 0)) { // first selected node fixme: ensure not to divert direction too much
        return true;
    }
    int selectedDist = distanceFunction(horizontalDir, verticalDir, selectedRect, selectedPoint);
    int newDist = distanceFunction(horizontalDir, verticalDir, newNodeRect, newFocusPoint);
    wkDebug()<<"WebTabbedNavigation::selectNode. selected x = "<<selectedPoint.iX<<" y = "<<selectedPoint.iY<<" new x = "<<newFocusPoint.iX<<" y = "<<newFocusPoint.iY<<"old distance = "<<selectedDist<<" new distance = "<<newDist<<flush;
    return newDist < selectedDist;
}

TPoint WebTabbedNavigation::potentialFocusPoint(int horizontalDir, int verticalDir, TRect& newNodeRect)
{
    int x = 0;
    int y = 0;
    // Focus points must be inside the element and not on the border in order for the outline to be drawn.
    if (horizontalDir) { // horizontal
        if (horizontalDir == -1) { // left
            x = newNodeRect.iBr.iX - 1;
        }
        else if (horizontalDir == 1) { // right
            x = newNodeRect.iTl.iX + 1;
        }
        if (newNodeRect.iBr.iY < m_focusPosition.iY) {
            y = newNodeRect.iBr.iY - 1;
        }
        else if (newNodeRect.iTl.iY > m_focusPosition.iY) {
            y = newNodeRect.iTl.iY + 1;
        }
        else {
            y = m_focusPosition.iY;
        }
    }
    else { // vertical
        if (verticalDir == -1) { // up
            y = newNodeRect.iBr.iY - 1;
        }
        else if (verticalDir == 1) { // down
            y = newNodeRect.iTl.iY + 1;
        }
        if (newNodeRect.iBr.iX < m_focusPosition.iX) {
            x = newNodeRect.iBr.iX - 1;
        }
        else if (newNodeRect.iTl.iX > m_focusPosition.iX) {
            x = newNodeRect.iTl.iX + 1;
        }
        else {
            x = m_focusPosition.iX;
        }
    }
    return TPoint(x, y);
}

int WebTabbedNavigation::distanceFunction(int horizontalDir, int verticalDir, TRect& rect, TPoint& point)
{
    // Based on http://www.w3.org/TR/WICD/#focus-navigation
    TReal x, y, euclidianDistance;
    int sameAxisDist, otherAxisDist, overlap;

    Math::Pow(x, (m_focusPosition.iX - point.iX), 2.0);
    Math::Pow(y, (m_focusPosition.iY - point.iY), 2.0);
    Math::Sqrt(euclidianDistance, x + y);
    sameAxisDist = horizontalDir * (point.iX - m_focusPosition.iX) + verticalDir * (point.iY - m_focusPosition.iY);
    otherAxisDist = (horizontalDir) ? (point.iY - m_focusPosition.iY) : (point.iX - m_focusPosition.iX) ;
    otherAxisDist = (otherAxisDist < 0 ) ? (otherAxisDist * -1) : otherAxisDist;
    if (horizontalDir) { // horizontal
        if (rect.iBr.iY < m_selectedElementRect.iTl.iY || rect.iTl.iY > m_selectedElementRect.iBr.iY) {
            overlap = 0;
        }
        else {
            int top = max(m_selectedElementRect.iTl.iY, rect.iTl.iY);
            int bottom = std::min(m_selectedElementRect.iBr.iY, rect.iBr.iY);
            overlap = bottom - top;
            if (overlap == rect.Height()) {
                euclidianDistance = (euclidianDistance * rect.Height()) / m_selectedElementRect.Height();
                otherAxisDist = 0;
            }
        }
    }
    else { // vertical    
        if (rect.iBr.iX < m_selectedElementRect.iTl.iX || rect.iTl.iX > m_selectedElementRect.iBr.iX) {
            overlap = 0;
        }
        else {
            int top = max(m_selectedElementRect.iTl.iX, rect.iTl.iX);
            int bottom = std::min(m_selectedElementRect.iBr.iX, rect.iBr.iX);
            overlap = bottom - top;
            if (overlap == rect.Width()) {
                euclidianDistance = (euclidianDistance * rect.Width()) / m_selectedElementRect.Width();
                otherAxisDist = 0;
            }
        }
    }
    long ed, o;
    Math::Int(ed, euclidianDistance);
    Math::Int(o, sqrt(overlap));
    return ed + sameAxisDist + 2 * otherAxisDist - o;
}
