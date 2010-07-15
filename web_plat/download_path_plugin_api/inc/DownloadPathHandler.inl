/*****************************************************************************
  File        : DownloadPathHandler.inl
  Part of     : web_plat / download_path_plugin_api
  Description : Music path plugin interface inline implementation file
  Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.

  Copyright (c) 2010 Nokia Corporation.
  This material, including documentation and any related
  computer programs, is protected by copyright controlled by
  Nokia Corporation. All rights are reserved. Copying,
  including reproducing, storing, adapting or translating, any
  or all of this material requires the prior written consent of
  Nokia Corporation. This material also contains confidential
  information which may not be disclosed to others without the
  prior written consent of Nokia Corporation.

 *****************************************************************************
              Definition of the CDownloadPathHandler inline functions
 *****************************************************************************/

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CDownloadPathHandler::CDownloadPathHandler()
    {
    }

// -----------------------------------------------------------------------------
// Destructor only uses the ECOM framework and the private member iDtor_ID_Key
// to destroy the instance
// -----------------------------------------------------------------------------
//
inline CDownloadPathHandler::~CDownloadPathHandler()
    {
    // Destroy any instance variables and then
    // inform the framework that this specific 
    // instance of the interface has been destroyed.
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// -----------------------------------------------------------------------------
// This factory uses a resolver parameter to find appropriate type of the 
// instance to be returned
// -----------------------------------------------------------------------------
//
inline CDownloadPathHandler* CDownloadPathHandler::NewL( const TDesC& aMatchString )
    {
    CDownloadPathHandler* handler = CDownloadPathHandler::NewLC( aMatchString );
    CleanupStack::Pop( handler );
    return handler;
    }

// -----------------------------------------------------------------------------
// This factory uses a resolver parameter to find appropriate type of the 
// instance to be returned. Leaves pointer on the cleanup stack.
// -----------------------------------------------------------------------------
//
inline CDownloadPathHandler* CDownloadPathHandler::NewLC( const TDesC& aMatchString )
    {
    HBufC8* matchString8 = HBufC8::NewLC( aMatchString.Length() );
    matchString8->Des().Copy( aMatchString );
    CDownloadPathHandler* handler = CDownloadPathHandler::NewLC( matchString8->Des() );
    CleanupStack::PopAndDestroy( matchString8 );
    return handler;
    }

// -----------------------------------------------------------------------------
// This factory uses a resolver parameter to find appropriate type of the 
// instance to be returned
// -----------------------------------------------------------------------------
//
inline CDownloadPathHandler* CDownloadPathHandler::NewL( const TDesC8& aMatchString )
    {
    CDownloadPathHandler* handler = CDownloadPathHandler::NewLC( aMatchString );
    CleanupStack::Pop( handler );
    return handler;
    }

// -----------------------------------------------------------------------------
// This factory uses a resolver parameter to find appropriate type of the 
// instance to be returned. Leaves pointer on the cleanup stack.
// -----------------------------------------------------------------------------
//
inline CDownloadPathHandler* CDownloadPathHandler::NewLC( const TDesC8& aMatchString )
    {
    // Set up the interface find for the default resolver.
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aMatchString );
    resolverParams.SetWildcardMatch( ETrue );     // Allow wildcard matching

    TAny* ptr = REComSession::CreateImplementationL( KCDownloadPathHandlerUid, 
                                                     _FOFF( CDownloadPathHandler,iDtor_ID_Key ), 
                                                     resolverParams );
    CleanupStack::PushL( ptr );
    return REINTERPRET_CAST( CDownloadPathHandler*, ptr );
    }

// -----------------------------------------------------------------------------
// This function lists all the implementations currently present in the
// ECOM framework.
// -----------------------------------------------------------------------------
//
inline void CDownloadPathHandler::ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
    {
    REComSession::ListImplementationsL( KCDownloadPathHandlerUid, aImplInfoArray );
    }

// End of file
