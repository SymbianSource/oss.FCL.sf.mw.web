/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface for Timer
*
*/

#ifndef TIMERINTERFACE_H_
#define TIMERINTERFACE_H_

class MTimerInterface
{
public:
	virtual void startTouchTimer(TInt aDelay, TInt aPointerNumber) = 0 ;
	virtual void cancelTouchTimer(TInt aPointerNumber) = 0 ;
	virtual void startHoldTimer(TInt aDelay, TInt aPointerNumber) = 0 ;
	virtual void cancelHoldTimer(TInt aPointerNumber) = 0 ;
	virtual void startSuppressTimer(TInt aDelay, TInt aPointerNumber) = 0 ;
	virtual void cancelSuppressTimer(TInt aPointerNumber) = 0 ;
};
#endif /* TIMERINTERFACE_H_ */
