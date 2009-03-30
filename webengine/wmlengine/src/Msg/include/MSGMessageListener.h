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


#ifndef MSGMESSAGELISTENER_H
#define MSGMESSAGELISTENER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
struct NW_Msg_Message_s;
typedef struct NW_Msg_Message_s NW_Msg_Message_t;

// CLASS DECLARATION

/**
*  Base class for.
*
*  @lib msg.lib
*  @since 2.6
*/
class MMessageListener
{

  public:
    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    virtual void HandleMessage (NW_Msg_Message_t* message) = 0;

};

#endif      // MSGMESSAGELISTENER_H

// End of File
