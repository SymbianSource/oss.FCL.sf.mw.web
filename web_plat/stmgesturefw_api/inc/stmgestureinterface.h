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


#ifndef __STMGESTUREINTERFACE_H__
#define __STMGESTUREINTERFACE_H__

#include <e32base.h>
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>
#include <rt_uievent.h>
#include <stmgesturelistener.h>

// FORWARD DECLARATION
namespace stmGesture
{
    class CGestureFramework;
    class MGestureRecogniserIf;
}

class CStmGestureContext;
class MStmGestureListener;

typedef stmGesture::MGestureIf MStmGesture;
typedef stmGesture::TGestureUid TStmGestureUid;
typedef stmGesture::MGestureListener MStmGestureEngineListener;
typedef stmGesture::MGestureRecogniserIf MStmGestureRecogniser;
typedef stmGesture::TGestureSettings TStmGestureSettings;
typedef stmGesture::TGestureAreaSettings TStmGestureAreaSettings;
typedef stmGesture::TGestureArea TStmGestureArea;

///==================================================================

/**
 * Interface class for defining gesture settings parameters
 */
class MStmGestureParameters
    {
public:
    virtual TStmGestureArea* Area(TStmGestureAreaSettings aArea) = 0;
    virtual TInt Param(TStmGestureSettings aParam) const = 0;
    virtual TUint8 Enabled(TStmGestureUid aGesture) const = 0;
    };

///==================================================================

/**
 * Class definition for gesture settings parameters
 */
NONSHARABLE_CLASS( CStmGestureParameters ): public CBase, public MStmGestureParameters
    {
public:
    /*!
     * Constructors
     */
    IMPORT_C CStmGestureParameters();
    IMPORT_C CStmGestureParameters(const CStmGestureParameters& aParams);

public: //getter methods

    /*!
     * To get the area settings
     */
    TStmGestureArea* Area(TStmGestureAreaSettings aArea) { return &iAreaSettings[aArea]; }

    /*!
     * To get Gesture params
     */
    TInt Param(TStmGestureSettings aParam) const { return iParam[aParam];  }

    /*!
     * Gesture enabled or not
     */
    TUint8 Enabled(TStmGestureUid aGesture) const { return iEnabled[aGesture] != 0; }

public: // setter methods

    /*!
     * To set the parameters
     */
    void SetParam(TStmGestureSettings aParam, TInt aValue) { iParam[aParam] = aValue; }

    /*!
     * To enable or disble the gestures
     */
    void SetEnabled(TStmGestureUid aGesture, TUint8 aParam) { iEnabled[aGesture] = aParam; }

    /*!
     * Gesture settings
     */
    TInt& operator[](TStmGestureSettings aParam) { return iParam[aParam]; }

private:
    TInt iParam[stmGesture::EGestureSettingsCount];
    TStmGestureArea iAreaSettings[stmGesture::EGestureAreaSettingsCount];
    TInt iModified;
    TUint8 iEnabled[stmGesture::EStmGestureUid_Count];
    };

///==================================================================

/**
 * CStmGestureEngine Class Declaration
 */
NONSHARABLE_CLASS( CStmGestureEngine ): public CBase
    {
public:

    /*!
     * Gesture Engine Creation
     */
    IMPORT_C static CStmGestureEngine* NewL();
    ~CStmGestureEngine();

public:

    /*!
     * To Create a context in gesture engine
     */
    IMPORT_C CStmGestureContext* CreateContextL(TInt aId);

    /*!
     * To get the context
     */
    IMPORT_C CStmGestureContext* Context(TInt aId);

    /*!
     * Application use HandlePointerEventL API to pass the raw hardware events to the gesture library
     */
    IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent, void* target);

private:

    /*!
     * default constructor
     */
    CStmGestureEngine();

    /*!
     * Two-phase constructor
     */
    void ConstructL();

private: // used by friend class CStmGestureContext

    void EnableContextL(CStmGestureContext& aCtxt);
    void DisableContextL(CStmGestureContext& aCtxt);

    template<class T>
    void InitRecognizerL( T*& aGesture, CStmGestureContext& aContext, TBool aNewCOntext);

    void SetupRecognizersL(CStmGestureContext& aCtxt, TBool aNewContext);

private:
    friend class CStmGestureContext;
    stmGesture::CGestureFramework* iGf;
    // Registered/owned contexts
    RPointerArray<CStmGestureContext> iContexts;
    // Active set of contexts; latest added defines parameters
    RPointerArray<CStmGestureContext> iCtxtStack;
    // Global/default parameters
    CStmGestureParameters* iConfig;
    TBool iDtorPhase;
    };

///==================================================================

//Forward declaration
class CGestureEventSender;

/**
 * Gesture context represents a certain configuration of gesture parameters
 * (including what gestures are enabled) and gesture listeners.
 * Context can be created beforehand and reused later.
 * The assumed use of a context is to have rough correspondence between
 * UI view and enabled gestures.
 *
 * @note Gesture contexts are created by an instance of CStmGestureEngine.
 */

class CStmGestureContext: public CBase, public MStmGestureEngineListener
    {
public:
    /*!
     * Constructor
     */
    CStmGestureContext(CStmGestureEngine& aEngine);

    ~CStmGestureContext();

public: /// Gesture listeners

    /*!
     * Add listener to the gesture processing context.
     * @param aListener Listener object
     * @param aPos Position of inserted listener which determines order in which it receives gesture events (0 == first, before existing)
     */
    IMPORT_C void AddListenerL(MStmGestureListener* aListener, TInt aPos = 0);

    /*!
     * Remove listener from the gesture processing context.
     * @param aListener Listener object
     */
    IMPORT_C TInt RemoveListener(MStmGestureListener* aListener);

    /*!
     * To get the list of listeners
     */
    const RPointerArray<MStmGestureListener>& Listeners() const { return iListeners; }

public: /// Context activation
    
    /*!
     * To set the context configuration in Gesture Engine.
     */
    IMPORT_C void SetContext(CCoeControl* aControl);
   
    /*!
     * Initialize context configuration on the Gesture Engine.
     */
    IMPORT_C void ActivateL();
    
    /*!
     * deactivate context configuration on the Gesture Engine.
     */
    IMPORT_C void Deactivate();

    /*!
     * gesture configuration
     */
    CStmGestureParameters& Config() { return *iConfig; }
    
    /*!
     * Enable Logging
     */
    void SetLogging(TInt aEnabled) { iLogging = aEnabled; }
    
    /*!
     * Context control
     */
    CCoeControl* ContextControl() { return iOwnerControl; }
    
    /*!
     * is gesture context is active ?
     */   
    TBool IsActive() { return iActivated; }
     
    /*!
     * Context id
     */      
    TInt Id() const { return iCtxtId; }
    
public: // from MStmGestureEngineListener

    void gestureEnter(MStmGesture& aGesture);
    void gestureExit(TStmGestureUid aGestureUid);
    CCoeControl* getOwner();

private:
    friend class CStmGestureEngine;

    void ConstructL();
    void DispatchGestureEventL(TStmGestureUid aUid, MStmGesture* aGesture);
    void SuspendRecognizer(TStmGestureUid aUid);
    void EnableRecognizersL();
    void EnableRecognizerL(TStmGestureUid aUid);

private:
    CStmGestureEngine& iEngine;
    // Application-specific ID for finding context
    TInt iCtxtId;
    TBool iActivated;
    CStmGestureParameters* iConfig;
    CCoeControl* iOwnerControl;
    //TBool iPinchUsed;
    RPointerArray<MStmGestureListener> iListeners;
    // for simplicity of addressing recognizers, we'll use fixed array
    // with UID-based index (range 0..N is assumed for UIDs)
    // @note Tap recognizer actually produces Tap and DoubleTap gesture,
    //       so should be handled carefully -- only Tap position is set
    MStmGestureRecogniser* iRecognizers[stmGesture::EStmGestureUid_Count];
    TBool iLogging;
    CGestureEventSender* iEventSender;
    };

#endif 
