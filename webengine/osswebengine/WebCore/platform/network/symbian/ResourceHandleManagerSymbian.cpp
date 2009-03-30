/*
 * Copyright (C) 2006 Enrico Ros <enrico.ros@m31engineering.it>
 * Copyright (C) 2006 Trolltech ASA
 *
 * All rights reserved.
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

#include "config.h"

#include "hashmap.h"
#include "CString.h"
#include "CookieJar.h"
#include "ResourceHandle.h"
#include "ResourceHandleClient.h"
#include "ResourceResponse.h"
#include "ResourceHandleManagerSymbian.h"
#include "ResourceHandleInternal.h"
#include "ResourceError.h"
#include "MimeTypeRegistry.h"
#include "WebCoreFrameBridge.h"
#include "WebCoreKeyValuePair.h"
#include "oom.h"
#include <e32hal.h>

const TUint	KPluginsRescueBuffer = 3*1024*1024; //3MB

#define notImplemented() {}

namespace WebCore {

static CResourceHandleManager* s_self = 0;


CResourceHandleManager::CResourceHandleManager()
{
}

CResourceHandleManager::~CResourceHandleManager()
{
}

CResourceHandleManager* CResourceHandleManager::self()
{
    if (!s_self)
        s_self = new CResourceHandleManager();

    return s_self;
}

int CResourceHandleManager::add(ResourceHandle* resource)
{
    ASSERT(resource);

    ResourceHandleInternal* d = resource->getInternal();
    ResourceRequest r = resource->request();
    if (r.httpMethod() != "GET" && r.httpMethod() != "POST" &&
        r.httpMethod() != "DELETE" && r.httpMethod() != "PUT") {
        notImplemented();
        return KErrNotSupported;
    }
	if (d->m_connection)
		return (d->m_connection->submit());
    return KErrNone;
}

void CResourceHandleManager::cancel(ResourceHandle* resource)
{
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;
    MUrlConnection *connection = resource->getInternal()->m_connection;
    if (!connection)
        return;
    if (!connection->m_cancelled) {
        connection->m_cancelled = true;
        connection->cancel();
    }
}

void CResourceHandleManager::receivedResponse(ResourceHandle* resource, ResourceResponse& response, MUrlConnection* connection)
{
    if (!connection)
        return;
    if (connection->m_cancelled)
        return;
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;
    client->didReceiveResponse(resource, response);
}

void CResourceHandleManager::receivedData(ResourceHandle* resource, const TDesC8& data, int contentLength, MUrlConnection* connection)
{
    if (!connection)
        return;
    if (connection->m_cancelled)
        return;
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;

    // check if we have enough memory to handle this request
    if (resource->request().mainLoad()) {        
        OOM_PRE_CHECK(contentLength<<4, contentLength, "CResourceHandleManager::receiveData()")
        client->didReceiveData(resource, (const char*)data.Ptr(), data.Length(), data.Length());
        OOM_POST_CHECK_FAILED(client->didFail(resource, ResourceError(String(), KErrNoMemory, String(), String()));)
    }
    else if (client->isLoadingPlugins()) {
                      
        TMemoryInfoV1Buf info;
        UserHal::MemoryInfo( info );        
        int needed = data.Length();        
        if( needed+KPluginsRescueBuffer < info().iFreeRamInBytes ) {
            client->didReceiveData(resource, (const char*)data.Ptr(), data.Length(), data.Length());            
        } else {
            client->didFail(resource, ResourceError(String(), KErrNoMemory, String(), String()));    
        }
        
    }
    else {
        int needed = std::max(contentLength, std::max(client->receivedDataBufferSize(), data.Length()));
        OOM_PRE_CHECK(needed<<2, needed<<1, "CResourceHandleManager::receiveData()")        
        client->didReceiveData(resource, (const char*)data.Ptr(), data.Length(), data.Length());
        OOM_POST_CHECK_FAILED(client->didFail(resource, ResourceError(String(), KErrNoMemory, String(), String()));)
    }
}

void CResourceHandleManager::receivedFinished(ResourceHandle* resource, TInt errorCode, MUrlConnection* connection)
{
    if (!connection)
        return;
    if (connection->m_cancelled)
        return;
    ResourceHandleClient* client = resource->client();
    if (!client)
        return;
    if (connection->m_cancelled) {
        return;
    }
    if (errorCode) {
        client->didFail(resource, ResourceError(String(), errorCode, String(), String()));
    } else {
        client->didFinishLoading(resource);
    }
}

} // namespace WebCore



