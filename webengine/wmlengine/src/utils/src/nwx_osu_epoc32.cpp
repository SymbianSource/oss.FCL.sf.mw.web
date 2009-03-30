/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides interfaces to common system utilities like;
*                threads, mutexes, etc
*
*/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <e32std.h>
#include <string.h>
#include "nwx_ctx.h"
#include "nwx_osu_epoc32.h"
#include "nwx_logger.h"
#include "nwx_string.h"
#include <ls_std.h>
#include "BrsrStatusCodes.h"

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
typedef struct 
	{
	RMutex iMutex;
	} NW_Osu_Mutex_Struct_t;

/* We use a Semaphore obj as a singal object */
typedef struct 
	{
	RSemaphore iSemaphore;
	} NW_Osu_Signal_Struct_t;

//#define NW_OSU_THREAD_T_CAST(expr) ((NW_Osu_Thread_Struct_t*)(expr))
//#define NW_OSU_SEMAPHORE_T_CAST(expr) ((NW_Osu_Signal_Struct_t*)(expr))
#define NW_OSA_MUTEX_T_CAST(expr) ((NW_Osu_Mutex_Struct_t*) (expr))
#define NW_OSU_SIGNAL_T_CAST(expr) ((NW_Osu_Signal_Struct_t*)(expr))


typedef struct {
	NW_Osu_Mutex_t g_NW_CriticalSectionMutex;
	NW_Osu_ThreadId_t gThread_Id;
} Osu_Context_t;

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
static Osu_Context_t *Osu_GetContext();
static Osu_Context_t* OsuInitialize();


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
static Osu_Context_t *Osu_GetContext()
{
  Osu_Context_t* context;

  context = (Osu_Context_t*)NW_Ctx_Get(NW_CTX_OSU, 0);
  if (context == NULL) 
  {
    context = OsuInitialize();
    NW_Ctx_Set(NW_CTX_OSU, 0, context);
  }

  return context;
}


/*****************************************************************
**  Name:   OsuInitialize
**  Description:  creates the Osu context and stores it
**  Parameters:   
**  Return Value: 
******************************************************************/
static Osu_Context_t* OsuInitialize()
{
  Osu_Context_t *context;

  context = (Osu_Context_t*)NW_Ctx_Get(NW_CTX_OSU, 0);
  if (context == NULL)
  {
    context = new Osu_Context_t;

    //lint -e{774} Significant prototype coercion
    if (context != NULL) {
      context->g_NW_CriticalSectionMutex = NW_Osu_NewMutex("Critical Section");
      if (context->g_NW_CriticalSectionMutex != NULL) {
        context->gThread_Id = 0;
        NW_Ctx_Set(NW_CTX_OSU, 0, context);
       return context;
      }
      delete context;
    }
  }

  return NULL;
}


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Name:   NW_Osu_NewMutex
**  Description:  creates a new mutex object
**  Parameters:   *name - name of mutex for debug logging
**  Return Value: handle to new mutex object or NULL
******************************************************************/
NW_Osu_Mutex_t NW_Osu_NewMutex(const char *name)
{
  NW_Osu_Mutex_Struct_t* m = new NW_Osu_Mutex_Struct_t;

  //lint -e{774} Significant prototype coercion
  if (!m) 
    return NULL;

  if (name == NULL) 
  {
    NW_LOG0(NW_LOG_LEVEL5, "osi_newMutex NoName\n");
    _LIT(KNoName, "NW_OS_Mutex_NoName");
    if(m->iMutex.OpenGlobal(KNoName, EOwnerProcess) != KErrNone)
      (void) m->iMutex.CreateGlobal(KNoName, EOwnerProcess);
  }
  else 
  {
    _LIT(KMutexBaseName, "NW_OS_Mutex_%s");
#if defined(_UNICODE)
    NW_Ucs2* wName = NW_Str_CvtFromAscii(name);
    if (!wName)
    {
      delete m;
      return NULL;
    }
    NW_LOG1(NW_LOG_LEVEL5, "osi_newMutex %s\n",wName);
    NW_Uint32 wNameLen = NW_Str_Strlen(wName);
    HBufC16* mutexName = HBufC16::New((TInt) (13+1+wNameLen));
    if (!mutexName)
    {
      NW_Str_Delete(wName);
      delete m;
      return NULL;
    }
    mutexName->Des().Format(KMutexBaseName, wName);
    NW_Str_Delete(wName);
#else
    NW_Uint32 nameLen = strlen(name);
    HBufC8* mutexName = HBufC8::New((TInt) (13+1+nameLen));
    mutexName->Des().Format(KMutexBaseName, name);
#endif
    if(m->iMutex.OpenGlobal(mutexName->Des(), EOwnerProcess) != KErrNone)
      (void) m->iMutex.CreateGlobal(mutexName->Des(), EOwnerProcess);
    delete mutexName;
  }
 
	return m;
}

/*****************************************************************
**  Name:   NW_Osu_UnlockMutex
**  Description:  unlocks a mutex 
**  Parameters:   handle - handle to the mutex object
**  Return Value: STAT_SUCCESS
******************************************************************/
TBrowserStatusCode NW_Osu_UnlockMutex(NW_Osu_Mutex_t handle)
{
  NW_ASSERT(handle != NULL);
  NW_LOG1(NW_LOG_LEVEL5, "osi_unlockMutex %d\n",handle);
  NW_OSA_MUTEX_T_CAST(handle)->iMutex.Signal();

  return KBrsrSuccess;
}

/*****************************************************************
**  Name:   NW_Osu_LockMutex
**  Description:  wait indefintely for a mutex and then lock it
**  Parameters:   handle - handle to the mutex object
**  Return Value: STAT_SUCCESS
**  Note: does *NOT* wait if called in succesion from the same
**        thread in order to prevent deadlocks.
******************************************************************/
TBrowserStatusCode NW_Osu_LockMutex(NW_Osu_Mutex_t handle)
{
  NW_ASSERT(handle != NULL);
  NW_LOG1(NW_LOG_LEVEL5, "osi_lockMutex %d\n",handle);

  NW_OSA_MUTEX_T_CAST(handle)->iMutex.Wait();
  return KBrsrSuccess;
}


/*****************************************************************
**  Name:   NW_Osu_DeleteMutex
**  Description:  deletes a mutex object
**  Parameters:   handle - handle to the mutex object
**  Return Value: void
**  Note: mutex object *MUST* no longer be in use by *ANY* thread
******************************************************************/
void NW_Osu_DeleteMutex(NW_Osu_Mutex_t handle)
{
  NW_ASSERT(handle != NULL);
  NW_LOG1(NW_LOG_LEVEL5, "osi_deleteMutex %d\n", handle);
	
  NW_OSA_MUTEX_T_CAST(handle)->iMutex.Close();
  delete handle;
}


/*******************************************************
**  Name:		  NW_Osu_BeginCriticalSection
**  Description:  Begins the Execution of a Critical Section
**  Parameters:   
**  Return Value: KBrsrSuccess or KBrsrUnexpectedError
******************************************************************/
TBrowserStatusCode NW_Osu_BeginCriticalSection()
{
  Osu_Context_t *ctx = Osu_GetContext();
  if(ctx == NULL)
    return KBrsrOutOfMemory;

  NW_Osu_LockMutex(ctx->g_NW_CriticalSectionMutex);
	
  return KBrsrSuccess;
}

/*******************************************************
**  Name:		  NW_Osu_EndCriticalSection
**  Description:  Ends the Execution of a Critical Section
**  Parameters:   
**  Return Value: 
******************************************************************/
TBrowserStatusCode NW_Osu_EndCriticalSection()
{
  Osu_Context_t *ctx = Osu_GetContext();
  NW_Osu_UnlockMutex(ctx->g_NW_CriticalSectionMutex);

  return KBrsrSuccess;
}

/*****************************************************************
**  Name:   NW_Osu_NewSignal
**  Description:  creates a new signal object
**  Parameters:   *name - name of signal for debug logging
**  Return Value: handle to new signal object or NULL
******************************************************************/
NW_Osu_Signal_t NW_Osu_NewSignal(const char *name)
{
  NW_Osu_Signal_Struct_t* handle = new NW_Osu_Signal_Struct_t;
/*  NW_ASSERT(handle != NULL); */

  //lint -e{774} Significant prototype coercion
  if (handle == NULL)
    return handle;

#if defined(_UNICODE)
  NW_Ucs2* wName = NW_Str_CvtFromAscii(name);
  if (!wName)
  {
    delete handle;
    return NULL;
  }
  TPtrC semaphoreName(wName, NW_Str_Strlen(wName));
  NW_LOG2(NW_LOG_LEVEL5, "osi_createSignal %d %s\n", handle, wName);
#else
  TPtrC semaphoreName((const unsigned char*)name, strlen(name));
#endif
	
  TInt ret = 
    NW_OSU_SIGNAL_T_CAST(handle)->iSemaphore.CreateGlobal(semaphoreName, 
        0, EOwnerProcess);

  /* Something wrong with the name */
  if (ret < 0) {
    /* Max Thread Id is 11 digits long ; hardset buflength to account for that
       16 + 11 + 1 = 28  */
    TBuf<28> semaphoreName2;
    _LIT(KSemBaseName, "NW_OS_Semaphore_%d");
    semaphoreName2.Format(KSemBaseName, NW_Osu_GetCurrentThreadId());
    ret = NW_OSU_SIGNAL_T_CAST(handle)->iSemaphore.CreateGlobal(semaphoreName2,
      0, EOwnerProcess);
  }

#if defined(_UNICODE)
  NW_Str_Delete(wName);
#endif

  return handle;
}

/*****************************************************************
**  Name:   NW_Osu_NotifySignal
**  Description:  sets signal object and notifies any waiting threads
**  Parameters:   handle - handle to the signal object
**  Return Value: STAT_SUCCESS
******************************************************************/
TBrowserStatusCode NW_Osu_NotifySignal(NW_Osu_Signal_t handle)
{
  NW_ASSERT(handle != NULL);
  NW_LOG1(NW_LOG_LEVEL5, "osi_notifySignal %d\n", handle);

  NW_OSU_SIGNAL_T_CAST(handle)->iSemaphore.Signal();

  return KBrsrSuccess;
}

/*****************************************************************
**  Name:   NW_Osu_WaitOnSignal
**  Description:  wait for a signal to be set
**  Parameters:   handle - handle to the signal object
**                howLong - number of mSec to wait or NW_WAIT_FOREVER
**  Return Value: STAT_SUCCESS is object is signaled, else STAT_WAIT_TIMEOUT
******************************************************************/
TBrowserStatusCode NW_Osu_WaitOnSignal(NW_Osu_Signal_t handle, const NW_Int32 howLong)
{
  NW_REQUIRED_PARAM(howLong);

  /*TODO: to add a time waited signal function... */
  /*However, EPOC port might not need it anyway. */
  /*NW_Int32 wait = howLong; */

  NW_ASSERT(handle != NULL);
  NW_LOG2(NW_LOG_LEVEL5, "osi_waitSignal %d, %d\n", handle, howLong);

  /*if (wait == NW_WAIT_FOREVER)
      wait = INFINITE; */

  /*TODO: Impletement a time waited semaphore class to use it here.
    Right now we just assume all wait all forever.  This can be
    dangerous. */
  NW_OSU_SIGNAL_T_CAST(handle)->iSemaphore.Wait();
  
  return KBrsrSuccess;
}

/*****************************************************************
**  Name:   NW_Osu_DeleteSignal
**  Description:  deletes a signal object
**  Parameters:   handle - handle to the signal object
**  Return Value: void
**  Note: signal object *MUST* no longer be in use by *ANY* thread
******************************************************************/
void NW_Osu_DeleteSignal(NW_Osu_Signal_t handle)
{
  NW_ASSERT(handle != NULL);
  NW_LOG1(NW_LOG_LEVEL5, "osi_deleteSignal %d\n", handle);

  NW_OSU_SIGNAL_T_CAST(handle)->iSemaphore.Close();
  delete handle;
}

/*****************************************************************
**  Name:   NW_Osu_GetCurrentThreadId
**  Description:  gets ID of the current thread
**  Parameters:   none
**  Return Value: ID of the thread
******************************************************************/
NW_Osu_ThreadId_t NW_Osu_GetCurrentThreadId()
{
  Osu_Context_t *ctx = Osu_GetContext();
  if (ctx == NULL)
    return 0;

  return ctx->gThread_Id;
}

/*****************************************************************
**  Name:   NW_Osu_Wait
**  Description:  Waits for the specified number of microseconds
**  Parameters:   anInterval: Number of microseconds to wait
**  Return Value: void
******************************************************************/
void NW_Osu_Wait(NW_Int32 anInterval)
{
  User::After((TTimeIntervalMicroSeconds32)anInterval);
}

/*****************************************************************
**  Name:   NW_Osu_DeleteContext
**  Description:  Deletes the Osu context
**  Parameters:
**  Return Value: 
******************************************************************/
void NW_Osu_DeleteContext()
{
  Osu_Context_t *ctx = Osu_GetContext();

  if (ctx == NULL)
    return;

  /* This may cause problems if the Mutex is being used. The intention
     is to only use this function for cleaning up; more work may
     need to be done. */
  NW_Osu_DeleteMutex(ctx->g_NW_CriticalSectionMutex);
  
  delete ctx;
  NW_Ctx_Set(NW_CTX_OSU, 0, NULL);
}
