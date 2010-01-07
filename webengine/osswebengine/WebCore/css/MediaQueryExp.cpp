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
* Description: 
*
*/


#include "config.h"
#include "MediaQueryExp.h"

#include "CSSParser.h"
#include "CSSPrimitiveValue.h"
#include "CSSValueList.h"

namespace WebCore {

MediaQueryExp::MediaQueryExp(const AtomicString& mediaFeature, ValueList* valueList)
    : m_mediaFeature(mediaFeature)
    , m_value(0)
{
    if (valueList) {
        if (valueList->size() == 1) {
            Value* value = valueList->current();

            if (value->id != 0)
                m_value = new CSSPrimitiveValue(value->id);
            else if (value->unit == CSSPrimitiveValue::CSS_STRING)
                m_value = new CSSPrimitiveValue(domString(value->string), (CSSPrimitiveValue::UnitTypes) value->unit);
            else if (value->unit >= CSSPrimitiveValue::CSS_NUMBER &&
                      value->unit <= CSSPrimitiveValue::CSS_KHZ)
                m_value = new CSSPrimitiveValue(value->fValue, (CSSPrimitiveValue::UnitTypes) value->unit);

            valueList->next();
        } else if (valueList->size() > 1) {
            // create list of values
            // currently accepts only <integer>/<integer>

            CSSValueList* list = new CSSValueList();
            Value* value = 0;
            bool isValid = true;

            while ((value = valueList->current()) && isValid) {
                if (value->unit == Value::Operator && value->iValue == '/')
                    list->append(new CSSPrimitiveValue("/", CSSPrimitiveValue::CSS_STRING));
                else if (value->unit == CSSPrimitiveValue::CSS_NUMBER)
                    list->append(new CSSPrimitiveValue(value->fValue, CSSPrimitiveValue::CSS_NUMBER));
                else
                    isValid = false;

                value = valueList->next();
            }

            if (isValid)
                m_value = list;
            else
                delete list;
        }
    }
}

MediaQueryExp::~MediaQueryExp()
{
    delete m_value;
}

} // namespace
