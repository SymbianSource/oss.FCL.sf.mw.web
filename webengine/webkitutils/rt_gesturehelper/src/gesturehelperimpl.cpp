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


#include "gesturehelperimpl.h"

#include <alf/alfevent.h>
#include <e32base.h>
#include <w32std.h>

#include "gesture.h"
#include "gesturedefs.h"
#include "utils.h"
#include "pointercapturer.h"
#include "gestureeventfilter.h"
#include "gesturehelpereventsender.h"
#include "flogger.h"
#include "gestureevent.h"

using namespace RT_GestureHelper;

namespace RT_GestureHelper
{

/// type of function in gesture helper to be called by the timer
/// when timer triggers
typedef void (CGestureHelperImpl::*CallbackFunctionL)();

NONSHARABLE_CLASS( CCallbackTimer ) : public CTimer
    {
public:
    /** Two-phase constructor */
    static CCallbackTimer* NewL( CGestureHelperImpl& aHelper, 
            CallbackFunctionL aCallbackFunctionL, TInt aDelay, TBool aIsEnabled )
        {
        CCallbackTimer* self = new ( ELeave ) CCallbackTimer( aHelper, 
            aCallbackFunctionL, aDelay, aIsEnabled );
        CleanupStack::PushL( self );
        self->ConstructL(); // construct base class
        CActiveScheduler::Add( self );
        CleanupStack::Pop( self );
        return self;
        }
        
    /** Destructor */
    ~CCallbackTimer()
        {
        Cancel();
        }
        
    /** Set whether sending holding events is currently enabled */
    void SetEnabled( TBool aEnabled )
        {
        iIsEnabled = aEnabled;
        // cancel in case hold timer is already running
        Cancel();
        }
        
    /** @return whether sending holding events is currently enabled */
    TBool IsEnabled() const
        {
        return iIsEnabled;
        }
        
    /** Start the timer. Calls CGestureHelperImpl::StartHoldingL upon completion */
    void Start()
        {
        // if sending hold events is disabled, do not ever start the hold timer, and 
        // hence hold events will never be triggered
        if ( iIsEnabled ) 
            {
            Cancel();
            After( iDelay );
            }
        }    
    void SetDelay(TInt aDelay) { iDelay = aDelay; }
    TInt GetDelay() { return iDelay; }
    
private:    
    /** Constructor */
    CCallbackTimer( CGestureHelperImpl& aHelper,
        CallbackFunctionL aCallbackFunctionL, TInt aDelay, TBool aIsEnabled )
            : CTimer( EPriorityUserInput - 1 ), // give higher priority to new pointer events with - 1
                iHelper( aHelper ), iCallbackFunctionL( aCallbackFunctionL ), 
                    iDelay( aDelay ), iIsEnabled( aIsEnabled ) 
        {
        }
        
    void RunL() // From CActive
        {
        (iHelper.*iCallbackFunctionL)();
        }

private:
    /// helper object that will be called back when timer is triggered
    CGestureHelperImpl& iHelper;
    /// Function in the iHelper object call 
    CallbackFunctionL iCallbackFunctionL;
    /// How long a time to wait befor calling back after Start()
    TInt iDelay;
    /// whether sending holding events is currently enabled
    TBool iIsEnabled;
    };

} // namespace GestureHelper

/** 
 * @return position from event. Use this instead of using aEvent direction to
 *         avoid accidentally using TPointerEvent::iPosition
 */
inline TPoint Position( const TPointerEvent& aEvent )
    {
    // use parent position, since the capturer is using full screen area,
    // and because the (Alfred) drag events are not local to visual even when
    // coming from the client
    
    return aEvent.iParentPosition;
    }

// ----------------------------------------------------------------------------
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CGestureHelperImpl* CGestureHelperImpl::NewL( MGestureObserver& aObserver )
    {
    CGestureHelperImpl* self = new ( ELeave ) CGestureHelperImpl( aObserver );
    CleanupStack::PushL( self );
    self->iEventSender = CGestureEventSender::NewL( aObserver );
    self->iDoubleTapTimer = CCallbackTimer::NewL( *self, EmitFirstTapEventL, 
            KMaxTapDuration, EFalse ); // double tap is disabled by default
    self->iHoldingTimer = CCallbackTimer::NewL( *self, StartHoldingL, 
        KHoldDuration, EFalse ); // holding is enabled by default
    
    self->iLongTouchTimer = CCallbackTimer::NewL( *self, HandleLongTouchL, 
            KLongTapDuration, ETrue ); // holding is enabled by default
    self->iPointerCapturer = CPointerCapturer::NewL();    
    self->iGesture = new ( ELeave ) CGesture();
    self->iUnusedGesture = new ( ELeave ) CGesture();
    TInt tapLimit = Mm2Pixels(KFingerSize_mm) / 2;
    self->iEventFilter = new (ELeave) CGestureEventFilter(tapLimit);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGestureHelperImpl::CGestureHelperImpl( MGestureObserver& aObserver )
        : iObserver( aObserver )
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGestureHelperImpl::~CGestureHelperImpl()
    {
    delete iDoubleTapTimer;
    delete iHoldingTimer;
    delete iGesture;
    delete iPreviousTapGesture;
    delete iUnusedGesture;
    delete iPointerCapturer;
    delete iLongTouchTimer;
    delete iEventFilter;
    delete iEventSender;
    }
    

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::SetHoldingEnabled( TBool aEnabled )
    {
    iHoldingTimer->SetEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::IsHoldingEnabled() const
    {
    return iHoldingTimer->IsEnabled();
    }

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::SetDoubleTapEnabled( TBool aEnabled )
    {
    iDoubleTapTimer->SetEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::IsDoubleTapEnabled() const
    {
    return iDoubleTapTimer->IsEnabled();
    }
    
// ----------------------------------------------------------------------------
// InitAlfredPointerEventCaptureL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::InitAlfredPointerCaptureL( CAlfEnv& aEnv, 
        CAlfDisplay& aDisplay, TInt aFreeControlGroupId )
    {
    iPointerCapturer->InitForAlfredL(*this, aEnv, aDisplay, aFreeControlGroupId );
    }

// ----------------------------------------------------------------------------
// Reset state
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::Reset()
    {
    iHoldingTimer->Cancel();
    iLongTouchTimer->Cancel();
    iGesture->Reset();
    iPointerCapturer->Stop();
    }

/** 
 * Helper function that calls Reset on the pointer to CGestureHelperImpl
 */
static void ResetHelper( TAny* aHelper )
    {
    static_cast< CGestureHelperImpl* >( aHelper )->Reset();
    }

// ----------------------------------------------------------------------------
// Handle a pointer event
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::HandlePointerEventL( const TPointerEvent& aEvent )
    {     
    TInt filterReason;
    SetLastEventTime();
    if (!iEventFilter->FilterDrag(aEvent, iLastEventTime, filterReason))
        {
        iGesture->SetVisual( NULL );
        return noneAlf_HandlePointerEventL( aEvent );
        }
    else
        {
        /*
        TBuf<10> num;
        num.Num( filterReason );
        TBuf<128> str;
        str.AppendFormat(_L("Filter reason: %d"), filterReason);
        RFileLogger::Write( _L("gh"), _L("gh.txt"), EFileLoggingModeAppend, str);
        */
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// OfferEventL
// ----------------------------------------------------------------------------
//
TBool CGestureHelperImpl::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsPointerEvent() )
        {
        return HandlePointerEventL( aEvent.PointerEvent(), aEvent.Visual() );
        }
    return EFalse;
    }






// ----------------------------------------------------------------------------
// Handle a pointer event
// ----------------------------------------------------------------------------
//


TBool CGestureHelperImpl::noneAlf_HandlePointerEventL( const TPointerEvent& aEvent)
    {
    
    switch ( aEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
            
            iPointerCapturer->StartL();
            HandleTouchDownL(aEvent);
            break;
            }
        case TPointerEvent::EDrag:
            {
            HandleMoveL(aEvent);
            break;
            }
        case TPointerEvent::EButton1Up:
            {
            CleanupStack::PushL( TCleanupItem( &ResetHelper, this ) );
            if (KErrNone == AddPoint( aEvent ))
                {
                HandleTouchUpL(aEvent);
                }
            else
                {
                EmitCancelEventL();
                }
            CleanupStack::PopAndDestroy( this ); 
            break;
            }
        default:
            break;
        }
    return ETrue;
    }


TBool CGestureHelperImpl::HandlePointerEventL( const TPointerEvent& aEvent,
        CAlfVisual* aVisual )
    {  
    // filter out events that do not start with button down. It is a stray
    // event from another visual
    if ( IsIdle() && aEvent.iType != TPointerEvent::EButton1Down )
        {
        return EFalse; // don't consume
        }
    
    switch ( aEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            // If no up event was received during previous gesture, cancel 
            // previous event and reset state
            if ( !IsIdle() )
                {
                // ambiguous what is the right thing when "cancel" event leaves
                // and "start" does not. Leaving for cancel *after* "start" could 
                // be unexpected to client, as client would have handled start 
                // event successfully. Assume that leaving upon cancellation 
                // can be ignored.
                TRAP_IGNORE( EmitCancelEventL() );
                Reset();  
                }
            // as long as down event of a double tap comes within the double 
            // tap timeout, it does not matter how long the user keeps the finger
            // pressed for the gesture to be a double tap. Therefore, cancel
            // the timeout, as it is no longer relevant. (Of course, this call
            // will only do something if the timer is actually running, which
            // is only if received a tap event very recently.)
            iDoubleTapTimer->Cancel();
            // adding the first point implicitly makes the state "not idle"
            AddPointL( aEvent );
            iGesture->SetVisual( aVisual );
            // if pointer capturer leaves, the remaining pointer events will
            // not be captured if stylus is dragged outside the capturing visual
            // an error note will be shown, so the potential problem is irrelevant,
            // assuming client does not (incorrectly) block the leave from reaching 
            // the framework
            iPointerCapturer->StartL();
            // Delay emitting a down event _until_ it is known that this beginning 
            // gesture is _not_ the second tap of a double tap event.
            // iPreviousTapGesture is only non-null if very recently received 
            // a tap event and double tap is enabled. 
            if ( !iPreviousTapGesture )
                {
                EmitEventL( *iGesture );
                }
            // else delay emitting an event, as it might be a double tap 
            // (allow the second tap of a double tap to be anywhere, so don't check
            // for start pos here)
            break;
            
        case TPointerEvent::EDrag:
            // While stylus down, the same event is received repeatedly
            // even if stylus does not move. Filter out by checking if point 
            // is the same as the latest point
            if ( !iGesture->IsLatestPoint( Position( aEvent ) ) )
                {
                AddPointL( aEvent );

                // as long as the starting gesture is seen as a tap, do not emit any
                // drag events
                if ( !iGesture->IsTap() )
                    {
                    // if there is a previous tap gesture, getting drag events means that
                    // the previous gesture is not a double tap. So emit the previous gesture.
                    if ( iPreviousTapGesture )
                        {
                        // this is a second gesture after a tap (double tap is enabled)
                        EmitFirstTapEventL();
                        // emit down event for the current gesture (since its down was delayed, until
                        // it was to be known if the event is a tap. That is known now.)
                        EmitStartEventL( *iGesture );
                        }
                    // restart holding timer every time the current stylus pos changes
                    StartHoldingTimer( aEvent );
                    // emit the drag event to client
                    EmitEventL( *iGesture );
                    }
                // else: do not emit drag events until it is known that the gesture is not a tap
                // (or the second tap of double tap)
                }
            break;

        case TPointerEvent::EButton1Up:
            // reset in case the down event for next gesture is not received for a reason 
            // in client, and instead drag or up events are received. 
            // reset via cleanup stack to ensure Reset is run even if
            // observer leaves
            CleanupStack::PushL( TCleanupItem( &ResetHelper, this ) );
            // if adding of the point fails, notify client with a 
            // cancelled event. It would be wrong to send another
            // gesture code when the up point is not known
            if ( KErrNone == AddPoint( aEvent ) )
                {
                
                // if the gesture is a tap, the gesture is either the first tap of a _potential_
                // double tap, or the second tap of a double tap
                if ( iDoubleTapTimer->IsEnabled() && iGesture->IsTap() )
                    {
                    __ASSERT_DEBUG( !iGesture->IsHolding(), Panic( EGesturePanicIllegalLogic ) );
                    if ( !iPreviousTapGesture )
                        {
                        // First tap. Delay emitting its code evemt and released events until it is known
                        // whether the tap is a double tap
                        iPreviousTapGesture = iGesture;
                        iGesture = NewGesture();
                        iDoubleTapTimer->Start(); 
                        }
                    else
                        {
                        // This is a second tap of a double tap. Do not emit anything for the second
                        // tap. Only down event has been emitted for the first tap. Emit the code 
                        // event (double tap) and released for the first tap.
                        iPreviousTapGesture->SetDoubleTap();
                        EmitFirstTapEventL();
                        }
                    }
                
                else 
                    {
                    // modified iGesture to be "released"
                    CompleteAndEmitL( *iGesture );
                    }
                }
            else
                { // adding a point failed
                EmitCancelEventL();
                }
            // reset state
            CleanupStack::PopAndDestroy( this ); 
            break;
            
        default:
            break;
        }
    return ETrue; // consume
    }


TBool CGestureHelperImpl::IsMovementGesture(TGestureCode aCode)
    {
    return (aCode == EGestureDrag || aCode == EGestureFlick || aCode == EGestureSwipeUp ||
            aCode == EGestureSwipeDown || aCode == EGestureSwipeRight || aCode == EGestureSwipeLeft);
    }

void CGestureHelperImpl::HandleLongTouchL()
    {
    iDoubleTapTimer->Cancel();
    iGesture->SetLongTap(ETrue);
    iGesture->SetComplete();
    TPoint startPos = iGesture->StartPos();
    EmitEventL(*iGesture);
    iGesture->Reset();
    iGesture->AddPoint( startPos, GetLastEventTime() );
    }

void CGestureHelperImpl::HandleTouchDownL(const TPointerEvent& aEvent)
    {
    TGestureCode prevCode = iGesture->PreviousGestureCode();
    if (prevCode == EGestureStart) return;
    AddPointL( aEvent );
    
    iLongTouchTimer->Start();
    if (!iDoubleTapTimer->IsActive())
        {
            EmitEventL( *iGesture );
        }
    }

void CGestureHelperImpl::HandleMoveL(const TPointerEvent& aEvent)
    {
    if (iGesture->IsLatestPoint( iGesture->Visual() ? Position ( aEvent ) : aEvent.iPosition)) return; // I'm not sure we need this
    //Cancel double tap time - it's neither tap nor double tap 
    iDoubleTapTimer->Cancel();
    iLongTouchTimer->Cancel();
    
    TBool isFirstPoint = IsIdle();
    
    AddPointL( aEvent );
    
    if (iPreviousTapGesture)
        {
        RecycleGesture(iPreviousTapGesture);
        }
    
    if (!isFirstPoint)
        {
        EmitEventL( *iGesture );
        }
    }

void CGestureHelperImpl::HandleTouchUpL(const TPointerEvent& /*aEvent*/)
    {
    TGestureCode prevCode = iGesture->PreviousGestureCode();
    iLongTouchTimer->Cancel();
    iDoubleTapTimer->Cancel();
    TInt64 fromLastTouchUp = iLastEventTime.MicroSecondsFrom(iLastTouchUpTime).Int64();
    TInt64 fromLastDoubleTap = iLastEventTime.MicroSecondsFrom(iLastDoubleTapTime).Int64();
    /*
    TBuf<1024> str;
    str.AppendFormat(_L("fromLastTouchUp: %d, "), fromLastTouchUp);
    str.AppendFormat(_L("fromLastDoubleTap: %d, "), fromLastTouchUp);
    str.AppendFormat(_L("iPreviousTapGesture: %d, "), iPreviousTapGesture);
    RFileLogger::Write( _L("gh"), _L("gh.txt"), EFileLoggingModeAppend, str);
    */
    if ( prevCode == EGestureLongTap )
        {
        EmitReleasedEventL();
        }
    else if (IsMovementGesture(prevCode) || 
             !iDoubleTapTimer->IsEnabled() /* || !iGesture->IsTap()*/ ) 
        {
        iGesture->SetComplete();
        EmitEventL(*iGesture);
        }
    
    else 
        {
        if ( iPreviousTapGesture && 
         (fromLastTouchUp > KDoubleTapMinActivationInterval) &&       
         (fromLastTouchUp < KDoubleTapMaxActivationInterval) &&
         (fromLastDoubleTap > KDoubleTapIdleInterval))
            {
            // it's a double tap
            iLastTouchUpTime = iLastEventTime;
            iLastDoubleTapTime = iLastEventTime;
            EmitDoubleTapEventL();
            }
        else
            {
            // it's a first tap
            iLastTouchUpTime = iLastEventTime;
            if (iPreviousTapGesture)
                {
                   RecycleGesture(iPreviousTapGesture);
                }
                        
            iPreviousTapGesture = iGesture;
            iGesture = NewGesture();
            iDoubleTapTimer->Start(); 
            }
        }
    }



void CGestureHelperImpl::EmitDoubleTapEventL()
    {
    iPreviousTapGesture->SetDoubleTap();
    EmitFirstTapEventL();
    }


void CGestureHelperImpl::EmitReleasedEventL()
    {
    iGesture->SetComplete();
    iGesture->SetReleased();
    EmitEventL(*iGesture);
    }


// ----------------------------------------------------------------------------
// Is the helper idle?
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline TBool CGestureHelperImpl::IsIdle() const
    {
    return iGesture->IsEmpty();
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline void CGestureHelperImpl::AddPointL( const TPointerEvent& aEvent )
    {
    User::LeaveIfError( AddPoint( aEvent ) );
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// inline ok in cpp file for a private member function
// ----------------------------------------------------------------------------
//
inline TInt CGestureHelperImpl::AddPoint( const TPointerEvent& aEvent )
    {
    TPoint pos = iGesture->Visual() ? Position ( aEvent ) : aEvent.iPosition;
    return iGesture->AddPoint( pos, GetLastEventTime() );
    }

// ----------------------------------------------------------------------------
// StartHoldingTimer
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::StartHoldingTimer( const TPointerEvent& aNewEvent )
    {
    if ( !( iGesture->IsHolding() ||
            iGesture->IsNearHoldingPoint( Position( aNewEvent ) ) ) )
        {
        // restart hold timer, since pointer has moved
        iHoldingTimer->Start();
        // Remember the point in which holding was started
        iGesture->SetHoldingPoint();
        }
    }

/** 
 * Helper function that calls ContinueHolding on the pointer to TGesture
 */
static void ContinueHolding( TAny* aGesture )
    {
    static_cast< CGesture* >( aGesture )->ContinueHolding();
    }

// ----------------------------------------------------------------------------
// Add a point to the sequence of points that together make up the gesture
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::StartHoldingL()
    {
    // hold & tap event is specifically filtered out. Use case: in list fast 
    // scrolling activation (e.g. enhanced coverflow), tap & hold should not
    // start fast scroll. In addition, after long tap on start position,
    // drag and drag & hold swiping should emit normal swipe and swipe&hold
    // events. Therefore, tap & hold is not supported.
    __ASSERT_DEBUG( !iGesture->IsTap() && !iPreviousTapGesture, Panic( EGesturePanicIllegalLogic ) );
    
    // holding has just started, and gesture code should be provided to client.
    // set gesture state so that it produces a gesture code (other than drag)
    iGesture->StartHolding();
    
    // create an item in the cleanup stack that will set the gesture state
    // to holding-was-started-earlier state. NotifyL may leave, but the
    // holding-was-started-earlier state must still be successfully set,
    // otherwise, the holding gesture code will be sent twice
    CleanupStack::PushL( TCleanupItem( &ContinueHolding, iGesture ) );
    
    EmitEventL( *iGesture );
    
    // set holding state to "post holding"
    CleanupStack::PopAndDestroy( iGesture );
    }

// ----------------------------------------------------------------------------
// RecyclePreviousTapGesture
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::RecyclePreviousTapGesture( TAny* aSelf )
    {
    CGestureHelperImpl& self = *reinterpret_cast<CGestureHelperImpl*>( aSelf );
    self.RecycleGesture( self.iPreviousTapGesture );
    }

// ----------------------------------------------------------------------------
// Emit the remainder of the previous tap event (tap + released)
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitFirstTapEventL()
    {
    // when this function is called, a tap has turned out to _not_ be a double tap
    __ASSERT_DEBUG( IsDoubleTapEnabled(), Panic( EGesturePanicIllegalLogic ) );
    __ASSERT_DEBUG( iPreviousTapGesture, Panic( EGesturePanicIllegalLogic ) );
    
    iDoubleTapTimer->Cancel();
    
    // ensure previous tap gesture is reset even if client leaves
    CleanupStack::PushL( TCleanupItem( &RecyclePreviousTapGesture, this ) );
    
    CompleteAndEmitL( *iPreviousTapGesture );
    
    // recycle the emitted gesture 
    CleanupStack::PopAndDestroy( this ); 
    }

// ----------------------------------------------------------------------------
// EmitStartEventL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitStartEventL( const CGesture& aGesture )    
    {
    CGesture* startGesture = aGesture.AsStartEventLC();
    EmitEventL( *startGesture );
    CleanupStack::PopAndDestroy( startGesture );    
    }
    
// ----------------------------------------------------------------------------
// EmitCompletionEventsL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::CompleteAndEmitL( CGesture& aGesture )
    {
    aGesture.SetComplete();
    // send gesture code if holding has not been started. If holding has 
    // been started, client has already received a "hold swipe left" e.g. event, in which
    // case don't another "swipe left" event
    if ( !aGesture.IsHolding() )
        {
        // if client leaves, the state is automatically reset.
        // In this case the client will not get the released event
        EmitEventL( aGesture ); 
        }
    
    // send an event that stylus was lifted
    aGesture.SetReleased();
    EmitEventL( aGesture ); 
    }
    
// ----------------------------------------------------------------------------
// EmitCancelEventL
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitCancelEventL()
    {
    iDoubleTapTimer->Cancel();

    // ensure previous tap gesture is reset even if client leaves
    CleanupStack::PushL( TCleanupItem( &RecyclePreviousTapGesture, this ) );

    CGesture& gestureToCancel = iPreviousTapGesture ? *iPreviousTapGesture : *iGesture;
    gestureToCancel.SetCancelled();
    EmitEventL( gestureToCancel );
    
    // recycle the emitted gesture 
    CleanupStack::PopAndDestroy( this ); 
    }

// ----------------------------------------------------------------------------
// Notify observer
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::EmitEventL( const CGesture& aGesture )
    {
    // deallocation of the event is happening in CGestureEventSender::RunL() 
    CGestureEvent* event = new(ELeave) CGestureEvent();
    event->iCode = const_cast<CGesture&>(aGesture).Code(MGestureEvent::EAxisBoth);
    event->iCurrPos = aGesture.CurrentPos();
    event->iDistance = aGesture.Distance();
    event->iStartPos = aGesture.StartPos();
    event->iIsHolding = aGesture.IsHolding();
    event->iSpeed = aGesture.Speed();
    event->iVisual = aGesture.Visual();
    iEventSender->AddEvent(event);
    }

// ----------------------------------------------------------------------------
// Return a fresh gesture from the gesture pool (pool of one gesture)
// ----------------------------------------------------------------------------
//
CGesture* CGestureHelperImpl::NewGesture()
    {
    __ASSERT_DEBUG( iUnusedGesture, Panic( EGesturePanicIllegalLogic ) ); // pool should no be empty
    
    iUnusedGesture->Reset();
    CGesture* freshGesture = iUnusedGesture;
    iUnusedGesture = NULL;
    return freshGesture;
    }

// ----------------------------------------------------------------------------
// Return a fresh gesture from the gesture pool (pool of one gesture)
// ----------------------------------------------------------------------------
//
void CGestureHelperImpl::RecycleGesture( CGesture*& aGesturePointer )
    {
    // only one object fits into the pool, and that should currently be enough
    // one pointer must be null, one non-null
    __ASSERT_DEBUG( !iUnusedGesture != !aGesturePointer, Panic( EGesturePanicIllegalLogic ) );
    if ( aGesturePointer )
        {
        iUnusedGesture = aGesturePointer;
        aGesturePointer = NULL;
        }
    }
