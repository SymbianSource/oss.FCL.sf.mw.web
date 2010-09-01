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
* Description: Zoom Gesture Recognizer
*
*/

#ifndef ZOOMGESTURERECOGNISER_H_
#define ZOOMGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CZoomGestureRecogniser handles zoomin gesture: start from near lower left / upper right corner and drag
 * This is useful if one wants to implement single handed zooming so can be done using thumb
 * (no need to use pinch i.e. hold device in one hand and use two fingers of the other)
 * Current spec is:
 * - touch lower left corner and start dragging => initialize ZOOM IN
 * - touch upper right corner and start dragging => initialize ZOOM OUT
 * store the touch point and then calculate the distance until release
 * The distance is the zoom factor (probably needs some adjustment, not just the pixels...)
 */
NONSHARABLE_CLASS( CZoomGestureRecogniser ): public CGestureRecogniser
{
public:
	static const TGestureUid KUid = EGestureUidCornerZoom;

	/** Two-phase constructor */
	static CZoomGestureRecogniser* NewL(MGestureListener* aListener);
	virtual ~CZoomGestureRecogniser();

	/*!
	 * MGestureRecogniserIf methods
	 */
	virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
	virtual void release(MGestureEngineIf* ge) ;
	virtual TGestureUid gestureUid() const
	{
		return KUid;
	}

	/*!
	 * Additional methods to set up zoom in gesture recogniser:
	 * define the rectangle where the lower left corner is
	 * \param theArea
	 */
	void setArea(const TRect& theArea) ;

	/*!
	 * Additional methods to set up zoom in gesture recogniser:
	 * Define how close to the lower left corner the touch must happen
	 * \param rangeInPixels
	 */
	void setRange(int rangeInPixels) ;

private:
	CZoomGestureRecogniser(MGestureListener* aListener) ;

	float calculateDistance(const TPoint& aTp) ;
	int adjustZoom(float& aPreviousDistance, float aNewDistance) ;

private:
	TRect m_area ;
	int  m_rangesizeInPixels ;
	TZoomType m_zoomtype ;
	bool m_zooming ;
	TPoint m_startingtouch ;
	TPoint m_previoustouch ;
	int m_delta ;

};

} // namespace

#endif /* ZOOMGESTURERECOGNISER_H_ */
