/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event log API interface.
*
*/


#ifndef NW_EVLOG_API_H
#define NW_EVLOG_API_H

/*
** Includes
*/
#include <stdarg.h>
#include "urlloader_urlresponse.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** Type Declarations
*/
typedef struct NW_EvLogApi_s NW_EvLogApi_t;

struct NW_EvLogApi_s {
  /* Log new context event (affects global vars only). */
  TBrowserStatusCode (*logVariableContextNew)(void);
  /* Log clearing the history stack event (right now affect WML history stack
     only). */
  TBrowserStatusCode (*logHistoryStackClear)(void);
  /* Log pushing an entry onto the history stack event. */
  TBrowserStatusCode (*logHistoryPush)(NW_Ucs2* url);
  /* Log poping an entry onto the history stack event. */
  TBrowserStatusCode (*logHistoryPop)(void);
  /* Log history back event. */
  TBrowserStatusCode (*logHistoryBack)(void);
  /* Log history forward event. */
  TBrowserStatusCode (*logHistoryForward)(void);
  /* Log setting variable event. */
  TBrowserStatusCode (*logSetVar)(NW_Ucs2* name, NW_Ucs2* value);
  /* Log cache add. */
  TBrowserStatusCode (*logCacheAdd)(NW_Ucs2* url);
  /* Log cache remove. */
  TBrowserStatusCode (*logCacheRemove)(NW_Ucs2* url);
  /* Log cache clear */
  TBrowserStatusCode (*logCacheClear)(void);
};

typedef enum {
  NW_EvLog_INVALID,
  NW_EvLog_FIRST,
  NW_EvLog_CONTENT_REQUEST = NW_EvLog_FIRST,
  NW_EvLog_CONTENT_RECEIVED,
  NW_EvLog_VARIABLE_CONTEXT_NEW,
  NW_EvLog_HISTORY_STACK_CLEAR,
  NW_EvLog_HISTORY_PUSH,
  NW_EvLog_HISTORY_POP,
  NW_EvLog_HISTORY_BACK,
  NW_EvLog_HISTORY_FORWARD,
  NW_EvLog_SET_VAR,
  NW_EvLog_CACHE_ADD,
  NW_EvLog_CACHE_REMOVE,
  NW_EvLog_CACHE_CLEAR,
  NW_EvLog_LAST = NW_EvLog_CACHE_CLEAR
} NW_EvLog_Index_t;

/*
** Global Function Declarations
*/

void NW_EvLog_Log( const NW_EvLogApi_t* evLogApi, NW_EvLog_Index_t index, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NW_EVLOG_API_H */
