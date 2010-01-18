/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
#include "IconLoader.h"

#include "Document.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClient.h"
#include "IconDatabase.h"
#include "Logging.h"
#include "ResourceHandle.h"
#include "ResourceResponse.h"
#include "ResourceRequest.h"
#include "SubresourceLoader.h"

using namespace std;

namespace WebCore {

IconLoader::IconLoader(Frame* frame)
    : m_frame(frame)
    , m_loadIsInProgress(false)
{
}

auto_ptr<IconLoader> IconLoader::create(Frame* frame)
{
    return auto_ptr<IconLoader>(new IconLoader(frame));
}

IconLoader::~IconLoader()
{
}

void IconLoader::startLoading()
{    
    if (m_resourceLoader)
        return;

    // FIXME: http://bugs.webkit.org/show_bug.cgi?id=10902
    // Once ResourceHandle will load without a DocLoader, we can remove this check.
    // A frame may be documentless - one example is a frame containing only a PDF.
    if (!m_frame->document()) {
        LOG(IconDatabase, "Documentless-frame - icon won't be loaded");
        return;
    }

    // Set flag so we can detect the case where the load completes before
    // SubresourceLoader::create returns.
    m_loadIsInProgress = true;

    RefPtr<SubresourceLoader> loader = SubresourceLoader::create(m_frame, this, m_frame->loader()->iconURL());
    if (!loader)
        LOG_ERROR("Failed to start load for icon at url %s", m_frame->loader()->iconURL().url().ascii());

    // Store the handle so we can cancel the load if stopLoading is called later.
    // But only do it if the load hasn't already completed.
    if (m_loadIsInProgress)
        m_resourceLoader = loader.release();
}

void IconLoader::stopLoading()
{
    clearLoadingState();
}

void IconLoader::didReceiveResponse(SubresourceLoader* resourceLoader, const ResourceResponse& response)
{
    // If we got a status code indicating an invalid response, then lets
    // ignore the data and not try to decode the error page as an icon.
    int status = response.httpStatusCode();
    LOG(IconDatabase, "IconLoader::didReceiveResponse() - Loader %p, response %i", resourceLoader, status);

    // It is possible that we are receiving the response for previous load request which is cancelled.
    // Process down response of last resource loader only. Ignore if we can not remember other icon load requests. 
    if(resourceLoader == m_resourceLoader) {
        if (status && (status < 200 || status > 299)) {
            ResourceHandle* handle = resourceLoader->handle();
            finishLoading(handle ? handle->request().url() : KURL(), 0);
        }
    }
}

void IconLoader::didReceiveData(SubresourceLoader* loader, const char*, int size)
{
    LOG(IconDatabase, "IconLoader::didReceiveData() - Loader %p, number of bytes %i", loader, size);
}

void IconLoader::didFail(SubresourceLoader* resourceLoader, const ResourceError&)
{
    LOG(IconDatabase, "IconLoader::didFail() - Loader %p", resourceLoader);
    
    // Until <rdar://problem/5463392> is resolved and we can properly cancel SubresourceLoaders when they get an error response,
    // we need to be prepared to receive this call even after we've "finished loading" once.
    // After it is resolved, we can restore an assertion that the load is in progress if ::didFail() is called
    
    // It is possible that we are receiving the response for previous load request which is cancelled.
    // Process down response of last resource loader only. Ignore if we can not remember other icon load requests.    
    if (m_loadIsInProgress && (resourceLoader == m_resourceLoader)) {
        //ASSERT(resourceLoader == m_resourceLoader);
        ResourceHandle* handle = resourceLoader->handle();
        finishLoading(handle ? handle->request().url() : KURL(), 0);
    }
}

void IconLoader::didFinishLoading(SubresourceLoader* resourceLoader)
{
    LOG(IconDatabase, "IconLoader::didFinishLoading() - Loader %p", resourceLoader);

    // Until <rdar://problem/5463392> is resolved and we can properly cancel SubresourceLoaders when they get an error response,
    // we need to be prepared to receive this call even after we've "finished loading" once.
    // After it is resolved, we can restore an assertion that the load is in progress if ::didFail() is called
    
    // It is possible that we are receiving the response for previous load request which is cancelled.
    // Process down response of last resource loader only. Ignore if we can not remember other icon load requests.    
    if (m_loadIsInProgress && (resourceLoader == m_resourceLoader)) {
        //ASSERT(resourceLoader == m_resourceLoader);
        ResourceHandle* handle = resourceLoader->handle();
        finishLoading(handle ? handle->request().url() : KURL(), m_resourceLoader->resourceData());
    }
}

void IconLoader::finishLoading(const KURL& iconURL, PassRefPtr<SharedBuffer> data)
{

    // When an icon load results in a 404 we commit it to the database here and clear the loading state.  
    // But the SubresourceLoader continues pulling in data in the background for the 404 page if the server sends one.  
    // Once that data finishes loading or if the load is cancelled while that data is being read, finishLoading ends up being called a second time.
    // We need to change SubresourceLoader to have a mode where it will stop itself after receiving a 404 so this won't happen -
    // in the meantime, we'll only commit this data to the IconDatabase if it's the first time ::finishLoading() is called
    // <rdar://problem/5463392> tracks that enhancement
    
    if (!iconURL.isEmpty() && m_loadIsInProgress) {
        iconDatabase()->setIconDataForIconURL(data, iconURL.url());
        LOG(IconDatabase, "IconLoader::finishLoading() - Committing iconURL %s to database", iconURL.url().ascii());
        m_frame->loader()->commitIconURLToIconDatabase(iconURL);
        m_frame->loader()->client()->dispatchDidReceiveIcon();
    }

    clearLoadingState();
}

void IconLoader::clearLoadingState()
{
    m_resourceLoader = 0;
    m_loadIsInProgress = false;
}

}
