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
#include <MemoryManager.h>

namespace WebCore {

/*
 * This handles collecting memory when OOM occurs, by clearing the cache.
 * The StopScheduler has two collection modes, low and middle priority.
 * When memory is at a certain threshold, all collectors low and above are called.
 * When memory is at a critical threshold, all collectors middle and above are called.
 * We want the cache to always clear whenever OOM happens so this collector's
 * priority is high.
 */
class OOMHandler : public MMemoryCollector
{
public:
    OOMHandler();
    virtual ~OOMHandler();

    // from collector
    TUint Collect(TUint aRequired);
    void Restore();

    TOOMPriority Priority()     { return EOOM_PriorityHigh; }
};

}

#endif // END OF FILE
