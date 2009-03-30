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
    $Workfile: wml_history.h $

    Purpose:

        Class: WmlBrowser

        Maintain browser's history state (interface file)
   
*/

#ifndef WML_HISTORY_H
#define WML_HISTORY_H


#include "wml_core.h"

#include "nwx_mem.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* Delete a history entry object */
void NW_Wml_HistoryDeleteEntry(NW_Wml_History_t *entry);

/* Init the history stack */
void NW_Wml_HistoryInit(NW_Wml_t *wml);

/* Clear the history stack, but keep last-load and resp Url */
void NW_Wml_HistoryClear(NW_Wml_t *wml);

TBrowserStatusCode NW_Wml_HistoryPushIntraDeckLoad(NW_Wml_t *wml, 
                                           NW_Ucs2  *url,
                                           NW_Ucs2  *respUrl);

/* Pop a history entry from the stack */
TBrowserStatusCode NW_Wml_HistoryToPrev(NW_Wml_t *wml, NW_Wml_History_t **entry);

/* Store the current response Url */
TBrowserStatusCode NW_Wml_HistorySetResponseUrl(NW_Wml_t* wml, 
                                         NW_Ucs2 *resp_url);

/* Return the current request Url */
TBrowserStatusCode NW_Wml_HistoryGetRequestUrl(NW_Wml_t* wml,
                                        NW_Ucs2 **req_url);

/* Return the current response Url */
TBrowserStatusCode NW_Wml_HistoryGetResponseUrl(NW_Wml_t* wml,
                                         NW_Ucs2 **resp_url);
/* Return the previous response Url */
TBrowserStatusCode NW_Wml_HistoryGetPrevResponseUrl(NW_Wml_t* wml,
                                             NW_Ucs2 **resp_url); 

/* Return the previous history entry */
TBrowserStatusCode NW_Wml_HistoryPrevCard(NW_Wml_t* wml, 
                                   NW_Wml_History_t **entry);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* WML_HISTORY_H */
