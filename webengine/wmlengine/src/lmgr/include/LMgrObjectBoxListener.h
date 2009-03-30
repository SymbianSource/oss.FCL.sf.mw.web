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
* Description:  Provides a means for a CLMgrObjectBox to communicate with its owner.
*
*/



#ifndef MLMGROBJECTBOXLISTENER_H
#define MLMGROBJECTBOXLISTENER_H

// INCLUDES
#include <e32def.h>

#include "LMgrBoxDestructionListener.h"

// FORWARD DECLARATIONS

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_GDI_Rectangle_s NW_GDI_Rectangle_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

// CLASS DECLARATION

class MLMgrObjectBoxListener : public MLMgrBoxDestructionListener
    {
    public:
        /**
        * Notifies the listener of the box's size and location.
        *
        * @since 2.6
        * @param aBounds the bounds of the box.
        * @return The error code from the plugin
        */
        virtual TBrowserStatusCode SetExtent(const NW_GDI_Rectangle_t& aBounds) = 0;

        /**
        * Notifies the listener to redraw
        *
        * @since 2.6
        * @param 
        * @return The error code from the plugin
        */
        virtual TBrowserStatusCode RefreshPlugin() = 0;


        /**
        * Notifies the listener of whether the box is visible or not.
        *
        * @since 2.6
        * @param aVisible whether the box is visible or not.
        * @return void
        */
        virtual void MakeVisible(TBool aVisible) = 0;

        /**
        * Returns if the plugin is active or not
        * @since 2.6
        * @param.
        * @return Returns if the object is active or not
        */
        virtual TBool IsActive() const = 0;
    };

#endif  // MLMGROBJECTBOXLISTENER_H
