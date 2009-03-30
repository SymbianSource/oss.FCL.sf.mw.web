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
    $Workfile: util_cdispatchereng.h $

*/
#ifndef __UTIL_CDISPATCHERENG_H__
#define __UTIL_CDISPATCHERENG_H__

#include <e32cons.h>
#include <e32base.h>
#include <e32std.h>

NONSHARABLE_CLASS(CDispatcherEng) : public CActive {
                
public:
  CDispatcherEng(CActive::TPriority aPriority);

  ~CDispatcherEng();
  
  void RequestEvent();
  void CompleteEvent();
  void RunL();
  void DoCancel();
  TInt RunError(TInt aError);

  NW_Bool IsDspContext();
  void SetDspContext(NW_Msg_Dispatcher_t* aDsp_cxt);
  
  //Data member (attribute);
  NW_Msg_Dispatcher_t* iDsp_cxt;
};

#endif //__UTIL_CDISPATCHERENG_H__
