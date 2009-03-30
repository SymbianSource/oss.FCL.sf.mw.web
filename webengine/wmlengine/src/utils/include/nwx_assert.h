/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


/***************************************************************************
**   File: nwx_assert.h
**   Purpose:  Provides interfaces to support assertions  in a platform 
**             independent  manner. 
**************************************************************************/
#ifndef NWX_ASSERT_H
#define NWX_ASSERT_H

/*
** Includes
*/
#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** Global Function Declarations
*/



#ifdef _DEBUG
#if defined(__WINS__)
    // Debug builds use the _BREAKPOINT() macro provided by E32def.h
    // It provides complier-specific assembly instructions for Wins and WinsCW.
    // We use a function in the NW_ASSERT macro, because raw assembly generates a syntax error.
    int kimono_assert (void);
    #define NW_ASSERT(expr) (void)( (expr) || kimono_assert() )
#else /* THUMB */
    #include <assert.h>
    #define NW_ASSERT(expr) assert(((NW_Uint32)(expr)) != 0)
#endif // __WINS__

#else
    /* Release builds for all configurations evaluate to  */
    /* something the compiler optimizer can throw away.   */
    #define NW_ASSERT(expr) ((void)0)

#endif



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_ASSERT_H */


