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
* Description:  Responsible for capturing pointer events
*
*/


#ifndef _POINTERCAPTURER_H_
#define _POINTERCAPTURER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CAlfControl;
class CAlfControlGroup;
class CAlfEnv;
class CAlfDisplay;

// CLASS DECLARATION

namespace RT_GestureHelper 
{

class CGestureHelperImpl;
class CPointerCaptureControl;

/**
 * CPointerCaptureControl
 * Gesture control own a gesture helper, and captures any pointer events that 
 * no other control captures. After it captures pointer down event, it will 
 * capture all pointer events until pointer up. It does this by bringing its
 * control group to the top upon pointer down, and lowering the control group
 * to the botton upon pointer up.
 */
NONSHARABLE_CLASS( CPointerCapturer ) : public CBase
    {
public: 
    /** Two-phased constructor */
    static CPointerCapturer* NewL();
    /** Destructor */
    ~CPointerCapturer();
    
    void InitForAlfredL( CGestureHelperImpl& aHelper, CAlfEnv& aEnv, 
        CAlfDisplay& aDisplay, TInt aControlGroupId );

    void StartL();
    void Stop();
    
private:    
    /// 
    CPointerCaptureControl* iCaptureControl;
    TInt iControlGroupId;
    };

} // namespace GestureHelper
    
#endif // _POINTERCAPTURER_H_
