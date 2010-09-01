/*
* Copyright (c) 1999 - 2002 Nokia Corporation and/or its subsidiary(-ies).
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
**    File name:  nwx_logger_epoc32.c
**    Description:  Provides the interface to logging
**                  Windows NT & Windows CE log to the DevStudio
**                  debugging window, all others to an I/O stream.
******************************************************************/

/*
**  Include Files
*/
#include <e32def.h>
#include <e32std.h>
#include <eikenv.h>
#include <f32file.h>
#include <flogger.h>
#include "nwx_logger.h"
#include "nwx_ctx.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/*
**  Constants
*/
enum {KMaxLogLen = 1500};

/*
**  File Scoped Structure
*/
typedef struct
  {
  NW_LogLevel_t logLevel;
  NW_Ucs2* logFileName;
  } NW_Logger_Struct_t;

/*
**  External Public (Exported) Functions
*/

#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))

_LIT( KWmlBrowserLogDir, "browser" );

/*#define DEFAULT_LOGFILENAME "default.log"*/

#endif /* (defined (_DEBUG)) || (defined(_ENABLE_LOGS)) */

/*****************************************************************
**  Name:  NW_Debug_NewLogger 
**  Description:  creates a new logger
**  Parameters:   *name -- name of the file to log info in
**                logLev -- level of logging during a session
**  Return Value: void 
**  Note:  not to be used with windows CE
******************************************************************/
void NW_Debug_NewLogger(const char *name, const NW_LogLevel_t logLev)
  {
#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))

  NW_REQUIRED_PARAM(name);
  
  NW_Logger_Struct_t* logger_ctx = 
    (NW_Logger_Struct_t*)NW_Mem_Malloc(sizeof(NW_Logger_Struct_t));
  
  if (logger_ctx == NULL)
    return;

  if (NW_Ctx_Set(NW_CTX_LOGGER, NULL, logger_ctx) != KBrsrSuccess) return;

  /* Set logfile name */
  logger_ctx->logFileName = NW_Str_CvtFromAscii(name);
  NW_ASSERT(logger_ctx->logFileName);

  /* Set logging level */
  logger_ctx->logLevel = logLev;
#else
  NW_REQUIRED_PARAM(name);
  NW_REQUIRED_PARAM(logLev);
#endif /* (defined (_DEBUG)) || (defined(_ENABLE_LOGS)) */

  }



/*****************************************************************
**  Name:  NW_Debug_SetLogLevel 
**  Description:  change the log level
**  Parameters:   logLev -- log level for the logger
**  Return Value: void
******************************************************************/
void NW_Debug_SetLogLevel(const NW_LogLevel_t logLev)
  {
#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))
  
  NW_Logger_Struct_t* logger_ctx = 
    (NW_Logger_Struct_t*)NW_Ctx_Get(NW_CTX_LOGGER, NULL);
  NW_ASSERT(logger_ctx);
  logger_ctx->logLevel = logLev;
  
#else
  NW_REQUIRED_PARAM(logLev);
#endif /* (defined (_DEBUG)) || (defined(_ENABLE_LOGS)) */
  }


/*****************************************************************
**  Name:  NW_Debug_Log 
**  Description:  To log some information
**  Parameters:   msgLevel -- level of the message (this will be 
**                            compared with the log level to 
**                            determine whether a message will
**                            need to be logged)
**                fmt -- format string
**                ... --  a list of parameters to be logged
**  Return Value: void
**
**  NOTE: This method will panic the browser when an extremely
**  long argument (~1K bytes) is presented for logging. This is
**  because the argument (usually url) exceeds the buf size of
**  KMaxLogLen.  Since we don't know the order or type of the
**  incoming va_list (...) we can't perform any size or type checking.
**  Since this is only internal code, used for debugging, it doesn't
**  effect our release code.
******************************************************************/
void NW_Debug_Log(const NW_LogLevel_t msgLevel, const char* fmt, ...)
  {
#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))
  
  /* Get a context. Will not exist if logger has not been started yet. */
  NW_Logger_Struct_t*logger_ctx = (NW_Logger_Struct_t*)NW_Ctx_Get(NW_CTX_LOGGER, NULL);
  if (!logger_ctx)
    return;

  NW_LogLevel_t logLevel = logger_ctx->logLevel;
  
  if (logLevel >= msgLevel)
    {
      VA_LIST ap;
      NW_Ucs2* fmt16;
      fmt16 = NW_Str_CvtFromAscii(fmt);
      if (fmt16 != NULL)
      {
        TBuf16<KMaxLogLen> buf;
        TPtrC16 format(fmt16);

        VA_START(ap, fmt);
        buf.FormatList(format, ap);
        VA_END(ap);
    
        buf.Append(0);

        TPtrC fileName(logger_ctx->logFileName);
        RFileLogger::Write( KWmlBrowserLogDir, fileName , EFileLoggingModeAppend, buf );
        NW_Mem_Free(fmt16);
      }
    }
  
#else
  NW_REQUIRED_PARAM(msgLevel);
  NW_REQUIRED_PARAM(fmt);
#endif /* (defined (_DEBUG)) || (defined(_ENABLE_LOGS)) */
  
  return;
  }


/*****************************************************************
**  Name:  NW_Debug_CloseLogger 
**  Description:  close debugger
**  Parameters:   void 
**  Return Value: void 
**  Note: not to be used for windows CE architecture
******************************************************************/
void NW_Debug_CloseLogger()
  {
#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))
  
  NW_Logger_Struct_t* logger_ctx = 
    (NW_Logger_Struct_t*)NW_Ctx_Get(NW_CTX_LOGGER, NULL);
  
  if (logger_ctx != NULL)
    {
    NW_Mem_Free(logger_ctx->logFileName);
    NW_Mem_Free (logger_ctx);
    NW_Ctx_Set(NW_CTX_LOGGER, NULL, NULL);
    }
#endif /* (defined (_DEBUG)) || (defined(_ENABLE_LOGS)) */
  }

