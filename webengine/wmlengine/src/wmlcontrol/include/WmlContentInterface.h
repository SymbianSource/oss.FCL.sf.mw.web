/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of MContentLoaderInterface 
*
*/


#ifndef MWMLCONTENTINTERFACE_H
#define MWMLCONTENTINTERFACE_H

// INCLUDES
#include <e32base.h>
#include "BrsrStatusCodes.h"
//#include "urlloader_loaderutils.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class provides an interface for the resource loader client
*  to manage transactions
*  @lib resLoader.lib
*  @since 3.1
*/
NONSHARABLE_CLASS(CWmlContentInterface) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWmlContentInterface* NewL();

        /**
        * Destructor.
        */
        virtual ~CWmlContentInterface();
        
    public: // New functions
        
        /**
        * sends the partial response.
        * @since 3.1
        * @return Kimono wide error code.
        */
       TBrowserStatusCode WMLPartialResponse( const TUint16* aUri,
        				      TPtr8& aBody,
        				      void* aHeaders,
                                              TUint8* aContentTypeString,
			                      TUint8* aContentLocationString,
			                      TBool aNoStore,
			                                TUint8* aBoundaryString,
			                                TUint16* aLastModified,
			                                TUint16 aCharset,
			                                TUint8* aCharsetString,
			                                TUint aHttpStatus,
			                                TUint8 aMethod,
			                                TUint16 aTransId,
			                                TInt aChunkIndex,
			                                void* aCertInfo,
			                                TUint aContentLength,
			                                TInt aErr,
			                                void* aLoadContext,
			                                void* aPartialCallback );
        void setPartialCallBack(void* callback){m_partialLoadCallback = callback;}
        void setLoadContext(void* loadcontext){m_loadContext = loadcontext;}
        void setTransId(TUint16 id){m_transId = id;}
        void setUrl(const TDesC& aUrl);

        void* getPartialCallBack(){return m_partialLoadCallback;}
        void* getLoadContext() {return m_loadContext;}
        TUint16 getTransId() {return m_transId;}
        HBufC* getUrl(){return m_resourceUrl;}
			                                
	private:

        /**
        * C++ default constructor.
        */
        CWmlContentInterface();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private:
    	// Boolean to determine if 300 or 500 errors have associated content
    	TBool iIsBodyPresent;

        void* m_partialLoadCallback;
        void* m_loadContext;
        TUint16 m_transId;
        HBufC*  m_resourceUrl;
     };

#endif      // MWMLCONTENTINTERFACE_H
            
// End of File
