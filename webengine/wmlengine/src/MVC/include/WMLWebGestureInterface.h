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
 *
 *  Created on: Oct 19, 2010

 */
#ifndef WMLWEBGESTUREINTERFACE_H_
#define WMLWEBGESTUREINTERFACE_H_
#include <stmgesturelistener.h>
#include <stmgestureinterface.h>

class CStmGestureContext;
class CView;

// CLASS DECLARATION
class WMLWebGestureInterface : public CBase, public MStmGestureListener
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a WMLWebGestureInterface object
     * @return a pointer to the created instance of WMLWebGestureInterface.
     */
    static WMLWebGestureInterface* NewL(CView* view);

    static WMLWebGestureInterface* NewLC(CView* view);

    /**
     * ~CgestureInterface
     * Virtual Destructor.
     */
    ~WMLWebGestureInterface();

public:   //member functions

    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
public:  //from MStmGestureListener    
    
    void HandleGestureEventL(const TStmGestureEvent& aGesture);

private:
    // Constructors

    void ConstructL();

    /**
     * WMLWebGestureInterface.
     * C++ default constructor.
     */
    WMLWebGestureInterface(CView* view);
    
private:   
    CStmGestureEngine  *iGestureEngine;   // <<owned>>
    CStmGestureContext *iGestureContext;  // <<not owned>>
    CView* m_WMLview;                   // <<not owned>>
    };

#endif /* WMLWMLWebGestureInterface_H_ */
