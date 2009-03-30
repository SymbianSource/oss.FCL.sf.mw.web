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


#if ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)

#include "Document.h"
#include "Frame.h"
#include "SVGDocumentExtensions.h"
#include "SVGElement.h"
#include "SVGAnimatedTemplate.h"
#include "JSSVGFEDistantLightElement.h"

#include <wtf/GetPtr.h>

#include "JSSVGAnimatedNumber.h"
#include "SVGFEDistantLightElement.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSSVGFEDistantLightElementTableEntries[] =
{
    { 0, 0, 0, 0, 0 },
    { "azimuth", JSSVGFEDistantLightElement::AzimuthAttrNum, DontDelete|ReadOnly, 0, &JSSVGFEDistantLightElementTableEntries[2] },
    { "elevation", JSSVGFEDistantLightElement::ElevationAttrNum, DontDelete|ReadOnly, 0, 0 }
};

static const HashTable JSSVGFEDistantLightElementTable = 
{
    2, 3, JSSVGFEDistantLightElementTableEntries, 2
};

/* Hash table for prototype */

static const HashEntry JSSVGFEDistantLightElementPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSSVGFEDistantLightElementPrototypeTable = 
{
    2, 1, JSSVGFEDistantLightElementPrototypeTableEntries, 1
};

const ClassInfo JSSVGFEDistantLightElementPrototype::info = { "SVGFEDistantLightElementPrototype", 0, &JSSVGFEDistantLightElementPrototypeTable, 0 };

JSObject* JSSVGFEDistantLightElementPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSSVGFEDistantLightElementPrototype>(exec, "[[JSSVGFEDistantLightElement.prototype]]");
}

const ClassInfo JSSVGFEDistantLightElement::info = { "SVGFEDistantLightElement", &JSSVGElement::info, &JSSVGFEDistantLightElementTable, 0 };

JSSVGFEDistantLightElement::JSSVGFEDistantLightElement(ExecState* exec, SVGFEDistantLightElement* impl)
    : JSSVGElement(exec, impl)
{
    setPrototype(JSSVGFEDistantLightElementPrototype::self(exec));
}

bool JSSVGFEDistantLightElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGFEDistantLightElement, JSSVGElement>(exec, &JSSVGFEDistantLightElementTable, this, propertyName, slot);
}

JSValue* JSSVGFEDistantLightElement::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case AzimuthAttrNum: {
        SVGFEDistantLightElement* imp = static_cast<SVGFEDistantLightElement*>(impl());

        ASSERT(exec && exec->dynamicInterpreter());

        RefPtr<SVGAnimatedNumber> obj = imp->azimuthAnimated();
        Frame* activeFrame = static_cast<ScriptInterpreter*>(exec->dynamicInterpreter())->frame();
        if (activeFrame) {
            SVGDocumentExtensions* extensions = (activeFrame->document() ? activeFrame->document()->accessSVGExtensions() : 0);
            if (extensions) {
                if (extensions->hasGenericContext<SVGAnimatedNumber>(obj.get()))
                    ASSERT(extensions->genericContext<SVGAnimatedNumber>(obj.get()) == imp);
                else
                    extensions->setGenericContext<SVGAnimatedNumber>(obj.get(), imp);
            }
        }

        return toJS(exec, obj.get());
    }
    case ElevationAttrNum: {
        SVGFEDistantLightElement* imp = static_cast<SVGFEDistantLightElement*>(impl());

        ASSERT(exec && exec->dynamicInterpreter());

        RefPtr<SVGAnimatedNumber> obj = imp->elevationAnimated();
        Frame* activeFrame = static_cast<ScriptInterpreter*>(exec->dynamicInterpreter())->frame();
        if (activeFrame) {
            SVGDocumentExtensions* extensions = (activeFrame->document() ? activeFrame->document()->accessSVGExtensions() : 0);
            if (extensions) {
                if (extensions->hasGenericContext<SVGAnimatedNumber>(obj.get()))
                    ASSERT(extensions->genericContext<SVGAnimatedNumber>(obj.get()) == imp);
                else
                    extensions->setGenericContext<SVGAnimatedNumber>(obj.get(), imp);
            }
        }

        return toJS(exec, obj.get());
    }
    }
    return 0;
}


}

#endif // ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)
