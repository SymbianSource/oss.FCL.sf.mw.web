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

#include "uieventsender.h"
#include "UiEvent.h"

#include "filelogger.h"

using namespace stmUiEventEngine;


CUiEventSender* CUiEventSender::NewL()
{
    CUiEventSender* self = new (ELeave) CUiEventSender();

    return self;
}

/*!
 * An active object for sending the UI events to the observers.
 * Depending on the m_directsending the asynchronous method is not
 * used but instead the observers will be called immediately.
 */
CUiEventSender::CUiEventSender()
{
    m_loggingenabled = false ;
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        iEvents[i] = NULL ;
    }
}

CUiEventSender::~CUiEventSender()
{
    // remove the possible events from the buffers if release was missed
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        if (iEvents[i]) delete iEvents[i] ;
    }
    iObserver.Reset() ;
}

/*!
 * Add new UI event to the list or send it directly to the observers
 * depending on the m_directsending flag.
 * \param aUiEvent the new UI event to be sent to the observers.
 */
TInt CUiEventSender::AddEvent(CUiEvent* aUiEvent)
{
    int pointerIndex = aUiEvent->Index() ;
    // Store the new UI event.  Check what kind of event it is and compress the set of events
    // stored so far if possible
    compressStack(aUiEvent) ;
    aUiEvent->setPrevious(iEvents[pointerIndex]) ;
    iEvents[pointerIndex] = aUiEvent ;  // Store the new event
    TRAPD(err, EmitEventL(*aUiEvent));
    if(err)
        return err;

    if (m_loggingenabled)
    {
        LOGARG("Sent event: %s: (ptr %d) (%d,%d)",
                stmUiEventEngine::EventName(aUiEvent->Code()), pointerIndex,
                aUiEvent->CurrentXY().iX, aUiEvent->CurrentXY().iY);
    }
    // If this was release event, then the chain can be removed
    if (aUiEvent->Code() == stmUiEventEngine::ERelease)
    {
        delete aUiEvent;    // This will delete the whole chain
        iEvents[pointerIndex] = NULL ;
    }
    return KErrNone;
}
/*!
 * Call each observer with the event
 */
void CUiEventSender::EmitEventL(const CUiEvent& aEvent)
{
    for (TInt i = 0; i < iObserver.Count(); i++)
    {
        iObserver[i]->HandleUiEventL(aEvent);
    }
}

/*!
 * Add a new observer.  Note that current implementation is very rude:
 * max 5 observers in a simple array.
 */
bool CUiEventSender::addObserver(MUiEventObserver* aObserver )
{
    iObserver.Append(aObserver) ;
    return true ;
}

/*
 * remove observer from list
 */
bool CUiEventSender::removeObserver(MUiEventObserver* aObserver )
{
    int x = iObserver.Find(aObserver) ;
    if (x != -1)
    {
        iObserver.Remove(x) ;
        return true ;

    }
    return EFalse ; // Could not find observer
}
void CUiEventSender::compressStack(CUiEvent* aUiEvent)
{
    int pointerIndex = aUiEvent->Index() ;
    CUiEvent*& top = iEvents[pointerIndex] ;
    if(!top)
    {
        return;
    }
    if (aUiEvent->Code() == stmUiEventEngine::EHold)
    {
        // assumption: in case of hold, we can discard all previous messages
        delete top ;
        top = NULL ;
    }
    else
    {
        // Check if there would too many moves
        CUiEvent* next = dynamic_cast<CUiEvent*>(top->previousEvent()) ;
        if (next != 0 && next->Code() == stmUiEventEngine::EMove)
        {
            // leave only the topmost to the stack
            top->setPrevious(0) ;
            delete next ;
        }
    }

}
