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


#ifndef MVCVIEWLISTENER_H
#define MVCVIEWLISTENER_H

//  INCLUDES
#include "BrsrTypes.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Base class for.
*
*  @lib browserengine.lib
*  @since 2.6
*/
class MViewListener
{

  public:
    /**
    * 
    * @since 2.6
    * @param 
    * @return
    */
    virtual void HandleError (TBrowserStatusCode status) = 0;
    virtual void FormattingComplete () = 0;
};

#endif      // MVCVIEWLISTENER_H

// End of File
