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
**   File: nw_errnotify.h
**   Subsystem Name: nwx_wae
**   Purpose:  Provides interface to Errors
**************************************************************************/
#ifndef NW_ERRNOTIFY_H
#define NW_ERRNOTIFY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"
/*
** Includes
*/

/*
** Type Declarations
*/
typedef struct _NW_ErrorApi_t     NW_ErrorApi_t;

/* The callback to call when done displaying a error dialog */
typedef TBrowserStatusCode (NW_ErrorCB_t)(void *callback_ctx, 
                                   TBrowserStatusCode status);

typedef TBrowserStatusCode (*NW_NotifyError_t) (void *wae, const TBrowserStatusCode status,
                                         void *callback_ctx,
                                         NW_ErrorCB_t *callback);

typedef TBrowserStatusCode (*NW_HttpError_t) (void *wae, const NW_WaeHttpStatus_t httpError,
                                       const NW_Ucs2 *uri,
                                       void *callback_ctx,
                                       NW_ErrorCB_t *callback);


struct _NW_ErrorApi_t {
  /* notify the user that an error has occured */
  NW_NotifyError_t notifyError;
  NW_HttpError_t   httpError;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NW_ERRNOTIFY_H */
