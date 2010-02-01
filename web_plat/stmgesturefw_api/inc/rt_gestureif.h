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
* Description:  Gesture helper implementation
*
*/

#ifndef RT_GESTUREIF_H_
#define RT_GESTUREIF_H_
// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coemain.h>
#include <aknutils.h>
#include <e32property.h>
#include <w32std.h>

namespace stmGesture
{

/**
 * UIDs for supported Gestures 
 */
enum TGestureUid
{
    EGestureUidUnknown     = 0,
    EGestureUidTouch,
    EGestureUidTap,
    EGestureUidDoubleTap   = EGestureUidTap,
    EGestureUidLongPress,
    EGestureUidHover,
    EGestureUidPan,
    EGestureUidRelease,
    EGestureUidLeftRight,
    EGestureUidUpDown,
    EGestureUidFlick,
    EGestureUidEdgeScroll,
    EGestureUidPinch,
    EGestureUidCornerZoom,
    //
    EStmGestureUid_Count
};

/**
 * Area Settings
 */
enum TGestureAreaSettings
{
    ETouchTimeArea,
    ETouchArea,
    EHoldArea,
    //
    EGestureAreaSettingsCount
};

/**
 * Gesture Specific Settings
 */
enum TGestureSettings
{
    //ETouchTimeout,
    EDoubleTapTimeout,
    ESuppressTimeout,
    EMoveSuppressTimeout,
    EHoverSpeed,
    EPanSpeedLow,
    EPanSpeedHigh,
    EFlickSpeed = EPanSpeedHigh, 
    EPinchSpeed,
    EZoomCornerSize,
    EEdgeScrollRange,
    ECapacitiveUpUsed,
    EPanDisabledWhileHovering,
    EHoverDisabledWhilePanning,
    EAdjustYPos,
    EEnableFiltering,
    EWServMessageInterception, 
    //
    EGestureSettingsCount
};

struct TGestureArea
{
public:
    enum TShape
    {
        /// Use GestureArea settings from globally defined default
        EDefaultSettings = -1,
        ERectangle  = 1, // == stmUiEventEngine::ERectangle,
        ECircle     = 2, // == stmUiEventEngine::ECircle,
        EEllipse    = 3  // == stmUiEventEngine::EEllipse
    };

    TShape iShape;
    TInt iTimeout;
    // At the moment iWidth is only used
    TSize iSize;
};

/**
 * Edge Scroll Type
 */
enum TEdgeScroll
{
    EEdgeScrollUp = 1,
    EEdgeScrollDown,
    EEdgeScrollLeft,
    EEdgeScrollRight,
    EEdgeScrollUnknown
};

/**
 * Tap Type
 */
enum TTapType
{
    ETapTypeSingle = 1,
    ETapTypeDouble
};

/**
 * Zoom Type
 */
enum TZoomType
{
    EZoomUndefined  = 0,
    EZoomIn         = 1,
    EZoomOut        = 2
};

/**
 * Direction Definition
 */
enum TGestureDirection
{
    ENoDirection = 0,
    //
    ENorth = 1,
    ENorthEast,
    EEast,
    ESouthEast,
    ESouth,
    ESouthWest,
    EWest,
    ENorthWest,
};

/*!
 * Generic gesture interface.  The gestures
 * contain a generic part and a gesture specific part
 * is obtained using the getDetails() method.
 *
 */
class MGestureIf
{
public:
    virtual TGestureUid gestureUid() const = 0; // Identifier of recognizer type that issues the gesture
    virtual TPoint getLocation() const = 0 ;    // Location where the gesture happened (if applicable)
    virtual int getType() const = 0 ;           // If the gesture can have different types (like tap or double tap)
    virtual float getSpeedX() const __SOFTFP = 0 ;  // px/msec, as calculated in the UI state machine, not valid in all cases
    virtual float getSpeedY() const __SOFTFP = 0 ;  // px/msec, as calculated in the UI state machine, not valid in all cases
    virtual int getDetails() const = 0 ;      // Other possible details....
	virtual TPtrC8 getGestureName() = 0 ;	    // String name for gesture, makes possible to use single callback for gestures
    virtual TPoint getLengthAndDirection() const { return TPoint(0,0); }  // very simple vector
    virtual TGestureDirection getDirection() const { return stmGesture::ENoDirection; }
    virtual TPoint getPinchEndPos() const { return TPoint(0,0); }
};

} // namespace

#endif /* RT_GESTUREIF_H_ */
