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
* Description:  
*
*/


#ifndef LMGRBOXTREELISTENER_H
#define LMGRBOXTREELISTENER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "BrsrTypes.h"
#include "nwx_defs.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Base class for.
*
*  @lib lmgr.lib
*  @since 2.6
*/
class MBoxTreeListener
{

  public:
    /**
    * 
    * @since 2.6
    * @param 
    * @return CGDIDeviceContext*
    */
    virtual CGDIDeviceContext* GetDeviceContext () = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return NW_LMgr_Box_t*
    */
    virtual NW_LMgr_Box_t* GetCurrentBox () = 0;

    /**
    * 
    * @since 2.6
    * @param box
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode ReformatBox (NW_LMgr_Box_t* box) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode BoxTreeListenerRelayout (NW_Bool createTabList) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    virtual void BoxTreeDestroyed () = 0;

    /**
    * 
    * @since 2.6
    * @param box
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode Refresh (NW_LMgr_Box_t* box) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return void*
    */
    virtual void* GetParentWindow () = 0;

    /**
    * 
    * @since 2.6
    * @param drawNow
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode RedrawDisplay (NW_Bool drawNow) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return NW_LMgr_Box_t*
    */
    virtual NW_LMgr_Box_t* GetInFocusBox () const = 0;

    /**
    * 
    * @since 2.6
    * @param collapseAnonymous
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode Collapse (NW_LMgr_FlowBox_t* containingFlow, NW_Bool collapseAnonymous) = 0;

    /**
    * 
    * @since 2.6
    * @param createTabList
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode FormatBoxTree (NW_Bool createTabList) = 0;

    /**
    * 
    * @since 2.6
    * @param box
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode GotoBox (NW_LMgr_Box_t* box) = 0;

    /**
    * 
    * @since 2.6
    * @param box
    * @return TBrowserStatusCode
    */
    virtual TBrowserStatusCode BoxTreeListenerAppendTabItem (NW_LMgr_Box_t* box) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    virtual void FormattingComplete () = 0;

    /**
    * 
    * @since 2.6
    * @param status
    * @return void
    */
    virtual void HandleError (TBrowserStatusCode status) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    virtual void StartCSSApply () = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    virtual void EnableSuppressDupReformats (NW_Int16 state) = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return NW_Bool
    */
    virtual NW_Bool IsForcedReformatNeeded () = 0;

    /**
    * 
    * @since 2.6
    * @param 
    * @return void
    */
    virtual void ForcedReformatNeeded () = 0;

    /**
    * 
    * @since 3.0
    * @param 
    * @return NW_TRUE if formatting was done, and NW_FALSE if not
    */
    virtual NW_Bool FormatPositionedBoxes () = 0;


};

#endif      // LMGRBOXTREELISTENER_H

// End of File
