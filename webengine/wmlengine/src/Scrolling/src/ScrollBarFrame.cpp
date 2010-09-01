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
* Description:  Implementation of CScrollBarFrame and 
*				 CScrollBarCornerWindow
*
*/

#include "ScrollBarFrame.h"
#include "ScrollBar.h"
#include "ScrollThumb.h"

#include <e32std.h>
#include <e32def.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// CScrollBarFrame::NewL(const CCoeControl& aParent)
//
// Two-phased construction
//--------------------------------------------------------------------
//
CScrollBarFrame* CScrollBarFrame::NewL(const CCoeControl& aParent)
	{
	CScrollBarFrame* self = new (ELeave) CScrollBarFrame();
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	CleanupStack::Pop();	// self
	return self;
	}

//--------------------------------------------------------------------
// CScrollBarFrame::CScrollBarFrame()
//
// First Phase Construction
//--------------------------------------------------------------------
//
CScrollBarFrame::CScrollBarFrame()
	{
	SetLayout(EOriginTopLeft);
	}

//--------------------------------------------------------------------
// CScrollBarFrame::ConstructL(const CCoeControl& aParent)
// 
// Second Phase Constructor
//--------------------------------------------------------------------
//
void CScrollBarFrame::ConstructL(const CCoeControl& aParent)
	{
	// Create Scroll Bars
	iVScrollBar = CScrollBar::NewL(CScrollBar::EVertical, aParent);
	iHScrollBar = CScrollBar::NewL(CScrollBar::EHorizontal, aParent);

    iCornerWindow = CScrollBarCornerWindow::NewL(aParent);
	}


//--------------------------------------------------------------------
// CScrollBarFrame::~CScrollBarFrame()
// 
// DESTRUCTOR
//--------------------------------------------------------------------
//
CScrollBarFrame::~CScrollBarFrame()
	{
	delete iCornerWindow;
	delete iVScrollBar;
	delete iHScrollBar;
	}

//--------------------------------------------------------------------
// void CScrollBarFrame::UpdateVScrollBarL()
//--------------------------------------------------------------------
//
void CScrollBarFrame::UpdateVScrollBarL()
	{
	SetScrollBar(CScrollBar::EVertical);
	}

//--------------------------------------------------------------------
// void CScrollBarFrame::UpdateHScrollBarL()
//--------------------------------------------------------------------
//
void CScrollBarFrame::UpdateHScrollBarL()
	{
	SetScrollBar(CScrollBar::EHorizontal);
	}


//-----------------------------------------------------------------------------
// void CScrollBarFrame::NotifyLayoutChange(TBrCtlLayout aLayout)
//-----------------------------------------------------------------------------
//
void CScrollBarFrame::NotifyLayoutChange(TBrCtlLayout aLayout)
	{
	iLayout = aLayout;
	}


//-----------------------------------------------------------------------------
// void CScrollBarFrame::SetScrollBar(const CScrollBar::TOrientation aOrientation)
//-----------------------------------------------------------------------------
//
void CScrollBarFrame::SetScrollBar(const CScrollBar::TOrientation aOrientation)
	{
	// scrollbar coordinates
	TInt tlX(0);
	TInt tlY(0);
	TInt brX(0);
	TInt brY(0);

	TInt height = iDisplay.Height();
	TInt width = iDisplay.Width();
	
	// Set Vertical Scroll Bar
	// Calculate size and position of Vertical Scroll Bar
	if (aOrientation == CScrollBar::EVertical)	
		{	
		switch (iLayout)
			{
			case EOriginTopLeft:
				{
				tlX = width;
				tlY = 0;
				brX = width + KBrowserVScrollBarWidth;
				brY = height;
				break;
				}
			case EOriginTopRight:
				{
				tlX = 0;
				tlY = 0;
				brX = KBrowserVScrollBarWidth;
				brY = height;
				break;
				}
			default:
				break;
			}
		iVScrollBar->SetTrackAndBackground(TRect(tlX, tlY, brX, brY));
		}

	// Set Horizontal Scroll Bar
	else
		{
		// If we need a horizontal scrollbar
		if (iDocument.Width() > iDisplay.Width())
			{
			TRect cornerWindow;
			switch (iLayout)
				{
				case EOriginTopLeft:
					{
					tlX = 0;
					tlY = height;
					brX = width;
					brY = height + KBrowserHScrollBarHeight;

					cornerWindow.SetRect(width, height,
                                         width + KBrowserVScrollBarWidth,
										 height + KBrowserHScrollBarHeight);
					iCornerWindow->SetRect(cornerWindow);
					break;				
					}
				case EOriginTopRight:
					{
					tlX = KBrowserVScrollBarWidth;
					tlY = height;
					brX = width + KBrowserVScrollBarWidth;
					brY = height + KBrowserHScrollBarHeight;
					
					cornerWindow.SetRect(0, height, KBrowserVScrollBarWidth, 
										 height + KBrowserHScrollBarHeight);
					iCornerWindow->SetRect(cornerWindow);					
					break;
					}
				default:
					break;
				}
			}
		iHScrollBar->SetTrackAndBackground(TRect(tlX, tlY, brX, brY));
		}	
	
	// Set scrollbar thumb attributes
	SetThumb(aOrientation);
	
	if (aOrientation == CScrollBar::EVertical)
		{
		iVScrollBar->DrawNow();
		}
	else
		{
		iHScrollBar->DrawNow();
		}
	iCornerWindow->DrawNow();
	}


//----------------------------------------------------------------------------
// void CScrollBarFrame::SetThumb(const CScrollBar::TOrientation aOrientation)
//----------------------------------------------------------------------------
//
void CScrollBarFrame::SetThumb(CScrollBar::TOrientation aOrientation)
	{	
	TInt display;
	TInt document;
	TInt displayPos;	
	TRect sbThumb;

	// Set Vertical THUMB
	if (aOrientation == CScrollBar::EVertical)
		{
		TInt height = 0;
		TInt posY = 0;

		display = iDisplay.Height();
		document = iDocument.Height();
		displayPos = iDisplayPos.iY;
 
		if (document)	// avoid dividing by zero
			{
			// Height of the thumb
			height = (display * display) / document;
			
			// Position of thumb
			posY = (display * displayPos) / document;
			}

		// Set thumb
		sbThumb.SetRect(iVScrollBar->Rect().iTl.iX, posY,
						iVScrollBar->Rect().iBr.iX, (posY + height));
		iVScrollBar->iThumb->SetThumbAndShadow(sbThumb);
		}
	// Set Horizontal THUMB
	else
		{
		TInt width = 0;
		TInt posX = 0;

		display = iDisplay.Width();
		document = iDocument.Width();
		displayPos = iDisplayPos.iX;

		if (document)	// avoid dividing by zero
			{
			// Width of the thumb		
			width = (display * display) / document;
			// Position of thumb
			posX = (display * displayPos) / document;
			}

		// shift thumb to the right
		if (iLayout == EOriginTopRight)
			{
			posX += KBrowserVScrollBarWidth;
			}

		// Set Thumb
		sbThumb.SetRect(posX, iHScrollBar->Rect().iTl.iY, (posX + width),
                        iHScrollBar->Rect().iBr.iY);
		iHScrollBar->iThumb->SetThumbAndShadow(sbThumb);
		}
	}


//----------------------------------------------------------------------------
// TInt CScrollBarFrame::VScrollBarNeeded(TInt aDocument, TInt aDisplay,
//										  TInt aDisplayPos)
//
//	External Function determining if Vertical Scroll Bar is needed
//	returns width of scrollbar
//----------------------------------------------------------------------------
//
/*
TInt CScrollBarFrame::VScrollBarNeeded(TInt aDocument, TInt aDisplay,
									   TInt aDisplayPos)
	{
	TInt ret = 0;

	// Set data members
	iDocument.SetHeight(aDocument);
	SetDisplayHeight(aDisplay);
	iDisplayPos.iY = aDisplayPos;
	
	// Modify Display rect to accomodate scrollbar - if not already present
	if ((iDisplay.Width() == KBrowserMaxDisplayWidth) && iDocument.Width())
		{
		ret = KBrowserVScrollBarWidth;
		}
	return ret;	
	}


//----------------------------------------------------------------------------
// TInt CScrollBarFrame::HScrollBarNeeded(TInt aDocument,
//										  TInt aDisplay,
//										  TInt aDisplayPos,
//										  TInt aDisplayHeight)
//----------------------------------------------------------------------------
//
TInt CScrollBarFrame::HScrollBarNeeded(TInt aDocument,
									   TInt aDisplay,
									   TInt aDisplayPos,
									   TInt aDisplayHeight)
	{
	TInt ret = 0;
	iDocument.SetWidth(aDocument);
	SetDisplayWidth(aDisplay);
	iDisplayPos.iX = aDisplayPos;

	// Modify Display rect to accomodate scrollbar - if not already present
	if (iDocument.Height())
		{
		// Calculate space needed for H Scrollbar
		if (iDocument.Width() > iDisplay.Width())
			{
			if (iDisplay.Height() == aDisplayHeight)	// H Scrollbar present?
				{
				ret = KBrowserHScrollBarHeight;
				}
			}
		// H Scrollbar not needed any more if doc not wider than display 
		// (with or without a V scrollbar being present)
		else if (iDocument.Width() <= iDisplay.Width() ||
				 iDisplay.Width() == KBrowserMaxDisplayWidth)
			{			
			if ((aDisplayHeight - iDisplay.Height()) == KBrowserHScrollBarHeight)
				{
				ret = KBrowserHScrollBarHeight * KConvertToNegativeValue;
				}
			// We also don't need a corner window
			iCornerWindow->SetRect(TRect(0, 0, 0, 0));
			}
		}	
	return ret;	
	}
*/
//----------------------------------------------------------------------------
// void CScrollBarFrame::SetDisplayHeight( const TInt aHeight )
//----------------------------------------------------------------------------
//
void CScrollBarFrame::SetDisplayHeight(const TInt aHeight)
	{
	iDisplay.SetHeight(aHeight);
	iScrollBarRect.SetHeight(aHeight + KBrowserHScrollBarHeight);
	}

//----------------------------------------------------------------------------
// void CScrollBarFrame::SetDisplayWidth(const TInt aWidth)
//----------------------------------------------------------------------------
//
void CScrollBarFrame::SetDisplayWidth(const TInt aWidth)
	{
	iDisplay.SetWidth(aWidth);
	iScrollBarRect.SetWidth(aWidth + KBrowserVScrollBarWidth);
	}


//----------------------------------------------------------------------------
//	CScrollBarCornerWindow Class Implementation
//----------------------------------------------------------------------------

//--------------------------------------------------------------------
// CScrollBarCornerWindow::CScrollBarCornerWindow(const CCoeControl& aParent)
//
// Second Phase Construction
//--------------------------------------------------------------------
//
CScrollBarCornerWindow* CScrollBarCornerWindow::NewL(const CCoeControl& aParent)
	{
    CScrollBarCornerWindow* self = new (ELeave)CScrollBarCornerWindow;
    CleanupStack::PushL(self);
    self->ConstructL(aParent);
    CleanupStack::Pop();
    return self;
    }

//--------------------------------------------------------------------
// CScrollBarCornerWindow::ConstructL(const CCoeControl& aParent)
//
// ConstructL
//--------------------------------------------------------------------
//
void CScrollBarCornerWindow::ConstructL(const CCoeControl& aParent)
    {
	SetContainerWindowL(aParent);
	}

//--------------------------------------------------------------------
// CScrollBarCornerWindow::~CScrollBarCornerWindow()
// 
// DESTRUCTOR
//--------------------------------------------------------------------
//
CScrollBarCornerWindow::~CScrollBarCornerWindow()
	{
	}

//----------------------------------------------------------------------------
//	void CScrollBarCornerWindow::Draw(const TRect& /*aRect*/) const
//----------------------------------------------------------------------------
//
void CScrollBarCornerWindow::Draw(const TRect& /*aRect*/) const
	{	
	const TRgb KBrowserCornerWindowColour = KRgbWhite;	// Corner Window Colour
	CWindowGc& gc = SystemGc();	 
	
	gc.SetPenStyle(CGraphicsContext::ENullPen );
    gc.SetBrushColor(KBrowserCornerWindowColour);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.DrawRect(Rect());
	}

