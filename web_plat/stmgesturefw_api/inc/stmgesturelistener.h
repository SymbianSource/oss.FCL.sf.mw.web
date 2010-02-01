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

#ifndef __STMGESTURELISTENER_H__
#define __STMGESTURELISTENER_H__

#include <e32base.h>
#include <rt_gestureif.h>


typedef stmGesture::MGestureIf MStmGesture;
typedef stmGesture::TGestureUid TStmGestureUid;
typedef stmGesture::TTapType TStmTapType;
typedef stmGesture::TGestureDirection TStmGestureDirection;

/**
 * Point of float accuracy
 */
NONSHARABLE_STRUCT( TRealPoint )
    {
    inline TRealPoint();
    inline TRealPoint( const TRealPoint& aPoint );
    inline TRealPoint( TReal aX, TReal aY );
    inline TBool operator==( const TRealPoint& aPoint ) const;

    TReal32 iX;
    TReal32 iY;
    };

/**
 * Enum to distinguish between Gesture Entry and Exit. 
 * Currently supported only for Pinch
 */
enum TStmGestureState {
    EGestureEnter     = 0,
    EGestureExit

};

/**
 * Interface Class to the Application
 */

class TStmGestureEvent
{
public:    
    inline TStmGestureUid Code() const                      { return iGestureUid; };
    inline TPoint CurrentPos() const                        { return iCurrPos; }; 
    inline TRealPoint Speed() const                         { return iSpeed; }; 
    inline TStmTapType Type() const                         { return iType; }; 
    inline int Details() const                              { return iDetails; };
    inline TStmGestureDirection GestureDirection() const    { return iDirection; }; 
    inline TPoint PinchEndPos() const                       { return iPinchEndPos; };
    inline TStmGestureState GestureState() const            { return iGestureState; };
            
    inline void SetCode(TStmGestureUid aGestureUid)         { iGestureUid = aGestureUid; };
    inline void SetCurrentPos(TPoint aPos)                  { iCurrPos = aPos; };
    inline void SetSpeed(TRealPoint aSpeed)                 { iSpeed = aSpeed; }; 
    inline void SetType(TStmTapType aType)                  { iType = aType; }; 
    inline void SetDetails(int aDetails)                    { iDetails = aDetails; }; 
    inline void SetGestureDirection(TStmGestureDirection aDirection)  { iDirection = aDirection; }; 
    inline void SetPinchEndPos(TPoint aPinchEndPos)         { iPinchEndPos = aPinchEndPos; }; 
    inline void SetGestureState(TStmGestureState aGestureState)       { iGestureState = aGestureState; }; 
    
private:    
    TStmGestureUid              iGestureUid;   // Code of the gesture 
    TPoint                      iCurrPos;      // Current position where gesture occured
    TRealPoint                  iSpeed;        // speed X and speed Y
    TStmTapType                 iType;         // single-tap or double-tap
    int                         iDetails;      // details like pinch factor
    TStmGestureDirection        iDirection;    // Gesture Direction  
    TPoint                      iPinchEndPos;  // End of pinching position
    TStmGestureState            iGestureState; // Gesture State. currently supports only for pinch
    
};

class MStmGestureListener
    {
public:
    /**
     * Handle the gesture event
     * @param aEvent event describing the gesture 
     */
    virtual void HandleGestureEventL(const TStmGestureEvent& aGesture) = 0;
    };

// ----------------------------------------------------------------------------
// Default constructor for real point
// ----------------------------------------------------------------------------
//
inline TRealPoint::TRealPoint()
        : iX( 0 ), iY( 0 )
    {    
    }

// ----------------------------------------------------------------------------
// Copy constructor for real point
// ----------------------------------------------------------------------------
//
inline TRealPoint::TRealPoint( const TRealPoint& aPoint )
        : iX( aPoint.iX ), iY( aPoint.iY )
    {    
    }

// ----------------------------------------------------------------------------
// Copy constructor for real point
// ----------------------------------------------------------------------------
//
inline TRealPoint::TRealPoint( TReal aX, TReal aY )
        : iX( aX ), iY( aY )
    {    
    }

// ----------------------------------------------------------------------------
// Default constructor for real point
// ----------------------------------------------------------------------------
//
inline TBool TRealPoint::operator==( const TRealPoint& aPoint ) const
    {    
    return iX == aPoint.iX && iY == aPoint.iY;
    }


#endif  // __ICS_GESTURELISTENER_H__
