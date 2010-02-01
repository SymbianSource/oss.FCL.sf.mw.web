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
* Description:   Timer implementation
*
*/

#ifndef CALLBACKTIMER_H_
#define CALLBACKTIMER_H_
// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coemain.h>
#include <aknutils.h>
#include <e32property.h>
#include <w32std.h>

namespace stmUiEventEngine
{
/**
 * define the method to be called by the timer; this includes the pointer number
 * the timer is associated with (maybe some other solution could do this, but...)
 */
class CStateMachine ;
typedef void (CStateMachine::*CallbackFunctionL)(TInt aPointerNumber);


/**
 * CCallbackTimer to implement the timers.  This needs to be replaced
 * with something else if cross platform is needed.
 */
NONSHARABLE_CLASS( CCallbackTimer ): public CTimer
{
public:
    /** Two-phase constructor */
    static CCallbackTimer* NewL(CStateMachine& aHelper,
            CallbackFunctionL aCallbackFunctionL, 
            TInt aDelay, 
            TInt aPointerNumber, 
            TBool aIsEnabled)
    {
        CCallbackTimer* self = new (ELeave) CCallbackTimer(aHelper,
                aCallbackFunctionL, aDelay, aPointerNumber, aIsEnabled);
        CleanupStack::PushL(self);
        self->ConstructL(); // construct base class
        CActiveScheduler::Add(self);
        CleanupStack::Pop(self);
        return self;
    }

    /** Destructor */
    ~CCallbackTimer()
    {
        Cancel();
    }

    /** Set whether sending events is currently enabled */
    void SetEnabled(TBool aEnabled)
    {
        iIsEnabled = aEnabled;
        // cancel in case timer is already running
        Cancel();
    }

    /** @return whether sending events is currently enabled */
    TBool IsEnabled() const
    {
        return iIsEnabled;
    }

    /** Start the timer. Calls callback upon completion.  It is possible to give temporary delay if needed,
     * otherwise use the defined delay.
     */
    void Start(TInt aNewDelay = 0)
    {
        if (iIsEnabled)
        {
            Cancel();
            if (aNewDelay != 0)
            {
                After(aNewDelay);
            }
            else
            {
                After(iDelay);
            }
        }
    }
    void SetDelay(TInt aDelay)
    {
        iDelay = aDelay;
    }
    TInt GetDelay()
    {
        return iDelay;
    }
    /**
     * method for checking are we inside the RunL method...
     */
    TBool isTriggered()
    {
        return iIsTriggered;
    }

private:
    /** Constructor */
    CCallbackTimer(CStateMachine& aHelper, CallbackFunctionL aCallbackFunctionL,
            TInt aDelay, TInt aPointerNumber, TBool aIsEnabled)
    :
                CTimer(EPriorityRealTime - 1), // handle the timers always first before anything else
                iHelper(aHelper), iCallbackFunctionL(aCallbackFunctionL),
                iDelay(aDelay), iIsEnabled(aIsEnabled), iPointerNumber(aPointerNumber)
    {
        iIsTriggered = EFalse;
    }

    void RunL() // From CActive
    {
        iIsTriggered = ETrue ;
        (iHelper .*iCallbackFunctionL)(iPointerNumber);
        iIsTriggered = EFalse ;
    }

private:
    /// helper object that will be called back when timer is triggered
    CStateMachine& iHelper;
    /// Function in the iHelper object call
    CallbackFunctionL iCallbackFunctionL;
    /// How long a time to wait before calling back after Start()
    TInt iDelay;
    /// whether sending holding events is currently enabled
    TBool iIsEnabled;
    /**
     * while we are inside RunL iIsTriggered = ETrue
     */
    TBool iIsTriggered;
    /*
     * in multitouch now we need timers for each pointer
     * could this be handled in some other way?
     */
    TInt iPointerNumber ;
};
} // namespace

#endif /* CALLBACKTIMER_H_ */
