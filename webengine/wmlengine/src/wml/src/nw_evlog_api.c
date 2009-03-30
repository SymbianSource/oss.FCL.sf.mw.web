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
**    File name:  nw_evlog_api.c
**    Part of: WML Interpreter
**    Description:  Provides an interface to event log api.
******************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nw_evlog_api.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/
/*****************************************************************
  Name: NW_EvLog_Log
  
  Description: Called to log events in content handlers.  Call the
               appropriate logging function in event log API based
               on the index argument.
               
  Parameters:
    evLogApi - in: data structure that contains function pointers
                   to logging functions.
    index - in: index of logging function to call.
    
    ... - in: any other parameters for the specific logging function.
    
  Return Value:
    None.  Since this is a logging function, it returns without logging
    if it encounters any problems.  We don't want logging to bring down
    the system.
******************************************************************/
void NW_EvLog_Log( const NW_EvLogApi_t* evLogApi, NW_EvLog_Index_t index, ... )
{
  /*lint --e{415, 416} Likely creation of out-of-bounds pointer, Likely access of out-of-bounds pointer*/
  va_list argp;

  if ( evLogApi == NULL )
  {
    /* No functions to call.  Just return. */
    return;
  }

  va_start(argp, index);
  
  switch (index) {
  case NW_EvLog_CONTENT_REQUEST:
      //Requests are logged in the loaders
    break;
  
  case NW_EvLog_CONTENT_RECEIVED:
      //Responses are logged in the loaders
    break;
  
  case NW_EvLog_VARIABLE_CONTEXT_NEW:
    if ( evLogApi->logVariableContextNew != NULL ) {
      (void)evLogApi->logVariableContextNew();
    }
    break;
  
  case NW_EvLog_HISTORY_STACK_CLEAR:
    if ( evLogApi->logHistoryStackClear != NULL ) {
      (void)evLogApi->logHistoryStackClear( );
    }
    break;
  
  case NW_EvLog_HISTORY_PUSH:
    if ( evLogApi->logHistoryPush != NULL ) {
      (void)evLogApi->logHistoryPush( va_arg(argp, NW_Ucs2*) );
    }
    break;
  
  case NW_EvLog_HISTORY_POP:
    if ( evLogApi->logHistoryPop != NULL ) {
      (void)evLogApi->logHistoryPop( );
    }
    break;
  
  case NW_EvLog_HISTORY_BACK:
    if ( evLogApi->logHistoryBack != NULL ) {
      (void)evLogApi->logHistoryBack( );
    }
    break;
  
  case NW_EvLog_HISTORY_FORWARD:
    if ( evLogApi->logHistoryForward != NULL ) {
      (void)evLogApi->logHistoryForward( );
    }
    break;
  
  case NW_EvLog_SET_VAR:
    if ( evLogApi->logSetVar != NULL ) {
      NW_Ucs2* name = va_arg(argp, NW_Ucs2*);
      NW_Ucs2* value = va_arg(argp, NW_Ucs2*);
      (void)evLogApi->logSetVar( name, value );
    }
    break;
  
  case NW_EvLog_CACHE_ADD:
    if ( evLogApi->logCacheAdd != NULL ) {
      (void)evLogApi->logCacheAdd(va_arg(argp, NW_Ucs2*));
    }
    break;
  
  case NW_EvLog_CACHE_REMOVE:
    if ( evLogApi->logCacheRemove != NULL ) {
      (void)evLogApi->logCacheRemove(va_arg(argp, NW_Ucs2*));
    }
    break;
  
  case NW_EvLog_CACHE_CLEAR:
    if ( evLogApi->logCacheClear != NULL ) {
      (void)evLogApi->logCacheClear( );
    }
    break;
  
  default:
    break;  /* Just return if invalid index. */
  }

  va_end(argp);
  return;
}
