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
* Description:  Gesture Definitions
*
*/

#ifndef GENERICSIMPLEGESTURE_H_
#define GENERICSIMPLEGESTURE_H_
#include <rt_gestureif.h>
#include <rt_uievent.h>


namespace stmGesture
{

/*!
 * Class for creating a simple gesture for notifying the listener.
 * If the basic features are enough for the gesture, then this class
 * is enough to be instantiated; there is the location and speed
 * determined from the UI event; also type can be specified as well
 * as integer detail data.
 */
NONSHARABLE_CLASS( TGenericSimpleGesture ): public MGestureIf
{
public:
    TGenericSimpleGesture(
        TGestureUid uid, const TPoint& loc, int type = 0,
        const stmUiEventEngine::MUiEventSpeed* speedIf = NULL);

public: // implementation of MGestureIf

    TGestureUid gestureUid() const { return m_uid; }
    void setDetails(int aDetails) { m_details = aDetails;}  // additional info can be passed here.
    /*!
     * MGestureIf methods
     */
    virtual TPoint getLocation() const  ;  // Location where the gesture happened (if applicable)
    virtual int getType() const ;         // If the gesture can have different types
    virtual float getSpeedX() const __SOFTFP
    {
        return m_speed ? m_speed->speedX() : 0.f;
    }
    virtual float getSpeedY() const __SOFTFP
    {
        return m_speed ? m_speed->speedY() : 0.f;
    }
    virtual int getDetails() const  ;    // Other possible details....

    virtual void setName(const TDesC8& aName) ;
    virtual TPtrC8 getGestureName() ;       // String name for gesture

public:
    void setType(int aType)
        {
        m_type = aType;
        }

protected:
    TPoint m_location ;
    TGestureUid m_uid;
    int m_type ;
    // What would be the other details?
    int m_details ;
    const stmUiEventEngine::MUiEventSpeed* m_speed ;
    TPtrC8 m_name ;
};

NONSHARABLE_CLASS( TDirectionalGesture ): public TGenericSimpleGesture
{
public:
    TDirectionalGesture(
        TGestureUid uid, const TPoint& loc, const TPoint& prevLoc,
        const stmUiEventEngine::MUiEventSpeed* speedIf = NULL, bool logging = false);

    void setVector(const TPoint& last, const TPoint& previous) ;
    void setLogging(bool enabled) { m_loggingEnabled = enabled; }

    TPoint getLengthAndDirection() const {return m_vector;}
    TGestureDirection getDirection() const ;
    TInt getLength() const
    {
        return ((m_vector.iX*m_vector.iX)+(m_vector.iY*m_vector.iY)) ;
    }

protected:
    TPoint m_vector ;
    bool m_loggingEnabled ;
};

NONSHARABLE_CLASS( TTwoPointGesture ): public TDirectionalGesture
{
public:
    TTwoPointGesture(TGestureUid uid, const TPoint& pos1, const TPoint& pos2);
    TPoint getPinchEndPos() const { return m_location2; }

private:
    TPoint m_location2 ;
};

} // namespace

#endif /* GENERICSIMPLEGESTURE_H_ */
