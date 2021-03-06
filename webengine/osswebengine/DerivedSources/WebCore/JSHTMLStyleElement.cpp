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

#include "JSHTMLStyleElement.h"

#include <wtf/GetPtr.h>

#include "HTMLStyleElement.h"
#include "JSStyleSheet.h"
#include "PlatformString.h"
#include "StyleSheet.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSHTMLStyleElementTableEntries[] =
{
    { "sheet", JSHTMLStyleElement::SheetAttrNum, DontDelete|ReadOnly, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "constructor", JSHTMLStyleElement::ConstructorAttrNum, DontDelete|DontEnum|ReadOnly, 0, 0 },
    { "disabled", JSHTMLStyleElement::DisabledAttrNum, DontDelete, 0, &JSHTMLStyleElementTableEntries[5] },
    { "media", JSHTMLStyleElement::MediaAttrNum, DontDelete, 0, 0 },
    { "type", JSHTMLStyleElement::TypeAttrNum, DontDelete, 0, 0 }
};

static const HashTable JSHTMLStyleElementTable = 
{
    2, 6, JSHTMLStyleElementTableEntries, 5
};

/* Hash table for constructor */

static const HashEntry JSHTMLStyleElementConstructorTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSHTMLStyleElementConstructorTable = 
{
    2, 1, JSHTMLStyleElementConstructorTableEntries, 1
};

class JSHTMLStyleElementConstructor : public DOMObject {
public:
    JSHTMLStyleElementConstructor(ExecState* exec)
    {
        setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
        putDirect(exec->propertyNames().prototype, JSHTMLStyleElementPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    JSValue* getValueProperty(ExecState*, int token) const;
    virtual const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    virtual bool implementsHasInstance() const { return true; }
};

const ClassInfo JSHTMLStyleElementConstructor::info = { "HTMLStyleElementConstructor", 0, &JSHTMLStyleElementConstructorTable, 0 };

bool JSHTMLStyleElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLStyleElementConstructor, DOMObject>(exec, &JSHTMLStyleElementConstructorTable, this, propertyName, slot);
}

JSValue* JSHTMLStyleElementConstructor::getValueProperty(ExecState*, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(token);
}

/* Hash table for prototype */

static const HashEntry JSHTMLStyleElementPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSHTMLStyleElementPrototypeTable = 
{
    2, 1, JSHTMLStyleElementPrototypeTableEntries, 1
};

const ClassInfo JSHTMLStyleElementPrototype::info = { "HTMLStyleElementPrototype", 0, &JSHTMLStyleElementPrototypeTable, 0 };

JSObject* JSHTMLStyleElementPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSHTMLStyleElementPrototype>(exec, "[[JSHTMLStyleElement.prototype]]");
}

const ClassInfo JSHTMLStyleElement::info = { "HTMLStyleElement", &JSHTMLElement::info, &JSHTMLStyleElementTable, 0 };

JSHTMLStyleElement::JSHTMLStyleElement(ExecState* exec, HTMLStyleElement* impl)
    : JSHTMLElement(exec, impl)
{
    setPrototype(JSHTMLStyleElementPrototype::self(exec));
}

bool JSHTMLStyleElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLStyleElement, JSHTMLElement>(exec, &JSHTMLStyleElementTable, this, propertyName, slot);
}

JSValue* JSHTMLStyleElement::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case DisabledAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        return jsBoolean(imp->disabled());
    }
    case MediaAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        return jsString(imp->media());
    }
    case TypeAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        return jsString(imp->type());
    }
    case SheetAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        return toJS(exec, WTF::getPtr(imp->sheet()));
    }
    case ConstructorAttrNum:
        return getConstructor(exec);
    }
    return 0;
}

void JSHTMLStyleElement::put(ExecState* exec, const Identifier& propertyName, JSValue* value, int attr)
{
    lookupPut<JSHTMLStyleElement, JSHTMLElement>(exec, propertyName, value, attr, &JSHTMLStyleElementTable, this);
}

void JSHTMLStyleElement::putValueProperty(ExecState* exec, int token, JSValue* value, int /*attr*/)
{
    switch (token) {
    case DisabledAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        imp->setDisabled(value->toBoolean(exec));
        break;
    }
    case MediaAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        imp->setMedia(valueToStringWithNullCheck(exec, value));
        break;
    }
    case TypeAttrNum: {
        HTMLStyleElement* imp = static_cast<HTMLStyleElement*>(impl());

        imp->setType(valueToStringWithNullCheck(exec, value));
        break;
    }
    }
}

JSValue* JSHTMLStyleElement::getConstructor(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSHTMLStyleElementConstructor>(exec, "[[HTMLStyleElement.constructor]]");
}

}
