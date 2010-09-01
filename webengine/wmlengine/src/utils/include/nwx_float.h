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
**   File: nwx_float.h
**   Purpose:  Provides the interface to platform-dependant float functions
**************************************************************************/

#ifndef NWX_FLOAT_H
#define NWX_FLOAT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include "nwx_defs.h"
#include "float.h"

/*
** Type Declarations
*/

/*
** Global Function Declarations
*/
extern NW_Bool is_finite_float64(NW_Float64 d);

extern NW_Bool is_zero_float64(NW_Float64 d);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_FLOAT_H */
