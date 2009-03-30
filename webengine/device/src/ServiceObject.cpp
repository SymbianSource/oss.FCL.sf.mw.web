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
* Description:  Implementation of Device Service Object
*
*/


#include <config.h>
#include "ServiceObject.h"
#include "Device.h"
#include "DeviceBinding.h"
#include "DeviceLiwInterface.h"
#include "DeviceLiwResult.h"
#include "DeviceLiwPeer.h"
#include <PropertyNameArray.h>

using namespace KJS;
const ClassInfo ServiceObject::info = { "ServiceObject", 0, 0, 0 };
_LIT8( KWildChar, "*" );
// ============================ MEMBER FUNCTIONS ===============================

/*
@begin ServiceObjectTable 1
    close ServiceObject::close DontDelete|Function 1
@end
*/
// ----------------------------------------------------------------------------
// ServiceObject::ServiceObject
//
//
//
// ----------------------------------------------------------------------------
ServiceObject::ServiceObject(
    ExecState* exec,
    HBufC8* svcName,
    MDeviceBinding* deviceBinding)
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() )
    {
        m_privateData = new ServiceObjectPrivate(svcName, deviceBinding);
        if (m_privateData && m_privateData->m_deviceBinding)
            {
            m_valid = true;
            // protect this object
            KJS::Collector::protect(this);  
            }            
        else
            {
             m_valid = false;
            }  
    }


// ----------------------------------------------------------------------------
// ServiceObject::~ServiceObject
//
//
//
// ----------------------------------------------------------------------------
ServiceObject::~ServiceObject()
    {
        // only can be called by garbage collection after the 
        // ServiceObject::Close() was called
    }

// ----------------------------------------------------------------------------
// ServiceObject::Close
//
// ----------------------------------------------------------------------------
void ServiceObject::Close( ExecState* exec, bool unmark )
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

    // set isClosing flag to true
    m_privateData->isClosing = true;

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
                if (prop->inherits( &DeviceLiwInterface::info ))
                    {
                    (static_cast<DeviceLiwInterface*>(prop))->Close(exec);
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
// ServiceObject::toString
//
//
//
// ----------------------------------------------------------------------------
UString ServiceObject::toString( ExecState* exec ) const
    {
    return "[object ServiceObject]";
    }


// ----------------------------------------------------------------------------
// ServiceObject::getOwnPropertySlot
//
//
//
// ----------------------------------------------------------------------------
bool ServiceObject::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    // When the ServiceObject is valid, the check order is
    //      ServiceObject table => own property => prototype property
    // When the ServiceObject is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1. when it is valid
    if(m_valid)
        {
        // 1.1 check ServiceObject table
        const HashEntry* entry = Lookup::findEntry(&ServiceObjectTable, propertyName);
        if (entry)
            {
            slot.setStaticEntry(this, entry, staticValueGetter<ServiceObject>);
            return true;
            }

        // 1.2 check own property
        m_privateData->m_propName = propertyName;
        JSValue* val = getDirect( propertyName );

        // if the property is an interface and interface is closed
        bool need_recreate = false;
        if ( val && val->isObject() &&
             val->toObject(exec)->inherits( &KJS::DeviceLiwInterface::info ) )
            {
            DeviceLiwInterface* interface = static_cast<DeviceLiwInterface*>(val);
            if ( !interface->isValid() && !m_privateData->isClosing)
                {
                need_recreate = true;
                }
            }

        if ( !val || need_recreate )
            {
            if ( !val )
                {
                // 1.3 check prototypes
                JSObject *proto = static_cast<JSObject *>(this->prototype());

                while (!proto->isNull() && proto->isObject()) {
                    if (proto->getOwnPropertySlot(exec, propertyName, slot))
                        return true;

                    proto = static_cast<JSObject *>(proto->prototype());
                    }
                }

            // Store the interface in the object so we get the same one each time.
            JSValue* resultVal = m_privateData->m_deviceBinding->CreateInterface(
                exec, m_privateData->m_svcName, m_privateData->m_propName );

            if ( resultVal->type() == UndefinedType || exec->hadException() )
                return false;
            else
                {
                JSValue* s = resultVal->toObject(exec)->get( exec, m_privateData->m_propName );
                this->putDirect( propertyName, s, DontDelete|ReadOnly );
                }

            // clean the DeviceLiwResult which is useless
            if(resultVal->isObject()) 
                {
                JSObject * jsobj = resultVal->toObject( exec );
                if(jsobj->inherits( &KJS::DeviceLiwResult::info ))
                    {
                    DeviceLiwResult* result = static_cast<DeviceLiwResult*>(jsobj);
                    result->quickClose();
                    }
                }
            }
        return JSObject::getOwnPropertySlot(exec, propertyName, slot);
        }
    // 2. when it is invalid
    else
        {
        // 2.1 check close function
        if (propertyName == "close")
            {
            const HashEntry* entry = Lookup::findEntry(&ServiceObjectTable, propertyName);
            if (entry)
                {
                slot.setStaticEntry(this, entry, staticValueGetter<ServiceObject>);
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
// ServiceObject::get
//
//
//
// ----------------------------------------------------------------------------
JSValue* ServiceObject::getValueProperty(KJS::ExecState* exec, int token) const
    {
    if (token == close)
        {
            return new ServiceObjectFunc( exec, token );
        }
    if(m_valid)
        return getDirect(m_privateData->m_propName);
    else
        return jsUndefined();
    }

// ----------------------------------------------------------------------------
// ServiceObject::IsRunningCallBack
//
//
//
// ----------------------------------------------------------------------------
TBool ServiceObject::IsRunningCallBack(ExecState *exec)
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
            if (prop->inherits( &DeviceLiwInterface::info ))
                {
                if((static_cast<DeviceLiwInterface*>(prop))->IsRunningCallBack())
                    {
                    return ETrue;
                    }
                }
            }
        }  
    return EFalse;      
    }

ServiceObjectFunc::ServiceObjectFunc( ExecState* exec, int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_func( token )
    {
    }

JSValue* ServiceObjectFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
    {
    JSValue* ret = jsUndefined();
    if (!thisObj->inherits( &KJS::ServiceObject::info ) )
        {
        return throwError(exec, GeneralError);
        }
    ServiceObject* so = static_cast<ServiceObject*>(thisObj);

    if(!so->isValid())
        return ret;
    
    if ( m_func == ServiceObject::close )
        {
        // the close function cann't be called in the callback function
        if(so->IsRunningCallBack(exec))
            {
            return throwError(exec, GeneralError, "Can not close service object in callback function.");
            }
        so->Close( exec, false );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate constructor
//
// ---------------------------------------------------------------------------
ServiceObjectPrivate::ServiceObjectPrivate(HBufC8* svcName, MDeviceBinding* deviceBinding )
    {
     m_svcName = svcName;    
     m_deviceBinding = deviceBinding;
     isClosing = false;
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate::Close
//
// ---------------------------------------------------------------------------
void ServiceObjectPrivate::Close()
    {
    m_deviceBinding->UnloadServiceProvider(KWildChar(), m_svcName->Des());
    m_deviceBinding = NULL;
        
    delete m_svcName;
    m_svcName = NULL;    
    }

//END OF FILE
