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
* Description:  This file contains the header file of the CWidgetEntry class.
*
*                This class implements the CWidgetEntry class.
*
*/

#ifndef WIDGETMMCHANDLER_H
#define WIDGETMMCHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <apgnotif.h>
#include "WidgetRegistryLog.h"

// FORWARD DECLARATION
class CWidgetRegistry;

/*
 * Inherited CActive, performs a asynchronous conversion operation
 *
 * @since 3.1
 */
class CWidgetMMCHandler : public CActive, public MApaAppListServObserver
    {
public:

    static CWidgetMMCHandler* NewL( CWidgetRegistry& aRegistry, RFs& aFs );

    virtual ~CWidgetMMCHandler();

    void Start();
    
    //from MApaAppListServObserver
    void HandleAppListEvent(TInt aEvent);
    
    void NeedToNotify(TBool aVal);
    
protected:

    void RunL();

    TInt RunError( TInt aError );

    void DoCancel();

private:

    // no copy constructor
    CWidgetMMCHandler( const CWidgetMMCHandler& );
    // no assignment
    CWidgetMMCHandler& operator=( const CWidgetMMCHandler& );

    CWidgetMMCHandler( CWidgetRegistry& aRegistry, RFs& aFs );

    void ConstructL();

    TInt ScanDrives( TInt& aDriveFlags );

    CWidgetRegistry* iRegistry; // not owned
    RFs iFs;

    TInt iDriveFlags;
    
    TInt iDeltaDriveFlags;
    
    CApaAppListNotifier* iApaAppListNotifier;
    
    TBool   iNeedToNotify;

public:

    LOG_MEMBER_VARS
    };

#endif // #ifndef WIDGETMMCHANDLER_H
