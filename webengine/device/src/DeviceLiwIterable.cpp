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
* Description:  Implementation of LIW Iterable wrapper
*
*/


// INCLUDE FILES
#include <config.h>
#include <lookup.h>
#include <LiwServiceHandler.h>
#include "DeviceLiwIterable.h"
#include "DeviceLiwBinding.h"

const TInt KMaxKeySize = 128;

using namespace KJS;

const ClassInfo DeviceLiwIterable::info = { "DeviceLiwIterable", 0, 0, 0 };
const TInt INIT_JSOBJ_ARRAY_SIZE = 10;   // initial jsobject array
// ============================= LOCAL FUNCTIONS ===============================
/*
@begin DeviceLiwIterableTable 1
    getNext DeviceLiwIterable::getNext DontDelete|Function 0
    reset   DeviceLiwIterable::reset   DontDelete|Function 0
    close   DeviceLiwIterable::close   DontDelete|Function 0
@end
*/

/*
 * The DeviceLiwIterable is a plain wrapper around the underlying LIW iterable variant
 * object. It acts as a pass through for the iterable methods to JS. This
 * is needed for efficiently processing returned iterables (instead of copying the
 * contents into a JS array). Of course much of the benefit depends on how well iterables
 * are implemented in the underlying LIW framework.
 */

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// DeviceLiwIterable::DeviceLiwIterable
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterable::DeviceLiwIterable(
    ExecState* exec,
    CDeviceLiwBinding* binding,
    CLiwIterable* variant)
    : JSObject(exec->lexicalInterpreter()->builtinObjectPrototype() )
    {
        m_privateData = new DeviceLiwIterablePrivate(variant, binding);
        if (!m_privateData || !m_privateData->m_jsobjArray )
            m_valid = false;
        else 
            {
            m_valid = true;
            KJS::Collector::protect(this);            
            }            
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterable::~DeviceLiwIterable
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterable::~DeviceLiwIterable()
    {
        // only can be called by garbage collection after the 
        // DeviceLiwIterable::Close() was called
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterable::Close()
//
// ----------------------------------------------------------------------------
//
void DeviceLiwIterable::Close(ExecState* exec, bool unmark)
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
    
    // need exec to close other jsobject
    m_privateData->m_exec = exec;
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
// DeviceLiwIterable::toString
// Returns string representation of the object
//
// ----------------------------------------------------------------------------
UString DeviceLiwIterable::toString(ExecState* /*exec*/) const
    {
    return "[object DeviceLiwIterable]";
    }

// ----------------------------------------------------------------------------
// Device::getOwnPropertySlot
//
// ----------------------------------------------------------------------------

bool DeviceLiwIterable::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
    {
    // When the DeviceLiwIterable is valid, the check order is
    //      DeviceLiwIterable table => own property => prototype property
    // When the DeviceLiwIterable is invalid (after close), the check order is
    //      close function in table => prototype property

    // 1. when it is valid
    if(m_valid) 
        {
        // 1.1 check DeviceLiwIterable table    
        const HashEntry* entry = Lookup::findEntry(&DeviceLiwIterableTable, propertyName);
        if (entry)
            {
            slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwIterable>);
            return true;
            }

        // 1.2 check own property
        if(JSObject::getOwnPropertySlot(exec, propertyName, slot))
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
            const HashEntry* entry = Lookup::findEntry(&DeviceLiwIterableTable, propertyName);
            if (entry)
                {
                slot.setStaticEntry(this, entry, staticValueGetter<DeviceLiwIterable>);
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
// JSVersion::getValueProperty
//
// ----------------------------------------------------------------------------
JSValue* DeviceLiwIterable::getValueProperty(ExecState *exec, int token) const
    {
    DeviceLiwIterableFunc *lf;
    switch (token)
            {
            // Unfortunately, the only operations that LIW Iterables support are next() and reset(). e.g. there
            // are no hasNext() or count() operations typical for iterators.
            case getNext:
            case reset:
            case close:
                {
                lf = new DeviceLiwIterableFunc(exec, token);
                break;
                }

            default:
                return throwError(exec, GeneralError);
            }
            return lf;
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwIterablePrivate constructor
//
// ---------------------------------------------------------------------------
DeviceLiwIterablePrivate::DeviceLiwIterablePrivate(const CLiwIterable* liwIterable, CDeviceLiwBinding* liwBinding)
    {
    TRAP_IGNORE(
            m_liwBinding = liwBinding;
            m_jsobjArray = new RPointerArray<JSObject>( INIT_JSOBJ_ARRAY_SIZE );
            m_exec = NULL;    
            m_iterable = (CLiwIterable*) liwIterable;
            if ( m_iterable )
                m_iterable->IncRef();
            )
    }
    
// ---------------------------------------------------------------------------
// DeviceLiwMapPrivate::Close
//
// ---------------------------------------------------------------------------
void DeviceLiwIterablePrivate::Close()
    {
    // close the jsobject
    if ( m_jsobjArray && m_exec )
        {
        // close all the DeviceLiwMap objects and DeviceLiwIterable objects
        for (int i = 0; i < m_jsobjArray->Count(); i++)
            {
            JSObject * jsobj = (*m_jsobjArray)[i];
            if (jsobj->inherits( &DeviceLiwIterable::info ))
                {
                (static_cast<DeviceLiwIterable*>(jsobj))->Close(m_exec, true);
                }
            else if (jsobj->inherits( &DeviceLiwMap::info ))
                {
                (static_cast<DeviceLiwMap*>(jsobj))->Close(m_exec, true);
                }
            }
        m_jsobjArray->Close();
        delete m_jsobjArray;
        m_jsobjArray = NULL;
        m_exec = NULL;
        }
    
    // release the map    
    if ( m_iterable ) 
        {
        m_iterable->DecRef();
        m_iterable = NULL;
        }        
    }
    
// ----------------------------------------------------------------------------
// DeviceLiwIterablePrivate::DeviceLiwIterableFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterableFunc::DeviceLiwIterableFunc( ExecState* exec, int token )
    : JSObject( exec->lexicalInterpreter()->builtinObjectPrototype() ),
    m_func( token )
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterableFunc::~DeviceLiwIterableFunc
//
// ----------------------------------------------------------------------------
//
DeviceLiwIterableFunc::~DeviceLiwIterableFunc()
    {
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterableFunc::implementsCall
//
// ----------------------------------------------------------------------------
//
bool DeviceLiwIterableFunc::implementsCall() const
    {
    return true;
    }

// ----------------------------------------------------------------------------
// DeviceLiwIterableFunc::callAsFunction
//
// ----------------------------------------------------------------------------
//
JSValue* DeviceLiwIterableFunc::callAsFunction(ExecState* exec, JSObject *thisObj, const List& aArgs )
    {
        JSValue* rval = jsUndefined();
        TBool found = EFalse;

        if (!thisObj->inherits(&DeviceLiwIterable::info)) {
                    return throwError(exec, GeneralError);
        }

        DeviceLiwIterable *it = static_cast<DeviceLiwIterable *>(thisObj);
        
        if ( !it->isValid() || !it->m_privateData->m_liwBinding || !it->m_privateData->m_iterable) {
            return rval;
        }

        if ( m_func == DeviceLiwIterable::getNext ) {
            TLiwVariant vv;
            TRAP_IGNORE( found = it->m_privateData->m_iterable->NextL( vv ) );
            if( found ) 
                {
                rval =  it->m_privateData->m_liwBinding->LiwVariant2JsVal( exec, vv );
                if(rval->isObject()) 
                    {
                    JSObject* obj =  static_cast<JSObject*> (rval);
                    if(obj->inherits( &KJS::DeviceLiwIterable::info ) || obj->inherits( &KJS::DeviceLiwMap::info ))
                        {
                        // insert into jsobject array
                        it->m_privateData->m_jsobjArray->Append(obj);
                        }
                    }                
                
                vv.Reset();
                }
        }
        else if ( m_func == DeviceLiwIterable::reset ){
            it->m_privateData->m_iterable->Reset();
        }
        else if ( m_func == DeviceLiwIterable::close ){
            it->Close(exec, false);
        }
        return rval;
    }
//END OF FILE



