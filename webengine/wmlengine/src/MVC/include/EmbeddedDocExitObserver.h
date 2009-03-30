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

#ifndef EMBEDDED_DOC_EXIT_OBSERVER_H
#define EMBEDDED_DOC_EXIT_OBSERVER_H

#include <apparc.h>

class RFs;

NONSHARABLE_CLASS(CEmbeddedDocExitObserver): public MApaEmbeddedDocObserver 
    {
    public:
        static CEmbeddedDocExitObserver* NewL(RFs& aRfs);

        void NotifyExit(TExitMode aMode);

        void SetTempFile(TFileName aFileName);

    private:
        CEmbeddedDocExitObserver(RFs& aRfs);
        CEmbeddedDocExitObserver() {};
        TFileName iFileName;
        RFs iRfs;
    };


#endif  /* EMBEDDED_DOC_EXIT_OBSERVER_H */
