/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <config.h>
#include <lookup.h>
#include <liwvariant.h>
#include <PropertyNameArray.h>
#include "DeviceLiwResult.h"
#include "DeviceLiwIterable.h"
#include "DeviceLiwMap.h"
#include "array_instance.h"

const TInt KMaxKeySize = 255;

using namespace KJS;
const ClassInfo DeviceLiwResult::info = { "DeviceLiwResult", 0/*&ArrayInstance::info*/, 0, 0 };

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceLiwResultTable 1
    close DeviceLiwResult::close DontDelete|Function 0
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// DeviceLiwResult::DeviceLiwResult
//
// ----------------------------------------------------------------------------
//
DeviceLiwResult::DeviceLiwResult( JSValue* proto)
    : JSObject(proto)
    {
        m_privateData = new DeviceLiwResultPrivate();
        if (!m_privateData)
            m_valid = false;
        else 
            {
            m_valid = true;
            // protect this object
            KJS::Collector::protect(this);
            }            
    }

// ----------------------------------------------------------------------------
// DeviceLiwResult::~DeviceLiwResult
//
// ----------------------------------------------------------------------------
//
DeviceLiwResult::~DeviceLiwResult()
    {
        // only can be called by garbage collection after the 
        // DeviceLiwResult::Close() was called
    }

// ----------------------------------------------------------------------------
// DeviceLiwResult::Close
//
// ----------------------------------------------------------------------------
void DeviceLiwResult::Close( ExecState* exec, bool unmark )
    {
    
    // avoid double close    
    if(!m_valid) 
        {   
        if(unmark) 
            {
            // unprotect this to allow the garbage collection to release this jsobject
            KJS::Collector::unprotect(this);
            }
        return;
        }
    
    // close thDeviceLiwIterable
    if ( exec )
        {
        PropertyNameArray propertyNames;
        this->getPropertyNames( exec, propertyNames );
        unsigned size = static_cast<unsigned>(propertyNames.size());

        for (unsigned i = 0; i < size; i++)
            {
            JSValue * jsvalue = this->get( exec, propertyNames[i] );
            if(jsvalue->isObject()) 
                {            
                JSObject * prop = jsvalue->toObject( exec );
                if (prop && prop->inherits( &DeviceLiwIterable::info ))
                    {
                    (static_cast<DeviceLiwIterable*>(prop))->Close(exec, true);
                    }
                }                
            }            
        }
    
    delete m_privateData;
    m_privateData = NULL;
    m_valid = false;
   
    if(unmark) 
        {
        // unprotect this to allow the garbage collection to release this jsobject
        KJS::Collector::unprotect(this);
        }
    }

// ----------------------------------------------------------------------------
// DeviceLiwResult::QuickClose
//
// ----------------------------------------------------------------------------
void DeviceLiwResult::quickClose()
    { 
    if(!m_valid) 
        return
 
    delete m_privateData;
    m_privateData = NULL;
    m_valid = false;
   
    // unprotect this to allow the garbage collection to release this jsobject
    KJS::Collector::unprotect(this);
    }
    
// ----------------------------------------------------------------------------
// DeviceLiwResult::toString
//
// 
// ----------------------------------------------------------------------------
UString DeviceLiwResult::toString( ExecState* /*exec*/ ) const
    {
    return "[object DeviceLiwResult]";
    }

// ----------------------------------------------------------------------------
// DeviceLiwResult::getOwnPropertySlot
//
//
// ----------------------------------------------------------------------------
bool DeviceLiwResult::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
    {
    // When the DeviceLiwResult is valid, the check order is
    //      DeviceLiwResult table => own property => prototype property
    // When the DeviceLiwResult is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1. when it is valid
    if(m_valid) 
        {
        m_privateData->m_propName = propertyName;
    
        // 1.1 check DeviceLiwResult table
        const HashEntry* entry = Lookup::findEntry(&DeviceLiwResultTable, propertyName);
        if (entry)
            {
            slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwResult>);
            return true;
            }

        // 1.2 check own property
        if (JSObject::getOwnPropertySlot(exec, propertyName, slot))
            return true;
        
        // 1.3 check prototypes
        JSObject *proto = static_cast<JSObject *>(this->prototype());
        while (!proto->isNull() && proto->isObject())
            {
            if (proto->getOwnPropertySlot(exec, propertyName, slot))
                return true;

            proto = static_cast<JSObject *>(proto->prototype());
            }
        }
    // 2. when it is invalid
    else
        {
        // 2.1 check close() function
        if (propertyName == "close")
            {
            const HashEntry* entry = Lookup::findEntry(&DeviceLiwResultTable, propertyName);
            if (entry)
                {
                slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwResult>);
                return true;
                }      
            }

        // 2.2 check prototypes
        JSObject *proto = static_cast<JSObject *>(this->prototype());
        while (!proto->isNull() && proto->isObject())
            {
            if (proto->getOwnPropertySlot(exec, propertyName, slot))
                return true;

            proto = static_cast<JSObject *>(proto->prototype());
            }
        }
    return false;
    }


// ----------------------------------------------------------------------------
// DeviceLiwResult::getValueProperty
//
// ----------------------------------------------------------------------------
JSValue* DeviceLiwResult::getValueProperty(ExecState *exec, int token) const
    {
    switch (token)
        {
        case close:
            {
            return new DeviceLiwResultFunc(exec, token);
            }
        default:
            if(m_valid)
                return getDirect(m_privateData->m_propName);
            else
                return jsUndefined();
        }
    }

// ---------------------------------------------------------------------------
// DeviceLiwIterablePrivate constructor
//
// ---------------------------------------------------------------------------
DeviceLiwResultPrivate::DeviceLiwResultPrivate()
    {
        // currently, do nothing
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate::Close
//
// ---------------------------------------------------------------------------
void DeviceLiwResultPrivate::Close()
    {
        // currently, do nothing
    }
    
// ----------------------------------------------------------------------------
// DeviceLiwResultFunc::DeviceLiwResultFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwResultFunc::DeviceLiwResultFunc( ExecState* exec, int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_func( token )
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwResultFunc::call
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceLiwResultFunc::callAsFunction(ExecState* exec, JSObject *thisObj, const List& aArgs )
    {
    if (!thisObj->inherits(&DeviceLiwResult::info)) 
        {
        return throwError(exec, GeneralError);
        }
    DeviceLiwResult *result = static_cast<DeviceLiwResult *>(thisObj);

    if ( m_func == DeviceLiwResult::close )
        {
        result->Close(exec, false);
        }
    return jsUndefined();
    }

//END OF FILE




