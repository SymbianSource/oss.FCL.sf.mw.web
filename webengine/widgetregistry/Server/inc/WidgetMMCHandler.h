/*
* ============================================================================
*  Name        : WidgetMMCHandler.h
*  Part of     : Widget Registry
*  Description : Acts on MMC card insertion/removal
*  Version     : 3.1
*
*  Copyright © 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* =============================================================================
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

public:

    LOG_MEMBER_VARS
    };

#endif // #ifndef WIDGETMMCHANDLER_H
