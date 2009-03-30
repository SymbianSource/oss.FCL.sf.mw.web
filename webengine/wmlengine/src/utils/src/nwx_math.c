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
**  File name:  nwx_math.c
**  Part of: Debug
**  Version: 1.0
**  Description: Provides interfaces to ANSI standard math functions.
******************************************************************/

#include "nwx_assert.h"
#include "nwx_defs.h"
#include "nwx_float.h"
#include "nwx_ctx.h"
#include "nwx_math.h"

/* abramowitz & stegun, 26.8.3 */

#define DEFAULT_SEED_VAL  47594118
#define T                 100000001
#define A                 23
#define B                 0

/* Cannot have static variables in code for Epoc. 
**  ie. static NW_Int32 currRand = DEFAULT_SEED_VAL; 
*/
typedef struct {
  NW_Int32 currRand;
} Math_Context_t;

static Math_Context_t *Math_Initialize(void);
static NW_Int32 Math_GetCurrRandom(void);
static Math_Context_t *Math_ContextGet(void);
static void Math_SaveCurrRandom(NW_Int32 currRand);
static NW_Int32 Math_PowerInt(NW_Int32 x, NW_Int32 ex);

static Math_Context_t *Math_Initialize(void)
{
  Math_Context_t *context = (Math_Context_t*) NW_Mem_Malloc(sizeof(Math_Context_t));
  if (context != NULL) {
    context->currRand = DEFAULT_SEED_VAL;
  }
  return context;
}

static Math_Context_t *Math_ContextGet(void)
{
  Math_Context_t *context = (Math_Context_t *)NW_Ctx_Get(NW_CTX_UTILS_MATH, 0);
  if (context == NULL) {
    context = Math_Initialize();
    NW_Ctx_Set(NW_CTX_UTILS_MATH, 0, (void*)context);
  }
  return context;
}

void Math_ContextFree(void)
{
  Math_Context_t *context = (Math_Context_t *)NW_Ctx_Get(NW_CTX_UTILS_MATH, 0);
  NW_Mem_Free(context);
  NW_Ctx_Set(NW_CTX_UTILS_MATH, 0, NULL);
}

static NW_Int32 Math_GetCurrRandom(void)
{
  Math_Context_t *context = Math_ContextGet();
  NW_ASSERT(context != NULL);
  return context->currRand;
}

static void Math_SaveCurrRandom(NW_Int32 currRand)
{
  Math_Context_t *context = Math_ContextGet();
  NW_ASSERT(context != NULL);
  context->currRand = currRand;
}

static NW_Int32 Math_PowerInt(NW_Int32 x, NW_Int32 ex)
{
  NW_Int32 res = 1;
  NW_Int32 factor = x;

  if (ex < 0) {
    ex = -ex;
  }

  while (ex > 0) {
    res *= factor;
    ex--;
  }
  return res;
}


void NW_Math_srand(NW_Int32 seed)
{
  if (seed > 0) {
    Math_SaveCurrRandom(seed);
  } else {
    Math_SaveCurrRandom(DEFAULT_SEED_VAL);
  }
}

NW_Int32 NW_Math_rand()
{
  NW_Int32 currRand = Math_GetCurrRandom();

  currRand = (A * currRand + B) % T;
  if (currRand < 0) {
    currRand = -currRand;
  }
  Math_SaveCurrRandom(currRand);
  return currRand;
}
  
NW_Int32 NW_Math_truncate(NW_Float32 x)
{
  /*lint --e{785, 747} Too few initializers, Significant prototype coercion*/
  NW_Float32 y; /* y is temp */
  NW_Float32 temp = 0.0;
  NW_Float32 factor = 0.0;
  NW_Float32 array [10] = {0.0};
  NW_Int32 answer = 0;
  NW_Int32 res = 0;
  NW_Int32 count = 0;
  NW_Int32 z = 0;

  if (x < 0.0) {
    y = -x;
  } else {
    y =  x;
  }

  /* we assume # of bits for mantissa/significand of double is less than */
  /* # of bits in int */
  while (y >= 1.0) {
    y /= 10.0;
    count++;
  }

  if (x < 0.0) {
    y = -x;
  } else {
    y =  x;
  }

  while (count > 0) {
    factor = NW_Math_powint(10.0, count-1);
    temp = array [res];
    for (z=0; z<10; ++z) {
      array [z] = (factor * z) + temp;
    }
    if (y < array [1]) {
      res = 0;
    } else if (y < array [2]) {
      res = 1;
    } else if (y < array [3]) {
      res = 2;
    } else if (y < array [4]) {
      res = 3;
    } else if (y < array [5]) {
      res = 4;
    } else if (y < array [6]) {
      res = 5;
    } else if (y < array [7]) {
      res = 6;
    } else if (y < array [8]) {
      res = 7;
    } else if (y < array [9]) {
      res = 8;
    } else {
      res = 9;
    }
    count--;
    answer = answer + (Math_PowerInt(10, count) * res);
  }
  if (x < 0.0) {
    answer = -answer;
  }
  return answer;
}

NW_Float32 NW_Math_powint(NW_Float32 x, NW_Int32 ex)
{
  NW_Float32 res = 1.0;
  NW_Float32 factor = ( (ex > 0) ? x : 1/x );

  if (ex < 0) {
    ex = -ex;
  }

  while (ex > 0) {
    res *= factor;
    ex--;
  }
  return res;
}

NW_Float64 NW_Math_powint64(NW_Float32 x, NW_Int32 ex)
{
  NW_Float64 res = 1.0;
  NW_Float64 factor = ( (ex > 0) ? (NW_Float64)x : (NW_Float64)1/x );

  if (ex < 0) {
    ex = -ex;
  }

  while (ex > 0) {
    res *= factor;
    ex--;
  }
  return res;
}

/* strategy - rewrite x^y as 10^(y+y') where x = 10^y' => y' = Log(x)   */
/* Log(x) given by abramowitz & stegun 4.1.42 for 10^-0.5 <= x <= 10^.5 */
/* and 10^(y+y') given by abramowitxz & stegun 4.2.47.  Highest order   */
/* error term is 10^-7, just on the edge of the spec for fp precision   */
/* in wmlscript. */
NW_Float32 NW_Math_pow(NW_Float32 x, NW_Float32 y)
{
  NW_Float32 xrange = x;
  NW_Int32 fac = 0;
  NW_Float32 t, t2, t1;
  NW_Float32 res = 1;
  NW_Float32 yprime;
  NW_Float32 fracpow;
  
  if (x < 0) {
    return 0.0;
  }
  while (xrange > 3.126227766) {
    xrange /= 10.0;
    fac++;
  }
  while (xrange < 0.3126227766) {
    xrange *= 10.0;
    fac--;
  }

  t1 = t = (xrange - 1)/(xrange + 1);
  t2 = t1*t1;
  yprime = ((NW_Float32)0.868591718)*t;
  t *= t2; /* t^3 */
  yprime += ((NW_Float32)0.289335524)*t;
  t *= t2; /* t^5 */
  yprime += ((NW_Float32)0.177522071)*t;
  t *= t2; /* t^7 */
  yprime += ((NW_Float32)0.094376476)*t;
  t *= t2;
  yprime += ((NW_Float32)0.191337714)*t;

  yprime += fac; /* log(ab) = log(a) + log(b) */
  yprime *= y;

  if (yprime > 1.0) {
    res = 10.0;
    yprime -= 1.0;
    while (yprime > 1.0) {
      res *= 10.0;
      yprime -= 1.0;
    }
  } else {
    res = 1.0;
    while (yprime < 0.0) {
      res *= (NW_Float32)0.1;
      yprime += (NW_Float32)1.0;
    }
  }
  
  t = yprime;
  fracpow = (NW_Float32)(1.0 + 1.15129277603*t);
  t *= yprime;
  fracpow += ((NW_Float32)0.66273088429)*t;
  t *= yprime;
  fracpow += ((NW_Float32)0.25439357484)*t;
  t *= yprime;
  fracpow += ((NW_Float32)0.07295173666)*t;
  t *= yprime;
  fracpow += ((NW_Float32)0.01742111988)*t;
  t *= yprime;
  fracpow += ((NW_Float32)0.00255491796)*t;
  t *= yprime;
  fracpow += ((NW_Float32)0.00093264267)*t;
  
  return res * fracpow * fracpow;
}
         
NW_Int32 NW_Math_abs(NW_Int32 i)
{
  if (i < 0) {
    return -i;
  } else {
    return i;
  }
}

NW_Float32 NW_Math_floor(NW_Float32 x)
{
  NW_Float32 y; 
  NW_Float32 digit;
  NW_Float32 res = 0;
  NW_Int32 count = 0;

  if (x < 0.0) {
    y = -x;
  } else {
    y = x;
  }

  /* we assume # of bits for mantissa/significand of double is less than */
  /* # of bits in int */
  while (y >= 1.0) {
    y /= 10.0;
    count++;
  }
  y *= 10;
  while (count > 0) {
    res = 10.0f*res;
    if (y < 1.0) {
      digit = 0.0;
    } else if (y < 2.0) {
      digit = 1.0;
    } else if (y < 3.0) {
      digit = 2.0;
    } else if (y < 4.0) {
      digit = 3.0;
    } else if (y < 5.0) {
      digit = 4.0;
    } else if (y < 6.0) {
      digit = 5.0;
    } else if (y < 7.0) {
      digit = 6.0;
    } else if (y < 8.0) {
      digit = 7.0;
    } else if (y < 9.0) {
      digit = 8.0;
    } else {
      digit = 9.0;
    }
    res += digit;
    count--;
    y = (y - digit)*10.0f;
  }
  if (x < 0.0) {
    res = -res-1;
  }
  return res;
}

NW_Float32 NW_Math_ceil(NW_Float32 x)
{
  NW_Float32 xFloor = NW_Math_floor(x);
  /* if x starts out an even integer then x = floor(x) = ceil(x) */
  if (xFloor < x) {
    return xFloor + 1;
  } else {
    return xFloor;
  }
}

NW_Float32 NW_Math_fabs(NW_Float32 x)
{
  if (x < 0) {
    return -x;
  } else {
    return x;
  }
}

NW_Int32 NW_Math_truncate64(NW_Float64 x)
{
  NW_Float64 y; 
  NW_Float64 digit;
  NW_Int32 res = 0;
  NW_Int32 prevRes = 0;
  NW_Int32 count = 0;

  if (x < 0.0) {
    y = -x;
  } else {
    y = x;
  }

  /* we assume # of bits for mantissa/significand of double is less than */
  /* # of bits in int */
  while (y >= 1.0) {
    y /= 10.0;
    count++;
  }
  y *= 10.0;
  while (count > 0) {
    prevRes = res;
    res = 10*res;
    if (y < 1.0) {
      digit = 0.0;
    } else if (y < 2.0) {
      res += 1;
      digit = 1.0;
    } else if (y < 3.0) {
      res += 2;
      digit = 2.0;
    } else if (y < 4.0) {
      res += 3;
      digit = 3.0;
    } else if (y < 5.0) {
      res += 4;
      digit = 4.0;
    } else if (y < 6.0) {
      res += 5;
      digit = 5.0;
    } else if (y < 7.0) {
      res += 6;
      digit = 6.0;
    } else if (y < 8.0) {
      res += 7;
      digit = 7.0;
    } else if (y < 9.0) {
      res += 8;
      digit = 8.0;
    } else {
      res += 9;
      digit = 9.0;
    }
    if (res < prevRes) {
      /* figure out correct error behavior, ERANGE? */
    }
    count--;
    y = (y - digit)*10.0;
  }
  if (x < 0.0) {
    res = -res;
  }
  return res;
}

void NW_Math_BaseandExp64(NW_Float64 x, NW_Int32 *base, NW_Int32 *exp)
{
  NW_Int32 sign = 1;
  NW_Int32 i;
  
  NW_ASSERT(base != NULL);
  NW_ASSERT(exp != NULL);

  *exp = 0;
  if (x < 0) {
    sign = -1;
    x = -x;
  }
  if (x != 0.0f) {
    while (x >= 1.0) {
      x *= 0.1f;
      *exp += 1;
    }
    while (x < 0.1) {
      x *= 10.0f;
      *exp -= 1;
    }
  }
  for (i=0; i<=FLT_DIG; i++) {
    x *= 10.0f;
  }
  *base = sign*NW_Math_truncate64(x);
}

NW_Float32 NW_Math_round(NW_Float32 x)
{
  return NW_Math_floor((NW_Float32)(x + 0.5));
}

