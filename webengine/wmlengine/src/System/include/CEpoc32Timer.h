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
#ifndef __EPOC32TIMER_H
#define __EPOC32TIMER_H

#include <e32base.h>

class CEpoc32Timer : public CTimer
  {
  public:
    static CEpoc32Timer* New( const TUint32 aPeriodInMicroseconds, 
                              const TBool aRepeatingTimer, 
                              TCallBack& aCallBackFunc );
    ~CEpoc32Timer();
    
    TUint32 MicrosecondsLeftUntilNextTick() const;
    void Stop();
    void Resume();
      
  protected:
    CEpoc32Timer( const TUint32 aPeriodInMicroseconds, 
                  const TBool aRepeatingTimer, 
                  TCallBack& aCallBackFunc );
    void ConstructL();
    void RunL();
    void Start();
      
    virtual void After( TUint32 aPeriodInMicroseconds );
      
  private:
    TTime       iResumeTime;
    TCallBack   iCallBackFunc;
    TUint32     iTimeOutInMicroseconds;
    // How many microseconds was left until next tick when we were stopped
    TUint32     iTimeLeftInMicroseconds; 
    // If the timer period is > MaxTTimeInMicroseconds32, then the extra 
    // milliseconds are stored here.
    TUint32     iPeriodInMicrosecondsExceedingRangeOfTInt; 
    TBool       iIsRepeating;
  };

#endif
