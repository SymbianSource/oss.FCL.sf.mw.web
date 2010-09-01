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
* Description:   Implements gesture interface functionality
*
*/

#ifndef WEBGESTUREINTERFACE_H
#define WEBGESTUREINTERFACE_H

//  INCLUDES
#include <stmgestureinterface.h>

// MACROS

// FORWARD DECLARATIONS
class WebView;

// CLASS DECLARATION
class WebGestureInterface : public CBase, public MStmGestureListener
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a WebGestureInterface object
     * @return a pointer to the created instance of WebGestureInterface.
     */
    static WebGestureInterface* NewL(WebView* view);

    static WebGestureInterface* NewLC(WebView* view);

    /**
     * ~CgestureInterface
     * Virtual Destructor.
     */
    ~WebGestureInterface();

public:   //member functions

    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
public:  //from MStmGestureListener    
    
    void HandleGestureEventL(const TStmGestureEvent& aGesture);

private:
    // Constructors

    void ConstructL();

    /**
     * WebGestureInterface.
     * C++ default constructor.
     */
    WebGestureInterface(WebView* view);
    
private:   
    CStmGestureEngine  *iGestureEngine;   // <<owned>>
    CStmGestureContext *iGestureContext;  // <<not owned>>
    WebView* m_webview;                   // <<not owned>>
    };

#endif //  WEBGESTUREINTERFACE_H
// End of File
