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


#ifndef MediaQuery_h
#define MediaQuery_h

#include "PlatformString.h"
#include <wtf/Vector.h>

namespace WebCore {
class MediaQueryExp;

class MediaQuery
{
public:
    enum Restrictor {
        Only, Not, None
    };

    MediaQuery(Restrictor r, const String& mediaType, Vector<MediaQueryExp*>* exprs);
    ~MediaQuery();

    Restrictor restrictor() const { return m_restrictor; }
    const Vector<MediaQueryExp*>* expressions() const { return m_expressions; }
    String mediaType() const { return m_mediaType; }
    bool operator==(const MediaQuery& other) const;
    void append(MediaQueryExp* newExp) { m_expressions->append(newExp); }
    String cssText() const;

 private:
    Restrictor m_restrictor;
    String m_mediaType;
    Vector<MediaQueryExp*>* m_expressions;
};

} // namespace

#endif
