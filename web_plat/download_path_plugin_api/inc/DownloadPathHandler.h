/*****************************************************************************
  File        : DownloadPathHandler.h
  Part of     : web_plat / download_path_plugin_api
  Description : Download path handler plugin interface definition file
  Version     : %version: sd1s60p2#1 % << Don't touch! Updated by Synergy at check-out.

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
                      Declaration of the CDownloadPathHandler class
 *****************************************************************************/

#ifndef DOWNLOADPATHHANDLER_H
#define DOWNLOADPATHHANDLER_H

// INCLUDES
#include <e32base.h>
#include <ECom.h>
#include <badesca.h>

// FORWARD DECLARATIONS

// UID of this interface
const TUid KCDownloadPathHandlerUid = { 0x2002682B };

/**
*
* @class    CDownloadPathHandler DownloadPathHandler.h
* @brief    This is an interface of the Music Download Path Handler ECOM plugin
*
* @version  1.0
*
*/
class CDownloadPathHandler : public CBase
    {
    public:

        /**
        * Instantiates an object of this type 
        * using the aMatchString as the resolver parameter.
        * @param  aMatchString - 8 bit resolver parameter (content mime type)
        * @return CDownloadPathHandler instance
        */
        static CDownloadPathHandler* NewL( const TDesC8& aMatchString );
        
        /**
        * Instantiates an object of this type 
        * using the aMatchString as the resolver parameter.
        * Leaves the pointer to the object on the cleanup stack.
        * @param  aMatchString - 8 bit resolver parameter (content mime type)
        * @return CDownloadPathHandler instance
        */
        static CDownloadPathHandler* NewLC( const TDesC8& aMatchString );

        /**
        * Instantiates an object of this type
        * using the aMatchString as the resolver parameter.
        * @param  aMatchString - 16 bit resolver parameter (content mime type)
        * @return CDownloadPathHandler instance
        */
        static CDownloadPathHandler* NewL( const TDesC& aMatchString );

        /**
        * Instantiates an object of this type
        * using the aMatchString as the resolver parameter.
        * Leaves the pointer to the object on the cleanup stack.
        * @param  aMatchString - 16 bit resolver parameter (content mime type)
        * @return CDownloadPathHandler instance
        */
        static CDownloadPathHandler* NewLC( const TDesC& aMatchString );

        /**
        * Destructor.
        */
        virtual ~CDownloadPathHandler();
    
        /**
        * Request a list of all available implementations which 
        * satisfy this given interface.
        * @param  aImplInfoArray - array to be filled in
        * @return none
        */
        static void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );

        /**
        * Gets the updated destination path to store downloaded content.
        * @param aFileName     File name with path.
        * @param aDestPath     Destination path of the content.
        */
        virtual void GetUpdatedPathL( const TDesC& aFileName, TDes& aDestPath ) = 0;

    protected:
        /**
        * Default C++ c'tor
        */
        inline CDownloadPathHandler();
    
    private:
        // Unique instance identifier key
        TUid iDtor_ID_Key;
    };

#include "DownloadPathHandler.inl"

#endif /* DownloadPathHandler_H */
