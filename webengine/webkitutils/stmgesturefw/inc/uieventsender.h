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
* Description:   Ui Event Sender Class
*
*/

#ifndef UIEVENTSENDER_H_
#define UIEVENTSENDER_H_

#include <e32base.h>
#include <coemain.h>
#include <rt_uievent.h>

namespace stmUiEventEngine
{

class CUiEvent;
/**
 * CUiEventSender class for handling the sending of events to the UI event listener
 */
NONSHARABLE_CLASS(CUiEventSender): public CBase
    {
public:
    enum TEventSenderState
        {
        ENoEvents,
        EEventsReady,
        EBusy
        };

    static CUiEventSender* NewL( );
    ~CUiEventSender();

    TInt AddEvent(CUiEvent* aGestureEvent);
    bool addObserver(MUiEventObserver* aObserver );
    bool removeObserver(MUiEventObserver* aObserver );
    void setLogging(bool a) {m_loggingenabled = a;} ;

private:
    CUiEventSender();
    void EmitEventL( const CUiEvent& aGestureEvent );

    /*!
     * Should we be prepared for more than 5 pointers ?  Jos tulee Torvisen voittaja
     */
    CUiEvent* iEvents[stmUiEventEngine::KMaxNumberOfPointers];
    void compressStack(CUiEvent* aUiEvent) ;

    /*
     * Let's be prepared for more than one observer,
     * although usually there is only the gesture engine
     */
    RPointerArray<MUiEventObserver> iObserver;
    bool m_loggingenabled ;

    };
}
#endif /* UIEVENTSENDER_H_ */
