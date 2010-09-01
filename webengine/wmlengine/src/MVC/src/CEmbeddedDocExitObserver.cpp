/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer for exit notification from Document Handler
*
*/


#include "EmbeddedDocExitObserver.h"


CEmbeddedDocExitObserver::CEmbeddedDocExitObserver(RFs& aRfs)
    {
    iRfs = aRfs;
    }

CEmbeddedDocExitObserver* CEmbeddedDocExitObserver::NewL(RFs& aRfs)
    {
    return new (ELeave) CEmbeddedDocExitObserver(aRfs);
    }

// We want hardcopy of the filename bacause the caller might have
// already been deleted.
void CEmbeddedDocExitObserver::SetTempFile(TFileName aFileName)
    {
    iFileName = aFileName;
    }

void CEmbeddedDocExitObserver::NotifyExit(TExitMode /*aMode*/)
    {
    RDebug::Print(_L("###### Back to browser")); 
    RThread().SetPriority(EPriorityNormal); 

    iRfs.Delete(iFileName);
    };


