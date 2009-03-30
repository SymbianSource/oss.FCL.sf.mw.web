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
**   File: nwx_defs.h
**   Purpose:  Contains definitions common to all components. 
**              Also contains defs for all of the Basic Scalar types
**************************************************************************/
#ifndef NWX_DEFS_H
#define NWX_DEFS_H

/*
**----------------------------------------------------------------------
** Preprocessor Macro Definitions
**----------------------------------------------------------------------
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
**----------------------------------------------------------------------
** Includes
**----------------------------------------------------------------------
*/

/*
 *  ANSI/ISO C header files needed to define basic data types
 * (ie. NULL, etc)
 */

#include <stdlib.h>


/*
**----------------------------------------------------------------------
** Type Declarations
**----------------------------------------------------------------------
*/

#define _NW_STR(str) L ## str
#define NW_STR(str) _NW_STR(str)

/* New type definitions with non-conflicting name scheme */
typedef unsigned char           NW_Bool;        /* NW_FALSE, NW_TRUE  */
typedef unsigned char           NW_Byte;        /* 0 ... 0xFF         */
typedef unsigned short          NW_Word;        /* 0 ... 0xFFFF       */
typedef unsigned long           NW_Dword;       /* 0 ... 0xFFFFFFFF   */

typedef unsigned char           NW_Uint8;       /* 0 ... 255          */
typedef unsigned short          NW_Uint16;      /* 0 ... 65535        */
typedef unsigned long           NW_Uint32;      /* 0 ... 4294967295   */

typedef signed   char           NW_Int8;        /* -128 ... +127      */
typedef signed   short          NW_Int16;       /* -32768 ... +32767  */
typedef signed   long           NW_Int32;       /* -2147483648 ... +2147483647 */

typedef float                   NW_Float32;
typedef double                  NW_Float64;

typedef unsigned short          NW_Ucs2;        /* 0 ... 65535        */
typedef unsigned long           NW_Ucs4;        /* 0 ... 4294967295   */

/*This type can hold both TBrowserStatusCode and http status*/
typedef NW_Int32      NW_WaeError_t;
typedef NW_Uint16     NW_WaeHttpStatus_t;


#define NW_TRUE         ((NW_Bool)  0x01U)
#define NW_FALSE        ((NW_Bool)  0x00U)

#define NW_UINT8_MIN    ((NW_Uint8)  0x00U)
#define NW_UINT8_MAX    ((NW_Uint8)  0xFFU)

#define NW_UINT16_MIN   ((NW_Uint16) 0x0000U)
#define NW_UINT16_MAX   ((NW_Uint16) 0xFFFFU)

#define NW_UINT32_MIN   ((NW_Uint32) 0x00000000U)
#define NW_UINT32_MAX   ((NW_Uint32) 0xFFFFFFFFU)

#define NW_INT8_MIN     ((NW_Int8)   0x80U)
#define NW_INT8_MAX     ((NW_Int8)   0x7FU)

#define NW_INT16_MIN    ((NW_Int16)  0x8000U)
#define NW_INT16_MAX    ((NW_Int16)  0x7FFFU)

#define NW_INT32_MIN    ((NW_Int32)  0x80000000U)
#define NW_INT32_MAX    ((NW_Int32)  0x7FFFFFFFU)

/* 
 * Definitions needed by the script engine for 
 * performing IEEE floating point math.
 * These *should* be platform independent constants defined
 * by the IEEE specifications, but in all cases they
 * can be found in the ANSI/ISO C header file <float.h> 
 */
#define NW_FLOAT32_DIG      FLT_DIG
#define NW_FLOAT32_MIN      FLT_MIN
#define NW_FLOAT32_MAX      FLT_MAX
#define NW_FLOAT32_MIN_EXP  FLT_MIN_10_EXP
#define NW_FLOAT32_MAX_EXP  FLT_MAX_10_EXP

#define NW_MIN(x, y) ( ((x) < (y)) ? (x) : (y) )
#define NW_MAX(x, y) ( ((x) > (y)) ? (x) : (y) )

/*
 * When casting to unsigned types, the comparison "-expr <= 0" is used
 * rather than  "expr >= 0" to prevent warnings about unnecessary
 * comparisons on Epoc hardware.
 */
#define NW_UINT8_CAST(expr) \
        ( (NW_ASSERT((-(NW_Int32)(expr)) <= 0 && \
                     ((NW_Uint32)(expr)) <= NW_UINT8_MAX)), \
          ((NW_Uint8)(expr)) )

#define NW_UINT16_CAST(expr) \
        ( (NW_ASSERT((-(NW_Int32)(expr)) <= 0 && \
                     ((NW_Uint32)(expr)) <= NW_UINT16_MAX)), \
          ((NW_Uint16)(expr)) )

#define NW_UINT32_CAST(expr) \
        ( (NW_ASSERT((-(NW_Int32)(expr)) <= 0 && \
                     ((NW_Uint32)(expr)) <= NW_UINT32_MAX)), \
          ((NW_Uint32)(expr)) )

#define NW_INT8_CAST(expr) \
        ( (NW_ASSERT(((NW_Int32)(expr)) >= NW_INT8_MIN && \
                     ((NW_Int32)(expr)) <= NW_INT8_MAX)), \
          ((NW_Int8)(expr)) )

#define NW_INT16_CAST(expr) \
        ( (NW_ASSERT(((NW_Int32)(expr)) >= NW_INT16_MIN && \
                     ((NW_Int32)(expr)) <= NW_INT16_MAX)), \
          ((NW_Int16)(expr)) )

#define NW_INT32_CAST(expr) \
        ( (NW_ASSERT(((NW_Int32)(expr)) >= NW_INT32_MIN && \
                     ((NW_Int32)(expr)) <= NW_INT32_MAX)), \
          ((NW_Int32)(expr)) )

#define NW_BOOL_CAST(expr) \
        ( ((NW_Bool)(((NW_Int32)(expr) != 0) ? NW_TRUE : NW_FALSE)) )

/* 
 * Used to supress compiler warnings about required, 
 * but unused function parameters 
 */
#define NW_REQUIRED_PARAM(param) if (param) {};

/* NW_Opaque_t is used for passing dynamically typed
   values between the Script engine and the browser.  */
  
typedef enum 
{
  NW_OPAQUE_VALUE_TYPE_INT32,
  NW_OPAQUE_VALUE_TYPE_STRING,
  NW_OPAQUE_VALUE_TYPE_BOOL,
  NW_OPAQUE_VALUE_TYPE_FLOAT,
  NW_OPAQUE_VALUE_TYPE_INVALID
} NW_Opaque_Value_Type_t;

typedef struct _NW_Opaque_t NW_Opaque_t;

/* Note: use "volatile" keyword to suppress compiler optimizer bug */
/*       which changes store/load operation into a data-type cast */
struct _NW_Opaque_t {
  volatile union {
    NW_Ucs2     *s;
    NW_Int32    i;
    NW_Float32  f;
    NW_Bool     b;
  } value;
  NW_Opaque_Value_Type_t type;
};

/* Facilitates run-time error checking */
#define NW_THROWIF_MEM(_call) {if ((_call) == NULL) goto _NW_CATCH_MEM;}
#define NW_THROW_ERROR() { goto _NW_CATCH_ERROR;}
#define NW_THROWIF(_call) {if ((_call)) goto _NW_CATCH_ERROR;}
#define NW_THROWIF_ERROR(_call) {if ((_call) != KBrsrSuccess) goto _NW_CATCH_ERROR;}
#define NW_THROWIF_NULL(_call) {if ((_call) == NULL) goto _NW_CATCH_ERROR;}

#define NW_CATCH_MEM _NW_CATCH_MEM:
#define NW_CATCH_ERROR _NW_CATCH_ERROR:

/*
**----------------------------------------------------------------------
** Includes
**----------------------------------------------------------------------
*/
#include "nwx_assert.h"
#include "nwx_mem.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
#define NW_HED_UrlRequest_Reason_Undefined     0  /* Shouldn't be used */
#define NW_HED_UrlRequest_Reason_ShellLoad     1  /* used by goto and bookmark dialogs */ 
#define NW_HED_UrlRequest_Reason_ShellPrev     2  /* used by history */
#define NW_HED_UrlRequest_Reason_ShellNext     3  /* used by history */
#define NW_HED_UrlRequest_Reason_ShellReload   4  /* used by history */
#define NW_HED_UrlRequest_Reason_DocLoad       5  /* used by content-handlers to load new top-level documents */
#define NW_HED_UrlRequest_Reason_DocPrev       6  /* used by Wml1x content-handler to load new top-level document as the result of the PREV task */
#define NW_HED_UrlRequest_Reason_DocLoadChild  7  /* used by content-handlers to load embedded documents */
#define NW_HED_UrlRequest_Reason_RestorePrev   8  /* used to restore a failed page */
#define NW_HED_UrlRequest_Reason_RestoreNext   9  /* used to restore a failed page */
#define NW_HED_UrlRequest_Reason_DocLoadHead   10 


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_DEFS_H */



