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
* Description:  Content handler for XHTML content type
*
*/


// INCLUDE FILES

#include "nw_msg_message.h"
#include "nw_text_ucs2.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "XHTMLMessageListener.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

 
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// TMessageListener::TMessageListener
// -----------------------------------------------------------------------------
//
TMessageListener::TMessageListener (NW_XHTML_ContentHandler_t* aContentHandler)
  {
  iContentHandler = aContentHandler;
  }

// -----------------------------------------------------------------------------
// NW_Msg_IMessageListener method implementations
// -----------------------------------------------------------------------------
void
TMessageListener::HandleMessage (NW_Msg_Message_t* message)
    {
    NW_ASSERT (message != NULL);

    switch (NW_Msg_Message_GetMessageId (message))
        {
        case NW_XHTML_ContentHandler_Msg_ShowFragment:
            {
            NW_Text_t* text = (NW_Text_t*) NW_Msg_Message_GetData (message);
            NW_XHTML_ContentHandler_ShowFragment (iContentHandler, text);
            }
        break;

        default:
        break;
        }

    /* delete the message */
    NW_Object_Delete (message);
    }
