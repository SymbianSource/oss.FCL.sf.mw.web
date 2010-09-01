/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef WMLENGINEDEFS_H
#define WMLENGINEDEFS_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class TWmlEngineDefs
{
public:

enum THistoryStackPosition
	{
     EHistoryAtTheBeginning,
     EHistoryInTheMiddle,
     EHistoryAtTheEnd
	};

enum TWMLOptions
	{
     WML_OPTION_UNKNOWN = 1,
	   WML_OPTION_ACCEPT  = 2,
     WML_OPTION_PREV    = 3,
     WML_OPTION_HELP    = 4,
	   WML_OPTION_RESET   = 5,
     WML_OPTION_OPTIONS = 6,
     WML_OPTION_DELETE  = 7
	};
};

#endif      // WMLENGINEDEFS_H           
// End of File
