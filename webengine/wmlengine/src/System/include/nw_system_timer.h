/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NW_System_Timer_h_
#define _NW_System_Timer_h_

#include "nw_system.h"
#include "nw_object_dynamic.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/** ----------------------------------------------------------------------- **
    @class:       NW_System_Timer

    @scope:       public

    @description: 
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_System_Timer_Class_s NW_System_Timer_Class_t;
typedef struct NW_System_Timer_s NW_System_Timer_t;

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */
typedef TBrowserStatusCode (*NW_System_Timer_Callback_t) (void* clientData);

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_System_Timer_Stop_t) (NW_System_Timer_t* timer);

typedef
TBrowserStatusCode
(*NW_System_Timer_Resume_t) (NW_System_Timer_t* timer);

typedef
TBrowserStatusCode
(*NW_System_Timer_IsRunning_t) (NW_System_Timer_t* timer,
                                NW_Bool* isRunning);

typedef
TBrowserStatusCode
(*NW_System_Timer_Read_t) (NW_System_Timer_t* timer,
                           NW_Uint32* remainingTime);

/* ------------------------------------------------------------------------- *
   object class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_System_Timer_ClassPart_s {
  NW_System_Timer_Stop_t stop;
  NW_System_Timer_Resume_t resume;
  NW_System_Timer_IsRunning_t isRunning;
  NW_System_Timer_Read_t read;
} NW_System_Timer_ClassPart_t;

struct NW_System_Timer_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_System_Timer_ClassPart_t NW_System_Timer;
};

/* ------------------------------------------------------------------------- *
   object instance structure
 * ------------------------------------------------------------------------- */
struct NW_System_Timer_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_System_Timer_Callback_t callback;
  void *clientData;
  NW_Uint32 period;
  NW_Bool repeating;  
  
  NW_Uint32 remainingTime;
  NW_Uint32 startTime;
  NW_Bool isRunning;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_System_Timer_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_System_Timer))

#define NW_System_TimerOf(_object) \
  (NW_Object_Cast (_object, NW_System_Timer))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_SYSTEM_EXPORT const NW_System_Timer_Class_t NW_System_Timer_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
NW_SYSTEM_EXPORT
NW_System_Timer_t*
_NW_System_Timer_New (NW_System_Timer_Callback_t callback,
                      void* clientData,
                      NW_Uint32 period,
                      NW_Bool repeating);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   Function: NW_System_Timer_StopTimer
   Purpose: stops the timer
   Paramters:  timer - pointer to the app. services timer object
   Return Values:
          status KBrsrSuccess if successful
 * ------------------------------------------------------------------------- */
#define NW_System_Timer_Stop(_timer) \
  (NW_Object_Invoke (_timer, NW_System_Timer, stop) \
     (NW_System_TimerOf (_timer)))

/* ------------------------------------------------------------------------- *
   Function: NW_System_Timer_ResumeTimer
   Purpose: resume the timer
   Paramters:  timer - pointer to the app. services timer object
   Return Values:
          status KBrsrSuccess if successful
 * ------------------------------------------------------------------------- */
#define NW_System_Timer_Resume(_timer) \
  (NW_Object_Invoke (_timer, NW_System_Timer, resume) \
     (NW_System_TimerOf (_timer)))

/* ------------------------------------------------------------------------- *
   Function: NW_System_Timer_IsTimerRunning
   Purpose: checks if the timer is running
   Paramters:  timer - pointer to the app. services timer object
   Return Values:
          isRunning is NW_TRUE if the timer is running
          status KBrsrSuccess if successful
 * ------------------------------------------------------------------------- */
#define NW_System_Timer_IsRunning(_timer, _isRunning) \
  (NW_Object_Invoke (_timer, NW_System_Timer, isRunning) \
     (NW_System_TimerOf (_timer), (_isRunning)))

/* ------------------------------------------------------------------------- *
   Function: NW_System_Timer_ReadTimer
   Purpose: return the timer period remaining in the timer
   Paramters:  timer - pointer to the app. services timer object
   Return Values:
          remainingTime - time remaining before the timer expiration (in milliseconds)
          status KBrsrSuccess if successful
 * ------------------------------------------------------------------------- */
#define NW_System_Timer_Read(_timer, _remainingTime) \
  (NW_Object_Invoke (_timer, NW_System_Timer, read) \
     (NW_System_TimerOf (_timer), (_remainingTime)))

/* ------------------------------------------------------------------------- *
   Function: NW_System_Timer_Construct
   Purpose: Creates a timer object and starts the timer
   Paramters:  callBack - this function is called when the timer expires
               clientData - data to be passed to the timer callback function
               period -  period for the timer 
               repeating - if NW_TRUE, the timer is repeating
   Return Values:
                Pointer to the app. services timer object, NULL in case of failure
 * ------------------------------------------------------------------------- */
#define NW_System_Timer_New(_callback, _clientData, _period, _repeating) \
  (_NW_System_Timer_New ((_callback), (_clientData), (_period), (_repeating)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_System_Timer_h_ */
