/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#ifndef MediaQueryExp_h
#define MediaQueryExp_h

#include "AtomicString.h"
#include "CSSValue.h"

namespace WebCore {
class ValueList;

class MediaQueryExp
{
public:
    MediaQueryExp(const AtomicString& mediaFeature, ValueList* values);
    ~MediaQueryExp();

    AtomicString mediaFeature() const { return m_mediaFeature; }

    CSSValue* value() const { return m_value; }

    bool operator==(const MediaQueryExp& other) const  {
        return (other.m_mediaFeature == m_mediaFeature)
            && ((!other.m_value && !m_value)
                || (other.m_value && m_value && other.m_value->cssText() == m_value->cssText()));
    }

private:
    AtomicString m_mediaFeature;
    CSSValue* m_value;
};

} // namespace

#endif
