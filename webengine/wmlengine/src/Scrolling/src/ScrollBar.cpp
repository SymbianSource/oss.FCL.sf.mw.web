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
* Description:  Implementation of the CScrollBar class.
*
*/

#include "ScrollBar.h"
#include "ScrollThumb.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
// CScrollBar* CScrollBar::NewL(const TOrientation aOrientation, const CCoeControl& aParent)
//
// Two-phased constructor
//----------------------------------------------------------------------------------------
//
CScrollBar* CScrollBar::NewL(const TOrientation aOrientation, const CCoeControl& aParent)
	{
	CScrollBar* self = new (ELeave) CScrollBar(aOrientation);
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	CleanupStack::Pop(self);   // self
	return self;
	}


//----------------------------------------------------------------------------------------
// CScrollBar::CScrollBar(const TOrientation aOrientation)
//
// First Phase Construction
//----------------------------------------------------------------------------------------
//
CScrollBar::CScrollBar(const TOrientation aOrientation)
	{
	const TRgb KTrackColour = 0xC0C0C0;	// light grey
	const TRgb KTrackBkgdColour = KRgbWhite;

	SetOrientation(aOrientation);
	SetTrackColours(KTrackColour, KTrackBkgdColour);
	}


//----------------------------------------------------------------------------------------
// CScrollBar::ConstructL(const CCoeControl& aParent)
// 
// EPOC Second Phase Constructor
//----------------------------------------------------------------------------------------
//
void CScrollBar::ConstructL(const CCoeControl& aParent)
	{
	SetContainerWindowL(aParent);
	
	SetRect(TRect(0, 0, 0, 0));

	iThumb = CScrollThumb::NewL(iOrientation);

	iThumb->SetContainerWindowL(*this);
	ActivateL();
	}


//--------------------------------------------------
// CScrollBar::~CScrollBar()
// 
// DESTRUCTOR
//--------------------------------------------------
//
CScrollBar::~CScrollBar()
	{
	delete iThumb;
	}


//-------------------------------------------------------
// void CScrollBar::Draw( const TRect& /*aRect*/ ) const
// 
// Draws a scroll bar
//-------------------------------------------------------
//
void CScrollBar::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();	 

	// Draw Track Background - it always redraws the rectangle.
	gc.SetPenStyle( CGraphicsContext::ESolidPen );
	gc.SetPenColor( iTrackBkgdColour );
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetBrushColor( iTrackBkgdColour );
	gc.DrawRect( Rect() );

	// Draw Track - iTrackCentre can be 'null' rectangle 
    // (e.g. until SetTrackAndBackground is not called), 
    // so in that case the following will not draw anything.
	gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( iTrackColour );
	gc.DrawRect(iTrackCentre);
	}


// ---------------------------------------------------------
// TInt CScrollBar::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CScrollBar::CountComponentControls() const
    {
	return 1;
    }



// ---------------------------------------------------------
// void CScrollBar::SizeChanged() const
// ---------------------------------------------------------
//
void CScrollBar::SizeChanged() 
    {
    }


// --------------------------------------------------------------
// CCoeControl* CScrollBar::ComponentControl(TInt aIndex) const
// --------------------------------------------------------------
//
CCoeControl* CScrollBar::ComponentControl(TInt aIndex) const
    {
	switch (aIndex)
		{
		case 0:
			return iThumb;

		default:
			return NULL;
		}
    //lint -e{1763} Error -- (Info -- Member function 'CBrCtl::ComponentControl(int) const' marked as const indirectly modifies class)
    }

//------------------------------------------------------------------------
// void CScrollBar::SetTrackColours(const TRgb aTrackColour,
//                                  const TRgb aTrackBkgdColour)
// Sets colours for scroll bar
//------------------------------------------------------------------------
//
void CScrollBar::SetTrackColours(const TRgb aTrackColour, 
								 const TRgb aTrackBkgdColour)
	{
	iTrackColour = aTrackColour;
	iTrackBkgdColour = aTrackBkgdColour;
	}

 
//------------------------------------------------------------------------
// void CScrollBar::SetThumb(TRect& aRect)
// 
// Sets the rect of a scrollbar thumb
//------------------------------------------------------------------------
//
void CScrollBar::SetThumb(const TRect& aRect)
	{
	iThumb->SetRect(aRect);
	}


//------------------------------------------------------------------------
// void CScrollBar::SetTrackAndBackground(const TRect& aRect)
//------------------------------------------------------------------------
//
void CScrollBar::SetTrackAndBackground(const TRect& aRect)
	{
	SetRect(aRect);

	// Fit into background
	iTrackCentre = aRect;
	iTrackCentre.Shrink(KTrackBkgdThickness, KTrackBkgdThickness);
	}

// End of File
