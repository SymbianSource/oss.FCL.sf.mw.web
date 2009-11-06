/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle special load events such as network connection, deal with non-http or non-html requests
*
*/



#ifndef BRCTLWINDOWOBSERVERIMPL_H
#define BRCTLWINDOWOBSERVERIMPL_H

//  INCLUDES
#include <BrCtlWindowObserver.h>


/**
*  This observer handles special load events such as network connection, deal with non-http(s) or non-html requests
*
*  @lib BrowserEngine.lib
*  @since 2.8
*/
class CBrCtlWindowObserver : public CBase, public MBrCtlWindowObserver
    {
    public: // From MBrCtlWindowObserver
        /**
        * Request the host applicaion to open the URL in a new window
        * @since 3.0
        * @param aUrl The Url of the request to be done in the new window
        * @param aTargetName The name of the new window
        * @param aUserInitiated ETrue if the new window is initiated by a user event (click)
        * @param aReserved For future use
        * @return Return Value is for future reference and is currently ignored
        */
        virtual CBrCtlInterface* OpenWindowL(TDesC& /*aUrl*/, TDesC* /*aTargetName*/, 
			TBool /*aUserInitiated*/,  TAny* /*aReserved*/) {return NULL;}
    
        /**
        * Find a window by target name
        * @since 3.0
        * @param aTargetName name of the window to find
        * @return Return Value is the browser control associated with the window name
        */
        virtual CBrCtlInterface* FindWindowL( const TDesC& /*aTargetName*/ ) const { return NULL; }             

        /**
        * Handle window events such as close/focus etc
        * @since 3.0
        * @param aTargetName name of the window to send the event to
        * @param aCommand Command to pass to the window
        * @return void
        */
        virtual void HandleWindowCommandL( const TDesC& /*aTargetName*/, TBrCtlWindowCommand /*aCommand*/ ) {}

    };

#endif      // BRCTLWINDOWOBSERVERIMPL_H
            
// End of File
