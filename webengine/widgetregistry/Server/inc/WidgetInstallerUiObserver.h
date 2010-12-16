/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Publish&Subscribe event listener 
*
*/
  

#ifndef C_WIDGETINSTALLERUIOBSERVER_H
#define C_WIDGETINSTALLERUIOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "WidgetRegistry.h"

// Widget registry server secure id
const TUid KPropertyCat = { 0x10282F06 };

enum TPropertyKeys { EWidgetInstallUninstallStart = 113 };

/**
 * Publish&Subscribe event listener
 *
 * @since S60 v5.0
 */
class CWidgetInstallerUiObserver : public CActive
    {    
    public:
        /**
         * Creates an instance of CWidgetInstallerUiObserver implementation.
         *
         * @since S60 v5.0
         * @param aCallBack Reference to notifier interface.
         */
        static CWidgetInstallerUiObserver* NewL( CWidgetRegistry* aRegistry );
        
        /**
         * Destructor.
         */
        ~CWidgetInstallerUiObserver();
        
        
    private:
    
        /**
        * 
        * @param aHarvester
        * @param aKey
        */
        CWidgetInstallerUiObserver( CWidgetRegistry* aRegistry );
        
        /**
        * 
        */
        void ConstructL();
        
        /**
        * from CActive
        */
        void RunL();

        /**
        * from CActive
        */
        void DoCancel();
        
        /**
        * from CActive
        */
        TInt RunError(TInt aError);
        
    private:
        /**
         * User side interface to Publish & Subscribe.
         */
        RProperty iProperty;
        
        /**
         * not own
         */
        CWidgetRegistry* iRegistry;
    };

#endif // C_WRTHARVESTERNOTIFIER_H

// End of File
