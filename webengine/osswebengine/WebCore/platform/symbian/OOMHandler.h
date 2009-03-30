/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __OOMHANDLER_H__
#define __OOMHANDLER_H__

#include <e32def.h>
#include "MemoryManager.h"

namespace WebCore {

class OOMHandler : public MMemoryCollector, public MOOMStopper
{
public:
    OOMHandler();
    virtual ~OOMHandler();

    // from collector
    TUint Collect(TUint aRequired);
    void Restore();

    // from stopper
    void Stop();

    TOOMPriority Priority()     { return EOOM_PriorityHigh; }
};

}

#endif // END OF FILE
