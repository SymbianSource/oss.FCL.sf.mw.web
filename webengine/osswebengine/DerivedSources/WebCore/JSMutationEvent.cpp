/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#include "JSMutationEvent.h"

#include <wtf/GetPtr.h>

#include "JSNode.h"
#include "MutationEvent.h"
#include "Node.h"
#include "PlatformString.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSMutationEventTableEntries[] =
{
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "prevValue", JSMutationEvent::PrevValueAttrNum, DontDelete|ReadOnly, 0, &JSMutationEventTableEntries[6] },
    { "constructor", JSMutationEvent::ConstructorAttrNum, DontDelete|DontEnum|ReadOnly, 0, 0 },
    { "attrName", JSMutationEvent::AttrNameAttrNum, DontDelete|ReadOnly, 0, &JSMutationEventTableEntries[7] },
    { "relatedNode", JSMutationEvent::RelatedNodeAttrNum, DontDelete|ReadOnly, 0, 0 },
    { "newValue", JSMutationEvent::NewValueAttrNum, DontDelete|ReadOnly, 0, 0 },
    { "attrChange", JSMutationEvent::AttrChangeAttrNum, DontDelete|ReadOnly, 0, 0 }
};

static const HashTable JSMutationEventTable = 
{
    2, 8, JSMutationEventTableEntries, 6
};

/* Hash table for constructor */

static const HashEntry JSMutationEventConstructorTableEntries[] =
{
    { "MODIFICATION", MutationEvent::MODIFICATION, DontDelete|ReadOnly, 0, &JSMutationEventConstructorTableEntries[3] },
    { "ADDITION", MutationEvent::ADDITION, DontDelete|ReadOnly, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "REMOVAL", MutationEvent::REMOVAL, DontDelete|ReadOnly, 0, 0 }
};

static const HashTable JSMutationEventConstructorTable = 
{
    2, 4, JSMutationEventConstructorTableEntries, 3
};

class JSMutationEventConstructor : public DOMObject {
public:
    JSMutationEventConstructor(ExecState* exec)
    {
        setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
        putDirect(exec->propertyNames().prototype, JSMutationEventPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    JSValue* getValueProperty(ExecState*, int token) const;
    virtual const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    virtual bool implementsHasInstance() const { return true; }
};

const ClassInfo JSMutationEventConstructor::info = { "MutationEventConstructor", 0, &JSMutationEventConstructorTable, 0 };

bool JSMutationEventConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSMutationEventConstructor, DOMObject>(exec, &JSMutationEventConstructorTable, this, propertyName, slot);
}

JSValue* JSMutationEventConstructor::getValueProperty(ExecState*, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(token);
}

/* Hash table for prototype */

static const HashEntry JSMutationEventPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "MODIFICATION", MutationEvent::MODIFICATION, DontDelete|ReadOnly, 0, &JSMutationEventPrototypeTableEntries[4] },
    { "ADDITION", MutationEvent::ADDITION, DontDelete|ReadOnly, 0, &JSMutationEventPrototypeTableEntries[5] },
    { "REMOVAL", MutationEvent::REMOVAL, DontDelete|ReadOnly, 0, 0 },
    { "initMutationEvent", JSMutationEvent::InitMutationEventFuncNum, DontDelete|Function, 8, 0 }
};

static const HashTable JSMutationEventPrototypeTable = 
{
    2, 6, JSMutationEventPrototypeTableEntries, 4
};

const ClassInfo JSMutationEventPrototype::info = { "MutationEventPrototype", 0, &JSMutationEventPrototypeTable, 0 };

JSObject* JSMutationEventPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSMutationEventPrototype>(exec, "[[JSMutationEvent.prototype]]");
}

bool JSMutationEventPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticPropertySlot<JSMutationEventPrototypeFunction, JSMutationEventPrototype, JSObject>(exec, &JSMutationEventPrototypeTable, this, propertyName, slot);
}

JSValue* JSMutationEventPrototype::getValueProperty(ExecState*, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(token);
}

const ClassInfo JSMutationEvent::info = { "MutationEvent", &JSEvent::info, &JSMutationEventTable, 0 };

JSMutationEvent::JSMutationEvent(ExecState* exec, MutationEvent* impl)
    : JSEvent(exec, impl)
{
    setPrototype(JSMutationEventPrototype::self(exec));
}

bool JSMutationEvent::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSMutationEvent, JSEvent>(exec, &JSMutationEventTable, this, propertyName, slot);
}

JSValue* JSMutationEvent::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case RelatedNodeAttrNum: {
        MutationEvent* imp = static_cast<MutationEvent*>(impl());

        return toJS(exec, WTF::getPtr(imp->relatedNode()));
    }
    case PrevValueAttrNum: {
        MutationEvent* imp = static_cast<MutationEvent*>(impl());

        return jsString(imp->prevValue());
    }
    case NewValueAttrNum: {
        MutationEvent* imp = static_cast<MutationEvent*>(impl());

        return jsString(imp->newValue());
    }
    case AttrNameAttrNum: {
        MutationEvent* imp = static_cast<MutationEvent*>(impl());

        return jsString(imp->attrName());
    }
    case AttrChangeAttrNum: {
        MutationEvent* imp = static_cast<MutationEvent*>(impl());

        return jsNumber(imp->attrChange());
    }
    case ConstructorAttrNum:
        return getConstructor(exec);
    }
    return 0;
}

JSValue* JSMutationEvent::getConstructor(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSMutationEventConstructor>(exec, "[[MutationEvent.constructor]]");
}
JSValue* JSMutationEventPrototypeFunction::callAsFunction(ExecState* exec, JSObject* thisObj, const List& args)
{
    if (!thisObj->inherits(&JSMutationEvent::info))
      return throwError(exec, TypeError);

    MutationEvent* imp = static_cast<MutationEvent*>(static_cast<JSMutationEvent*>(thisObj)->impl());

    switch (id) {
    case JSMutationEvent::InitMutationEventFuncNum: {
        AtomicString type = args[0]->toString(exec);
        bool canBubble = args[1]->toBoolean(exec);
        bool cancelable = args[2]->toBoolean(exec);
        Node* relatedNode = toNode(args[3]);
        String prevValue = args[4]->toString(exec);
        String newValue = args[5]->toString(exec);
        String attrName = args[6]->toString(exec);
        unsigned short attrChange = args[7]->toInt32(exec);

        imp->initMutationEvent(type, canBubble, cancelable, relatedNode, prevValue, newValue, attrName, attrChange);
        return jsUndefined();
    }
    }
    return 0;
}

}
