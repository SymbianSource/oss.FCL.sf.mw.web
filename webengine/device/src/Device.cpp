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
#include "config.h"
#include "Device.h"
#include "ServiceObject.h"
#include "ServiceEventHandler.h"
#include "DeviceBridge.h"
#include "DeviceBinding.h"
#include "DeviceLiwBinding.h"
#include <LiwServiceHandler.h>

using namespace KJS;

const ClassInfo Device::info = { "Device", 0, 0, 0 };
const TInt INIT_SO_ARRAY_SIZE = 10;   // initial service object array

// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceTable 1
    getServiceObject Device::getServiceObject DontDelete|Function 2
    listProviders Device::listProviders DontDelete|Function 2
@end
*/

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// Device::Device
//
// ----------------------------------------------------------------------------
//
Device::Device( ExecState* exec )
    : JSObject()
    {
    m_privateData = new DevicePrivate();
    if (!m_privateData || !m_privateData->m_deviceBinding )
        m_valid = false;
    else
        m_valid = true;
    }


// ----------------------------------------------------------------------------
// Device::SetUid
//
// ----------------------------------------------------------------------------
//
void Device::SetUid( const TUint& aValue)
    {
    if(m_privateData)
        m_privateData->SetUid( aValue);
    }

// ----------------------------------------------------------------------------
// Device::Close
//
// ----------------------------------------------------------------------------
//
void Device::Close()
    {
    delete m_privateData;
    m_privateData = NULL;
    }


// ----------------------------------------------------------------------------
// Device::~Device
//
// ----------------------------------------------------------------------------
//
Device::~Device()
    {
    Close();
    }

// ----------------------------------------------------------------------------
// Device::toString
//
// ----------------------------------------------------------------------------
//
UString Device::toString( ExecState* /*exec*/ ) const
    {
    return "[object Device]";
    }

// ----------------------------------------------------------------------------
// Device::getOwnPropertySlot
//
//
// ----------------------------------------------------------------------------
bool Device::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    m_privateData->m_exec = exec;
    m_privateData->m_propName = propertyName;
    const HashEntry* entry = Lookup::findEntry(&DeviceTable, propertyName);
    if (entry)
        {
        slot.setStaticEntry(this, entry, staticValueGetter<Device>);
        return true;
        }

    return JSObject::getOwnPropertySlot(exec, propertyName, slot);
}


// ----------------------------------------------------------------------------
// JSVersion::getValueProperty
//
//
// ----------------------------------------------------------------------------
JSValue* Device::getValueProperty(ExecState *exec, int token) const
    {
    switch( token )
        {
        case getServiceObject:
        case listProviders:
                return new DeviceFunc( exec, m_privateData->m_deviceBinding, token );

        default:
            return throwError(exec, GeneralError);
        }
    }

// ---------------------------------------------------------------------------
// DevicePrivate constructor
//
// ---------------------------------------------------------------------------
DevicePrivate::DevicePrivate()
    {
	m_deviceBinding = NULL;
    TRAP_IGNORE(
        m_serviceObjArray = new RPointerArray<ServiceObject>( INIT_SO_ARRAY_SIZE );
        m_deviceBinding = CDeviceLiwBinding::NewL();
        m_exec = NULL;)
    }

// ---------------------------------------------------------------------------
// DevicePrivate Close
//
// ---------------------------------------------------------------------------
void DevicePrivate::Close()
    {
    if ( m_serviceObjArray )
        {
        // close all the service objects created for this device
        for (int i = 0; i < m_serviceObjArray->Count(); i++)
            {
            (*m_serviceObjArray)[i]->Close( m_exec, true );
            }
        m_serviceObjArray->Close();
        delete m_serviceObjArray;
        m_serviceObjArray = NULL;
        }
    delete m_deviceBinding;
    m_deviceBinding = NULL;
    }

// ---------------------------------------------------------------------------
// DevicePrivate SetUid
//
// ---------------------------------------------------------------------------
void DevicePrivate::SetUid( const TUint& aValue)
    {
    if(m_deviceBinding)
        m_deviceBinding->SetUid( aValue);
    }

// ----------------------------------------------------------------------------
// DeviceFunc::DeviceFunc
//
//
// ----------------------------------------------------------------------------
//
DeviceFunc::DeviceFunc( ExecState* exec, MDeviceBinding* deviceBinding,
    int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_deviceBinding( deviceBinding ),
    m_func( token )
    {
    }

// ----------------------------------------------------------------------------
// DeviceFunc::callAsFunction
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceFunc::callAsFunction(ExecState *exec, JSObject *thisObj, const List &args)
    {
    TInt err = KErrNone;
    JSValue* ret = jsUndefined();

    if ( !m_deviceBinding )
        {
        return ret;
        }

    if ( m_func == Device::getServiceObject )
        {
        err = m_deviceBinding->LoadServiceProvider( exec, args );
        if ( err != KErrNone )
            {
            char * err_msg = "general error ";
            // process  TLiwLoadStatus errors
            switch ( err )
                {
                case KLiwUnknown: // -5
                        err_msg = "unknown error";
                        break;
                case KLiwMetaDataInvalidFormat: //-4
                    err_msg = "meta data invalid format";
                        break;
                case KLiwInvalidVersionSpecification: //-3
                    err_msg = "invalid version specification";
                        break;
                case KLiwVersionOutOfRange: //-2
                    err_msg = "version out of range";
                        break;
                case KLiwSecurityAccessCheckFailed: //-1
                    err_msg = "security access check failed";
                        break;
                }
            throwError(exec, GeneralError, err_msg);
            }
        else
            {
            int argcount = args.size();
            if ( argcount == 0 || args[0]->type() != StringType || args[0]->toString( exec ).size() == 0 )
                return ret;
            // Get service name
            HBufC8* svcName = KJS::GetAsciiBufferL( args[0]->toString( exec ) );

            ServiceObject *so = new ServiceObject( exec, svcName, m_deviceBinding );
            if ( so != NULL )
                {
                (static_cast<Device*>(thisObj))->m_privateData->m_serviceObjArray->Append( so );
                ret = so;
                }
            }
        }
    else if ( m_func == Device::listProviders )
        {
        ret = m_deviceBinding->ListProviders( exec, args );
        }

    return ret;
    }

//END OF FILE



