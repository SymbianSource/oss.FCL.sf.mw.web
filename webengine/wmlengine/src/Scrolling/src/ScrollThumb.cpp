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
* Description:  Implementation of the CScrollThumb class.
*
*/

#include "ScrollThumb.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// CScrollThumb::NewL(CScrollBar::TOrientation aOrientation)
//
// Two-phased construction
//-----------------------------------------------------------------------------
//
CScrollThumb* CScrollThumb::NewL(CScrollBar::TOrientation aOrientation)
	{
	CScrollThumb* self = new (ELeave) CScrollThumb(aOrientation);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
	}


//-----------------------------------------------------------------------------
// CScrollThumb::CScrollThumb(CScrollBar::TOrientation aOrientation)
//
// First Phase Construction
//-----------------------------------------------------------------------------
//
CScrollThumb::CScrollThumb(CScrollBar::TOrientation aOrientation)
	{
	const TRgb KThumbColour = 0x808080;	// dark grey
	const TRgb KThumbShadowColour = KRgbWhite;

	iOrientation = aOrientation;
	
	SetRect(TRect(0,0,0,0));

	SetThumbColours(KThumbColour, KThumbShadowColour);
	}


//-----------------------------------------------------------------------------
// CScrollThumb::ConstructL()
// 
// EPOC Second Phase Constructor
//-----------------------------------------------------------------------------
//
void CScrollThumb::ConstructL( )
	{
	}	

//-----------------------------------------------------------------------------
// CScrollThumb::~CScrollThumb()
// 
// DESTRUCTOR
//-----------------------------------------------------------------------------
//
CScrollThumb::~CScrollThumb()
	{
	}


//-----------------------------------------------------------------------------
// void CScrollThumb::Draw(const TRect& /*aRect*/) const
// 
// Draws the thumb with a shadow
//-----------------------------------------------------------------------------
//
void CScrollThumb::Draw(const TRect& /*aRect*/) const
	{
	CWindowGc& gc = SystemGc();

	// Thumb Shadow
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(iThumbShadowColour);
	gc.DrawRect(iThumbShadow);

	// Thumb
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushColor(iThumbColour);
	gc.DrawRect(iThumbCentre);
	}


//-----------------------------------------------------------------------------
// void CScrollThumb::SizeChanged() 
//-----------------------------------------------------------------------------
//
void CScrollThumb::SizeChanged() 
    {
    }


//-----------------------------------------------------------------------------
// void CScrollThumb::SetThumbColours(TRgb aThumbColour, TRgb aThumbShadowColour)
// 
// Sets colours for thumb
//-----------------------------------------------------------------------------
//
void CScrollThumb::SetThumbColours(TRgb aThumbColour, TRgb aThumbShadowColour)
	{
	iThumbColour = aThumbColour;
	iThumbShadowColour = aThumbShadowColour;
	}

//-----------------------------------------------------------------------------
// void CScrollBarThumb::SetThumbAndShadow(const TRect& aRect)
// 
// Sets the rect of the thumb and its shadow
//-----------------------------------------------------------------------------
//
void CScrollThumb::SetThumbAndShadow(const TRect& aRect)
	{
	SetRect(aRect);

	iThumbCentre = iThumbShadow = aRect;
	iThumbCentre.Shrink(KTrackBkgdThickness, KTrackBkgdThickness);

	// Shadow shown only at its ends
	if (iOrientation == CScrollBar::EVertical)
		{
		iThumbShadow.Shrink(KTrackBkgdThickness, 0);		
		}
	else
		{
		iThumbShadow.Shrink(0, KTrackBkgdThickness);
		}
	}
