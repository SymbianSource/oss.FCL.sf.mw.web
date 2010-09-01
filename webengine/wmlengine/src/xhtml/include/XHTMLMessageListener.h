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
* Description:  
*
*/


#ifndef _XHTMLMESSAGELISTENER_H
#define _XHTMLMESSAGELISTENER_H

// INCLUDES
#include "MSGMessageListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
struct NW_XHTML_ContentHandler_s;
typedef struct NW_XHTML_ContentHandler_s NW_XHTML_ContentHandler_t;

// CLASS DECLARATION

/**
*  TMessageListener
*  
*  @lib xhtml.lib
*  @since 2.6
*/
class TMessageListener : public MMessageListener
{
  public:  // Constructors and destructor
    /**
    * C++ default constructor.
    */
    TMessageListener (NW_XHTML_ContentHandler_t* aContentHandler);
      
  public: // New functions    
    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    void HandleMessage (NW_Msg_Message_t* message);

  private: // New functions

  private:    // Data
    NW_XHTML_ContentHandler_t* iContentHandler;
};

#endif // _XHTMLMESSAGELISTENER_H
