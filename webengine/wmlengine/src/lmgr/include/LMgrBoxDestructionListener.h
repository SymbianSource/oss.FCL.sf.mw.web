/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides a means for a box to notify its listener when it is deleted.
*
*/



#ifndef MLMGRBOXDESTRUCTIONLISTENER_H
#define MLMGRBOXDESTRUCTIONLISTENER_H


// CLASS DECLARATION

class MLMgrBoxDestructionListener
    {
    public:
        /**
        * Notify the listener that the box has been destroyed. 
        *
        * @return void
        */
        virtual void Destroyed() = 0;
    };

#endif  // MLMGRBOXDESTRUCTIONLISTENER_H
