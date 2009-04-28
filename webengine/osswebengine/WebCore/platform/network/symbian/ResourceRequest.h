// -*- mode: c++; c-basic-offset: 4 -*-
/*
 * Copyright (C) 2003, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef ResourceRequest_h
#define ResourceRequest_h

#include "ResourceRequestBase.h"
#include "FormData.h"
#include "KURL.h"
#include "HTTPHeaderMap.h"

namespace WebCore {

    struct ResourceRequest : ResourceRequestBase {

        ResourceRequest(const String& url, bool mainLoad = false) 
            : ResourceRequestBase(KURL(url.deprecatedString()), UseProtocolCachePolicy)
            , m_mainLoad(mainLoad)
        {
        }

        ResourceRequest(const KURL& url, bool mainLoad = false) 
            : ResourceRequestBase(url, UseProtocolCachePolicy)
            , m_mainLoad(mainLoad)
        {
        }

        ResourceRequest(const KURL& url, const String& referrer,  bool mainLoad = false, ResourceRequestCachePolicy policy = UseProtocolCachePolicy) 
            : ResourceRequestBase(url, policy)
            , m_mainLoad(mainLoad)
        {
            setHTTPReferrer(referrer);
        }
        
        ResourceRequest()
            : ResourceRequestBase(KURL(), UseProtocolCachePolicy)
            , m_mainLoad(false)
        {
        }

        void setMainLoad(bool mainLoad) { m_mainLoad = mainLoad; }
        bool mainLoad() const { return m_mainLoad; }

        void doUpdatePlatformRequest() const {}
        void doUpdateResourceRequest() const {}
        bool m_mainLoad;
    };

} // namespace WebCore

#endif // ResourceRequest_h