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

#ifndef GESTUREHELPEREVENTSENDER_H_
#define GESTUREHELPEREVENTSENDER_H_

#include <e32base.h>
#include <coemain.h>
#include "stmgestureinterface.h"

NONSHARABLE_CLASS(CGestureEventSender) : public CBase
    {
public:

    static CGestureEventSender* NewL(  MStmGestureListener& aObserver );
    ~CGestureEventSender();
    void AddEvent(TStmGestureUid aUid, const MStmGesture* aGestureEvent);

private:
    CGestureEventSender(MStmGestureListener& aObserver);
    void EmitEventL( const TStmGestureEvent& aGestureEvent );
    
private:    
    MStmGestureListener& iObserver;
    };

#endif /* GESTUREHELPEREVENTSENDER_H_ */
