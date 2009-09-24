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
* Description:  Acts on MMC card insertion/removal
*
*/


#ifndef WIDGETMMCHANDLER_H
#define WIDGETMMCHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "wrtharvester.h"
//hb
#include "wrtharvesterpsnotifier.h"

// FORWARD DECLARATION
class CWrtHarvester;

/*
 * Inherited CActive, performs a asynchronous conversion operation
 *
 * @since 3.1
 */
class CWidgetMMCHandler : public CActive
    {
public:

    static CWidgetMMCHandler* NewL( CWrtHarvester* aHarvester, RFs& aFs );

    virtual ~CWidgetMMCHandler();

    void Start();
    void ToggleUSBMode();

protected:

    void RunL();

    TInt RunError( TInt aError );

    void DoCancel();

private:

    // no copy constructor
    CWidgetMMCHandler( const CWidgetMMCHandler& );
    // no assignment
    CWidgetMMCHandler& operator=( const CWidgetMMCHandler& );

    CWidgetMMCHandler( CWrtHarvester* aHarvester, RFs& aFs );

    void ConstructL();

    TInt ScanDrives( TInt& aDriveFlags );

   // CWidgetRegistry* iRegistry; // not owned
    
    CWrtHarvester* iHarvester;
    RFs iFs;

    TInt iDriveFlags;

    };

#endif // #ifndef WIDGETMMCHANDLER_H
