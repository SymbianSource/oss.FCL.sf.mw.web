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


// Gesture UI Engine
#include "statemachine.h"

// Gesture Library: Framework
#include "gestureframework.h"
#include "rt_gestureengineif.h"

// Gesture Library: Recognizers
#include "tapgesturerecogniser.h"
#include "pangesturerecogniser.h"
#include "edgescrollgesturerecogniser.h"
#include "longpressgesturerecogniser.h"
#include "pinchgesturerecogniser.h"
#include "touchgesturerecogniser.h"
#include "releasegesturerecogniser.h"
#include "flickgesturerecogniser.h"
#include "zoomgesturerecogniser.h"

#include "stmgestureinterface.h"
#include "gesturehelpereventsender.h"

#ifndef ICS_DCHECK
#define ICS_DCHECK(test)
#endif

#define LOG(args...)


EXPORT_C CStmGestureParameters::CStmGestureParameters()
    {

    }

EXPORT_C CStmGestureParameters::CStmGestureParameters(
    const CStmGestureParameters& aParams )
    {
    Mem::Copy(&iParam[0], &aParams.iParam[0], sizeof(iParam));
    Mem::Copy(&iAreaSettings[0], &aParams.iAreaSettings[0], sizeof(iAreaSettings));
    Mem::Copy(&iEnabled[0], &aParams.iEnabled[0], sizeof(iEnabled));
    }

EXPORT_C CStmGestureEngine* CStmGestureEngine::NewL()
    {
    CStmGestureEngine* eng = new(ELeave) CStmGestureEngine();
    CleanupStack::PushL(eng);
    eng->ConstructL();
    CleanupStack::Pop(eng);
    return eng;
    }

CStmGestureEngine::CStmGestureEngine()
    {

    }

void CStmGestureEngine::ConstructL()
    {
    iGf = stmGesture::CGestureFramework::NewL();
    }

CStmGestureEngine::~CStmGestureEngine()
    {
    // Prevent reactivation of contexts as they remove themselves
    iDtorPhase = ETrue;

    iContexts.ResetAndDestroy();
    iCtxtStack.Reset();
    delete iGf;
    }

EXPORT_C CStmGestureContext* CStmGestureEngine::CreateContextL(
    TInt /*aId*/ )
    {
    CStmGestureContext* ctxt = new(ELeave) CStmGestureContext(*this);
    CleanupStack::PushL(ctxt);
    ctxt->ConstructL();
    CleanupStack::Pop(ctxt);
    return ctxt;
    }

EXPORT_C CStmGestureContext* CStmGestureEngine::Context(
    TInt /*aId*/ )
    {
    return NULL;
    }

void CStmGestureEngine::EnableContextL(
    CStmGestureContext& aContext )
    {
    // NB: enabling context puts it on top of the Context Stack
    TInt idx = iCtxtStack.Find(&aContext);
    if(idx >= 0)
        {
        iCtxtStack.Remove(idx);
        }

    SetupRecognizersL(aContext, ETrue);

    iCtxtStack.AppendL(&aContext);
    }

void CStmGestureEngine::DisableContextL(
    CStmGestureContext& aContext )
    {
    TInt idx = iCtxtStack.Find(&aContext);
    ICS_DCHECK(idx >= 0);
    if(idx == KErrNotFound)
        {
        return;
        }

    iCtxtStack.Remove(idx);

    stmGesture::MGestureEngineIf* ge = iGf->getGestureEngine();

    for(TInt i = 0; i < stmGesture::EStmGestureUid_Count; ++i)
        {
        MStmGestureRecogniser* rec = aContext.iRecognizers[i];
        if(rec)
            {
            ge->removeGesture(rec);
            }
        }

    // re-activate previous (new top-most) context
    // NB: if deleted context is not top-most (active) one, no updates done
    TInt count = iCtxtStack.Count();
    if(idx == count && count > 0 && !iDtorPhase)
        {
        CStmGestureContext* previous = iCtxtStack[count - 1];
        SetupRecognizersL(*previous, EFalse);
        }
    }

template<class T>
void CStmGestureEngine::InitRecognizerL(
    T*& aGesture,
    CStmGestureContext& aContext,
    TBool aNewContext )
    {
    stmGesture::MGestureRecogniserIf*& recognizer = aContext.iRecognizers[T::KUid];
    aGesture = static_cast<T*>(recognizer);
    if(!aGesture && aNewContext)
        {
        recognizer = aGesture = T::NewL(&aContext);
        recognizer->enableLogging((aContext.iLogging & (1<<T::KUid)) != 0);
        }
    else
        {
        ICS_DCHECK(gesture && !aNewContext); // should exist in restored top context
        }

    // TODO: Add in right place according to gesture class
    TBool added = iGf->getGestureEngine()->addGesture(aGesture);
    if(!added)
        {
        User::Leave(KErrNoMemory);
        }
    }

void CStmGestureEngine::SetupRecognizersL(
    CStmGestureContext& aContext,
    TBool aNewContext )
    {
    LOG("Setup recognizers");
    // Order of recognizers in the Gesture Engine (upper ones receive input before lower ones)
    //
    // PINCH
    // CORNER_ZOOM
    // EDGE_SCROLL
    // LONGPRESS
    // TOUCH
    // UP_DOWN
    // LEFT_RIGHT
    // HOVER
    // PAN
    // TAP / DOUBLE_TAP
    // FLICK
    // RELEASE
    // UNKNOWN

    stmUiEventEngine::CStateMachine* stateMachine = iGf->getUiStateMachine();
    stmGesture::MGestureEngineIf* gestureEngine = iGf->getGestureEngine();

    MStmGestureParameters& conf = aContext.Config();

    TBool filter = conf.Param(stmGesture::EEnableFiltering) != 0;

    // TOUCH AREA
    TStmGestureArea& touchArea = *conf.Area(stmGesture::ETouchArea);
    TInt touchAreaSizeInMm = touchArea.iSize.iWidth;
    stateMachine->setTouchAreaShape ( stmUiEventEngine::TAreaShape(touchArea.iShape) );
    stateMachine->setTouchArea      ( !filter ? 0 : touchAreaSizeInMm );
    stateMachine->setTouchTimeout   ( !filter ? 0 : touchArea.iTimeout * 1000);
    CCoeControl* gestureContext = aContext.getOwner();
    TRect ctxtRect = gestureContext->Rect();

    // ===================================================== PINCH
    using stmGesture::CPinchGestureRecogniser;

    if (conf.Enabled(CPinchGestureRecogniser::KUid))
        {
        CPinchGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            TReal32 pspeed = conf.Param(stmGesture::EPinchSpeed) / 1000.f;
            gesture->setPinchingSpeed(pspeed);
            }
        }

    // ===================================================== LONG PRESS
    using stmGesture::CLongPressGestureRecogniser;

    if (conf.Enabled(CLongPressGestureRecogniser::KUid))
        {
        CLongPressGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            gesture->setArea(ctxtRect);
            }
        }

    // ===================================================== TOUCH
    using stmGesture::CTouchGestureRecogniser;

    if (conf.Enabled(CTouchGestureRecogniser::KUid))
        {
        CTouchGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            // define empty area so that touch is reported only inside
            // our window (touch recogniser handles either an area or the target window)
            gesture->setArea(TRect());
            }
        }


    // ===================================================== PAN
    using stmGesture::CPanGestureRecogniser;

    if (conf.Enabled(CPanGestureRecogniser::KUid))
        {
        CPanGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            gesture->setPanningSpeedLow ( conf.Param(stmGesture::EPanSpeedLow)  / 1000.f );
            gesture->setPanningSpeedHigh( conf.Param(stmGesture::EPanSpeedHigh) / 1000.f );
            }
        }

    // ===================================================== TAP / DOUBLE TAP
    /// Add recognizer before any existing Flick, Release, Unknown
    // Add the gesture to the gesture engine
    // TODO: Tap recognizer is special - it can combine multiple listeners,
    //       so no need to create new one if it already exists, just add new listeners to it

    using stmGesture::CTapGestureRecogniser;

    if (conf.Enabled(CTapGestureRecogniser::KUid))
        {
        CTapGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            gesture->setDoubleTapTimeout(conf.Param(stmGesture::EDoubleTapTimeout) * 1000);
            gesture->setDoubleTapRange( touchAreaSizeInMm );
            gesture->ignoreFirstTap(EFalse);

            aContext.iLogging |= 1<<CTapGestureRecogniser::KUid;
            }
        }

    // ===================================================== FLICK
    using stmGesture::CFlickGestureRecogniser;

    if (conf.Enabled(CFlickGestureRecogniser::KUid))
        {
        CFlickGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            TReal32 flickSpeed = conf.Param(stmGesture::EFlickSpeed) / 1000.f;
            gesture->setFlickingSpeed(flickSpeed);
            }
        }

    // ===================================================== RELEASE
    using stmGesture::CReleaseGestureRecogniser;

    if (conf.Enabled(CReleaseGestureRecogniser::KUid))
        {
        CReleaseGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            gesture->setArea(TRect());
            }
        }

#if 0 // use in future depending upon browser requirement
    // ===================================================== CORNER ZOOM
    using stmGesture::CZoomGestureRecogniser;

    if (conf.Enabled(CZoomGestureRecogniser::KUid))
        {
        CZoomGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            gesture->setArea(ctxtRect);
            gesture->setRange(conf.Param(stmGesture::EZoomCornerSize));
            }
        }

    // ===================================================== EDGE SCROLL
    using stmGesture::CEdgeScrollGestureRecogniser;

    if (conf.Enabled(CEdgeScrollGestureRecogniser::KUid))
        {
        CEdgeScrollGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);

        if(gesture)
            {
            gesture->setArea(ctxtRect);
            gesture->setScrollRange(conf.Param(stmGesture::EEdgeScrollRange)); // range is 20 pixels from the edge TODO: add this to settings...
            }
        }

    // ===================================================== LEFT-RIGHT
    using stmGesture::CLeftrightGestureRecogniser;

    if (conf.Enabled(CLeftrightGestureRecogniser::KUid))
        {
        CLeftrightGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);
        }

    // ===================================================== UP-DOWN
    using stmGesture::CUpdownGestureRecogniser;

    if (conf.Enabled(CUpdownGestureRecogniser::KUid))
        {
        CUpdownGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);
        }

    // ===================================================== HOVER
    using stmGesture::CHoveringGestureRecogniser;

    if (conf.Enabled(CHoveringGestureRecogniser::KUid))
        {
        CHoveringGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);
        if(gesture)
            {
            gesture->setHoveringSpeed(conf.Param(stmGesture::EHoverSpeed) / 1000.f);
            }
        }

    // ===================================================== UNKNOWN
    using stmGesture::CUnknownGestureRecogniser;

    if (conf.Enabled(CUnknownGestureRecogniser::KUid))
        {
        CUnknownGestureRecogniser* gesture = NULL;
        InitRecognizerL(gesture, aContext, aNewContext);
        }
#endif

    // ===========================================================
    if(aNewContext)
        {
        for(TInt i = 0; i < stmGesture::EStmGestureUid_Count; ++i)
            {
            MStmGestureRecogniser* rec = aContext.iRecognizers[i];
            if(rec)
                {
                rec->enableLogging(aContext.iLogging & (1<<i)) ;
                }
            }
        }


    // HOLD AREA
    TStmGestureArea& holdArea = *conf.Area(stmGesture::EHoldArea);
    TInt holdAreaSizeInMm = holdArea.iSize.iWidth;
    stateMachine->setHoldAreaShape  ( stmUiEventEngine::TAreaShape(holdArea.iShape) );
    stateMachine->setHoldArea       ( !filter ? 0 : holdAreaSizeInMm );
    stateMachine->setHoldTimeout    ( !filter ? 0 : holdArea.iTimeout * 1000);

    // TOUCH-TIME AREA
    TStmGestureArea& tTimeArea = *conf.Area(stmGesture::ETouchTimeArea);
    TInt tTimeAreaSizeInMm = tTimeArea.iSize.iWidth;
    // NB: shape <-- TouchAreaShape
    stateMachine->setTouchTimeArea      ( !filter ? 0 : tTimeAreaSizeInMm );

    // Timeouts, Adjustments, etc.
    stateMachine->setTouchSuppressTimeout(!filter ? 0 :
                                            conf.Param(stmGesture::ESuppressTimeout)*1000) ;
    stateMachine->setMoveSuppressTimeout(!filter ? 0 :
                                            conf.Param(stmGesture::EMoveSuppressTimeout)*1000) ;
    stateMachine->enableCapacitiveUp    (!filter ? 0 :
                                            conf.Param(stmGesture::ECapacitiveUpUsed));
    stateMachine->enableYadjustment     (!filter ? 0 : conf.Param(stmGesture::EAdjustYPos));
    stateMachine->enableLogging(false);

    stateMachine->EnableWsEventMonitoring(false);
    LOG("Setup recognizers -- DONE");
    }

EXPORT_C void CStmGestureEngine::HandlePointerEventL(
    const TPointerEvent& aPointerEvent,
    void* target)
    {
    iGf->getUiStateMachine()->HandlePointerEventL(aPointerEvent,target);
    }

// =============================================================

CStmGestureContext::CStmGestureContext(
    CStmGestureEngine& aEngine ) :
    iEngine(aEngine),
    iEventSender(NULL)
    {

    }

void CStmGestureContext::ConstructL()
    {
    iEngine.iContexts.AppendL(this);
    iConfig = new(ELeave) CStmGestureParameters();
    }

CStmGestureContext::~CStmGestureContext()
    {
    // Remove all gesture listeners
    for(TInt i = iListeners.Count() - 1; i >= 0; --i)
        {
        MStmGestureListener* listener = iListeners[i];
        iListeners.Remove(i);
        }

    iListeners.Reset();

    // Remove all context's recognizers from the Gesture Engine
    Deactivate();

    if (iEventSender)
        delete iEventSender;

    // Destroy all recognizers
    for(TInt r = 0; r < stmGesture::EStmGestureUid_Count; ++r)
        {
        MStmGestureRecogniser*& rec = iRecognizers[r];
        if(rec)
            {
            delete rec;
            rec = NULL;
            }
        }

    // Remove context from the Gesture Engine
    TInt idx = iEngine.iContexts.Find(this);
    if(idx != -1)
        {
        iEngine.iContexts.Remove(idx);
        }
    delete iConfig;
    }

EXPORT_C void CStmGestureContext::AddListenerL(
    MStmGestureListener* aListener,
    TInt aPos )
    {
    iEventSender = CGestureEventSender::NewL(*aListener);
    iListeners.InsertL(aListener, aPos);
    }

EXPORT_C TInt CStmGestureContext::RemoveListener(
    MStmGestureListener* aListener )
    {
    TInt ind = iListeners.Find(aListener);
    if(ind >= 0)
        {
        iListeners.Remove(ind);
        }
    return ind;
    }

EXPORT_C void CStmGestureContext::SetContext(
    CCoeControl* aControl )
    {
    iOwnerControl = aControl;

    for(TInt i = 0; i < stmGesture::EStmGestureUid_Count; ++i)
        {
        MStmGestureRecogniser* rec = iRecognizers[i];
        if(rec)
            {
            rec->setOwner(aControl);
            }
        }
    }

EXPORT_C void CStmGestureContext::ActivateL()
    {
    if(IsActive())
        {
        // re-enable all recognizers
        EnableRecognizersL();
        return;
        }
    /// 1. Tell Engine to deactivate active context

    /// 2. Setup all gesture recognizers
    iEngine.EnableContextL(*this);

    /// 3. Notify listeners

    //
    EnableRecognizersL();

    ///
    iActivated = ETrue;
    }

EXPORT_C void CStmGestureContext::Deactivate()
    {
    // NB: reactivation of previous context may leave
    TRAP_IGNORE(iEngine.DisableContextL(*this));
    iActivated = EFalse;
    }

void CStmGestureContext::gestureEnter(
    MStmGesture& aGesture )
    {
    TRAP_IGNORE(DispatchGestureEventL(aGesture.gestureUid(), &aGesture));
    }

void CStmGestureContext::gestureExit(
    TStmGestureUid aGestureUid )
    {
    TRAP_IGNORE(DispatchGestureEventL(aGestureUid, NULL));
    }

CCoeControl* CStmGestureContext::getOwner()
    {
    return iOwnerControl;
    }

void CStmGestureContext::DispatchGestureEventL(
    TStmGestureUid aUid,
    MStmGesture* aGesture )
    {
    iEventSender->AddEvent(aUid, aGesture);
    }

void CStmGestureContext::SuspendRecognizer(
    TStmGestureUid aUid )
    {
    MStmGestureRecogniser* rec = iRecognizers[aUid];
    if(rec && !rec->isEnabled())
        {
        rec->enable(EFalse);
        // TODO: Notify listener
        }
    }

void CStmGestureContext::EnableRecognizersL()
    {
    for(TInt i = 0; i < stmGesture::EStmGestureUid_Count; ++i)
        {
        EnableRecognizerL(TStmGestureUid(i));
        }
    }

void CStmGestureContext::EnableRecognizerL(
    TStmGestureUid aUid )
    {
    MStmGestureRecogniser* rec = iRecognizers[aUid];
    if(rec && !rec->isEnabled())
        {
        rec->enable(ETrue);
        // TODO: Notify listener
        }
    }

