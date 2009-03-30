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


 /*
    $Workfile: nwx_float_epoc32.c $

     Purpose:
        
          Contains platform-dependant float functions
 */

#include <math.h>
#include "nwx_float.h"

/*
 * This doesn't work on the Epoc hardware - any attempt to
 * access an invalid float causes a crash.
NW_Bool is_finite_float( NW_Float32 f ) 
{
  * remove sign bit and fraction part *
  NW_Uint32 fBits =  ( 0x7F800000 & *((NW_Uint32 *)(&f)) ); 
  
  * if exponent is 8 ones, it is either NaN or  plus/minus infinity *
  return NW_BOOL_CAST(fBits != 0x7F800000);
}
*/

/* Test to see if this NW_Float64 is a valid 32 bit float */
NW_Bool is_finite_float64(NW_Float64 d) 
{
  return NW_BOOL_CAST(fabs(d) < NW_FLOAT32_MAX);
}

/* Test to see if this NW_Float64 is smaller than can be represented
 * in a 32 bit float
 */
NW_Bool is_zero_float64(NW_Float64 d) 
{
  return NW_BOOL_CAST(fabs(d) < NW_FLOAT32_MIN);
}

