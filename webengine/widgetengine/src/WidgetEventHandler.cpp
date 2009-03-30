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
* Description:  Implemetation of WidgetEventHandler
*
*/
#include "WidgetEventHandler.h"
#include "config.h"
#include <kjs/object.h>

// ============================ MEMBER FUNCTIONS ===============================
using namespace KJS;


// ----------------------------------------------------------------------------
// WidgetEventHandler::WidgetEventHandler
//
//
//
// ----------------------------------------------------------------------------
WidgetEventHandler::WidgetEventHandler( JSValue* aValue, ExecState* aExecState ) : iGlobalExecState (aExecState)
{
    iEventHandler = aValue;
    Collector::protect(iEventHandler);    
}


// ----------------------------------------------------------------------------
// WidgetEventHandler::~WidgetEventHandler
//
//
//
// ----------------------------------------------------------------------------
WidgetEventHandler::~WidgetEventHandler()
{
    Collector::unprotect(iEventHandler);
}

// ----------------------------------------------------------------------------
// WidgetEventHandler::Delete
// Delete event handler if it isn't handling an event
// If InvokeCall is executing then delete after it's done.
//
// ----------------------------------------------------------------------------
void WidgetEventHandler::Delete()
{
    if (iInUse) {
        iSelfDelete = ETrue;
    }
    else {
        delete this;
    }
        
}
// ----------------------------------------------------------------------------
// WidgetEventHandler::InvokeCall
// Invokes the callback function
//
//
// ----------------------------------------------------------------------------
void WidgetEventHandler::InvokeCall( const TInt& aParam )
{
    
    iInUse = ETrue;
    if (iEventHandler) {

        JSLock::lock();
        if (iGlobalExecState &&
            iGlobalExecState->dynamicInterpreter()) 
        {
                       
            if (iEventHandler &&
                !iEventHandler->isNull() &&
                iEventHandler->isObject()) {
                
                KJS::List args;
                if ( aParam != -1 ) {
                    args.append( jsNumber(aParam) );
                }
 
                JSObject* objFunc = static_cast<JSObject*>(iEventHandler);
                if (objFunc->implementsCall()) {
                    objFunc->call(iGlobalExecState,iGlobalExecState->dynamicInterpreter()->globalObject(),args);
                }
                
                if ( iGlobalExecState->hadException() ) {
                    iGlobalExecState->clearException();
                }
            }
        
        }
        
        JSLock::unlock();
    }
        
    iInUse = EFalse;
    if (iSelfDelete)
        {
        delete this;
        }

}

