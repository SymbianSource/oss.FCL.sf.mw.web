/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/*****************************************************************
**    File name:  nwx_assert.c
**    Description:   Provides re[placement for assert
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <e32def.h>
/*lint --e{1065} Symbol not declared as "C" conflicts */

// Debug builds use the _BREAKPOINT() macro provided by E32def.h
// It provides complier-specific assembly instructions for Wins and WinsCW.
// We use this function in the NW_ASSERT macro, because raw assembly generates a syntax error.

#if defined(__WINS__)
int kimono_assert (void)
{
#ifdef _DEBUG
    __BREAKPOINT()
#endif
    return 0;
}
#endif // WINS
