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
**   File: nwx_logger.h
**   Purpose:  Used for logging (presently will be used for debug routines)
**             (Windows NT & Windows CE will output in the debug windows)
**************************************************************************/
#ifndef NWX_LOGGER_H
#define NWX_LOGGER_H

#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
**---------------------------------------------------------------------------
** Type Declarations
**---------------------------------------------------------------------------
*/
typedef enum {
  NW_LOG_OFF             = 0,  /* No debugging info */
  NW_LOG_LEVEL1          = 1,  /* Minimum amount of debugging info */
  NW_LOG_LEVEL2          = 2,
  NW_LOG_LEVEL3          = 3,
  NW_LOG_LEVEL4          = 4,
  NW_LOG_LEVEL5          = 5   /* Maximum amount of debugging info */
} NW_LogLevel_t;

/*
**---------------------------------------------------------------------------
** Global Function Declarations
**---------------------------------------------------------------------------
*/

// GAF: These 4 exported functions should be always in .DEF file

/* create a new debugging log */
void NW_Debug_NewLogger(const char* name, const NW_LogLevel_t logLev);

/* write formatted list of values to log */
void NW_Debug_Log(const NW_LogLevel_t level, const char* fmt, ...);

/* set logging level (message level must be *less* than log level) */
void NW_Debug_SetLogLevel(const NW_LogLevel_t logLev);

/* close the debugging log */
void NW_Debug_CloseLogger();

/* Uncomment following line if you want to produce logs in release mode */

//#define _ENABLE_LOGS

#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))

/* log string */
void NW_Debug_LogString(const char *message);

/* Macros for Debug Builds */
#define NW_LOG_NEWLOGGER(name, level) NW_Debug_NewLogger(name, level)
#define NW_LOG_SETLOGLEVEL(level) NW_Debug_SetLogLevel(level)
#define NW_LOG_CLOSELOGGER() NW_Debug_CloseLogger()
        
#define NW_LOG0(level, fmt) \
          NW_Debug_Log(level, fmt)
#define NW_LOG1(level, fmt, arg1) \
          NW_Debug_Log(level, fmt, arg1)
#define NW_LOG2(level, fmt, arg1, arg2) \
          NW_Debug_Log(level, fmt, arg1, arg2)
#define NW_LOG3(level, fmt, arg1, arg2, arg3) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3)
#define NW_LOG4(level, fmt, arg1, arg2, arg3, arg4) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4)
#define NW_LOG5(level, fmt, arg1, arg2, arg3, arg4, arg5) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5)
#define NW_LOG6(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
#define NW_LOG7(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
#define NW_LOG8(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
#define NW_LOG9(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
#define NW_LOG10(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
#define NW_LOG11(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11)
#define NW_LOG12(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12) \
          NW_Debug_Log(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12)

#else

/* Macros for Release Builds */
#define NW_LOG_NEWLOGGER(name, level) ((void) 0)
#define NW_LOG_SETLOGLEVEL(level) ((void) 0)
#define NW_LOG_CLOSELOGGER() ((void) 0)

#define NW_LOG0(level, fmt) ((void) 0)
#define NW_LOG1(level, fmt, arg1) ((void) 0)
#define NW_LOG2(level, fmt, arg1, arg2) ((void) 0)
#define NW_LOG3(level, fmt, arg1, arg2, arg3) ((void) 0)
#define NW_LOG4(level, fmt, arg1, arg2, arg3, arg4) ((void) 0)
#define NW_LOG5(level, fmt, arg1, arg2, arg3, arg4, arg5) ((void) 0)
#define NW_LOG6(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6) ((void) 0)
#define NW_LOG7(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7) ((void) 0)
#define NW_LOG8(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) ((void) 0)
#define NW_LOG9(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) ((void) 0)
#define NW_LOG10(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) ((void) 0)
#define NW_LOG11(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) ((void) 0)
#define NW_LOG12(level, fmt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12) ((void) 0)

#endif  /* (defined (_DEBUG)) || (defined(_ENABLE_LOGS)) */



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NWX_LOGGER_H */
