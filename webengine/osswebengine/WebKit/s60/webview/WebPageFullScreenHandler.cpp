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
* Description:   
*
*/


// INCLUDE FILES
#include <../bidi.h>
#include <aknAppUi.h>
#include <aknutils.h>
#include "StaticObjectsContainer.h"
#include "WebCannedImages.h"
#include "WebPageFullScreenHandler.h"
#include "WebView.h"
#include "BrCtl.h"

using namespace WebCore;

const int KFullScreenButtonHeight = 60;
const int KFullScreenButtonWidth = 60;
const int KFullScreenButtonBuff  = 5;

// -----------------------------------------------------------------------------
// WebPageFullScreenHandler::NewL
// The two-phase Symbian constructor
// -----------------------------------------------------------------------------
//
WebPageFullScreenHandler* WebPageFullScreenHandler::NewL(WebView& webView)
{
    WebPageFullScreenHandler* self = new (ELeave) WebPageFullScreenHandler( webView );
    CleanupStack::PushL(self);
    self->constructL();
    CleanupStack::Pop(); //self
    return self;
}

// -----------------------------------------------------------------------------
// WebPageFullScreenHandler::WebPointerEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebPageFullScreenHandler::WebPageFullScreenHandler(WebView& webView)
: m_webView(&webView), m_spriteVisible(EFalse), m_tappedOnSprite(EFalse)
{
}

// -----------------------------------------------------------------------------
// WebPageFullScreenHandler::constructL
// The constructor that can contain code that might leave.
// -----------------------------------------------------------------------------
//
void WebPageFullScreenHandler::constructL()
{
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
WebPageFullScreenHandler::~WebPageFullScreenHandler()
{
}


//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::showEscBtnL
// Draws the full screen button on the screen
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::showEscBtnL() 
{
    if (AknLayoutUtils::PenEnabled() && !m_spriteVisible) {
        constructSprite();
        SizeChanged();
        AddWsObserverToControl();
    }
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::hideEscBtnL
// Draws the button on the screen
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::hideEscBtnL()
{
    if (AknLayoutUtils::PenEnabled() && m_spriteVisible) {
        destructSprite();
        m_eventMonitor->RemoveObserver(this);
    }
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::SizeChanged
// Handle changes in size of the displayable area.  
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::SizeChanged(void)
{
    if (AknLayoutUtils::PenEnabled() && m_spriteVisible) {
        m_pos = m_webView->Rect().iBr;
        TPoint point (m_webView->PositionRelativeToScreen());
        m_pos -= TSize(  KFullScreenButtonWidth + KFullScreenButtonBuff, 
        				 KFullScreenButtonHeight - point.iY + KFullScreenButtonBuff);
        m_sprite.SetPosition(m_pos);
    }
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::fullScreenMode
// Returns true if a page is in the full screen mode
//-------------------------------------------------------------------------------

bool WebPageFullScreenHandler::isFullScreenMode()
{
    return StaticObjectsContainer::instance()->fullScreenMode();
}


//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::HitRegionContains
// Defines a control's hit region
//-------------------------------------------------------------------------------

TBool WebPageFullScreenHandler::HitRegionContains(const TPoint& aPoint)
{
    TRect spriteRect (m_pos, TSize(KFullScreenButtonHeight, KFullScreenButtonWidth));
    return spriteRect.Contains(aPoint);
}


//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::destructSprite
// close/hide sprite for time being
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::destructSprite()
{
    m_sprite.Close();
    m_spriteVisible = EFalse;
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::constructSprite
// Show/create sprite at m_pos
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::constructSprite()
{
    m_sprite = RWsSprite(m_webView->brCtl()->CCoeControlParent()->ControlEnv()->WsSession());
    m_sprite.Construct(m_webView->brCtl()->CCoeControlParent()->ControlEnv()->RootWin(), m_pos, ESpriteNoShadows);
    TCannedImageData cannedImage = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageEscFullScreen);

	TSpriteMember spriteMem;
	spriteMem.iBitmap = cannedImage.m_img; 
	spriteMem.iMaskBitmap = cannedImage.m_msk; //masked bitmap still of the canned image need to be changed

    m_sprite.AppendMember(spriteMem);
	m_sprite.Activate();
	
	m_spriteVisible = ETrue;
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::HandleWsEventL
// check if click region hits sprite
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination)
{
    if (!aDestination) return;
    
    if(aEvent.Type() == EEventPointer)
       {
       switch (aEvent.Pointer()->iType)
           {
           case TPointerEvent::EButton1Down:
               if (m_spriteVisible)
                   {
                   TPoint point (aDestination->PositionRelativeToScreen());
                   m_tappedOnSprite = HitRegionContains(aEvent.Pointer()->iPosition + point);
                   }
               break;

           case TPointerEvent::EButton1Up:
               {
               TPoint point (aDestination->PositionRelativeToScreen());
               if (m_spriteVisible && m_tappedOnSprite && 
                    HitRegionContains(aEvent.Pointer()->iPosition + point))
                   {
                   hideEscBtnL();
                   m_webView->notifyFullscreenModeChangeL( false );
                   }
               m_tappedOnSprite = EFalse;
               }
               break;
           
            }
       }
}


//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::AddWsObserverToControl
// register for events from WSession
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::AddWsObserverToControl()
{
    m_eventMonitor = ((CAknAppUi*)m_webView->ControlEnv()->AppUi())->EventMonitor();
    
    if(m_eventMonitor)
        {
        m_eventMonitor->AddObserverL(this);
        m_eventMonitor->Enable(ETrue);
        }
}
