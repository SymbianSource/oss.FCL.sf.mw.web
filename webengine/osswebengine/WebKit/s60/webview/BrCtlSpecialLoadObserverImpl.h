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



#ifndef BRCTLSPECIALLOADOBSERVERIMPL_H
#define BRCTLSPECIALLOADOBSERVERIMPL_H

//  INCLUDES
#include <brctlspecialloadobserver.h>

/**
*  This observer handles special load events such as network connection, deal with non-http(s) or non-html requests
*
*  @lib BrowserEngine.lib
*  @since 2.8
*/
//NONSHARABLE_CLASS(CBrCtlSpecialLoadObserver) : public CBase, public MBrCtlSpecialLoadObserver
class CBrCtlSpecialLoadObserver : public CBase, public MBrCtlSpecialLoadObserver
    {
    public: // From MBrCtlSpecialLoadObserver
        /**
        * Request to create a network connection.
        * @since 2.8
        * @param aConnectionPtr A pointer to the new connection. If NULL, the proxy filter will automatically create a network connection
        * @param aSockSvrHandle A handle to the socket server.
        * @param aNewConn A flag if a new connection was created. If the connection is not new, proxy filter optimization will not read the proxy again from CommsBd
        * @param aBearerType The bearer type of the new connection
        * @return void
        */
        virtual void NetworkConnectionNeededL(TInt* /*aConnectionPtr*/,
                                              TInt* /*aSockSvrHandle*/,
                                              TBool* /*aNewConn*/,
                                              TApBearerType* /*aBearerType*/) {}

        /**
        * Request the host applicaion to handle non-http request.
        * @since 2.8
        * @param aUrl The non-http(s) or file URL
        * @param aParamList Parameters to pass to the host application. Contain referer header. It could be NULL
        * @return ETrue is handled by the host application. EFlase if not
        */
        virtual TBool HandleRequestL(RArray<TUint>* /*aTypeArray*/, CDesCArrayFlat* /*aDesArray*/) {return EFalse;}

        /**
        * Request the host applicaion to handle downloads
        * @since 2.8
        * @param aTypeArray array of download parameter types
        * @param aDesArray array of values associated with the types in the type array
        * @return ETrue is handled by the host application. EFlase if not
        */
        virtual TBool HandleDownloadL(RArray<TUint>* /*aTypeArray*/,
						CDesCArrayFlat* /*aDesArray*/) {return EFalse;}
    };

#endif      // BRCTLSPECIALLOADOBSERVERIMPL_H
            
// End of File
