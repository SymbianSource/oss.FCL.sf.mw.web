/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CEpoc32Timer.h"

CEpoc32Timer* CEpoc32Timer::New( const TUint32 aPeriodInMicroseconds, 
                                 const TBool aRepeatingTimer, 
                                 TCallBack& aCallBackFunc )
  {
  CEpoc32Timer* timer = new CEpoc32Timer( aPeriodInMicroseconds, 
                                          aRepeatingTimer, aCallBackFunc);
  if( timer )
    {
    TRAPD( err, timer->ConstructL() );
    if( err != KErrNone )
      {
      delete timer;
      timer = NULL;
      }
    }
  return timer;
  }

CEpoc32Timer::CEpoc32Timer( const TUint32 aPeriodInMicroseconds, 
                            const TBool aRepeatingTimer, 
                            TCallBack& aCallBackFunc ) :
  CTimer( EPriorityLow ), 
  iCallBackFunc( aCallBackFunc ), 
  iTimeOutInMicroseconds( aPeriodInMicroseconds ),
  iTimeLeftInMicroseconds( 0 ),
  iPeriodInMicrosecondsExceedingRangeOfTInt( 0 ),
  iIsRepeating( aRepeatingTimer )
  {
  }
    
CEpoc32Timer::~CEpoc32Timer()
  {
  Cancel();
  }

void CEpoc32Timer::ConstructL()
  {
  CTimer::ConstructL();
  CActiveScheduler::Add(this);
  // at the very beginning, time-left equals to the timeout 
  iTimeLeftInMicroseconds = iTimeOutInMicroseconds;
  // start the timer;
  Start();
  }

void CEpoc32Timer::RunL()
  {
  // TIMER WILL BE CANCELLED IF SYSTEM TIME CHANGES!!!!!!!!!!!!!!!!!!
  // EPOC undocumented feature: If system time was changed, the system will 
  // cancel all active objects. Timer active objects must react to this.  
  const TInt status = iStatus.Int();
  
  if( status == KErrNone )
    {
    if( iPeriodInMicrosecondsExceedingRangeOfTInt == 0 )
      {
      if( iIsRepeating )
        {
        // start the timer again
        iTimeLeftInMicroseconds = iTimeOutInMicroseconds;
        // kick off
        Start();
        }
      iCallBackFunc.CallBack(); // Callback to Kimono
      }
    else
      {
      After( iPeriodInMicrosecondsExceedingRangeOfTInt );
      iPeriodInMicrosecondsExceedingRangeOfTInt = 0;
      }
    }
  else if( status == KErrCancel )
    {
    // Possible causes:
    // 1. Timer was stopped
    // 2. System time was changed
    //
    // If timer was stopped, we can detect it by checking 
    // if iTimeLeftOnStop != 0
    // Otherwise the system time was changed.
    if( iTimeLeftInMicroseconds == 0 )
      {
      // System time changed
      Stop(); // Save the state of timers
      Resume(); // Start timer over as if it was stopped.
      }
    }
  }

TUint32 CEpoc32Timer::MicrosecondsLeftUntilNextTick() const
  {
  TTime timeoutTime;
  TTime currentTime;
  // get the current time
  currentTime.HomeTime();
  // first figure out when the timer is supposed to stop. (end of timer)
  timeoutTime = iResumeTime + TTimeIntervalMicroSeconds( TInt64( (TUint)iTimeLeftInMicroseconds ) );
  // then see how far we are from the to happen. ( this is gonna be the time left value )
  TTimeIntervalMicroSeconds timeDifference =  currentTime.MicroSecondsFrom( timeoutTime );
  // convert it to uint32
  TTimeIntervalMicroSeconds32 timeDiff32 =  I64INT(Abs( timeDifference.Int64() ) );
  // end return
  return timeDiff32.Int();
  }

void CEpoc32Timer::Start()
  {
  if( !IsActive() )
    {
    // timer start-up time.
    iResumeTime.HomeTime();
    // kick off the timer.
    After( iTimeLeftInMicroseconds );
    }
  }

void CEpoc32Timer::Stop()
  {
  iTimeLeftInMicroseconds = MicrosecondsLeftUntilNextTick();
  Cancel();
  }

void CEpoc32Timer::Resume()
  {
  // resume the timer
  Start();
  }

void CEpoc32Timer::After( TUint32 aPeriodInMicroseconds )
  {
  TTimeIntervalMicroSeconds32 interval(0);
  
  if( aPeriodInMicroseconds > (TUint32)KMaxTInt )
    {
    // Set the timer value in 2 phases, because now internal would 
    // have negative time value, and CTimer::After would panic.
    iPeriodInMicrosecondsExceedingRangeOfTInt = 
        aPeriodInMicroseconds - KMaxTInt;
    interval = KMaxTInt;
    }
  else
    {
    // OK. CTimer::After will not panic
    iPeriodInMicrosecondsExceedingRangeOfTInt = 0;
    interval = aPeriodInMicroseconds;
    }  
  CTimer::After( interval );
  }
