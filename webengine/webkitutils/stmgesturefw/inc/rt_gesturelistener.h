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
* Description: MGestureListener
*
*/

#ifndef RT_GESTURELISTENER_H_
#define RT_GESTURELISTENER_H_

#include <rt_gestureif.h>

namespace stmGesture
{
/*!
 * The gesture listener interface.  This interface needs to be implemented by the
 * listener of the standard gestures implemented in the gesture framework.
 */
class MGestureListener
{
public:
    /*!
     * The method that will be called by the gesture framework
     * to inform about a gesture being recognised.
     * \param theGesture: contains the recognised gesture.
     * The MGestureIf contains generic part and gesture specific part
     * so the listener needs to know how to handle specific gestures.
     */
    virtual void gestureEnter(MGestureIf& theGesture) = 0 ;
    /*!
     * The method to inform the listener that the gesture has ended.
     * This happens if another gesture is recognised or e.g. the Release UI
     * event is handled.
     * \note Some gestures produce only gestureEnter() callback and no gestureExit().
     */
    virtual void gestureExit(TGestureUid gestureUid) = 0 ;
    /*!
     * The listener needs to provide the target control (CCoeControl*) to the
     * standard gestures.  Since the gesture framework processes all events in the
     * application, the gesture recognisers check whether the events are targeted
     * to the listener owning control.
     * Not all gesture recognisers need to perform this check.
     * \return the owning CCoeControl of the gesture listener.
     */
    virtual CCoeControl* getOwner() = 0 ;
};

}
#endif /* RT_GESTURELISTENER_H_ */
