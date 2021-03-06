/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CCodEng.   
*      
*
*/


#ifndef COD_ENG_H
#define COD_ENG_H

// INCLUDES

#include <e32base.h>
#include <apmrec.h>

#include "CodEngBase.h"

// FORWARD DECLARATION

class CEikProcess;
class MCodLoadObserver;

// CLASS DECLARATION

/**
* COD Handler Engine (processing COD, fetching content, posting status).
*/
NONSHARABLE_CLASS( CCodEng ): public CCodEngBase
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param aProcess Host process.
        * @param aObserver Observer or NULL.
        * @return The created model.
        */      
        IMPORT_C static CCodEng* NewL
            ( CEikProcess* aProcess, MCodLoadObserver* aObserver = NULL );
        
        /**
        * Destructor.
        */      
        IMPORT_C ~CCodEng();

    protected:  // Constructors and destructor

        /**
        * Constructor.
        * @param aObserver Observer.
        */      
        CCodEng( MCodLoadObserver* aObserver );
        
    protected:  // from CCodEngBase

        /**
        * Check data (storage space, MIME type, mandatory attributes etc.)
        * Leave on any errors.
        */
        void CheckDataL();

        /**
        * Parse data buffer.
        */
        void ParseDataL();

        /**
        * Get http status code from error code and state. This method contains
        * the  semantic interpretation of errors; e.g. failure to notify due
        * to loss of service is regarded as success, etc.
        * @param aError Error code.
        * @param aState State.
        * @return HTTP status code (to post).
        */
        TInt StatusCode( TInt aError, TState aState ) const;

        /**
        * Get textual representation of COD notify HTTP code. This text is
        * sent back as notification.
        * @param aCode HTTP code, values should only be the ones returned by
        * StatusCode(). Other values panic.
        * @return Textual form of HTTP code.
        */
        const TDesC8& StatusText( TInt aCode ) const;

    };

#endif /* def COD_ENG_H */
