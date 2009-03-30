/*
 * Copyright (C) 2008 Nokia, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "UnknownContentHandler.h"
#include "HttpConnection.h"
#include "PlatformString.h"
#include <Uri8.h>
#include <apgcli.h>
using namespace WebCore;


UnknownContentHandler::UnknownContentHandler(HttpConnection* connection)
{
    m_connection = connection;
    m_response = NULL;
}

UnknownContentHandler::~UnknownContentHandler()
{
    delete m_response;
}

UnknownContentHandler* UnknownContentHandler::NewL(HttpConnection* connection, WebCore::ResourceResponse& response)
{
    UnknownContentHandler* self = NULL;
    self = new(ELeave) UnknownContentHandler(connection);
    CleanupStack::PushL(self);
    self->ConstructL(response);
    CleanupStack::Pop(self);
    return self;
}

bool UnknownContentHandler::isUnknownContent(ResourceResponse& response)
{
    const String& mimeType = response.mimeType();
    const KURL& url = response.url();

    if( equalIgnoringCase(mimeType, "application/octet-stream")) {
        TUriParser8 parser;
        if( parser.Parse(url.des()) == KErrNone ) {
            TPtrC8 path = parser.Extract( EUriPath );
            int length = path.Length();
            if( length > 4 ) {
                if (path.Find(_L8(".html")) != KErrNotFound || path.Find(_L8(".htm")) != KErrNotFound) {
                    response.setMimeType("text/html");
                    return false; // good chance of being html content
                }
                if (path.Find(_L8(".xhtml")) != KErrNotFound ) {
                    response.setMimeType("application/xhtml+xml");
                    return false; // good chance of being xhtml content
                }
                if (path.Find(_L8(".wml")) != KErrNotFound) {
                    response.setMimeType("text/vnd.wap.wml");
                    return false; // good chance of being wml content
                }
            }
        }
        return true; // could be html
    }
    return false;
}


void UnknownContentHandler::updateContentTypeL(TPtrC8& chunkPtr)
{
    RApaLsSession apaSession;
    TInt ret;
    User::LeaveIfError( apaSession.Connect() );
    TDataRecognitionResult dataType;
    ret = apaSession.RecognizeData( KNullDesC, chunkPtr, dataType );
    apaSession.Close();
    
    if( ret == KErrNone &&
      ( dataType.iConfidence == CApaDataRecognizerType::ECertain ) ||
      ( dataType.iConfidence == CApaDataRecognizerType::EProbable ) ) {       
        // "text/plain" shouldn't be treated as a strong type
        if (dataType.iDataType.Des8().FindF(_L8("text/plain")) == KErrNotFound) {
            m_response->setMimeType(dataType.iDataType.Des8());
        }
    }
}

ResourceResponse* UnknownContentHandler::handOffResponse()
{
    ResourceResponse* response = m_response;
    m_response = NULL;
    return response;
}

void UnknownContentHandler::ConstructL(ResourceResponse& response)
{
    m_response = new(ELeave) ResourceResponse(response.url(), response.mimeType(), response.expectedContentLength(), response.textEncodingName(), response.suggestedFilename());

}


// end of file
