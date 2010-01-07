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


#ifndef __OOMSTOPPER_H__
#define __OOMSTOPPER_H__

#include <e32def.h>
#include <MemoryManager.h>

namespace WebCore {

/*
 * This handles stopping things when OOM occurs, by stopping page loading.
 * The StopScheduler has two OOM modes, Normal and Emergency.  Normal mode 
 * calls all low priority stoppers.  Emergency mode calls all high priority stoppers.
 * (For some reason, emergency mode never seems to occur).   This stopper is low 
 * priority because we want to always stop page loading whenever OOM happens
 * (not just in emergency OOM situations).
 */
class OOMStopper : public MOOMStopper
{
public:
    OOMStopper();
    virtual ~OOMStopper();

    void Stop();

    // Priority is low because we always want to stop page loading when we're low on memory; not just during an emergency memory situation
    TOOMPriority Priority()     { return EOOM_PriorityLow; }
};

}

#endif // END OF FILE
