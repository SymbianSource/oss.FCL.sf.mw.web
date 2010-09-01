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


/***************************************************************************
**   File: nwx_math.h
**   Purpose:  Provides the interface to platform-independent math functions
**************************************************************************/

#ifndef NWX_MATH_H
#define NWX_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include "nwx_defs.h"

/*
** Type Declarations
*/

/*
** Global Function Declarations
*/
extern void Math_ContextFree();

extern NW_Float32 NW_Math_powint(NW_Float32 x, NW_Int32 ex);

extern NW_Float64 NW_Math_powint64(NW_Float32 x, NW_Int32 ex);

extern NW_Int32 NW_Math_rand();

extern void NW_Math_srand(NW_Int32 seed);

extern NW_Float32 NW_Math_floor(NW_Float32 x);

extern NW_Float32 NW_Math_ceil(NW_Float32 x);

extern NW_Int32 NW_Math_truncate(NW_Float32 x);

extern NW_Int32 NW_Math_abs(NW_Int32 x);

extern NW_Float32 NW_Math_fabs(NW_Float32 x);

extern NW_Float32 NW_Math_pow(NW_Float32 x, NW_Float32 pow);

extern void NW_Math_BaseandExp64(NW_Float64 x, NW_Int32 *base, NW_Int32 *exp);

extern NW_Float32 NW_Math_round(NW_Float32 x);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_MATH_H */
