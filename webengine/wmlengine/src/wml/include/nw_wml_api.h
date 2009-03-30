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


#ifndef NW_WML_API_H
#define NW_WML_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"
#include "nw_wml_decoder.h"
#include "nw_wml_defs.h"
#include "BrsrStatusCodes.h"

/* interface to the display */
struct _NW_DisplayApi_t
{
  /* create the card */
  TBrowserStatusCode (*createCard) (void *usrAgent);  
  /* show the card */
  TBrowserStatusCode (*showCard) (void *usrAgent);    
  /* destroy the card */
  TBrowserStatusCode (*destroyCard) (void *usrAgent);
  /* add an element to the card */
  TBrowserStatusCode (*addElement) (void *usrAgent, NW_Wml_ElType_e elemType, NW_Int16 *elemId);
  /* clean up the display. indicates that a event has happened*/
  TBrowserStatusCode (*cleanUp) (void *usrAgent);
  /* get the state of the option element */
  TBrowserStatusCode (*getOptState) (void *usrAgent, NW_Uint16 elemId, NW_Bool *st);
  /* set the state of the option element */
  TBrowserStatusCode (*setOptState) (void *usrAgent, NW_Uint16 elemId, NW_Bool st);
  /* refresh the display */
  TBrowserStatusCode (*refresh) (void *usrAgent);
};

/* interface to timers */
struct _NW_TimerApi_t
{
  /* create the timer */
  TBrowserStatusCode (*create) (void *usrAgent, NW_Uint32 period);  
  /* read the time remaining in the timer */
  TBrowserStatusCode (*read) (void *usrAgent, NW_Uint32 *period);
  /* destroy the timer */
  TBrowserStatusCode (*destroy) (void *usrAgent);                
  /* resume the timer */
  TBrowserStatusCode (*resume) (void *usrAgent);                 
  /* stop the timer */
  TBrowserStatusCode (*stop) (void *usrAgent);                   
  /* check if timer is running */
  TBrowserStatusCode (*isRunning) (void *usrAgent, NW_Bool *isRunning);
};

typedef struct _NW_DisplayApi_t NW_DisplayApi_t;
typedef struct _NW_TimerApi_t   NW_TimerApi_t;

typedef struct _NW_WmlApi_t     NW_WmlApi_t;
struct _NW_WmlApi_t
{
  /* display routines */
  const NW_DisplayApi_t *display;   
  /* timer routines */
  const NW_TimerApi_t   *timer;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_WML_API_H */
