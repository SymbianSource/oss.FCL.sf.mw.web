/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture control
*
*/


#ifndef _RT_GESTURECONTROL_H_
#define _RT_GESTURECONTROL_H_

#include <alf/alfcontrol.h>

class CAlfEnv;
class CAlfDisplay;

namespace RT_GestureHelper
{

class CGestureHelper;
class MGestureObserver;

/** 
 * Gesture control provides pointer event forwarding to gesture helper.
 * 
 * Gesture control can be created as
 *  - Stand-alone: This functionality is usable by clients that want to 
 *    capture pointer events from anywhere in the screen. For example, a view 
 *    that wants to know if user tapped anywhere on the screen could use this. 
 *  - Derived: client can use the forward-pointer-events-to-gesture-helper
 *    functionality by deriving from CGestureControl. Client has to create at 
 *    least one visual that captures pointer events. This base class will 
 *    receive the events and forward them to CGestureHelper.
 * 
 * The client can keep other widgets on top of this control, to allow them
 * priority of capturing the pointer events.
 */
class CGestureControl : public CAlfControl
    {
public:
    /** 
     * Creates a stand-alone gesture control. 
     * Creates a full screen visual that captures pointer events. Converts
     * pointer events to gestures and calls back on aObserver.
     * @param aFreeControlGroupId control group id that client does not use
     */
    IMPORT_C static CGestureControl* NewLC( MGestureObserver& aObserver,
        CAlfEnv& aEnv, CAlfDisplay& aDisplay, TInt aFreeControlGroupId );
    
    /** Destructor */
    IMPORT_C ~CGestureControl();
    
    // From CAlfControl
    IMPORT_C TBool OfferEventL( const TAlfEvent& aEvent );
    
    /** @see CGestureHelper::SetHoldingEnabled */
    IMPORT_C void SetHoldingEnabled( TBool aEnabled );
    /** @see CGestureHelper::IsHoldingEnabled */
    IMPORT_C TBool IsHoldingEnabled() const;    
    /** @see CGestureHelper::SetDoubleTapEnabled */
    IMPORT_C void SetDoubleTapEnabled( TBool aEnabled );
    /** @see CGestureHelper::IsDoubleTapEnabled */
    IMPORT_C TBool IsDoubleTapEnabled() const;
        
protected:
    /** Constructor */
    IMPORT_C CGestureControl();
    
    /**
     * Deriving class is expected to create a visual to receive pointer events.
     * This base class does not create a visual (unless creates via NewLC, i.e.,
     * in that case this class is not derived from)
     * @param aFreeControlGroupId control group id that client does not use
     */
    IMPORT_C void ConstructL( MGestureObserver& aObserver,
        CAlfEnv& aEnv, CAlfDisplay& aDisplay, TInt aFreeControlGroupId );

private:
    /// Helper that provides gestures
    CGestureHelper* iHelper;
    };

} // namespace RT_GestureHelper

#endif // _RT_GESTURECONTROL_H_
