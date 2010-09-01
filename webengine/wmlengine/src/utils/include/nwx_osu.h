/*
* Copyright (c) 1999 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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

  Subsystem Name: Operating System Utilities
  Version: V1.0
  Description:    
    Provides interfaces to common system utilities like;
    threads, mutexes, etc

******************************************************************/
#ifndef NWX_OSU_H
#define NWX_OSU_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include "nwx_defs.h"
#include "BrsrStatusCodes.h"

/*
** Type Definitions
*/ 

#define NW_WAIT_FOREVER      -1
#define NW_WAIT_UNSPECIFIED  -2


/* 
**  These OSU types should be opaque to the user.
**  They are returned from, and passed to, the OSU functions
**  but they are never manipulated directly by the user.
*/

typedef void*  NW_Osu_Thread_t;
typedef void*  NW_Osu_SemaphoreHandle_t;
typedef void*  NW_Osu_Mutex_t;
typedef void*  NW_Osu_File_t;
typedef void*  NW_Osu_Signal_t;
typedef NW_Uint32 NW_Osu_ThreadId_t;
typedef void*  NW_Osu_Hwnd_t;

/*
** Global Function Declarations
*/

/*********************** Mutex Functions ************************************/

/* create a new mutex object */
NW_Osu_Mutex_t NW_Osu_NewMutex(const char *name);

/* wait for the mutex to be unlocked and then lock it */
TBrowserStatusCode NW_Osu_LockMutex(NW_Osu_Mutex_t handle);

/* unlock a mutex */
TBrowserStatusCode NW_Osu_UnlockMutex(NW_Osu_Mutex_t handle);

/* delete a mutex object */
void NW_Osu_DeleteMutex(NW_Osu_Mutex_t handle);


/*********************** Signal Functions ************************************/

/* create a new signal object */
NW_Osu_Signal_t NW_Osu_NewSignal(const char *name);

/* set the signal */
TBrowserStatusCode NW_Osu_NotifySignal(NW_Osu_Signal_t handle);

 /* wait for a signal to be set */
TBrowserStatusCode NW_Osu_WaitOnSignal(NW_Osu_Signal_t handle, const NW_Int32 howLong);

/* delete a signal object */
void NW_Osu_DeleteSignal(NW_Osu_Signal_t handle);


/************************* Thread Functions **********************************/

typedef TBrowserStatusCode NW_Osu_StartRoutine_t(void *argument); 

/* Return ID of current thread */
NW_Osu_ThreadId_t NW_Osu_GetCurrentThreadId();


/************************* Critical Section Functions ************************/

/* Enter a mutex protected critical section */
TBrowserStatusCode NW_Osu_BeginCriticalSection();

/* Leave a mutex protected critical section */
TBrowserStatusCode NW_Osu_EndCriticalSection();

/************************* Timer Function ***********************************/

/* Wait a specified number of microseconds */
void NW_Osu_Wait(NW_Int32 anInterval);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_OSU_H */
