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
#include "PluginStreamLoaderClient.h"

#include "Page.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "DocumentLoader.h"
#include "ProgressTracker.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"

#include "StaticObjectsContainer.h"

#include "PluginSkin.h"
#include "PluginStream.h"
#include "PluginHandler.h"

#include "NetScapePlugInStreamLoader.h"

using namespace WebCore;

    
NetscapePlugInStreamLoaderClient* NetscapePlugInStreamLoaderClient::NewL(const String& url, PluginSkin* pluginskin, Frame* frame, void* notifydata, TBool notify/*=EFalse*/)
{
    NetscapePlugInStreamLoaderClient* self = new (ELeave) NetscapePlugInStreamLoaderClient();    
    CleanupStack::PushL( self );
    self->ConstructL( url, pluginskin, frame, notifydata, notify );
    CleanupStack::Pop();    
    return self;    
}


NetscapePlugInStreamLoaderClient* NetscapePlugInStreamLoaderClient::NewL(const ResourceRequest& request, PluginSkin* pluginskin, Frame* frame, void* notifydata, TBool notify/*=EFalse*/)
{
    NetscapePlugInStreamLoaderClient* self = new (ELeave) NetscapePlugInStreamLoaderClient();
    CleanupStack::PushL( self );
            
    self->ConstructL( request, pluginskin, frame, notifydata, notify );
    
    CleanupStack::Pop();    
    return self;    
}

void NetscapePlugInStreamLoaderClient::ConstructL(const String& url, PluginSkin* pluginskin, Frame* frame, void* notifydata, TBool notify/*=EFalse*/)  
{            
    m_loader = 0; 
    m_request = 0;
    m_pluginstream = 0;
    m_pluginskin = pluginskin;
    m_notifydata = notifydata;
    m_frame = frame;
    m_notify = notify;
    m_pluginstream = new (ELeave) PluginStream(pluginskin, this, notifydata);
    m_request = new (ELeave) ResourceRequest(m_frame->loader()->completeURL(url));
    
    if ( !m_frame->loader()->canLoad(m_request->url(), frame->document()) )
        return;
           
    if ( m_frame->loader()->shouldHideReferrer(m_request->url(), frame->loader()->outgoingReferrer()) )
        m_request->clearHTTPReferrer();
        
    
    m_loader = NetscapePlugInStreamLoader::create(frame,this);
    if (m_loader)
        m_loader->setShouldBufferData(false);
}

void NetscapePlugInStreamLoaderClient::ConstructL(const ResourceRequest& request, PluginSkin* pluginskin, Frame* frame, void* notifydata, TBool notify/*=EFalse*/)  
{            
    
    m_loader = 0; 
    m_request = 0;
    m_pluginstream = 0;
    m_pluginskin = pluginskin;
    m_notifydata = notifydata;
    m_frame = frame;
    m_notify = notify;
    m_pluginstream = new (ELeave) PluginStream(pluginskin, this, notifydata);
    m_request = new (ELeave) ResourceRequest(request.url());

    m_request->setCachePolicy(request.cachePolicy());        
    m_request->setTimeoutInterval(request.timeoutInterval());
    m_request->setMainDocumentURL(request.mainDocumentURL());
    m_request->setHTTPMethod(request.httpMethod());
    m_request->addHTTPHeaderFields(request.httpHeaderFields());
    m_request->setHTTPContentType(request.httpContentType());
    m_request->setHTTPReferrer(request.httpReferrer());
    m_request->setHTTPUserAgent(request.httpUserAgent());
    m_request->setHTTPAccept(request.httpAccept());
    m_request->setHTTPBody(request.httpBody());        
    m_request->setAllowHTTPCookies(request.allowHTTPCookies());

    
    if ( !m_frame->loader()->canLoad(m_request->url(), frame->document()) )
        return;
           
    if ( m_frame->loader()->shouldHideReferrer(m_request->url(), frame->loader()->outgoingReferrer()) )
        m_request->clearHTTPReferrer();
        
    
    m_loader = NetscapePlugInStreamLoader::create(m_frame,this);
    
}

NetscapePlugInStreamLoaderClient::NetscapePlugInStreamLoaderClient() : m_frame(0), m_loader(0), m_error(KErrNone)
{   
}

NetscapePlugInStreamLoaderClient::~NetscapePlugInStreamLoaderClient()
{  

    if (m_loader)
        m_loader->releaseResources();       
    
    delete m_request;     
    delete m_pluginstream;    
    m_pluginskin = NULL;
    m_notifydata = NULL;
    
}


void NetscapePlugInStreamLoaderClient::start()
{
    if (m_loader) {
        // Protect the loader from being deleted by errors before we finish with it (prevents a crash)
        RefPtr<ResourceLoader> protector(m_loader);
        m_loader->documentLoader()->addSubresourceLoader(m_loader);
        m_loader->load(*m_request);            
    }
}

void NetscapePlugInStreamLoaderClient::stop()
{
    
    if (m_loader && !m_loader->isDone())
        m_loader->cancel();        
    }

void NetscapePlugInStreamLoaderClient::cancelWithError(const ResourceError& error)
{
    if (m_loader && !m_loader->isDone()){
        m_loader->cancel(error);
    }
}

void NetscapePlugInStreamLoaderClient::didReceiveResponse(const ResourceResponse& response)
{
    int status = response.httpStatusCode();
    if (status < 200 || status >= 300) {          
        cancelWithError(ResourceError(String(response.url()), KErrGeneral, String(response.url()), String(response.httpStatusText())));        
        return;
    }
    
    // Currently we fill only response encoding type in the header field of NPStream structure, as url,MimeType and content-Length
    // are already a part of NPStream Structure
    
    HBufC8* headerPtr = HBufC8::NewLC(response.textEncodingName().length() + 1);
    headerPtr->Des().Copy(response.textEncodingName().des());
    headerPtr->Des().Append('\0');
    const char* headers = (const char*)headerPtr->Ptr();
    
    if (m_pluginstream) {
        TRAP(m_error, m_pluginstream->createNPStreamL(response.url().des(), response.mimeType().des(), response.expectedContentLength(), headers));
    }
    
    CleanupStack::PopAndDestroy(headerPtr);
}

void NetscapePlugInStreamLoaderClient::didReceiveData(const char* data, int length, long long lengthReceived)
{    

    if (m_pluginstream && m_error == KErrNone) {            
        TRAP(m_error, m_pluginstream->writeStreamL(data, length));
    }

    if (m_error && m_pluginstream) {
        m_pluginstream->destroyStream(m_error);
        
        if (m_loader && !m_loader->isDone()) {
            m_loader->cancel();
        }
    }
}

void NetscapePlugInStreamLoaderClient::didFinishLoading()
{
    if (m_pluginstream) {    
        m_pluginstream->destroyStream(m_error);
    }
}

void NetscapePlugInStreamLoaderClient::didFail(const ResourceError& error)
{
    if (m_pluginstream) {
        int err = error.errorCode() ? error.errorCode() : KErrCancel;
        HBufC* failedURL = HBufC::NewLC(error.failingURL().length());
        failedURL->Des().Copy(error.failingURL());
        m_pluginstream->destroyStream(err, failedURL);
        CleanupStack::PopAndDestroy(failedURL);
    }
}

void NetscapePlugInStreamLoaderClient::didCancel(const ResourceError& error)
{
    if (m_pluginstream) {
        int err = error.errorCode() ? error.errorCode() : KErrCancel;
        HBufC* failedURL = HBufC::NewLC(error.failingURL().length());
        failedURL->Des().Copy(error.failingURL());
        m_pluginstream->destroyStream(err, failedURL);
        CleanupStack::PopAndDestroy(failedURL);
    }
}






