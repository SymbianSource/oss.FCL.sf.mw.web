/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling of general url loading
*
*/

#ifndef URLLOADERI_H
#define URLLOADERI_H

//  INCLUDES
#include <brctldefs.h>
#include "urlloader_urlloaderint.h"
#include "httploader_mlinkresolvercallbacks.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

    // CONSTANTS

    // MACROS

    // DATA TYPES

    // FUNCTION PROTOTYPES

    // FORWARD DECLARATIONS

    // CLASS DECLARATION

    // BrowserAppUi Interface
    class MBrCtlSpecialLoadObserver;
     //webkit Interface
     class MWmlInterface;

    /**
    * Initialize the loaders.
	* aHttpCallbacks A callback structure for dialogs
    */
    void UrlLoader_InitializeL(MBrCtlSpecialLoadObserver* aSpecialLoadObserver,
    							MHttpLoader_LinkResolverCallbacks* aLinkCallbacks, MWmlInterface* aWKWmlInterface);

    /**
    * Initialize the system.
    */
    TBrowserStatusCode NW_Initialize_Loadercli();


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif      // URLLOADERI_H

// End of File
